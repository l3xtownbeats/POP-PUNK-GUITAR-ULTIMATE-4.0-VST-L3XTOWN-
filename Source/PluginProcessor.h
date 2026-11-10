#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <functional>
#include <array>
#include <vector>
#include "Sampler/SamplePool.h"
#include "Performance/PatternScheduler.h"

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

    juce::AudioProcessorValueTreeState parameters;
    juce::MidiKeyboardState keyboardState;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    bool consumePanicRequest() noexcept { return panicRequested.exchange(false, std::memory_order_acq_rel); }

private:
    std::atomic<bool> panicRequested { false };
    double currentSampleRate = 44100.0;
    int maximumBlockSize = 512;
    ppgu::sampler::SamplerEngine sampler;
    ppgu::performance::PatternScheduler patternScheduler;
    ppgu::performance::BlockClock patternTransportClock;
    std::array<std::array<bool, 128>, 16> patternHeldNotes {};
    std::array<ppgu::performance::PatternEvent, 512> patternEvents {};
    std::array<ppgu::performance::ScheduledEvent, ppgu::performance::PatternScheduler::maximumEventsPerBlock> scheduledPatternEvents {};
    juce::MidiBuffer patternMidiBuffer;
    int previousPlayMode = -1;
    bool patternCycleStarted = false;
    int previousPatternChordCount = 0;
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
    // Declared after sampler so the worker pool is destroyed first; the destructor
    // also waits for in-flight loads before sampler storage is released.
    juce::ThreadPool sampleLoadPool { 1 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PopPunkGuitarUltimateAudioProcessor)
};
