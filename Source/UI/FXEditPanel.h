#pragma once

#include <JuceHeader.h>
#include <memory>
#include <array>
#include <vector>
#include <utility>
#include "ThemeManager.h"

class PPGUFXEditPanel final : public juce::Component
{
public:
    PPGUFXEditPanel(juce::AudioProcessorValueTreeState& state,
                    const PPGUThemeManager::Palette& palette);
    void setPalette(const PPGUThemeManager::Palette& value);
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    struct Control
    {
        juce::String id, label;
        juce::Slider slider;
        juce::Label caption;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    struct Module
    {
        juce::String name, enabledId;
        std::vector<std::pair<juce::String, juce::String>> controls;
        juce::TextButton select;
        juce::TextButton enabled;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledAttachment;
    };
    void selectModule(int index);
    juce::AudioProcessorValueTreeState& state;
    PPGUThemeManager::Palette palette;
    std::array<Module, 8> modules;
    std::vector<std::unique_ptr<Control>> controls;
    int selectedModule = 0;
    juce::Label hint;
};
