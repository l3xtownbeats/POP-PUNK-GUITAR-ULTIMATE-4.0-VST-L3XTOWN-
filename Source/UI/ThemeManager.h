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
            juce::Colour windowBg { 0xff080b14 };
            juce::Colour panel { 0xff101827 };
            juce::Colour panelElevated { 0xff17243a };
            juce::Colour controlBg { 0xff0c1422 };
            juce::Colour controlFace { 0xff1a2940 };
            juce::Colour controlBorder { 0xff31506f };
            juce::Colour textPrimary { 0xfff4f7ff };
            juce::Colour textSecondary { 0xffb5c4dc };
            juce::Colour textDisabled { 0xff71829d };
            juce::Colour accent { 0xff00d9ff };
            juce::Colour accentGlow { 0xff6b8cff };
            juce::Colour danger { 0xffff557a };
            juce::Colour warning { 0xffffbe55 };
            juce::Colour success { 0xff42e6b0 };
            juce::Colour lcdBg { 0xff050d1b };
            juce::Colour lcdText { 0xff8debff };
            juce::Colour ledOn { 0xff00e5ff };
            juce::Colour ledOff { 0xff263b55 };
            juce::Colour shadow { 0xff03060b };
            juce::Colour highlight { 0xff8dcbff };
            juce::Colour lowlight { 0xff080f1b };
        };

        juce::String id { "skin-053" };
        juce::String name { "NEON FORGE (DEFAULT)" };
        juce::String uiTypeface { "Segoe UI" };
        juce::String displayTypeface { "Consolas" };
        juce::String typographyWeight { "semibold" };
        float fontScale = 1.0f;
        float letterSpacing = 0.35f;
        Tokens tokens;
        // Compatibility aliases used by the existing editor shell.
        juce::Colour canvas { 0xff080b14 };
        juce::Colour canvasInset { 0xff0c1422 };
        juce::Colour surface { 0xff101827 };
        juce::Colour surfaceSecondary { 0xff0c1422 };
        juce::Colour surfaceRaised { 0xff17243a };
        juce::Colour text { 0xfff4f7ff };
        juce::Colour textSecondary { 0xffb5c4dc };
        juce::Colour accent { 0xff00d9ff };
        juce::Colour accentSecondary { 0xff6b8cff };
        juce::Colour border { 0xff31506f };
        juce::Colour focus { 0xff8dcbff };
        bool usedFallback = true;

        juce::Colour token(const juce::String& name) const noexcept;
    };

    PPGUThemeManager() = default;

    // Parsing and file access are UI/control-thread operations only.
    // A failed load always restores the canonical NEON FORGE (DEFAULT) palette.
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
