#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <array>
#include <vector>
#include "Sampler/SamplePool.h"
#include "Performance/PatternScheduler.h"
#include "Performance/PatternTriggerState.h"

class PopPunkGuitarUltimateAudioProcessor final : public juce::AudioProcessor
{
public:
    PopPunkGuitarUltimateAudioProcessor();
    ~PopPunkGuitarUltimateAudioProcessor() override;

    void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 30.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destinationData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    [[nodiscard]] ppgu::sampler::LoadReport loadSfzFile(const juce::File& sfzFile);
    void loadSfzFileAsync(const juce::File& sfzFile,
                          std::function<void(ppgu::sampler::LoadReport)> completion);
    [[nodiscard]] juce::String getCurrentSfzFilePath() const;

    juce::AudioProcessorValueTreeState parameters;
    juce::MidiKeyboardState keyboardState;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    bool consumePanicRequest() noexcept { return panicRequested.exchange(false, std::memory_order_acq_rel); }
    // Lock-free read model for the UI; never query MidiKeyboardState from a UI timer.
    bool isMidiNoteHeld(int midiNote) const noexcept
    {
        if (midiNote < 0 || midiNote >= 128) return false;
        for (const auto& channel : displayHeldNotes)
            if (channel[static_cast<size_t>(midiNote)].load(std::memory_order_relaxed)) return true;
        return false;
    }

private:
    std::atomic<bool> panicRequested { false };
    std::array<std::array<std::atomic<bool>, 128>, 16> displayHeldNotes {};
    double currentSampleRate = 44100.0;
    int maximumBlockSize = 512;
    ppgu::sampler::SamplerEngine sampler;
    ppgu::performance::PatternScheduler patternScheduler;
    ppgu::performance::PatternTriggerState patternTriggerState;
    ppgu::performance::BlockClock patternTransportClock;
    std::array<std::array<bool, 128>, 16> patternHeldNotes {};
    std::array<ppgu::performance::PatternEvent, 512> patternEvents {};
    std::array<ppgu::performance::ScheduledEvent, ppgu::performance::PatternScheduler::maximumEventsPerBlock> scheduledPatternEvents {};
    int previousPlayMode = -1;
    bool patternCycleStarted = false;
    int previousPatternChordCount = 0;
    std::array<int, 6> previousPatternChordNotes {};
    int previousPatternChordChannel = 1;
    int previousPatternSlot = -1;
    int previousPatternLengthChoice = -1;
    double patternCycleStartPpq = 0.0;

    // Preallocated stereo effect memories. All resizing happens in prepareToPlay,
    // never in processBlock.
    std::array<std::vector<float>, 2> delayBuffers;
    std::array<std::vector<float>, 2> chorusBuffers;
    std::array<std::vector<float>, 2> flangerBuffers;
    std::array<float, 2> eqLowState {};
    std::array<float, 2> eqHighState {};
    std::array<float, 2> compressorEnvelope {};
    std::array<float, 64> cabinetImpulse {};
    std::array<std::array<float, 64>, 2> cabinetHistory {};
    size_t cabinetWritePosition = 0;
    std::array<std::vector<float>, 2> reverbDryBuffers;
    size_t delayWritePosition = 0;
    size_t chorusWritePosition = 0;
    size_t flangerWritePosition = 0;
    double flangerPhase = 0.0;
    double chorusPhase = 0.0;
    juce::Reverb reverb;
    // Protect the serialized library path from concurrent UI/worker/state callbacks.
    mutable juce::CriticalSection sampleStateLock;
    juce::String currentSfzFilePath;
    // Monotonic request token: only the newest requested bank may publish its
    // path/callback. A single worker serializes sampler mutations; stale queued
    // jobs are skipped before doing disk/index work.
    std::shared_ptr<std::atomic<std::uint64_t>> sampleLoadGeneration {
        std::make_shared<std::atomic<std::uint64_t>>(0) };
    // Declared after sampler so the worker pool is destroyed first; the destructor
    // also waits for in-flight loads before sampler storage is released.
    juce::ThreadPool sampleLoadPool { 1 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PopPunkGuitarUltimateAudioProcessor)
};
