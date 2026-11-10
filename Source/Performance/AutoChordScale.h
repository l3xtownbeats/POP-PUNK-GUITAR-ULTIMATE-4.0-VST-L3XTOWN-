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

    // Tracks the relationship between physical/source MIDI keys and mapped
    // pitches. Multiple source keys may snap to the same scale pitch; a
    // reference count prevents one NoteOff from releasing another held key.
    // Fixed-size storage keeps all operations allocation- and lock-free.
    class MappedNoteTracker
    {
    public:
        void clear() noexcept
        {
            for (auto& channel : sourceToMapped)
                channel.fill(-1);
            for (auto& channel : mappedRefCounts)
                channel.fill(0);
        }

        void noteOn(int channel, int sourceNote, int mappedNote) noexcept
        {
            if (!valid(channel, sourceNote) || !valid(channel, mappedNote))
                return;

            auto& previous = sourceToMapped[static_cast<std::size_t>(channel)][static_cast<std::size_t>(sourceNote)];
            auto& counts = mappedRefCounts[static_cast<std::size_t>(channel)];
            if (previous >= 0)
                decrement(counts[static_cast<std::size_t>(previous)]);

            previous = mappedNote;
            auto& count = counts[static_cast<std::size_t>(mappedNote)];
            // There are 128 possible source pitches on each MIDI channel.
            // unsigned char can represent 128, so never saturate early: doing
            // so would release the mapped note while one source key remained held.
            if (count < 128)
                ++count;
        }

        int noteOff(int channel, int sourceNote) noexcept
        {
            if (!valid(channel, sourceNote))
                return sourceNote;

            auto& mapped = sourceToMapped[static_cast<std::size_t>(channel)][static_cast<std::size_t>(sourceNote)];
            if (mapped < 0)
                return sourceNote;

            const int result = mapped;
            decrement(mappedRefCounts[static_cast<std::size_t>(channel)][static_cast<std::size_t>(mapped)]);
            mapped = -1;
            return result;
        }

        bool isHeld(int channel, int mappedNote) const noexcept
        {
            return valid(channel, mappedNote)
                && mappedRefCounts[static_cast<std::size_t>(channel)][static_cast<std::size_t>(mappedNote)] > 0;
        }

    private:
        static constexpr bool valid(int channel, int note) noexcept
        {
            return channel >= 0 && channel < 16 && note >= 0 && note < 128;
        }

        static void decrement(unsigned char& value) noexcept
        {
            if (value > 0)
                --value;
        }

        std::array<std::array<int, 128>, 16> sourceToMapped = [] {
            std::array<std::array<int, 128>, 16> values {};
            for (auto& channel : values) channel.fill(-1);
            return values;
        }();
        std::array<std::array<unsigned char, 128>, 16> mappedRefCounts {};
    };
}
