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
            juce::Colour windowBg { 0xff050811 };
            juce::Colour panel { 0xff091321 };
            juce::Colour panelElevated { 0xff101f32 };
            juce::Colour controlBg { 0xff07101c };
            juce::Colour controlFace { 0xff14263a };
            juce::Colour controlBorder { 0xff52718b };
            juce::Colour textPrimary { 0xffffffff };
            juce::Colour textSecondary { 0xffd9e8f7 };
            juce::Colour textDisabled { 0xffaabbd0 };
            juce::Colour accent { 0xff66ffff };
            juce::Colour accentGlow { 0xffb8ffff };
            juce::Colour danger { 0xffff5577 };
            juce::Colour warning { 0xffffd166 };
            juce::Colour success { 0xff67f5b5 };
            juce::Colour lcdBg { 0xff020a12 };
            juce::Colour lcdText { 0xffc9ffff };
            juce::Colour ledOn { 0xff66ffff };
            juce::Colour ledOff { 0xff263c50 };
            juce::Colour shadow { 0xff02040a };
            juce::Colour highlight { 0xffb8d9f0 };
            juce::Colour lowlight { 0xff0a1421 };
        };

        juce::String id { "skin-053" };
        juce::String name { "NEON STAGE" };
        Tokens tokens;
        // Compatibility aliases used by the existing editor shell.
        juce::Colour canvas { 0xff050811 };
        juce::Colour canvasInset { 0xff07101c };
        juce::Colour surface { 0xff091321 };
        juce::Colour surfaceSecondary { 0xff07101c };
        juce::Colour surfaceRaised { 0xff101f32 };
        juce::Colour text { 0xffffffff };
        juce::Colour textSecondary { 0xffd9e8f7 };
        juce::Colour accent { 0xff66ffff };
        juce::Colour accentSecondary { 0xffb8ffff };
        juce::Colour border { 0xff52718b };
        juce::Colour focus { 0xffb8d9f0 };
        bool usedFallback = true;

        juce::Colour token(const juce::String& name) const noexcept;
    };

    PPGUThemeManager() = default;

    // Parsing and file access are UI/control-thread operations only.
    // A failed load always restores the canonical NEON STAGE palette.
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
