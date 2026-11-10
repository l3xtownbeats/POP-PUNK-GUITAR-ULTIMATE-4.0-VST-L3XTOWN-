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
    void chooseAudioLibraryFolder();
    void configureKnob(juce::Slider&, juce::Label&, const juce::String&);
    void drawPanel(juce::Graphics&, juce::Rectangle<int>);
    void loadFactoryPresetCatalog();
    void applyFactoryPreset(int index);
    void applySkin(int index);
    void refreshSkinColours();
    void loadExternalSkin(const juce::File& file);
    PopPunkGuitarUltimateAudioProcessor& processor;
    juce::TextButton loadButton { "LOAD SFZ" }, importButton { "IMPORT SOUNDS" }, fxOpenButton { "FX STUDIO" };
    juce::TextButton skinsButton { "SKINS" }, midiButton { "MIDI" }, doubleButton { "DOUBLE" }, multicoreButton { "MULTICORE" };
    juce::TextButton autoChordsButton { "AUTOCHORDS  OFF" };
    juce::ComboBox autoChordKeyBox, autoChordScaleBox;
    juce::ComboBox melodyFilterBox, styleFilterBox, processorBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> doubleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> processorAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoChordsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> autoChordKeyAttachment, autoChordScaleAttachment;
    int activeSkinIndex = 0;
    juce::Image approvedMelodyArtwork, approvedPatternArtwork;
    juce::Image externalSkinMelodyArtwork, externalSkinPatternArtwork;
    std::unique_ptr<juce::DocumentWindow> fxWindow;
    juce::ComboBox presetBox, categoryBox;
    juce::TextEditor presetSearch;
    struct FactoryPreset {
        juce::String id, name, category, mode, soundCharacter, groove, articulation, styleProfileId;
        juce::String sampleProfile, tonalTarget, noteBehavior, sampleRequirement;
        juce::var parameters, performance;
        int palmMuteMask = 0, accentMaskHigh = 0, accentMaskLow = 0;
        bool humanizeEnabled = false, velocityAccents = false, strokeVariation = true;
        float drive=0.0f, tone=5.0f, reverb=0.0f, outputDb=0.0f;
    };
    std::vector<FactoryPreset> factoryPresets;
    std::vector<int> visiblePresetIndices;
    juce::Label titleLabel, subtitleLabel, statusLabel, ampTitle, fxTitle, playbackTitle;
    juce::TextButton melodyTab { "♫  MELODY" }, patternTab { "PATTERN" };
    juce::ComboBox modeBox, startModeBox;
    std::array<juce::Slider, 10> knobs;
    std::array<juce::Label, 10> knobLabels;
    std::array<juce::String, 10> knobNames { "GAIN", "TREBLE", "MID", "BASS", "VOICES", "VELOCITY", "DYNAMICS", "SWING", "HUMANIZE", "ACCENT" };
    std::array<juce::String, 10> parameterIDs { "ampGain", "eqHigh", "eqMid", "eqLow", "voices", "velocity", "dynamics", "swing", "humanize", "accent" };
    std::array<juce::TextButton, 7> fxButtons {
        juce::TextButton("COMPRESSOR"), juce::TextButton("EQ"), juce::TextButton("DISTORTION"),
        juce::TextButton("CHORUS"), juce::TextButton("DELAY"), juce::TextButton("REVERB"),
        juce::TextButton("CABINET IR")
    };
    std::array<juce::String, 7> fxParameterIDs { "compressorOn", "eqOn", "distortionOn", "chorusOn", "delayOn", "reverbOn", "cabinetOn" };
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 10> knobAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 7> fxAttachments;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment, startModeAttachment;
    std::unique_ptr<juce::MidiKeyboardComponent> keyboard;
    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<juce::FileChooser> skinChooser;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PopPunkGuitarUltimateAudioProcessorEditor)
};
