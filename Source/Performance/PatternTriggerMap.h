#pragma once

namespace ppgu::performance
{
// Approved PPGU layout: C0 through C1 are reserved exclusively for PATTERN
// launch commands. Melody notes begin above C1. The 13 keys select 13 slots;
// browsing a variation replaces slot content, never this MIDI assignment.
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
