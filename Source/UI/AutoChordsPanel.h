#pragma once

#include <JuceHeader.h>

class PPGUAutoChordsPanel final : public juce::Component
{
public:
    PPGUAutoChordsPanel();
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    juce::Label title, note;
    juce::ComboBox mode, key, progression;
    juce::TextButton preview { "PREVIEW" };
    juce::Label modeLabel, keyLabel, progressionLabel;
};
