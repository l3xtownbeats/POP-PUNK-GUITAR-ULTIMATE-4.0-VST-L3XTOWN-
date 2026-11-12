#include "HarmonyProgressionEngine.h"
#include <array>
#include <iostream>

using namespace ppgu::performance;

static bool isChord(const ProgressionChord& chord, int root, ChordQuality quality,
                    int degree, std::string_view roman)
{
    return chord.valid() && chord.rootPitchClass == root && chord.quality == quality
        && chord.scaleDegree == degree && chord.romanNumeral == roman;
}

int main()
{
    std::array<ProgressionChord, 8> chords {};
    auto result = HarmonyProgressionEngine::generate(0, HarmonyMode::Major, 1,
                                                       chords.data(), chords.size());
    if (!result.valid || result.count != 4) return 1;
    if (!isChord(chords[0], 0, ChordQuality::Major, 1, "I")) return 2;
    if (!isChord(chords[1], 7, ChordQuality::Major, 5, "V")) return 3;
    if (!isChord(chords[2], 9, ChordQuality::Minor, 6, "vi")) return 4;
    if (!isChord(chords[3], 5, ChordQuality::Major, 4, "IV")) return 5;

    result = HarmonyProgressionEngine::generate(9, HarmonyMode::NaturalMinor, 1,
                                                 chords.data(), chords.size());
    if (!result.valid || result.count != 4) return 6;
    if (!isChord(chords[0], 9, ChordQuality::Minor, 1, "i")) return 7;
    if (!isChord(chords[1], 5, ChordQuality::Major, 6, "VI")) return 8;
    if (!isChord(chords[2], 0, ChordQuality::Major, 3, "III")) return 9;
    if (!isChord(chords[3], 7, ChordQuality::Major, 7, "VII")) return 10;

    if (HarmonyProgressionEngine::relativeMinorRoot(0) != 9
        || HarmonyProgressionEngine::relativeMinorRoot(7) != 4
        || HarmonyProgressionEngine::relativeMinorRoot(-1) != -1) return 11;
    constexpr std::array<int, 12> expectedCircle {{ 0,7,2,9,4,11,6,1,8,3,10,5 }};
    for (int i = 0; i < 12; ++i)
        if (HarmonyProgressionEngine::circleKeyAt(i) != expectedCircle[static_cast<std::size_t>(i)]) return 12;
    if (HarmonyProgressionEngine::circleKeyAt(-1) != -1
        || HarmonyProgressionEngine::circleKeyAt(12) != -1) return 13;
    if (HarmonyProgressionEngine::noteName(0) != "C"
        || HarmonyProgressionEngine::noteName(11) != "B"
        || !HarmonyProgressionEngine::noteName(12).empty()) return 14;

    const auto untouched = chords;
    if (HarmonyProgressionEngine::generate(12, HarmonyMode::Major, 1,
                                            chords.data(), chords.size()).valid) return 15;
    if (HarmonyProgressionEngine::generate(0, HarmonyMode::Major, 0,
                                            chords.data(), chords.size()).valid) return 16;
    if (HarmonyProgressionEngine::generate(0, HarmonyMode::Major, 1,
                                            chords.data(), 3).valid) return 17;
    if (HarmonyProgressionEngine::generate(0, HarmonyMode::Major, 1,
                                            nullptr, chords.size()).valid) return 18;
    for (std::size_t i = 0; i < chords.size(); ++i)
        if (chords[i].rootPitchClass != untouched[i].rootPitchClass
            || chords[i].scaleDegree != untouched[i].scaleDegree) return 19;

    std::cout << "Harmony progression engine tests passed\n";
    return 0;
}
