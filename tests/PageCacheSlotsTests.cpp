#include "Sampler/PageCacheSlots.h"

#include <iostream>

namespace
{
bool publishPage(ppgu::sampler::PageCacheSlots& cache, ppgu::sampler::PageKey key,
                 float value, std::size_t& slot)
{
    if (!cache.tryBeginLoad(key, slot))
        return false;
    auto* data = cache.writableData(slot);
    if (data == nullptr)
    {
        cache.cancelLoad(slot);
        return false;
    }
    for (std::uint32_t frame = 0; frame < cache.pageFrames(); ++frame)
        data[frame] = value;
    if (!cache.publish(slot, key, 1, cache.pageFrames()))
    {
        cache.cancelLoad(slot);
        return false;
    }
    return true;
}
}

int main()
{
    using namespace ppgu::sampler;
    PageCacheSlots cache(2, 4, 1);
    if (!cache.isValid() || cache.capacity() != 2 || cache.pageFrames() != 4)
        return 1;

    std::size_t firstSlot = 0;
    std::size_t secondSlot = 0;
    if (!publishPage(cache, { 1, 0 }, 1.0f, firstSlot)
        || !publishPage(cache, { 2, 0 }, 2.0f, secondSlot))
        return 2;

    PageCacheSlots::ReadLease firstLease;
    PageCacheSlots::ReadLease secondLease;
    if (!cache.tryAcquire({ 1, 0 }, firstLease)
        || !cache.tryAcquire({ 2, 0 }, secondLease)
        || firstLease.samples[0] != 1.0f || secondLease.samples[0] != 2.0f)
        return 3;

    std::size_t unavailableSlot = 0;
    if (cache.tryBeginLoad({ 3, 0 }, unavailableSlot))
        return 4; // Both slots are pinned: the worker must not evict either page.

    cache.release(firstLease);
    std::size_t thirdSlot = 0;
    if (!publishPage(cache, { 3, 0 }, 3.0f, thirdSlot))
        return 5;

    PageCacheSlots::ReadLease stillPinned;
    PageCacheSlots::ReadLease evicted;
    if (!cache.tryAcquire({ 2, 0 }, stillPinned)
        || stillPinned.samples[0] != 2.0f
        || cache.tryAcquire({ 1, 0 }, evicted))
        return 6;

    cache.release(secondLease);
    cache.release(stillPinned);

    std::size_t cancelledSlot = 0;
    if (!cache.tryBeginLoad({ 4, 0 }, cancelledSlot))
        return 7;
    cache.cancelLoad(cancelledSlot);
    if (cache.state(cancelledSlot) != PageCacheSlots::State::empty)
        return 8;
    std::size_t retrySlot = 0;
    if (!cache.tryBeginLoad({ 4, 0 }, retrySlot))
        return 9;
    cache.cancelLoad(retrySlot);

    std::cout << "Page cache contracts passed: pinned-page safety, eviction, cancel/retry.\n";
    return 0;
}
