#pragma once

#include <JuceHeader.h>
#include <functional>
#include "ThemeManager.h"

class PPGUEditorOverlayPanel final : public juce::Component
{
public:
    PPGUEditorOverlayPanel(const juce::String& title, juce::Component& content,
                           std::function<void()> onClose,
                           std::function<void(bool)> onExpand,
                           const PPGUThemeManager::Palette& palette);
    void setPalette(const PPGUThemeManager::Palette& value);
    void setExpanded(bool value);
    bool isExpanded() const noexcept { return expanded; }
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::Component& content;
    juce::Label titleLabel;
    juce::TextButton expandButton { "□" }, closeButton { "×" };
    std::function<void()> closeCallback;
    std::function<void(bool)> expandCallback;
    PPGUThemeManager::Palette palette;
    bool expanded = false;
};
