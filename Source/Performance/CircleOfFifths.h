#pragma once

#include <array>
#include <cstdint>

namespace ppgu::performance
{
struct CircleKey
{
    std::uint8_t majorPitchClass;
    std::uint8_t relativeMinorPitchClass;
    const char* majorName;
    const char* relativeMinorName;
};

// Clockwise order follows ascending perfect fifths. The relative minor is
// the sixth degree of each major key. This is immutable, allocation-free data.
class CircleOfFifths final
{
public:
    static constexpr std::array<CircleKey, 12> keys {{
        { 0, 9,  "C",  "Am"  },
        { 7, 4,  "G",  "Em"  },
        { 2, 11, "D",  "Bm"  },
        { 9, 6,  "A",  "F#m" },
        { 4, 1,  "E",  "C#m" },
        { 11, 8, "B",  "G#m" },
        { 6, 3,  "F#", "D#m" },
        { 1, 10, "Db", "Bbm" },
        { 8, 5,  "Ab", "Fm"  },
        { 3, 0,  "Eb", "Cm"  },
        { 10, 7, "Bb", "Gm"  },
        { 5, 2,  "F",  "Dm"  }
    }};

    static constexpr const CircleKey& at(int index) noexcept
    {
        return keys[wrap(index)];
    }

    static constexpr int indexForMajorPitchClass(int pitchClass) noexcept
    {
        const int pc = wrap(pitchClass);
        for (int i = 0; i < 12; ++i)
            if (keys[static_cast<std::size_t>(i)].majorPitchClass == pc)
                return i;
        return -1;
    }

    static constexpr int indexForRelativeMinorPitchClass(int pitchClass) noexcept
    {
        const int pc = wrap(pitchClass);
        for (int i = 0; i < 12; ++i)
            if (keys[static_cast<std::size_t>(i)].relativeMinorPitchClass == pc)
                return i;
        return -1;
    }

    static constexpr int nextFifth(int pitchClass) noexcept
    {
        return wrap(pitchClass + 7);
    }

    static constexpr int nextFourth(int pitchClass) noexcept
    {
        return wrap(pitchClass + 5);
    }

    static constexpr int relativeMinorOfMajor(int majorPitchClass) noexcept
    {
        const int index = indexForMajorPitchClass(majorPitchClass);
        return index < 0 ? -1 : keys[static_cast<std::size_t>(index)].relativeMinorPitchClass;
    }

private:
    static constexpr int wrap(int value) noexcept
    {
        return (value % 12 + 12) % 12;
    }
};
} // namespace ppgu::performance
