#include "PageCacheSlots.h"

#include <algorithm>
#include <limits>

namespace ppgu::sampler
{
PageCacheSlots::PageCacheSlots(std::size_t count, std::uint32_t pageFrames,
                               std::uint32_t maxChannels)
    : slotCount(count), framesPerPage(pageFrames), channelLimit(maxChannels)
{
    if (count == 0 || pageFrames == 0 || maxChannels == 0)
        return;
    if (static_cast<std::size_t>(pageFrames) >
        std::numeric_limits<std::size_t>::max() / static_cast<std::size_t>(maxChannels))
        return;
    samplesPerSlot = static_cast<std::size_t>(pageFrames) * maxChannels;
    if (count > std::numeric_limits<std::size_t>::max() / samplesPerSlot)
        return;

    slots = std::make_unique<Slot[]>(count);
    storage = std::make_unique<float[]>(count * samplesPerSlot);
    std::fill_n(storage.get(), count * samplesPerSlot, 0.0f);
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
            auto expected = pass == 0 ? State::empty : State::ready;
            if (slots[i].state.compare_exchange_strong(expected, State::loading,
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
        || slots[slot].state.load(std::memory_order_acquire) != State::loading)
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
    if (target.state.load(std::memory_order_acquire) != State::loading
        || target.sampleId.load(std::memory_order_relaxed) != key.sampleId
        || target.pageIndex.load(std::memory_order_relaxed) != key.pageIndex)
        return false;

    target.channels.store(channels, std::memory_order_relaxed);
    target.frames.store(frames, std::memory_order_relaxed);
    target.state.store(State::ready, std::memory_order_release);
    return true;
}

void PageCacheSlots::cancelLoad(std::size_t slot) noexcept
{
    if (isValid() && slot < slotCount)
    {
        auto expected = State::loading;
        (void) slots[slot].state.compare_exchange_strong(expected, State::empty,
            std::memory_order_release, std::memory_order_relaxed);
    }
}

bool PageCacheSlots::tryAcquire(PageKey key, ReadLease& lease) noexcept
{
    lease = {};
    if (!isValid())
        return false;

    for (std::size_t i = 0; i < slotCount; ++i)
    {
        auto expected = State::ready;
        if (!slots[i].state.compare_exchange_strong(expected, State::reading,
                std::memory_order_acq_rel, std::memory_order_acquire))
            continue;

        auto& candidate = slots[i];
        const bool matches = candidate.sampleId.load(std::memory_order_relaxed) == key.sampleId
            && candidate.pageIndex.load(std::memory_order_relaxed) == key.pageIndex;
        if (!matches)
        {
            candidate.state.store(State::ready, std::memory_order_release);
            continue;
        }

        lease.slot = i;
        lease.samples = storage.get() + i * samplesPerSlot;
        lease.channels = candidate.channels.load(std::memory_order_relaxed);
        lease.frames = candidate.frames.load(std::memory_order_relaxed);
        return true;
    }
    return false;
}

void PageCacheSlots::release(ReadLease& lease) noexcept
{
    if (!isValid() || !lease || lease.slot >= slotCount)
    {
        lease = {};
        return;
    }
    auto expected = State::reading;
    (void) slots[lease.slot].state.compare_exchange_strong(expected, State::ready,
        std::memory_order_release, std::memory_order_relaxed);
    lease = {};
}

PageCacheSlots::State PageCacheSlots::state(std::size_t slot) const noexcept
{
    if (!isValid() || slot >= slotCount)
        return State::empty;
    return slots[slot].state.load(std::memory_order_acquire);
}
}
