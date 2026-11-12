#pragma once

#include <JuceHeader.h>
#include <functional>
#include <memory>
#include <vector>
#include "ThemeManager.h"

class PPGUAuxiliaryDocumentWindow;

class PPGUAuxiliaryPanelWindowManager final
{
public:
    using PaletteUpdater = std::function<void(const PPGUThemeManager::Palette&)>;

    explicit PPGUAuxiliaryPanelWindowManager(
        const PPGUThemeManager::Palette& initialPalette,
        juce::PropertiesFile* persistentSettings = nullptr);
    ~PPGUAuxiliaryPanelWindowManager();

    bool addPanel(int id, const juce::String& title, juce::Component& content,
                  PaletteUpdater paletteUpdater = {});
    bool openPanel(int id);
    void closePanel(int id);
    void closeAll();
    bool isOpen(int id) const noexcept;
    int getOpenPanelCount() const noexcept;
    void setPalette(const PPGUThemeManager::Palette& value);

private:
    struct Entry;
    Entry* findEntry(int id) noexcept;
    const Entry* findEntry(int id) const noexcept;
    void toggleExpanded(int id);
    void updateLayout();
    void saveWindowState(const Entry& entry);

    PPGUThemeManager::Palette palette;
    juce::PropertiesFile* settings = nullptr;
    std::vector<std::unique_ptr<Entry>> entries;
    int expandedId = 0;
    int focusedId = 0;

    friend class PPGUAuxiliaryDocumentWindow;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PPGUAuxiliaryPanelWindowManager)
};
