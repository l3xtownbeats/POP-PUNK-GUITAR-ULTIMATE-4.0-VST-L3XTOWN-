#include "PatternTriggerState.h"

int main()
{
    using ppgu::performance::PatternTriggerState;
    using ppgu::performance::PatternTriggerMap;

    PatternTriggerState state;
    if (state.isActive() || state.selectedPatternIndex() != -1)
        return 1;

    // Every reserved C0-C1 note selects its corresponding one of 13 slots.
    for (int slot = 0; slot < PatternTriggerMap::triggerCount; ++slot)
    {
        const int note = PatternTriggerMap::midiNoteForPattern(slot);
        if (!state.noteOn(note, 1) || !state.isActive()
            || state.selectedPatternIndex() != slot)
            return 2;
        if (!state.noteOff(note, 1) || state.isActive())
            return 3;
    }

    // Non-trigger pitches and invalid channels are not consumed.
    if (state.noteOn(25, 1) || state.noteOn(12, 0) || state.noteOff(24, 17))
        return 4;

    // Multiple held triggers: most recently pressed wins; releasing it
    // restores the most recently pressed trigger that remains held.
    if (!state.noteOn(12, 1) || !state.noteOn(18, 2)
        || state.selectedPatternIndex() != 6)
        return 5;
    if (!state.noteOff(18, 2) || state.selectedPatternIndex() != 0)
        return 6;

    // Retriggering the same key makes it newest without changing its slot.
    if (!state.noteOn(13, 1) || state.selectedPatternIndex() != 1)
        return 7;
    if (!state.noteOff(13, 1) || state.selectedPatternIndex() != 0)
        return 8;

    state.clear();
    return state.isActive() || state.selectedPatternIndex() != -1 ? 9 : 0;
}
