#pragma once

#include <JuceHeader.h>

// Data-only skin contract. All parsing/application must happen on the message thread;
// this class is deliberately independent of the audio processor and audio callback.
class PPGUThemeManager final
{
public:
    struct Palette
    {
        juce::String id { "skin-01" };
        juce::String name { "Neon Riot" };
        juce::Colour canvas { 0xff111116 };
        juce::Colour surface { 0xff202027 };
        juce::Colour surfaceSecondary { 0xff17171d };
        juce::Colour text { 0xfff4f0f4 };
        juce::Colour textSecondary { 0xffaaa8b2 };
        juce::Colour accent { 0xffed238b };
        juce::Colour accentSecondary { 0xff39d8e8 };
        juce::Colour border { 0xff49434f };
        bool usedFallback = true;
    };

    PPGUThemeManager() = default;

    // Returns true only when a complete, compatible theme contract is accepted.
    // On any parse/contract error, the active palette is reset to Neon Riot.
    bool loadFromJson(const juce::String& jsonText);
    void resetToDefault() noexcept;

    const Palette& getPalette() const noexcept { return activePalette; }
    juce::String getLastError() const { return lastError; }

private:
    static bool readColour(const juce::var& object, const juce::Identifier& key,
                           juce::Colour& destination);
    static bool hasRequiredTokenGroups(const juce::var& tokens);

    Palette activePalette;
    juce::String lastError;
};
