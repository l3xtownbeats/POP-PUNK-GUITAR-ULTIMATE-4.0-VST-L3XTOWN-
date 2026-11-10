#pragma once

#include "SampleMetadataIndex.h"

#include <juce_audio_formats/juce_audio_formats.h>

#include <cstdint>
#include <memory>
#include <string>

namespace ppgu::sampler
{
struct SamplePageDescriptor
{
    std::uint64_t pageIndex = 0;
    std::uint64_t firstFrame = 0;
    std::uint32_t frameCount = 0;
};

struct SamplePage
{
    SamplePageDescriptor descriptor;
    juce::AudioBuffer<float> audio;
};

struct PageReadReport
{
    bool success = false;
    std::string error;
};

// Worker-thread-only WAV reader. The caller owns the reader on a non-audio
// thread and must never invoke open/readPage/close from processBlock().
class WavPageReader final
{
public:
    static constexpr std::uint32_t defaultPageFrames = 4096;

    WavPageReader() = default;
    ~WavPageReader();

    WavPageReader(const WavPageReader&) = delete;
    WavPageReader& operator=(const WavPageReader&) = delete;

    [[nodiscard]] bool open(const SampleMetadata& metadata, std::string& error);
    [[nodiscard]] PageReadReport readPage(std::uint64_t pageIndex,
                                          SamplePage& destination,
                                          std::uint32_t pageFrames = defaultPageFrames);
    // Worker-only zero-intermediate-allocation path. The caller supplies
    // preallocated planar channel pointers (for example, a reserved cache
    // slot); samples are decoded directly into that storage.
    [[nodiscard]] PageReadReport readPageInto(std::uint64_t pageIndex,
                                              float* const* destinationChannels,
                                              int destinationChannelCount,
                                              int destinationOffset,
                                              std::uint32_t& framesWritten,
                                              std::uint32_t pageFrames = defaultPageFrames);
    void close() noexcept;

    [[nodiscard]] bool isOpen() const noexcept { return reader != nullptr; }
    [[nodiscard]] std::uint64_t getPageCount(std::uint32_t pageFrames = defaultPageFrames) const noexcept;
    [[nodiscard]] std::uint64_t getFrameCount() const noexcept { return frameCount; }
    [[nodiscard]] unsigned int getChannelCount() const noexcept { return channels; }
    [[nodiscard]] double getSampleRate() const noexcept { return sampleRate; }

private:
    std::unique_ptr<juce::AudioFormatReader> reader;
    juce::File sourceFile;
    std::uint64_t frameCount = 0;
    unsigned int channels = 0;
    double sampleRate = 0.0;
};
}
