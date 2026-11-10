#include "SampleMetadataIndex.h"
#include "SampleStreamingService.h"
#include "StreamingVoiceReader.h"

#include <juce_audio_formats/juce_audio_formats.h>

#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <limits>
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

    // Oversized page geometry must fail closed before attempting a
    // multi-gigabyte cache allocation or starting a worker thread.
    {
        ppgu::sampler::SampleStreamingService oversizedService(
            scan.samples, 1, std::numeric_limits<std::uint32_t>::max());
        if (oversizedService.isValid() || oversizedService.start())
        {
            std::cerr << "Streaming service accepted an oversized cache configuration.\\n";
            return 1;
        }
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

    // Repeated voices asking for one cold page must share one queued decode.
    for (int duplicate = 0; duplicate < 64; ++duplicate)
        if (!service.requestPage({ 0, 2 }))
        {
            std::cerr << "Streaming service rejected a coalesced page request.\\n";
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

    // Re-requesting a resident page is a no-op and must not occupy another slot.
    if (!service.requestPage({ 0, 2 })) return 1;
    const auto duplicateDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (std::chrono::steady_clock::now() < duplicateDeadline
           && service.statistics().requestsProcessed < 1)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    const auto stats = service.statistics();
    if (stats.pagesDecoded != 1 || stats.invalidRequests != 1
        || stats.requestsSubmitted != 1 || stats.requestsProcessed != 1
        || stats.requestsCoalesced == 0 || stats.decodeFailures != 0)
    {
        std::cerr << "Streaming statistics or ready-page duplicate suppression failed.\\n";
        return 1;
    }

    // Hermite reads that straddle a page boundary must pin both pages and
    // reproduce the continuous source waveform without reading outside either.
    {
    ppgu::sampler::StreamingVoiceReader voiceReader;
    voiceReader.attach(&service, 0, sourceFrames);
    float streamedLeft = 0.0f, streamedRight = 0.0f;
    bool hermiteReady = false;
    const auto hermiteDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (std::chrono::steady_clock::now() < hermiteDeadline && !hermiteReady)
    {
        hermiteReady = voiceReader.readHermite(255, 0.5f, 0, sourceFrames,
                                               streamedLeft, streamedRight);
        if (!hermiteReady)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    if (!hermiteReady || std::abs(streamedLeft + streamedRight) > 0.0001f
        || std::abs(streamedLeft) < 0.01f)
    {
        std::cerr << "Streaming voice reader failed Hermite interpolation across a page boundary.\\n";
        return 1;
    }
    voiceReader.prefetch(510);
    } // RAII must release all page pins before the stream is stopped.

    // Region-edge interpolation must clamp its support taps to the active
    // region, including the first and final source frames. Invalid regions and
    // out-of-region positions must fail without exposing stale output.
    {
        ppgu::sampler::StreamingVoiceReader edgeReader;
        edgeReader.attach(&service, 0, sourceFrames);
        const auto verifyEdge = [&](std::uint64_t frame, float expected)
        {
            float left = 123.0f, right = 456.0f;
            bool ready = false;
            const auto edgeDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
            while (std::chrono::steady_clock::now() < edgeDeadline && !ready)
            {
                ready = edgeReader.readHermite(frame, 0.0f, 0, sourceFrames, left, right);
                if (!ready) std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return ready && std::abs(left - expected) < 0.0001f
                && std::abs(left + right) < 0.0001f;
        };

        const float firstExpected = 0.25f * std::sin(0.0);
        const float lastExpected = 0.25f * std::sin(
            2.0 * juce::MathConstants<double>::pi * 330.0 * (sourceFrames - 1) / 48000.0);
        if (!verifyEdge(0, firstExpected) || !verifyEdge(sourceFrames - 1, lastExpected))
        {
            std::cerr << "Streaming voice reader failed interpolation at a sample-region edge.\\n";
            return 1;
        }

        float untouchedLeft = 123.0f, untouchedRight = 456.0f;
        if (edgeReader.readHermite(sourceFrames, 0.0f, 0, sourceFrames,
                                   untouchedLeft, untouchedRight)
            || edgeReader.readHermite(0, 0.0f, 10, 10, untouchedLeft, untouchedRight)
            || untouchedLeft != 123.0f || untouchedRight != 456.0f)
        {
            std::cerr << "Streaming voice reader accepted an invalid region or changed outputs on failure.\\n";
            return 1;
        }
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
