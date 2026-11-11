#pragma once

#include <JuceHeader.h>

// Runtime contract for visual-only themes. The manager is intentionally isolated
// from PluginProcessor, APVTS, DSP, sampler and scheduler code.
class PPGUThemeManager final
{
public:
    struct Palette
    {
        juce::String id { "skin-51" };
        juce::String name { "Blackout Voltage" };
        juce::Colour canvas { 0xff090c10 };
        juce::Colour canvasInset { 0xff101820 };
        juce::Colour surface { 0xff17232b };
        juce::Colour surfaceSecondary { 0xff101820 };
        juce::Colour surfaceRaised { 0xff22313b };
        juce::Colour text { 0xfff4fafc };
        juce::Colour textSecondary { 0xffb5c4cb };
        juce::Colour accent { 0xffb7f34a };
        juce::Colour accentSecondary { 0xff35d7c5 };
        juce::Colour border { 0xff526875 };
        juce::Colour focus { 0xffb7f34a };
        bool usedFallback = true;
    };

    PPGUThemeManager() = default;

    // Parsing and file access are UI/control-thread operations only.
    // A failed load always restores the safe Blackout Voltage palette.
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
