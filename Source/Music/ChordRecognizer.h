#pragma once
#include <array>
#include <string>
#include <vector>

namespace ppgu::music
{
enum class ChordQuality { Note, Major, Minor, Power, Sus2, Sus4, Diminished, Augmented, Major7, Minor7, Dominant7, Unknown };
struct ChordAnalysis
{
    bool valid=false;
    bool ambiguous=false;
    int rootPitchClass=-1;
    int bassPitchClass=-1;
    ChordQuality quality=ChordQuality::Unknown;
    std::string symbol;
    std::string qualityLabel;
    std::vector<int> midiNotes;
    std::vector<int> pitchClasses;
};
class ChordRecognizer
{
public:
    static ChordAnalysis analyze(const std::vector<int>& midiNotes);
    static const char* noteName(int pitchClass) noexcept;
    static const char* qualityName(ChordQuality quality) noexcept;
};
}
