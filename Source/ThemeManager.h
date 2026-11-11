#pragma once

#include <JuceHeader.h>
#include <map>

namespace ppgu
{
struct ThemePalette
{
    std::map<juce::String, juce::Colour> colours;
    juce::Colour get (const juce::String& key, juce::Colour fallback) const;
};

struct Theme
{
    juce::String id, name, author, version;
    juce::StringArray styleTags;
    ThemePalette palette;
    std::map<juce::String, juce::File> assets;
    std::map<juce::String, juce::String> fonts;
    juce::File rootDir;
};

// Reads the supplied catalog JSON. Theme changes affect UI only; this class
// never touches the audio processor or audio thread.
class ThemeManager final : private juce::AsyncUpdater
{
public:
    ThemeManager() = default;
    bool loadCatalogText (const juce::String& jsonText, const juce::File& assetRoot = {});
    // Loads one theme.json from each immediate child directory. This matches
    // the shipped Themes/<theme-id>/theme.json folder layout.
    bool loadFromDirectory (const juce::File& themesDirectory);
    const std::vector<std::unique_ptr<Theme>>& availableThemes() const noexcept { return themes; }
    Theme* current() const noexcept { return currentTheme; }
    int currentIndex() const noexcept;
    int indexOf (const juce::String& id) const noexcept;
    juce::String defaultThemeId() const { return defaultId; }
    void applyTheme (const juce::String& id);
    void applyTheme (int index);
    std::function<void()> onThemeChanged;

private:
    void handleAsyncUpdate() override;
    std::vector<std::unique_ptr<Theme>> themes;
    Theme* currentTheme = nullptr;
    juce::String defaultId { "neon_riot" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemeManager)
};

class PpguLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void setThemeManager (ThemeManager* managerIn) noexcept { manager = managerIn; }
    void drawRotarySlider (juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    void drawLinearSlider (juce::Graphics&, int, int, int, int, float, float, float,
                           juce::Slider::SliderStyle, juce::Slider&) override;
private:
    juce::Colour colour (const juce::String& key, juce::Colour fallback) const;
    ThemeManager* manager = nullptr;
};
} // namespace ppgu
