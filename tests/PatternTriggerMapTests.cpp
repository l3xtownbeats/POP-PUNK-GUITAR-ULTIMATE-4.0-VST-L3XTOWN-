#include "PatternTriggerMap.h"

#include <iostream>

int main()
{
    using ppgu::performance::PatternTriggerMap;

    if (PatternTriggerMap::firstMidiNote != 12
        || PatternTriggerMap::lastMidiNote != 24
        || PatternTriggerMap::triggerCount != 13)
        return 1;

    for (int note = PatternTriggerMap::firstMidiNote;
         note <= PatternTriggerMap::lastMidiNote; ++note)
    {
        const int index = note - PatternTriggerMap::firstMidiNote;
        if (!PatternTriggerMap::isTriggerNote(note)
            || PatternTriggerMap::patternIndexForNote(note) != index
            || PatternTriggerMap::midiNoteForPattern(index) != note)
        {
            std::cerr << "Pattern trigger mapping failed at MIDI note " << note << "\\n";
            return 1;
        }
    }

    if (PatternTriggerMap::isTriggerNote(11)
        || PatternTriggerMap::isTriggerNote(25)
        || PatternTriggerMap::patternIndexForNote(11) != -1
        || PatternTriggerMap::patternIndexForNote(25) != -1
        || PatternTriggerMap::midiNoteForPattern(-1) != -1
        || PatternTriggerMap::midiNoteForPattern(13) != -1)
    {
        std::cerr << "Out-of-range MIDI notes were incorrectly treated as pattern triggers.\\n";
        return 1;
    }

    return 0;
}
