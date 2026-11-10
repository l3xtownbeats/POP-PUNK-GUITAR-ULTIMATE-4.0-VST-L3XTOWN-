#include "AutoChordEngine.h"
#include <array>
#include <iostream>

using namespace ppgu::performance;

int main()
{
    const std::array<int, 3> cMajor {{ 60, 64, 67 }};
    auto c = AutoChordEngine::detect(cMajor.data(), cMajor.size());
    if (!c.valid() || c.rootPitchClass != 0 || c.quality != ChordQuality::Major) return 1;

    const std::array<int, 4> aMinor7 {{ 57, 60, 64, 67 }};
    auto am7 = AutoChordEngine::detect(aMinor7.data(), aMinor7.size());
    if (!am7.valid() || am7.rootPitchClass != 9 || am7.quality != ChordQuality::Minor7) return 2;

    const std::array<int, 2> gPower {{ 55, 62 }};
    auto power = AutoChordEngine::detect(gPower.data(), gPower.size());
    if (!power.valid() || power.rootPitchClass != 7 || power.quality != ChordQuality::Power) return 3;

    const std::array<int, 3> ambiguous {{ 60, 61, 67 }};
    if (AutoChordEngine::detect(ambiguous.data(), ambiguous.size()).valid()) return 4;
    if (AutoChordEngine::detect(nullptr, 0).valid()) return 5;

    auto voicing = FingeringGenerator::generate(c);
    if (!voicing.valid || voicing.coveredChordTones < 2
        || voicing.highestFret - voicing.lowestFret > 17) return 6;
    for (int i = 0; i < 6; ++i)
        if (voicing.midiNote[i] >= 0
            && (voicing.midiNote[i] < 40 || voicing.midiNote[i] > 84
                || voicing.midiNote[i] % 12 < 0)) return 7;

    if (FingeringGenerator::generate({}, 40, 84).valid) return 8;
    std::cout << "AutoChord and fingering tests passed\n";
    return 0;
}
