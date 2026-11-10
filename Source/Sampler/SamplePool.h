#pragma once

#include "SfzParser.h"
#include "SampleStreamingService.h"
#include "StreamingVoiceReader.h"
#include <juce_audio_formats/juce_audio_formats.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <map>
#include <memory>
#include <limits>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>

namespace ppgu::sampler
{
struct LoadReport
{
    bool success = false;
    std::size_t loadedRegions = 0;
    std::size_t loadedSamples = 0;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

class SamplePool final
{
public:
    SamplePool();
    ~SamplePool() = default;

    SamplePool(const SamplePool&) = delete;
    SamplePool& operator=(const SamplePool&) = delete;

    // Call from a non-audio thread only. Published snapshots and streaming
    // library services remain alive until SamplePool destruction, so audio-
    // thread region and service pointers stay valid across bank replacement.
    [[nodiscard]] LoadReport loadSfz(const juce::File& sfzFile);

    struct RegionView
    {
        // Exactly one source is active: decoded audio (legacy) or the
        // background streaming service. Production SFZ loads use streaming.
        const juce::AudioBuffer<float>* audio = nullptr;
        SampleStreamingService* streaming = nullptr;
        std::uint32_t sampleId = 0;
        std::uint64_t totalFrames = 0;
        double sourceSampleRate = 44100.0;
        int lowKey = 0;
        int highKey = 127;
        int rootKey = 60;
        int lowVelocity = 0;
        int highVelocity = 127;
        int lowChannel = 1;
        int highChannel = 16;
        int group = 0;
        int offBy = 0;
        int seqLength = 1;
        int seqPosition = 1;
        int roundRobinSetIndex = -1;
        int xfinLowVelocity = -1;
        int xfinHighVelocity = -1;
        int xfoutLowVelocity = -1;
        int xfoutHighVelocity = -1;
        int startFrame = 0;
        int endFrame = 0;
        int loopStartFrame = 0;
        int loopEndFrame = 0;
        int loopMode = 0; // 0 none/one-shot, 1 continuous, 2 sustain, 3 ping-pong
        float tuneCents = 0.0f;
        float gain = 1.0f;
        float pan = 0.0f;
        float releaseSeconds = 0.15f;
    };

    struct Snapshot
    {
        std::vector<RegionView> regions;
        // Audio-thread-owned cursors: [RR set][MIDI channel][MIDI note].
        mutable std::vector<std::uint32_t> roundRobinCounters;
    };

    [[nodiscard]] const RegionView* findRegion(int midiNote, int velocity, int midiChannel = 1) noexcept;
    [[nodiscard]] std::size_t getRegionCount() const noexcept;
    [[nodiscard]] std::size_t getSampleCount() const noexcept;

private:
    struct StreamingLibrary
    {
        std::string rootKey;
        std::vector<SampleMetadata> metadata;
        std::map<std::string, std::uint32_t> sampleIds;
        std::unique_ptr<SampleStreamingService> service;
    };

    mutable std::mutex loaderMutex;
    // Streaming services and immutable snapshots are retained until pool
    // destruction so audio-thread region/service pointers never dangle.
    // Repeated banks from one library root share one worker and one bounded
    // page cache instead of spawning a thread per preset/bank.
    static constexpr std::size_t maximumSnapshots = 128;
    static constexpr std::size_t maximumStreamingLibraries = 8;
    static constexpr std::size_t streamingCacheSlots = 256;
    static constexpr std::uint32_t streamingPageFrames = SampleStreamingService::defaultPageFrames;
    std::vector<std::unique_ptr<StreamingLibrary>> streamingLibraries;
    std::vector<std::unique_ptr<Snapshot>> snapshots;
    std::atomic<const Snapshot*> activeSnapshot { nullptr };
};

class SamplerEngine final
{
public:
    static constexpr int maximumVoices = 64;

    void prepare(double outputSampleRate) noexcept;
    void reset() noexcept;
    [[nodiscard]] LoadReport loadSfz(const juce::File& sfzFile);
    [[nodiscard]] bool noteOn(int midiNote, int velocity, int maximumActiveVoices = maximumVoices, int midiChannel = 1, double globalTuningSemitones = 0.0) noexcept;
    void noteOff(int midiNote, int midiChannel = 1) noexcept;
    void allNotesOff() noexcept;
    void panic(int fadeSamples) noexcept;
    void render(juce::AudioBuffer<float>& output, int startSample, int numSamples) noexcept;
    [[nodiscard]] bool hasSamples() const noexcept;
    [[nodiscard]] std::size_t getRegionCount() const noexcept;
    [[nodiscard]] std::size_t getSampleCount() const noexcept;
    [[nodiscard]] int getActiveVoiceCount() const noexcept;

private:
    struct Voice
    {
        const SamplePool::RegionView* region = nullptr;
        double position = 0.0;
        double increment = 1.0;
        float gain = 0.0f;
        float releaseStep = 0.0f;
        bool releasing = false;
        int direction = 1;
        std::uint64_t age = 0;
        std::uint64_t prefetchedPage = std::numeric_limits<std::uint64_t>::max();
        std::uint64_t streamingMissSamples = 0;
        int note = -1;
        int midiChannel = 1;
    };

    [[nodiscard]] int chooseVoice(int maximumActiveVoices) noexcept;
    void releaseVoice(Voice& voice) noexcept;

    SamplePool pool;
    // Declared after pool so readers are destroyed/released before their
    // SampleStreamingService owners during SamplerEngine teardown.
    std::array<StreamingVoiceReader, maximumVoices> streamingReaders {};
    std::array<Voice, maximumVoices> voices {};
    double outputSampleRate = 44100.0;
    std::uint64_t nextAge = 1;
};
}
