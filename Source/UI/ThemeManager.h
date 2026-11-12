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
            juce::Colour windowBg { 0xff090c10 };
            juce::Colour panel { 0xff17232b };
            juce::Colour panelElevated { 0xff22313b };
            juce::Colour controlBg { 0xff101820 };
            juce::Colour controlFace { 0xff22313b };
            juce::Colour controlBorder { 0xff526875 };
            juce::Colour textPrimary { 0xfff4fafc };
            juce::Colour textSecondary { 0xffb5c4cb };
            juce::Colour textDisabled { 0xff7c8b93 };
            juce::Colour accent { 0xffb7f34a };
            juce::Colour accentGlow { 0xff35d7c5 };
            juce::Colour danger { 0xffff5a66 };
            juce::Colour warning { 0xffffc857 };
            juce::Colour success { 0xff6ee7a8 };
            juce::Colour lcdBg { 0xff08110e };
            juce::Colour lcdText { 0xffd7ffb2 };
            juce::Colour ledOn { 0xffa8ff42 };
            juce::Colour ledOff { 0xff33443b };
            juce::Colour shadow { 0xff050708 };
            juce::Colour highlight { 0xff90a7a0 };
            juce::Colour lowlight { 0xff0d1417 };
        };

        juce::String id { "skin-51" };
        juce::String name { "Blackout Voltage" };
        Tokens tokens;
        // Compatibility aliases used by the existing editor shell.
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

        juce::Colour token(const juce::String& name) const noexcept;
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
