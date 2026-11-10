#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <type_traits>

namespace ppgu::sampler
{
// Fixed-capacity single-producer/single-consumer queue.
// One audio thread may submit requests; one worker thread consumes them.
// Capacity must be a power of two. One slot is intentionally left unused
// to distinguish full from empty, so usable capacity is Capacity - 1.
// No allocation, locks, or waits occur in push/pop.
template <typename T, std::size_t Capacity>
class BoundedSpscQueue final
{
    static_assert(Capacity >= 2, "Queue capacity must be at least two.");
    static_assert((Capacity & (Capacity - 1)) == 0, "Queue capacity must be a power of two.");
    static_assert(std::is_trivially_copyable_v<T>, "Queue items must be trivially copyable.");

public:
    BoundedSpscQueue() = default;
    BoundedSpscQueue(const BoundedSpscQueue&) = delete;
    BoundedSpscQueue& operator=(const BoundedSpscQueue&) = delete;

    [[nodiscard]] bool tryPush(const T& item) noexcept
    {
        const auto write = writeIndex.load(std::memory_order_relaxed);
        const auto next = (write + 1) & mask;
        if (next == readIndex.load(std::memory_order_acquire))
            return false;

        items[write] = item;
        writeIndex.store(next, std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool tryPop(T& item) noexcept
    {
        const auto read = readIndex.load(std::memory_order_relaxed);
        if (read == writeIndex.load(std::memory_order_acquire))
            return false;

        item = items[read];
        readIndex.store((read + 1) & mask, std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return readIndex.load(std::memory_order_acquire)
            == writeIndex.load(std::memory_order_acquire);
    }

    static constexpr std::size_t usableCapacity() noexcept { return Capacity - 1; }

private:
    static constexpr std::size_t mask = Capacity - 1;
    alignas(64) std::array<T, Capacity> items {};
    alignas(64) std::atomic<std::size_t> writeIndex { 0 };
    alignas(64) std::atomic<std::size_t> readIndex { 0 };
};
}
