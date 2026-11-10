#include "StreamingVoiceReader.h"

#include <algorithm>
#include <cmath>

namespace ppgu::sampler
{
void StreamingVoiceReader::attach(SampleStreamingService* service,
                                  std::uint32_t sampleId,
                                  std::uint64_t totalFrames) noexcept
{
    release();
    if (service == nullptr || !service->isValid() || service->pageFrames() == 0 || totalFrames == 0)
        return;
    stream = service;
    sourceSampleId = sampleId;
    sourceFrames = totalFrames;
}

void StreamingVoiceReader::release() noexcept
{
    if (stream != nullptr)
        for (auto& lease : leases)
            if (lease)
                stream->release(lease);

    leasePages.fill(noPage);
    pendingPages.fill(noPage);
    retryCountdown.fill(0);
    stream = nullptr;
    sourceSampleId = 0;
    sourceFrames = 0;
}

int StreamingVoiceReader::findLease(std::uint64_t pageIndex) const noexcept
{
    for (std::size_t i = 0; i < leaseCount; ++i)
        if (leases[i] && leasePages[i] == pageIndex)
            return static_cast<int>(i);
    return -1;
}

bool StreamingVoiceReader::ensurePage(std::uint64_t pageIndex) noexcept
{
    if (stream == nullptr || stream->pageFrames() == 0
        || pageIndex >= 1 + (sourceFrames - 1) / stream->pageFrames())
        return false;
    if (findLease(pageIndex) >= 0)
        return true;

    const auto slot = static_cast<std::size_t>(pageIndex % leaseCount);
    if (leases[slot])
    {
        stream->release(leases[slot]);
        leasePages[slot] = noPage;
    }

    if (pendingPages[slot] == pageIndex)
    {
        if (stream->tryAcquire({ sourceSampleId, pageIndex }, leases[slot]))
        {
            pendingPages[slot] = noPage;
            retryCountdown[slot] = 0;
            leasePages[slot] = pageIndex;
            return true;
        }

        // A worker may have dropped a request because the cache was fully
        // pinned or decoding failed. Retry at a bounded cadence, not once per
        // output sample, and never let a stale pending marker deadlock a voice.
        if (retryCountdown[slot] > 0)
            --retryCountdown[slot];
        if (retryCountdown[slot] == 0)
        {
            (void) stream->requestPage({ sourceSampleId, pageIndex });
            retryCountdown[slot] = 64;
        }
        return false;
    }

    pendingPages[slot] = noPage;
    retryCountdown[slot] = 0;
    if (stream->tryAcquire({ sourceSampleId, pageIndex }, leases[slot]))
    {
        leasePages[slot] = pageIndex;
        return true;
    }

    (void) stream->requestPage({ sourceSampleId, pageIndex });
    pendingPages[slot] = pageIndex;
    retryCountdown[slot] = 64;
    return false;
}

void StreamingVoiceReader::prefetch(std::uint64_t frame) noexcept
{
    if (stream == nullptr || sourceFrames == 0 || frame >= sourceFrames)
        return;
    const auto page = frame / stream->pageFrames();
    // Keep the current page and both neighbours resident. The previous page
    // is needed by Hermite taps during reverse/ping-pong playback; the next
    // page prevents a forward boundary miss.
    if (page > 0)
        (void) ensurePage(page - 1);
    (void) ensurePage(page);
    const auto nextPage = page + 1;
    if (nextPage < 1 + (sourceFrames - 1) / stream->pageFrames())
        (void) ensurePage(nextPage);
}

bool StreamingVoiceReader::readFrame(std::uint64_t frame, float& left,
                                     float& right) const noexcept
{
    if (stream == nullptr || frame >= sourceFrames)
        return false;
    const auto pageIndex = frame / stream->pageFrames();
    const int leaseIndex = findLease(pageIndex);
    if (leaseIndex < 0)
        return false;

    const auto& lease = leases[static_cast<std::size_t>(leaseIndex)];
    const auto localFrame = static_cast<std::uint32_t>(frame % stream->pageFrames());
    if (localFrame >= lease.frames || lease.channels == 0 || lease.channels > 2)
        return false;

    left = lease.samples[localFrame];
    right = lease.channels > 1
        ? lease.samples[stream->pageFrames() + localFrame] : left;
    return true;
}

bool StreamingVoiceReader::readHermite(std::uint64_t frame, float fraction,
                                       std::uint64_t regionStart,
                                       std::uint64_t regionEnd,
                                       float& left, float& right) noexcept
{
    if (stream == nullptr || regionStart >= regionEnd || regionEnd > sourceFrames
        || regionEnd > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) - 2
        || frame < regionStart || frame >= regionEnd || !std::isfinite(fraction))
        return false;

    const auto clampFrame = [regionStart, regionEnd](std::int64_t value) noexcept
    {
        const auto low = static_cast<std::int64_t>(regionStart);
        const auto high = static_cast<std::int64_t>(regionEnd - 1);
        return static_cast<std::uint64_t>(std::clamp(value, low, high));
    };
    const auto base = static_cast<std::int64_t>(frame);
    const std::array<std::uint64_t, 4> indices {
        clampFrame(base - 1), clampFrame(base), clampFrame(base + 1), clampFrame(base + 2)
    };

    for (const auto index : indices)
        if (!ensurePage(index / stream->pageFrames()))
            return false;

    std::array<float, 4> l {}, r {};
    for (std::size_t i = 0; i < indices.size(); ++i)
        if (!readFrame(indices[i], l[i], r[i]))
            return false;

    const float t = std::clamp(fraction, 0.0f, 1.0f);
    const float t2 = t * t;
    const float t3 = t2 * t;
    const auto interpolate = [t, t2, t3](const std::array<float, 4>& y) noexcept
    {
        return 0.5f * ((2.0f * y[1])
            + (-y[0] + y[2]) * t
            + (2.0f * y[0] - 5.0f * y[1] + 4.0f * y[2] - y[3]) * t2
            + (-y[0] + 3.0f * y[1] - 3.0f * y[2] + y[3]) * t3);
    };

    left = interpolate(l);
    right = interpolate(r);
    return true;
}
}
