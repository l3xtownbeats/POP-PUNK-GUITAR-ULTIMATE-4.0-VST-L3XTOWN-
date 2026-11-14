#pragma once

#include <JuceHeader.h>

// Runtime contract for visual-only themes. The manager is intentionally isolated
// from PluginProcessor, APVTS, DSP, sampler and scheduler code.
class PPGUThemeManager final
{
public:
    struct Palette
    {
        struct Tokens
        {
            juce::Colour windowBg { 0xff080d16 };
            juce::Colour panel { 0xff101b2b };
            juce::Colour panelElevated { 0xff17263a };
            juce::Colour controlBg { 0xff0b1421 };
            juce::Colour controlFace { 0xff17263a };
            juce::Colour controlBorder { 0xff28415d };
            juce::Colour textPrimary { 0xfff2f8ff };
            juce::Colour textSecondary { 0xffa9c1d8 };
            juce::Colour textDisabled { 0xff60758a };
            juce::Colour accent { 0xff00bfff };
            juce::Colour accentGlow { 0xff8a5cff };
            juce::Colour danger { 0xffff5d73 };
            juce::Colour warning { 0xffffd166 };
            juce::Colour success { 0xff39d98a };
            juce::Colour lcdBg { 0xff070c14 };
            juce::Colour lcdText { 0xfff2f8ff };
            juce::Colour ledOn { 0xff00bfff };
            juce::Colour ledOff { 0xff29445e };
            juce::Colour shadow { 0xff050810 };
            juce::Colour highlight { 0xff8a5cff };
            juce::Colour lowlight { 0xff0d1624 };
        };

        juce::String id { "skin-01" };
        juce::String name { "SIGNAL FORGE / NEON" };
        Tokens tokens;
        // Compatibility aliases used by the existing editor shell.
        juce::Colour canvas { 0xff080d16 };
        juce::Colour canvasInset { 0xff0b1421 };
        juce::Colour surface { 0xff101b2b };
        juce::Colour surfaceSecondary { 0xff0b1421 };
        juce::Colour surfaceRaised { 0xff17263a };
        juce::Colour text { 0xfff2f8ff };
        juce::Colour textSecondary { 0xffa9c1d8 };
        juce::Colour accent { 0xff00bfff };
        juce::Colour accentSecondary { 0xff8a5cff };
        juce::Colour border { 0xff28415d };
        juce::Colour focus { 0xff00bfff };
        bool usedFallback = true;

        juce::Colour token(const juce::String& name) const noexcept;
    };

    PPGUThemeManager() = default;

    // Parsing and file access are UI/control-thread operations only.
    // A failed load always restores the approved SIGNAL FORGE / NEON fallback.
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
