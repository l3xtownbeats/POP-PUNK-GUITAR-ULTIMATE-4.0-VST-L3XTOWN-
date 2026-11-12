#pragma once

#include <JuceHeader.h>
#include "AuxiliaryPanelLayout.h"
#include "ThemeManager.h"

#include <memory>
#include <vector>

class PPGUThemeAwarePanel
{
public:
    virtual ~PPGUThemeAwarePanel() = default;
    virtual void setThemePalette(const PPGUThemeManager::Palette& palette) = 0;
};

// JUCE child-panel workspace. Panels are independent, closable views hosted
// inside the plugin editor; this component does not own processor/audio state.
class PPGUMultiWindowWorkspace final : public juce::Component
{
public:
    struct PanelSpec
    {
        int id = 0;
        juce::String title;
        std::unique_ptr<juce::Component> content;
        ppgu::ui::PanelSize desired { 520, 360 };
        ppgu::ui::PanelSize minimum { 280, 190 };
    };

    PPGUMultiWindowWorkspace();
    ~PPGUMultiWindowWorkspace() override;

    bool openPanel(PanelSpec spec);
    bool closePanel(int id);
    void closeAllPanels();
    bool isPanelOpen(int id) const noexcept;
    int getOpenPanelCount() const noexcept;

    void setPanelGap(int pixels);
    void setPanelScale(float scale);
    void setPersistenceProperties(juce::PropertiesFile* properties);
    void setThemePalette(const PPGUThemeManager::Palette& palette);
    void setSurfaceColours(juce::Colour background,
                           juce::Colour panel,
                           juce::Colour border,
                           juce::Colour heading,
                           juce::Colour text);

    void resized() override;
    void paint(juce::Graphics&) override;

private:
    class Window;
    class TileSurface;

    void handleClose(int id);
    void handleGeometryChanged(int id);
    juce::String geometryKey(int id, const juce::String& suffix) const;
    void reorderPanels(int draggedId, int targetId);
    void layoutPanels();

    juce::Viewport viewport;
    std::unique_ptr<TileSurface> tileSurface;
    std::vector<std::unique_ptr<Window>> windows;
    int panelGap = 12;
    std::unique_ptr<juce::PropertiesFile> ownedProperties;
    juce::PropertiesFile* persistenceProperties = nullptr;
    int nextCascadeOffset = 0;
    float panelScale = 1.0f;
    bool layingOutPanels = false;
    bool hasThemePalette = false;
    PPGUThemeManager::Palette currentThemePalette;
    juce::Colour backgroundColour { 0xff090c10 };
    juce::Colour panelColour { 0xff17232b };
    juce::Colour borderColour { 0xff526875 };
    juce::Colour headingColour { 0xffb7f34a };
    juce::Colour textColour { 0xfff4fafc };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PPGUMultiWindowWorkspace)
};
