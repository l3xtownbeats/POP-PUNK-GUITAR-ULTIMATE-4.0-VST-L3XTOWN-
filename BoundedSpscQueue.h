#pragma once
#include <atomic>
#include <cstdint>
#include <vector>

namespace ppgu {

// Lock-free bounded SPSC queue with overflow counter; wrap-around by design.
template <typename T>
class BoundedSpscQueue {
public:
    explicit BoundedSpscQueue(size_t capacity)
        : slots_(capacity), mask_(capacity - 1) {
        // capacity must be power of two
        for (size_t c = 1; c < capacity; c <<= 1) { if ((capacity & (capacity - 1)) != 0) break; }
    }
    bool push(const T& v) {
        auto h = head_.load(std::memory_order_relaxed);
        auto t = tail_.load(std::memory_order_acquire);
        if (h - t == slots_.size()) { overflows_.fetch_add(1, std::memory_order_relaxed); return false; }
        slots_[h & mask_] = v;
        head_.store(h + 1, std::memory_order_release);
        return true;
    }
    bool pop(T& out) {
        auto t = tail_.load(std::memory_order_relaxed);
        auto h = head_.load(std::memory_order_acquire);
        if (t == h) return false;
        out = slots_[t & mask_];
        tail_.store(t + 1, std::memory_order_release);
        return true;
    }
    void drain() { T tmp; while (pop(tmp)) {} }
    uint64_t overflowCount() const { return overflows_.load(std::memory_order_relaxed); }
private:
    std::vector<T> slots_;
    size_t mask_;
    std::atomic<uint64_t> head_{0}, tail_{0};
    std::atomic<uint64_t> overflows_{0};
};

} // namespace ppgu