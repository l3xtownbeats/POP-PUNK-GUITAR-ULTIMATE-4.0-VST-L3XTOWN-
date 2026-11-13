#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"
#include "UI/Panels.h"
#include "UI/MultiWindowWorkspace.h"
#include "UI/ThemeManager.h"
#include <BinaryData.h>
#include <PPGUData.h>
#include <PPGUTestData.h>

namespace ppgu {

class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit PluginEditor(PluginProcessor& p);
    ~PluginEditor() override { stopTimer(); }
    void paint(juce::Graphics&) override;
    void resized() override;
    bool keyPressed(const juce::KeyPress&) override;

private:
    void timerCallback() override;
    void togglePanelWindow(PanelId id);
    void applyThemeIndex(int i);
    PluginProcessor& proc_;
    juce::MidiKeyboardComponent keyboard { proc_.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard };

    // Zone A top bar
    juce::TextButton btnPrev { "<" }, btnNext { ">" }, btnFav { "F" },
        btnSkins { "SKINS" }, btnImport { "IMPORT GUITAR" }, btnAuto { "AUTO DISCOVER" },
        btnCurator { "CURATOR" }, btnSettings { "SETTINGS" }, btnAbout { "ABOUT" };
    juce::Label presetName { {}, "Preset" }, presetCategory { {}, "Clean" };
    juce::TextEditor presetSearch;
    // Zone B browser
    juce::ListBox browser;
    juce::ComboBox categories;
    // Zone C knobs
    juce::Slider knobs[9];
    juce::AudioProcessorValueTreeState::SliderAttachment* knobAtt[9] {};
    juce::ComboBox playModeBox;
    // Zone D FX toggles
    juce::ToggleButton fxToggles[8];
    juce::AudioProcessorValueTreeState::ButtonAttachment* fxAtt[8] {};
    // Buttons opening panel windows
    juce::TextButton btnFxEdit { "FX EDIT" }, btnAutochords { "AUTOCHORDS" }, btnLoadSfz { "LOAD SFZ" };
    juce::FileChooser sfzChooser { "SFZ", juce::File(), "*.sfz" };
    juce::Random themeRandom_;
    std::vector<MultiWindowWorkspace::WindowState> windows_ = MultiWindowWorkspace::defaults();
    int themeIndex_ = 51; // fallback skin-51 until registry loads
    JUCE_DECLARE_NON_COPYABLE(PluginEditor)
};

} // namespace ppgu