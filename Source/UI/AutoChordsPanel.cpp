#include "AutoChordsPanel.h"

PPGUAutoChordsPanel::PPGUAutoChordsPanel()
{
    title.setText("AUTOCHORDS  /  HARMONY WORKSPACE", juce::dontSendNotification);
    title.setFont(juce::Font(17.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, juce::Colour(0xfff4f0f4));
    addAndMakeVisible(title);
    note.setText("Chord progression editor UI is prepared. MIDI chord-generation routing is not connected yet; Preview is intentionally disabled.", juce::dontSendNotification);
    note.setFont(juce::Font(11.0f));
    note.setColour(juce::Label::textColourId, juce::Colour(0xffaab2bf));
    addAndMakeVisible(note);
    mode.addItem("MAJOR", 1); mode.addItem("MINOR", 2); mode.addItem("CUSTOM", 3);
    key.addItem("C", 1); key.addItem("C#", 2); key.addItem("D", 3); key.addItem("D#", 4);
    key.addItem("E", 5); key.addItem("F", 6); key.addItem("F#", 7); key.addItem("G", 8);
    key.addItem("G#", 9); key.addItem("A", 10); key.addItem("A#", 11); key.addItem("B", 12);
    progression.addItem("I – V – vi – IV", 1);
    progression.addItem("I – vi – IV – V", 2);
    progression.addItem("vi – IV – I – V", 3);
    progression.addItem("I – IV – V – IV", 4);
    mode.setSelectedId(1); key.setSelectedId(1); progression.setSelectedId(1);
    for (auto* box : { &mode, &key, &progression })
    {
        box->setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff242a34));
        box->setColour(juce::ComboBox::textColourId, juce::Colour(0xfff4f0f4));
        box->setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff454e5b));
        addAndMakeVisible(*box);
    }
    auto configure = [](juce::Label& l, const juce::String& text)
    {
        l.setText(text, juce::dontSendNotification);
        l.setFont(juce::Font(10.0f, juce::Font::bold));
        l.setColour(juce::Label::textColourId, juce::Colour(0xffed238b));
    };
    configure(modeLabel, "SCALE"); configure(keyLabel, "KEY"); configure(progressionLabel, "PROGRESSION");
    addAndMakeVisible(modeLabel); addAndMakeVisible(keyLabel); addAndMakeVisible(progressionLabel);
    preview.setEnabled(false);
    preview.setTooltip("Disabled until AutoChords MIDI generation is connected to the performance engine.");
    addAndMakeVisible(preview);
}

void PPGUAutoChordsPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff171c24));
    g.setColour(juce::Colour(0xff202630));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(2.0f), 10.0f);
    g.setColour(juce::Colour(0xffed238b));
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.drawText("HARMONY SETUP", getLocalBounds().reduced(20, 68).removeFromTop(20), juce::Justification::left);
}

void PPGUAutoChordsPanel::resized()
{
    auto area = getLocalBounds().reduced(24);
    title.setBounds(area.removeFromTop(36));
    note.setBounds(area.removeFromTop(54));
    area.removeFromTop(12);
    auto row = area.removeFromTop(82);
    auto col = row.removeFromLeft(row.getWidth() / 3).reduced(8, 0);
    modeLabel.setBounds(col.removeFromTop(22)); mode.setBounds(col.removeFromTop(38));
    col = row.removeFromLeft(row.getWidth() / 2).reduced(8, 0);
    keyLabel.setBounds(col.removeFromTop(22)); key.setBounds(col.removeFromTop(38));
    col = row.reduced(8, 0);
    progressionLabel.setBounds(col.removeFromTop(22)); progression.setBounds(col.removeFromTop(38));
    preview.setBounds(area.removeFromTop(42).removeFromLeft(150));
}
