#include "SettingsPanel.h"
#include <utility>

PPGUSettingsPanel::PPGUSettingsPanel(juce::AudioProcessorValueTreeState& state,
                                     const PPGUThemeManager::Palette& initialPalette)
    : palette(initialPalette)
{
    title.setText("PERFORMANCE & ENGINE SETTINGS", juce::dontSendNotification);
    title.setFont(juce::Font(17.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, juce::Colour(0xfff4f0f4));
    addAndMakeVisible(title);
    description.setText("These controls are attached to the existing processor parameters. They do not change the audio routing or parameter identifiers.", juce::dontSendNotification);
    description.setFont(juce::Font(11.0f));
    description.setColour(juce::Label::textColourId, juce::Colour(0xffaab2bf));
    addAndMakeVisible(description);
    const std::pair<const char*, const char*> definitions[] = {
        { "voices", "POLYPHONY / VOICES" }, { "master", "MASTER LEVEL" },
        { "tuning", "GLOBAL TUNING" }, { "pan", "OUTPUT PAN" },
        { "output", "OUTPUT GAIN (dB)" }, { "palmMuteThreshold", "PALM MUTE VELOCITY THRESHOLD" }
    };
    for (const auto& definition : definitions)
    {
        auto control = std::make_unique<Control>();
        control->id = definition.first;
        control->name = definition.second;
        auto* parameter = dynamic_cast<juce::RangedAudioParameter*>(state.getParameter(control->id));
        if (parameter == nullptr) continue;
        const auto range = parameter->getNormalisableRange();
        control->slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        control->slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 18);
        control->slider.setRange(range.start, range.end, range.interval);
        control->slider.setColour(juce::Slider::rotarySliderFillColourId, palette.token("accent"));
        control->slider.setColour(juce::Slider::rotarySliderOutlineColourId, palette.token("control_border"));
        control->slider.setColour(juce::Slider::thumbColourId, palette.token("text_primary"));
        control->slider.setColour(juce::Slider::textBoxTextColourId, palette.token("text_primary"));
        control->label.setText(control->name, juce::dontSendNotification);
        control->label.setFont(juce::Font(10.0f, juce::Font::bold));
        control->label.setColour(juce::Label::textColourId, palette.token("text_secondary"));
        control->label.setJustificationType(juce::Justification::centred);
        control->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, control->id, control->slider);
        addAndMakeVisible(control->slider);
        addAndMakeVisible(control->label);
        controls.push_back(std::move(control));
    }
    setPalette(palette);
}

void PPGUSettingsPanel::setPalette(const PPGUThemeManager::Palette& value)
{
    palette = value;
    title.setColour(juce::Label::textColourId, palette.token("text_primary"));
    description.setColour(juce::Label::textColourId, palette.token("text_secondary"));
    for (auto& control : controls)
    {
        control->slider.setColour(juce::Slider::rotarySliderFillColourId, palette.token("accent"));
        control->slider.setColour(juce::Slider::rotarySliderOutlineColourId, palette.token("control_border"));
        control->slider.setColour(juce::Slider::thumbColourId, palette.token("text_primary"));
        control->slider.setColour(juce::Slider::textBoxTextColourId, palette.token("text_primary"));
        control->label.setColour(juce::Label::textColourId, palette.token("text_secondary"));
    }
    repaint();
}
void PPGUSettingsPanel::paint(juce::Graphics& g)
{
    g.fillAll(palette.token("window_bg"));
    g.setColour(palette.token("panel"));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(2.0f), 10.0f);
}
void PPGUSettingsPanel::resized()
{
    const bool compact = getHeight() < 280;
    auto area = getLocalBounds().reduced(compact ? 12 : 20);
    title.setBounds(area.removeFromTop(compact ? 24 : 34));
    description.setBounds(area.removeFromTop(compact ? 30 : 44));
    area.removeFromTop(compact ? 4 : 10);
    const int count = static_cast<int>(controls.size());
    const int columns = juce::jmin(3, juce::jmax(1, count));
    const int rows = (count + columns - 1) / columns;
    const int cellW = area.getWidth() / columns;
    const int cellH = area.getHeight() / juce::jmax(1, rows);
    for (int i = 0; i < count; ++i)
    {
        auto cell = juce::Rectangle<int>(area.getX() + (i % columns) * cellW,
                                         area.getY() + (i / columns) * cellH, cellW, cellH).reduced(6);
        auto& c = *controls[static_cast<std::size_t>(i)];
        c.label.setBounds(cell.removeFromTop(24));
        c.slider.setBounds(cell.reduced(8, 0));
    }
}
