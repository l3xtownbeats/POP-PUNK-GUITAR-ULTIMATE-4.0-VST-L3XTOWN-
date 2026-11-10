#include "PageCacheSlots.h"

#include <cstddef>
#include <iostream>

using ppgu::sampler::PageCacheSlots;
using ppgu::sampler::PageKey;

int main()
{
    PageCacheSlots cache(2, 8, 2);
    if (!cache.isValid() || cache.capacity() != 2)
        return 1;

    std::size_t first = 99, second = 99;
    if (!cache.tryBeginLoad({ 10, 0 }, first) || first >= 2)
        return 1;
    auto* data = cache.writableData(first);
    if (data == nullptr)
        return 1;
    for (std::size_t i = 0; i < 8; ++i)
    {
        data[i] = static_cast<float>(i + 1);
        data[8 + i] = static_cast<float>(100 + i);
    }
    if (!cache.publish(first, { 10, 0 }, 2, 8))
        return 1;

    PageCacheSlots::ReadLease lease;
    if (!cache.tryAcquire({ 10, 0 }, lease) || !lease
        || lease.channels != 2 || lease.frames != 8
        || lease.samples[0] != 1.0f || lease.samples[8] != 100.0f)
        return 1;

    // A pinned page must never be selected for worker overwrite.
    if (!cache.tryBeginLoad({ 20, 1 }, second))
        return 1;
    if (second == first)
        return 1;
    if (cache.tryBeginLoad({ 30, 2 }, second))
    {
        std::cerr << "Cache overwrote a pinned/loading page.\n";
        return 1;
    }

    cache.cancelLoad(second);
    if (cache.state(first) != PageCacheSlots::State::reading)
        return 1;

    cache.release(lease);
    if (lease || cache.state(first) != PageCacheSlots::State::ready)
        return 1;

    // Once unpinned, a ready page can be evicted and republished.
    if (!cache.tryBeginLoad({ 99, 7 }, second))
        return 1;
    auto* replacement = cache.writableData(second);
    if (replacement == nullptr)
        return 1;
    replacement[0] = -0.5f;
    if (!cache.publish(second, { 99, 7 }, 1, 1))
        return 1;
    if (!cache.tryAcquire({ 99, 7 }, lease) || lease.samples[0] != -0.5f)
        return 1;
    cache.release(lease);

    if (cache.tryAcquire({ 1234, 999 }, lease))
        return 1;


    // A sample page is shared by concurrent voices. Each reader pins the same
    // slot independently; the worker must not evict it until every lease ends.
    PageCacheSlots sharedPageCache(1, 4, 1);
    std::size_t sharedSlot = 0;
    if (!sharedPageCache.tryBeginLoad({ 7, 3 }, sharedSlot)) return 1;
    auto* sharedData = sharedPageCache.writableData(sharedSlot);
    if (sharedData == nullptr) return 1;
    sharedData[0] = 0.75f;
    if (!sharedPageCache.publish(sharedSlot, { 7, 3 }, 1, 1)) return 1;
    PageCacheSlots::ReadLease voiceA, voiceB;
    if (!sharedPageCache.tryAcquire({ 7, 3 }, voiceA)
        || !sharedPageCache.tryAcquire({ 7, 3 }, voiceB)
        || voiceA.samples != voiceB.samples || voiceA.samples[0] != 0.75f)
    {
        std::cerr << "Concurrent voices could not acquire the same cached page.\\n";
        return 1;
    }
    sharedPageCache.release(voiceA);
    if (sharedPageCache.tryBeginLoad({ 8, 0 }, sharedSlot))
    {
        std::cerr << "Cache evicted a page while a second voice still held a read lease.\\n";
        return 1;
    }
    sharedPageCache.release(voiceB);
    if (!sharedPageCache.tryBeginLoad({ 8, 0 }, sharedSlot))
    {
        std::cerr << "Cache did not make a page evictable after all readers released it.\\n";
        return 1;
    }

    PageCacheSlots invalid(0, 0, 0);
    if (invalid.isValid() || invalid.tryBeginLoad({ 1, 1 }, first))
        return 1;

    std::cout << "Page cache slot tests passed: publish/acquire, planar data, pin protection, cancel, eviction and misses.\n";
    return 0;
}
