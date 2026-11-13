#include "PageCache.h"
#include <algorithm>

namespace ppgu {

PageCache::Page* PageCache::acquire(uint64_t pageId, uint64_t gen) {
    (void)pageId; (void)gen;
    return nullptr; // resolved by streaming service; header kept for contract tests
}

void PageCache::pin(Page* p) { if (p) p->pins.fetch_add(1, std::memory_order_relaxed); }
void PageCache::unpin(Page* p) { if (p) p->pins.fetch_sub(1, std::memory_order_relaxed); }

size_t PageCache::residentCount() const {
    size_t n = 0;
    for (auto& p : pages_) if (p.generation.load(std::memory_order_relaxed) != 0) ++n;
    return n;
}

size_t PageCache::evictLRU(size_t maxEvict) {
    size_t evicted = 0;
    for (auto& p : pages_) {
        if (evicted >= maxEvict) break;
        if (p.pins.load(std::memory_order_relaxed) != 0) continue;
        if (p.generation.exchange(0) != 0) ++evicted;
    }
    (void)clock_;
    return evicted;
}

} // namespace ppgu