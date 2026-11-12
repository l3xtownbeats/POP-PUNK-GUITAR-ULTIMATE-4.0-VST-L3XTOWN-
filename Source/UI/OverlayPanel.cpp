#include "OverlayPanel.h"
#include <array>

namespace ppgu::ui
{
namespace {
const juce::Colour panelBg { 0xff171923 };
const juce::Colour accent { 0xff29e9ff };
const juce::Colour text { 0xfff3f5fa };
const juce::Colour muted { 0xff9ca7b8 };
const std::array<const char*, 7> fxIds {
    "compressorOn", "eqOn", "distortionOn", "chorusOn", "delayOn", "reverbOn", "cabinetOn"
};
}
OverlayPanel::OverlayPanel(OverlayPanelId panelId, PopPunkGuitarUltimateAudioProcessor& p,
                           std::function<void()> onClose, std::function<void(bool)> onCompactChanged)
    : id(panelId), processor(p), closeCallback(std::move(onClose)),
      compactCallback(std::move(onCompactChanged))
{
    setOpaque(false);
    title.setText(id == OverlayPanelId::AutoChords ? "♫  AUTOCHORDS" : "✦  FX EDIT",
                  juce::dontSendNotification);
    title.setFont(juce::Font(19.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, accent);
    addAndMakeVisible(title);
    for (auto* button : { &closeButton, &sizeButton }) {
        button->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff252b38));
        button->setColour(juce::TextButton::textColourOffId, text);
        addAndMakeVisible(*button);
    }
    closeButton.onClick = [this] { if (closeCallback) closeCallback(); };
    sizeButton.onClick = [this] { setCompact(!compact); };
    status.setColour(juce::Label::textColourId, muted);
    status.setFont(juce::Font(11.0f));
    addAndMakeVisible(status);

    if (id == OverlayPanelId::AutoChords) {
        const char* keys[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        for (int i = 0; i < 12; ++i) keyBox.addItem(keys[i], i + 1);
        modeBox.addItem("MAJOR", 1); modeBox.addItem("MINOR", 2); modeBox.addItem("CUSTOM", 3);
        patternBox.addItem("I – V – vi – IV", 1);
        patternBox.addItem("I – vi – IV – V", 2);
        patternBox.addItem("vi – IV – I – V", 3);
        keyBox.setSelectedId(1); modeBox.setSelectedId(1); patternBox.setSelectedId(1);
        for (auto* box : { &keyBox, &modeBox, &patternBox }) {
            box->setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff202838));
            box->setColour(juce::ComboBox::textColourId, text);
            box->setColour(juce::ComboBox::outlineColourId, accent.withAlpha(0.5f));
            addAndMakeVisible(*box);
        }
        keyBox.onChange = [this] { updateChordReadout(); };
        modeBox.onChange = [this] { updateChordReadout(); };
        patternBox.onChange = [this] { updateChordReadout(); };
        chordReadout.setColour(juce::Label::textColourId, text);
        chordReadout.setFont(juce::Font(24.0f, juce::Font::bold));
        chordReadout.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(chordReadout);
        previewButton.setEnabled(false);
        previewButton.setTooltip("In development: audio/MIDI preview integration is not available.");
        addAndMakeVisible(previewButton);
        status.setText("UI HARMONY EDITOR  /  PREVIEW REQUIRES AUDIO INTEGRATION",
                       juce::dontSendNotification);
        updateChordReadout();
    } else {
        for (size_t i = 0; i < fxButtons.size(); ++i) {
            auto& b = fxButtons[i];
            b.setClickingTogglesState(true);
            b.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff202838));
            b.setColour(juce::TextButton::buttonOnColourId, accent.withAlpha(0.35f));
            b.setColour(juce::TextButton::textColourOffId, text);
            b.setColour(juce::TextButton::textColourOnId, accent);
            fxAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                processor.parameters, fxIds[i], b);
            addAndMakeVisible(b);
        }
        status.setText("MODULE BYPASS IS LIVE  /  EDIT PARAMETERS: IN DEVELOPMENT",
                       juce::dontSendNotification);
    }
}
void OverlayPanel::updateChordReadout()
{
    static const char* notes[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    const auto note = notes[juce::jlimit(0, 11, keyBox.getSelectedId() - 1)];
    const bool minor = modeBox.getSelectedId() == 2;
    chordReadout.setText(juce::String(note) + (minor ? "m" : "") + "  •  "
                         + (patternBox.getSelectedId() == 1 ? "I – V – vi – IV"
                            : patternBox.getSelectedId() == 2 ? "I – vi – IV – V"
                                                              : "vi – IV – I – V"),
                         juce::dontSendNotification);
}
void OverlayPanel::setCompact(bool value)
{
    if (compact == value) return;
    compact = value;
    resized();
    repaint();
    if (compactCallback) compactCallback(compact);
}
void OverlayPanel::mouseDown(const juce::MouseEvent&)
{
    toFront(false);
}
void OverlayPanel::paint(juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xf0121722)); g.fillRoundedRectangle(r, 12.0f);
    g.setColour(accent.withAlpha(0.9f)); g.drawRoundedRectangle(r.reduced(0.5f), 12.0f, 1.4f);
    g.setColour(accent.withAlpha(0.14f)); g.fillRect(1.0f, 48.0f, r.getWidth() - 2.0f, 1.0f);
}
void OverlayPanel::resized()
{
    auto area = getLocalBounds().reduced(14);
    auto header = area.removeFromTop(34);
    title.setBounds(header.removeFromLeft(header.getWidth() - 78));
    closeButton.setBounds(header.removeFromRight(32));
    sizeButton.setBounds(header.removeFromRight(36).reduced(2, 0));
    status.setBounds(area.removeFromBottom(24));
    if (compact) {
        for (auto* c : { static_cast<juce::Component*>(&keyBox), &modeBox, &patternBox,
                         &chordReadout, &previewButton })
            c->setVisible(false);
        for (auto& b : fxButtons) b.setVisible(false);
        return;
    }
    if (id == OverlayPanelId::AutoChords) {
        for (auto* c : { static_cast<juce::Component*>(&keyBox), &modeBox, &patternBox,
                         &chordReadout, &previewButton }) c->setVisible(true);
        auto row = area.removeFromTop(42);
        keyBox.setBounds(row.removeFromLeft(row.getWidth() / 3).reduced(3));
        modeBox.setBounds(row.removeFromLeft(row.getWidth() / 2).reduced(3));
        patternBox.setBounds(row.reduced(3));
        chordReadout.setBounds(area.removeFromTop(90));
        previewButton.setBounds(area.removeFromTop(36).reduced(80, 2));
    } else {
        const int cellH = juce::jmax(32, area.getHeight() / 7);
        for (auto& b : fxButtons) {
            b.setVisible(true);
            b.setBounds(area.removeFromTop(cellH).reduced(2, 2));
        }
    }
}
}
