#pragma once

#include <cstdint>

namespace ppgu::performance
{
// MIDI note layout reserved for pattern-launch commands.
// MIDI 12..24 inclusive corresponds to C0..C1 in the agreed PPGU layout.
// These notes are control-only and must never be sent to the guitar sampler.
class PatternTriggerMap final
{
public:
    static constexpr int firstMidiNote = 12; // C0
    static constexpr int lastMidiNote = 24;  // C1
    static constexpr int triggerCount = lastMidiNote - firstMidiNote + 1;

    [[nodiscard]] static constexpr bool isTriggerNote(int midiNote) noexcept
    {
        return midiNote >= firstMidiNote && midiNote <= lastMidiNote;
    }

    // Returns a zero-based pattern slot for C0..C1, or -1 outside the range.
    [[nodiscard]] static constexpr int patternIndexForNote(int midiNote) noexcept
    {
        return isTriggerNote(midiNote) ? midiNote - firstMidiNote : -1;
    }

    [[nodiscard]] static constexpr int midiNoteForPattern(int patternIndex) noexcept
    {
        return patternIndex >= 0 && patternIndex < triggerCount
            ? firstMidiNote + patternIndex : -1;
    }
};
}
