#include "ImportGuitarPanel.h"
#include <utility>

PPGUImportGuitarPanel::PPGUImportGuitarPanel(std::function<void()> callback,
                                             const PPGUThemeManager::Palette& initialPalette)
    : browseCallback(std::move(callback)), palette(initialPalette)
{
    title.setText("IMPORT GUITAR LIBRARY", juce::dontSendNotification);
    title.setFont(juce::Font(18.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, juce::Colour(0xfff4f0f4));
    addAndMakeVisible(title);
    description.setText("Load a licensed SFZ guitar instrument. The sampler validates regions and WAV references, then loads the library asynchronously.", juce::dontSendNotification);
    description.setFont(juce::Font(12.0f));
    description.setColour(juce::Label::textColourId, juce::Colour(0xffaab2bf));
    addAndMakeVisible(description);
    status.setText("READY  /  SELECT AN SFZ FILE", juce::dontSendNotification);
    status.setFont(juce::Font(11.0f, juce::Font::bold));
    status.setColour(juce::Label::textColourId, juce::Colour(0xffed238b));
    addAndMakeVisible(status);
    browse.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffed238b));
    browse.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    browse.onClick = [this] { if (browseCallback) browseCallback(); };
    addAndMakeVisible(browse);
    setPalette(palette);
}

void PPGUImportGuitarPanel::setPalette(const PPGUThemeManager::Palette& value)
{
    palette = value;
    title.setColour(juce::Label::textColourId, palette.token("text_primary"));
    description.setColour(juce::Label::textColourId, palette.token("text_secondary"));
    status.setColour(juce::Label::textColourId, palette.token("accent"));
    browse.setColour(juce::TextButton::buttonColourId, palette.token("accent"));
    browse.setColour(juce::TextButton::textColourOffId, palette.token("window_bg"));
    repaint();
}
void PPGUImportGuitarPanel::setStatus(const juce::String& text) { status.setText(text, juce::dontSendNotification); }
void PPGUImportGuitarPanel::paint(juce::Graphics& g)
{
    g.fillAll(palette.token("window_bg"));
    g.setColour(palette.token("panel"));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(2.0f), 10.0f);
    g.setColour(palette.token("accent"));
    g.fillRoundedRectangle(22.0f, 68.0f, 54.0f, 3.0f, 1.5f);
}
void PPGUImportGuitarPanel::resized()
{
    const bool compact = getHeight() < 260;
    auto a = getLocalBounds().reduced(compact ? 12 : 26);
    title.setBounds(a.removeFromTop(compact ? 28 : 42));
    description.setBounds(a.removeFromTop(compact ? 46 : 70));
    a.removeFromTop(compact ? 6 : 16);
    browse.setBounds(a.removeFromTop(compact ? 34 : 46).removeFromLeft(240));
    status.setBounds(a.removeFromTop(compact ? 28 : 38));
}
