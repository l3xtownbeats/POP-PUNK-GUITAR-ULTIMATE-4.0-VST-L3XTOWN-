#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "Engine/SamplerEngine.h"
#include "Engine/SamplePool.h"
#include "Engine/SampleStreamingService.h"
#include "Engine/FxChain.h"
#include "Engine/ChordEngine.h"
#include "Engine/PatternScheduler.h"
#include "Engine/PresetRuntime.h"
#include "Engine/ImportPipeline.h"

namespace ppgu {

class PluginEditor;

class PluginProcessor : public juce::AudioProcessor {
public:
    PluginProcessor();
    ~PluginProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "POP PUNK GUITAR ULTIMATE"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 3.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;
    juce::MidiKeyboardState keyboardState;
    ppgu::SamplePool pool;
    ppgu::SampleStreamingService streaming;
    ppgu::SamplerEngine engine;
    ppgu::FxChain fx;
    ppgu::PatternScheduler scheduler;
    ppgu::ImportPipeline importer;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
    void handleMidi(const juce::MidiBuffer& midi);
    void applyFxParams();
    ppgu::PlayMode currentMode() const;
};

} // namespace ppgu