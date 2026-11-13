#include "ChordEngine.h"

namespace ppgu {

const char* const ChordEngine::kCircle[12] = {
    "C","G","D","A","E","B","F#","Db","Ab","Eb","Bb","F"};

static std::vector<int> maj(int r)  { return {r, r+4, r+7}; }
static std::vector<int> min(int r)  { return {r, r+3, r+7}; }
static std::vector<int> dim(int r)  { return {r, r+3, r+6}; }

ChordEngine::Chord ChordEngine::diatonic(int rootPc, int degree) {
    static const int qualities[7] = {0,1,0,0,1,1,0}; // major/minor pattern for major key
    static const int semis[7] = {0,2,4,5,7,9,11};
    int r = (rootPc + semis[degree % 7]) % 12;
    Chord c;
    c.name = kCircle[r] + (qualities[degree % 7] == 1 ? "m" : "");
    c.midi = qualities[degree % 7] == 1 ? min(r) : (degree % 7 == 6 ? dim(r) : maj(r));
    return c;
}

int ChordEngine::circleNeighbour(int rootPc, int step) {
    int pos = -1;
    for (int i = 0; i < 12; ++i) if (i == rootPc) { pos = i; break; }
    if (pos < 0) pos = 0;
    return (pos + step + 12) % 12;
}

ChordEngine::Chord ChordEngine::powerChord(int rootPc) {
    Chord c; c.name = kCircle[rootPc] + "5"; c.midi = {rootPc, rootPc + 7, rootPc + 12};
    return c;
}

} // namespace ppgu