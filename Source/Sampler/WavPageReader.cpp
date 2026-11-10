#include "WavPageReader.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace ppgu::sampler
{
WavPageReader::~WavPageReader()
{
    close();
}

bool WavPageReader::open(const SampleMetadata& metadata, std::string& error)
{
    close();

    if (metadata.channels == 0 || metadata.channels > 2)
    {
        error = "WAV page reader supports mono/stereo files only.";
        return false;
    }

    if (!metadata.file.existsAsFile())
    {
        error = "Indexed WAV file no longer exists: " + metadata.file.getFullPathName().toStdString();
        return false;
    }

    const auto currentBytes = metadata.file.getSize();
    const auto currentModificationTime = metadata.file.getLastModificationTime().toMilliseconds();
    if (currentBytes < 0 || static_cast<std::uint64_t>(currentBytes) != metadata.fileBytes
        || currentModificationTime != metadata.modificationTimeMs)
    {
        error = "WAV file changed since it was indexed: " + metadata.file.getFullPathName().toStdString();
        return false;
    }

    juce::AudioFormatManager formats;
    formats.registerBasicFormats();
    auto candidate = std::unique_ptr<juce::AudioFormatReader>(
        formats.createReaderFor(metadata.file));
    if (candidate == nullptr)
    {
        error = "Could not open indexed WAV file: " + metadata.file.getFullPathName().toStdString();
        return false;
    }

    if (candidate->lengthInSamples <= 0
        || static_cast<std::uint64_t>(candidate->lengthInSamples) != metadata.frames
        || candidate->numChannels != metadata.channels
        || std::abs(candidate->sampleRate - metadata.sampleRate) > 0.01)
    {
        error = "WAV header no longer matches indexed metadata: " + metadata.file.getFullPathName().toStdString();
        return false;
    }

    sourceFile = metadata.file;
    frameCount = metadata.frames;
    channels = metadata.channels;
    sampleRate = metadata.sampleRate;
    reader = std::move(candidate);
    return true;
}

PageReadReport WavPageReader::readPage(std::uint64_t pageIndex,
                                       SamplePage& destination,
                                       std::uint32_t pageFrames)
{
    PageReadReport report;
    if (reader == nullptr)
    {
        report.error = "WAV page reader is not open.";
        return report;
    }
    if (pageFrames == 0)
    {
        report.error = "Page frame count must be greater than zero.";
        return report;
    }

    const auto pageCount = getPageCount(pageFrames);
    if (pageIndex >= pageCount)
    {
        report.error = "Requested WAV page is outside the indexed file.";
        return report;
    }

    const auto firstFrame = pageIndex * static_cast<std::uint64_t>(pageFrames);
    const auto remaining = frameCount - firstFrame;
    const auto frames = static_cast<std::uint32_t>(
        std::min<std::uint64_t>(remaining, pageFrames));
    if (frames == 0 || firstFrame > static_cast<std::uint64_t>(
            std::numeric_limits<juce::int64>::max()))
    {
        report.error = "Requested WAV page has invalid frame bounds.";
        return report;
    }

    // Allocation and decode occur here on the caller's worker thread, never
    // in the plugin's audio callback.
    destination.audio.setSize(static_cast<int>(channels), static_cast<int>(frames),
                              false, true, false);
    if (!reader->read(&destination.audio, 0, static_cast<int>(frames),
                      static_cast<juce::int64>(firstFrame), true, channels > 1))
    {
        destination.audio.setSize(0, 0);
        report.error = "WAV page decode failed at page " + std::to_string(pageIndex) + ".";
        return report;
    }

    destination.descriptor.pageIndex = pageIndex;
    destination.descriptor.firstFrame = firstFrame;
    destination.descriptor.frameCount = frames;
    report.success = true;
    return report;
}

void WavPageReader::close() noexcept
{
    reader.reset();
    sourceFile = {};
    frameCount = 0;
    channels = 0;
    sampleRate = 0.0;
}

std::uint64_t WavPageReader::getPageCount(std::uint32_t pageFrames) const noexcept
{
    if (pageFrames == 0 || frameCount == 0)
        return 0;
    return 1 + (frameCount - 1) / pageFrames;
}
}
