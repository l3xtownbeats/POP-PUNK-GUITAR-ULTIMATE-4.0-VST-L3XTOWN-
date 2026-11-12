#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>

class PPGUPerformanceDisplay final : public juce::Component,
                                     private juce::Timer,
                                     private juce::MidiKeyboardState::Listener
{
public:
    explicit PPGUPerformanceDisplay(juce::MidiKeyboardState& state);
    ~PPGUPerformanceDisplay() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void handleNoteOn(juce::MidiKeyboardState*, int channel, int note, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState*, int channel, int note, float velocity) override;
    void timerCallback() override;
    juce::String makeNoteName(int note) const;
    juce::String makeChordName(const std::array<bool, 128>& active) const;

    juce::MidiKeyboardState& keyboardState;
    std::array<std::atomic<bool>, 128> activeNotes {};
    std::atomic<int> latestNote { -1 };
    std::atomic<int> latestVelocity { 0 };
    juce::Label noteValue, chordValue, detailValue, velocityValue, midiValue;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PPGUPerformanceDisplay)
};
