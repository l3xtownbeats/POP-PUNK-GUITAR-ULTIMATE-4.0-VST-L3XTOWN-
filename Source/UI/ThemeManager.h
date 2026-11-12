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
            juce::Colour windowBg { 0xff0b0f12 };
            juce::Colour panel { 0xff141b20 };
            juce::Colour panelElevated { 0xff1d2930 };
            juce::Colour controlBg { 0xff10171b };
            juce::Colour controlFace { 0xff26343a };
            juce::Colour controlBorder { 0xff53656b };
            juce::Colour textPrimary { 0xfff3f7f5 };
            juce::Colour textSecondary { 0xffb7c5c2 };
            juce::Colour textDisabled { 0xff788984 };
            juce::Colour accent { 0xffd5ff45 };
            juce::Colour accentGlow { 0xffb7ff3c };
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

        juce::String id { "skin-052" };
        juce::String name { "SIGNAL FORGE" };
        Tokens tokens;
        // Compatibility aliases used by the existing editor shell.
        juce::Colour canvas { 0xff0b0f12 };
        juce::Colour canvasInset { 0xff10171b };
        juce::Colour surface { 0xff141b20 };
        juce::Colour surfaceSecondary { 0xff10171b };
        juce::Colour surfaceRaised { 0xff1d2930 };
        juce::Colour text { 0xfff3f7f5 };
        juce::Colour textSecondary { 0xffb7c5c2 };
        juce::Colour accent { 0xffd5ff45 };
        juce::Colour accentSecondary { 0xffb7ff3c };
        juce::Colour border { 0xff53656b };
        juce::Colour focus { 0xff90a7a0 };
        bool usedFallback = true;

        juce::Colour token(const juce::String& name) const noexcept;
    };

    PPGUThemeManager() = default;

    // Parsing and file access are UI/control-thread operations only.
    // A failed load always restores the canonical SIGNAL FORGE palette.
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
