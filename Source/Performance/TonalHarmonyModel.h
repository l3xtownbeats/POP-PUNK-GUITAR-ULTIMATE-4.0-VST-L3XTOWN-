#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace ppgu::performance
{
enum class ScaleMode : std::uint8_t { Major, NaturalMinor };
enum class ChordKind : std::uint8_t
{
    Major, Minor, Diminished, Augmented, Sus2, Sus4,
    Dominant7, Major7, Minor7, Power
};

struct NoteChoice
{
    int pitchClass = 0;
    const char* sharpName = "C";
    const char* flatName = "C";
};

struct ChordShape
{
    ChordKind kind = ChordKind::Major;
    std::array<int, 4> intervals {{ 0, 4, 7, -1 }};
    int noteCount = 3;
};

struct BuiltChord
{
    std::array<int, 4> midiNotes {{ -1, -1, -1, -1 }};
    int noteCount = 0;
};

class TonalHarmonyModel final
{
public:
    static constexpr std::array<NoteChoice, 12> chromaticNotes {{
        {0,"C","C"}, {1,"C#","Db"}, {2,"D","D"}, {3,"D#","Eb"},
        {4,"E","E"}, {5,"F","F"}, {6,"F#","Gb"}, {7,"G","G"},
        {8,"G#","Ab"}, {9,"A","A"}, {10,"A#","Bb"}, {11,"B","B"}
    }};

    // The 12-note selector is always available. AutoChords OFF never filters
    // incoming MIDI notes; key/scale helpers are optional composition tools.
    static constexpr const NoteChoice& chromaticNote(int index) noexcept
    {
        return chromaticNotes[static_cast<std::size_t>((index % 12 + 12) % 12)];
    }

    static constexpr const char* noteName(int pitchClass, bool preferFlats) noexcept
    {
        const auto& note = chromaticNote(pitchClass);
        return preferFlats ? note.flatName : note.sharpName;
    }

    static constexpr bool acceptsMidiNote(int midiNote, bool /*autoChordsEnabled*/) noexcept
    {
        return midiNote >= 0 && midiNote <= 127;
    }

    static constexpr int scalePitchClass(int rootPitchClass, ScaleMode mode,
                                         int degree) noexcept
    {
        constexpr std::array<int, 7> major {{ 0, 2, 4, 5, 7, 9, 11 }};
        constexpr std::array<int, 7> minor {{ 0, 2, 3, 5, 7, 8, 10 }};
        const int root = (rootPitchClass % 12 + 12) % 12;
        const int step = (degree % 7 + 7) % 7;
        return (root + (mode == ScaleMode::Major ? major[step] : minor[step])) % 12;
    }

    static constexpr std::array<int, 7> scalePitchClasses(int rootPitchClass,
                                                           ScaleMode mode) noexcept
    {
        std::array<int, 7> result {};
        for (int degree = 0; degree < 7; ++degree)
            result[static_cast<std::size_t>(degree)] =
                scalePitchClass(rootPitchClass, mode, degree);
        return result;
    }

    static constexpr ChordShape diatonicChord(ScaleMode mode, int degree) noexcept
    {
        constexpr std::array<ChordKind, 7> majorKinds {{
            ChordKind::Major, ChordKind::Minor, ChordKind::Minor,
            ChordKind::Major, ChordKind::Major, ChordKind::Minor,
            ChordKind::Diminished
        }};
        constexpr std::array<ChordKind, 7> minorKinds {{
            ChordKind::Minor, ChordKind::Diminished, ChordKind::Major,
            ChordKind::Minor, ChordKind::Minor, ChordKind::Major,
            ChordKind::Major
        }};
        const auto d = static_cast<std::size_t>((degree % 7 + 7) % 7);
        const auto kind = mode == ScaleMode::Major ? majorKinds[d] : minorKinds[d];
        return shape(kind);
    }

    static constexpr ChordShape shape(ChordKind kind) noexcept
    {
        switch (kind)
        {
            case ChordKind::Major:      return { kind, {{0,4,7,-1}}, 3 };
            case ChordKind::Minor:      return { kind, {{0,3,7,-1}}, 3 };
            case ChordKind::Diminished: return { kind, {{0,3,6,-1}}, 3 };
            case ChordKind::Augmented:  return { kind, {{0,4,8,-1}}, 3 };
            case ChordKind::Sus2:       return { kind, {{0,2,7,-1}}, 3 };
            case ChordKind::Sus4:       return { kind, {{0,5,7,-1}}, 3 };
            case ChordKind::Dominant7:  return { kind, {{0,4,7,10}}, 4 };
            case ChordKind::Major7:     return { kind, {{0,4,7,11}}, 4 };
            case ChordKind::Minor7:     return { kind, {{0,3,7,10}}, 4 };
            case ChordKind::Power:      return { kind, {{0,7,12,-1}}, 3 };
        }
        return {};
    }

    static constexpr BuiltChord buildChord(int rootMidi, ChordKind kind) noexcept
    {
        BuiltChord result;
        if (rootMidi < 0 || rootMidi > 127) return result;
        const auto chord = shape(kind);
        for (int i = 0; i < chord.noteCount; ++i)
        {
            const int note = rootMidi + chord.intervals[static_cast<std::size_t>(i)];
            if (note > 127) return {};
            result.midiNotes[static_cast<std::size_t>(i)] = note;
        }
        result.noteCount = chord.noteCount;
        return result;
    }
};
} // namespace ppgu::performance
