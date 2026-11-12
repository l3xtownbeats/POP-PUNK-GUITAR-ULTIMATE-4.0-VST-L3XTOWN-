#pragma once
#include <JuceHeader.h>
#include <memory>
#include <vector>
#include "ThemeManager.h"

class PPGUSettingsPanel final : public juce::Component
{
public:
    PPGUSettingsPanel(juce::AudioProcessorValueTreeState& state,
                      const PPGUThemeManager::Palette& palette);
    void setPalette(const PPGUThemeManager::Palette& value);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    struct Control
    {
        juce::String id, name;
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    std::vector<std::unique_ptr<Control>> controls;
    PPGUThemeManager::Palette palette;
    juce::Label title, description;
};
