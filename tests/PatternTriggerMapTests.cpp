#include "PatternTriggerMap.h"

int main()
{
    using ppgu::performance::PatternTriggerMap;

    static_assert(PatternTriggerMap::firstMidiNote == 12);
    static_assert(PatternTriggerMap::lastMidiNote == 24);
    static_assert(PatternTriggerMap::triggerCount == 13);

    for (int note = 0; note < 128; ++note)
    {
        const bool expected = note >= 12 && note <= 24;
        if (PatternTriggerMap::isTriggerNote(note) != expected)
            return 1;
        const int index = PatternTriggerMap::patternIndexForNote(note);
        if (expected && (index != note - 12 || PatternTriggerMap::midiNoteForPattern(index) != note))
            return 2;
        if (!expected && index != -1)
            return 3;
    }

    return PatternTriggerMap::midiNoteForPattern(-1) == -1
        && PatternTriggerMap::midiNoteForPattern(13) == -1 ? 0 : 4;
}
