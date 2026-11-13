#pragma once

#include "PatternTriggerMap.h"
#include <array>
#include <cstdint>

namespace ppgu::performance
{
// Audio-thread-safe fixed-storage state for the reserved C0-C1 PATTERN lane.
// A trigger selects one pattern slot and is never interpreted as a guitar note.
class PatternTriggerState final
{
public:
    [[nodiscard]] bool noteOn(int midiNote, int midiChannel) noexcept
    {
        const int slot = PatternTriggerMap::patternIndexForNote(midiNote);
        if (slot < 0 || midiChannel < 1 || midiChannel > 16)
            return false;

        auto& order = pressOrder_[static_cast<std::size_t>(midiChannel - 1)]
                                 [static_cast<std::size_t>(slot)];
        if (nextOrder_ == UINT64_MAX)
        {
            // Extremely defensive wrap handling; preserve current held state
            // while rebuilding a stable deterministic order.
            std::uint64_t next = 1;
            for (auto& channel : pressOrder_)
                for (auto& value : channel)
                    if (value != 0)
                        value = next++;
            nextOrder_ = next;
        }
        order = nextOrder_++;
        return true;
    }

    [[nodiscard]] bool noteOff(int midiNote, int midiChannel) noexcept
    {
        const int slot = PatternTriggerMap::patternIndexForNote(midiNote);
        if (slot < 0 || midiChannel < 1 || midiChannel > 16)
            return false;

        pressOrder_[static_cast<std::size_t>(midiChannel - 1)]
                   [static_cast<std::size_t>(slot)] = 0;
        return true;
    }

    void clear() noexcept
    {
        for (auto& channel : pressOrder_)
            channel.fill(0);
        nextOrder_ = 1;
    }

    [[nodiscard]] bool isActive() const noexcept
    {
        return selectedPatternIndex() >= 0;
    }

    [[nodiscard]] int selectedPatternIndex() const noexcept
    {
        std::uint64_t newestOrder = 0;
        int selected = -1;
        for (const auto& channel : pressOrder_)
            for (std::size_t slot = 0; slot < channel.size(); ++slot)
                if (channel[slot] > newestOrder)
                {
                    newestOrder = channel[slot];
                    selected = static_cast<int>(slot);
                }
        return selected;
    }

private:
    std::array<std::array<std::uint64_t, PatternTriggerMap::triggerCount>, 16> pressOrder_ {};
    std::uint64_t nextOrder_ = 1;
};
}
