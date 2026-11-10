#pragma once

#include <array>

namespace ppgu::performance
{
    // Snap a MIDI note to the nearest pitch in the selected key. This is
    // deliberately a small, deterministic helper: no allocation, locking or
    // floating-point work is performed in the audio callback.
    constexpr int mapNoteToScale(int midiNote, int scaleRoot, int scaleMode) noexcept
    {
        constexpr std::array<int, 7> major { 0, 2, 4, 5, 7, 9, 11 };
        constexpr std::array<int, 7> minor { 0, 2, 3, 5, 7, 8, 10 };
        if (midiNote < 0 || midiNote > 127 || scaleMode <= 0 || scaleMode > 2)
            return midiNote;

        const int root = ((scaleRoot % 12) + 12) % 12;
        const int pitchClass = midiNote % 12;
        int bestNote = midiNote;
        int bestDistance = 13;
        const auto consider = [&](int interval)
        {
            const int targetPitch = (root + interval) % 12;
            int delta = (targetPitch - pitchClass + 12) % 12;
            if (delta > 6) delta -= 12;
            const int distance = delta < 0 ? -delta : delta;
            const int candidate = midiNote + delta;
            if (candidate >= 0 && candidate <= 127 && distance < bestDistance)
            {
                bestDistance = distance;
                bestNote = candidate;
            }
        };

        if (scaleMode == 1)
            for (const int interval : major) consider(interval);
        else
            for (const int interval : minor) consider(interval);

        return bestNote;
    }
}
