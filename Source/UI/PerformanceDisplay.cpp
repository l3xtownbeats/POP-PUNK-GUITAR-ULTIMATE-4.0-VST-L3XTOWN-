#include "PerformanceDisplay.h"

#include <algorithm>
#include <array>
#include <cmath>
#include "../Performance/AutoChordEngine.h"

PPGUPerformanceDisplay::PPGUPerformanceDisplay(juce::MidiKeyboardState& state,
                                               const PPGUThemeManager::Palette& initialPalette)
    : keyboardState(state), palette(initialPalette)
{
    for (auto& note : activeNotes) note.store(false, std::memory_order_relaxed);
    auto configure = [](juce::Label& label, float size, juce::Colour colour, bool bold)
    {
        label.setFont(juce::Font(size, bold ? juce::Font::bold : juce::Font::plain));
        label.setColour(juce::Label::textColourId, colour);
        label.setJustificationType(juce::Justification::centredLeft);
        label.setInterceptsMouseClicks(false, false);
    };
    configure(noteValue, 21.0f, palette.textPrimary, true);
    configure(chordValue, 26.0f, palette.accent, true);
    configure(detailValue, 10.0f, palette.textSecondary, false);
    configure(velocityValue, 11.0f, palette.textPrimary, true);
    configure(midiValue, 10.0f, palette.textSecondary, true);
    noteValue.setText("--", juce::dontSendNotification);
    chordValue.setText("--", juce::dontSendNotification);
    detailValue.setText("PLAY A NOTE OR CHORD  /  LIVE MIDI MONITOR", juce::dontSendNotification);
    velocityValue.setText("VEL --", juce::dontSendNotification);
    midiValue.setText("MIDI IDLE", juce::dontSendNotification);
    const std::array<juce::Component*, 5> labels {
        &noteValue, &chordValue, &detailValue, &velocityValue, &midiValue
    };
    for (auto* component : labels)
        addAndMakeVisible(*component);
    keyboardState.addListener(this);
    setPalette(palette);
}

void PPGUPerformanceDisplay::setPalette(const PPGUThemeManager::Palette& value)
{
    palette = value;
    noteValue.setColour(juce::Label::textColourId, palette.textPrimary);
    chordValue.setColour(juce::Label::textColourId, palette.accent);
    detailValue.setColour(juce::Label::textColourId, palette.textSecondary);
    velocityValue.setColour(juce::Label::textColourId, palette.textPrimary);
    midiValue.setColour(juce::Label::textColourId, palette.textSecondary);
    repaint();
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
    std::array<int, 128> notes {};
    std::size_t count = 0;
    for (int note = 0; note < 128; ++note)
        if (active[static_cast<std::size_t>(note)])
            notes[count++] = note;
    if (count == 0) return "--";
    const auto chord = ppgu::performance::AutoChordEngine::detect(notes.data(), count);
    if (!chord.valid()) return count == 1 ? "NOTE" : "NOTE SET";
    static const char* names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    const char* suffix = "";
    using ppgu::performance::ChordQuality;
    switch (chord.quality)
    {
        case ChordQuality::Major: suffix = ""; break;
        case ChordQuality::Minor: suffix = "m"; break;
        case ChordQuality::Diminished: suffix = "dim"; break;
        case ChordQuality::Augmented: suffix = "aug"; break;
        case ChordQuality::Sus2: suffix = "sus2"; break;
        case ChordQuality::Sus4: suffix = "sus4"; break;
        case ChordQuality::Dominant7: suffix = "7"; break;
        case ChordQuality::Major7: suffix = "maj7"; break;
        case ChordQuality::Minor7: suffix = "m7"; break;
        case ChordQuality::Power: suffix = "5"; break;
        default: return "NOTE SET";
    }
    return juce::String(names[chord.rootPitchClass]) + suffix;
}

void PPGUPerformanceDisplay::visibilityChanged()
{
    if (isShowing())
        startTimerHz(20);
    else
        stopTimer();
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
    g.setGradientFill(juce::ColourGradient(palette.panelElevated.brighter(0.08f), b.getX(), b.getY(),
                                           palette.windowBg.darker(0.15f), b.getRight(), b.getBottom(), false));
    g.fillRoundedRectangle(b, 10.0f);
    g.setColour(palette.accent.withAlpha(0.85f));
    g.fillRoundedRectangle(b.getX(), b.getY() + 10.0f, 3.0f, b.getHeight() - 20.0f, 1.5f);
    g.setColour(palette.controlBorder);
    g.drawRoundedRectangle(b.reduced(0.5f), 10.0f, 1.0f);
    g.setColour(palette.textSecondary);
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
