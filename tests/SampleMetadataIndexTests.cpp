#include "SampleMetadataIndex.h"
#include "WavPageReader.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <cmath>
#include <array>
#include <filesystem>
#include <iostream>
#include <memory>
#include <algorithm>
#include <string>

int main()
{
    const auto root = std::filesystem::temp_directory_path() / "ppgu-metadata-index-tests";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root / "nested");
    const juce::File wav((root / "nested" / "fixture.wav").string());

    juce::AudioBuffer<float> audio(2, 1024);
    for (int i = 0; i < audio.getNumSamples(); ++i)
    {
        const float value = 0.25f * std::sin(2.0 * juce::MathConstants<double>::pi * 220.0 * i / 48000.0);
        audio.setSample(0, i, value);
        audio.setSample(1, i, -value);
    }

    juce::WavAudioFormat format;
    std::unique_ptr<juce::OutputStream> stream(wav.createOutputStream());
    if (!stream) return 1;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        format.createWriterFor(stream.get(), 48000.0, 2, 24, {}, 0));
    if (!writer) return 1;
    stream.release();
    if (!writer->writeFromAudioSampleBuffer(audio, 0, audio.getNumSamples())) return 1;
    writer.reset();

    // The metadata index must agree with the current cache/DSP channel
    // contract. A four-channel file must be skipped, never indexed for a
    // reader/cache path that can only represent mono or stereo.
    const juce::File surroundWav((root / "surround.wav").string());
    juce::AudioBuffer<float> surroundAudio(4, 128);
    surroundAudio.clear();
    std::unique_ptr<juce::OutputStream> surroundStream(surroundWav.createOutputStream());
    if (!surroundStream) return 1;
    std::unique_ptr<juce::AudioFormatWriter> surroundWriter(
        format.createWriterFor(surroundStream.get(), 48000.0, 4, 16, {}, 0));
    if (!surroundWriter) return 1;
    surroundStream.release();
    if (!surroundWriter->writeFromAudioSampleBuffer(surroundAudio, 0, surroundAudio.getNumSamples()))
        return 1;
    surroundWriter.reset();

    ppgu::sampler::SampleMetadataIndex index;
    const auto result = index.scanDirectory(juce::File(root.string()));
    if (!result.success || result.samples.size() != 1)
    {
        std::cerr << "Metadata index failed to find nested WAV.\n";
        for (const auto& error : result.errors) std::cerr << error << "\n";
        for (const auto& warning : result.warnings) std::cerr << warning << "\n";
        return 1;
    }

    const auto hasChannelWarning = std::any_of(result.warnings.begin(), result.warnings.end(),
        [](const std::string& warning) { return warning.find("mono/stereo only") != std::string::npos; });
    if (!hasChannelWarning || result.samples.size() != 1)
    {
        std::cerr << "Metadata index did not consistently reject multichannel WAV data.\\n";
        return 1;
    }

    const auto& metadata = result.samples.front();
    if (metadata.frames != 1024 || metadata.channels != 2
        || std::abs(metadata.sampleRate - 48000.0) > 0.01
        || metadata.bitsPerSample != 24 || metadata.fileBytes == 0
        || result.totalFileBytes != metadata.fileBytes)
    {
        std::cerr << "Metadata index returned incorrect WAV header information.\n";
        return 1;
    }

    ppgu::sampler::WavPageReader pageReader;
    std::string openError;
    if (!pageReader.open(metadata, openError))
    {
        std::cerr << "WAV page reader failed to open indexed file: " << openError << "\n";
        return 1;
    }

    // Use a non-divisible page size to verify the final partial page rather
    // than testing only pages that exactly fill the cache slot.
    constexpr std::uint32_t pageFrames = 300;
    if (pageReader.getPageCount(pageFrames) != 4 || pageReader.getFrameCount() != 1024)
    {
        std::cerr << "WAV page reader reported incorrect page geometry.\n";
        return 1;
    }

    ppgu::sampler::SamplePage page;
    const auto firstPage = pageReader.readPage(0, page, pageFrames);
    if (!firstPage.success || page.descriptor.pageIndex != 0
        || page.descriptor.firstFrame != 0 || page.descriptor.frameCount != pageFrames
        || page.audio.getNumChannels() != 2 || page.audio.getNumSamples() != static_cast<int>(pageFrames))
    {
        std::cerr << "WAV page reader failed first-page bounds/decode.\n";
        return 1;
    }
    if (std::abs(page.audio.getSample(0, 100)) < 0.001f
        || std::abs(page.audio.getSample(0, 100) + page.audio.getSample(1, 100)) > 0.0001f)
    {
        std::cerr << "WAV page reader returned incorrect decoded sample data.\n";
        return 1;
    }

    // Decode a page directly into caller-owned planar cache memory. The
    // non-zero offset verifies that the worker can write into a cache slot
    // without allocating an intermediate AudioBuffer or disturbing its halo.
    std::array<float, 310> directLeft {};
    std::array<float, 310> directRight {};
    float* directChannels[] { directLeft.data(), directRight.data() };
    std::uint32_t directFrames = 0;
    const auto directRead = pageReader.readPageInto(3, directChannels, 2, 7,
                                                     directFrames, pageFrames);
    const float expectedDirectSample = 0.25f * std::sin(
        2.0 * juce::MathConstants<double>::pi * 220.0 * 1100.0 / 48000.0);
    if (!directRead.success || directFrames != 124
        || std::abs(directLeft[7 + 100] - expectedDirectSample) > 0.0001f
        || std::abs(directLeft[7 + 100] + directRight[7 + 100]) > 0.0001f)
    {
        std::cerr << "WAV page reader failed direct preallocated planar decode.\\n";
        return 1;
    }
    const auto wrongChannels = pageReader.readPageInto(0, directChannels, 1, 0,
                                                         directFrames, pageFrames);
    if (wrongChannels.success || directFrames != 0)
    {
        std::cerr << "WAV page reader accepted a mismatched preallocated channel layout.\\n";
        return 1;
    }

    const auto middlePage = pageReader.readPage(2, page, pageFrames);
    if (!middlePage.success || page.descriptor.pageIndex != 2
        || page.descriptor.firstFrame != 600 || page.descriptor.frameCount != pageFrames)
    {
        std::cerr << "WAV page reader failed a middle-page seek/decode.\n";
        return 1;
    }

    const auto lastPage = pageReader.readPage(3, page, pageFrames);
    if (!lastPage.success || page.descriptor.pageIndex != 3
        || page.descriptor.firstFrame != 900 || page.descriptor.frameCount != 124
        || page.audio.getNumSamples() != 124)
    {
        std::cerr << "WAV page reader failed final partial-page decode.\n";
        return 1;
    }

    const auto outsidePage = pageReader.readPage(4, page, pageFrames);
    if (outsidePage.success)
    {
        std::cerr << "WAV page reader accepted an out-of-range page.\n";
        return 1;
    }
    const auto zeroPage = pageReader.readPage(0, page, 0);
    if (zeroPage.success)
    {
        std::cerr << "WAV page reader accepted a zero-sized page.\n";
        return 1;
    }
    pageReader.close();

    // A page reader must reject metadata that no longer describes the file.
    auto staleMetadata = metadata;
    ++staleMetadata.fileBytes;
    ppgu::sampler::WavPageReader staleReader;
    std::string staleError;
    if (staleReader.open(staleMetadata, staleError))
    {
        std::cerr << "WAV page reader accepted stale file-size metadata.\n";
        return 1;
    }

    auto multichannelMetadata = metadata;
    multichannelMetadata.channels = 4;
    ppgu::sampler::WavPageReader multichannelReader;
    std::string multichannelError;
    if (multichannelReader.open(multichannelMetadata, multichannelError))
    {
        std::cerr << "WAV page reader accepted unsupported multichannel metadata.\\n";
        return 1;
    }

    auto invalidMetadata = metadata;
    ++invalidMetadata.frames;
    ppgu::sampler::WavPageReader invalidReader;
    std::string invalidError;
    if (invalidReader.open(invalidMetadata, invalidError))
    {
        std::cerr << "WAV page reader accepted a mismatched WAV header.\n";
        return 1;
    }

    const auto missing = index.scanDirectory(juce::File((root / "missing").string()));
    if (missing.success || missing.errors.empty())
    {
        std::cerr << "Metadata index did not reject a missing directory.\n";
        return 1;
    }

    std::filesystem::remove_all(root);
    std::cout << "Sample metadata/page reader tests passed: recursive scan, metadata validation, full and partial pages, seek, decode and stale-file rejection.\n";
    return 0;
}
