#pragma once
#include <JuceHeader.h>
#include <array>
#include "ThemeManager.h"

class PPGUAutoChordsPanel final : public juce::Component
{
public:
    explicit PPGUAutoChordsPanel(const PPGUThemeManager::Palette& palette);
    void setPalette(const PPGUThemeManager::Palette& value);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void styleCombo(juce::ComboBox&);
    void styleButton(juce::TextButton&);
    void label(juce::Label&, const juce::String&, float, bool bold = true);

    juce::Label title, status, currentCaption, currentValue, chordInfo;
    juce::Label scaleLabel, keyLabel, progressionLabel, inversionLabel, durationLabel, rhythmLabel, executionLabel;
    juce::ComboBox mode, key, progression, inversion, duration, rhythm, execution;
    juce::ToggleButton enabledToggle { "AUTOCHORDS  ON" }, syncToTempo { "SYNC TO TEMPO" }, smartTranspose { "SMART TRANSPOSE" };
    juce::TextButton preview { "▶  PREVIEW" }, reset { "RESET" };
    std::array<juce::TextButton, 4> chordPads { juce::TextButton("I"), juce::TextButton("V"), juce::TextButton("vi"), juce::TextButton("IV") };
    PPGUThemeManager::Palette palette;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PPGUAutoChordsPanel)
};
