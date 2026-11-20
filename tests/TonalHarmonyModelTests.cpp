#include "TonalHarmonyModel.h"
#include <array>
#include <iostream>

using namespace ppgu::performance;

int main()
{
    // AutoChords ON: every degree in a selected key is exposed.
    constexpr auto cMajor = TonalHarmonyModel::scalePitchClasses(0, ScaleMode::Major);
    constexpr std::array<int, 7> expectedMajor {{ 0, 2, 4, 5, 7, 9, 11 }};
    if (cMajor != expectedMajor) return 1;

    constexpr auto aMinor = TonalHarmonyModel::scalePitchClasses(9, ScaleMode::NaturalMinor);
    constexpr std::array<int, 7> expectedMinor {{ 9, 11, 0, 2, 4, 5, 7 }};
    if (aMinor != expectedMinor) return 2;

    // Each scale degree has its correct diatonic triad quality.
    constexpr std::array<ChordKind, 7> majorQualities {{
        ChordKind::Major, ChordKind::Minor, ChordKind::Minor,
        ChordKind::Major, ChordKind::Major, ChordKind::Minor,
        ChordKind::Diminished
    }};
    for (int degree = 0; degree < 7; ++degree)
        if (TonalHarmonyModel::diatonicChord(ScaleMode::Major, degree).kind
            != majorQualities[static_cast<std::size_t>(degree)]) return 3;

    // Users can form chords, not merely select preset names.
    const auto cMaj7 = TonalHarmonyModel::buildChord(60, ChordKind::Major7);
    constexpr std::array<int, 4> expectedMaj7 {{ 60, 64, 67, 71 }};
    if (cMaj7.noteCount != 4 || cMaj7.midiNotes != expectedMaj7) return 4;

    const auto dPower = TonalHarmonyModel::buildChord(62, ChordKind::Power);
    if (dPower.noteCount != 3 || dPower.midiNotes[0] != 62
        || dPower.midiNotes[1] != 69 || dPower.midiNotes[2] != 74) return 5;

    // AutoChords OFF: all twelve chromatic choices and enharmonic spellings.
    for (int pc = 0; pc < 12; ++pc)
        if (TonalHarmonyModel::chromaticNote(pc).pitchClass != pc) return 6;
    if (TonalHarmonyModel::noteName(1, true) != "Db"
        || TonalHarmonyModel::noteName(1, false) != "C#"
        || TonalHarmonyModel::noteName(10, true) != "Bb") return 7;

    // MIDI remains chromatic and unrestricted by AutoChords state.
    for (int note = 0; note <= 127; ++note)
        if (!TonalHarmonyModel::acceptsMidiNote(note, false)
            || !TonalHarmonyModel::acceptsMidiNote(note, true)) return 8;
    if (TonalHarmonyModel::acceptsMidiNote(-1, false)
        || TonalHarmonyModel::acceptsMidiNote(128, false)) return 9;

    // Invalid/out-of-range generated chords are rejected safely.
    if (TonalHarmonyModel::buildChord(-1, ChordKind::Major).noteCount != 0
        || TonalHarmonyModel::buildChord(125, ChordKind::Major7).noteCount != 0) return 10;

    std::cout << "Tonal harmony contract tests passed\n";
    return 0;
}
