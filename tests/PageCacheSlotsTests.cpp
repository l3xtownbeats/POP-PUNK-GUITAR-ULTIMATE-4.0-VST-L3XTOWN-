#include "PageCacheSlots.h"

#include <cstddef>
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <type_traits>

using ppgu::sampler::PageCacheSlots;
using ppgu::sampler::PageKey;

static_assert(!std::is_copy_constructible_v<PageCacheSlots::ReadLease>);
static_assert(!std::is_copy_assignable_v<PageCacheSlots::ReadLease>);
static_assert(!std::is_move_constructible_v<PageCacheSlots::ReadLease>);
static_assert(!std::is_move_assignable_v<PageCacheSlots::ReadLease>);

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

    if (!cache.containsReady({ 10, 0 }) || cache.containsReady({ 10, 1 }))
    {
        std::cerr << "Worker cache-ready lookup returned an incorrect key match.\\n";
        return 1;
    }

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
    // A cache must not release another cache instance's lease even if
    // both caches have a slot with the same numeric index.
    PageCacheSlots otherCache(1, 8, 2);
    if (!otherCache.isValid() || !cache.tryAcquire({ 10, 0 }, lease))
        return 1;
    otherCache.release(lease);
    if (!lease || cache.state(first) != PageCacheSlots::State::reading)
    {
        std::cerr << "A foreign cache incorrectly released another cache's pin.\\n";
        return 1;
    }
    cache.release(lease);
    if (lease || cache.state(first) != PageCacheSlots::State::ready)
        return 1;
    // Re-acquiring into an already-owned lease must fail without
    // discarding the original pin (otherwise the cache leaks a reader count).
    if (!cache.tryAcquire({ 10, 0 }, lease))
        return 1;
    const auto* originalLeaseData = lease.samples;
    PageCacheSlots::ReadLease accidentalOverwrite;
    if (cache.tryAcquire({ 10, 0 }, lease)
        || !lease || lease.samples != originalLeaseData
        || cache.state(first) != PageCacheSlots::State::reading)
    {
        std::cerr << "tryAcquire overwrote an active lease or lost its pin.\\n";
        return 1;
    }
    cache.release(lease);
    if (lease || cache.state(first) != PageCacheSlots::State::ready)
    {
        std::cerr << "Active lease was not released after rejected double acquire.\\n";
        return 1;
    }

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


    // Concurrent stress: several audio-like readers repeatedly pin and read
    // the same immutable page while a worker-like thread attempts eviction.
    // A failed acquire is an allowed cache miss; a corrupted value is not.
    PageCacheSlots stressCache(2, 16, 1);
    std::size_t stressSlot = 0;
    if (!stressCache.tryBeginLoad({ 42, 9 }, stressSlot)) return 1;
    auto* stressData = stressCache.writableData(stressSlot);
    if (stressData == nullptr) return 1;
    for (int i = 0; i < 16; ++i) stressData[i] = 0.375f;
    if (!stressCache.publish(stressSlot, { 42, 9 }, 1, 16)) return 1;

    std::atomic<bool> startReaders { false };
    std::atomic<bool> startEvictor { false };
    std::atomic<int> corruptReads { 0 };
    std::atomic<int> successfulReads { 0 };
    std::vector<std::thread> readers;
    for (int threadIndex = 0; threadIndex < 4; ++threadIndex)
    {
        readers.emplace_back([&]
        {
            while (!startReaders.load(std::memory_order_acquire))
                std::this_thread::yield();
            for (int iteration = 0; iteration < 20000; ++iteration)
            {
                PageCacheSlots::ReadLease concurrentLease;
                if (stressCache.tryAcquire({ 42, 9 }, concurrentLease))
                {
                    if (concurrentLease.frames != 16
                        || concurrentLease.samples[0] != 0.375f
                        || concurrentLease.samples[15] != 0.375f)
                        corruptReads.fetch_add(1, std::memory_order_relaxed);
                    successfulReads.fetch_add(1, std::memory_order_relaxed);
                    stressCache.release(concurrentLease);
                }
            }
        });
    }
    std::thread evictor([&]
    {
        while (!startEvictor.load(std::memory_order_acquire))
            std::this_thread::yield();
        for (int iteration = 0; iteration < 20000; ++iteration)
        {
            std::size_t workerSlot = 0;
            if (stressCache.tryBeginLoad({ 100, static_cast<std::uint64_t>(iteration) }, workerSlot))
            {
                auto* workerData = stressCache.writableData(workerSlot);
                if (workerData != nullptr)
                {
                    workerData[0] = -0.25f;
                    (void) stressCache.publish(workerSlot,
                        { 100, static_cast<std::uint64_t>(iteration) }, 1, 1);
                }
            }
        }
    });
    // First prove readers can acquire the published page before allowing the
    // evictor to race them. This prevents a scheduler-dependent false failure
    // where the worker evicts the page before any reader gets CPU time.
    startReaders.store(true, std::memory_order_release);
    while (successfulReads.load(std::memory_order_acquire) < 4)
        std::this_thread::yield();
    startEvictor.store(true, std::memory_order_release);
    for (auto& readerThread : readers) readerThread.join();
    evictor.join();
    if (corruptReads.load(std::memory_order_relaxed) != 0
        || successfulReads.load(std::memory_order_relaxed) == 0)
    {
        std::cerr << "Concurrent page-cache stress detected corrupted reads or no successful reads.\\n";
        return 1;
    }

    PageCacheSlots invalid(0, 0, 0);
    if (invalid.isValid() || invalid.tryBeginLoad({ 1, 1 }, first))
        return 1;

    std::cout << "Page cache slot tests passed: publish/acquire, planar data, pin protection, cancel, eviction and misses.\n";
    return 0;
}
