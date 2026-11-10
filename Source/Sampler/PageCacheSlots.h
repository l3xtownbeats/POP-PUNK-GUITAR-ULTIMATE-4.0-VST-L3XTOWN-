#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace ppgu::sampler
{
struct PageKey
{
    std::uint32_t sampleId = 0;
    std::uint64_t pageIndex = 0;
};

// Fixed-slot cache storage. Construct/resize on a control or worker thread,
// never from processBlock(). Audio-side acquire/release and sample access do
// not allocate, lock, wait, or perform file I/O.
class PageCacheSlots final
{
public:
    enum class State : std::uint8_t { empty, loading, ready, reading };

    struct ReadLease
    {
        std::size_t slot = 0;
        const float* samples = nullptr; // planar: channel * pageFrames + frame
        std::uint32_t channels = 0;
        std::uint32_t frames = 0;
        explicit operator bool() const noexcept { return samples != nullptr; }
    };

    PageCacheSlots(std::size_t slotCount, std::uint32_t pageFrames,
                   std::uint32_t maxChannels);
    ~PageCacheSlots() = default;

    PageCacheSlots(const PageCacheSlots&) = delete;
    PageCacheSlots& operator=(const PageCacheSlots&) = delete;

    [[nodiscard]] bool isValid() const noexcept { return storage != nullptr; }
    [[nodiscard]] std::size_t capacity() const noexcept { return slotCount; }
    [[nodiscard]] std::uint32_t pageFrames() const noexcept { return framesPerPage; }
    [[nodiscard]] std::uint32_t maxChannels() const noexcept { return channelLimit; }

    // Worker thread: reserve an empty or unpinned ready slot for decoding.
    // Returns false if every slot is currently pinned/loading.
    [[nodiscard]] bool tryBeginLoad(PageKey key, std::size_t& slot) noexcept;
    // Worker thread only. Writes decoded planar data to the reserved slot.
    [[nodiscard]] float* writableData(std::size_t slot) noexcept;
    // Worker thread: publish a completely decoded page atomically.
    [[nodiscard]] bool publish(std::size_t slot, PageKey key,
                               std::uint32_t channels, std::uint32_t frames) noexcept;
    // Worker thread: cancel a reservation after decode failure.
    void cancelLoad(std::size_t slot) noexcept;

    // Audio thread: acquire pins a ready page until release().
    [[nodiscard]] bool tryAcquire(PageKey key, ReadLease& lease) noexcept;
    void release(ReadLease& lease) noexcept;

    [[nodiscard]] State state(std::size_t slot) const noexcept;

private:
    struct Slot
    {
        std::atomic<State> state { State::empty };
        std::atomic<std::uint32_t> sampleId { 0 };
        std::atomic<std::uint64_t> pageIndex { 0 };
        std::atomic<std::uint32_t> channels { 0 };
        std::atomic<std::uint32_t> frames { 0 };
    };

    std::size_t slotCount = 0;
    std::uint32_t framesPerPage = 0;
    std::uint32_t channelLimit = 0;
    std::unique_ptr<Slot[]> slots;
    std::unique_ptr<float[]> storage;
    std::size_t samplesPerSlot = 0;
};
}
