#include "PluginEditor.h"
#include "BinaryData.h"

#include <algorithm>

// Approved PPGU 4.0 dark/pink skin: all visible rotary and FX controls use APVTS attachments.

namespace
{
    const juce::Colour bg { 0xff111116 };
    const juce::Colour panel { 0xff202027 };
    const juce::Colour panel2 { 0xff17171d };
    const juce::Colour pink { 0xffed238b };
    const juce::Colour pale { 0xfff4f0f4 };
    const juce::Colour muted { 0xffaaa8b2 };
}

PopPunkGuitarUltimateAudioProcessorEditor::PopPunkGuitarUltimateAudioProcessorEditor(PopPunkGuitarUltimateAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(1040, 790);
    setResizable(true, true);
    // The current fixed-height control stack needs ~788 px including outer margins.
    // Do not allow a window size that clips the keyboard or lower controls.
    setResizeLimits(800, 790, 1600, 1100);
    titleLabel.setText("PPGU", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
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
    categoryBox.addItem("ALL PRESETS", 1);
    categoryBox.addItem("MELODY", 2);
    categoryBox.addItem("PATTERN", 3);
    categoryBox.setSelectedId(1, juce::dontSendNotification);
    categoryBox.setColour(juce::ComboBox::backgroundColourId, panel2);
    categoryBox.setColour(juce::ComboBox::textColourId, pale);
    addAndMakeVisible(categoryBox);
    presetSearch.setTextToShowWhenEmpty("SEARCH PRESETS...", muted);
    presetSearch.setColour(juce::TextEditor::backgroundColourId, panel2);
    presetSearch.setColour(juce::TextEditor::textColourId, pale);
    presetSearch.setColour(juce::TextEditor::outlineColourId, pink.withAlpha(0.4f));
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
                || item.articulation.toLowerCase().contains(query)
                || item.searchMetadata.toLowerCase().contains(query);
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
    loadButton.setColour(juce::TextButton::buttonColourId, pink);
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    loadButton.onClick = [this] { chooseSfzFile(); };
    addAndMakeVisible(loadButton);
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
    melodyTab.onClick = [this] { modeBox.setSelectedId(1); };
    patternTab.onClick = [this] { modeBox.setSelectedId(2); };
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
    g.setColour(juce::Colour(0xff08080b));
    g.fillRect(0, 0, getWidth(), 62);
    g.setColour(pink);
    g.fillRect(0, 0, getWidth(), 3);
    auto hero = getLocalBounds().reduced(16);
    hero.removeFromTop(72);
    hero.setHeight(194);
    juce::ColourGradient grad(juce::Colour(0xff30202f), static_cast<float>(hero.getX()), static_cast<float>(hero.getY()), juce::Colour(0xff101116), static_cast<float>(hero.getRight()), static_cast<float>(hero.getBottom()), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(hero.toFloat(), 12.0f);
    g.setColour(juce::Colour(0xff7b526c).withAlpha(0.5f));
    g.drawRoundedRectangle(hero.toFloat(), 12.0f, 1.0f);
    auto textArea = hero.reduced(28, 18);
    g.setColour(pale);
    g.setFont(juce::Font(juce::jmin(42.0f, getWidth() * 0.047f), juce::Font::bold));
    g.drawText("POP PUNK", textArea.removeFromTop(50), juce::Justification::left);
    g.setColour(pink);
    g.setFont(juce::Font(juce::jmin(37.0f, getWidth() * 0.041f), juce::Font::italic | juce::Font::bold));
    g.drawText("GUITAR ULTIMATE", textArea.removeFromTop(42), juce::Justification::left);
    g.setColour(pale.withAlpha(0.8f));
    g.setFont(juce::Font(16.0f, juce::Font::italic));
    g.drawText("Play. Create. Rock.", textArea.removeFromTop(28), juce::Justification::left);
    // Stylised electric-guitar silhouette as a vector fallback; no external image dependency.
    auto guitar = hero.removeFromRight(static_cast<int>(hero.getWidth() * 0.43f)).reduced(22, 16);
    juce::Path body;
    body.startNewSubPath(guitar.getX() + guitar.getWidth()*0.48f, guitar.getY()+guitar.getHeight()*0.52f);
    body.cubicTo(guitar.getX()+guitar.getWidth()*0.28f,guitar.getY()+guitar.getHeight()*0.23f,guitar.getX()+guitar.getWidth()*0.12f,guitar.getY()+guitar.getHeight()*0.45f,guitar.getX()+guitar.getWidth()*0.28f,guitar.getY()+guitar.getHeight()*0.68f);
    body.cubicTo(guitar.getX()+guitar.getWidth()*0.38f,guitar.getY()+guitar.getHeight()*0.83f,guitar.getX()+guitar.getWidth()*0.60f,guitar.getY()+guitar.getHeight()*0.79f,guitar.getX()+guitar.getWidth()*0.68f,guitar.getY()+guitar.getHeight()*0.60f);
    body.cubicTo(guitar.getX()+guitar.getWidth()*0.83f,guitar.getY()+guitar.getHeight()*0.57f,guitar.getX()+guitar.getWidth()*0.83f,guitar.getY()+guitar.getHeight()*0.38f,guitar.getX()+guitar.getWidth()*0.68f,guitar.getY()+guitar.getHeight()*0.40f);
    body.closeSubPath();
    g.setColour(juce::Colour(0xffd9d9df));
    g.fillPath(body);
    g.setColour(juce::Colour(0xff292932));
    g.fillEllipse(guitar.getX()+guitar.getWidth()*0.43f,guitar.getY()+guitar.getHeight()*0.47f, guitar.getWidth()*0.17f,guitar.getHeight()*0.22f);
    g.setColour(juce::Colour(0xffd8b77c));
    juce::Path neck;
    neck.addQuadrilateral(static_cast<float>(guitar.getX())+static_cast<float>(guitar.getWidth())*0.60f, static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.45f, static_cast<float>(guitar.getRight()), static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.08f, static_cast<float>(guitar.getRight()), static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.20f, static_cast<float>(guitar.getX())+static_cast<float>(guitar.getWidth())*0.66f, static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.57f);
    g.fillPath(neck);
    g.setColour(pink.withAlpha(0.75f));
    g.drawLine(static_cast<float>(guitar.getX()+guitar.getWidth()*0.58f),static_cast<float>(guitar.getY()+guitar.getHeight()*0.49f),static_cast<float>(guitar.getRight()),static_cast<float>(guitar.getY()+guitar.getHeight()*0.14f),1.2f);
    auto panelArea = getLocalBounds().reduced(16);
    panelArea.removeFromTop(355);
    panelArea.removeFromBottom(80);
    const int gap=12, ampW=static_cast<int>(panelArea.getWidth()*0.39f), fxW=static_cast<int>(panelArea.getWidth()*0.39f);
    auto amp=panelArea.removeFromLeft(ampW); panelArea.removeFromLeft(gap);
    auto fx=panelArea.removeFromLeft(fxW); panelArea.removeFromLeft(gap);
    drawPanel(g,amp); drawPanel(g,fx); drawPanel(g,panelArea);
}

void PopPunkGuitarUltimateAudioProcessorEditor::resized()
{
    auto area=getLocalBounds().reduced(16);
    auto top=area.removeFromTop(48); top.removeFromLeft(4);
    titleLabel.setBounds(top.removeFromLeft(74));
    presetBox.setBounds(top.removeFromLeft(220).reduced(3, 5));
    presetSearch.setBounds(top.removeFromLeft(145).reduced(3, 7));
    categoryBox.setBounds(top.removeFromLeft(130).reduced(3, 7));
    loadButton.setBounds(top.removeFromRight(108).reduced(2, 5));
    subtitleLabel.setBounds(top.reduced(4, 0));
    area.removeFromTop(220);
    auto tabs=area.removeFromTop(62);
    melodyTab.setBounds(tabs.removeFromLeft(220).reduced(2,4));
    patternTab.setBounds(tabs.removeFromLeft(190).reduced(6,4));
    modeBox.setBounds(tabs.removeFromRight(140).reduced(2,8));
    area.removeFromTop(8);
    auto status=area.removeFromTop(28); statusLabel.setBounds(status);
    area.removeFromTop(12);
    auto panels=area.removeFromTop(300);
    const int gap=12, ampW=static_cast<int>(panels.getWidth()*0.39f), fxW=static_cast<int>(panels.getWidth()*0.39f);
    auto amp=panels.removeFromLeft(ampW); panels.removeFromLeft(gap);
    auto fx=panels.removeFromLeft(fxW); panels.removeFromLeft(gap); auto playback=panels;
    ampTitle.setBounds(amp.removeFromTop(34).reduced(12,2));
    fxTitle.setBounds(fx.removeFromTop(34).reduced(12,2));
    playbackTitle.setBounds(playback.removeFromTop(34).reduced(12,2));
    const int kw=amp.getWidth()/4;
    for(size_t i=0;i<4;++i){auto cell=amp.removeFromLeft(kw); knobLabels[i].setBounds(cell.removeFromBottom(22)); knobs[i].setBounds(cell.reduced(4,0));}
    for(size_t i=0;i<fxButtons.size();++i){auto cell=fx.removeFromTop(fx.getHeight()/static_cast<int>(fxButtons.size()-i)); fxButtons[i].setBounds(cell.reduced(8,2));}
    auto keyboardArea = area.removeFromBottom(78);
    if (keyboard != nullptr) keyboard->setBounds(keyboardArea.reduced(2, 4));
    auto playbackControls = playback.removeFromTop(190);
    const int pw = juce::jmax(1, playbackControls.getWidth() / 5);
    for (size_t i = 4; i < 9; ++i)
    {
        auto cell = playbackControls.removeFromLeft(pw);
        knobLabels[i].setBounds(cell.removeFromBottom(28));
        knobs[i].setBounds(cell.reduced(1, 4));
    }
    startModeBox.setBounds(playback.removeFromTop(34).reduced(8, 2));
}

void PopPunkGuitarUltimateAudioProcessorEditor::loadFactoryPresetCatalog()
{
    factoryPresets.clear();
    const auto jsonText = juce::String::fromUTF8(BinaryData::factory_bank_432_json, BinaryData::factory_bank_432_jsonSize);
    const auto root = juce::JSON::parse(jsonText);
    if (!root.isObject()) { statusLabel.setText("FACTORY PRESET CATALOG ERROR", juce::dontSendNotification); return; }
    const auto append = [this](const juce::var& array, const juce::String& mode) {
        if (auto* values = array.getArray()) for (const auto& value : *values) {
            if (!value.isObject()) continue;
            FactoryPreset preset;
            const auto joinStringArray = [](const juce::var& source) {
                juce::String joined;
                if (auto* items = source.getArray())
                    for (const auto& item : *items)
                    {
                        if (joined.isNotEmpty()) joined += " ";
                        joined += item.toString();
                    }
                return joined;
            };
            preset.id = value.getProperty("id", "").toString();
            preset.name = value.getProperty("name", "Unnamed").toString();
            preset.category = value.getProperty("referenceArtist", "Original").toString();
            preset.artistReference = preset.category;
            preset.referenceTracks = joinStringArray(value.getProperty("referenceTracks", juce::var()));
            preset.mode = mode;
            preset.soundCharacter = value.getProperty("soundCharacter", "Electric").toString();
            preset.groove = value.getProperty("groove", "").toString();
            preset.articulation = value.getProperty("articulation", "").toString();
            preset.tonalTarget = value.getProperty("tonalTarget", "").toString();
            preset.sampleProfile = value.getProperty("sampleProfile", "").toString();
            preset.sampleRequirement = value.getProperty("sampleRequirement", "").toString();
            preset.noteBehavior = value.getProperty("noteBehavior", "").toString();
            preset.catalogStatus = value.getProperty("status", "").toString();
            preset.styleProfileId = value.getProperty("styleProfileId", "").toString();
            preset.playingStyle = value.getProperty("playingStyle", "").toString();
            preset.availableSoundCharacters = joinStringArray(value.getProperty("availableSoundCharacters", juce::var()));
            preset.searchMetadata = preset.id + " " + preset.artistReference + " " + preset.referenceTracks
                + " " + preset.tonalTarget + " " + preset.sampleProfile + " " + preset.sampleRequirement
                + " " + preset.noteBehavior + " " + preset.catalogStatus + " " + preset.styleProfileId
                + " " + preset.playingStyle + " " + preset.availableSoundCharacters;
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
