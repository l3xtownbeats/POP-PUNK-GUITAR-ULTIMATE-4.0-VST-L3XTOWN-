#include "PluginEditor.h"
#include "PPGUData.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <set>
#include <limits>
#include <vector>
#if JUCE_WINDOWS
 #ifndef NOMINMAX
  #define NOMINMAX
 #endif
 #include <windows.h>
juce::String fromWidePath(const wchar_t* value, int length)
{
    if (value == nullptr || length <= 0)
        return {};
    const int required = WideCharToMultiByte(CP_UTF8, 0, value, length, nullptr, 0, nullptr, nullptr);
    if (required <= 0)
        return {};
    std::string utf8(static_cast<size_t>(required), '\0');
    const int written = WideCharToMultiByte(CP_UTF8, 0, value, length,
                                             utf8.data(), required, nullptr, nullptr);
    return written == required ? juce::String::fromUTF8(utf8.data(), written) : juce::String();
}
#endif

// Import panel delegates to the existing asynchronous SFZ loader and reports its real result.
class PPGUImportGuitarPanel final : public juce::Component
{
public:
    explicit PPGUImportGuitarPanel(std::function<void(juce::Label*, juce::Button*)> loadActionIn)
        : loadAction(std::move(loadActionIn))
    {
        heading.setText("LICENSED GUITAR LIBRARY", juce::dontSendNotification);
        heading.setFont(juce::Font(18.0f, juce::Font::bold));
        heading.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(heading);
        description.setText("Load an SFZ guitar instrument and its referenced WAV samples. "
                            "The existing background loader validates and registers the bank.",
                            juce::dontSendNotification);
        description.setJustificationType(juce::Justification::topLeft);
        description.setColour(juce::Label::textColourId, juce::Colour(0xffb5c4cb));
        addAndMakeVisible(description);
        status.setText("No SFZ library loaded in this session.", juce::dontSendNotification);
        status.setJustificationType(juce::Justification::topLeft);
        status.setColour(juce::Label::textColourId, juce::Colour(0xffd7ffb2));
        addAndMakeVisible(status);
        loadButton.setButtonText("LOAD SFZ");
        loadButton.onClick = [this] { if (loadAction) loadAction(&status, &loadButton); };
        addAndMakeVisible(loadButton);
    }
    void resized() override
    {
        auto area = getLocalBounds().reduced(18);
        heading.setBounds(area.removeFromTop(34));
        area.removeFromTop(12);
        description.setBounds(area.removeFromTop(64));
        area.removeFromTop(18);
        loadButton.setBounds(area.removeFromTop(42).removeFromLeft(180));
        area.removeFromTop(16);
        status.setBounds(area.removeFromTop(90));
    }
private:
    std::function<void(juce::Label*, juce::Button*)> loadAction;
    juce::Label heading, description, status;
    juce::TextButton loadButton;
};

// Compact live harmony monitor. It observes the existing MIDI keyboard state and
// scale parameters only; it never writes to APVTS or changes processor behaviour.
class PPGUPerformanceDisplay final : public juce::Component, private juce::Timer
{
public:
    explicit PPGUPerformanceDisplay(PopPunkGuitarUltimateAudioProcessor& processorIn)
        : processor(processorIn), parameters(processorIn.parameters)
    {
        setOpaque(false);
        refreshState();
        startTimerHz(20);
    }

    void setPalette(const PPGUThemeManager::Palette& value)
    {
        palette = value;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.setGradientFill(juce::ColourGradient(palette.tokens.panelElevated,
                                               bounds.getX(), bounds.getY(),
                                               palette.tokens.panel,
                                               bounds.getX(), bounds.getBottom(), false));
        g.fillRoundedRectangle(bounds, 9.0f);
        g.setColour(palette.tokens.controlBorder.withAlpha(0.9f));
        g.drawRoundedRectangle(bounds.reduced(0.5f), 9.0f, 1.0f);
        g.setColour(palette.tokens.accent);
        g.fillRoundedRectangle(bounds.getX() + 1.0f, bounds.getY() + 8.0f, 3.0f,
                               juce::jmax(8.0f, bounds.getHeight() - 16.0f), 1.5f);

        auto area = getLocalBounds().reduced(14, 5);
        auto harmony = area.removeFromLeft(190);
        g.setColour(palette.tokens.textSecondary);
        g.setFont(juce::Font(8.5f, juce::Font::bold));
        g.drawText("LIVE HARMONY", harmony.removeFromTop(13), juce::Justification::left);
        g.setColour(palette.tokens.accent);
        g.setFont(juce::Font(23.0f, juce::Font::bold));
        g.drawFittedText(chordText, harmony, juce::Justification::centredLeft, 1);

        area.removeFromLeft(10);
        auto keyArea = area.removeFromRight(148);
        g.setColour(palette.tokens.textSecondary);
        g.setFont(juce::Font(8.5f, juce::Font::bold));
        g.drawText("SCALE / KEY", keyArea.removeFromTop(13), juce::Justification::left);
        g.setColour(palette.tokens.textPrimary);
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        g.drawFittedText(keyText, keyArea, juce::Justification::centredLeft, 1);

        area.removeFromRight(8);
        g.setColour(palette.tokens.textSecondary);
        g.setFont(juce::Font(8.5f, juce::Font::bold));
        g.drawText("NOTES HELD", area.removeFromTop(13), juce::Justification::left);
        g.setColour(palette.tokens.textPrimary);
        g.setFont(juce::Font(12.0f));
        g.drawFittedText(notesText, area, juce::Justification::centredLeft, 1);
    }

private:
    struct ChordShape
    {
        const char* suffix;
        std::vector<int> intervals;
        int priority;
    };

    static juce::String pitchName(int pitchClass)
    {
        static const char* names[] = {
            "C", "C#", "D", "D#", "E", "F",
            "F#", "G", "G#", "A", "A#", "B"
        };
        return names[juce::jlimit(0, 11, pitchClass)];
    }

    static juce::String noteName(int midiNote)
    {
        return pitchName(midiNote % 12) + juce::String(midiNote / 12 - 1);
    }

    juce::String detectChord(const std::vector<int>& notes) const
    {
        if (notes.empty())
            return "-";

        if (notes.size() == 1)
            return noteName(notes.front());

        std::array<bool, 12> present {};
        int pitchClassCount = 0;
        for (const auto note : notes)
        {
            const int pc = note % 12;
            if (!present[static_cast<size_t>(pc)])
            {
                present[static_cast<size_t>(pc)] = true;
                ++pitchClassCount;
            }
        }

        static const std::vector<ChordShape> shapes {
            { "m",    { 0, 3, 7 },       12 },
            { "maj",  { 0, 4, 7 },       11 },
            { "m7",   { 0, 3, 7, 10 },  10 },
            { "7",    { 0, 4, 7, 10 },   9 },
            { "maj7", { 0, 4, 7, 11 },   9 },
            { "sus2", { 0, 2, 7 },       8 },
            { "sus4", { 0, 5, 7 },       8 },
            { "dim",  { 0, 3, 6 },       7 },
            { "aug",  { 0, 4, 8 },       7 },
            { "m7b5", { 0, 3, 6, 10 },   6 },
            { "dim7", { 0, 3, 6, 9 },    6 },
            { "6",    { 0, 4, 7, 9 },    5 },
            { "m6",   { 0, 3, 7, 9 },    5 },
            { "mMaj7",{ 0, 3, 7, 11 },  4 },
            { "5",    { 0, 7 },          2 }
        };

        const int bassPc = notes.front() % 12;
        int bestScore = std::numeric_limits<int>::min();
        juce::String best;

        for (int root = 0; root < 12; ++root)
        {
            for (const auto& shape : shapes)
            {
                bool complete = true;
                for (const auto interval : shape.intervals)
                    if (!present[static_cast<size_t>((root + interval) % 12)])
                    {
                        complete = false;
                        break;
                    }

                if (!complete)
                    continue;

                const int extras = pitchClassCount - static_cast<int>(shape.intervals.size());
                const int score = 100 - extras * 18 + shape.priority + (root == bassPc ? 4 : 0);
                if (score > bestScore)
                {
                    bestScore = score;
                    best = pitchName(root) + shape.suffix;
                }
            }
        }

        if (best.isNotEmpty())
            return best;

        return pitchName(bassPc) + "  /  " + juce::String(pitchClassCount) + " tones";
    }

    void refreshState()
    {
        std::vector<int> held;
        held.reserve(8);
        for (int note = 0; note < 128; ++note)
        {
            bool isHeld = false;
            isHeld = processor.isMidiNoteHeld(note);
            if (isHeld)
                held.push_back(note);
        }

        juce::StringArray noteNames;
        for (size_t i = 0; i < held.size() && i < 6; ++i)
            noteNames.add(noteName(held[i]));
        if (held.size() > 6)
            noteNames.add("+" + juce::String(static_cast<int>(held.size() - 6)));

        const auto newChord = detectChord(held);
        const auto newNotes = held.empty() ? juce::String("Waiting for MIDI input")
                                           : noteNames.joinIntoString("   /   ");

        auto* rootParameter = parameters.getRawParameterValue("scaleRoot");
        auto* modeParameter = parameters.getRawParameterValue("scaleMode");
        const int root = rootParameter != nullptr
            ? juce::jlimit(0, 11, static_cast<int>(rootParameter->load(std::memory_order_relaxed))) : 0;
        const int mode = modeParameter != nullptr
            ? juce::jlimit(0, 2, static_cast<int>(modeParameter->load(std::memory_order_relaxed))) : 0;
        const auto newKey = mode == 1 ? pitchName(root) + " MAJOR"
                         : mode == 2 ? pitchName(root) + " MINOR"
                                     : juce::String("CHROMATIC");

        if (newChord != chordText || newNotes != notesText || newKey != keyText)
        {
            chordText = newChord;
            notesText = newNotes;
            keyText = newKey;
            repaint();
        }
    }

    void timerCallback() override { refreshState(); }

    PopPunkGuitarUltimateAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& parameters;
    PPGUThemeManager::Palette palette;
    juce::String chordText { "—" };
    juce::String notesText { "Waiting for MIDI input" };
    juce::String keyText { "CHROMATIC" };
};


namespace
{
class PPGUFXEditPanel final : public juce::Component
{
public:
    explicit PPGUFXEditPanel(juce::AudioProcessorValueTreeState& state, int initialEffect)
        : apvts(state)
    {
        groups = {
            { "COMPRESSOR", "compressorOn", { { "THRESHOLD", "compressorThreshold" }, { "RATIO", "compressorRatio" }, { "ATTACK", "compressorAttack" }, { "RELEASE", "compressorRelease" }, { "AMOUNT", "compressorAmount" } } },
            { "EQ", "eqOn", { { "LOW", "eqLow" }, { "MID", "eqMid" }, { "HIGH", "eqHigh" }, { "LOW FREQ", "eqLowFreq" }, { "MID FREQ", "eqMidFreq" }, { "MID Q", "eqMidQ" }, { "HIGH FREQ", "eqHighFreq" } } },
            { "DISTORTION", "distortionOn", { { "DRIVE", "distortionDrive" }, { "TONE", "distortionTone" }, { "MIX", "distortionMix" } } },
            { "CHORUS", "chorusOn", { { "RATE", "chorusRate" }, { "DEPTH", "chorusDepth" }, { "MIX", "chorusMix" } } },
            { "FLANGER", "flangerOn", { { "RATE", "flangerRate" }, { "DEPTH", "flangerDepth" }, { "FEEDBACK", "flangerFeedback" }, { "MIX", "flangerMix" } } },
            { "DELAY", "delayOn", { { "TIME", "delayTime" }, { "FEEDBACK", "delayFeedback" }, { "MIX", "delayMix" } } },
            { "REVERB", "reverbOn", { { "ROOM", "reverbRoomSize" }, { "DAMPING", "reverbDamping" }, { "MIX", "reverbMix" } } },
            { "CABINET IR", "cabinetOn", { { "MIX", "cabinetMix" }, { "LOW CUT", "cabinetLowCut" }, { "HIGH CUT", "cabinetHighCut" } } }
        };

        title.setText("SIGNAL FORGE  /  FX EDIT", juce::dontSendNotification);
        title.setFont(juce::Font(20.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(0xffb7f34a));
        addAndMakeVisible(title);

        for (size_t i = 0; i < groups.size(); ++i)
            effectSelector.addItem(groups[i].name, static_cast<int>(i + 1));
        effectSelector.setSelectedId(juce::jlimit(1, static_cast<int>(groups.size()), initialEffect + 1), juce::dontSendNotification);
        effectSelector.onChange = [this] { selectGroup(effectSelector.getSelectedId() - 1); };
        addAndMakeVisible(effectSelector);

        powerButton.setButtonText("POWER");
        powerButton.setClickingTogglesState(true);
        addAndMakeVisible(powerButton);
        graphCaption.setText("PROCESS RESPONSE  /  VISUAL GUIDE", juce::dontSendNotification);
        graphCaption.setColour(juce::Label::textColourId, juce::Colour(0xff91a5b0));
        addAndMakeVisible(graphCaption);
        status.setText("Controls are attached to the existing APVTS parameters.", juce::dontSendNotification);
        status.setColour(juce::Label::textColourId, juce::Colour(0xff91a5b0));
        addAndMakeVisible(status);

        for (size_t groupIndex = 0; groupIndex < groups.size(); ++groupIndex)
        {
            for (const auto& item : groups[groupIndex].parameters)
            {
                auto label = std::make_unique<juce::Label>();
                label->setText(item.first, juce::dontSendNotification);
                label->setColour(juce::Label::textColourId, juce::Colour(0xffb5c2ca));
                label->setFont(juce::Font(11.0f, juce::Font::bold));
                addAndMakeVisible(*label);

                auto slider = std::make_unique<juce::Slider>();
                slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
                slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
                slider->setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffb7f34a));
                slider->setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff43545e));
                slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xffe7f0f4));
                slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffe7f0f4));
                addAndMakeVisible(*slider);

                auto attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, item.second, *slider);
                controls.push_back({ static_cast<int>(groupIndex), std::move(label), std::move(slider), std::move(attachment) });
            }
        }
        selectGroup(effectSelector.getSelectedId() - 1);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff0c1218));
        auto area = getLocalBounds().toFloat().reduced(12.0f);
        g.setColour(juce::Colour(0xff263944));
        g.drawRoundedRectangle(area, 10.0f, 1.0f);
        if (!graphArea.isEmpty())
        {
            auto graph = graphArea.toFloat();
            g.setColour(juce::Colour(0xff101c24));
            g.fillRoundedRectangle(graph, 8.0f);
            g.setColour(juce::Colour(0xff304550));
            for (int i = 1; i < 5; ++i)
                g.drawHorizontalLine(graphArea.getY() + graphArea.getHeight() * i / 5, graphArea.getX(), graphArea.getRight());
            for (int i = 1; i < 9; ++i)
                g.drawVerticalLine(graphArea.getX() + graphArea.getWidth() * i / 9, graphArea.getY(), graphArea.getBottom());
            juce::Path curve;
            curve.startNewSubPath(static_cast<float>(graphArea.getX()), static_cast<float>(graphArea.getCentreY()));
            curve.cubicTo(graphArea.getX() + graphArea.getWidth() * 0.28f, graphArea.getY() + graphArea.getHeight() * 0.78f,
                          graphArea.getX() + graphArea.getWidth() * 0.62f, graphArea.getY() + graphArea.getHeight() * 0.22f,
                          static_cast<float>(graphArea.getRight()), static_cast<float>(graphArea.getCentreY()));
            g.setColour(juce::Colour(0xffb7f34a));
            g.strokePath(curve, juce::PathStrokeType(2.0f));
        }
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(20);
        title.setBounds(area.removeFromTop(34));
        auto top = area.removeFromTop(44);
        effectSelector.setBounds(top.removeFromLeft(230).reduced(2, 4));
        powerButton.setBounds(top.removeFromLeft(110).reduced(2, 4));
        graphCaption.setBounds(top);
        const int graphHeight = getHeight() >= 600 ? 140 : (getHeight() >= 500 ? 90 : 0);
        graphCaption.setVisible(graphHeight > 0);
        graphArea = graphHeight > 0
            ? area.removeFromBottom(graphHeight).reduced(4, 8)
            : juce::Rectangle<int>{};
        status.setBounds(area.removeFromBottom(24));
        area.removeFromBottom(8);
        const int visibleCount = static_cast<int>(std::count_if(controls.begin(), controls.end(),
            [this](const Control& c) { return c.group == selectedGroup; }));
        const int columns = visibleCount > 5 ? 4 : juce::jmax(1, visibleCount);
        const int rows = (visibleCount + columns - 1) / columns;
        const int cellWidth = juce::jmax(1, area.getWidth() / columns);
        const int cellHeight = juce::jmax(1, area.getHeight() / juce::jmax(1, rows));
        int index = 0;
        for (auto& control : controls)
        {
            if (control.group != selectedGroup)
            {
                control.label->setVisible(false);
                control.slider->setVisible(false);
                continue;
            }
            control.label->setVisible(true);
            control.slider->setVisible(true);
            const int column = index % columns;
            const int row = index / columns;
            auto cell = juce::Rectangle<int>(area.getX() + column * cellWidth,
                area.getY() + row * cellHeight, cellWidth, cellHeight).reduced(6, 2);
            control.label->setBounds(cell.removeFromTop(20));
            control.slider->setBounds(cell);
            ++index;
        }
    }

private:
    struct Group
    {
        juce::String name, enabledParameter;
        std::vector<std::pair<juce::String, juce::String>> parameters;
    };
    struct Control
    {
        int group;
        std::unique_ptr<juce::Label> label;
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    void selectGroup(int index)
    {
        selectedGroup = juce::jlimit(0, static_cast<int>(groups.size()) - 1, index);
        powerAttachment.reset();
        powerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, groups[static_cast<size_t>(selectedGroup)].enabledParameter, powerButton);
        resized();
        repaint();
    }
    juce::AudioProcessorValueTreeState& apvts;
    std::vector<Group> groups;
    std::vector<Control> controls;
    juce::Label title, graphCaption, status;
    juce::ComboBox effectSelector;
    juce::TextButton powerButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
    juce::Rectangle<int> graphArea;
    int selectedGroup = 0;
};

class PPGUAutoChordsPanel final : public juce::Component
{
public:
    explicit PPGUAutoChordsPanel(juce::AudioProcessorValueTreeState& state) : apvts(state)
    {
        title.setText("AUTOCHORDS  /  HARMONY SETUP", juce::dontSendNotification);
        title.setFont(juce::Font(20.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(0xffb7f34a));
        addAndMakeVisible(title);
        stateLabel.setText("SETUP ONLY — MIDI CHORD GENERATION / CUSTOM MODE / PREVIEW ENGINE NOT CONNECTED", juce::dontSendNotification);
        stateLabel.setColour(juce::Label::textColourId, juce::Colour(0xffffc66d));
        addAndMakeVisible(stateLabel);
        offButton.setButtonText("AUTOCHORDS OFF");
        offButton.setEnabled(false);
        offButton.setTooltip("The AutoChords MIDI-generation engine is not implemented yet.");
        addAndMakeVisible(offButton);

        keyBox.addItemList({ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 1);
        modeBox.addItemList({ "Chromatic", "Major", "Minor" }, 1);
        patternBox.addItemList({ "Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4",
                                   "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8" }, 1);
        addAndMakeVisible(keyBox);
        addAndMakeVisible(modeBox);
        addAndMakeVisible(patternBox);
        keyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "scaleRoot", keyBox);
        modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "scaleMode", modeBox);
        patternAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "patternSlot", patternBox);
        for (auto* label : { &keyLabel, &modeLabel, &patternLabel })
        {
            label->setColour(juce::Label::textColourId, juce::Colour(0xffb5c2ca));
            addAndMakeVisible(*label);
        }
        keyLabel.setText("KEY", juce::dontSendNotification);
        modeLabel.setText("SCALE / QUALITY", juce::dontSendNotification);
        patternLabel.setText("RHYTHM PATTERN SLOT", juce::dontSendNotification);
    }
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff0c1218));
        g.setColour(juce::Colour(0xff263944));
        g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(12), 10, 1);
    }
    void resized() override
    {
        auto area = getLocalBounds().reduced(24);
        title.setBounds(area.removeFromTop(38));
        stateLabel.setBounds(area.removeFromTop(32));
        offButton.setBounds(area.removeFromTop(44).removeFromLeft(210).reduced(2));
        area.removeFromTop(24);
        keyLabel.setBounds(area.removeFromTop(22));
        keyBox.setBounds(area.removeFromTop(44).removeFromLeft(260));
        area.removeFromTop(20);
        modeLabel.setBounds(area.removeFromTop(22));
        modeBox.setBounds(area.removeFromTop(44).removeFromLeft(260));
        area.removeFromTop(20);
        patternLabel.setBounds(area.removeFromTop(22));
        patternBox.setBounds(area.removeFromTop(44).removeFromLeft(360));
    }
private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::Label title, stateLabel, keyLabel, modeLabel, patternLabel;
    juce::TextButton offButton;
    juce::ComboBox keyBox, modeBox, patternBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> keyAttachment, modeAttachment, patternAttachment;
};
}

// Premium SIGNAL FORGE shell. Visual theme code never touches processor/audio state.

namespace { void getPluginModuleAnchor() {} }

void PPGUPremiumLookAndFeel::drawRotarySlider(juce::Graphics& g,int x,int y,int w,int h,float pos,float start,float end,juce::Slider& slider)
{
 const float d=juce::jmin((float)w,(float)h)*.76f,r=d*.5f,cx=x+w*.5f,cy=y+h*.5f-2,angle=start+pos*(end-start);
 g.setColour(juce::Colours::black.withAlpha(.34f));g.fillEllipse(cx-r-1,cy-r+3,d+2,d+2);
 juce::Path arc;arc.addCentredArc(cx,cy,r+5,r+5,0,start,end,true);g.setColour(palette.border);g.strokePath(arc,juce::PathStrokeType(4,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
 juce::Path value;value.addCentredArc(cx,cy,r+5,r+5,0,start,angle,true);g.setColour(slider.isEnabled()?palette.accent:palette.textSecondary.withAlpha(.4f));g.strokePath(value,juce::PathStrokeType(4,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
 g.setGradientFill(juce::ColourGradient(palette.surfaceRaised.brighter(.12f),cx,cy-r,palette.canvasInset.darker(.12f),cx,cy+r,false));g.fillEllipse(cx-r+4,cy-r+4,d-8,d-8);g.setColour(palette.border);g.drawEllipse(cx-r+4,cy-r+4,d-8,d-8,1.2f);
 juce::Path pointer;pointer.startNewSubPath(cx,cy);pointer.lineTo(cx+std::sin(angle)*r*.56f,cy-std::cos(angle)*r*.56f);g.setColour(palette.text);g.strokePath(pointer,juce::PathStrokeType(2.6f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));g.setColour(palette.accentSecondary);g.fillEllipse(cx-2.5f,cy-2.5f,5,5);
}
void PPGUPremiumLookAndFeel::drawButtonBackground(juce::Graphics& g,juce::Button& b,const juce::Colour& base,bool hi,bool down)
{
 auto r=b.getLocalBounds().toFloat().reduced(.5f);auto c=b.getToggleState()?palette.accent:base;if(!b.isEnabled())c=palette.surfaceSecondary;else if(down)c=c.darker(.18f);else if(hi)c=c.brighter(.12f);
 g.setGradientFill(juce::ColourGradient(c.brighter(.08f),r.getX(),r.getY(),c.darker(.16f),r.getX(),r.getBottom(),false));g.fillRoundedRectangle(r,7);g.setColour(palette.border);g.drawRoundedRectangle(r,7,1);
 if(b.getToggleState()&&b.isEnabled()){g.setColour(palette.accentSecondary);g.fillRoundedRectangle(r.removeFromLeft(3),1.5f);}
}
void PPGUPremiumLookAndFeel::drawComboBox(juce::Graphics& g,int w,int h,bool down,int,int,int,int,juce::ComboBox& box)
{
 auto r=juce::Rectangle<float>(.5f,.5f,(float)w-1,(float)h-1);auto c=box.findColour(juce::ComboBox::backgroundColourId);g.setColour(down?c.brighter(.08f):c);g.fillRoundedRectangle(r,7);g.setColour(palette.border);g.drawRoundedRectangle(r,7,1);
 float cx=r.getRight()-17,cy=r.getCentreY();juce::Path p;p.startNewSubPath(cx-4,cy-2);p.lineTo(cx,cy+2);p.lineTo(cx+4,cy-2);g.setColour(palette.accent);g.strokePath(p,juce::PathStrokeType(1.8f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
}

PopPunkGuitarUltimateAudioProcessorEditor::PopPunkGuitarUltimateAudioProcessorEditor(PopPunkGuitarUltimateAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setWantsKeyboardFocus(true);
    setLookAndFeel(&premiumLookAndFeel);
    runtimeSkinRoot = getRuntimeSkinRoot();
    setSize(1120, 840);
    setResizable(true, true);
    // The current fixed-height control stack needs ~788 px including outer margins.
    // Do not allow a window size that clips the keyboard or lower controls.
    setResizeLimits(980, 800, 1600, 1100);
    titleLabel.setText("PPGU", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, themeManager.getPalette().token("text_primary"));
    addAndMakeVisible(titleLabel);
    subtitleLabel.setText("L3XTOWN AUDIO LAB", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(9.5f, juce::Font::bold));
    subtitleLabel.setColour(juce::Label::textColourId, themeManager.getPalette().token("accent"));
    addAndMakeVisible(subtitleLabel);
    presetBox.setTextWhenNothingSelected("FACTORY PRESET CATALOG");
    presetBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    presetBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    presetBox.setColour(juce::ComboBox::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.5f));
    addAndMakeVisible(presetBox);
    categoryBox.addItem("ALL PRESETS", 1);
    categoryBox.addItem("MELODY", 2);
    categoryBox.addItem("PATTERN", 3);
    categoryBox.setSelectedId(1, juce::dontSendNotification);
    categoryBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    categoryBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    addAndMakeVisible(categoryBox);
    presetSearch.setTextToShowWhenEmpty("SEARCH PRESETS...", themeManager.getPalette().token("text_secondary"));
    presetSearch.setColour(juce::TextEditor::backgroundColourId, themeManager.getPalette().token("control_bg"));
    presetSearch.setColour(juce::TextEditor::textColourId, themeManager.getPalette().token("text_primary"));
    presetSearch.setColour(juce::TextEditor::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.4f));
    addAndMakeVisible(presetSearch);
    loadFactoryPresetCatalog();
    presetBox.onChange = [this] { if (presetBox.getSelectedId() > 0 && presetBox.getSelectedId() <= static_cast<int>(visiblePresetIndices.size())) applyFactoryPreset(visiblePresetIndices[static_cast<size_t>(presetBox.getSelectedId()-1)]); };
    auto refreshPresetList = [this] {
        // Keep the current sound selected when it remains in the filtered list.
        // Filtering must never silently load a different preset.
        int previouslySelectedPreset = -1;
        const int oldSelectedId = presetBox.getSelectedId();
        if (oldSelectedId > 0 && oldSelectedId <= static_cast<int>(visiblePresetIndices.size()))
            previouslySelectedPreset = visiblePresetIndices[static_cast<size_t>(oldSelectedId - 1)];

        const auto query = presetSearch.getText().trim().toLowerCase();
        const int category = categoryBox.getSelectedId();
        visiblePresetIndices.clear();
        presetBox.clear(juce::dontSendNotification);
        for (size_t i = 0; i < factoryPresets.size(); ++i) {
            const auto& item = factoryPresets[i];
            if (category == 2 && item.mode != "MELODY") continue;
            if (category == 3 && item.mode != "PATTERN") continue;

            const auto matchesQuery = query.isEmpty()
                || item.name.toLowerCase().contains(query)
                || item.category.toLowerCase().contains(query)
                || item.mode.toLowerCase().contains(query)
                || item.soundCharacter.toLowerCase().contains(query)
                || item.groove.toLowerCase().contains(query)
                || item.articulation.toLowerCase().contains(query);
            if (!matchesQuery) continue;

            visiblePresetIndices.push_back(static_cast<int>(i));
            presetBox.addItem(item.name, static_cast<int>(visiblePresetIndices.size()));
        }

        const auto selected = std::find(visiblePresetIndices.begin(), visiblePresetIndices.end(),
                                        previouslySelectedPreset);
        if (previouslySelectedPreset >= 0 && selected != visiblePresetIndices.end())
            presetBox.setSelectedId(static_cast<int>(std::distance(visiblePresetIndices.begin(), selected)) + 1,
                                    juce::dontSendNotification);
        else
            presetBox.setSelectedId(0, juce::dontSendNotification);
    };
    categoryBox.onChange = refreshPresetList;
    presetSearch.onTextChange = refreshPresetList;
    refreshPresetList();
    loadButton.setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("accent"));
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    loadButton.onClick = [this] { openImportGuitarPanel(); };
    addAndMakeVisible(loadButton);
    auxiliaryWorkspace = std::make_unique<PPGUMultiWindowWorkspace>();
    auxiliaryWorkspace->setSurfaceColours(themeManager.getPalette().token("window_bg"),
        themeManager.getPalette().token("panel"), themeManager.getPalette().token("control_border"),
        themeManager.getPalette().token("accent"), themeManager.getPalette().token("text_primary"));
    addAndMakeVisible(*auxiliaryWorkspace);
    auxiliaryWorkspace->setVisible(false);
    skinButton.setTooltip("Visual theme only. Ctrl+K opens the skin menu.");
    skinButton.onClick = [this] { showSkinMenu(); };
    addAndMakeVisible(skinButton);
    autoChordsButton.setTooltip("Open chord and harmony configuration workspace.");
    autoChordsButton.setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("control_bg"));
    autoChordsButton.setColour(juce::TextButton::textColourOffId, themeManager.getPalette().token("text_primary"));
    autoChordsButton.onClick = [this] { openAutoChordsPanel(); };
    addAndMakeVisible(autoChordsButton);
    statusLabel.setText("NO LIBRARY LOADED  /  LOAD A LICENSED SFZ INSTRUMENT", juce::dontSendNotification);
    const auto savedSfzPath = processor.getCurrentSfzFilePath();
    if (savedSfzPath.isNotEmpty())
    {
        const juce::File savedSfzFile(savedSfzPath);
        statusLabel.setText(savedSfzFile.existsAsFile()
            ? "SAVED LIBRARY  /  " + savedSfzFile.getFileName()
            : "LIBRARY FILE NOT FOUND  /  " + savedSfzFile.getFileName(),
            juce::dontSendNotification);
    }
    statusLabel.setColour(juce::Label::textColourId, themeManager.getPalette().token("text_secondary"));
    statusLabel.setFont(juce::Font(11.0f));
    addAndMakeVisible(statusLabel);
    melodyTab.setClickingTogglesState(false);
    patternTab.setClickingTogglesState(false);
    for (auto* b : { &melodyTab, &patternTab }) {
        b->setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("control_bg"));
        b->setColour(juce::TextButton::textColourOffId, themeManager.getPalette().token("text_primary"));
        addAndMakeVisible(*b);
    }
    modeBox.addItem("MELODY", 1);
    modeBox.addItem("PATTERN", 2);
    modeBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    modeBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    modeBox.setColour(juce::ComboBox::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.5f));
    addAndMakeVisible(modeBox);
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "playMode", modeBox);
    startModeBox.addItem("HOST", 1);
    startModeBox.addItem("HOST START", 2);
    startModeBox.addItem("INTERNAL", 3);
    startModeBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    startModeBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    startModeBox.setColour(juce::ComboBox::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.5f));
    addAndMakeVisible(startModeBox);
    startModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "syncMode", startModeBox);
    keyboard = std::make_unique<juce::MidiKeyboardComponent>(processor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard);
    keyboard->setAvailableRange(36, 84);
    keyboard->setKeyWidth(18.0f);
    keyboard->setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xffe8e7eb));
    keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff17171d));
    keyboard->setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colour(0xff34343d));
    keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, themeManager.getPalette().token("accent").withAlpha(0.35f));
    keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, themeManager.getPalette().token("accent").withAlpha(0.85f));
    addAndMakeVisible(*keyboard);
    performanceDisplay = std::make_unique<PPGUPerformanceDisplay>(processor);
    addAndMakeVisible(*performanceDisplay);
    melodyTab.onClick = [this] { modeBox.setSelectedId(1); };
    patternTab.onClick = [this] { modeBox.setSelectedId(2); };
    for (size_t i = 0; i < knobs.size(); ++i) {
        configureKnob(knobs[i], knobLabels[i], knobNames[i]);
        knobAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, parameterIDs[i], knobs[i]);
    }
    for (size_t i = 0; i < fxButtons.size(); ++i) {
        auto& b = fxButtons[i];
        b.setClickingTogglesState(true);
        b.setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("control_bg"));
        b.setColour(juce::TextButton::buttonOnColourId, themeManager.getPalette().token("accent"));
        b.setColour(juce::TextButton::textColourOffId, themeManager.getPalette().token("text_primary"));
        b.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        fxAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters, fxParameterIDs[i], b);
        b.onClick = [this, i] { openFxEditPanel(static_cast<int>(i)); };
        addAndMakeVisible(b);
    }
    ampTitle.setText("AMP & TONE", juce::dontSendNotification);
    fxTitle.setText("FX CHAIN", juce::dontSendNotification);
    playbackTitle.setText("PLAYBACK", juce::dontSendNotification);
    for (auto* l : { &ampTitle, &fxTitle, &playbackTitle }) {
        l->setFont(juce::Font(15.0f, juce::Font::bold));
        l->setColour(juce::Label::textColourId, themeManager.getPalette().token("accent"));
        addAndMakeVisible(*l);
    }
    juce::PropertiesFile::Options options;
    options.applicationName = "PPGU";
    options.filenameSuffix = "settings";
    options.folderName = "L3XTOWN";
    uiProperties.setStorageParameters(options);
    loadSavedSkin();
    applyPalette();
}

PopPunkGuitarUltimateAudioProcessorEditor::~PopPunkGuitarUltimateAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void PopPunkGuitarUltimateAudioProcessorEditor::configureKnob(juce::Slider& s, juce::Label& l, const juce::String& name)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 46, 16);
    s.setColour(juce::Slider::rotarySliderFillColourId, themeManager.getPalette().token("accent"));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff44444e));
    s.setColour(juce::Slider::thumbColourId, themeManager.getPalette().token("text_primary"));
    s.setColour(juce::Slider::textBoxTextColourId, themeManager.getPalette().token("text_primary"));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(s);
    l.setText(name, juce::dontSendNotification);
    l.setFont(juce::Font(9.5f, juce::Font::bold));
    l.setColour(juce::Label::textColourId, themeManager.getPalette().token("text_secondary"));
    l.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(l);
}

void PopPunkGuitarUltimateAudioProcessorEditor::drawPanel(juce::Graphics& g, juce::Rectangle<int> area)
{
    const auto& p = themeManager.getPalette();
    auto bounds = area.toFloat();
    g.setGradientFill(juce::ColourGradient(p.token("panel_elevated"), bounds.getX(), bounds.getY(),
                                           p.token("panel"), bounds.getX(), bounds.getBottom(), false));
    g.fillRoundedRectangle(bounds, 12.0f);
    g.setColour(p.token("control_border").withAlpha(0.78f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 12.0f, 1.0f);
    g.setColour(p.token("accent"));
    g.fillRoundedRectangle(bounds.getX() + 16.0f, bounds.getY() + 1.0f, 42.0f, 2.0f, 1.0f);
}

void PopPunkGuitarUltimateAudioProcessorEditor::paint(juce::Graphics& g)
{
 const auto& p=themeManager.getPalette();g.fillAll(p.token("window_bg"));g.setColour(p.token("lowlight").withAlpha(.35f));
 for(int x=16;x<getWidth();x+=32)g.drawVerticalLine(x,0,(float)getHeight());
 for(int y=16;y<getHeight();y+=32)g.drawHorizontalLine(y,0,(float)getWidth());
 g.setGradientFill(juce::ColourGradient(p.token("control_bg"),0,0,p.token("window_bg"),0,74,false));g.fillRect(0,0,getWidth(),76);g.setColour(p.token("accent"));g.fillRect(0,0,getWidth(),3);
 auto hero=getLocalBounds().reduced(18);hero.removeFromTop(66);hero.setHeight(184);
 g.setGradientFill(juce::ColourGradient(p.token("panel_elevated").brighter(.08f),(float)hero.getX(),(float)hero.getY(),p.token("control_bg"),(float)hero.getRight(),(float)hero.getBottom(),false));
 g.fillRoundedRectangle(hero.toFloat(),13);g.setColour(p.token("control_border"));g.drawRoundedRectangle(hero.toFloat().reduced(.5f),13,1);
 auto t=hero.reduced(28,17);g.setColour(p.token("accent_glow"));g.setFont(juce::Font(9.5f,juce::Font::bold));g.drawText("L3XTOWN AUDIO LAB  /  SIGNATURE INSTRUMENT",t.removeFromTop(18),juce::Justification::left);
 g.setColour(p.token("text_primary"));g.setFont(juce::Font(33,juce::Font::bold));g.drawText("POP PUNK GUITAR",t.removeFromTop(43),juce::Justification::left);
 g.setColour(p.token("accent"));g.setFont(juce::Font(39,juce::Font::bold));g.drawText("ULTIMATE",t.removeFromTop(45),juce::Justification::left);
 g.setColour(p.token("text_secondary"));g.setFont(juce::Font(10.5f));g.drawText("RIFFS  /  HOOKS  /  POWER CHORDS  /  MELODY",t.removeFromTop(17),juce::Justification::left);
 auto gr=hero.removeFromRight((int)(hero.getWidth()*.39f)).reduced(24,12);float x=(float)gr.getX(),y=(float)gr.getY(),w=(float)gr.getWidth(),h=(float)gr.getHeight();
 juce::Path neck;neck.addQuadrilateral(x+w*.56f,y+h*.43f,x+w*.96f,y+h*.05f,x+w*.99f,y+h*.17f,x+w*.65f,y+h*.57f);
 g.setGradientFill(juce::ColourGradient(juce::Colour(0xff8e9299),x+w*.5f,y,juce::Colour(0xff292d35),x+w,y+h,false));g.fillPath(neck);
 juce::Path body;body.startNewSubPath(x+w*.48f,y+h*.48f);body.cubicTo(x+w*.28f,y+h*.24f,x+w*.12f,y+h*.44f,x+w*.27f,y+h*.67f);body.cubicTo(x+w*.37f,y+h*.86f,x+w*.61f,y+h*.83f,x+w*.70f,y+h*.62f);body.cubicTo(x+w*.83f,y+h*.58f,x+w*.84f,y+h*.40f,x+w*.68f,y+h*.39f);body.closeSubPath();
 g.setGradientFill(juce::ColourGradient(juce::Colour(0xffc5c8ce),x+w*.2f,y+h*.3f,juce::Colour(0xff353a43),x+w*.8f,y+h*.8f,false));g.fillPath(body);g.setColour(p.token("control_border"));g.strokePath(body,juce::PathStrokeType(1.2f));
 g.setColour(juce::Colour(0xff171a20));g.fillRoundedRectangle(x+w*.42f,y+h*.45f,w*.22f,h*.18f,4);g.setColour(juce::Colour(0xffaeb4bc));g.fillRoundedRectangle(x+w*.43f,y+h*.49f,w*.20f,h*.035f,2);g.fillRoundedRectangle(x+w*.43f,y+h*.56f,w*.20f,h*.035f,2);
 for(int i=0;i<6;++i){float o=(float)i*1.4f;g.setColour(juce::Colours::white.withAlpha(.72f));g.drawLine(x+w*.55f+o,y+h*.46f,x+w*.96f+o*.2f,y+h*.12f,.65f);}
 g.setColour(p.token("accent"));g.fillEllipse(x+w*.56f,y+h*.69f,7,7);g.fillEllipse(x+w*.64f,y+h*.66f,7,7);
 auto panels=getLocalBounds().reduced(18);panels.removeFromTop(356);panels.removeFromBottom(152);int gap=12,usable=panels.getWidth()-gap*2,aw=(int)(usable*.38f),fw=(int)(usable*.35f);
 auto amp=panels.removeFromLeft(aw);panels.removeFromLeft(gap);auto fx=panels.removeFromLeft(fw);panels.removeFromLeft(gap);drawPanel(g,amp);drawPanel(g,fx);drawPanel(g,panels);
}

void PopPunkGuitarUltimateAudioProcessorEditor::resized()
{
 if (auxiliaryWorkspace != nullptr)
        auxiliaryWorkspace->setBounds(getLocalBounds().reduced(18));
    auto a=getLocalBounds().reduced(18);auto head=a.removeFromTop(56);titleLabel.setBounds(head.removeFromLeft(150).removeFromTop(29));subtitleLabel.setBounds(18,47,150,16);head.removeFromLeft(10);
 presetBox.setBounds(head.removeFromLeft(205).reduced(2,8));presetSearch.setBounds(head.removeFromLeft(160).reduced(2,8));categoryBox.setBounds(head.removeFromLeft(128).reduced(2,8));
 skinButton.setBounds(head.removeFromRight(94).reduced(2,8));autoChordsButton.setBounds(head.removeFromRight(112).reduced(2,8));loadButton.setBounds(head.removeFromRight(106).reduced(2,8));
 a.removeFromTop(10);a.removeFromTop(184);a.removeFromTop(10);auto tabs=a.removeFromTop(52);melodyTab.setBounds(tabs.removeFromLeft(190).reduced(2,3));patternTab.setBounds(tabs.removeFromLeft(190).reduced(8,3));modeBox.setBounds(tabs.removeFromRight(145).reduced(2,7));
 a.removeFromTop(8);statusLabel.setBounds(a.removeFromTop(26));a.removeFromTop(10);auto panels=a.removeFromTop(296);
 int gap=12,usable=panels.getWidth()-gap*2,aw=(int)(usable*.38f),fw=(int)(usable*.35f);auto amp=panels.removeFromLeft(aw);panels.removeFromLeft(gap);auto fx=panels.removeFromLeft(fw);panels.removeFromLeft(gap);auto play=panels;
 ampTitle.setBounds(amp.removeFromTop(38).reduced(14,2));fxTitle.setBounds(fx.removeFromTop(38).reduced(14,2));playbackTitle.setBounds(play.removeFromTop(38).reduced(12,2));
 int kw=juce::jmax(1,amp.getWidth()/4);for(size_t i=0;i<4;++i){auto cell=amp.removeFromLeft(kw);knobLabels[i].setBounds(cell.removeFromBottom(23));knobs[i].setBounds(cell.reduced(5,8));}
 for(size_t i=0;i<fxButtons.size();++i){auto row=fx.removeFromTop(fx.getHeight()/(int)(fxButtons.size()-i));fxButtons[i].setBounds(row.reduced(10,3));}
 auto controls=play.removeFromTop(214);int pw=juce::jmax(1,controls.getWidth()/5);for(size_t i=4;i<9;++i){auto cell=controls.removeFromLeft(pw);knobLabels[i].setBounds(cell.removeFromBottom(23));knobs[i].setBounds(cell.reduced(1,8));}
 startModeBox.setBounds(play.removeFromTop(36).reduced(8,2));a.removeFromTop(8);auto display=a.removeFromTop(52);if(performanceDisplay)performanceDisplay->setBounds(display);a.removeFromTop(8);auto keys=a.removeFromBottom(82);if(keyboard)keyboard->setBounds(keys.reduced(2,4));
}

juce::File PopPunkGuitarUltimateAudioProcessorEditor::getRuntimeSkinRoot() const
{
#if JUCE_WINDOWS
 HMODULE module=nullptr;auto address=reinterpret_cast<LPCWSTR>(reinterpret_cast<std::uintptr_t>(&getPluginModuleAnchor));
 DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
 if(GetModuleHandleExW(flags,address,&module)!=0&&module){std::array<wchar_t,32768> path{};DWORD n=GetModuleFileNameW(module,path.data(),(DWORD)path.size());
  if(n>0&&n<path.size()){juce::File f(fromWidePath(path.data(), static_cast<int>(n)));for(int i=0;i<8;++i){auto d=f.isDirectory()?f:f.getParentDirectory();if(d.getFileName().endsWithIgnoreCase(".vst3"))return d.getParentDirectory().getChildFile("Skins");f=d.getParentDirectory();}}}
#endif
 auto exe=juce::File::getSpecialLocation(juce::File::currentExecutableFile);auto beside=exe.getParentDirectory().getChildFile("Skins");if(beside.isDirectory())return beside;
 auto dev=juce::File::getCurrentWorkingDirectory().getChildFile("SKIN");return dev.isDirectory()?dev:beside;
}
int PopPunkGuitarUltimateAudioProcessorEditor::getSkinCount() const
{
    const auto registryFile = runtimeSkinRoot.getChildFile("theme_manifest.json");
    if (!registryFile.existsAsFile())
        return 0;

    const auto registry = juce::JSON::parse(registryFile.loadFileAsString());
    if (!registry.isObject())
        return 0;

    const auto totalValue = registry.getProperty("totalSkins", juce::var());
    const auto themesValue = registry.getProperty("themes", juce::var());
    const auto* themes = themesValue.getArray();
    if ((!totalValue.isInt() && !totalValue.isInt64()) || themes == nullptr)
        return 0;

    const auto total64 = totalValue.toString().getLargeIntValue();
    if (total64 < 1 || total64 > std::numeric_limits<int>::max()
        || total64 != static_cast<std::int64_t>(themes->size()))
        return 0;
    const auto total = static_cast<int>(total64);

    std::set<int> indices;
    std::set<std::string> ids;
    for (const auto& entry : *themes)
    {
        if (!entry.isObject())
            return 0;

        const auto indexValue = entry.getProperty("index", juce::var());
        const auto id = entry.getProperty("id", "").toString();
        const auto path = entry.getProperty("path", "").toString();
        if ((!indexValue.isInt() && !indexValue.isInt64())
            || id.isEmpty() || path.isEmpty()
            || path.startsWith("/") || path.contains("..")
            || !path.startsWith("Skin_") || !path.endsWith("/theme.json"))
            return 0;

        const auto index64 = indexValue.toString().getLargeIntValue();
        if (index64 < 1 || index64 > total || !id.startsWith("skin-")
            || id.substring(5).getLargeIntValue() != index64
            || !indices.insert(static_cast<int>(index64)).second
            || !ids.insert(id.toStdString()).second)
            return 0;
    }

    return static_cast<int>(indices.size()) == total ? total : 0;
}

juce::File PopPunkGuitarUltimateAudioProcessorEditor::getSkinManifest(int index) const
{
    if (index < 1)
        return {};

    const auto registryFile = runtimeSkinRoot.getChildFile("theme_manifest.json");
    if (!registryFile.existsAsFile())
        return {};

    const auto registry = juce::JSON::parse(registryFile.loadFileAsString());
    const auto themesValue = registry.getProperty("themes", juce::var());
    const auto* themes = themesValue.getArray();
    if (themes == nullptr)
        return {};

    for (const auto& entry : *themes)
    {
        if (!entry.isObject())
            continue;
        const auto indexValue = entry.getProperty("index", juce::var());
        if ((!indexValue.isInt() && !indexValue.isInt64())
            || indexValue.toString().getLargeIntValue() != index)
            continue;

        const auto relativePath = entry.getProperty("path", "").toString();
        if (relativePath.isEmpty() || relativePath.startsWith("/")
            || relativePath.contains("..") || !relativePath.startsWith("Skin_")
            || !relativePath.endsWith("/theme.json"))
            return {};

        const auto manifest = runtimeSkinRoot.getChildFile(relativePath);
        return manifest.isAChildOf(runtimeSkinRoot) ? manifest : juce::File{};
    }
    return {};
}

void PopPunkGuitarUltimateAudioProcessorEditor::loadSavedSkin()
{
    auto* settings = uiProperties.getUserSettings();
    const auto id = settings ? settings->getValue("skinId", "skin-052") : juce::String("skin-052");
    const auto index = id.startsWith("skin-") ? id.substring(5).getIntValue() : 0;
    if (index < 1 || index > getSkinCount() || !loadSkinByIndex(index, false))
    {
        themeManager.resetToDefault();
        applyPalette();
    }
}

bool PopPunkGuitarUltimateAudioProcessorEditor::loadSkinByIndex(int index, bool save)
{
    if (index < 1 || index > getSkinCount())
        return false;

    const auto manifest = getSkinManifest(index);
    const bool loaded = manifest.existsAsFile() && themeManager.loadFromFile(manifest);
    if (!loaded)
        themeManager.resetToDefault();
    applyPalette();
    if (save && loaded)
        persistSkinPreference();
    return loaded;
}

void PopPunkGuitarUltimateAudioProcessorEditor::showSkinMenu()
{
    juce::PopupMenu menu;
    menu.addSectionHeader("SELECT VISUAL SHELL");
    const auto skinCount = getSkinCount();
    for (int index = 1; index <= skinCount; ++index)
    {
        PPGUThemeManager candidate;
        const auto manifest = getSkinManifest(index);
        if (manifest.existsAsFile() && candidate.loadFromFile(manifest))
        {
            const auto palette = candidate.getPalette();
            menu.addItem(index, palette.name, true, palette.id == themeManager.getPalette().id);
        }
    }

    if (menu.getNumItems() == 0)
        menu.addSectionHeader("NO VALID THEMES FOUND");

    juce::Component::SafePointer<PopPunkGuitarUltimateAudioProcessorEditor> safe(this);
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&skinButton),
        [safe](int selectedIndex)
        {
            if (safe && selectedIndex >= 1 && selectedIndex <= safe->getSkinCount())
                safe->loadSkinByIndex(selectedIndex, true);
        });
}
void PopPunkGuitarUltimateAudioProcessorEditor::applyPalette()
{
 const auto& p=themeManager.getPalette();premiumLookAndFeel.setPalette(p);if(performanceDisplay)performanceDisplay->setPalette(p);
 if (auxiliaryWorkspace != nullptr)
     auxiliaryWorkspace->setSurfaceColours(p.token("window_bg"), p.token("panel"),
         p.token("control_border"), p.token("accent"), p.token("text_primary"));
 titleLabel.setColour(juce::Label::textColourId,p.token("text_primary"));subtitleLabel.setColour(juce::Label::textColourId,p.token("text_secondary"));statusLabel.setColour(juce::Label::textColourId,p.token("text_secondary"));
 ampTitle.setColour(juce::Label::textColourId,p.token("accent"));fxTitle.setColour(juce::Label::textColourId,p.token("accent"));playbackTitle.setColour(juce::Label::textColourId,p.token("accent"));
 for(auto* b:{&presetBox,&categoryBox,&modeBox,&startModeBox}){b->setColour(juce::ComboBox::backgroundColourId,p.token("control_bg"));b->setColour(juce::ComboBox::textColourId,p.token("text_primary"));b->setColour(juce::ComboBox::outlineColourId,p.token("control_border"));b->setColour(juce::ComboBox::arrowColourId,p.token("accent"));}
 presetSearch.setColour(juce::TextEditor::backgroundColourId,p.token("control_bg"));presetSearch.setColour(juce::TextEditor::textColourId,p.token("text_primary"));presetSearch.setColour(juce::TextEditor::outlineColourId,p.token("control_border"));presetSearch.setColour(juce::TextEditor::focusedOutlineColourId,p.token("highlight"));presetSearch.setColour(juce::TextEditor::highlightColourId,p.token("accent").withAlpha(.3f));
 loadButton.setColour(juce::TextButton::buttonColourId,p.token("accent"));loadButton.setColour(juce::TextButton::textColourOffId,p.token("window_bg"));skinButton.setColour(juce::TextButton::buttonColourId,p.token("panel_elevated"));skinButton.setColour(juce::TextButton::textColourOffId,p.token("text_primary"));skinButton.setTooltip("Active skin: "+p.name+"  /  Ctrl+K");
 for(auto* b:{&melodyTab,&patternTab}){b->setColour(juce::TextButton::buttonColourId,p.token("control_bg"));b->setColour(juce::TextButton::buttonOnColourId,p.token("accent"));b->setColour(juce::TextButton::textColourOffId,p.token("text_primary"));b->setColour(juce::TextButton::textColourOnId,p.token("window_bg"));}
 for(auto& s:knobs){s.setColour(juce::Slider::rotarySliderFillColourId,p.token("accent"));s.setColour(juce::Slider::rotarySliderOutlineColourId,p.token("control_border"));s.setColour(juce::Slider::thumbColourId,p.token("text_primary"));s.setColour(juce::Slider::textBoxTextColourId,p.token("text_primary"));}
 for(auto& l:knobLabels)l.setColour(juce::Label::textColourId,p.token("text_secondary"));
 for(auto& b:fxButtons){b.setColour(juce::TextButton::buttonColourId,p.token("control_bg"));b.setColour(juce::TextButton::buttonOnColourId,p.token("accent"));b.setColour(juce::TextButton::textColourOffId,p.token("text_primary"));b.setColour(juce::TextButton::textColourOnId,p.token("window_bg"));}
 if(keyboard){keyboard->setColour(juce::MidiKeyboardComponent::whiteNoteColourId,juce::Colour(0xffeaf0f2));keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId,p.token("control_bg"));keyboard->setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId,p.token("control_border"));keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId,p.token("accent_glow").withAlpha(.3f));keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId,p.token("accent").withAlpha(.88f));}
 repaint();
}
void PopPunkGuitarUltimateAudioProcessorEditor::persistSkinPreference(){if(auto* s=uiProperties.getUserSettings()){s->setValue("skinId",themeManager.getPalette().id);s->saveIfNeeded();}}
bool PopPunkGuitarUltimateAudioProcessorEditor::keyPressed(const juce::KeyPress& k){if(k.getKeyCode()=='K'&&k.getModifiers().isCtrlDown()){showSkinMenu();return true;}return juce::AudioProcessorEditor::keyPressed(k);}

void PopPunkGuitarUltimateAudioProcessorEditor::loadFactoryPresetCatalog()
{
    factoryPresets.clear();
    const auto jsonText = juce::String::fromUTF8(BinaryData::factory_bank_300_json, BinaryData::factory_bank_300_jsonSize);
    const auto root = juce::JSON::parse(jsonText);
    if (!root.isObject()) { statusLabel.setText("FACTORY PRESET CATALOG ERROR", juce::dontSendNotification); return; }
    const auto append = [this](const juce::var& array, const juce::String& mode) {
        if (auto* values = array.getArray()) for (const auto& value : *values) {
            if (!value.isObject()) continue;
            FactoryPreset preset;
            auto reference = value.getProperty("referenceArtist", "").toString();
            auto displayName = value.getProperty("name", "Unnamed").toString();
            if (reference.isNotEmpty() && displayName.startsWithIgnoreCase(reference))
                displayName = displayName.substring(reference.length()).trim();
            preset.name = displayName;
            preset.category = "Original";
            preset.mode = mode;
            preset.soundCharacter = value.getProperty("soundCharacter", "Electric").toString();
            preset.groove = value.getProperty("groove", "").toString();
            preset.articulation = value.getProperty("articulation", "").toString();
            const auto params = value.getProperty("parameters", juce::var());
            preset.drive = static_cast<float>(params.getProperty("drive", 0.0));
            preset.tone = static_cast<float>(params.getProperty("tone", 5.0));
            preset.reverb = static_cast<float>(params.getProperty("reverb", 0.0));
            preset.outputDb = static_cast<float>(params.getProperty("outputDb", 0.0));
            factoryPresets.push_back(std::move(preset));
        }
    };
    append(root.getProperty("melodyPresets", juce::var()), "MELODY");
    append(root.getProperty("patternPresets", juce::var()), "PATTERN");
    statusLabel.setText("FACTORY CATALOG: "+juce::String(static_cast<int>(factoryPresets.size()))+" PRESETS", juce::dontSendNotification);
}

void PopPunkGuitarUltimateAudioProcessorEditor::applyFactoryPreset(int index)
{
    if (index < 0 || index >= static_cast<int>(factoryPresets.size())) return;
    const auto& preset = factoryPresets[static_cast<size_t>(index)];
    const auto setParameter = [this](const juce::String& id, float value) {
        if (auto* parameter = processor.parameters.getParameter(id))
            parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
    };
    setParameter("distortionDrive", juce::jlimit(0.0f, 1.0f, preset.drive));
    setParameter("tone", juce::jlimit(0.0f, 10.0f, preset.tone));
    setParameter("reverbMix", juce::jlimit(0.0f, 1.0f, preset.reverb));
    setParameter("output", juce::jlimit(-24.0f, 12.0f, preset.outputDb));
    modeBox.setSelectedId(preset.mode == "PATTERN" ? 2 : 1, juce::sendNotification);
    if (preset.mode == "PATTERN")
    {
        const auto detail = preset.soundCharacter + (preset.groove.isNotEmpty() ? "  /  " + preset.groove : juce::String{});
        statusLabel.setText("PATTERN STYLE: " + detail + "  /  " + preset.articulation + "  /  PERFORMANCE ENGINE REQUIRED", juce::dontSendNotification);
    }
    else
        statusLabel.setText("MELODY TONE: " + preset.soundCharacter + "  /  ONE KEY = ONE NOTE  /  LOAD A LICENSED SFZ BANK", juce::dontSendNotification);
}
void PopPunkGuitarUltimateAudioProcessorEditor::openFxEditPanel(int selectedEffect)
{
    if (auxiliaryWorkspace == nullptr)
        return;
    constexpr int fxEditPanelId = 2101;
    if (!auxiliaryWorkspace->isPanelOpen(fxEditPanelId))
    {
        PPGUMultiWindowWorkspace::PanelSpec spec;
        spec.id = fxEditPanelId;
        spec.title = "FX EDIT  /  SIGNAL FORGE";
        spec.desired = { 1000, 680 };
        spec.minimum = { 620, 440 };
        spec.content = std::make_unique<PPGUFXEditPanel>(processor.parameters, selectedEffect);
        if (!auxiliaryWorkspace->openPanel(std::move(spec)))
            return;
    }
    auxiliaryWorkspace->setVisible(true);
    auxiliaryWorkspace->toFront(false);
}

void PopPunkGuitarUltimateAudioProcessorEditor::openAutoChordsPanel()
{
    if (auxiliaryWorkspace == nullptr)
        return;
    constexpr int autoChordsPanelId = 2102;
    if (!auxiliaryWorkspace->isPanelOpen(autoChordsPanelId))
    {
        PPGUMultiWindowWorkspace::PanelSpec spec;
        spec.id = autoChordsPanelId;
        spec.title = "AUTOCHORDS  /  HARMONY WORKSPACE";
        spec.desired = { 900, 620 };
        spec.minimum = { 560, 420 };
        spec.content = std::make_unique<PPGUAutoChordsPanel>(processor.parameters);
        if (!auxiliaryWorkspace->openPanel(std::move(spec)))
            return;
    }
    auxiliaryWorkspace->setVisible(true);
    auxiliaryWorkspace->toFront(false);
}

void PopPunkGuitarUltimateAudioProcessorEditor::openImportGuitarPanel()
{
    if (auxiliaryWorkspace == nullptr)
        return;
    constexpr int importGuitarPanelId = 1001;
    if (!auxiliaryWorkspace->isPanelOpen(importGuitarPanelId))
    {
        juce::Component::SafePointer<PopPunkGuitarUltimateAudioProcessorEditor> safeThis(this);
        PPGUMultiWindowWorkspace::PanelSpec spec;
        spec.id = importGuitarPanelId;
        spec.title = "IMPORT GUITAR";
        spec.desired = { 560, 360 };
        spec.minimum = { 300, 220 };
        spec.content = std::make_unique<PPGUImportGuitarPanel>(
            [safeThis](juce::Label* targetStatus, juce::Button* targetButton)
            {
                if (safeThis != nullptr)
                    safeThis->chooseSfzFile(targetStatus, targetButton);
            });
        if (!auxiliaryWorkspace->openPanel(std::move(spec)))
            return;
    }
    auxiliaryWorkspace->setVisible(true);
    auxiliaryWorkspace->toFront(false);
    resized();
}

void PopPunkGuitarUltimateAudioProcessorEditor::chooseSfzFile(juce::Label* panelStatus, juce::Button* panelLoadButton)
{
    fileChooser=std::make_unique<juce::FileChooser>("Choose an SFZ guitar instrument",juce::File::getSpecialLocation(juce::File::userHomeDirectory),"*.sfz");
    juce::Component::SafePointer<PopPunkGuitarUltimateAudioProcessorEditor> safeThis(this);
    juce::Component::SafePointer<juce::Label> safePanelStatus(panelStatus);
    juce::Component::SafePointer<juce::Button> safePanelLoadButton(panelLoadButton);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safeThis, safePanelStatus, safePanelLoadButton](const juce::FileChooser& chooser){
        if(safeThis==nullptr) return; const auto selected=chooser.getResult(); if(selected==juce::File{}) return;
        safeThis->loadButton.setEnabled(false);
        if (safePanelLoadButton != nullptr)
            safePanelLoadButton->setEnabled(false);
        safeThis->statusLabel.setText("LOADING SFZ IN BACKGROUND...", juce::dontSendNotification);
        if (safePanelStatus != nullptr)
            safePanelStatus->setText("Loading SFZ in background...", juce::dontSendNotification);
        safeThis->processor.loadSfzFileAsync(selected, [safeThis, safePanelStatus, safePanelLoadButton](ppgu::sampler::LoadReport report)
        {
            if (safeThis == nullptr)
                return;
            safeThis->loadButton.setEnabled(true);
            if (safePanelLoadButton != nullptr)
                safePanelLoadButton->setEnabled(true);
            if (report.success)
            {
                const auto readyText = "READY  /  " + juce::String(static_cast<int>(report.loadedRegions))
                    + " REGIONS  /  " + juce::String(static_cast<int>(report.loadedSamples)) + " WAV SAMPLES";
                safeThis->statusLabel.setText(readyText, juce::dontSendNotification);
                if (safePanelStatus != nullptr)
                    safePanelStatus->setText(readyText, juce::dontSendNotification);
            }
            else
            {
                const auto errorText = report.errors.empty()
                    ? juce::String("SFZ LOAD FAILED") : juce::String(report.errors.front());
                safeThis->statusLabel.setText(errorText, juce::dontSendNotification);
                if (safePanelStatus != nullptr)
                    safePanelStatus->setText(errorText, juce::dontSendNotification);
            }
        });
    });
}
