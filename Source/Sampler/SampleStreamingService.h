#pragma once

#include "BoundedSpscQueue.h"
#include "PageCacheSlots.h"
#include "SampleMetadataIndex.h"

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <semaphore>
#include <thread>
#include <vector>

namespace ppgu::sampler
{
// Background WAV page streamer. The audio thread only submits fixed-size
// requests and acquires/releases cache leases. File access, decoding and
// worker sleeps are isolated to the worker thread.
class SampleStreamingService final
{
public:
    static constexpr std::uint32_t defaultPageFrames = 4096;
    static constexpr std::size_t requestQueueCapacity = 256;

    struct Statistics
    {
        std::uint64_t requestsSubmitted = 0;
        std::uint64_t requestsProcessed = 0;
        std::uint64_t queueOverflows = 0;
        std::uint64_t invalidRequests = 0;
        std::uint64_t cacheHits = 0;
        std::uint64_t cacheMisses = 0;
        std::uint64_t pagesDecoded = 0;
        std::uint64_t decodeFailures = 0;
        std::uint64_t cacheFull = 0;
    };

    SampleStreamingService(std::vector<SampleMetadata> samples,
                           std::size_t cacheSlots = 64,
                           std::uint32_t pageFrames = defaultPageFrames);
    ~SampleStreamingService();

    SampleStreamingService(const SampleStreamingService&) = delete;
    SampleStreamingService& operator=(const SampleStreamingService&) = delete;

    [[nodiscard]] bool isValid() const noexcept;
    [[nodiscard]] bool start();
    void stop() noexcept;

    // Single audio-thread producer. This function never allocates, locks,
    // waits, opens files, or decodes audio. The producer must be quiescent
    // before stop() is called; stop drains requests already in the queue.
    [[nodiscard]] bool requestPage(PageKey key) noexcept;
    [[nodiscard]] bool tryAcquire(PageKey key, PageCacheSlots::ReadLease& lease) noexcept;
    void release(PageCacheSlots::ReadLease& lease) noexcept;

    [[nodiscard]] std::size_t sampleCount() const noexcept { return samples.size(); }
    [[nodiscard]] std::uint32_t pageFrames() const noexcept { return pages.pageFrames(); }
    [[nodiscard]] std::size_t cacheBytes() const noexcept;
    [[nodiscard]] Statistics statistics() const noexcept;

private:
    struct Request
    {
        PageKey key;
    };

    void workerLoop() noexcept;

    const std::vector<SampleMetadata> samples;
    PageCacheSlots pages;
    BoundedSpscQueue<Request, requestQueueCapacity> requests;
    std::atomic<bool> running { false };
    // The audio thread signals successful enqueues without waiting. A permit
    // per request lets the worker sleep completely while the bank is idle.
    // One extra permit is reserved for shutdown wake-up.
    std::counting_semaphore<static_cast<std::ptrdiff_t>(requestQueueCapacity + 1)> workAvailable { 0 };
    std::thread worker;

    std::atomic<std::uint64_t> requestsSubmitted { 0 };
    std::atomic<std::uint64_t> requestsProcessed { 0 };
    std::atomic<std::uint64_t> queueOverflows { 0 };
    std::atomic<std::uint64_t> invalidRequests { 0 };
    std::atomic<std::uint64_t> cacheHits { 0 };
    std::atomic<std::uint64_t> cacheMisses { 0 };
    std::atomic<std::uint64_t> pagesDecoded { 0 };
    std::atomic<std::uint64_t> decodeFailures { 0 };
    std::atomic<std::uint64_t> cacheFull { 0 };
};
}
