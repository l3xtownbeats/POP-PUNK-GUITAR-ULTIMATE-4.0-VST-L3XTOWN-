#include "AutoChordScale.h"
#include <array>
#include <cassert>

int main()
{
    using ppgu::performance::mapNoteToScale;

    // C major: chromatic notes snap to the nearest in-key semitone.
    assert(mapNoteToScale(60, 0, 1) == 60); // C
    assert(mapNoteToScale(61, 0, 1) == 60); // C# -> C (tie resolves downward)
    assert(mapNoteToScale(63, 0, 1) == 62); // D# -> D
    assert(mapNoteToScale(66, 0, 1) == 65); // F# -> F
    assert(mapNoteToScale(70, 0, 1) == 69); // A#

    // A minor has the same pitch classes as C major, with A as root.
    assert(mapNoteToScale(61, 9, 2) == 60);
    assert(mapNoteToScale(63, 9, 2) == 62); // D# is equidistant; deterministic lower-note tie break

    // Root transposition and octave boundaries.
    assert(mapNoteToScale(60, 2, 1) == 61); // C -> C# in D major
    assert(mapNoteToScale(0, 0, 1) == 0);
    assert(mapNoteToScale(127, 11, 1) == 127);

    // Chromatic mode and invalid inputs are pass-through.
    assert(mapNoteToScale(61, 0, 0) == 61);
    assert(mapNoteToScale(61, 0, 3) == 61);
    assert(mapNoteToScale(-1, 0, 1) == -1);
    assert(mapNoteToScale(128, 0, 1) == 128);


    // AutoChords: one root key generates a diatonic triad in Pattern mode.
    std::array<int, 3> triad {};
    assert(ppgu::performance::makeDiatonicTriad(60, 0, 1, triad));
    assert((triad == std::array<int, 3> { 60, 64, 67 })); // C major
    assert(ppgu::performance::makeDiatonicTriad(69, 9, 2, triad));
    assert((triad == std::array<int, 3> { 69, 72, 76 })); // A minor
    assert(ppgu::performance::makeDiatonicTriad(67, 0, 1, triad));
    assert((triad == std::array<int, 3> { 67, 71, 74 })); // G major
    assert(!ppgu::performance::makeDiatonicTriad(60, 0, 0, triad));

    // Two physical keys can snap to the same pitch. Releasing one must not
    // clear the mapped pitch while the other source key is still held.
    ppgu::performance::MappedNoteTracker tracker;
    tracker.noteOn(0, 60, 60); // C -> C
    tracker.noteOn(0, 61, 60); // C# -> C
    assert(tracker.isHeld(0, 60));
    assert(tracker.noteOff(0, 61) == 60);
    assert(tracker.isHeld(0, 60)); // C remains held
    assert(tracker.noteOff(0, 60) == 60);
    assert(!tracker.isHeld(0, 60));

    // Retriggering one source key replaces its ownership without leaking a
    // reference, and channels remain isolated.
    tracker.noteOn(0, 61, 60);
    tracker.noteOn(0, 61, 62);
    assert(!tracker.isHeld(0, 60));
    assert(tracker.isHeld(0, 62));
    tracker.noteOn(1, 61, 62);
    assert(tracker.isHeld(1, 62));
    assert(tracker.noteOff(0, 61) == 62);
    assert(tracker.isHeld(1, 62));
    tracker.clear();
    assert(!tracker.isHeld(1, 62));

    // All 128 source pitches may map to one target pitch. The counter must
    // preserve the final held key instead of saturating at 127.
    for (int source = 0; source < 128; ++source)
        tracker.noteOn(0, source, 60);
    for (int source = 0; source < 127; ++source)
        tracker.noteOff(0, source);
    assert(tracker.isHeld(0, 60));
    tracker.noteOff(0, 127);
    assert(!tracker.isHeld(0, 60));
}
