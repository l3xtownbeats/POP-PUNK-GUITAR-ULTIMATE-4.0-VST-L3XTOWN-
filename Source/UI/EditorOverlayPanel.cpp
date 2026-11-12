#include "EditorOverlayPanel.h"

PPGUEditorOverlayPanel::PPGUEditorOverlayPanel(const juce::String& title, juce::Component& child,
                                               std::function<void()> onClose,
                                               std::function<void(bool)> onExpand,
                                               const PPGUThemeManager::Palette& initialPalette)
    : content(child), closeCallback(std::move(onClose)), expandCallback(std::move(onExpand)),
      palette(initialPalette)
{
    titleLabel.setText(title, juce::dontSendNotification);
    titleLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff4f0f4));
    addAndMakeVisible(titleLabel);
    for (auto* button : { &expandButton, &closeButton })
    {
        button->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff252a33));
        button->setColour(juce::TextButton::textColourOffId, juce::Colour(0xfff4f0f4));
        addAndMakeVisible(*button);
    }
    expandButton.setTooltip("Expand / restore panel");
    closeButton.setTooltip("Close panel");
    expandButton.onClick = [this] { expanded = !expanded; expandButton.setButtonText(expanded ? "□" : "▣"); if (expandCallback) expandCallback(expanded); };
    closeButton.onClick = [this] { if (closeCallback) closeCallback(); };
    addAndMakeVisible(content);
    setPalette(palette);
}

void PPGUEditorOverlayPanel::setExpanded(bool value)
{
    expanded = value;
    expandButton.setButtonText(expanded ? "▣" : "□");
}

void PPGUEditorOverlayPanel::setPalette(const PPGUThemeManager::Palette& value)
{
    palette = value;
    titleLabel.setColour(juce::Label::textColourId, palette.token("text_primary"));
    for (auto* button : { &expandButton, &closeButton })
    {
        button->setColour(juce::TextButton::buttonColourId, palette.token("control_bg"));
        button->setColour(juce::TextButton::textColourOffId, palette.token("text_primary"));
    }
    repaint();
}

void PPGUEditorOverlayPanel::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setColour(palette.token("panel"));
    g.fillRoundedRectangle(b, 12.0f);
    g.setColour(palette.token("control_border"));
    g.drawRoundedRectangle(b.reduced(0.5f), 12.0f, 1.0f);
    g.setColour(palette.token("accent"));
    g.fillRoundedRectangle(b.getX() + 16.0f, b.getY() + 1.0f, 48.0f, 2.0f, 1.0f);
    g.setColour(palette.token("panel_elevated"));
    g.fillRoundedRectangle(b.getX() + 1.0f, b.getY() + 1.0f, b.getWidth() - 2.0f, 42.0f, 10.0f);
}

void PPGUEditorOverlayPanel::resized()
{
    auto head = getLocalBounds().reduced(10, 7).removeFromTop(30);
    titleLabel.setBounds(head.removeFromLeft(head.getWidth() - 78));
    closeButton.setBounds(head.removeFromRight(32));
    head.removeFromRight(5);
    expandButton.setBounds(head.removeFromRight(32));
    content.setBounds(getLocalBounds().reduced(12).withTrimmedTop(46));
}
