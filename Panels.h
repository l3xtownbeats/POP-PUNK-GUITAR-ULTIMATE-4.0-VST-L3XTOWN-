#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "Engine/ChordEngine.h"
#include "Engine/ImportPipeline.h"
#include "UI/ThemeManager.h"

namespace ppgu {

// Panel palette holder (shared across windows; no global mutable state).
class PaletteHolder {
public:
    static juce::Colour c(unsigned int v) { return juce::Colour(v); }
};

// ---------------- FX EDIT ----------------
class FxEditPanel : public juce::Component {
public:
    explicit FxEditPanel(juce::AudioProcessorValueTreeState& s) : apvts(s) {
        for (auto& b : power) addAndMakeVisible(b);
        addAndMakeVisible(selector);
    }
    void resized() override {
        auto r = getLocalBounds().reduced(8);
        auto row = r.removeFromTop(28);
        int x = row.getX();
        for (auto& b : power) { b.setBounds(x, row.getY(), 76, 24); x += 82; }
        selector.setBounds(r.removeFromBottom(28));
    }
    juce::TextButton power[8] { {"Dist"}, {"Cho"}, {"Del"}, {"Rev"}, {"Comp"}, {"Lim"}, {"Cab"}, {"Flg"} };
    juce::ComboBox selector;
    juce::AudioProcessorValueTreeState& apvts;
};

// ---------------- AUTOCHORDS (Circle of Fifths) ----------------
class AutoChordsPanel : public juce::Component {
public:
    AutoChordsPanel() {
        addAndMakeVisible(wheel); addAndMakeVisible(padArea); addAndMakeVisible(label);
    }
    void resized() override {
        auto r = getLocalBounds().reduced(8);
        wheel.setBounds(r.removeFromLeft(220).withSizeKeepingCentre(200, 200));
        label.setBounds(r.removeFromTop(24));
        padArea.setBounds(r);
    }
    void paint(juce::Graphics& g) override {
        g.fillAll(PaletteHolder::c(0xFF181D23));
    }
    // Simple dial selector for circle of fifths position 0..11
    juce::Slider wheel { juce::Slider::Rotary, juce::Slider::NoTextBox, 0, 11, 0 };
    juce::TextButton padArea { "PROGRESSION PADS" };
    juce::Label label { {}, "AUTOCHORDS — Circle of Fifths" };
};

// ---------------- IMPORT GUITAR ----------------
class ImportGuitarPanel : public juce::Component {
public:
    ImportGuitarPanel() {
        addAndMakeVisible(formatBox); addAndMakeVisible(importBtn);
        addAndMakeVisible(status); addAndMakeVisible(autoDiscover);
    }
    void resized() override {
        auto r = getLocalBounds().reduced(10);
        formatBox.setBounds(r.removeFromTop(28));
        autoDiscover.setBounds(r.removeFromTop(28));
        importBtn.setBounds(r.removeFromTop(30));
        status.setBounds(r);
    }
    juce::ComboBox formatBox;
    juce::TextButton importBtn { "IMPORT GUITAR" }, autoDiscover { "AUTO DISCOVER" };
    juce::Label status { {}, "Formats: WAV / FLAC / OGG / AIFF / MP3 — single files or libraries" };
};

// ---------------- CURATOR ----------------
class CuratorPanel : public juce::Component {
public:
    CuratorPanel() { addAndMakeVisible(table); }
    void resized() override { table.setBounds(getLocalBounds().reduced(8)); }
    juce::ListBox table;
};

// ---------------- SETTINGS ----------------
class SettingsPanel : public juce::Component {
public:
    explicit SettingsPanel(juce::AudioProcessorValueTreeState& s) : apvts(s) {
        addAndMakeVisible(velocity); addAndMakeVisible(swing); addAndMakeVisible(note);
    }
    void resized() override {
        auto r = getLocalBounds().reduced(10);
        velocity.setBounds(r.removeFromTop(30));
        swing.setBounds(r.removeFromTop(30));
        note.setBounds(r);
    }
    juce::Slider velocity, swing;
    juce::AudioProcessorValueTreeState& apvts;
    juce::Label note { {}, "Settings bind to existing APVTS ids only." };
};

// ---------------- PERFORMANCE ----------------
class PerformancePanel : public juce::Component, private juce::Timer {
public:
    PerformancePanel() { startTimerHz(4); addAndMakeVisible(stats); }
    ~PerformancePanel() override { stopTimer(); }
    void timerCallback() override {
        // Read-only monitor; no side effects (journal fix).
        stats.setText(performText(), juce::dontSendNotification);
    }
    void resized() override { stats.setBounds(getLocalBounds().reduced(10)); }
    juce::Label stats { {}, "" };
private:
    juce::String performText() { return "PERFORMANCE (read-only): voices / cache / streaming visible in engine counters"; }
};

// ---------------- ABOUT ----------------
class AboutPanel : public juce::Component {
public:
    AboutPanel() { addAndMakeVisible(text); }
    void resized() override { text.setBounds(getLocalBounds().reduced(12)); }
    juce::Label text { {}, "POP PUNK GUITAR ULTIMATE - SIGNAL FORGE REV.17\nL3XTOWN AUDIO LAB\nPLAY. CREATE. ROCK." };
};

} // namespace ppgu