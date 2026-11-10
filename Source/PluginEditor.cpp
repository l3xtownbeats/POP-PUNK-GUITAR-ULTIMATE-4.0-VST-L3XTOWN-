#include "PluginEditor.h"
#include "BinaryData.h"

#include <algorithm>

// Approved PPGU 4.0 dark/pink skin: all visible rotary and FX controls use APVTS attachments.

PopPunkGuitarUltimateAudioProcessorEditor::PopPunkGuitarUltimateAudioProcessorEditor(PopPunkGuitarUltimateAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    coverImage = juce::ImageCache::getFromMemory(BinaryData::Cover_Interface_jpg, BinaryData::Cover_Interface_jpgSize);
    currentSkin = juce::jlimit(0, 2, static_cast<int>(processor.parameters.state.getProperty("uiSkin", 0)));
    skinBox.addItem("SKIN: PINK", 1);
    skinBox.addItem("SKIN: CYAN", 2);
    skinBox.addItem("SKIN: AMBER", 3);
    skinBox.setSelectedId(currentSkin + 1, juce::dontSendNotification);
    skinBox.onChange = [this] { applySkin(skinBox.getSelectedId() - 1); };
    addAndMakeVisible(skinBox);
    setSize(1200, 900);
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
    loadButton.setColour(juce::TextButton::buttonColourId, pink);
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    loadButton.onClick = [this] { chooseSfzFile(); };
    addAndMakeVisible(loadButton);
    powerProfileBox.addItem("GOOD", 1);
    powerProfileBox.addItem("NORMAL", 2);
    powerProfileBox.addItem("NIGHT", 3);
    powerProfileBox.setColour(juce::ComboBox::backgroundColourId, panel2);
    powerProfileBox.setColour(juce::ComboBox::textColourId, pale);
    powerProfileBox.setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.5f));
    addAndMakeVisible(powerProfileBox);
    powerProfileAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "powerProfile", powerProfileBox);
    doubleTrackButton.setClickingTogglesState(true);
    doubleTrackButton.setColour(juce::TextButton::buttonColourId, panel2);
    doubleTrackButton.setColour(juce::TextButton::buttonOnColourId, pink);
    doubleTrackButton.setColour(juce::TextButton::textColourOffId, pale);
    doubleTrackButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    doubleTrackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters, "doubleTrackOn", doubleTrackButton);
    addAndMakeVisible(doubleTrackButton);
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
    autoChordsButton.setClickingTogglesState(true);
    autoChordsButton.setColour(juce::TextButton::buttonColourId, panel2);
    autoChordsButton.setColour(juce::TextButton::buttonOnColourId, pink);
    autoChordsButton.setColour(juce::TextButton::textColourOffId, pale);
    autoChordsButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    autoChordsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters, "autoChords", autoChordsButton);
    addAndMakeVisible(autoChordsButton);
    for (int note = 0; note < 12; ++note)
        scaleRootBox.addItem(juce::StringArray { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }[note], note + 1);
    scaleRootBox.setColour(juce::ComboBox::backgroundColourId, panel2);
    scaleRootBox.setColour(juce::ComboBox::textColourId, pale);
    scaleRootBox.setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.5f));
    addAndMakeVisible(scaleRootBox);
    scaleRootAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "scaleRoot", scaleRootBox);
    scaleModeBox.addItem("CHROMATIC", 1);
    scaleModeBox.addItem("MAJOR", 2);
    scaleModeBox.addItem("MINOR", 3);
    scaleModeBox.setColour(juce::ComboBox::backgroundColourId, panel2);
    scaleModeBox.setColour(juce::ComboBox::textColourId, pale);
    scaleModeBox.setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.5f));
    addAndMakeVisible(scaleModeBox);
    scaleModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "scaleMode", scaleModeBox);
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
    applySkin(currentSkin);
}

void PopPunkGuitarUltimateAudioProcessorEditor::applySkin(int skinIndex)
{
    currentSkin = juce::jlimit(0, 2, skinIndex);
    // Skin selection is UI state, not an audio parameter. Store it in the
    // processor ValueTree so normal DAW session save/restore remembers it.
    processor.parameters.state.setProperty("uiSkin", currentSkin, nullptr);

    if (currentSkin == 0) { bg = juce::Colour(0xff111116); panel = juce::Colour(0xff202027); panel2 = juce::Colour(0xff17171d); pink = juce::Colour(0xffed238b); pale = juce::Colour(0xfff4f0f4); muted = juce::Colour(0xffaaa8b2); }
    else if (currentSkin == 1) { bg = juce::Colour(0xff0b1419); panel = juce::Colour(0xff17272e); panel2 = juce::Colour(0xff101e25); pink = juce::Colour(0xff20d7e8); pale = juce::Colour(0xffe9fbff); muted = juce::Colour(0xff9ab9c2); }
    else { bg = juce::Colour(0xff17120c); panel = juce::Colour(0xff302317); panel2 = juce::Colour(0xff21180f); pink = juce::Colour(0xffffb13b); pale = juce::Colour(0xfffff5e6); muted = juce::Colour(0xffc4ad8d); }

    skinBox.setColour(juce::ComboBox::backgroundColourId, panel2);
    skinBox.setColour(juce::ComboBox::textColourId, pale);
    skinBox.setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.7f));
    for (auto* combo : { &presetBox, &categoryBox, &powerProfileBox, &modeBox, &startModeBox, &scaleRootBox, &scaleModeBox })
    {
        combo->setColour(juce::ComboBox::backgroundColourId, panel2);
        combo->setColour(juce::ComboBox::textColourId, pale);
        combo->setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.5f));
    }
    presetSearch.setColour(juce::TextEditor::backgroundColourId, panel2);
    presetSearch.setColour(juce::TextEditor::textColourId, pale);
    presetSearch.setColour(juce::TextEditor::outlineColourId, pink.withAlpha(0.4f));
    loadButton.setColour(juce::TextButton::buttonColourId, pink);
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    for (auto* button : { &melodyTab, &patternTab, &autoChordsButton })
    {
        button->setColour(juce::TextButton::buttonColourId, panel2);
        button->setColour(juce::TextButton::buttonOnColourId, pink);
        button->setColour(juce::TextButton::textColourOffId, pale);
        button->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }
    for (auto& button : fxButtons)
    {
        button.setColour(juce::TextButton::buttonColourId, panel2);
        button.setColour(juce::TextButton::buttonOnColourId, pink);
        button.setColour(juce::TextButton::textColourOffId, pale);
        button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }
    for (size_t i = 0; i < knobs.size(); ++i)
    {
        knobs[i].setColour(juce::Slider::rotarySliderFillColourId, pink);
        knobs[i].setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff44444e));
        knobs[i].setColour(juce::Slider::thumbColourId, pale);
        knobs[i].setColour(juce::Slider::textBoxTextColourId, pale);
        knobLabels[i].setColour(juce::Label::textColourId, muted);
    }
    for (auto* label : { &titleLabel, &subtitleLabel, &ampTitle, &fxTitle, &playbackTitle })
        label->setColour(juce::Label::textColourId, label == &subtitleLabel || label == &ampTitle || label == &fxTitle || label == &playbackTitle ? pink : pale);
    statusLabel.setColour(juce::Label::textColourId, muted);
    if (keyboard != nullptr)
    {
        keyboard->setColour(juce::MidiKeyboardComponent::whiteNoteColourId, pale);
        keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId, panel2);
        keyboard->setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, muted.withAlpha(0.35f));
        keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, pink.withAlpha(0.35f));
        keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, pink.withAlpha(0.85f));
    }
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

    // Header: match the black hardware strip and hot-pink edge from the approved cover.
    g.setColour(juce::Colour(0xff08080b));
    g.fillRect(0, 0, getWidth(), 62);
    g.setColour(pink);
    g.fillRect(0, 0, getWidth(), 3);
    g.setColour(juce::Colour(0xff41414a));
    g.drawLine(0.0f, 61.0f, static_cast<float>(getWidth()), 61.0f, 1.0f);

    // Use the actual repository cover artwork, cropped to its hero-banner area.
    auto hero = juce::Rectangle<int>(16, 68, getWidth() - 32, 202);
    if (coverImage.isValid())
    {
        const int sourceY = juce::jlimit(0, juce::jmax(0, coverImage.getHeight() - 1), 65);
        const int sourceHeight = juce::jmin(312, coverImage.getHeight() - sourceY);
        g.drawImage(coverImage, hero.getX(), hero.getY(), hero.getWidth(), hero.getHeight(),
                    0, sourceY, coverImage.getWidth(), sourceHeight);
        g.setColour(juce::Colour(0xff08080b).withAlpha(0.12f));
        g.fillRoundedRectangle(hero.toFloat(), 10.0f);
    }
    else
    {
        juce::ColourGradient fallback(juce::Colour(0xff30202f), 0.0f, 68.0f,
                                       juce::Colour(0xff101116), static_cast<float>(getWidth()), 270.0f, false);
        g.setGradientFill(fallback);
        g.fillRoundedRectangle(hero.toFloat(), 10.0f);
        g.setColour(pale);
        g.setFont(juce::Font(36.0f, juce::Font::bold));
        g.drawText("POP PUNK GUITAR ULTIMATE", hero.reduced(24), juce::Justification::centredLeft);
    }
    g.setColour(pink.withAlpha(0.72f));
    g.drawRoundedRectangle(hero.toFloat().reduced(0.5f), 10.0f, 1.2f);

    auto panelArea = getLocalBounds().reduced(16);
    panelArea.removeFromTop(355);
    panelArea.removeFromBottom(80);
    const int gap = 12;
    const int ampW = static_cast<int>(panelArea.getWidth() * 0.39f);
    const int fxW = static_cast<int>(panelArea.getWidth() * 0.39f);
    auto amp = panelArea.removeFromLeft(ampW); panelArea.removeFromLeft(gap);
    auto fx = panelArea.removeFromLeft(fxW); panelArea.removeFromLeft(gap);
    drawPanel(g, amp);
    drawPanel(g, fx);
    drawPanel(g, panelArea);

    // Fine hardware-style separators and lower keyboard frame.
    g.setColour(juce::Colour(0xff09090d));
    g.fillRect(0, getHeight() - 84, getWidth(), 84);
    g.setColour(pink.withAlpha(0.35f));
    g.drawLine(16.0f, static_cast<float>(getHeight() - 84), static_cast<float>(getWidth() - 16),
               static_cast<float>(getHeight() - 84), 1.0f);
}

void PopPunkGuitarUltimateAudioProcessorEditor::resized()
{
    auto area=getLocalBounds().reduced(16);
    auto top=area.removeFromTop(48); top.removeFromLeft(4);
    titleLabel.setBounds(top.removeFromLeft(74));
    presetBox.setBounds(top.removeFromLeft(220).reduced(3, 5));
    presetSearch.setBounds(top.removeFromLeft(145).reduced(3, 7));
    categoryBox.setBounds(top.removeFromLeft(130).reduced(3, 7));
    powerProfileBox.setBounds(top.removeFromLeft(105).reduced(3, 7));
    doubleTrackButton.setBounds(top.removeFromLeft(86).reduced(3, 7));
    skinBox.setBounds(top.removeFromLeft(112).reduced(3, 7));
    loadButton.setBounds(top.removeFromRight(108).reduced(2, 5));
    subtitleLabel.setBounds(top.reduced(4, 0));
    area.removeFromTop(220);
    auto tabs=area.removeFromTop(62);
    melodyTab.setBounds(tabs.removeFromLeft(220).reduced(2,4));
    patternTab.setBounds(tabs.removeFromLeft(190).reduced(6,4));
    modeBox.setBounds(tabs.removeFromRight(116).reduced(2,8));
    scaleModeBox.setBounds(tabs.removeFromRight(92).reduced(2,8));
    scaleRootBox.setBounds(tabs.removeFromRight(66).reduced(2,8));
    autoChordsButton.setBounds(tabs.removeFromRight(112).reduced(2,8));
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
    const auto jsonText = juce::String::fromUTF8(BinaryData::factory_bank_300_json, BinaryData::factory_bank_300_jsonSize);
    const auto root = juce::JSON::parse(jsonText);
    if (!root.isObject()) { statusLabel.setText("FACTORY PRESET CATALOG ERROR", juce::dontSendNotification); return; }
    const auto append = [this](const juce::var& array, const juce::String& mode) {
        if (auto* values = array.getArray()) for (const auto& value : *values) {
            if (!value.isObject()) continue;
            FactoryPreset preset;
            preset.name = value.getProperty("name", "Unnamed").toString();
            preset.category = value.getProperty("referenceArtist", "Original").toString();
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
