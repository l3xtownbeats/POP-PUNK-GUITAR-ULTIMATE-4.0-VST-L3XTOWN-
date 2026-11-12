#pragma once

#include <JuceHeader.h>
#include <memory>
#include <array>
#include <vector>

class PPGUFXEditPanel final : public juce::Component
{
public:
    explicit PPGUFXEditPanel(juce::AudioProcessorValueTreeState& state);
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
    std::array<Module, 7> modules;
    std::vector<std::unique_ptr<Control>> controls;
    int selectedModule = 0;
    juce::Label hint;
};
