#pragma once
#include <JuceHeader.h>
#include <functional>

class PPGUImportGuitarPanel final : public juce::Component
{
public:
    explicit PPGUImportGuitarPanel(std::function<void()> browseCallback);
    void setStatus(const juce::String& text);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    juce::Label title, description, status;
    juce::TextButton browse { "BROWSE SFZ LIBRARY" };
    std::function<void()> browseCallback;
};
