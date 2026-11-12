#pragma once

#include "AutoChordEngine.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace ppgu::performance
{
enum class HarmonyMode : std::uint8_t { Major, NaturalMinor };

struct ProgressionChord
{
    int rootPitchClass = -1;
    ChordQuality quality = ChordQuality::Unknown;
    int scaleDegree = 0; // 1..7
    std::uint16_t pitchClassMask = 0;
    std::string_view romanNumeral {};
    bool valid() const noexcept { return rootPitchClass >= 0 && scaleDegree >= 1 && scaleDegree <= 7; }
};

struct ProgressionResult
{
    std::size_t count = 0;
    bool valid = false;
};

// Bounded, deterministic theory model. This is intentionally independent of JUCE,
// the sampler, MIDI routing and the audio callback. Call it on demand from a
// control/message thread; it performs no heap allocation.
class HarmonyProgressionEngine final
{
public:
    static constexpr std::size_t maximumProgressionChords = 8;
    static constexpr int progressionTemplateCount = 8;

    static constexpr std::array<int, 12> circleOfFifthsMajorKeys {{
        0, 7, 2, 9, 4, 11, 6, 1, 8, 3, 10, 5
    }}; // C, G, D, A, E, B, F#, C#, G#, D#, A#, F

    static constexpr int relativeMinorRoot(int majorRootPitchClass) noexcept
    {
        return majorRootPitchClass >= 0 && majorRootPitchClass < 12
            ? (majorRootPitchClass + 9) % 12 : -1;
    }

    static constexpr int circleKeyAt(int index) noexcept
    {
        return index >= 0 && index < static_cast<int>(circleOfFifthsMajorKeys.size())
            ? circleOfFifthsMajorKeys[static_cast<std::size_t>(index)] : -1;
    }

    static constexpr std::string_view noteName(int pitchClass) noexcept
    {
        constexpr std::array<std::string_view, 12> names {{
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
        }};
        return pitchClass >= 0 && pitchClass < 12
            ? names[static_cast<std::size_t>(pitchClass)] : std::string_view {};
    }

    // templateId is 1..8. Invalid input leaves output untouched.
    static ProgressionResult generate(int keyRootPitchClass, HarmonyMode mode,
                                      int templateId, ProgressionChord* output,
                                      std::size_t capacity) noexcept
    {
        if (output == nullptr || keyRootPitchClass < 0 || keyRootPitchClass > 11
            || templateId < 1 || templateId > progressionTemplateCount
            || capacity < 4)
            return {};

        constexpr std::array<std::array<int, 4>, 8> majorTemplates {{
            {{ 1, 5, 6, 4 }}, {{ 1, 6, 4, 5 }}, {{ 6, 4, 1, 5 }}, {{ 1, 4, 5, 4 }},
            {{ 1, 5, 4, 5 }}, {{ 2, 5, 1, 4 }}, {{ 1, 3, 4, 5 }}, {{ 6, 5, 4, 5 }}
        }};
        constexpr std::array<std::array<int, 4>, 8> minorTemplates {{
            {{ 1, 6, 3, 7 }}, {{ 1, 7, 6, 7 }}, {{ 1, 4, 6, 7 }}, {{ 1, 6, 4, 7 }},
            {{ 1, 3, 6, 4 }}, {{ 1, 5, 6, 7 }}, {{ 6, 3, 7, 1 }}, {{ 1, 4, 1, 7 }}
        }};
        constexpr std::array<int, 7> majorScale {{ 0, 2, 4, 5, 7, 9, 11 }};
        constexpr std::array<int, 7> minorScale {{ 0, 2, 3, 5, 7, 8, 10 }};
        constexpr std::array<int, 7> majorThirds {{ 4, 3, 3, 4, 4, 3, 3 }};
        constexpr std::array<int, 7> minorThirds {{ 3, 3, 4, 3, 3, 4, 4 }};
        constexpr std::array<int, 7> majorFifths {{ 7, 7, 7, 7, 7, 7, 6 }};
        constexpr std::array<int, 7> minorFifths {{ 7, 6, 7, 7, 7, 7, 7 }};
        constexpr std::array<std::string_view, 7> majorRoman {{
            "I", "ii", "iii", "IV", "V", "vi", "vii°"
        }};
        constexpr std::array<std::string_view, 7> minorRoman {{
            "i", "ii°", "III", "iv", "v", "VI", "VII"
        }};

        const auto templateIndex = static_cast<std::size_t>(templateId - 1);
        const auto& degrees = mode == HarmonyMode::Major
            ? majorTemplates[templateIndex] : minorTemplates[templateIndex];
        const auto& scale = mode == HarmonyMode::Major ? majorScale : minorScale;
        const auto& thirds = mode == HarmonyMode::Major ? majorThirds : minorThirds;
        const auto& fifths = mode == HarmonyMode::Major ? majorFifths : minorFifths;
        const auto& roman = mode == HarmonyMode::Major ? majorRoman : minorRoman;

        for (std::size_t i = 0; i < degrees.size(); ++i)
        {
            const int degreeIndex = degrees[i] - 1;
            const int root = (keyRootPitchClass + scale[static_cast<std::size_t>(degreeIndex)]) % 12;
            const int third = thirds[static_cast<std::size_t>(degreeIndex)];
            const int fifth = fifths[static_cast<std::size_t>(degreeIndex)];
            const auto mask = static_cast<std::uint16_t>(
                (1u << root) | (1u << ((root + third) % 12)) | (1u << ((root + fifth) % 12)));
            const auto quality = third == 4 ? ChordQuality::Major
                : (fifth == 6 ? ChordQuality::Diminished : ChordQuality::Minor);
            output[i] = { root, quality, degrees[i], mask,
                          roman[static_cast<std::size_t>(degreeIndex)] };
        }
        return { degrees.size(), true };
    }
};
} // namespace ppgu::performance
