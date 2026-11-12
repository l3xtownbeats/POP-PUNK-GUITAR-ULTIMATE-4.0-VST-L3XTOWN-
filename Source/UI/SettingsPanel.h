#pragma once
#include <JuceHeader.h>
#include <memory>
#include <vector>

class PPGUSettingsPanel final : public juce::Component
{
public:
    explicit PPGUSettingsPanel(juce::AudioProcessorValueTreeState& state);
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
    juce::Label title, description;
};
