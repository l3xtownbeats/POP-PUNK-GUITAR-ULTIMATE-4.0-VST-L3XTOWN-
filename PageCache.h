#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <vector>

namespace ppgu {

// Fixed page cache with pin ownership; LRU eviction only among unpinned pages.
class PageCache {
public:
    static constexpr size_t kMaxPages = 64;
    static constexpr size_t kPageFrames = 4096; // samples per page

    struct Page {
        std::vector<float> frames{std::vector<float>(kPageFrames, 0.0f)};
        std::atomic<int> pins{0};
        std::atomic<uint64_t> lastUse{0};
        std::atomic<uint64_t> generation{0};
    };

    Page* acquire(uint64_t pageId, uint64_t gen);
    void  pin(Page* p);
    void  unpin(Page* p);
    size_t residentCount() const;
    // Returns count evicted (only unpinned, oldest-use pages).
    size_t evictLRU(size_t maxEvict);
private:
    std::array<Page, kMaxPages> pages_{};
    uint64_t clock_ = 0;
};

} // namespace ppgu