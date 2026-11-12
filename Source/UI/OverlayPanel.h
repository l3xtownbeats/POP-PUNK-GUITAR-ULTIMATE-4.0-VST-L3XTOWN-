#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include <array>
#include <functional>
#include <memory>

namespace ppgu::ui
{
enum class OverlayPanelId { AutoChords = 1, FXEdit = 2 };

class OverlayPanel final : public juce::Component
{
public:
    OverlayPanel(OverlayPanelId, PopPunkGuitarUltimateAudioProcessor&, std::function<void()> onClose);
    void paint(juce::Graphics&) override;
    void resized() override;
    void setCompact(bool);
    bool isCompact() const noexcept { return compact; }
private:
    void updateChordReadout();
    OverlayPanelId id;
    PopPunkGuitarUltimateAudioProcessor& processor;
    std::function<void()> closeCallback;
    juce::Label title, chordReadout, status;
    juce::TextButton closeButton { "×" }, sizeButton { "□" }, previewButton { "PREVIEW" };
    juce::ComboBox keyBox, modeBox, patternBox;
    std::array<juce::TextButton, 7> fxButtons {
        juce::TextButton("COMPRESSOR"), juce::TextButton("EQ"), juce::TextButton("DISTORTION"),
        juce::TextButton("CHORUS"), juce::TextButton("DELAY"), juce::TextButton("REVERB"),
        juce::TextButton("CABINET IR")
    };
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 7> fxAttachments;
    bool compact = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverlayPanel)
};
}
