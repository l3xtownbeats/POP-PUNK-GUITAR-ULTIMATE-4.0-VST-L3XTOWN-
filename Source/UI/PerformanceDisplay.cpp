#include "PerformanceDisplay.h"

#include <algorithm>
#include <cmath>

namespace
{
const juce::Colour displayBg { 0xff11151c };
const juce::Colour displayAccent { 0xffed238b };
const juce::Colour displayText { 0xfff4f0f4 };
const juce::Colour displayMuted { 0xff969eaa };
}

PPGUPerformanceDisplay::PPGUPerformanceDisplay(juce::MidiKeyboardState& state)
    : keyboardState(state)
{
    for (auto& note : activeNotes) note.store(false, std::memory_order_relaxed);
    auto configure = [](juce::Label& label, float size, juce::Colour colour, bool bold)
    {
        label.setFont(juce::Font(size, bold ? juce::Font::bold : juce::Font::plain));
        label.setColour(juce::Label::textColourId, colour);
        label.setJustificationType(juce::Justification::centredLeft);
        label.setInterceptsMouseClicks(false, false);
    };
    configure(noteValue, 21.0f, displayText, true);
    configure(chordValue, 26.0f, displayAccent, true);
    configure(detailValue, 10.0f, displayMuted, false);
    configure(velocityValue, 11.0f, displayText, true);
    configure(midiValue, 10.0f, displayMuted, true);
    noteValue.setText("--", juce::dontSendNotification);
    chordValue.setText("--", juce::dontSendNotification);
    detailValue.setText("PLAY A NOTE OR CHORD  /  LIVE MIDI MONITOR", juce::dontSendNotification);
    velocityValue.setText("VEL --", juce::dontSendNotification);
    midiValue.setText("MIDI IDLE", juce::dontSendNotification);
    for (auto* c : { static_cast<juce::Component*>(&noteValue), &chordValue, &detailValue, &velocityValue, &midiValue })
        addAndMakeVisible(*c);
    keyboardState.addListener(this);
    startTimerHz(20);
}

PPGUPerformanceDisplay::~PPGUPerformanceDisplay()
{
    stopTimer();
    keyboardState.removeListener(this);
}

void PPGUPerformanceDisplay::handleNoteOn(juce::MidiKeyboardState*, int, int note, float velocity)
{
    if (note < 0 || note >= 128) return;
    activeNotes[static_cast<std::size_t>(note)].store(true, std::memory_order_relaxed);
    latestNote.store(note, std::memory_order_relaxed);
    latestVelocity.store(juce::jlimit(0, 127, juce::roundToInt(velocity * 127.0f)), std::memory_order_relaxed);
}

void PPGUPerformanceDisplay::handleNoteOff(juce::MidiKeyboardState*, int, int note, float)
{
    if (note >= 0 && note < 128)
        activeNotes[static_cast<std::size_t>(note)].store(false, std::memory_order_relaxed);
}

juce::String PPGUPerformanceDisplay::makeNoteName(int note) const
{
    static const char* names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    return juce::String(names[note % 12]) + juce::String(note / 12 - 1);
}

juce::String PPGUPerformanceDisplay::makeChordName(const std::array<bool, 128>& active) const
{
    std::array<bool, 12> pitchClasses {};
    int unique = 0;
    for (int note = 0; note < 128; ++note)
        if (active[static_cast<std::size_t>(note)] && !pitchClasses[static_cast<std::size_t>(note % 12)])
        {
            pitchClasses[static_cast<std::size_t>(note % 12)] = true;
            ++unique;
        }
    if (unique < 3) return unique == 0 ? "--" : "NOTE";
    static const char* names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    for (int root = 0; root < 12; ++root)
    {
        const bool major = pitchClasses[static_cast<std::size_t>(root)]
            && pitchClasses[static_cast<std::size_t>((root + 4) % 12)]
            && pitchClasses[static_cast<std::size_t>((root + 7) % 12)];
        const bool minor = pitchClasses[static_cast<std::size_t>(root)]
            && pitchClasses[static_cast<std::size_t>((root + 3) % 12)]
            && pitchClasses[static_cast<std::size_t>((root + 7) % 12)];
        if (major && unique == 3) return juce::String(names[root]) + " Maj";
        if (minor && unique == 3) return juce::String(names[root]) + " Min";
    }
    return "NOTE SET";
}

void PPGUPerformanceDisplay::timerCallback()
{
    std::array<bool, 128> snapshot {};
    for (std::size_t i = 0; i < snapshot.size(); ++i)
        snapshot[i] = activeNotes[i].load(std::memory_order_relaxed);
    int count = 0;
    juce::String notes;
    for (int i = 0; i < 128; ++i)
        if (snapshot[static_cast<std::size_t>(i)])
        {
            if (count++ < 5) notes += (notes.isEmpty() ? "" : "  ") + makeNoteName(i);
        }
    const int note = latestNote.load(std::memory_order_relaxed);
    const int velocity = latestVelocity.load(std::memory_order_relaxed);
    noteValue.setText(note >= 0 ? makeNoteName(note) : "--", juce::dontSendNotification);
    chordValue.setText(makeChordName(snapshot), juce::dontSendNotification);
    detailValue.setText(count > 0 ? notes : "PLAY A NOTE OR CHORD  /  LIVE MIDI MONITOR", juce::dontSendNotification);
    velocityValue.setText(count > 0 ? "VEL " + juce::String(velocity) : "VEL --", juce::dontSendNotification);
    midiValue.setText(count > 0 ? "MIDI ACTIVE" : "MIDI IDLE", juce::dontSendNotification);
}

void PPGUPerformanceDisplay::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient(displayBg.brighter(0.08f), b.getX(), b.getY(),
                                           displayBg.darker(0.15f), b.getRight(), b.getBottom(), false));
    g.fillRoundedRectangle(b, 10.0f);
    g.setColour(displayAccent.withAlpha(0.85f));
    g.fillRoundedRectangle(b.getX(), b.getY() + 10.0f, 3.0f, b.getHeight() - 20.0f, 1.5f);
    g.setColour(juce::Colour(0xff303640));
    g.drawRoundedRectangle(b.reduced(0.5f), 10.0f, 1.0f);
    g.setColour(displayMuted);
    g.setFont(9.0f);
    g.drawText("PERFORMANCE", getLocalBounds().reduced(12, 4).removeFromTop(14), juce::Justification::left);
}

void PPGUPerformanceDisplay::resized()
{
    auto a = getLocalBounds().reduced(14, 20);
    auto left = a.removeFromLeft(76);
    noteValue.setBounds(left.removeFromTop(30));
    auto chord = a.removeFromLeft(130);
    chordValue.setBounds(chord.removeFromTop(32));
    auto right = a.removeFromRight(88);
    velocityValue.setBounds(right.removeFromTop(20));
    midiValue.setBounds(right.removeFromTop(18));
    detailValue.setBounds(a);
}
