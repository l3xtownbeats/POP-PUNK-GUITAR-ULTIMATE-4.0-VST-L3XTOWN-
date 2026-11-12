#pragma once
#include <JuceHeader.h>
#include <functional>
#include "ThemeManager.h"

class PPGUImportGuitarPanel final : public juce::Component
{
public:
    PPGUImportGuitarPanel(std::function<void()> browseCallback,
                          const PPGUThemeManager::Palette& palette);
    void setPalette(const PPGUThemeManager::Palette& value);
    void setStatus(const juce::String& text);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    juce::Label title, description, status;
    juce::TextButton browse { "BROWSE SFZ LIBRARY" };
    std::function<void()> browseCallback;
    PPGUThemeManager::Palette palette;
};
