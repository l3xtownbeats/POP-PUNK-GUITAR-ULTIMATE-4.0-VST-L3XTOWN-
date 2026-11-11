#pragma once

#include <array>

namespace ppgu::music
{
struct AutoChordResult
{
    std::array<int, 3> notes { -1, -1, -1 };
    int count = 0;
    bool snappedToScale = false;
};

// Build a compact diatonic triad from a single trigger note.
// scaleMode: 0 = chromatic major triad, 1 = major scale, 2 = natural minor.
// Out-of-scale roots snap to the nearest scale degree (ties resolve downward).
// This helper is allocation-free and safe to call from the audio thread.
inline AutoChordResult buildAutoChord(int midiRoot, int scaleRoot, int scaleMode) noexcept
{
    AutoChordResult result;
    if (midiRoot < 0 || midiRoot > 127 || scaleRoot < 0 || scaleRoot > 11
        || scaleMode < 0 || scaleMode > 2)
        return result;

    static constexpr std::array<int, 7> majorScale { 0, 2, 4, 5, 7, 9, 11 };
    static constexpr std::array<int, 7> minorScale { 0, 2, 3, 5, 7, 8, 10 };

    int chordRoot = midiRoot;
    int third = 4;
    int fifth = 7;

    if (scaleMode != 0)
    {
        const auto& scale = scaleMode == 1 ? majorScale : minorScale;
        const int relativePitch = (midiRoot % 12 - scaleRoot + 12) % 12;
        int degree = 0;
        int bestDistance = 13;
        for (int i = 0; i < 7; ++i)
        {
            const int clockwise = (scale[static_cast<std::size_t>(i)] - relativePitch + 12) % 12;
            const int counterClockwise = (relativePitch - scale[static_cast<std::size_t>(i)] + 12) % 12;
            const int distance = (clockwise < counterClockwise) ? clockwise : counterClockwise;
            if (distance < bestDistance)
            {
                bestDistance = distance;
                degree = i;
            }
        }

        const int targetPitchClass = (scaleRoot + scale[static_cast<std::size_t>(degree)]) % 12;
        int adjustment = (targetPitchClass - midiRoot % 12 + 12) % 12;
        if (adjustment > 6)
            adjustment -= 12;
        chordRoot += adjustment;
        result.snappedToScale = adjustment != 0;

        const int thirdDegree = (degree + 2) % 7;
        const int fifthDegree = (degree + 4) % 7;
        third = scale[static_cast<std::size_t>(thirdDegree)]
            - scale[static_cast<std::size_t>(degree)] + (thirdDegree <= degree ? 12 : 0);
        fifth = scale[static_cast<std::size_t>(fifthDegree)]
            - scale[static_cast<std::size_t>(degree)] + (fifthDegree <= degree ? 12 : 0);
    }

    while (chordRoot + fifth > 127)
        chordRoot -= 12;
    if (chordRoot < 0)
        return AutoChordResult{};

    result.notes = { chordRoot, chordRoot + third, chordRoot + fifth };
    result.count = 3;
    return result;
}
}
