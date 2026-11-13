#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace ppgu {

struct SfzRegion {
    int key = -1;
    int lokey = 0, hikey = 127;
    int lovel = 0, hivel = 127;
    std::string sample;
    bool loopMode = false;
    bool oneShot = false;
    int transpose = 0;
    int seqLength = 1, seqPosition = 1;
};

struct SfzDocument {
    std::vector<SfzRegion> regions;
    bool ok = false;
    std::string error;
};

// Pure logic; no JUCE; deterministic. Clamped per INFO contract.
class SfzParser {
public:
    static SfzDocument parse(const std::string& text);
};

} // namespace ppgu