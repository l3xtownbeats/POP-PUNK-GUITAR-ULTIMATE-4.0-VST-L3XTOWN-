#include "CircleOfFifths.h"
#include <array>
#include <iostream>

using ppgu::performance::CircleOfFifths;

int main()
{
    std::array<bool, 12> seenMajor {};
    std::array<bool, 12> seenMinor {};
    for (int i = 0; i < 12; ++i)
    {
        const auto& key = CircleOfFifths::at(i);
        if (seenMajor[key.majorPitchClass] || seenMinor[key.relativeMinorPitchClass]) return 1;
        seenMajor[key.majorPitchClass] = true;
        seenMinor[key.relativeMinorPitchClass] = true;
        if (CircleOfFifths::nextFifth(key.majorPitchClass)
            != CircleOfFifths::at(i + 1).majorPitchClass) return 2;
        if (CircleOfFifths::relativeMinorOfMajor(key.majorPitchClass)
            != key.relativeMinorPitchClass) return 3;
        if (CircleOfFifths::indexForMajorPitchClass(key.majorPitchClass) != i) return 4;
        if (CircleOfFifths::indexForRelativeMinorPitchClass(key.relativeMinorPitchClass) != i) return 5;
    }
    if (CircleOfFifths::nextFourth(0) != 5) return 6;
    if (CircleOfFifths::nextFifth(-1) != 6) return 7;
    if (CircleOfFifths::at(-1).majorPitchClass != 5) return 8;
    std::cout << "Circle of Fifths contract passed: 12 major keys, 12 relative minors, fifth/fourth navigation\n";
    return 0;
}
