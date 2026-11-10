#pragma once

#include "SfzParser.h"
#include <juce_audio_formats/juce_audio_formats.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <map>
#include <memory>
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

    // Call from a non-audio thread only. Published snapshots and sample buffers
    // remain alive until SamplePool destruction, so audio-thread raw pointers stay valid.
    [[nodiscard]] LoadReport loadSfz(const juce::File& sfzFile);

    struct RegionView
    {
        const juce::AudioBuffer<float>* audio = nullptr;
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
    struct SampleAsset
    {
        juce::AudioBuffer<float> audio;
        double sampleRate = 44100.0;
    };

    mutable std::mutex loaderMutex;
    // Assets/snapshots are retained until destruction so audio-thread raw
    // pointers never dangle. Explicit caps make that lifetime policy bounded.
    static constexpr std::size_t maximumSampleStorageBytes = 1024ULL * 1024ULL * 1024ULL;
    static constexpr std::size_t maximumSnapshots = 128;
    std::size_t sampleStorageBytes = 0;
    std::map<std::string, std::unique_ptr<SampleAsset>> sampleStorage;
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
        int note = -1;
        int midiChannel = 1;
    };

    [[nodiscard]] int chooseVoice(int maximumActiveVoices) noexcept;
    void releaseVoice(Voice& voice) noexcept;

    SamplePool pool;
    std::array<Voice, maximumVoices> voices {};
    double outputSampleRate = 44100.0;
    std::uint64_t nextAge = 1;
};
}
