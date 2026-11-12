#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/ThemeManager.h"
#include "Performance/AutoChordEngine.h"
#include "UI/AuxiliaryPanelWindowManager.h"
#include "UI/FXEditPanel.h"
#include "UI/AutoChordsPanel.h"
#include "UI/ImportGuitarPanel.h"
#include "UI/SettingsPanel.h"
#include "UI/PerformanceDisplay.h"
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


class PPGUFeatureLandingPanel final : public juce::Component
{
public:
    PPGUFeatureLandingPanel(const juce::String& heading, const juce::String& descriptionTextValue,
                            const PPGUThemeManager::Palette& initialPalette)
        : titleText(heading), bodyText(descriptionTextValue), palette(initialPalette)
    {
        title.setText(titleText, juce::dontSendNotification);
        title.setFont(juce::Font(22.0f, juce::Font::bold));
        title.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(title);
        body.setText(bodyText, juce::dontSendNotification);
        body.setFont(juce::Font(14.0f));
        body.setJustificationType(juce::Justification::topLeft);
        body.setMinimumHorizontalScale(0.85f);
        addAndMakeVisible(body);
        status.setText("DESIGN ENTRY POINT  /  FUNCTIONAL BACKEND PENDING",
                       juce::dontSendNotification);
        status.setFont(juce::Font(10.5f, juce::Font::bold));
        status.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(status);
        setPalette(initialPalette);
    }
    void setPalette(const PPGUThemeManager::Palette& value)
    {
        palette = value;
        title.setColour(juce::Label::textColourId, palette.token("text_primary"));
        body.setColour(juce::Label::textColourId, palette.token("text_secondary"));
        status.setColour(juce::Label::textColourId, palette.token("warning"));
        repaint();
    }
    void paint(juce::Graphics& g) override
    {
        auto area = getLocalBounds().toFloat();
        g.setColour(palette.token("panel_elevated"));
        g.fillRoundedRectangle(area, 10.0f);
        g.setColour(palette.token("control_border"));
        g.drawRoundedRectangle(area.reduced(0.5f), 10.0f, 1.0f);
        g.setColour(palette.token("accent"));
        g.fillRoundedRectangle(area.getX() + 18.0f, area.getY() + 16.0f, 4.0f,
                               juce::jmax(28.0f, area.getHeight() - 32.0f), 2.0f);
        auto card = area.reduced(18.0f, 14.0f);
        card.removeFromLeft(18.0f);
        g.setColour(palette.token("panel").withAlpha(0.62f));
        g.fillRoundedRectangle(card, 8.0f);
    }
    void resized() override
    {
        auto area = getLocalBounds().reduced(34, 24);
        title.setBounds(area.removeFromTop(42));
        area.removeFromTop(10);
        status.setBounds(area.removeFromBottom(28));
        area.removeFromBottom(12);
        body.setBounds(area);
    }
private:
    juce::String titleText, bodyText;
    PPGUThemeManager::Palette palette;
    juce::Label title, body, status;
};

class PopPunkGuitarUltimateAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit PopPunkGuitarUltimateAudioProcessorEditor(PopPunkGuitarUltimateAudioProcessor&);
    ~PopPunkGuitarUltimateAudioProcessorEditor() override;
    bool keyPressed(const juce::KeyPress&) override;
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    void updateKeyboardReadout();
    void setKeyboardCollapsed(bool collapsed);
    void chooseSfzFile();
    juce::File getRuntimeSkinRoot() const;
    int getSkinCount() const;
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
    std::array<juce::TextButton, 7> panelButtons { juce::TextButton("FX EDIT"), juce::TextButton("AUTOCHORDS"), juce::TextButton("IMPORT"), juce::TextButton("SETTINGS"), juce::TextButton("MIDI MONITOR"), juce::TextButton("CIRCLE OF FIFTHS"), juce::TextButton("HARMONY ASSIST") };
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
    std::array<juce::TextButton, 8> fxButtons {
        juce::TextButton("COMPRESSOR"), juce::TextButton("EQ"), juce::TextButton("DISTORTION"),
        juce::TextButton("FLANGER"), juce::TextButton("CHORUS"), juce::TextButton("DELAY"),
        juce::TextButton("REVERB"), juce::TextButton("CABINET IR")
    };
    std::array<juce::String, 8> fxParameterIDs {
        "compressorOn", "eqOn", "distortionOn", "flangerOn",
        "chorusOn", "delayOn", "reverbOn", "cabinetOn"
    };
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 9> knobAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 8> fxAttachments;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment, startModeAttachment;
    std::unique_ptr<juce::MidiKeyboardComponent> keyboard;
    juce::TextButton keyboardToggleButton { "⌄  HIDE KEYS" };
    juce::Label activeNotesLabel, chordLabel;
    bool keyboardCollapsed = false;
    std::unique_ptr<PPGUFXEditPanel> fxEditPanel;
    std::unique_ptr<PPGUAutoChordsPanel> autoChordsPanel;
    std::unique_ptr<PPGUImportGuitarPanel> importGuitarPanel;
    std::unique_ptr<PPGUSettingsPanel> settingsPanel;
    std::unique_ptr<PPGUPerformanceDisplay> performanceDisplay;
    std::unique_ptr<PPGUFeatureLandingPanel> circleOfFifthsPanel;
    std::unique_ptr<PPGUFeatureLandingPanel> harmonyAssistantPanel;
    std::unique_ptr<PPGUAuxiliaryPanelWindowManager> auxiliaryPanels;
    std::unique_ptr<juce::FileChooser> fileChooser;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PopPunkGuitarUltimateAudioProcessorEditor)
};
