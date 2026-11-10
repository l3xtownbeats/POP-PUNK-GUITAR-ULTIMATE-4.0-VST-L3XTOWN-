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
}
