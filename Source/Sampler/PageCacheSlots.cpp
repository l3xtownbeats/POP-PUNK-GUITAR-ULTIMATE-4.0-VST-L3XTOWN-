#include "PageCacheSlots.h"

#include <algorithm>
#include <limits>
#include <utility>

namespace ppgu::sampler
{
PageCacheSlots::ReadLease::ReadLease(ReadLease&& other) noexcept
{
    *this = std::move(other);
}

PageCacheSlots::ReadLease& PageCacheSlots::ReadLease::operator=(ReadLease&& other) noexcept
{
    if (this == &other) return *this;
    if (owner != nullptr) owner->release(*this);
    owner = other.owner; slot = other.slot; samples = other.samples;
    channels = other.channels; frames = other.frames;
    other.owner = nullptr; other.slot = 0; other.samples = nullptr;
    other.channels = 0; other.frames = 0;
    return *this;
}

PageCacheSlots::PageCacheSlots(std::size_t count, std::uint32_t pageFrames,
                               std::uint32_t maxChannels)
    : slotCount(count), framesPerPage(pageFrames), channelLimit(maxChannels)
{
    if (count == 0 || pageFrames == 0 || maxChannels == 0)
        return;
    if (static_cast<std::size_t>(pageFrames) >
        std::numeric_limits<std::size_t>::max() / static_cast<std::size_t>(maxChannels))
        return;
    const auto candidateSamplesPerSlot =
        static_cast<std::size_t>(pageFrames) * static_cast<std::size_t>(maxChannels);
    if (count > std::numeric_limits<std::size_t>::max() / candidateSamplesPerSlot)
        return;

    const auto totalSamples = count * candidateSamplesPerSlot;
    if (totalSamples > maximumCacheBytes / sizeof(float))
        return;

    // Cache construction happens off the audio thread. Treat memory pressure
    // as an invalid cache rather than allowing an allocation exception to
    // escape plugin initialization or a background library-load operation.
    try
    {
        auto candidateSlots = std::make_unique<Slot[]>(count);
        auto candidateStorage = std::make_unique<float[]>(totalSamples);
        std::fill_n(candidateStorage.get(), totalSamples, 0.0f);
        slots = std::move(candidateSlots);
        storage = std::move(candidateStorage);
        samplesPerSlot = candidateSamplesPerSlot;
    }
    catch (...)
    {
        slots.reset();
        storage.reset();
        samplesPerSlot = 0;
    }
}

bool PageCacheSlots::tryBeginLoad(PageKey key, std::size_t& resultSlot) noexcept
{
    if (!isValid())
        return false;

    // Prefer an empty slot. A ready slot may be evicted only if no audio
    // callback currently owns its read lease.
    for (int pass = 0; pass < 2; ++pass)
    {
        for (std::size_t i = 0; i < slotCount; ++i)
        {
            auto expected = pass == 0 ? emptyAccess : readyAccess;
            if (slots[i].access.compare_exchange_strong(expected, loadingAccess,
                    std::memory_order_acq_rel, std::memory_order_acquire))
            {
                slots[i].sampleId.store(key.sampleId, std::memory_order_relaxed);
                slots[i].pageIndex.store(key.pageIndex, std::memory_order_relaxed);
                slots[i].channels.store(0, std::memory_order_relaxed);
                slots[i].frames.store(0, std::memory_order_relaxed);
                resultSlot = i;
                return true;
            }
        }
    }
    return false;
}

float* PageCacheSlots::writableData(std::size_t slot) noexcept
{
    if (!isValid() || slot >= slotCount
        || slots[slot].access.load(std::memory_order_acquire) != loadingAccess)
        return nullptr;
    return storage.get() + slot * samplesPerSlot;
}

bool PageCacheSlots::publish(std::size_t slot, PageKey key,
                             std::uint32_t channels, std::uint32_t frames) noexcept
{
    if (!isValid() || slot >= slotCount || channels == 0
        || channels > channelLimit || frames == 0 || frames > framesPerPage)
        return false;

    auto& target = slots[slot];
    if (target.access.load(std::memory_order_acquire) != loadingAccess
        || target.sampleId.load(std::memory_order_relaxed) != key.sampleId
        || target.pageIndex.load(std::memory_order_relaxed) != key.pageIndex)
        return false;

    target.channels.store(channels, std::memory_order_relaxed);
    target.frames.store(frames, std::memory_order_relaxed);
    target.access.store(readyAccess, std::memory_order_release);
    return true;
}

void PageCacheSlots::cancelLoad(std::size_t slot) noexcept
{
    if (isValid() && slot < slotCount)
    {
        auto expected = loadingAccess;
        (void) slots[slot].access.compare_exchange_strong(expected, emptyAccess,
            std::memory_order_release, std::memory_order_relaxed);
    }
}

bool PageCacheSlots::containsReady(PageKey key) const noexcept
{
    if (!isValid())
        return false;
    for (std::size_t i = 0; i < slotCount; ++i)
    {
        const auto& candidate = slots[i];
        const auto access = candidate.access.load(std::memory_order_acquire);
        if (access < readyAccess)
            continue;
        if (candidate.sampleId.load(std::memory_order_relaxed) == key.sampleId
            && candidate.pageIndex.load(std::memory_order_relaxed) == key.pageIndex)
            return true;
    }
    return false;
}

bool PageCacheSlots::tryAcquire(PageKey key, ReadLease& lease) noexcept
{
    // A lease is an owning pin. Never overwrite a live lease: doing so
    // would leak its reader count and permanently prevent cache eviction.
    if (lease)
        return false;
    if (!isValid())
        return false;

    for (std::size_t i = 0; i < slotCount; ++i)
    {
        auto& candidate = slots[i];
        auto access = candidate.access.load(std::memory_order_acquire);
        // Multiple voices may read the same immutable page concurrently.
        // Increment the pin count atomically; eviction succeeds only from
        // readyAccess (zero readers), so the page cannot be overwritten.
        while (access >= readyAccess
               && access < std::numeric_limits<std::uint32_t>::max())
        {
            if (candidate.access.compare_exchange_weak(access, access + 1,
                    std::memory_order_acquire, std::memory_order_relaxed))
            {
                const bool matches = candidate.sampleId.load(std::memory_order_relaxed) == key.sampleId
                    && candidate.pageIndex.load(std::memory_order_relaxed) == key.pageIndex;
                if (!matches)
                {
                    candidate.access.fetch_sub(1, std::memory_order_release);
                    break;
                }

                lease.owner = this;
                lease.slot = i;
                lease.samples = storage.get() + i * samplesPerSlot;
                lease.channels = candidate.channels.load(std::memory_order_relaxed);
                lease.frames = candidate.frames.load(std::memory_order_relaxed);
                return true;
            }
        }
    }
    return false;
}

void PageCacheSlots::release(ReadLease& lease) noexcept
{
    // A lease may only be released by the cache instance that issued it.
    // A wrong-cache release must leave the original pin intact for its owner.
    if (!lease || lease.owner != this || !isValid() || lease.slot >= slotCount)
        return;
    // Each successful acquire owns exactly one pin.
    slots[lease.slot].access.fetch_sub(1, std::memory_order_release);
    lease.owner = nullptr;
    lease.slot = 0;
    lease.samples = nullptr;
    lease.channels = 0;
    lease.frames = 0;
}

PageCacheSlots::State PageCacheSlots::state(std::size_t slot) const noexcept
{
    if (!isValid() || slot >= slotCount)
        return State::empty;
    const auto access = slots[slot].access.load(std::memory_order_acquire);
    if (access == emptyAccess) return State::empty;
    if (access == loadingAccess) return State::loading;
    return access == readyAccess ? State::ready : State::reading;
}
}
