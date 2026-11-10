#include "SampleMetadataIndex.h"
#include "SampleStreamingService.h"

#include <juce_audio_formats/juce_audio_formats.h>

#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <thread>

int main()
{
    const auto root = std::filesystem::temp_directory_path() / "ppgu-streaming-service-tests";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);
    const juce::File wav((root / "stream.wav").string());

    constexpr int sourceFrames = 4096;
    juce::AudioBuffer<float> source(2, sourceFrames);
    for (int frame = 0; frame < sourceFrames; ++frame)
    {
        const float value = 0.25f * std::sin(
            2.0 * juce::MathConstants<double>::pi * 330.0 * frame / 48000.0);
        source.setSample(0, frame, value);
        source.setSample(1, frame, -value);
    }

    juce::WavAudioFormat format;
    std::unique_ptr<juce::OutputStream> stream(wav.createOutputStream());
    if (!stream) return 1;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        format.createWriterFor(stream.get(), 48000.0, 2, 24, {}, 0));
    if (!writer) return 1;
    stream.release();
    if (!writer->writeFromAudioSampleBuffer(source, 0, sourceFrames)) return 1;
    writer.reset();

    ppgu::sampler::SampleMetadataIndex index;
    auto scan = index.scanDirectory(juce::File(root.string()));
    if (!scan.success || scan.samples.size() != 1)
    {
        std::cerr << "Streaming test could not index its WAV fixture.\\n";
        return 1;
    }

    ppgu::sampler::SampleStreamingService service(std::move(scan.samples), 4, 256);
    if (!service.isValid() || service.cacheBytes() != 4U * 256U * 2U * sizeof(float))
    {
        std::cerr << "Streaming service did not create the expected bounded cache.\\n";
        return 1;
    }
    if (!service.start())
    {
        std::cerr << "Streaming service failed to start its worker.\\n";
        return 1;
    }

    if (service.requestPage({ 99, 0 }))
    {
        std::cerr << "Streaming service accepted an invalid sample ID.\\n";
        return 1;
    }
    if (!service.requestPage({ 0, 2 }))
    {
        std::cerr << "Streaming service rejected a valid page request.\\n";
        return 1;
    }

    ppgu::sampler::PageCacheSlots::ReadLease lease;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (std::chrono::steady_clock::now() < deadline
           && !service.tryAcquire({ 0, 2 }, lease))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if (!lease || lease.channels != 2 || lease.frames != 256)
    {
        std::cerr << "Worker did not publish the requested page before timeout.\\n";
        return 1;
    }

    const int sourceFrame = 2 * 256 + 37;
    const float expected = 0.25f * std::sin(
        2.0 * juce::MathConstants<double>::pi * 330.0 * sourceFrame / 48000.0);
    if (std::abs(lease.samples[37] - expected) > 0.0001f
        || std::abs(lease.samples[37] + lease.samples[256 + 37]) > 0.0001f)
    {
        std::cerr << "Streaming worker published corrupted planar page data.\\n";
        return 1;
    }
    service.release(lease);

    // Duplicate requests for a resident page must not decode and occupy
    // another cache slot. The worker processes the request asynchronously.
    if (!service.requestPage({ 0, 2 })) return 1;
    const auto duplicateDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (std::chrono::steady_clock::now() < duplicateDeadline
           && service.statistics().requestsProcessed < 2)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    const auto stats = service.statistics();
    if (stats.pagesDecoded != 1 || stats.invalidRequests != 1
        || stats.requestsSubmitted != 2 || stats.requestsProcessed != 2
        || stats.decodeFailures != 0)
    {
        std::cerr << "Streaming statistics or ready-page duplicate suppression failed.\\n";
        return 1;
    }

    service.stop();
    if (service.requestPage({ 0, 1 }))
    {
        std::cerr << "Streaming service accepted requests after worker shutdown.\\n";
        return 1;
    }

    std::filesystem::remove_all(root);
    std::cout << "Streaming service tests passed: bounded cache, worker decode, audio-side acquire, duplicate suppression and shutdown.\\n";
    return 0;
}
