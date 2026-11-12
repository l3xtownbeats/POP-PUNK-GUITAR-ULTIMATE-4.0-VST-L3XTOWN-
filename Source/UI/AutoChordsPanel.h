#pragma once

#include <JuceHeader.h>
#include "ThemeManager.h"

class PPGUAutoChordsPanel final : public juce::Component
{
public:
    explicit PPGUAutoChordsPanel(const PPGUThemeManager::Palette& palette);
    void setPalette(const PPGUThemeManager::Palette& value);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    juce::Label title, note;
    juce::ComboBox mode, key, progression;
    juce::TextButton preview { "PREVIEW" };
    juce::Label modeLabel, keyLabel, progressionLabel;
    PPGUThemeManager::Palette palette;
};
