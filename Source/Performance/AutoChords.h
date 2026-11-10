#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace ppgu::performance
{
enum class ChordQuality : std::uint8_t
{
    Major, Minor, Diminished, Augmented, Sus2, Sus4,
    Dominant7, Major7, Minor7, Power
};

struct ChordDefinition
{
    std::array<int, 4> intervals {};
    std::size_t count = 0;
};

struct ChordNotes
{
    std::array<int, 6> midi {};
    std::size_t count = 0;
};

class AutoChords final
{
public:
    static ChordDefinition definition(ChordQuality quality) noexcept
    {
        switch (quality)
        {
            case ChordQuality::Minor:      return {{{ 0, 3, 7, 0 }}, 3};
            case ChordQuality::Diminished: return {{{ 0, 3, 6, 0 }}, 3};
            case ChordQuality::Augmented:  return {{{ 0, 4, 8, 0 }}, 3};
            case ChordQuality::Sus2:       return {{{ 0, 2, 7, 0 }}, 3};
            case ChordQuality::Sus4:       return {{{ 0, 5, 7, 0 }}, 3};
            case ChordQuality::Dominant7:  return {{{ 0, 4, 7, 10 }}, 4};
            case ChordQuality::Major7:     return {{{ 0, 4, 7, 11 }}, 4};
            case ChordQuality::Minor7:     return {{{ 0, 3, 7, 10 }}, 4};
            case ChordQuality::Power:      return {{{ 0, 7, 12, 0 }}, 3};
            case ChordQuality::Major:
            default:                      return {{{ 0, 4, 7, 0 }}, 3};
        }
    }

    // Generates an ascending, bounded MIDI voicing. Inversion rotates chord
    // tones without allocation; octave keeps the root in a practical guitar range.
    static ChordNotes generate(int rootMidi, ChordQuality quality,
                               int octave, int inversion = 0) noexcept
    {
        ChordNotes result;
        const auto chord = definition(quality);
        if (chord.count == 0)
            return result;

        const int root = clamp(rootMidi + (octave * 12), 0, 108);
        const int rotation = ((inversion % static_cast<int>(chord.count))
                              + static_cast<int>(chord.count)) % static_cast<int>(chord.count);
        int previous = -1;
        for (std::size_t i = 0; i < chord.count; ++i)
        {
            const auto tone = static_cast<std::size_t>((static_cast<int>(i) + rotation) % static_cast<int>(chord.count));
            int note = root + chord.intervals[tone];
            while (note <= previous)
                note += 12;
            if (note > 127)
                break;
            result.midi[result.count++] = note;
            previous = note;
        }
        return result;
    }

private:
    static int clamp(int value, int low, int high) noexcept
    {
        return (std::max)(low, (std::min)(high, value));
    }
};

enum class ScaleMode : std::uint8_t
{
    Major, NaturalMinor
};

struct ScaleChord
{
    int rootMidi = -1;
    int degree = -1;
    ChordQuality quality = ChordQuality::Major;
    bool inputWasInScale = false;
    bool valid = false;
};

class AutoChordMap final
{
public:
    // Chromatic MIDI keys are mapped to the nearest scale tone. Equal-distance
    // ties resolve downward, making the mapping deterministic and predictable.
    static ScaleChord mapScaleAware(int midiNote, int tonicPitchClass,
                                    ScaleMode mode) noexcept
    {
        ScaleChord result;
        if (midiNote < 0 || midiNote > 127)
            return result;

        const int tonic = ((tonicPitchClass % 12) + 12) % 12;
        constexpr std::array<int, 7> majorScale { 0, 2, 4, 5, 7, 9, 11 };
        constexpr std::array<int, 7> minorScale { 0, 2, 3, 5, 7, 8, 10 };
        constexpr std::array<ChordQuality, 7> majorChords {
            ChordQuality::Major, ChordQuality::Minor, ChordQuality::Minor,
            ChordQuality::Major, ChordQuality::Major, ChordQuality::Minor,
            ChordQuality::Diminished
        };
        constexpr std::array<ChordQuality, 7> minorChords {
            ChordQuality::Minor, ChordQuality::Diminished, ChordQuality::Major,
            ChordQuality::Minor, ChordQuality::Minor, ChordQuality::Major,
            ChordQuality::Major
        };

        const int pitchClass = midiNote % 12;
        const int relative = (pitchClass - tonic + 12) % 12;
        int bestDegree = 0;
        int bestSignedDistance = 0;
        int bestDistance = 13;
        bool exact = false;

        for (int degree = 0; degree < 7; ++degree)
        {
            const int scaleTone = mode == ScaleMode::Major
                ? majorScale[static_cast<std::size_t>(degree)]
                : minorScale[static_cast<std::size_t>(degree)];
            int signedDistance = scaleTone - relative;
            while (signedDistance > 6) signedDistance -= 12;
            while (signedDistance < -6) signedDistance += 12;
            const int distance = std::abs(signedDistance);
            if (distance < bestDistance || (distance == bestDistance && signedDistance < bestSignedDistance))
            {
                bestDistance = distance;
                bestDegree = degree;
                bestSignedDistance = signedDistance;
                exact = distance == 0;
            }
        }

        const int mappedRoot = midiNote + bestSignedDistance;
        if (mappedRoot < 0 || mappedRoot > 127)
            return result;

        result.rootMidi = mappedRoot;
        result.degree = bestDegree;
        result.quality = mode == ScaleMode::Major
            ? majorChords[static_cast<std::size_t>(bestDegree)]
            : minorChords[static_cast<std::size_t>(bestDegree)];
        result.inputWasInScale = exact;
        result.valid = true;
        return result;
    }

    static ChordNotes generateScaleChord(int midiNote, int tonicPitchClass,
                                         ScaleMode mode, int inversion = 0) noexcept
    {
        const auto mapped = mapScaleAware(midiNote, tonicPitchClass, mode);
        if (!mapped.valid)
            return {};
        return AutoChords::generate(mapped.rootMidi, mapped.quality, 0, inversion);
    }
};

struct GuitarPosition
{
    int midiNote = -1;
    int stringIndex = -1; // 0 = low E, 5 = high E
    int fret = -1;        // -1 means muted/unassigned
};

struct GuitarFingering
{
    std::array<GuitarPosition, 6> positions {};
    std::size_t count = 0;
    int lowestFret = 0;
    int highestFret = 0;
    bool complete = false;
};

class FingeringGenerator final
{
public:
    static constexpr std::array<int, 6> standardTuning { 40, 45, 50, 55, 59, 64 };

    // Notes must be ascending. Greedy bounded search assigns each note to a
    // successively higher string, preferring low positions and compact stretches.
    static GuitarFingering generate(const ChordNotes& notes,
                                    int maximumFret = 12,
                                    int previousPosition = 0) noexcept
    {
        GuitarFingering result;
        maximumFret = (std::max)(0, (std::min)(maximumFret, 24));
        previousPosition = (std::max)(0, (std::min)(previousPosition, maximumFret));
        int nextString = 0;
        int minFret = maximumFret;
        int maxFret = 0;

        for (std::size_t n = 0; n < notes.count && n < notes.midi.size(); ++n)
        {
            const int note = notes.midi[n];
            int bestString = -1;
            int bestFret = -1;
            int bestScore = 0x7fffffff;
            const int remainingNotes = static_cast<int>(notes.count - n);
            const int lastAllowedString = static_cast<int>(standardTuning.size()) - remainingNotes;
            for (int stringIndex = nextString; stringIndex <= lastAllowedString; ++stringIndex)
            {
                const int fret = note - standardTuning[static_cast<std::size_t>(stringIndex)];
                if (fret < 0 || fret > maximumFret)
                    continue;
                const int score = fret * 4 + std::abs(fret - previousPosition)
                                  + stringIndex;
                if (score < bestScore)
                {
                    bestScore = score;
                    bestString = stringIndex;
                    bestFret = fret;
                }
            }
            if (bestString < 0)
                break;

            result.positions[result.count++] = { note, bestString, bestFret };
            nextString = bestString + 1;
            minFret = (std::min)(minFret, bestFret);
            maxFret = (std::max)(maxFret, bestFret);
            previousPosition = bestFret;
        }

        result.lowestFret = result.count == 0 ? 0 : minFret;
        result.highestFret = result.count == 0 ? 0 : maxFret;
        result.complete = result.count == notes.count;
        return result;
    }
};
}
