#pragma once

#include <JuceHeader.h>
#include "ThemeManager.h"
#include <array>

class PPGUAutoChordsPanel final : public juce::Component
{
public:
    explicit PPGUAutoChordsPanel(const PPGUThemeManager::Palette& palette);
    void setPalette(const PPGUThemeManager::Palette& value);
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void configureLabel(juce::Label&, const juce::String&, float, bool, juce::Colour);
    void drawCard(juce::Graphics&, juce::Rectangle<int>, const juce::String&, const juce::String&);
    juce::Label title, subtitle, note, presetLabel, keyLabel, scaleLabel;
    juce::Label styleTitle, rhythmTitle, statusTitle;
    juce::ComboBox key, scale, progression;
    juce::TextButton preview { "▶  PREVIEW" };
    std::array<juce::TextButton, 8> styleChoices {
        juce::TextButton("OPEN CHORDS"), juce::TextButton("PALM MUTE"),
        juce::TextButton("POWER CHORDS"), juce::TextButton("DOWNSTROKE"),
        juce::TextButton("ALTERNATE"), juce::TextButton("CHUGS"),
        juce::TextButton("SKANK"), juce::TextButton("ARPEGGIO")
    };
    std::array<juce::TextButton, 8> rhythmChoices {
        juce::TextButton("STRAIGHT 8THS"), juce::TextButton("FAST SKATE-PUNK"),
        juce::TextButton("GALLOP"), juce::TextButton("HALF-TIME"),
        juce::TextButton("STOP-START"), juce::TextButton("VERSE → CHORUS"),
        juce::TextButton("OFFBEAT"), juce::TextButton("OCTAVE HOOK")
    };
    PPGUThemeManager::Palette palette;
};
