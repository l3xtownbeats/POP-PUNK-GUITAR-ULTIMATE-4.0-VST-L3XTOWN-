#pragma once

#include <juce_audio_formats/juce_audio_formats.h>

#include <cstdint>
#include <string>
#include <vector>

namespace ppgu::sampler
{
struct SampleMetadata
{
    juce::File file;
    std::uint64_t frames = 0;
    double sampleRate = 0.0;
    unsigned int channels = 0;
    unsigned int bitsPerSample = 0;
    bool floatingPoint = false;
    std::uint64_t fileBytes = 0;
    std::int64_t modificationTimeMs = 0;
};

struct SampleIndexReport
{
    bool success = false;
    std::uint64_t totalFileBytes = 0;
    std::uint64_t totalFrames = 0;
    std::vector<SampleMetadata> samples;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
};

// Metadata-only directory scan. It opens WAV headers through JUCE readers,
// but never decodes or retains sample audio. Call from a non-audio thread.
class SampleMetadataIndex final
{
public:
    [[nodiscard]] SampleIndexReport scanDirectory(const juce::File& directory) const;
};
}
