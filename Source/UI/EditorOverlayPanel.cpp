#include "EditorOverlayPanel.h"

#include <utility>

PPGUEditorOverlayPanel::PPGUEditorOverlayPanel(
    const juce::String& title, juce::Component& child,
    std::function<void()> onClose,
    std::function<void(bool)> onExpand,
    const PPGUThemeManager::Palette& initialPalette)
    : content(child), closeCallback(std::move(onClose)),
      expandCallback(std::move(onExpand)), palette(initialPalette)
{
    titleLabel.setText(title, juce::dontSendNotification);
    titleLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    for (auto* button : { &expandButton, &closeButton })
        addAndMakeVisible(*button);

    expandButton.setTooltip("Expand / restore panel");
    closeButton.setTooltip("Close panel");
    expandButton.onClick = [this]
    {
        setExpanded(!expanded);
        if (expandCallback)
            expandCallback(expanded);
    };
    closeButton.onClick = [this]
    {
        if (closeCallback)
            closeCallback();
    };

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
    titleLabel.setColour(juce::Label::textColourId, palette.textPrimary);
    for (auto* button : { &expandButton, &closeButton })
    {
        button->setColour(juce::TextButton::buttonColourId, palette.controlBg);
        button->setColour(juce::TextButton::textColourOffId, palette.textPrimary);
        button->setColour(juce::TextButton::buttonOnColourId, palette.accent);
    }
    repaint();
}

void PPGUEditorOverlayPanel::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    g.setColour(palette.panel);
    g.fillRoundedRectangle(bounds, 12.0f);
    g.setColour(palette.controlBorder);
    g.drawRoundedRectangle(bounds.reduced(0.5f), 12.0f, 1.0f);
    g.setColour(palette.accent);
    g.fillRoundedRectangle(bounds.getX() + 16.0f, bounds.getY() + 1.0f,
                           48.0f, 2.0f, 1.0f);
    g.setColour(palette.panelElevated);
    g.fillRoundedRectangle(bounds.getX() + 1.0f, bounds.getY() + 1.0f,
                           bounds.getWidth() - 2.0f, 42.0f, 10.0f);
}

void PPGUEditorOverlayPanel::resized()
{
    auto header = getLocalBounds().reduced(10, 7).removeFromTop(30);
    titleLabel.setBounds(header.removeFromLeft(header.getWidth() - 78));
    closeButton.setBounds(header.removeFromRight(32));
    header.removeFromRight(5);
    expandButton.setBounds(header.removeFromRight(32));
    content.setBounds(getLocalBounds().reduced(12).withTrimmedTop(46));
}
