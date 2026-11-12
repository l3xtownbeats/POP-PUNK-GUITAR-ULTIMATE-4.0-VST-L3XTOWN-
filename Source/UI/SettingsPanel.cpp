#include "SettingsPanel.h"
#include <utility>

PPGUSettingsPanel::PPGUSettingsPanel(juce::AudioProcessorValueTreeState& state)
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
        control->slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffed238b));
        control->slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff4a5360));
        control->slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff4f0f4));
        control->label.setText(control->name, juce::dontSendNotification);
        control->label.setFont(juce::Font(10.0f, juce::Font::bold));
        control->label.setColour(juce::Label::textColourId, juce::Colour(0xffb8c0cb));
        control->label.setJustificationType(juce::Justification::centred);
        control->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, control->id, control->slider);
        addAndMakeVisible(control->slider);
        addAndMakeVisible(control->label);
        controls.push_back(std::move(control));
    }
}
void PPGUSettingsPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff171c24));
    g.setColour(juce::Colour(0xff202630));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(2.0f), 10.0f);
}
void PPGUSettingsPanel::resized()
{
    auto area = getLocalBounds().reduced(20);
    title.setBounds(area.removeFromTop(34));
    description.setBounds(area.removeFromTop(44));
    area.removeFromTop(10);
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
