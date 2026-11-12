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
    juce::String getOpenPanelIds() const;
    void restoreOpenPanels(const juce::String& ids);
    void setPalette(const PPGUThemeManager::Palette& value);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    struct Window
    {
        int id = 0;
        juce::Component* content = nullptr;
        bool open = false;
        std::unique_ptr<PPGUEditorOverlayPanel> overlay;
    };

    Window* findWindow(int id) noexcept;
    const Window* findWindow(int id) const noexcept;
    void updateLayout();
    void changePage(int delta);
    int getPageCount() const noexcept;

    PPGUThemeManager::Palette palette;
    std::vector<Window> windows;
    int expandedId = 0;
    int currentPage = 0;
    bool updatingLayout = false;
    juce::TextButton previousPageButton { "‹" }, nextPageButton { "›" };
    juce::Label pageIndicator;
    static constexpr int panelsPerPage = 6;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PPGUAuxiliaryPanelWindowManager)
};
