#include "PluginEditor.h"
#include "BinaryData.h"

#include <algorithm>
#include <cstdint>

// UI theme changes run on the message thread and never touch audio processing.

PopPunkGuitarUltimateAudioProcessorEditor::PopPunkGuitarUltimateAudioProcessorEditor(PopPunkGuitarUltimateAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    loadThemes();
    setSize(1180, 860);
    setResizable(true, true);
    // The current fixed-height control stack needs ~788 px including outer margins.
    // Do not allow a window size that clips the keyboard or lower controls.
    setResizeLimits(1040, 820, 1920, 1400);
    titleLabel.setText("PPGU", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, theme.text);
    addAndMakeVisible(titleLabel);
    subtitleLabel.setText("POP PUNK GUITAR ULTIMATE   /   PLAY. CREATE. ROCK.", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    subtitleLabel.setColour(juce::Label::textColourId, theme.accent);
    addAndMakeVisible(subtitleLabel);
    presetBox.setTextWhenNothingSelected("FACTORY PRESETS  /  300");
    presetBox.setColour(juce::ComboBox::backgroundColourId, theme.panelRaised);
    presetBox.setColour(juce::ComboBox::textColourId, theme.text);
    presetBox.setColour(juce::ComboBox::outlineColourId, theme.accent.withAlpha(0.5f));
    addAndMakeVisible(presetBox);
    categoryBox.addItem("ALL PRESETS", 1);
    categoryBox.addItem("MELODY", 2);
    categoryBox.addItem("PATTERN", 3);
    categoryBox.setSelectedId(1, juce::dontSendNotification);
    categoryBox.setColour(juce::ComboBox::backgroundColourId, theme.panelRaised);
    categoryBox.setColour(juce::ComboBox::textColourId, theme.text);
    addAndMakeVisible(categoryBox);
    presetSearch.setTextToShowWhenEmpty("SEARCH PRESETS...", theme.muted);
    presetSearch.setColour(juce::TextEditor::backgroundColourId, theme.panelRaised);
    presetSearch.setColour(juce::TextEditor::textColourId, theme.text);
    presetSearch.setColour(juce::TextEditor::outlineColourId, theme.accent.withAlpha(0.4f));
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
    loadButton.setColour(juce::TextButton::buttonColourId, theme.accent);
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    loadButton.onClick = [this] { chooseSfzFile(); };
    addAndMakeVisible(loadButton);
    themeButton.setTooltip("SKIN: choose one of 20 complete visual themes. Changes never interrupt playback.");
    themeButton.setColour(juce::TextButton::buttonColourId, theme.accent);
    themeButton.setColour(juce::TextButton::textColourOffId, theme.text);
    themeButton.onClick = [this]
    {
        juce::PopupMenu menu;
        int selectedItem = 0;
        for (size_t i = 0; i < themes.size(); ++i)
        {
            const int itemId = static_cast<int>(i + 1);
            const bool active = themes[i].name == theme.name;
            menu.addItem(itemId, themes[i].name, true, active);
            if (active) selectedItem = itemId;
        }
        juce::Component::SafePointer<PopPunkGuitarUltimateAudioProcessorEditor> safeThis(this);
        menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&themeButton),
            [safeThis](int result)
            {
                if (safeThis != nullptr && result > 0)
                    safeThis->applyTheme(result - 1);
            });
        juce::ignoreUnused(selectedItem);
    };
    addAndMakeVisible(themeButton);
    const auto savedTheme = processor.parameters.state.getProperty("uiTheme", "Neon Riot").toString();
    int savedThemeIndex = 0;
    for (size_t i = 0; i < themes.size(); ++i)
        if (themes[i].name == savedTheme) { savedThemeIndex = static_cast<int>(i); break; }
    applyTheme(savedThemeIndex);

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
    statusLabel.setColour(juce::Label::textColourId, theme.muted);
    statusLabel.setFont(juce::Font(11.0f));
    addAndMakeVisible(statusLabel);
    melodyTab.setClickingTogglesState(false);
    patternTab.setClickingTogglesState(false);
    for (auto* b : { &melodyTab, &patternTab }) {
        b->setColour(juce::TextButton::buttonColourId, theme.panelRaised);
        b->setColour(juce::TextButton::textColourOffId, theme.text);
        addAndMakeVisible(*b);
    }
    modeBox.addItem("MELODY", 1);
    modeBox.addItem("PATTERN", 2);
    modeBox.setColour(juce::ComboBox::backgroundColourId, theme.panelRaised);
    modeBox.setColour(juce::ComboBox::textColourId, theme.text);
    modeBox.setColour(juce::ComboBox::outlineColourId, theme.accent.withAlpha(0.5f));
    addAndMakeVisible(modeBox);
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "playMode", modeBox);
    startModeBox.addItem("HOST", 1);
    startModeBox.addItem("HOST START", 2);
    startModeBox.addItem("INTERNAL", 3);
    startModeBox.setColour(juce::ComboBox::backgroundColourId, theme.panelRaised);
    startModeBox.setColour(juce::ComboBox::textColourId, theme.text);
    startModeBox.setColour(juce::ComboBox::outlineColourId, theme.accent.withAlpha(0.5f));
    addAndMakeVisible(startModeBox);
    startModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "syncMode", startModeBox);
    keyboard = std::make_unique<juce::MidiKeyboardComponent>(processor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard);
    keyboard->setAvailableRange(36, 84);
    keyboard->setKeyWidth(18.0f);
    keyboard->setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xffe8e7eb));
    keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff17171d));
    keyboard->setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colour(0xff34343d));
    keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, theme.accent.withAlpha(0.35f));
    keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, theme.accent.withAlpha(0.85f));
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
        b.setColour(juce::TextButton::buttonColourId, theme.panelRaised);
        b.setColour(juce::TextButton::buttonOnColourId, theme.accent);
        b.setColour(juce::TextButton::textColourOffId, theme.text);
        b.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        fxAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters, fxParameterIDs[i], b);
        addAndMakeVisible(b);
    }
    ampTitle.setText("AMP & TONE", juce::dontSendNotification);
    fxTitle.setText("FX CHAIN", juce::dontSendNotification);
    playbackTitle.setText("PLAYBACK", juce::dontSendNotification);
    for (auto* l : { &ampTitle, &fxTitle, &playbackTitle }) {
        l->setFont(juce::Font(15.0f, juce::Font::bold));
        l->setColour(juce::Label::textColourId, theme.accent);
        addAndMakeVisible(*l);
    }
}

void PopPunkGuitarUltimateAudioProcessorEditor::configureKnob(juce::Slider& s, juce::Label& l, const juce::String& name)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, theme.accent);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff44444e));
    s.setColour(juce::Slider::thumbColourId, theme.text);
    s.setColour(juce::Slider::textBoxTextColourId, theme.text);
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(s);
    l.setText(name, juce::dontSendNotification);
    l.setFont(juce::Font(11.0f, juce::Font::bold));
    l.setColour(juce::Label::textColourId, theme.muted);
    l.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(l);
}

void PopPunkGuitarUltimateAudioProcessorEditor::drawPanel(juce::Graphics& g, juce::Rectangle<int> r)
{
    juce::ColourGradient card(theme.panelRaised, static_cast<float>(r.getX()), static_cast<float>(r.getY()),
                              theme.panel, static_cast<float>(r.getRight()), static_cast<float>(r.getBottom()), false);
    g.setGradientFill(card);
    g.fillRoundedRectangle(r.toFloat(), 14.0f);
    g.setColour(theme.outline);
    g.drawRoundedRectangle(r.toFloat().reduced(0.5f), 14.0f, 1.0f);
    g.setColour(theme.accent.withAlpha(0.8f));
    g.fillRoundedRectangle(static_cast<float>(r.getX() + 14), static_cast<float>(r.getY() + 12), 3.0f, 18.0f, 1.5f);
}

void PopPunkGuitarUltimateAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(theme.background);

    // DIY pop-punk poster language: photocopy grain, checkerboard, tape,
    // marker strokes and stage-light accents. Everything is vector-drawn,
    // cached by JUCE's normal paint path and requires no external artwork.
    const auto w = getWidth();
    const auto h = getHeight();
    g.setColour(theme.text.withAlpha(0.025f));
    for (int y = 76; y < h; y += 22)
        for (int x = 8 + ((y / 22) % 2) * 11; x < w; x += 22)
            g.fillEllipse(static_cast<float>(x), static_cast<float>(y), 1.4f, 1.4f);

    // Top stage fascia and high-contrast checker strip.
    g.setColour(theme.panel);
    g.fillRect(0, 0, w, 62);
    g.setColour(theme.accent);
    g.fillRect(0, 0, w, 4);
    const int checkerSize = 12;
    for (int x = w - 156; x < w; x += checkerSize)
        for (int y = 8; y < 44; y += checkerSize)
            if (((x / checkerSize) + (y / checkerSize)) % 2 == 0)
            {
                g.setColour(theme.text.withAlpha(0.20f));
                g.fillRect(x, y, checkerSize, checkerSize);
            }

    // Slanted hand-cut poster tape at the top-left and small starburst mark.
    juce::Path tape;
    tape.addQuadrilateral(18.0f, 68.0f, 116.0f, 68.0f, 108.0f, 79.0f, 10.0f, 79.0f);
    g.setColour(theme.warning.withAlpha(0.70f));
    g.fillPath(tape);
    g.setColour(theme.accent);
    juce::Path spark;
    spark.startNewSubPath(0.0f, 0.0f);
    const float sx = static_cast<float>(w - 44), sy = 88.0f;
    for (int i = 0; i < 16; ++i)
    {
        const float angle = juce::MathConstants<float>::twoPi * static_cast<float>(i) / 16.0f;
        const float radius = (i % 2 == 0) ? 17.0f : 7.0f;
        const auto point = juce::Point<float>(sx + std::cos(angle) * radius, sy + std::sin(angle) * radius);
        if (i == 0) spark.startNewSubPath(point); else spark.lineTo(point);
    }
    spark.closeSubPath();
    g.fillPath(spark);
    auto hero = getLocalBounds().reduced(16);
    hero.removeFromTop(72);
    hero.setHeight(194);
    juce::ColourGradient grad(theme.panelRaised, static_cast<float>(hero.getX()), static_cast<float>(hero.getY()), theme.background, static_cast<float>(hero.getRight()), static_cast<float>(hero.getBottom()), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(hero.toFloat(), 12.0f);
    g.setColour(theme.accent.withAlpha(0.35f));
    g.drawRoundedRectangle(hero.toFloat(), 12.0f, 1.0f);
    auto textArea = hero.reduced(28, 18);
    g.setColour(theme.text);
    g.setFont(juce::Font(juce::jmin(43.0f, getWidth() * 0.048f), juce::Font::bold));
    g.drawText("POP PUNK", textArea.removeFromTop(49), juce::Justification::left);
    g.setColour(theme.accent);
    g.setFont(juce::Font(juce::jmin(36.0f, getWidth() * 0.040f), juce::Font::italic | juce::Font::bold));
    g.drawText("GUITAR ULTIMATE", textArea.removeFromTop(40), juce::Justification::left);
    g.setColour(theme.text.withAlpha(0.86f));
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.drawText("FAST RIFFS  /  BIG CHORUSES  /  LOUD GUITARS", textArea.removeFromTop(24), juce::Justification::left);

    // Marker underline and cut-out genre stickers.
    const float underlineY = static_cast<float>(hero.getY() + 118);
    g.setColour(theme.warning);
    g.drawLine(static_cast<float>(hero.getX() + 27), underlineY,
               static_cast<float>(hero.getX() + 245), underlineY - 3.0f, 3.0f);
    auto sticker = hero.removeFromBottom(30).removeFromLeft(300);
    g.setColour(theme.accent.withAlpha(0.95f));
    g.fillRoundedRectangle(sticker.toFloat(), 4.0f);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawText("SKATE PUNK  •  2000s ALT ROCK  •  DIY", sticker.reduced(8, 2), juce::Justification::centredLeft);
    // Stylised electric-guitar silhouette as a vector fallback; no external image dependency.
    auto guitar = hero.removeFromRight(static_cast<int>(hero.getWidth() * 0.43f)).reduced(22, 16);
    juce::Path body;
    body.startNewSubPath(guitar.getX() + guitar.getWidth()*0.48f, guitar.getY()+guitar.getHeight()*0.52f);
    body.cubicTo(guitar.getX()+guitar.getWidth()*0.28f,guitar.getY()+guitar.getHeight()*0.23f,guitar.getX()+guitar.getWidth()*0.12f,guitar.getY()+guitar.getHeight()*0.45f,guitar.getX()+guitar.getWidth()*0.28f,guitar.getY()+guitar.getHeight()*0.68f);
    body.cubicTo(guitar.getX()+guitar.getWidth()*0.38f,guitar.getY()+guitar.getHeight()*0.83f,guitar.getX()+guitar.getWidth()*0.60f,guitar.getY()+guitar.getHeight()*0.79f,guitar.getX()+guitar.getWidth()*0.68f,guitar.getY()+guitar.getHeight()*0.60f);
    body.cubicTo(guitar.getX()+guitar.getWidth()*0.83f,guitar.getY()+guitar.getHeight()*0.57f,guitar.getX()+guitar.getWidth()*0.83f,guitar.getY()+guitar.getHeight()*0.38f,guitar.getX()+guitar.getWidth()*0.68f,guitar.getY()+guitar.getHeight()*0.40f);
    body.closeSubPath();
    g.setColour(theme.text.withAlpha(0.88f));
    g.fillPath(body);
    g.setColour(theme.panelRaised);
    g.fillEllipse(guitar.getX()+guitar.getWidth()*0.43f,guitar.getY()+guitar.getHeight()*0.47f, guitar.getWidth()*0.17f,guitar.getHeight()*0.22f);
    g.setColour(theme.accentAlt);
    juce::Path neck;
    neck.addQuadrilateral(static_cast<float>(guitar.getX())+static_cast<float>(guitar.getWidth())*0.60f, static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.45f, static_cast<float>(guitar.getRight()), static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.08f, static_cast<float>(guitar.getRight()), static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.20f, static_cast<float>(guitar.getX())+static_cast<float>(guitar.getWidth())*0.66f, static_cast<float>(guitar.getY())+static_cast<float>(guitar.getHeight())*0.57f);
    g.fillPath(neck);
    g.setColour(theme.accent.withAlpha(0.75f));
    g.drawLine(static_cast<float>(guitar.getX()+guitar.getWidth()*0.58f),static_cast<float>(guitar.getY()+guitar.getHeight()*0.49f),static_cast<float>(guitar.getRight()),static_cast<float>(guitar.getY()+guitar.getHeight()*0.14f),1.2f);

    // Tiny set-list labels evoke gig posters without copying band logos,
    // album covers or protected artwork.
    g.setColour(theme.warning);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("RIFF / HOOK / REPEAT", guitar.getX() + 2, guitar.getBottom() - 18,
               guitar.getWidth() - 4, 14, juce::Justification::centredRight);
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
    auto top=area.removeFromTop(54); top.removeFromLeft(4);
    titleLabel.setBounds(top.removeFromLeft(78));
    presetBox.setBounds(top.removeFromLeft(205).reduced(3, 7));
    presetSearch.setBounds(top.removeFromLeft(150).reduced(3, 8));
    categoryBox.setBounds(top.removeFromLeft(118).reduced(3, 8));
    themeButton.setBounds(top.removeFromRight(150).reduced(2, 7));
    loadButton.setBounds(top.removeFromRight(112).reduced(2, 7));
    subtitleLabel.setBounds(top.reduced(4, 0));
    area.removeFromTop(214);
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
void PopPunkGuitarUltimateAudioProcessorEditor::loadThemes()
{
    const auto jsonText = juce::String::fromUTF8(BinaryData::theme_manifest_json,
                                                 BinaryData::theme_manifest_jsonSize);
    const auto root = juce::JSON::parse(jsonText);
    const auto parseColour = [](const juce::var& value, juce::Colour fallback)
    {
        auto hex = value.toString().trimCharactersAtStart("#");
        if (hex.length() == 8) return juce::Colour::fromRGBA(static_cast<juce::uint8>(hex.substring(0, 2).getHexValue32()), static_cast<juce::uint8>(hex.substring(2, 4).getHexValue32()), static_cast<juce::uint8>(hex.substring(4, 6).getHexValue32()), static_cast<juce::uint8>(hex.substring(6, 8).getHexValue32()));
        if (hex.length() != 6) return fallback;
        return juce::Colour::fromRGB(
            static_cast<juce::uint8>(hex.substring(0, 2).getHexValue32()),
            static_cast<juce::uint8>(hex.substring(2, 4).getHexValue32()),
            static_cast<juce::uint8>(hex.substring(4, 6).getHexValue32()));
    };

    themes.clear();
    const auto entriesVar = root.getProperty("themes", juce::var());
    if (auto* entries = entriesVar.getArray())
        for (const auto& entry : *entries)
        {
            if (!entry.isObject()) continue;
            const auto colors = entry.getProperty("colors", juce::var());
            ThemePalette item;
            item.name = entry.getProperty("name", "Unnamed Theme").toString();
            item.background = parseColour(colors.getProperty("background", {}), juce::Colour(0xff0b0d13));
            item.panel = parseColour(colors.getProperty("panel", {}), juce::Colour(0xff151923));
            item.panelRaised = parseColour(colors.getProperty("panelRaised", {}), juce::Colour(0xff202635));
            item.text = parseColour(colors.getProperty("text", {}), juce::Colour(0xfff2f4fa));
            item.muted = parseColour(colors.getProperty("muted", {}), juce::Colour(0xff9aa6b8));
            item.accent = parseColour(colors.getProperty("accent", {}), juce::Colour(0xffff2e93));
            item.accentAlt = parseColour(colors.getProperty("accentAlt", {}), juce::Colour(0xff22d3ee));
            item.outline = parseColour(colors.getProperty("outline", {}), juce::Colour(0x22ffffff));
            item.meter = parseColour(colors.getProperty("meter", {}), juce::Colour(0xff42d392));
            item.warning = parseColour(colors.getProperty("warning", {}), juce::Colour(0xfff5b942));
            item.error = parseColour(colors.getProperty("error", {}), juce::Colour(0xffff5964));
            themes.push_back(item);
        }

    if (themes.empty())
    {
        ThemePalette fallback;
        fallback.name = "Neon Riot";
        fallback.background = juce::Colour(0xff0b0d13);
        fallback.panel = juce::Colour(0xff151923);
        fallback.panelRaised = juce::Colour(0xff202635);
        fallback.text = juce::Colour(0xfff2f4fa);
        fallback.muted = juce::Colour(0xff9aa6b8);
        fallback.accent = juce::Colour(0xffff2e93);
        fallback.accentAlt = juce::Colour(0xff22d3ee);
        fallback.outline = juce::Colour(0x22ffffff);
        fallback.meter = juce::Colour(0xff42d392);
        fallback.warning = juce::Colour(0xfff5b942);
        fallback.error = juce::Colour(0xffff5964);
        themes.push_back(fallback);
    }
    theme = themes.front();
}

void PopPunkGuitarUltimateAudioProcessorEditor::applyTheme(int index)
{
    if (index < 0 || index >= static_cast<int>(themes.size())) return;
    theme = themes[static_cast<size_t>(index)];
    const auto styleCombo = [this](juce::ComboBox& c)
    {
        c.setColour(juce::ComboBox::backgroundColourId, theme.panelRaised);
        c.setColour(juce::ComboBox::textColourId, theme.text);
        c.setColour(juce::ComboBox::outlineColourId, theme.outline);
        c.setColour(juce::ComboBox::arrowColourId, theme.accent);
    };
    const auto styleButton = [this](juce::Button& b)
    {
        b.setColour(juce::TextButton::buttonColourId, theme.panelRaised);
        b.setColour(juce::TextButton::buttonOnColourId, theme.accent);
        b.setColour(juce::TextButton::textColourOffId, theme.text);
        b.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    };
    for (auto* c : { &presetBox, &categoryBox, &modeBox, &startModeBox }) styleCombo(*c);
    for (auto* b : { &loadButton, &themeButton, &melodyTab, &patternTab }) styleButton(*b);
    for (auto& b : fxButtons) styleButton(b);
    presetSearch.setColour(juce::TextEditor::backgroundColourId, theme.panelRaised);
    presetSearch.setColour(juce::TextEditor::textColourId, theme.text);
    presetSearch.setColour(juce::TextEditor::outlineColourId, theme.outline);
    presetSearch.setColour(juce::TextEditor::highlightColourId, theme.accent.withAlpha(0.35f));
    for (auto* l : { &titleLabel, &ampTitle, &fxTitle, &playbackTitle }) l->setColour(juce::Label::textColourId, theme.accent);
    subtitleLabel.setColour(juce::Label::textColourId, theme.accentAlt);
    statusLabel.setColour(juce::Label::textColourId, theme.muted);
    for (auto& label : knobLabels) label.setColour(juce::Label::textColourId, theme.muted);
    for (auto& knob : knobs)
    {
        knob.setColour(juce::Slider::rotarySliderFillColourId, theme.accent);
        knob.setColour(juce::Slider::rotarySliderOutlineColourId, theme.outline);
        knob.setColour(juce::Slider::thumbColourId, theme.text);
        knob.setColour(juce::Slider::textBoxTextColourId, theme.text);
        knob.setColour(juce::Slider::textBoxBackgroundColourId, theme.panelRaised);
        knob.setColour(juce::Slider::textBoxOutlineColourId, theme.outline);
    }
    if (keyboard != nullptr)
    {
        keyboard->setColour(juce::MidiKeyboardComponent::whiteNoteColourId, theme.text.withAlpha(0.95f));
        keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId, theme.panelRaised);
        keyboard->setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, theme.outline);
        keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, theme.accentAlt.withAlpha(0.35f));
        keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, theme.accent.withAlpha(0.85f));
    }
    processor.parameters.state.setProperty("uiTheme", theme.name, nullptr);
    themeButton.setButtonText("SKIN  /  " + theme.name.toUpperCase() + "  v");
    repaint();
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
