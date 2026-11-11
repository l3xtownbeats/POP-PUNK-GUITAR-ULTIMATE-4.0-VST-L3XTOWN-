#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <array>

class PopPunkGuitarUltimateAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit PopPunkGuitarUltimateAudioProcessorEditor(PopPunkGuitarUltimateAudioProcessor&);
    ~PopPunkGuitarUltimateAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void chooseSfzFile();
    void configureKnob(juce::Slider&, juce::Label&, const juce::String&);
    void drawPanel(juce::Graphics&, juce::Rectangle<int>);
    void loadFactoryPresetCatalog();
    void applyFactoryPreset(int index);
    PopPunkGuitarUltimateAudioProcessor& processor;
    juce::TextButton loadButton { "LOAD SFZ" };
    juce::ComboBox presetBox, categoryBox;
    juce::TextEditor presetSearch;
    struct FactoryPreset { juce::String name, category, mode, soundCharacter, groove, articulation, sampleProfile; float drive=0.0f, tone=5.0f, reverb=0.0f, outputDb=0.0f; };
    std::vector<FactoryPreset> factoryPresets;
    std::vector<int> visiblePresetIndices;
    juce::Label titleLabel, subtitleLabel, statusLabel, ampTitle, fxTitle, playbackTitle;
    juce::TextButton melodyTab { "♫  MELODY" }, patternTab { "PATTERN" };
    juce::ComboBox modeBox, startModeBox;
    std::array<juce::Slider, 9> knobs;
    std::array<juce::Label, 9> knobLabels;
    std::array<juce::String, 9> knobNames { "GAIN", "TREBLE", "MID", "BASS", "VELOCITY", "DYNAMICS", "SWING", "HUMANIZE", "ACCENT" };
    std::array<juce::String, 9> parameterIDs { "ampGain", "eqHigh", "eqMid", "eqLow", "velocity", "dynamics", "swing", "humanize", "accent" };
    std::array<juce::TextButton, 7> fxButtons {
        juce::TextButton("COMPRESSOR"), juce::TextButton("EQ"), juce::TextButton("DISTORTION"),
        juce::TextButton("CHORUS"), juce::TextButton("DELAY"), juce::TextButton("REVERB"),
        juce::TextButton("CABINET IR")
    };
    std::array<juce::String, 7> fxParameterIDs { "compressorOn", "eqOn", "distortionOn", "chorusOn", "delayOn", "reverbOn", "cabinetOn" };
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 9> knobAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 7> fxAttachments;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment, startModeAttachment;
    std::unique_ptr<juce::MidiKeyboardComponent> keyboard;
    std::unique_ptr<juce::FileChooser> fileChooser;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PopPunkGuitarUltimateAudioProcessorEditor)
};
