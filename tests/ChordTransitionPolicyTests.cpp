#include "ChordTransitionPolicy.h"
#include "PatternScheduler.h"

#include <array>
#include <cmath>
#include <iostream>
#include <limits>

using namespace ppgu::performance;

int main()
{
    // The first chord starts a phrase immediately when the option is enabled.
    const auto immediateStart = makeChordTransitionDecision(0, 3, 2.375, 0.0, true);
    if (!immediateStart.startPatternCycle || immediateStart.releaseVoices
        || immediateStart.clearPendingNoteOffs
        || std::abs(immediateStart.patternCycleStartPpq - 2.375) > 1.0e-9)
        return 1;

    // Otherwise the first chord waits for the next 4/4 bar boundary.
    const auto barStart = makeChordTransitionDecision(0, 2, 4.125, 0.0, false);
    if (!barStart.startPatternCycle
        || std::abs(barStart.patternCycleStartPpq - 8.0) > 1.0e-9)
        return 2;

    // A non-empty chord-to-chord transition changes harmony without stopping
    // current voices, deleting their gates, or resetting the phrase clock.
    const auto transition = makeChordTransitionDecision(3, 2, 1.25, 0.5, false);
    if (transition.releaseVoices || transition.clearPendingNoteOffs
        || transition.startPatternCycle
        || std::abs(transition.patternCycleStartPpq - 0.5) > 1.0e-9)
        return 3;

    // Repeated changes inside one quantize window must not repeatedly restart
    // the cycle or erase NoteOffs belonging to already-triggered notes.
    const auto secondTransition = makeChordTransitionDecision(
        2, 4, 1.375, transition.patternCycleStartPpq, false);
    if (secondTransition.releaseVoices || secondTransition.clearPendingNoteOffs
        || secondTransition.startPatternCycle
        || std::abs(secondTransition.patternCycleStartPpq - 0.5) > 1.0e-9)
        return 4;

    // Releasing the last held chord note stops the cycle and requests both
    // voice release and pending-gate cleanup.
    const auto emptyChord = makeChordTransitionDecision(2, 0, 1.5, 0.5, false);
    if (!emptyChord.releaseVoices || !emptyChord.clearPendingNoteOffs
        || emptyChord.startPatternCycle
        || std::abs(emptyChord.patternCycleStartPpq - 0.5) > 1.0e-9)
        return 5;

    // Invalid PPQ must not propagate NaN into the phrase clock.
    const auto invalidPpq = makeChordTransitionDecision(
        0, 1, std::numeric_limits<double>::quiet_NaN(), 0.0, true);
    if (!invalidPpq.startPatternCycle
        || !std::isfinite(invalidPpq.patternCycleStartPpq)
        || invalidPpq.patternCycleStartPpq != 0.0)
        return 6;

    // Exercise the policy together with the real fixed-capacity scheduler:
    // a NoteOff queued for the old chord must survive a non-empty transition.
    PatternScheduler scheduler;
    std::array<ScheduledEvent, PatternScheduler::maximumEventsPerBlock> output {};
    const std::array<PatternEvent, 1> oldChordPattern {{
        { 0.0, 1.0, 60, 100, 1 }
    }};
    const BlockClock firstInterval { true, true, false, false, 120.0, 0.0, 0.25 };
    const auto firstCount = scheduler.schedule(
        oldChordPattern, 4.0, 0.0, firstInterval, 48000.0, 6000, 0.0, 0, output);
    if (firstCount != 1 || output[0].kind != EventKind::NoteOn || output[0].note != 60)
        return 7;

    const auto schedulerTransition = makeChordTransitionDecision(1, 2, 0.25, 0.0, false);
    if (schedulerTransition.releaseVoices || schedulerTransition.clearPendingNoteOffs)
        return 8;
    if (schedulerTransition.clearPendingNoteOffs)
        scheduler.clearPendingNoteOffs();

    const std::array<PatternEvent, 1> newChordPattern {{
        { 0.25, 0.25, 64, 100, 1 }
    }};
    const BlockClock secondInterval { true, true, false, false, 120.0, 0.25, 0.5 };
    const auto secondCount = scheduler.schedule(
        newChordPattern, 4.0, 0.0, secondInterval, 48000.0, 6000, 0.0, 0, output);
    if (secondCount != 1 || output[0].kind != EventKind::NoteOn || output[0].note != 64)
        return 9;

    const BlockClock noteOffInterval { true, true, false, false, 120.0, 1.0, 1.25 };
    const auto noteOffCount = scheduler.schedule(
        newChordPattern, 4.0, 0.0, noteOffInterval, 48000.0, 6000, 0.0, 0, output);
    bool oldNoteOffFound = false;
    for (std::size_t i = 0; i < noteOffCount; ++i)
        oldNoteOffFound = oldNoteOffFound
            || (output[i].kind == EventKind::NoteOff && output[i].note == 60
                && output[i].sampleOffset == 0);
    if (!oldNoteOffFound)
        return 10;

    std::cout << "Chord transition policy tests passed.\n";
    return 0;
}
