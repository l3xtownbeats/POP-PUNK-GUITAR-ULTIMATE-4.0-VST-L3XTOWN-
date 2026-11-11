#pragma once

#include <JuceHeader.h>

// Runtime contract for visual-only themes. The manager is intentionally isolated
// from PluginProcessor, APVTS, DSP, sampler and scheduler code.
class PPGUThemeManager final
{
public:
    struct Palette
    {
        juce::String id { "skin-01" };
        juce::String name { "Neon Riot" };
        juce::Colour canvas { 0xff111116 };
        juce::Colour canvasInset { 0xff17171d };
        juce::Colour surface { 0xff202027 };
        juce::Colour surfaceSecondary { 0xff17171d };
        juce::Colour surfaceRaised { 0xff292932 };
        juce::Colour text { 0xfff4f0f4 };
        juce::Colour textSecondary { 0xffaaa8b2 };
        juce::Colour accent { 0xffed238b };
        juce::Colour accentSecondary { 0xff39d8e8 };
        juce::Colour border { 0xff49434f };
        juce::Colour focus { 0xffed238b };
        bool usedFallback = true;
    };

    PPGUThemeManager() = default;

    // Parsing and file access are UI/control-thread operations only.
    // A failed load always restores the safe Neon Riot palette.
    bool loadFromJson(const juce::String& jsonText);
    bool loadFromFile(const juce::File& manifestFile);
    void resetToDefault() noexcept;

    const Palette& getPalette() const noexcept { return activePalette; }
    juce::String getLastError() const { return lastError; }

private:
    static bool readColour(const juce::var& object, const juce::Identifier& key,
                           juce::Colour& destination);
    static bool validateManifest(const juce::var& root, Palette& palette,
                                 juce::String& error);

    Palette activePalette;
    juce::String lastError;
};
