#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <limits>

namespace ppgu::performance
{
enum class ChordQuality : std::uint8_t
{
    Unknown, Major, Minor, Diminished, Augmented, Sus2, Sus4,
    Dominant7, Major7, Minor7, Power
};

struct ChordDescriptor
{
    int rootPitchClass = -1;
    ChordQuality quality = ChordQuality::Unknown;
    std::uint16_t pitchClassMask = 0;
    int inputNoteCount = 0;
    bool valid() const noexcept { return rootPitchClass >= 0 && quality != ChordQuality::Unknown; }
};

class AutoChordEngine final
{
public:
    // Input is MIDI note numbers. Duplicate octaves are collapsed; invalid notes
    // are ignored. Exact pitch-class templates avoid guessing extensions.
    static ChordDescriptor detect(const int* notes, std::size_t count) noexcept
    {
        std::uint16_t mask = 0;
        int validCount = 0;
        if (notes == nullptr) return {};
        for (std::size_t i = 0; i < count; ++i)
        {
            if (notes[i] < 0 || notes[i] > 127) continue;
            mask |= static_cast<std::uint16_t>(1u << (notes[i] % 12));
            ++validCount;
        }
        if (validCount == 0) return {};
        const int distinct = popcount(mask);
        if (distinct < 2) return {};
        struct Template { ChordQuality quality; std::uint16_t intervals; int minimum; };
        constexpr std::array<Template, 10> templates {{
            { ChordQuality::Major,      0b000010010001, 3 }, // 0,4,7
            { ChordQuality::Minor,      0b000010001001, 3 }, // 0,3,7
            { ChordQuality::Diminished,  0b000001001001, 3 }, // 0,3,6
            { ChordQuality::Augmented,   0b000100010001, 3 }, // 0,4,8
            { ChordQuality::Sus2,        0b000010000101, 3 }, // 0,2,7
            { ChordQuality::Sus4,        0b000010100001, 3 }, // 0,5,7
            { ChordQuality::Dominant7,   0b010010010001, 4 }, // 0,4,7,10
            { ChordQuality::Major7,      0b100010010001, 4 }, // 0,4,7,11
            { ChordQuality::Minor7,      0b010010001001, 4 }, // 0,3,7,10
            { ChordQuality::Power,       0b000010000001, 2 } // 0,7
        }};
        for (int root = 0; root < 12; ++root)
        {
            const auto rotated = rotateToRoot(mask, root);
            for (const auto& t : templates)
            {
                if (distinct >= t.minimum && rotated == t.intervals)
                    return { root, t.quality, mask, validCount };
            }
        }
        return {};
    }

private:
    static int popcount(std::uint16_t value) noexcept
    {
        int n = 0;
        while (value) { value &= static_cast<std::uint16_t>(value - 1); ++n; }
        return n;
    }
    static std::uint16_t rotateToRoot(std::uint16_t mask, int root) noexcept
    {
        std::uint16_t result = 0;
        for (int pc = 0; pc < 12; ++pc)
            if ((mask & (1u << pc)) != 0)
                result |= static_cast<std::uint16_t>(1u << ((pc - root + 12) % 12));
        return result;
    }
};

struct GuitarVoicing
{
    std::array<int, 6> fret {{ -1, -1, -1, -1, -1, -1 }};
    std::array<int, 6> midiNote {{ -1, -1, -1, -1, -1, -1 }};
    int lowestFret = 0;
    int highestFret = 0;
    int coveredChordTones = 0;
    bool valid = false;
};

class FingeringGenerator final
{
public:
    // Standard tuning low-to-high: E2 A2 D3 G3 B3 E4. Search is bounded:
    // 13 anchor positions x 6 strings x 19 frets, suitable for control/message
    // thread use; no heap allocation and no audio-thread dependency.
    static GuitarVoicing generate(const ChordDescriptor& chord, int minimumMidi = 40,
                                  int maximumMidi = 84, int maxFret = 17) noexcept
    {
        GuitarVoicing best;
        if (!chord.valid() || minimumMidi < 0 || maximumMidi > 127
            || minimumMidi > maximumMidi || maxFret < 0 || maxFret > 24)
            return best;
        constexpr std::array<int, 6> open {{ 40, 45, 50, 55, 59, 64 }};
        int bestScore = std::numeric_limits<int>::max();
        for (int anchor = 0; anchor <= (std::min)(12, maxFret); ++anchor)
        {
            GuitarVoicing candidate;
            int minF = maxFret + 1, maxF = 0;
            std::uint16_t tones = 0;
            int totalFret = 0;
            for (int string = 0; string < 6; ++string)
            {
                int selected = -1;
                int selectedDistance = 1000;
                for (int fret = 0; fret <= maxFret; ++fret)
                {
                    const int note = open[string] + fret;
                    if (note < minimumMidi || note > maximumMidi) continue;
                    const int pc = note % 12;
                    if ((chord.pitchClassMask & (1u << pc)) == 0) continue;
                    const int distance = (std::abs)(fret - anchor);
                    if (distance < selectedDistance || (distance == selectedDistance && fret < selected))
                    {
                        selected = fret;
                        selectedDistance = distance;
                    }
                }
                if (selected >= 0)
                {
                    const int note = open[string] + selected;
                    candidate.fret[string] = selected;
                    candidate.midiNote[string] = note;
                    minF = (std::min)(minF, selected);
                    maxF = (std::max)(maxF, selected);
                    totalFret += selected;
                    tones |= static_cast<std::uint16_t>(1u << (note % 12));
                }
            }
            const int coverage = popcount(static_cast<std::uint16_t>(tones & chord.pitchClassMask));
            if (coverage < 2) continue;
            candidate.lowestFret = minF;
            candidate.highestFret = maxF;
            candidate.coveredChordTones = coverage;
            candidate.valid = true;
            const int score = (maxF - minF) * 100 + totalFret * 2 - coverage * 30
                            + (minF > 0 ? minF * 3 : 0);
            if (score < bestScore) { bestScore = score; best = candidate; }
        }
        return best;
    }

private:
    static int popcount(std::uint16_t value) noexcept
    {
        int n = 0;
        while (value) { value &= static_cast<std::uint16_t>(value - 1); ++n; }
        return n;
    }
};
} // namespace ppgu::performance
