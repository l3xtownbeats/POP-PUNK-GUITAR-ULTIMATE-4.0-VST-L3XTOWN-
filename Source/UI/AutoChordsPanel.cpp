#include "AutoChordsPanel.h"

namespace
{
    constexpr int cardRadius = 12;
}

PPGUAutoChordsPanel::PPGUAutoChordsPanel(const PPGUThemeManager::Palette& initialPalette)
    : palette(initialPalette)
{
    configureLabel(title, "AUTOCHORDS", 22.0f, true, palette.textPrimary);
    configureLabel(subtitle, "HARMONY & GUITAR PERFORMANCE WORKSPACE", 10.0f, true, palette.accent);
    configureLabel(note, "Choose a chord progression on the left. Playing technique and groove are separate choices on the right.", 11.0f, false, palette.textSecondary);
    configureLabel(presetLabel, "CHORD / PROGRESSION PRESET", 10.0f, true, palette.accent);
    configureLabel(keyLabel, "KEY", 9.0f, true, palette.textSecondary);
    configureLabel(scaleLabel, "SCALE", 9.0f, true, palette.textSecondary);
    configureLabel(styleTitle, "01  PLAYING STYLE  /  ARTICULATION", 12.0f, true, palette.textPrimary);
    configureLabel(rhythmTitle, "02  RHYTHM PATTERN", 12.0f, true, palette.textPrimary);
    configureLabel(statusTitle, "ENGINE STATUS  •  MIDI GENERATION NOT CONNECTED", 9.0f, true, palette.warning);

    progression.addItem("I – V – vi – IV   •   POP PUNK", 1);
    progression.addItem("I – vi – IV – V   •   CLASSIC", 2);
    progression.addItem("vi – IV – I – V   •   EMOTIVE", 3);
    progression.addItem("I – IV – V – IV   •   ANTHEMIC", 4);
    progression.setSelectedId(1, juce::dontSendNotification);

    const juce::StringArray keys { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    for (int i = 0; i < keys.size(); ++i) key.addItem(keys[i], i + 1);
    key.setSelectedId(1, juce::dontSendNotification);
    scale.addItem("MAJOR", 1);
    scale.addItem("MINOR", 2);
    scale.addItem("CUSTOM", 3);
    scale.setSelectedId(1, juce::dontSendNotification);

    for (auto* box : { &key, &scale, &progression })
        addAndMakeVisible(*box);

    for (auto& button : styleChoices)
        addAndMakeVisible(button);
    for (auto& button : rhythmChoices)
        addAndMakeVisible(button);

    // These are intentionally presented as a visual preview only until the
    // chord/performance event routing is implemented and acceptance-tested.
    for (auto& button : styleChoices) {
        button.setEnabled(false);
        button.setTooltip("Unavailable until a compatible recorded performance map is connected.");
    }
    for (auto& button : rhythmChoices) {
        button.setEnabled(false);
        button.setTooltip("Unavailable until AutoChords pattern playback is connected.");
    }
    for (auto* box : { &key, &scale, &progression }) box->setEnabled(false);
    preview.setEnabled(false);
    preview.setTooltip("Preview will be enabled after MIDI chord and performance routing is implemented.");
    addAndMakeVisible(preview);
    addAndMakeVisible(title);
    addAndMakeVisible(subtitle);
    addAndMakeVisible(note);
    addAndMakeVisible(presetLabel);
    addAndMakeVisible(keyLabel);
    addAndMakeVisible(scaleLabel);
    addAndMakeVisible(styleTitle);
    addAndMakeVisible(rhythmTitle);
    addAndMakeVisible(statusTitle);
    setPalette(initialPalette);
}

void PPGUAutoChordsPanel::configureLabel(juce::Label& label, const juce::String& text,
                                         float size, bool bold, juce::Colour colour)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(size, bold ? juce::Font::bold : juce::Font::plain));
    label.setColour(juce::Label::textColourId, colour);
    label.setJustificationType(juce::Justification::centredLeft);
}

void PPGUAutoChordsPanel::setPalette(const PPGUThemeManager::Palette& value)
{
    palette = value;
    for (auto* label : { &title, &subtitle, &note, &presetLabel, &keyLabel, &scaleLabel,
                         &styleTitle, &rhythmTitle, &statusTitle })
    {
        if (label == &title) label->setColour(juce::Label::textColourId, palette.textPrimary);
        else if (label == &subtitle || label == &presetLabel) label->setColour(juce::Label::textColourId, palette.accent);
        else if (label == &statusTitle) label->setColour(juce::Label::textColourId, palette.warning);
        else if (label == &styleTitle || label == &rhythmTitle) label->setColour(juce::Label::textColourId, palette.textPrimary);
        else label->setColour(juce::Label::textColourId, palette.textSecondary);
    }

    for (auto* box : { &key, &scale, &progression }) {
        box->setColour(juce::ComboBox::backgroundColourId, palette.controlBg);
        box->setColour(juce::ComboBox::textColourId, palette.textPrimary);
        box->setColour(juce::ComboBox::outlineColourId, palette.controlBorder);
        box->setColour(juce::ComboBox::arrowColourId, palette.accent);
    }
    for (auto& button : styleChoices) {
        button.setColour(juce::TextButton::buttonColourId, palette.controlBg);
        button.setColour(juce::TextButton::textColourOffId, palette.textDisabled);
    }
    for (auto& button : rhythmChoices) {
        button.setColour(juce::TextButton::buttonColourId, palette.controlBg);
        button.setColour(juce::TextButton::textColourOffId, palette.textDisabled);
    }
    preview.setColour(juce::TextButton::buttonColourId, palette.accent);
    preview.setColour(juce::TextButton::textColourOffId, palette.windowBg);
    repaint();
}

void PPGUAutoChordsPanel::drawCard(juce::Graphics& g, juce::Rectangle<int> area,
                                   const juce::String& eyebrow, const juce::String& detail)
{
    auto r = area.toFloat();
    g.setColour(palette.shadow.withAlpha(0.42f));
    g.fillRoundedRectangle(r.translated(0.0f, 3.0f), (float)cardRadius);
    g.setGradientFill(juce::ColourGradient(palette.panelElevated, r.getX(), r.getY(),
                                           palette.panel, r.getRight(), r.getBottom(), false));
    g.fillRoundedRectangle(r, (float)cardRadius);
    g.setColour(palette.controlBorder.withAlpha(0.72f));
    g.drawRoundedRectangle(r.reduced(0.5f), (float)cardRadius, 1.0f);
    g.setColour(palette.accentGlow);
    g.fillRoundedRectangle(r.getX() + 14.0f, r.getY() + 1.0f, 34.0f, 2.0f, 1.0f);
    auto text = area.reduced(16, 10);
    g.setColour(palette.accent);
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText(eyebrow, text.removeFromTop(14), juce::Justification::left);
    g.setColour(palette.textSecondary);
    g.setFont(juce::Font(9.0f));
    g.drawText(detail, text.removeFromTop(13), juce::Justification::left);
}

void PPGUAutoChordsPanel::paint(juce::Graphics& g)
{
    g.fillAll(palette.windowBg);
    auto bounds = getLocalBounds().reduced(14);
    g.setColour(palette.lowlight.withAlpha(0.5f));
    for (int x = 0; x < getWidth(); x += 28) g.drawVerticalLine(x, 0.0f, (float)getHeight());
    for (int y = 0; y < getHeight(); y += 28) g.drawHorizontalLine(y, 0.0f, (float)getWidth());

    auto header = bounds.removeFromTop(70);
    drawCard(g, header, "L3XTOWN AUDIO LAB  /  PERFORMANCE DESIGN", "HARMONY  •  ARTICULATION  •  GROOVE");
    auto body = bounds;
    const int gap = 12;
    const int leftWidth = juce::jlimit(250, 360, (body.getWidth() - gap) * 34 / 100);
    auto left = body.removeFromLeft(leftWidth);
    body.removeFromLeft(gap);
    drawCard(g, left, "A  /  HARMONY PRESETS", "Progression and tonal centre");
    drawCard(g, body, "B  /  GUITAR PERFORMANCE", "Playing style and rhythm are independent layers");
}

void PPGUAutoChordsPanel::resized()
{
    auto area = getLocalBounds().reduced(28);
    area.removeFromTop(70);
    area.removeFromTop(8);
    const int gap = 12;
    const int leftWidth = juce::jlimit(250, 360, (area.getWidth() - gap) * 34 / 100);
    auto left = area.removeFromLeft(leftWidth).reduced(16, 12);
    area.removeFromLeft(gap);
    auto right = area.reduced(16, 12);

    title.setBounds(28, 20, getWidth() - 56, 30);
    subtitle.setBounds(30, 49, getWidth() - 60, 16);
    note.setBounds(30, 76, getWidth() - 60, 30);

    presetLabel.setBounds(left.removeFromTop(24));
    progression.setBounds(left.removeFromTop(42));
    left.removeFromTop(16);
    auto keyRow = left.removeFromTop(54);
    auto keyCol = keyRow.removeFromLeft(keyRow.getWidth() / 2).reduced(0, 2);
    keyLabel.setBounds(keyCol.removeFromTop(16));
    key.setBounds(keyCol.removeFromTop(34));
    keyCol = keyRow.reduced(8, 2);
    scaleLabel.setBounds(keyCol.removeFromTop(16));
    scale.setBounds(keyCol.removeFromTop(34));
    preview.setBounds(left.removeFromTop(40).withWidth(150));
    statusTitle.setBounds(left.removeFromBottom(38));

    styleTitle.setBounds(right.removeFromTop(26));
    const int chipGap = 7;
    const int chipW = (right.getWidth() - chipGap * 3) / 4;
    const int chipH = juce::jlimit(32, 42, (right.getHeight() - 100) / 5);
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 4; ++col) {
            const int index = row * 4 + col;
            styleChoices[(size_t)index].setBounds(right.getX() + col * (chipW + chipGap),
                                                   right.getY() + row * (chipH + 7), chipW, chipH);
        }
    }
    right.removeFromTop(chipH * 2 + 18);
    rhythmTitle.setBounds(right.removeFromTop(26));
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 4; ++col) {
            const int index = row * 4 + col;
            rhythmChoices[(size_t)index].setBounds(right.getX() + col * (chipW + chipGap),
                                                    right.getY() + row * (chipH + 7), chipW, chipH);
        }
    }
}
