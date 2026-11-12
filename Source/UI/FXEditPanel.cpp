#include "FXEditPanel.h"

PPGUFXEditPanel::PPGUFXEditPanel(juce::AudioProcessorValueTreeState& apvts) : state(apvts)
{
    modules[0].name = "COMPRESSOR"; modules[0].enabledId = "compressorOn"; modules[0].controls = { { "compressorThreshold", "THRESHOLD" }, { "compressorRatio", "RATIO" }, { "compressorAttack", "ATTACK" }, { "compressorRelease", "RELEASE" }, { "compressorAmount", "AMOUNT" } };
    modules[1].name = "EQ"; modules[1].enabledId = "eqOn"; modules[1].controls = { { "eqLow", "LOW" }, { "eqMid", "MID" }, { "eqHigh", "HIGH" }, { "eqLowFreq", "LOW FREQ" }, { "eqMidFreq", "MID FREQ" }, { "eqMidQ", "MID Q" }, { "eqHighFreq", "HIGH FREQ" } };
    modules[2].name = "DISTORTION"; modules[2].enabledId = "distortionOn"; modules[2].controls = { { "distortionDrive", "DRIVE" }, { "distortionTone", "TONE" }, { "distortionMix", "MIX" } };
    modules[3].name = "CHORUS"; modules[3].enabledId = "chorusOn"; modules[3].controls = { { "chorusRate", "RATE" }, { "chorusDepth", "DEPTH" }, { "chorusMix", "MIX" } };
    modules[4].name = "DELAY"; modules[4].enabledId = "delayOn"; modules[4].controls = { { "delayTime", "TIME" }, { "delayFeedback", "FEEDBACK" }, { "delayMix", "MIX" } };
    modules[5].name = "REVERB"; modules[5].enabledId = "reverbOn"; modules[5].controls = { { "reverbRoomSize", "ROOM" }, { "reverbDamping", "DAMPING" }, { "reverbMix", "MIX" } };
    modules[6].name = "CABINET IR"; modules[6].enabledId = "cabinetOn"; modules[6].controls = { { "cabinetMix", "MIX" }, { "cabinetLowCut", "LOW CUT" }, { "cabinetHighCut", "HIGH CUT" } };
    for (int i = 0; i < static_cast<int>(modules.size()); ++i)
    {
        auto& module = modules[static_cast<std::size_t>(i)];
        module.select.setButtonText(module.name);
        module.select.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff252a33));
        module.select.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffe8eaf0));
        module.select.onClick = [this, i] { selectModule(i); };
        addAndMakeVisible(module.select);
        module.enabled.setButtonText("ON");
        module.enabled.setClickingTogglesState(true);
        module.enabled.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff252a33));
        module.enabled.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xffed238b));
        module.enabled.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffe8eaf0));
        module.enabledAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, module.enabledId, module.enabled);
        addAndMakeVisible(module.enabled);
    }
    hint.setText("LIVE PARAMETERS  /  CHANGES ARE LINKED TO THE PLUGIN ENGINE", juce::dontSendNotification);
    hint.setFont(juce::Font(10.0f, juce::Font::bold));
    hint.setColour(juce::Label::textColourId, juce::Colour(0xff9ba4b2));
    addAndMakeVisible(hint);
    selectModule(0);
}

void PPGUFXEditPanel::selectModule(int index)
{
    selectedModule = juce::jlimit(0, static_cast<int>(modules.size()) - 1, index);
    controls.clear();
    for (auto& module : modules)
        module.select.setToggleState(&module == &modules[static_cast<std::size_t>(selectedModule)], juce::dontSendNotification);

    for (const auto& item : modules[static_cast<std::size_t>(selectedModule)].controls)
    {
        auto control = std::make_unique<Control>();
        control->id = item.first;
        control->label = item.second;
        auto* parameter = dynamic_cast<juce::RangedAudioParameter*>(state.getParameter(control->id));
        if (parameter == nullptr) continue;
        const auto range = parameter->getNormalisableRange();
        control->slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        control->slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 66, 18);
        control->slider.setRange(range.start, range.end, range.interval);
        control->slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffed238b));
        control->slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff4a5360));
        control->slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff4f0f4));
        control->caption.setText(control->label, juce::dontSendNotification);
        control->caption.setFont(juce::Font(10.0f, juce::Font::bold));
        control->caption.setColour(juce::Label::textColourId, juce::Colour(0xffb8c0cb));
        control->caption.setJustificationType(juce::Justification::centred);
        control->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, control->id, control->slider);
        addAndMakeVisible(control->slider);
        addAndMakeVisible(control->caption);
        controls.push_back(std::move(control));
    }
    resized();
}

void PPGUFXEditPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff171c24));
    auto left = getLocalBounds().removeFromLeft(174).toFloat();
    g.setColour(juce::Colour(0xff202630));
    g.fillRoundedRectangle(left.reduced(2.0f), 9.0f);
    auto work = getLocalBounds().toFloat().withTrimmedLeft(184.0f).reduced(10.0f);
    g.setColour(juce::Colour(0xff202630));
    g.fillRoundedRectangle(work, 9.0f);
    g.setColour(juce::Colour(0xffed238b));
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawText(modules[static_cast<std::size_t>(selectedModule)].name + "  /  PROCESSING", work.removeFromTop(22.0f), juce::Justification::left);
}

void PPGUFXEditPanel::resized()
{
    auto left = getLocalBounds().removeFromLeft(174).reduced(7, 8);
    for (auto& module : modules)
    {
        auto row = left.removeFromTop(37);
        module.select.setBounds(row.removeFromLeft(108));
        row.removeFromLeft(4);
        module.enabled.setBounds(row);
        left.removeFromTop(5);
    }
    hint.setBounds(getLocalBounds().removeFromBottom(22).reduced(190, 0));
    auto work = getLocalBounds().withTrimmedLeft(184).reduced(12, 28).withTrimmedBottom(24);
    const int count = juce::jmax(1, static_cast<int>(controls.size()));
    const int columns = juce::jmin(4, count);
    const int rows = (count + columns - 1) / columns;
    const int cellW = work.getWidth() / columns;
    const int cellH = work.getHeight() / rows;
    for (int i = 0; i < static_cast<int>(controls.size()); ++i)
    {
        auto& c = *controls[static_cast<std::size_t>(i)];
        auto cell = juce::Rectangle<int>(work.getX() + (i % columns) * cellW,
                                         work.getY() + (i / columns) * cellH, cellW, cellH).reduced(5);
        c.caption.setBounds(cell.removeFromTop(22));
        c.slider.setBounds(cell.reduced(4, 0));
    }
}
