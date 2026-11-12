#pragma once

#include <JuceHeader.h>
#include <functional>
#include <memory>
#include <vector>
#include "EditorOverlayPanel.h"
#include "AuxiliaryPanelLayout.h"

// Owns in-editor auxiliary windows. All operations are message-thread only;
// the manager never references processor or audio-thread state.
class PPGUAuxiliaryPanelWindowManager final : public juce::Component
{
public:
    explicit PPGUAuxiliaryPanelWindowManager(const PPGUThemeManager::Palette& palette);

    bool addPanel(int id, const juce::String& title, juce::Component& content);
    bool openPanel(int id);
    void closePanel(int id);
    void closeAll();
    bool isOpen(int id) const noexcept;
    int getOpenPanelCount() const noexcept;
    void setPalette(const PPGUThemeManager::Palette& value);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    struct Window
    {
        int id = 0;
        juce::Component* content = nullptr;
        std::unique_ptr<PPGUEditorOverlayPanel> overlay;
    };

    Window* findWindow(int id) noexcept;
    const Window* findWindow(int id) const noexcept;
    void updateLayout();

    PPGUThemeManager::Palette palette;
    std::vector<Window> windows;
    int expandedId = 0;
    bool updatingLayout = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PPGUAuxiliaryPanelWindowManager)
};
