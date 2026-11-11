#pragma once

#include <cmath>

namespace ppgu::performance
{
    struct ChordTransitionDecision
    {
        bool releaseVoices = false;
        bool clearPendingNoteOffs = false;
        bool startPatternCycle = false;
        double patternCycleStartPpq = 0.0;
    };

    // A chord change is a harmonic input update, not a pattern restart.
    // Keep the phrase clock and already scheduled gates when moving between
    // non-empty chords. Only the first chord starts a cycle; an empty chord
    // stops playback and releases outstanding voices/gates.
    inline ChordTransitionDecision makeChordTransitionDecision(
        int previousChordNoteCount,
        int nextChordNoteCount,
        double changePpq,
        double currentPatternCycleStartPpq,
        bool startOnFirstChord) noexcept
    {
        if (nextChordNoteCount <= 0)
            return { previousChordNoteCount > 0, previousChordNoteCount > 0,
                     false, currentPatternCycleStartPpq };

        if (previousChordNoteCount <= 0)
        {
            const double safeChangePpq = std::isfinite(changePpq) ? changePpq : 0.0;
            double cycleStart = safeChangePpq;
            if (!startOnFirstChord)
            {
                const double nextBar = std::ceil(safeChangePpq / 4.0 - 1.0e-9) * 4.0;
                if (std::isfinite(nextBar))
                    cycleStart = nextBar;
            }
            return { false, false, true, cycleStart };
        }

        return { false, false, false, currentPatternCycleStartPpq };
    }
}
