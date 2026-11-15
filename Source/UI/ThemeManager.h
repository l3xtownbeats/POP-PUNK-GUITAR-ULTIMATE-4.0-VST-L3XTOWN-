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
            juce::Colour windowBg { 0xff100b18 };
            juce::Colour panel { 0xff1c1224 };
            juce::Colour panelElevated { 0xff291936 };
            juce::Colour controlBg { 0xff160f20 };
            juce::Colour controlFace { 0xff24182f };
            juce::Colour controlBorder { 0xff5a4568 };
            juce::Colour textPrimary { 0xfff7f3fb };
            juce::Colour textSecondary { 0xffc9bdd2 };
            juce::Colour textDisabled { 0xff8f829b };
            juce::Colour accent { 0xff2dd4ff };
            juce::Colour accentGlow { 0xff7ce8ff };
            juce::Colour danger { 0xffff5a66 };
            juce::Colour warning { 0xffffc857 };
            juce::Colour success { 0xff6ee7a8 };
            juce::Colour lcdBg { 0xff080d16 };
            juce::Colour lcdText { 0xffd9f7ff };
            juce::Colour ledOn { 0xff2dd4ff };
            juce::Colour ledOff { 0xff3b2b49 };
            juce::Colour shadow { 0xff05030a };
            juce::Colour highlight { 0xffb58bd8 };
            juce::Colour lowlight { 0xff150d1e };
        };

        juce::String id { "skin-053" };
        juce::String name { "NEON STAGE" };
        Tokens tokens;
        // Compatibility aliases used by the existing editor shell.
        juce::Colour canvas { 0xff100b18 };
        juce::Colour canvasInset { 0xff160f20 };
        juce::Colour surface { 0xff1c1224 };
        juce::Colour surfaceSecondary { 0xff160f20 };
        juce::Colour surfaceRaised { 0xff291936 };
        juce::Colour text { 0xfff7f3fb };
        juce::Colour textSecondary { 0xffc9bdd2 };
        juce::Colour accent { 0xff2dd4ff };
        juce::Colour accentSecondary { 0xff7ce8ff };
        juce::Colour border { 0xff5a4568 };
        juce::Colour focus { 0xffb58bd8 };
        bool usedFallback = true;

        juce::Colour token(const juce::String& name) const noexcept;
    };

    PPGUThemeManager() = default;

    // Parsing and file access are UI/control-thread operations only.
    // A failed load always restores the canonical NEON STAGE SKIN-1 palette.
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
