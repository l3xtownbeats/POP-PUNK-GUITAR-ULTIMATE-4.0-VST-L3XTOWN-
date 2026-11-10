#include "SampleStreamingService.h"

#include "WavPageReader.h"

#include <chrono>
#include <limits>
#include <string>
#include <utility>

namespace ppgu::sampler
{
SampleStreamingService::SampleStreamingService(std::vector<SampleMetadata> sourceSamples,
                                               std::size_t cacheSlots,
                                               std::uint32_t pageFrames)
    : samples(std::move(sourceSamples)),
      pages(cacheSlots, pageFrames, 2)
{
}

SampleStreamingService::~SampleStreamingService()
{
    stop();
}

bool SampleStreamingService::isValid() const noexcept
{
    return pages.isValid() && pages.pageFrames() > 0 && !samples.empty();
}

bool SampleStreamingService::start()
{
    if (!isValid())
        return false;
    if (running.load(std::memory_order_acquire))
        return true;
    if (worker.joinable())
        return false;

    running.store(true, std::memory_order_release);
    try
    {
        worker = std::thread([this] { workerLoop(); });
    }
    catch (...)
    {
        running.store(false, std::memory_order_release);
        return false;
    }
    return true;
}

void SampleStreamingService::stop() noexcept
{
    running.store(false, std::memory_order_release);
    if (worker.joinable())
        worker.join();
}

bool SampleStreamingService::requestPage(PageKey key) noexcept
{
    if (!running.load(std::memory_order_acquire))
        return false;
    if (key.sampleId >= samples.size())
    {
        invalidRequests.fetch_add(1, std::memory_order_relaxed);
        return false;
    }

    const auto frames = samples[key.sampleId].frames;
    const auto pageCount = frames == 0 ? 0 : 1 + (frames - 1) / pages.pageFrames();
    if (key.pageIndex >= pageCount)
    {
        invalidRequests.fetch_add(1, std::memory_order_relaxed);
        return false;
    }

    if (!requests.tryPush(Request { key }))
    {
        queueOverflows.fetch_add(1, std::memory_order_relaxed);
        return false;
    }
    requestsSubmitted.fetch_add(1, std::memory_order_relaxed);
    return true;
}

bool SampleStreamingService::tryAcquire(PageKey key, PageCacheSlots::ReadLease& lease) noexcept
{
    if (pages.tryAcquire(key, lease))
    {
        cacheHits.fetch_add(1, std::memory_order_relaxed);
        return true;
    }
    cacheMisses.fetch_add(1, std::memory_order_relaxed);
    return false;
}

void SampleStreamingService::release(PageCacheSlots::ReadLease& lease) noexcept
{
    pages.release(lease);
}

std::size_t SampleStreamingService::cacheBytes() const noexcept
{
    constexpr auto bytesPerSample = sizeof(float);
    const auto slots = pages.capacity();
    const auto frames = static_cast<std::size_t>(pages.pageFrames());
    const auto channels = static_cast<std::size_t>(pages.maxChannels());
    if (frames != 0 && slots > std::numeric_limits<std::size_t>::max() / frames)
        return 0;
    const auto slotFrames = slots * frames;
    if (channels != 0 && slotFrames > std::numeric_limits<std::size_t>::max() / channels)
        return 0;
    const auto values = slotFrames * channels;
    if (values > std::numeric_limits<std::size_t>::max() / bytesPerSample)
        return 0;
    return values * bytesPerSample;
}

SampleStreamingService::Statistics SampleStreamingService::statistics() const noexcept
{
    return {
        requestsSubmitted.load(std::memory_order_relaxed),
        requestsProcessed.load(std::memory_order_relaxed),
        queueOverflows.load(std::memory_order_relaxed),
        invalidRequests.load(std::memory_order_relaxed),
        cacheHits.load(std::memory_order_relaxed),
        cacheMisses.load(std::memory_order_relaxed),
        pagesDecoded.load(std::memory_order_relaxed),
        decodeFailures.load(std::memory_order_relaxed),
        cacheFull.load(std::memory_order_relaxed)
    };
}

void SampleStreamingService::workerLoop() noexcept
{
    try
    {
    WavPageReader reader;
    std::uint32_t openSampleId = std::numeric_limits<std::uint32_t>::max();

    while (running.load(std::memory_order_acquire) || !requests.empty())
    {
        Request request;
        if (!requests.tryPop(request))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        requestsProcessed.fetch_add(1, std::memory_order_relaxed);
        const auto key = request.key;
        if (key.sampleId >= samples.size() || pages.containsReady(key))
            continue;

        if (openSampleId != key.sampleId)
        {
            reader.close();
            std::string error;
            if (!reader.open(samples[key.sampleId], error))
            {
                openSampleId = std::numeric_limits<std::uint32_t>::max();
                decodeFailures.fetch_add(1, std::memory_order_relaxed);
                continue;
            }
            openSampleId = key.sampleId;
        }

        std::size_t slot = 0;
        if (!pages.tryBeginLoad(key, slot))
        {
            cacheFull.fetch_add(1, std::memory_order_relaxed);
            continue;
        }

        auto* storage = pages.writableData(slot);
        if (storage == nullptr)
        {
            pages.cancelLoad(slot);
            decodeFailures.fetch_add(1, std::memory_order_relaxed);
            continue;
        }

        const auto channels = static_cast<int>(reader.getChannelCount());
        if (channels < 1 || channels > 2)
        {
            pages.cancelLoad(slot);
            decodeFailures.fetch_add(1, std::memory_order_relaxed);
            continue;
        }

        std::array<float*, 2> destination {
            storage,
            storage + pages.pageFrames()
        };
        std::uint32_t framesWritten = 0;
        const auto report = reader.readPageInto(key.pageIndex, destination.data(),
            channels, 0, framesWritten, pages.pageFrames());
        if (!report.success || framesWritten == 0
            || !pages.publish(slot, key, static_cast<std::uint32_t>(channels), framesWritten))
        {
            pages.cancelLoad(slot);
            decodeFailures.fetch_add(1, std::memory_order_relaxed);
            continue;
        }

        pagesDecoded.fetch_add(1, std::memory_order_relaxed);
    }

    reader.close();
    }
    catch (...)
    {
        // An allocation/decoder exception must never escape a std::thread
        // entry point. Mark the worker stopped and expose the failure.
        decodeFailures.fetch_add(1, std::memory_order_relaxed);
        running.store(false, std::memory_order_release);
    }
}
}
