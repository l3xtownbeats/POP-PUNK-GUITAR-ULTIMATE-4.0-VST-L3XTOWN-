#include "AutoChords.h"
#include <iostream>

using namespace ppgu::performance;

int main()
{
    const auto major = AutoChords::generate(60, ChordQuality::Major, 0);
    if (major.count != 3 || major.midi[0] != 60 || major.midi[1] != 64 || major.midi[2] != 67)
        return 1;

    const auto minor7 = AutoChords::generate(57, ChordQuality::Minor7, 0);
    if (minor7.count != 4 || minor7.midi[0] != 57 || minor7.midi[1] != 60
        || minor7.midi[2] != 64 || minor7.midi[3] != 67)
        return 2;

    const auto inversion = AutoChords::generate(60, ChordQuality::Major, 0, 1);
    if (inversion.count != 3 || inversion.midi[0] != 64
        || inversion.midi[1] != 67 || inversion.midi[2] != 72)
        return 3;

    const auto cMajorShape = FingeringGenerator::generate(major, 12);
    if (!cMajorShape.complete || cMajorShape.count != 3)
        return 4;
    for (std::size_t i = 1; i < cMajorShape.count; ++i)
        if (cMajorShape.positions[i].stringIndex <= cMajorShape.positions[i - 1].stringIndex)
            return 5;

    const auto impossible = FingeringGenerator::generate(major, 0);
    if (impossible.count > 3 || impossible.complete != (impossible.count == major.count))
        return 6;

    const auto bounded = AutoChords::generate(120, ChordQuality::Dominant7, 1);
    if (bounded.count > 4)
        return 7;

    std::cout << "M4 AutoChords/FingeringGenerator tests passed\n";
    return 0;
}
