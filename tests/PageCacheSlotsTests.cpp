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

    // Invalid page geometry must roll the worker reservation back so the
    // bounded cache cannot leak capacity after a rejected publish.
    std::size_t rejected = 99;
    if (!cache.tryBeginLoad({ 77, 4 }, rejected))
        return 1;
    if (cache.publish(rejected, { 77, 4 }, 0, 8)
        || cache.state(rejected) != PageCacheSlots::State::empty)
        return 1;
    if (!cache.tryBeginLoad({ 77, 4 }, rejected))
        return 1;
    cache.cancelLoad(rejected);

    // A stale worker key must not cancel a different live reservation.
    if (!cache.tryBeginLoad({ 88, 5 }, rejected))
        return 1;
    if (cache.publish(rejected, { 88, 6 }, 1, 8)
        || cache.state(rejected) != PageCacheSlots::State::loading)
        return 1;
    cache.cancelLoad(rejected);

    PageCacheSlots invalid(0, 0, 0);
    if (invalid.isValid() || invalid.tryBeginLoad({ 1, 1 }, first))
        return 1;

    std::cout << "Page cache slot tests passed: publish/acquire, planar data, pin protection, cancel, eviction and misses.\n";
    return 0;
}
