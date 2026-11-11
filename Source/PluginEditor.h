#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/ThemeManager.h"
#include <array>

class PPGUPremiumLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void setPalette(const PPGUThemeManager::Palette& value) { palette = value; }
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    void drawComboBox(juce::Graphics&, int, int, bool, int, int, int, int, juce::ComboBox&) override;
private:
    PPGUThemeManager::Palette palette;
};

class PopPunkGuitarUltimateAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit PopPunkGuitarUltimateAudioProcessorEditor(PopPunkGuitarUltimateAudioProcessor&);
    ~PopPunkGuitarUltimateAudioProcessorEditor() override;
    bool keyPressed(const juce::KeyPress&) override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void chooseSfzFile();
    juce::File getRuntimeSkinRoot() const;
    juce::File getSkinManifest(int index) const;
    void loadSavedSkin();
    bool loadSkinByIndex(int index, bool persist);
    void showSkinMenu();
    void applyPalette();
    void persistSkinPreference();
    void configureKnob(juce::Slider&, juce::Label&, const juce::String&);
    void drawPanel(juce::Graphics&, juce::Rectangle<int>);
    void loadFactoryPresetCatalog();
    void applyFactoryPreset(int index);
    PopPunkGuitarUltimateAudioProcessor& processor;
    PPGUThemeManager themeManager;
    PPGUPremiumLookAndFeel premiumLookAndFeel;
    juce::ApplicationProperties uiProperties;
    juce::File runtimeSkinRoot;
    juce::TextButton loadButton { "LOAD SFZ" }, skinButton { "SKIN  v" };
    juce::ComboBox presetBox, categoryBox;
    juce::TextEditor presetSearch;
    struct FactoryPreset { juce::String name, category, mode, soundCharacter, groove, articulation; float drive=0.0f, tone=5.0f, reverb=0.0f, outputDb=0.0f; };
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
