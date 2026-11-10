#pragma once

#include <juce_audio_formats/juce_audio_formats.h>
#include <cstdint>
#include <string>
#include <vector>

namespace ppgu::sampler
{
struct LibraryImportReport
{
    bool success = false;
    bool activated = false;
    juce::File libraryDirectory;
    juce::File generatedSfz;
    std::uint32_t importedFiles = 0;
    std::uint32_t mappedFiles = 0;
    std::uint32_t needsMapping = 0;
    std::uint64_t sourceBytes = 0;
    std::uint64_t convertedBytes = 0;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
};

// Worker-thread-only importer. Reads formats registered by JUCE's basic
// AudioFormatManager and converts accepted audio to float32 WAV, preserving
// the original sample rate and channel count. Never call from processBlock().
class SampleLibraryImporter final
{
public:
    static constexpr std::uint32_t maximumFiles = 4096;
    static constexpr double maximumSampleDurationSeconds = 900.0;
    static constexpr unsigned int maximumChannels = 2;
    static constexpr double minimumSampleRate = 8000.0;
    static constexpr double maximumSampleRate = 192000.0;
    static constexpr std::uint32_t conversionChunkFrames = 8192;

    [[nodiscard]] LibraryImportReport importDirectory(const juce::File& sourceDirectory,
                                                       const juce::File& librariesRoot) const;
};
}
