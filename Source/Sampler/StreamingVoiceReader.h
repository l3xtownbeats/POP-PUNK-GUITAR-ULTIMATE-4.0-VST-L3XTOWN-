#pragma once

#include "PageCacheSlots.h"
#include "SampleStreamingService.h"

#include <array>
#include <cstdint>
#include <limits>

namespace ppgu::sampler
{
// Fixed-storage, per-voice page reader. It owns at most three cache leases,
// enough for four-point Hermite interpolation at a page boundary. All methods
// are audio-thread safe: no allocation, locks, waits, or file operations.
class StreamingVoiceReader final
{
public:
    StreamingVoiceReader() = default;
    ~StreamingVoiceReader() { release(); }

    StreamingVoiceReader(const StreamingVoiceReader&) = delete;
    StreamingVoiceReader& operator=(const StreamingVoiceReader&) = delete;
    StreamingVoiceReader(StreamingVoiceReader&&) = delete;
    StreamingVoiceReader& operator=(StreamingVoiceReader&&) = delete;

    void attach(SampleStreamingService* service, std::uint32_t sampleId,
                std::uint64_t totalFrames) noexcept;
    void release() noexcept;
    // Transfer reader state and leases without changing cache pin counts.
    void transferTo(StreamingVoiceReader& destination) noexcept;

    // Requests the current page and its previous/next neighbours without
    // waiting. Neighbours keep Hermite taps safe at forward and reverse page
    // boundaries. Call at voice allocation and whenever the cursor changes page.
    void prefetch(std::uint64_t frame) noexcept;

    // Queue loop-region pages ahead of time without pinning them in this
    // voice's three-lease working set. This keeps attack pages resident while
    // the worker warms loop-start/continuation pages for seamless wraparound.
    void requestPrefetch(std::uint64_t frame) noexcept;

    // Returns false on a cache miss; the required page request is queued and
    // the caller should output silence for this sample rather than block.
    [[nodiscard]] bool readHermite(std::uint64_t frame, float fraction,
                                   std::uint64_t regionStart,
                                   std::uint64_t regionEnd,
                                   float& left, float& right) noexcept;

private:
    [[nodiscard]] bool ensurePage(std::uint64_t pageIndex) noexcept;
    [[nodiscard]] bool readFrame(std::uint64_t frame, float& left,
                                 float& right) const noexcept;
    [[nodiscard]] int findLease(std::uint64_t pageIndex) const noexcept;

    static constexpr std::size_t leaseCount = 3;
    static constexpr auto noPage = std::numeric_limits<std::uint64_t>::max();

    SampleStreamingService* stream = nullptr;
    std::uint32_t sourceSampleId = 0;
    std::uint64_t sourceFrames = 0;
    std::array<PageCacheSlots::ReadLease, leaseCount> leases {};
    std::array<std::uint64_t, leaseCount> leasePages { noPage, noPage, noPage };
    std::array<std::uint64_t, leaseCount> pendingPages { noPage, noPage, noPage };
    std::array<std::uint8_t, leaseCount> retryCountdown {};
};
}
