#include "PluginEditor.h"
#include "BinaryData.h"

#include <algorithm>

// Approved PPGU 4.0 dark/pink skin: all visible rotary and FX controls use APVTS attachments.

namespace
{
    juce::Colour bg { 0xff111116 };
    juce::Colour panel { 0xff202027 };
    juce::Colour panel2 { 0xff17171d };
    juce::Colour pink { 0xffed238b };
    juce::Colour pale { 0xfff4f0f4 };
    juce::Colour muted { 0xffaaa8b2 };
    struct SkinPalette { const char* name; juce::uint32 background, panel, panel2, accent, text, muted; };
    const std::array<SkinPalette, 10> skinPalettes {{
        { "NEON PINK", 0xff111116, 0xff202027, 0xff17171d, 0xffed238b, 0xfff4f0f4, 0xffaaa8b2 },
        { "MIDNIGHT BLUE", 0xff0b111b, 0xff172638, 0xff101d2c, 0xff38a9ff, 0xffeef6ff, 0xff91a9c0 },
        { "BLOOD RED", 0xff160d10, 0xff2b171d, 0xff211116, 0xffff4058, 0xfffff0f0, 0xffc39da4 },
        { "VIOLET", 0xff120d1b, 0xff241a35, 0xff1a1329, 0xffa77bff, 0xfff5efff, 0xffb3a5c8 },
        { "CYBER GREEN", 0xff0b1512, 0xff172a23, 0xff102019, 0xff50e3a4, 0xffedfff7, 0xff9bb9aa },
        { "AMBER", 0xff17120b, 0xff302518, 0xff21190f, 0xffffb347, 0xfffff7e8, 0xffc4b394 },
        { "ARCTIC", 0xff10171b, 0xff1d2a31, 0xff152127, 0xff62d7d9, 0xfff0ffff, 0xffa4bec1 },
        { "GRAPHITE", 0xff111214, 0xff24262a, 0xff191b1f, 0xffd2d5dc, 0xfff5f6f8, 0xffa4a7ae },
        { "SUNSET", 0xff1a1011, 0xff35201f, 0xff241716, 0xffff765e, 0xfffff1e9, 0xffc8a59a },
        { "GOLD EDITION", 0xff15130e, 0xff2b2619, 0xff201c13, 0xffe5c46a, 0xfffff8e8, 0xffbdb08c }
    }};
}

class FxRackContent final : public juce::Component
{
    struct Control { juce::String label, parameter; };
    class Module final : public juce::Component
    {
    public:
        Module(juce::AudioProcessorValueTreeState& state, const juce::String& title,
               const juce::String& enableId, std::initializer_list<Control> controls)
        {
            setSize(900, 116);
            heading.setText(title, juce::dontSendNotification);
            heading.setFont(juce::Font(14.0f, juce::Font::bold));
            heading.setColour(juce::Label::textColourId, pink);
            addAndMakeVisible(heading);
            if (enableId.isNotEmpty()) {
                enabled.setButtonText("ON");
                enabled.setClickingTogglesState(true);
                enabledAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, enableId, enabled);
                addAndMakeVisible(enabled);
            }
            for (const auto& item : controls) {
                auto label = std::make_unique<juce::Label>();
                label->setText(item.label, juce::dontSendNotification);
                label->setFont(juce::Font(10.0f, juce::Font::bold));
                label->setColour(juce::Label::textColourId, muted);
                label->setJustificationType(juce::Justification::centred);
                addAndMakeVisible(*label);
                labels.push_back(std::move(label));
                auto slider = std::make_unique<juce::Slider>();
                slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
                slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
                slider->setColour(juce::Slider::rotarySliderFillColourId, pink);
                slider->setColour(juce::Slider::rotarySliderOutlineColourId, panel);
                slider->setColour(juce::Slider::thumbColourId, pale);
                slider->setColour(juce::Slider::textBoxTextColourId, pale);
                addAndMakeVisible(*slider);
                attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, item.parameter, *slider));
                sliders.push_back(std::move(slider));
            }
        }
        void applyTheme() {
            heading.setColour(juce::Label::textColourId, pink);
            enabled.setColour(juce::TextButton::buttonColourId, panel2);
            enabled.setColour(juce::TextButton::buttonOnColourId, pink);
            enabled.setColour(juce::TextButton::textColourOffId, pale);
            enabled.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
            for (auto& label : labels) label->setColour(juce::Label::textColourId, muted);
            for (auto& slider : sliders) {
                slider->setColour(juce::Slider::rotarySliderFillColourId, pink);
                slider->setColour(juce::Slider::rotarySliderOutlineColourId, panel);
                slider->setColour(juce::Slider::thumbColourId, pale);
                slider->setColour(juce::Slider::textBoxTextColourId, pale);
            }
        }
        void resized() override {
            auto area = getLocalBounds().reduced(10, 5);
            auto header = area.removeFromTop(23);
            heading.setBounds(header.removeFromLeft(240));
            if (enabled.isVisible()) enabled.setBounds(header.removeFromRight(55));
            const int count = juce::jmax(1, static_cast<int>(sliders.size()));
            const int width = juce::jmax(88, area.getWidth() / count);
            for (size_t i = 0; i < sliders.size(); ++i) {
                auto cell = area.removeFromLeft(width);
                labels[i]->setBounds(cell.removeFromTop(15));
                sliders[i]->setBounds(cell);
            }
        }
    private:
        juce::Label heading;
        juce::TextButton enabled;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledAttachment;
        std::vector<std::unique_ptr<juce::Label>> labels;
        std::vector<std::unique_ptr<juce::Slider>> sliders;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
    };
public:
    explicit FxRackContent(PopPunkGuitarUltimateAudioProcessor& p) {
        addModule(p,"COMPRESSOR","compressorOn",{{"AMOUNT","compressorAmount"},{"THRESHOLD","compressorThreshold"},{"RATIO","compressorRatio"},{"ATTACK","compressorAttack"},{"RELEASE","compressorRelease"}});
        addModule(p,"EQUALIZER","eqOn",{{"LOW","eqLow"},{"MID","eqMid"},{"HIGH","eqHigh"},{"LOW FREQ","eqLowFreq"},{"MID FREQ","eqMidFreq"},{"MID Q","eqMidQ"},{"HIGH FREQ","eqHighFreq"}});
        addModule(p,"AMP / DISTORTION","distortionOn",{{"DRIVE","distortionDrive"},{"TONE","distortionTone"},{"MIX","distortionMix"},{"AMP GAIN","ampGain"},{"AMP TONE","tone"}});
        addModule(p,"CHORUS","chorusOn",{{"RATE","chorusRate"},{"DEPTH","chorusDepth"},{"MIX","chorusMix"}});
        addModule(p,"FLANGER","flangerOn",{{"RATE","flangerRate"},{"DEPTH","flangerDepth"},{"FEEDBACK","flangerFeedback"},{"MIX","flangerMix"}});
        addModule(p,"DELAY","delayOn",{{"TIME MS","delayTime"},{"FEEDBACK","delayFeedback"},{"MIX","delayMix"}});
        addModule(p,"REVERB","reverbOn",{{"ROOM SIZE","reverbRoomSize"},{"DAMPING","reverbDamping"},{"MIX","reverbMix"}});
        addModule(p,"CABINET","cabinetOn",{{"LOW CUT","cabinetLowCut"},{"HIGH CUT","cabinetHighCut"},{"MIX","cabinetMix"}});
        addModule(p,"DOUBLE TRACK","doubleTrackOn",{{"DOUBLE MIX","doubleTrackMix"}});
        addModule(p,"OUTPUT",{},{{"MASTER","master"},{"OUTPUT DB","output"},{"PAN","pan"}});
        viewport.setViewedComponent(&body, false);
        viewport.setScrollBarsShown(true, false);
        addAndMakeVisible(viewport);
        setSize(960,820);
    }
    void applyTheme() {
        for (auto& module : modules) module->applyTheme();
        repaint();
    }
    void paint(juce::Graphics& g) override {
        g.fillAll(bg);
        g.setColour(pink); g.setFont(juce::Font(22.0f,juce::Font::bold));
        g.drawText("FX STUDIO",20,10,300,30,juce::Justification::left);
        g.setColour(muted); g.setFont(juce::Font(11.0f));
        g.drawText("TONE SHAPING  /  PER-PRESET PARAMETERS  /  HOST AUTOMATION",22,40,700,20,juce::Justification::left);
    }
    void resized() override {
        auto area=getLocalBounds().reduced(14); area.removeFromTop(66); viewport.setBounds(area);
        body.setSize(juce::jmax(600,area.getWidth()-14),static_cast<int>(modules.size())*122);
        auto r=body.getLocalBounds(); for(auto& m:modules) m->setBounds(r.removeFromTop(122).reduced(0,3));
    }
private:
    void addModule(PopPunkGuitarUltimateAudioProcessor& p,const juce::String& title,const juce::String& enable,std::initializer_list<Control> controls) {
        auto m=std::make_unique<Module>(p.parameters,title,enable,controls); body.addAndMakeVisible(*m); modules.push_back(std::move(m));
    }
    juce::Viewport viewport; juce::Component body; std::vector<std::unique_ptr<Module>> modules;
};
class FxRackWindow final : public juce::DocumentWindow {
public:
    explicit FxRackWindow(PopPunkGuitarUltimateAudioProcessor& p):DocumentWindow("PPGU 5.0 / FX STUDIO",juce::Colour(0xff111116),allButtons) {
        setUsingNativeTitleBar(true); setContentOwned(new FxRackContent(p),true); setResizable(true,true); centreWithSize(960,820); applyTheme();
    }
    void applyTheme() {
        if (auto* content = dynamic_cast<FxRackContent*>(getContentComponent()))
            content->applyTheme();
        setBackgroundColour(bg);
    }
    void closeButtonPressed() override { setVisible(false); }
};


PopPunkGuitarUltimateAudioProcessorEditor::PopPunkGuitarUltimateAudioProcessorEditor(PopPunkGuitarUltimateAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(1040, 790);
    setResizable(true, true);
    // The current fixed-height control stack needs ~788 px including outer margins.
    // Do not allow a window size that clips the keyboard or lower controls.
    setResizeLimits(800, 790, 1600, 1100);
    titleLabel.setText("BROWSER PRESETS:", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, pale);
    addAndMakeVisible(titleLabel);
    subtitleLabel.setText("POP PUNK GUITAR ULTIMATE   /   PLAY. CREATE. ROCK.", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    subtitleLabel.setColour(juce::Label::textColourId, pink);
    addAndMakeVisible(subtitleLabel);
    presetBox.setTextWhenNothingSelected("FACTORY PRESETS  /  300");
    presetBox.setColour(juce::ComboBox::backgroundColourId, panel2);
    presetBox.setColour(juce::ComboBox::textColourId, pale);
    presetBox.setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.5f));
    addAndMakeVisible(presetBox);
    categoryBox.addItem("OFF", 1);
    categoryBox.addItem("PUNK GROUP", 2);
    categoryBox.setSelectedId(2, juce::dontSendNotification);
    categoryBox.setColour(juce::ComboBox::backgroundColourId, panel2);
    categoryBox.setColour(juce::ComboBox::textColourId, pale);
    addAndMakeVisible(categoryBox);
    presetSearch.setTextToShowWhenEmpty("SEARCH PRESETS...", muted);
    presetSearch.setColour(juce::TextEditor::backgroundColourId, panel2);
    presetSearch.setColour(juce::TextEditor::textColourId, pale);
    presetSearch.setColour(juce::TextEditor::outlineColourId, pink.withAlpha(0.4f));
    addAndMakeVisible(presetSearch);
    loadFactoryPresetCatalog();
    melodyFilterBox.addItem("OFF", 1);
    melodyFilterBox.addItem("MELODY", 2);
    styleFilterBox.addItem("OFF", 1);
    styleFilterBox.addItem("STYLE", 2);
    // MELODY describes tone colour / guitar articulation, not playing pattern.
    // The expanded taxonomy is inspired by the full punk-group reference catalogue.
    const juce::StringArray melodyValues {
        "Clean", "Bright Clean", "Chorus Clean", "Tremolo Clean", "Warm Clean",
        "Acoustic Steel", "Nylon Acoustic", "Hybrid Acoustic / Electric",
        "Muted", "Palm Muted", "Flageolet / Harmonics",
        "Crunch", "Classic Drive", "Lead Drive", "High-Gain Distortion",
        "Saturated Punk", "Wide Open", "Ambient / Delay", "Bright Pop-Punk", "Dark / Warm"
    };
    for (int i = 0; i < melodyValues.size(); ++i)
        melodyFilterBox.addItem(melodyValues[i], i + 3);

    // STYLE describes the physical playing technique and rhythmic feel.
    const juce::StringArray styleValues {
        "Lead Guitar", "Solo Guitar", "Octave Hooks", "Melodic Runs", "Alternate Picking",
        "Tight Downstrokes", "Down-Up Strumming", "Palm-Muted Eighths", "Palm-Mute Gallop",
        "Open Power Chords", "Open Chord Strums", "Muted Verse / Open Chorus",
        "Stop-Start Accents", "Syncopated Riffs", "Straight Eighths", "Half-Time Groove",
        "Arpeggio", "Ambient Arpeggio", "Ska Upstrokes", "Acoustic Strumming",
        "Clean Chord Chops", "Sustained Lead Notes"
    };
    for (int i = 0; i < styleValues.size(); ++i)
        styleFilterBox.addItem(styleValues[i], i + 3);
    // PUNK GROUP choices come from the catalogue's authoritative artists list,
    // not from mode/category labels or whichever presets happen to be present.
    for (int i = 0; i < catalogArtists.size(); ++i)
        categoryBox.addItem(catalogArtists[i], i + 3);
    for (auto* box : { &melodyFilterBox, &styleFilterBox, &processorBox })
    {
        box->setColour(juce::ComboBox::backgroundColourId, panel2);
        box->setColour(juce::ComboBox::textColourId, pale);
        box->setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.5f));
        addAndMakeVisible(*box);
    }
    melodyFilterBox.setSelectedId(2, juce::dontSendNotification);
    styleFilterBox.setSelectedId(2, juce::dontSendNotification);
    processorBox.addItem("GOOD", 1);
    processorBox.addItem("NORMAL", 2);
    processorBox.addItem("NIGHT", 3);
    processorBox.addItem("ECO", 4);
    processorBox.setSelectedId(2, juce::dontSendNotification);
    processorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.parameters, "processorQuality", processorBox);
    presetBox.onChange = [this] {
        const int selectedId = presetBox.getSelectedId();
        if (selectedId <= 1) {
            clearFactoryPreset();
            statusLabel.setText("PRESET OFF  /  DEFAULT SOUND SETTINGS RESTORED", juce::dontSendNotification);
            return;
        }
        const int index = selectedId - 2;
        if (index >= 0 && index < static_cast<int>(visiblePresetIndices.size()))
            applyFactoryPreset(visiblePresetIndices[static_cast<size_t>(index)]);
    };
    auto refreshPresetList = [this] {
        // Keep the current sound selected when it remains in the filtered list.
        // Filtering must never silently load a different preset.
        int previouslySelectedPreset = -1;
        const int oldSelectedId = presetBox.getSelectedId();
        if (oldSelectedId > 1 && oldSelectedId - 2 < static_cast<int>(visiblePresetIndices.size()))
            previouslySelectedPreset = visiblePresetIndices[static_cast<size_t>(oldSelectedId - 2)];

        const auto query = presetSearch.getText().trim().toLowerCase();
        visiblePresetIndices.clear();
        presetBox.clear(juce::dontSendNotification);
        presetBox.addItem("OFF  /  NO PRESET", 1);
        const auto groupFilter = categoryBox.getText();
        for (size_t i = 0; i < factoryPresets.size(); ++i) {
            const auto& item = factoryPresets[i];
            if (groupFilter.isNotEmpty() && groupFilter != "OFF" && groupFilter != "PUNK GROUP"
                && !item.artistReference.equalsIgnoreCase(groupFilter)) continue;
            const auto melodyFilter = melodyFilterBox.getText();
            const auto melodyText = (item.soundCharacter + " " + item.tonalTarget + " "
                                     + item.sampleProfile + " " + item.artistReference + " "
                                     + item.styleProfileId + " " + item.tags).toLowerCase();
            const auto containsAny = [](const juce::String& text, std::initializer_list<const char*> terms) {
                for (const auto* term : terms)
                    if (text.containsIgnoreCase(term)) return true;
                return false;
            };
            bool matchesMelody = true;
            if (melodyFilter == "Clean")
                matchesMelody = containsAny(melodyText, {"clean"});
            else if (melodyFilter == "Bright Clean")
                matchesMelody = containsAny(melodyText, {"bright"}) && containsAny(melodyText, {"clean", "polished", "glassy"});
            else if (melodyFilter == "Chorus Clean")
                matchesMelody = containsAny(melodyText, {"chorus", "wide", "layered"}) && containsAny(melodyText, {"clean", "open", "polished"});
            else if (melodyFilter == "Tremolo Clean")
                matchesMelody = containsAny(melodyText, {"tremolo", "pulsing", "shimmer"});
            else if (melodyFilter == "Warm Clean")
                matchesMelody = containsAny(melodyText, {"warm", "round", "soft"}) && containsAny(melodyText, {"clean", "acoustic", "polished"});
            else if (melodyFilter == "Acoustic Steel")
                matchesMelody = containsAny(melodyText, {"acoustic", "steel"});
            else if (melodyFilter == "Nylon Acoustic")
                matchesMelody = containsAny(melodyText, {"nylon"});
            else if (melodyFilter == "Hybrid Acoustic / Electric")
                matchesMelody = containsAny(melodyText, {"hybrid", "acoustic/electric", "acoustic-electric"});
            else if (melodyFilter == "Muted")
                matchesMelody = containsAny(melodyText, {"muted", "palm mute", "chug"});
            else if (melodyFilter == "Palm Muted")
                matchesMelody = containsAny(melodyText, {"palm", "muted", "chug"});
            else if (melodyFilter == "Flageolet / Harmonics")
                matchesMelody = containsAny(melodyText, {"flageolet", "harmonic", "natural overtone"});
            else if (melodyFilter == "Crunch")
                matchesMelody = containsAny(melodyText, {"crunch", "gritty", "skate-punk"});
            else if (melodyFilter == "Classic Drive")
                matchesMelody = containsAny(melodyText, {"drive", "driven", "saturated", "distortion"});
            else if (melodyFilter == "Lead Drive")
                matchesMelody = containsAny(melodyText, {"lead", "solo", "octave", "melodic run"});
            else if (melodyFilter == "High-Gain Distortion")
                matchesMelody = containsAny(melodyText, {"high-gain", "aggressive", "heavy", "distortion", "hard-hitting"});
            else if (melodyFilter == "Saturated Punk")
                matchesMelody = containsAny(melodyText, {"saturated", "dense", "aggressive", "punk"});
            else if (melodyFilter == "Wide Open")
                matchesMelody = containsAny(melodyText, {"wide", "open", "ringing", "sustained"});
            else if (melodyFilter == "Ambient / Delay")
                matchesMelody = containsAny(melodyText, {"ambient", "dotted-eighth", "spacious", "delay", "atmospheric"});
            else if (melodyFilter == "Bright Pop-Punk")
                matchesMelody = containsAny(melodyText, {"bright", "pop-punk", "polished"});
            else if (melodyFilter == "Dark / Warm")
                matchesMelody = containsAny(melodyText, {"dark", "warm", "thick", "low-mid"});
            else if (melodyFilter.isNotEmpty() && melodyFilter != "OFF" && melodyFilter != "MELODY")
                matchesMelody = melodyText.containsIgnoreCase(melodyFilter);
            if (!matchesMelody) continue;

            const auto styleFilter = styleFilterBox.getText();
            const auto styleText = (item.name + " " + item.groove + " " + item.articulation + " "
                                    + item.tonalTarget + " " + item.styleProfileId + " "
                                    + item.artistReference + " " + item.referenceTracks + " " + item.tags).toLowerCase();
            bool matchesStyle = true;
            if (styleFilter == "Lead Guitar" || styleFilter == "Solo Guitar")
                matchesStyle = containsAny(styleText, {"lead", "solo", "octave", "hook", "melodic run", "alternate-picked"});
            else if (styleFilter == "Octave Hooks")
                matchesStyle = containsAny(styleText, {"octave", "hook"});
            else if (styleFilter == "Melodic Runs")
                matchesStyle = containsAny(styleText, {"melodic run", "lead-picked", "alternate picking", "flageolet"});
            else if (styleFilter == "Alternate Picking")
                matchesStyle = containsAny(styleText, {"alternate", "picked melodic", "fast picking"});
            else if (styleFilter == "Tight Downstrokes")
                matchesStyle = containsAny(styleText, {"downstroke", "continuous downstrokes", "straight eighth", "tight power"});
            else if (styleFilter == "Down-Up Strumming")
                matchesStyle = containsAny(styleText, {"down-up", "down up", "strum", "upstroke/downstroke"});
            else if (styleFilter == "Palm-Muted Eighths")
                matchesStyle = containsAny(styleText, {"palm-muted", "palm muted", "palm-mute", "tight palm", "muted eighth"});
            else if (styleFilter == "Palm-Mute Gallop")
                matchesStyle = containsAny(styleText, {"gallop", "palm-muted 16ths", "palm-muted gallop"});
            else if (styleFilter == "Open Power Chords")
                matchesStyle = containsAny(styleText, {"open power", "power-chord chorus", "open electric chorus", "wide open"});
            else if (styleFilter == "Open Chord Strums")
                matchesStyle = containsAny(styleText, {"open chord", "open-chord", "acoustic-strum", "open chord strum"});
            else if (styleFilter == "Muted Verse / Open Chorus")
                matchesStyle = containsAny(styleText, {"muted verse", "palm-muted verse", "open chorus", "wide chorus"});
            else if (styleFilter == "Stop-Start Accents")
                matchesStyle = containsAny(styleText, {"stop-start", "stop start", "short chord chops", "punctuated"});
            else if (styleFilter == "Syncopated Riffs")
                matchesStyle = containsAny(styleText, {"syncopated", "offbeat", "accent pattern"});
            else if (styleFilter == "Straight Eighths")
                matchesStyle = containsAny(styleText, {"eighth", "straight", "continuous"});
            else if (styleFilter == "Half-Time Groove")
                matchesStyle = containsAny(styleText, {"half-time", "half time", "mid-tempo"});
            else if (styleFilter == "Arpeggio" || styleFilter == "Ambient Arpeggio")
                matchesStyle = containsAny(styleText, {"arpeggio", "picked melodic", "picked arpeggios", "ambient"});
            else if (styleFilter == "Ska Upstrokes")
                matchesStyle = containsAny(styleText, {"ska", "upstroke", "offbeat"});
            else if (styleFilter == "Acoustic Strumming")
                matchesStyle = containsAny(styleText, {"acoustic", "strum", "down-up"});
            else if (styleFilter == "Clean Chord Chops")
                matchesStyle = containsAny(styleText, {"clean chord", "short clean", "chord chops", "chops"});
            else if (styleFilter == "Sustained Lead Notes")
                matchesStyle = containsAny(styleText, {"sustain", "sustained", "lead", "ambient", "ringing"});
            else if (styleFilter.isNotEmpty() && styleFilter != "OFF" && styleFilter != "STYLE")
                matchesStyle = styleText.containsIgnoreCase(styleFilter);
            if (!matchesStyle) continue;

            const auto matchesQuery = query.isEmpty()
                || item.name.toLowerCase().contains(query)
                || item.category.toLowerCase().contains(query)
                || item.artistReference.toLowerCase().contains(query)
                || item.referenceTracks.toLowerCase().contains(query)
                || item.tonalTarget.toLowerCase().contains(query)
                || item.sampleProfile.toLowerCase().contains(query)
                || item.catalogStatus.toLowerCase().contains(query)
                || item.tags.toLowerCase().contains(query)
                || item.soundCharacter.toLowerCase().contains(query)
                || item.styleProfileId.toLowerCase().contains(query)
                || item.groove.toLowerCase().contains(query)
                || item.articulation.toLowerCase().contains(query);
            if (!matchesQuery) continue;

            visiblePresetIndices.push_back(static_cast<int>(i));
            presetBox.addItem(item.name, static_cast<int>(visiblePresetIndices.size()) + 1);
        }

        const auto selected = std::find(visiblePresetIndices.begin(), visiblePresetIndices.end(),
                                        previouslySelectedPreset);
        if (previouslySelectedPreset >= 0 && selected != visiblePresetIndices.end())
            presetBox.setSelectedId(static_cast<int>(std::distance(visiblePresetIndices.begin(), selected)) + 2,
                                    juce::dontSendNotification);
        else
            presetBox.setSelectedId(1, juce::dontSendNotification);
    };
    categoryBox.onChange = refreshPresetList;
    melodyFilterBox.onChange = refreshPresetList;
    styleFilterBox.onChange = refreshPresetList;
    presetSearch.onTextChange = refreshPresetList;
    refreshPresetList();
    fxOpenButton.setButtonText("FX STUDIO");
    fxOpenButton.setColour(juce::TextButton::buttonColourId, panel2);
    fxOpenButton.setColour(juce::TextButton::textColourOffId, pink);
    fxOpenButton.onClick = [this] {
        if (fxWindow == nullptr)
            fxWindow = std::make_unique<FxRackWindow>(processor);
        fxWindow->setVisible(true);
        fxWindow->toFront(true);
    };
    addAndMakeVisible(fxOpenButton);
    loadButton.setButtonText("IMPORT SOUNDS");
    loadButton.setTooltip("Current build imports SFZ instruments and their referenced WAV samples.");
    loadButton.setColour(juce::TextButton::buttonColourId, pink);
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    loadButton.onClick = [this] { chooseSfzFile(); };
    addAndMakeVisible(loadButton);

    skinsButton.setTooltip("Choose one of 10 interface skins. The selected theme is saved with this plugin instance.");
    skinsButton.setEnabled(true);
    skinsButton.setColour(juce::TextButton::buttonColourId, panel2);
    skinsButton.setColour(juce::TextButton::textColourOffId, pink);
    skinsButton.onClick = [this] {
        juce::PopupMenu menu;
        for (int i = 0; i < static_cast<int>(skinPalettes.size()); ++i)
            menu.addItem(i + 1, skinPalettes[static_cast<size_t>(i)].name, true, i == activeSkinIndex);
        menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&skinsButton),
            [this](int result) { if (result > 0) applySkin(result - 1); });
    };
    midiButton.setTooltip("MIDI Piano Roll is not connected in this build.");
    multicoreButton.setTooltip("Background multicore processing is not enabled in this build.");
    addAndMakeVisible(skinsButton);
    for (auto* button : { &midiButton, &multicoreButton })
    {
        button->setEnabled(false);
        button->setColour(juce::TextButton::buttonColourId, panel2);
        button->setColour(juce::TextButton::textColourOffId, muted);
        addAndMakeVisible(*button);
    }
    doubleButton.setClickingTogglesState(true);
    doubleButton.setColour(juce::TextButton::buttonColourId, panel2);
    doubleButton.setColour(juce::TextButton::buttonOnColourId, pink);
    doubleButton.setColour(juce::TextButton::textColourOffId, pale);
    doubleButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    doubleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.parameters, "doubleTrackOn", doubleButton);
    addAndMakeVisible(doubleButton);
    autoChordsButton.setTooltip("PATTERN mode only: one MIDI note becomes a diatonic triad in the selected key and scale. Turn OFF to use incoming MIDI chords.");
    autoChordsButton.setClickingTogglesState(true);
    autoChordsButton.setColour(juce::TextButton::buttonColourId, panel2);
    autoChordsButton.setColour(juce::TextButton::buttonOnColourId, pink);
    autoChordsButton.setColour(juce::TextButton::textColourOffId, pale);
    autoChordsButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    autoChordsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.parameters, "autoChordsOn", autoChordsButton);
    autoChordsButton.setButtonText(autoChordsButton.getToggleState() ? "AUTOCHORDS  ON" : "AUTOCHORDS  OFF");
    autoChordsButton.onClick = [this] {
        const bool enabled = autoChordsButton.getToggleState();
        autoChordsButton.setButtonText(enabled ? "AUTOCHORDS  ON" : "AUTOCHORDS  OFF");
        const bool patternMode = modeBox.getSelectedId() == 2;
        autoChordKeyBox.setEnabled(patternMode && enabled);
        autoChordScaleBox.setEnabled(patternMode && enabled);
        statusLabel.setText(enabled
            ? "AUTOCHORDS ON  /  PATTERN MODE ONLY  /  SINGLE MIDI NOTE BUILDS A DIATONIC CHORD"
            : "AUTOCHORDS OFF  /  PLAY CHORDS FROM MIDI INPUT", juce::dontSendNotification);
    };
    addAndMakeVisible(autoChordsButton);
    autoChordKeyBox.addItemList({ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 1);
    autoChordScaleBox.addItemList({ "CHROMATIC", "MAJOR", "MINOR" }, 1);
    for (auto* box : { &autoChordKeyBox, &autoChordScaleBox }) {
        box->setColour(juce::ComboBox::backgroundColourId, panel2);
        box->setColour(juce::ComboBox::textColourId, pale);
        box->setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.5f));
        addAndMakeVisible(*box);
    }
    autoChordKeyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.parameters, "scaleRoot", autoChordKeyBox);
    autoChordScaleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.parameters, "scaleMode", autoChordScaleBox);
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
    statusLabel.setColour(juce::Label::textColourId, muted);
    statusLabel.setFont(juce::Font(11.0f));
    addAndMakeVisible(statusLabel);
    melodyTab.setClickingTogglesState(false);
    patternTab.setClickingTogglesState(false);
    for (auto* b : { &melodyTab, &patternTab }) {
        b->setColour(juce::TextButton::buttonColourId, panel2);
        b->setColour(juce::TextButton::textColourOffId, pale);
        addAndMakeVisible(*b);
    }
    modeBox.addItem("MELODY", 1);
    modeBox.addItem("PATTERN", 2);
    modeBox.setColour(juce::ComboBox::backgroundColourId, panel2);
    modeBox.setColour(juce::ComboBox::textColourId, pale);
    modeBox.setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.5f));
    addAndMakeVisible(modeBox);
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "playMode", modeBox);
    modeBox.setVisible(false);
    startModeBox.addItem("HOST", 1);
    startModeBox.addItem("HOST START", 2);
    startModeBox.addItem("INTERNAL", 3);
    startModeBox.setColour(juce::ComboBox::backgroundColourId, panel2);
    startModeBox.setColour(juce::ComboBox::textColourId, pale);
    startModeBox.setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.5f));
    addAndMakeVisible(startModeBox);
    startModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "syncMode", startModeBox);
    keyboard = std::make_unique<juce::MidiKeyboardComponent>(processor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard);
    keyboard->setAvailableRange(36, 84);
    keyboard->setKeyWidth(18.0f);
    keyboard->setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xffe8e7eb));
    keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff17171d));
    keyboard->setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colour(0xff34343d));
    keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, pink.withAlpha(0.35f));
    keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, pink.withAlpha(0.85f));
    addAndMakeVisible(*keyboard);
    melodyTab.setColour(juce::TextButton::buttonOnColourId, pink);
    patternTab.setColour(juce::TextButton::buttonOnColourId, pink);
    melodyTab.onClick = [this] { modeBox.setSelectedId(1); };
    patternTab.onClick = [this] { modeBox.setSelectedId(2); };
    modeBox.onChange = [this] {
        const bool isPattern = modeBox.getSelectedId() == 2;
        melodyTab.setToggleState(!isPattern, juce::dontSendNotification);
        patternTab.setToggleState(isPattern, juce::dontSendNotification);

        // AutoChords is a Pattern-only performance feature. Keep its saved
        // toggle state, but make the button and key/scale controls unavailable
        // while Melody is active so the UI cannot imply chromatic notes are
        // being transformed.
        autoChordsButton.setEnabled(isPattern);
        const bool autoChordsEnabled = autoChordsButton.getToggleState();
        autoChordKeyBox.setEnabled(isPattern && autoChordsEnabled);
        autoChordScaleBox.setEnabled(isPattern && autoChordsEnabled);
    };
    modeBox.onChange();
    for (size_t i = 0; i < knobs.size(); ++i) {
        configureKnob(knobs[i], knobLabels[i], knobNames[i]);
        knobAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, parameterIDs[i], knobs[i]);
    }
    for (size_t i = 0; i < fxButtons.size(); ++i) {
        auto& b = fxButtons[i];
        b.setClickingTogglesState(true);
        b.setColour(juce::TextButton::buttonColourId, panel2);
        b.setColour(juce::TextButton::buttonOnColourId, pink);
        b.setColour(juce::TextButton::textColourOffId, pale);
        b.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        fxAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters, fxParameterIDs[i], b);
        addAndMakeVisible(b);
    }
    ampTitle.setText("AMP & TONE", juce::dontSendNotification);
    fxTitle.setText("FX CHAIN", juce::dontSendNotification);
    playbackTitle.setText("PLAYBACK", juce::dontSendNotification);
    for (auto* l : { &ampTitle, &fxTitle, &playbackTitle }) {
        l->setFont(juce::Font(15.0f, juce::Font::bold));
        l->setColour(juce::Label::textColourId, pink);
        addAndMakeVisible(*l);
    }
    activeSkinIndex = juce::jlimit(0, static_cast<int>(skinPalettes.size()) - 1,
        static_cast<int>(processor.parameters.state.getProperty("uiSkinIndex", 0)));
    applySkin(activeSkinIndex);
}

void PopPunkGuitarUltimateAudioProcessorEditor::applySkin(int index)
{
    activeSkinIndex = juce::jlimit(0, static_cast<int>(skinPalettes.size()) - 1, index);
    processor.parameters.state.setProperty("uiSkinIndex", activeSkinIndex, nullptr);
    const auto& skin = skinPalettes[static_cast<size_t>(activeSkinIndex)];
    bg = juce::Colour(skin.background);
    panel = juce::Colour(skin.panel);
    panel2 = juce::Colour(skin.panel2);
    pink = juce::Colour(skin.accent);
    pale = juce::Colour(skin.text);
    muted = juce::Colour(skin.muted);

    for (auto* label : { &titleLabel, &subtitleLabel, &statusLabel, &ampTitle, &fxTitle, &playbackTitle }) {
        label->setColour(juce::Label::textColourId, label == &subtitleLabel || label == &ampTitle
            || label == &fxTitle || label == &playbackTitle ? pink : pale);
    }
    for (auto* box : { &presetBox, &categoryBox, &melodyFilterBox, &styleFilterBox, &processorBox,
                       &autoChordKeyBox, &autoChordScaleBox, &startModeBox }) {
        box->setColour(juce::ComboBox::backgroundColourId, panel2);
        box->setColour(juce::ComboBox::textColourId, pale);
        box->setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.55f));
    }
    presetSearch.setColour(juce::TextEditor::backgroundColourId, panel2);
    presetSearch.setColour(juce::TextEditor::textColourId, pale);
    presetSearch.setColour(juce::TextEditor::outlineColourId, pink.withAlpha(0.5f));
    for (auto* button : { &loadButton, &fxOpenButton, &skinsButton, &midiButton, &doubleButton,
                          &multicoreButton, &melodyTab, &patternTab, &autoChordsButton }) {
        button->setColour(juce::TextButton::buttonColourId, panel2);
        button->setColour(juce::TextButton::buttonOnColourId, pink);
        button->setColour(juce::TextButton::textColourOffId, pale);
        button->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }
    loadButton.setColour(juce::TextButton::buttonColourId, pink);
    for (auto& button : fxButtons) {
        button.setColour(juce::TextButton::buttonColourId, panel2);
        button.setColour(juce::TextButton::buttonOnColourId, pink);
        button.setColour(juce::TextButton::textColourOffId, pale);
        button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }
    for (size_t i = 0; i < knobs.size(); ++i) {
        knobs[i].setColour(juce::Slider::rotarySliderFillColourId, pink);
        knobs[i].setColour(juce::Slider::rotarySliderOutlineColourId, panel);
        knobs[i].setColour(juce::Slider::thumbColourId, pale);
        knobs[i].setColour(juce::Slider::textBoxTextColourId, pale);
    }
    if (keyboard != nullptr) {
        keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, pink.withAlpha(0.85f));
        keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, pink.withAlpha(0.35f));
        keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId, panel2);
    }
    if (fxWindow != nullptr)
        if (auto* rack = dynamic_cast<FxRackWindow*>(fxWindow.get()))
            rack->applyTheme();
    repaint();
}

void PopPunkGuitarUltimateAudioProcessorEditor::configureKnob(juce::Slider& s, juce::Label& l, const juce::String& name)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, pink);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff44444e));
    s.setColour(juce::Slider::thumbColourId, pale);
    s.setColour(juce::Slider::textBoxTextColourId, pale);
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(s);
    l.setText(name, juce::dontSendNotification);
    l.setFont(juce::Font(11.0f, juce::Font::bold));
    l.setColour(juce::Label::textColourId, muted);
    l.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(l);
}

void PopPunkGuitarUltimateAudioProcessorEditor::drawPanel(juce::Graphics& g, juce::Rectangle<int> r)
{
    g.setColour(panel);
    g.fillRoundedRectangle(r.toFloat(), 12.0f);
    g.setColour(pink.withAlpha(0.45f));
    g.drawRoundedRectangle(r.toFloat().reduced(0.5f), 12.0f, 1.0f);
}

void PopPunkGuitarUltimateAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(bg);
    g.setColour(panel2);
    g.fillRect(0, 0, getWidth(), 62);
    g.setColour(pink);
    g.fillRect(0, 0, getWidth(), 3);
    auto hero = getLocalBounds().reduced(16);
    hero.removeFromTop(88);
    hero.setHeight(154);
    juce::ColourGradient grad(panel, static_cast<float>(hero.getX()), static_cast<float>(hero.getY()), bg, static_cast<float>(hero.getRight()), static_cast<float>(hero.getBottom()), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(hero.toFloat(), 12.0f);
    g.setColour(pink.withAlpha(0.5f));
    g.drawRoundedRectangle(hero.toFloat(), 12.0f, 1.0f);
    auto textArea = hero.reduced(28, 18);
    g.setColour(pale);
    g.setFont(juce::Font(juce::jmin(36.0f, getWidth() * 0.041f), juce::Font::bold));
    g.drawText("POP PUNK", textArea.removeFromTop(42), juce::Justification::left);
    g.setColour(pink);
    g.setFont(juce::Font(juce::jmin(31.0f, getWidth() * 0.036f), juce::Font::italic | juce::Font::bold));
    g.drawText("GUITAR ULTIMATE", textArea.removeFromTop(36), juce::Justification::left);
    g.setColour(pale.withAlpha(0.8f));
    g.setFont(juce::Font(14.0f, juce::Font::italic));
    g.drawText("Play. Create. Rock.", textArea.removeFromTop(22), juce::Justification::left);
    // Stylised electric-guitar silhouette as a vector fallback; no external image dependency.
    auto guitar = hero.removeFromRight(static_cast<int>(hero.getWidth() * 0.43f)).reduced(22, 16);
    juce::Path body;
    body.startNewSubPath(guitar.getX() + guitar.getWidth()*0.48f, guitar.getY()+guitar.getHeight()*0.52f);
    body.cubicTo(guitar.getX()+guitar.getWidth()*0.28f,guitar.getY()+guitar.getHeight()*0.23f,guitar.getX()+guitar.getWidth()*0.12f,guitar.getY()+guitar.getHeight()*0.45f,guitar.getX()+guitar.getWidth()*0.28f,guitar.getY()+guitar.getHeight()*0.68f);
    body.cubicTo(guitar.getX()+guitar.getWidth()*0.38f,guitar.getY()+guitar.getHeight()*0.83f,guitar.getX()+guitar.getWidth()*0.60f,guitar.getY()+guitar.getHeight()*0.79f,guitar.getX()+guitar.getWidth()*0.68f,guitar.getY()+guitar.getHeight()*0.60f);
    body.cubicTo(guitar.getX()+guitar.getWidth()*0.83f,guitar.getY()+guitar.getHeight()*0.57f,guitar.getX()+guitar.getWidth()*0.83f,guitar.getY()+guitar.getHeight()*0.38f,guitar.getX()+guitar.getWidth()*0.68f,guitar.getY()+guitar.getHeight()*0.40f);
    body.closeSubPath();
    g.setColour(pale);
    g.fillPath(body);
    g.setColour(panel2);
    g.fillEllipse(guitar.getX()+guitar.getWidth()*0.43f,guitar.getY()+guitar.getHeight()*0.47f, guitar.getWidth()*0.17f,guitar.getHeight()*0.22f);
    g.setColour(pink.brighter(0.2f));
    juce::Path neck;
    neck.addQuadrilateral(static_cast<float>(guitar.getX())+static_cast<float>(guitar.getWidth())*0.60f, static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.45f, static_cast<float>(guitar.getRight()), static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.08f, static_cast<float>(guitar.getRight()), static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.20f, static_cast<float>(guitar.getX())+static_cast<float>(guitar.getWidth())*0.66f, static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.57f);
    g.fillPath(neck);
    g.setColour(pink.withAlpha(0.75f));
    g.drawLine(static_cast<float>(guitar.getX()+guitar.getWidth()*0.58f),static_cast<float>(guitar.getY()+guitar.getHeight()*0.49f),static_cast<float>(guitar.getRight()),static_cast<float>(guitar.getY()+guitar.getHeight()*0.14f),1.2f);
    auto panelArea = getLocalBounds().reduced(16);
    panelArea.removeFromTop(276);
    panelArea.removeFromBottom(82);
    const int gap=12, ampW=static_cast<int>(panelArea.getWidth()*0.34f), fxW=static_cast<int>(panelArea.getWidth()*0.34f);
    auto amp=panelArea.removeFromLeft(ampW); panelArea.removeFromLeft(gap);
    auto fx=panelArea.removeFromLeft(fxW); panelArea.removeFromLeft(gap);
    drawPanel(g,amp); drawPanel(g,fx); drawPanel(g,panelArea);
}

void PopPunkGuitarUltimateAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(16);

    // Unified browser: preset, search, mode, sound character and playing style.
    auto browser = area.removeFromTop(46);
    titleLabel.setBounds(browser.removeFromLeft(126).reduced(1, 5));
    presetBox.setBounds(browser.removeFromLeft(154).reduced(3, 5));
    presetSearch.setBounds(browser.removeFromLeft(104).reduced(3, 7));
    categoryBox.setBounds(browser.removeFromLeft(122).reduced(3, 7));
    melodyFilterBox.setBounds(browser.removeFromLeft(120).reduced(3, 7));
    styleFilterBox.setBounds(browser.removeFromLeft(120).reduced(3, 7));
    subtitleLabel.setVisible(browser.getWidth() > 180);
    subtitleLabel.setBounds(browser.reduced(4, 0));

    // Main action strip.
    auto toolbar = area.removeFromTop(42);
    loadButton.setBounds(toolbar.removeFromLeft(108).reduced(2, 4));
    melodyTab.setBounds(toolbar.removeFromLeft(78).reduced(2, 4));
    patternTab.setBounds(toolbar.removeFromLeft(78).reduced(2, 4));
    fxOpenButton.setBounds(toolbar.removeFromLeft(88).reduced(2, 4));
    skinsButton.setBounds(toolbar.removeFromLeft(64).reduced(2, 4));
    midiButton.setBounds(toolbar.removeFromLeft(54).reduced(2, 4));
    doubleButton.setBounds(toolbar.removeFromLeft(72).reduced(2, 4));
    processorBox.setBounds(toolbar.removeFromLeft(88).reduced(2, 4));
    multicoreButton.setBounds(toolbar.removeFromLeft(92).reduced(2, 4));

    area.removeFromTop(154);
    auto status = area.removeFromTop(26);
    statusLabel.setBounds(status);
    area.removeFromTop(8);
    auto panels = area.removeFromTop(400);
    const int gap=12, ampW=static_cast<int>(panels.getWidth()*0.34f), fxW=static_cast<int>(panels.getWidth()*0.34f);
    auto amp=panels.removeFromLeft(ampW); panels.removeFromLeft(gap);
    auto fx=panels.removeFromLeft(fxW); panels.removeFromLeft(gap); auto playback=panels;
    ampTitle.setBounds(amp.removeFromTop(34).reduced(12,2));
    fxTitle.setBounds(fx.removeFromTop(34).reduced(12,2));
    playbackTitle.setBounds(playback.removeFromTop(34).reduced(12,2));
    const int kw=amp.getWidth()/4;
    for(size_t i=0;i<4;++i){auto cell=amp.removeFromLeft(kw); knobLabels[i].setBounds(cell.removeFromBottom(22)); knobs[i].setBounds(cell.reduced(4,0));}
    for(size_t i=0;i<fxButtons.size();++i){auto cell=fx.removeFromTop(fx.getHeight()/static_cast<int>(fxButtons.size()-i)); fxButtons[i].setBounds(cell.reduced(8,2));}
    auto keyboardArea = area.removeFromBottom(72);
    if (keyboard != nullptr) keyboard->setBounds(keyboardArea.reduced(2, 4));
    auto playbackControls = playback.removeFromTop(164);
    playbackControls.removeFromTop(8);
    const int pw = juce::jmax(1, playbackControls.getWidth() / 5);
    for (size_t i = 4; i < 9; ++i)
    {
        auto cell = playbackControls.removeFromLeft(pw);
        knobLabels[i].setBounds(cell.removeFromBottom(28));
        knobs[i].setBounds(cell.reduced(1, 4));
    }
    auto autoChordRow = playback.removeFromTop(68);
    autoChordsButton.setBounds(autoChordRow.removeFromTop(32).reduced(3, 2));
    autoChordKeyBox.setBounds(autoChordRow.removeFromLeft(autoChordRow.getWidth() / 2).reduced(2, 2));
    autoChordScaleBox.setBounds(autoChordRow.reduced(2, 2));
    startModeBox.setBounds(playback.removeFromTop(34).reduced(8, 2));
}

void PopPunkGuitarUltimateAudioProcessorEditor::loadFactoryPresetCatalog()
{
    factoryPresets.clear();
    const auto jsonText = juce::String::fromUTF8(BinaryData::factory_bank_300_json, BinaryData::factory_bank_300_jsonSize);
    const auto root = juce::JSON::parse(jsonText);
    if (!root.isObject()) { statusLabel.setText("FACTORY PRESET CATALOG ERROR", juce::dontSendNotification); return; }
    catalogArtists.clear();
    if (auto* artistArray = root.getProperty("artists", juce::var()).getArray())
        for (const auto& artist : *artistArray)
        {
            const auto artistName = artist.getProperty("name", "").toString().trim();
            if (artistName.isNotEmpty() && !catalogArtists.contains(artistName))
                catalogArtists.add(artistName);
        }
    const auto append = [this](const juce::var& array, const juce::String& mode) {
        if (auto* values = array.getArray()) for (const auto& value : *values) {
            if (!value.isObject()) continue;
            FactoryPreset preset;
            preset.id = value.getProperty("id", "").toString();
            preset.name = value.getProperty("name", "Unnamed").toString();
            preset.category = value.getProperty("referenceArtist", "Original").toString();
            preset.artistReference = preset.category;
            preset.mode = mode;
            preset.styleProfileId = value.getProperty("styleProfileId", "").toString();
            preset.soundCharacter = value.getProperty("soundCharacter", "Electric").toString();
            preset.groove = value.getProperty("groove", "").toString();
            preset.articulation = value.getProperty("articulation", "").toString();
            preset.tonalTarget = value.getProperty("tonalTarget", "").toString();
            preset.sampleProfile = value.getProperty("sampleProfile", value.getProperty("sampleRequirement", "")).toString();
            preset.catalogStatus = value.getProperty("status", "").toString();
            const auto tracks = value.getProperty("referenceTracks", juce::var());
            if (auto* trackArray = tracks.getArray())
                for (const auto& track : *trackArray)
                    preset.referenceTracks += (preset.referenceTracks.isEmpty() ? "" : "  ") + track.toString();
            const auto tags = value.getProperty("tags", juce::var());
            if (auto* tagArray = tags.getArray())
                for (const auto& tag : *tagArray)
                    preset.tags += (preset.tags.isEmpty() ? "" : "  ") + tag.toString();
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

    const auto profileText = (preset.soundCharacter + " " + preset.tonalTarget + " " + preset.tags).toLowerCase();
    const bool acousticProfile = preset.sampleProfile.containsIgnoreCase("acoustic")
        || preset.soundCharacter.containsIgnoreCase("acoustic");
    const bool ambientProfile = profileText.contains("ambient") || profileText.contains("dotted-eighth")
        || profileText.contains("delay") || profileText.contains("spacious");
    const bool wideProfile = profileText.contains("wide") || profileText.contains("layered")
        || profileText.contains("double-track") || profileText.contains("double track");
    const bool chorusProfile = profileText.contains("chorus") || profileText.contains("chorused");
    const bool flangerProfile = profileText.contains("flanger");
    const bool brightProfile = profileText.contains("bright");
    const bool darkProfile = profileText.contains("dark") || profileText.contains("warm");
    const bool tightProfile = profileText.contains("tight") || profileText.contains("muted");

    // The factory preset is a real parameter snapshot for the currently implemented
    // amp/EQ/FX engine. Acoustic sample selection remains a separate SFZ-bank action.
    const float drive = acousticProfile ? 0.0f : juce::jlimit(0.0f, 1.0f, preset.drive);
    setParameter("distortionDrive", drive);
    setParameter("distortionMix", acousticProfile ? 0.0f : 0.82f);
    setParameter("distortionOn", drive > 0.25f ? 1.0f : 0.0f);
    setParameter("ampGain", acousticProfile ? 4.8f : 5.0f + drive * 0.8f);
    setParameter("tone", juce::jlimit(0.0f, 10.0f, preset.tone));
    setParameter("eqLow", tightProfile ? -1.0f : (acousticProfile ? 1.0f : 0.0f));
    setParameter("eqMid", darkProfile ? 0.8f : 0.0f);
    setParameter("eqHigh", brightProfile ? 1.4f : (darkProfile ? -1.2f : (acousticProfile ? 0.8f : 0.0f)));
    setParameter("eqOn", 1.0f);
    setParameter("reverbMix", juce::jlimit(0.0f, 1.0f, preset.reverb));
    setParameter("reverbOn", preset.reverb > 0.01f ? 1.0f : 0.0f);
    setParameter("cabinetOn", acousticProfile ? 0.0f : 1.0f);
    setParameter("cabinetMix", acousticProfile ? 0.0f : 0.28f);
    setParameter("chorusOn", (chorusProfile || wideProfile) ? 1.0f : 0.0f);
    setParameter("chorusMix", chorusProfile ? 0.18f : (wideProfile ? 0.12f : 0.0f));
    setParameter("chorusDepth", wideProfile ? 0.55f : 0.35f);
    setParameter("chorusRate", 0.32f);
    setParameter("flangerOn", flangerProfile ? 1.0f : 0.0f);
    setParameter("flangerMix", flangerProfile ? 0.18f : 0.0f);
    setParameter("delayOn", ambientProfile ? 1.0f : 0.0f);
    setParameter("delayTime", 375.0f);
    setParameter("delayMix", ambientProfile ? 0.18f : 0.0f);
    setParameter("delayFeedback", ambientProfile ? 0.28f : 0.18f);
    setParameter("doubleTrackOn", wideProfile ? 1.0f : 0.0f);
    setParameter("doubleTrackMix", wideProfile ? 0.28f : 0.0f);
    setParameter("output", juce::jlimit(-24.0f, 12.0f, preset.outputDb));

    // Preset identity is independent of the performance mode. The user chooses MELODY or PATTERN.
    const auto styleText = (preset.groove + " " + preset.articulation).toLowerCase();
    int patternStyle = 2;
    if (styleText.contains("arpeggio") || styleText.contains("picked melodic")) patternStyle = 7;
    else if (styleText.contains("stop-start") || styleText.contains("syncopated")) patternStyle = 6;
    else if (styleText.contains("ska") || styleText.contains("upstroke")) patternStyle = 4;
    else if (styleText.contains("downstroke") || styleText.contains("eighth")) patternStyle = 0;
    else if (styleText.contains("palm") || styleText.contains("muted") || styleText.contains("gallop")) patternStyle = 1;
    else if (styleText.contains("open") || styleText.contains("chorus") || styleText.contains("strum")) patternStyle = 3;
    else if (styleText.contains("alternate")) patternStyle = 5;
    if (preset.groove.isEmpty()) {
        const auto profile = preset.styleProfileId.toLowerCase();
        if (profile.contains("blink-182") || profile.contains("the-offspring") || profile.contains("green-day")) patternStyle = 0;
        else if (profile.contains("sum-41") || profile.contains("mgk") || profile.contains("fenix")) patternStyle = 1;
        else if (profile.contains("simple-plan") || profile.contains("new-found-glory") || profile.contains("good-charlotte")) patternStyle = 3;
        else if (profile.contains("mest") || profile.contains("midtown") || profile.contains("my-chemical")) patternStyle = 6;
        else if (profile.contains("angels-airwaves") || profile.contains("yellowcard")) patternStyle = 7;
        else if (profile.contains("less-than-jake")) patternStyle = 4;
        else patternStyle = 2;
    }
    setParameter("patternSlot", static_cast<float>(patternStyle));

    int articulationIndex = 0;
    if (preset.articulation.containsIgnoreCase("palm") || preset.articulation.containsIgnoreCase("muted")) articulationIndex = 1;
    else if (preset.articulation.containsIgnoreCase("flageolet") || preset.articulation.containsIgnoreCase("harmonic")) articulationIndex = 2;
    else if (preset.articulation.containsIgnoreCase("tremolo")) articulationIndex = 3;
    setParameter("articulation", static_cast<float>(articulationIndex));

    if (preset.mode == "PATTERN")
    {
        const auto detail = preset.soundCharacter + (preset.groove.isNotEmpty() ? "  /  " + preset.groove : juce::String{});
        statusLabel.setText("STYLE LOADED: " + detail + "  /  " + preset.articulation + "  /  SELECT MELODY OR PATTERN", juce::dontSendNotification);
    }
    else
    {
        auto message = "PRESET LOADED: " + preset.name + "  /  " + preset.soundCharacter;
        if (acousticProfile)
            message += "  /  LOAD AN ACOUSTIC SFZ BANK FOR ACOUSTIC SAMPLES";
        statusLabel.setText(message, juce::dontSendNotification);
    }
}
void PopPunkGuitarUltimateAudioProcessorEditor::clearFactoryPreset()
{
    // OFF is an actual deselection: restore only the parameters owned by a
    // factory preset. Keep play mode, AutoChords, key/scale, transport and
    // user-loaded sample library untouched.
    static const juce::StringArray presetOwnedParameters {
        "distortionDrive", "distortionMix", "ampGain", "tone", "eqLow", "eqMid", "eqHigh", "eqOn",
        "reverbMix", "output", "patternSlot", "articulation", "distortionOn", "reverbOn",
        "cabinetOn", "cabinetMix", "chorusOn", "chorusMix", "chorusDepth", "chorusRate",
        "flangerOn", "flangerMix", "delayOn", "delayTime", "delayMix", "delayFeedback",
        "doubleTrackOn", "doubleTrackMix"
    };

    for (const auto& parameterId : presetOwnedParameters)
        if (auto* parameter = processor.parameters.getParameter(parameterId))
            parameter->setValueNotifyingHost(parameter->getDefaultValue());
}

void PopPunkGuitarUltimateAudioProcessorEditor::chooseSfzFile()
{
    fileChooser=std::make_unique<juce::FileChooser>("Choose an SFZ guitar instrument",juce::File::getSpecialLocation(juce::File::userHomeDirectory),"*.sfz");
    juce::Component::SafePointer<PopPunkGuitarUltimateAudioProcessorEditor> safeThis(this);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safeThis](const juce::FileChooser& chooser){
        if(safeThis==nullptr) return; const auto selected=chooser.getResult(); if(selected==juce::File{}) return;
        safeThis->loadButton.setEnabled(false); safeThis->statusLabel.setText("LOADING SFZ IN BACKGROUND...",juce::dontSendNotification);
        safeThis->processor.loadSfzFileAsync(selected,[safeThis](ppgu::sampler::LoadReport report){
            if(safeThis==nullptr) return; safeThis->loadButton.setEnabled(true);
            if(report.success) safeThis->statusLabel.setText("READY  /  "+juce::String(static_cast<int>(report.loadedRegions))+" REGIONS  /  "+juce::String(static_cast<int>(report.loadedSamples))+" WAV SAMPLES",juce::dontSendNotification);
            else safeThis->statusLabel.setText(report.errors.empty()?"SFZ LOAD FAILED":juce::String(report.errors.front()),juce::dontSendNotification);
        });
    });
}
