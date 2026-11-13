#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace ppgu {

// Circle of Fifths / chord manager (AUTOCHORDS). Pure logic, testable.
class ChordEngine {
public:
    struct Chord { std::string name; std::vector<int> midi; };
    static const char* const kCircle[12];
    // Returns diatonic chord for scale degree (0..6) in major key of `rootPc`.
    static Chord diatonic(int rootPc, int degree);
    // Neighbours on the circle of fifths (clockwise/counter-clockwise).
    static int circleNeighbour(int rootPc, int step); // step +/-1..6
    static Chord powerChord(int rootPc);
};

} // namespace ppgu