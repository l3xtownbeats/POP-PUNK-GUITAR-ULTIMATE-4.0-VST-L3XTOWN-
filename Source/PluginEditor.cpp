#include "PluginEditor.h"
#include "PPGUData.h"
#include "UI/AuxiliaryPanelLayout.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <set>
#include <limits>
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

// Premium Blackout Voltage shell. Visual theme code never touches processor/audio state.

namespace
{
    const juce::Colour bg { 0xff111116 };
    const juce::Colour panel { 0xff202027 };
    const juce::Colour panel2 { 0xff17171d };
    const juce::Colour pink { 0xffed238b };
    const juce::Colour pale { 0xfff4f0f4 };
    const juce::Colour muted { 0xffaaa8b2 };
}


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
    setSize(1648, 928);
    setResizable(true, true);
    // The current fixed-height control stack needs ~788 px including outer margins.
    // Do not allow a window size that clips the keyboard or lower controls.
    setResizeLimits(1280, 800, 1920, 1200);
    titleLabel.setText("PPGU", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, pale);
    addAndMakeVisible(titleLabel);
    subtitleLabel.setText("L3XTOWN AUDIO LAB", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(9.5f, juce::Font::bold));
    subtitleLabel.setColour(juce::Label::textColourId, pink);
    addAndMakeVisible(subtitleLabel);
    presetBox.setTextWhenNothingSelected("FACTORY PRESET CATALOG");
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
    skinButton.setTooltip("Visual theme only. Ctrl+K opens the skin menu.");
    skinButton.onClick = [this] { showSkinMenu(); };
    addAndMakeVisible(skinButton);
    autoChordsButton.onClick = [this] { openOverlay(ppgu::ui::OverlayPanelId::AutoChords); };
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
    for (auto& b : fxButtons)
        b.onClick = [this] { openOverlay(ppgu::ui::OverlayPanelId::FXEdit); };

    ampTitle.setText("AMP & TONE", juce::dontSendNotification);
    fxTitle.setText("FX CHAIN", juce::dontSendNotification);
    playbackTitle.setText("PLAYBACK", juce::dontSendNotification);
    for (auto* l : { &ampTitle, &fxTitle, &playbackTitle }) {
        l->setFont(juce::Font(15.0f, juce::Font::bold));
        l->setColour(juce::Label::textColourId, pink);
        addAndMakeVisible(*l);
    }
    juce::PropertiesFile::Options options;
    options.applicationName = "PPGU";
    options.filenameSuffix = "settings";
    options.folderName = "L3XTOWN";
    uiProperties.setStorageParameters(options);
    loadSavedSkin();
    applyPalette();
    restoreOverlayState();
}

PopPunkGuitarUltimateAudioProcessorEditor::~PopPunkGuitarUltimateAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void PopPunkGuitarUltimateAudioProcessorEditor::configureKnob(juce::Slider& s, juce::Label& l, const juce::String& name)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 46, 16);
    s.setColour(juce::Slider::rotarySliderFillColourId, pink);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff44444e));
    s.setColour(juce::Slider::thumbColourId, pale);
    s.setColour(juce::Slider::textBoxTextColourId, pale);
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(s);
    l.setText(name, juce::dontSendNotification);
    l.setFont(juce::Font(9.5f, juce::Font::bold));
    l.setColour(juce::Label::textColourId, muted);
    l.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(l);
}

void PopPunkGuitarUltimateAudioProcessorEditor::drawPanel(juce::Graphics& g, juce::Rectangle<int> area)
{
    const auto& p = themeManager.getPalette();
    auto bounds = area.toFloat();
    g.setGradientFill(juce::ColourGradient(p.surfaceRaised, bounds.getX(), bounds.getY(),
                                           p.surface, bounds.getX(), bounds.getBottom(), false));
    g.fillRoundedRectangle(bounds, 12.0f);
    g.setColour(p.border.withAlpha(0.78f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 12.0f, 1.0f);
    g.setColour(p.accent);
    g.fillRoundedRectangle(bounds.getX() + 16.0f, bounds.getY() + 1.0f, 42.0f, 2.0f, 1.0f);
}

void PopPunkGuitarUltimateAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto& p = themeManager.getPalette();
    const auto bounds = getLocalBounds();
    g.fillAll(p.canvas.darker(0.18f));

    // Brushed-metal outer chassis and inset bezel.
    g.setGradientFill(juce::ColourGradient(p.surfaceRaised.brighter(0.18f), 0.0f, 0.0f,
                                           p.canvas.darker(0.65f), 0.0f, (float)getHeight(), false));
    g.fillRoundedRectangle(bounds.toFloat().reduced(2.0f), 13.0f);
    g.setColour(p.border.withAlpha(0.95f));
    g.drawRoundedRectangle(bounds.toFloat().reduced(2.5f), 13.0f, 1.5f);
    g.setColour(p.accent.withAlpha(0.9f));
    g.fillRect(5, 5, getWidth() - 10, 2);

    // Top navigation strip.
    auto top = juce::Rectangle<int>(8, 8, getWidth() - 16, 88);
    g.setGradientFill(juce::ColourGradient(p.surfaceRaised, 0.0f, (float)top.getY(),
                                           p.canvasInset, 0.0f, (float)top.getBottom(), false));
    g.fillRoundedRectangle(top.toFloat(), 10.0f);
    g.setColour(p.border);
    g.drawRoundedRectangle(top.toFloat(), 10.0f, 1.0f);
    g.setColour(p.accent);
    g.fillRoundedRectangle(18.0f, 87.0f, (float)getWidth() - 36.0f, 2.0f, 1.0f);

    // Left preset browser and right amp stack.
    const int left = 16, topY = 108, bottom = getHeight() - 154;
    const int leftW = juce::jlimit(250, 330, getWidth() / 5);
    const int rightW = juce::jlimit(230, 300, getWidth() / 6);
    auto browser = juce::Rectangle<float>((float)left, (float)topY, (float)leftW, (float)(bottom - topY));
    auto amp = juce::Rectangle<float>((float)(getWidth() - rightW - 16), (float)(topY + 270),
                                      (float)rightW, (float)(bottom - topY - 270));
    for (const auto& panelBounds : { browser, amp })
    {
        g.setGradientFill(juce::ColourGradient(p.surfaceRaised, panelBounds.getX(), panelBounds.getY(),
                                               p.surface.darker(0.25f), panelBounds.getX(), panelBounds.getBottom(), false));
        g.fillRoundedRectangle(panelBounds, 12.0f);
        g.setColour(p.border.withAlpha(0.9f));
        g.drawRoundedRectangle(panelBounds, 12.0f, 1.0f);
    }

    // Hero guitar banner: vector artwork placeholder preserves the approved composition
    // until the supplied production artwork is installed as a real skin asset.
    const float centreX = (float)(left + leftW + 12);
    const float centreW = (float)(getWidth() - leftW - rightW - 56);
    auto hero = juce::Rectangle<float>(centreX, (float)topY, centreW, 260.0f);
    g.setGradientFill(juce::ColourGradient(p.canvasInset, hero.getX(), hero.getY(),
                                           p.surface, hero.getRight(), hero.getBottom(), false));
    g.fillRoundedRectangle(hero, 12.0f);
    g.setColour(p.border);
    g.drawRoundedRectangle(hero, 12.0f, 1.0f);
    // Electric-punk paint splashes.
    juce::Random rng(0x50504755);
    for (int i = 0; i < 110; ++i)
    {
        const float x = hero.getX() + rng.nextFloat() * hero.getWidth();
        const float y = hero.getY() + rng.nextFloat() * hero.getHeight();
        const float radius = 1.5f + rng.nextFloat() * 7.0f;
        g.setColour((i % 2 == 0 ? p.accent : p.accentSecondary).withAlpha(0.08f + rng.nextFloat() * 0.22f));
        g.fillEllipse(x, y, radius * 1.8f, radius);
    }
    // Stylised electric guitar across the banner.
    juce::Path neck;
    neck.addQuadrilateral(hero.getX() + hero.getWidth() * 0.34f, hero.getY() + hero.getHeight() * 0.49f,
                          hero.getX() + hero.getWidth() * 0.97f, hero.getY() + hero.getHeight() * 0.13f,
                          hero.getX() + hero.getWidth() * 0.98f, hero.getY() + hero.getHeight() * 0.25f,
                          hero.getX() + hero.getWidth() * 0.38f, hero.getY() + hero.getHeight() * 0.61f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xffd7dce4), hero.getX(), hero.getY(),
                                           juce::Colour(0xff303844), hero.getRight(), hero.getBottom(), false));
    g.fillPath(neck);
    juce::Path body;
    const float bx = hero.getX() + hero.getWidth() * 0.28f;
    const float by = hero.getY() + hero.getHeight() * 0.42f;
    body.startNewSubPath(bx + 50.0f, by + 10.0f);
    body.cubicTo(bx - 5.0f, by - 35.0f, bx - 38.0f, by + 35.0f, bx + 12.0f, by + 83.0f);
    body.cubicTo(bx + 62.0f, by + 122.0f, bx + 133.0f, by + 91.0f, bx + 125.0f, by + 38.0f);
    body.cubicTo(bx + 190.0f, by + 8.0f, bx + 143.0f, by - 37.0f, bx + 82.0f, by - 5.0f);
    body.closeSubPath();
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xffdce3ed), bx, by,
                                           juce::Colour(0xff202631), bx + 150.0f, by + 110.0f, false));
    g.fillPath(body);
    g.setColour(p.border);
    g.strokePath(body, juce::PathStrokeType(1.4f));
    for (int i = 0; i < 6; ++i)
    {
        const float offset = (float)i * 2.1f;
        g.setColour(juce::Colours::white.withAlpha(0.78f));
        g.drawLine(hero.getX() + hero.getWidth() * 0.36f + offset, hero.getY() + hero.getHeight() * 0.54f,
                   hero.getRight() - 18.0f + offset * 0.15f, hero.getY() + 44.0f + offset, 0.8f);
    }
    g.setColour(p.text.withAlpha(0.92f));
    g.setFont(juce::Font(juce::jmin(62.0f, hero.getWidth() * 0.075f), juce::Font::bold));
    g.drawText("PPGU", hero.toNearestInt().removeFromRight((int)(hero.getWidth() * 0.39f)).withTrimmedTop(42),
               juce::Justification::centred, false);
    g.setColour(p.accent);
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText("POP PUNK GUITAR ULTIMATE", hero.toNearestInt().reduced(20).removeFromBottom(26),
               juce::Justification::left, false);

    // Amp illustration in the right-hand strip.
    auto ampFace = amp.reduced(18.0f).withHeight(amp.getHeight() * 0.40f);
    g.setColour(p.canvas.darker(0.6f));
    g.fillRoundedRectangle(ampFace, 7.0f);
    g.setColour(p.border);
    g.drawRoundedRectangle(ampFace, 7.0f, 1.0f);
    auto grille = ampFace.reduced(9.0f, 10.0f);
    g.setColour(p.textSecondary.withAlpha(0.18f));
    for (int x = 0; x < 15; ++x)
        g.drawVerticalLine((int)(grille.getX() + x * grille.getWidth() / 15.0f), grille.getY(), grille.getBottom());
    g.setColour(p.accent.withAlpha(0.65f));
    g.drawRoundedRectangle(ampFace.reduced(3.0f), 5.0f, 1.0f);

    // Bottom performance deck and keyboard bezel.
    auto deck = juce::Rectangle<float>(8.0f, (float)getHeight() - 144.0f, (float)getWidth() - 16.0f, 128.0f);
    g.setGradientFill(juce::ColourGradient(p.surfaceRaised, deck.getX(), deck.getY(),
                                           p.canvasInset, deck.getX(), deck.getBottom(), false));
    g.fillRoundedRectangle(deck, 10.0f);
    g.setColour(p.border);
    g.drawRoundedRectangle(deck, 10.0f, 1.0f);
}
void PopPunkGuitarUltimateAudioProcessorEditor::resized()
{
    const auto area = getLocalBounds();
    auto header = area.reduced(18, 12).removeFromTop(72);
    titleLabel.setBounds(header.removeFromLeft(190).removeFromTop(38));
    subtitleLabel.setBounds(28, 68, 190, 16);
    // Preset browser controls live only in the left browser column.
    // Do not assign them here: a second setBounds() later in this method
    // used to overwrite these header bounds and made the header layout misleading.
    header.removeFromLeft(12);
    skinButton.setBounds(header.removeFromRight(116).reduced(2, 9));
    loadButton.setBounds(header.removeFromRight(132).reduced(2, 9));
    autoChordsButton.setBounds(header.removeFromRight(126).reduced(2, 9));

    const int leftW = juce::jlimit(250, 330, getWidth() / 5);
    const int rightW = juce::jlimit(230, 300, getWidth() / 6);
    const int topY = 108;
    const int bottomY = getHeight() - 154;
    auto left = juce::Rectangle<int>(16, topY, leftW, bottomY - topY);
    auto centre = juce::Rectangle<int>(left.getRight() + 12, topY,
                                       getWidth() - leftW - rightW - 56, bottomY - topY);
    auto right = juce::Rectangle<int>(getWidth() - rightW - 16, topY + 270,
                                      rightW, bottomY - topY - 270);

    // Browser column.
    presetSearch.setBounds(left.removeFromTop(38).reduced(10, 2));
    categoryBox.setBounds(left.removeFromTop(38).reduced(10, 2));
    presetBox.setBounds(left.removeFromTop(54).reduced(10, 3));
    statusLabel.setBounds(left.removeFromBottom(46).reduced(10, 3));

    // Main instrument surface: mode selector, amp controls, FX strip and transport.
    auto modeRow = centre.removeFromTop(64).reduced(8, 3);
    melodyTab.setBounds(modeRow.removeFromLeft(modeRow.getWidth() / 3).reduced(3));
    patternTab.setBounds(modeRow.removeFromLeft(modeRow.getWidth() / 2).reduced(3));
    modeBox.setBounds(modeRow.removeFromRight(130).reduced(3, 8));
    centre.removeFromTop(12);
    auto ampRow = centre.removeFromTop(174).reduced(10, 4);
    const int knobW = juce::jmax(1, ampRow.getWidth() / 5);
    for (size_t i = 4; i < knobs.size(); ++i)
    {
        auto cell = ampRow.removeFromLeft(knobW);
        knobLabels[i].setBounds(cell.removeFromBottom(24));
        knobs[i].setBounds(cell.reduced(4, 3));
    }
    auto fxRow = centre.removeFromBottom(104).reduced(8, 5);
    fxTitle.setBounds(fxRow.removeFromTop(22));
    const int fxW = juce::jmax(1, fxRow.getWidth() / (int)fxButtons.size());
    for (size_t i = 0; i < fxButtons.size(); ++i)
    {
        auto cell = fxRow.removeFromLeft(fxW);
        fxButtons[i].setBounds(cell.reduced(3, 5));
    }
    auto transport = centre.removeFromBottom(48).reduced(10, 3);
    playbackTitle.setBounds(transport);

    // Amp & tone strip: four retained APVTS controls.
    ampTitle.setBounds(right.removeFromTop(34).reduced(10, 2));
    right.removeFromTop(112); // cabinet/grille visual
    const int toneW = juce::jmax(1, right.getWidth() / 4);
    for (size_t i = 0; i < 4; ++i)
    {
        auto cell = right.removeFromLeft(toneW);
        knobLabels[i].setBounds(cell.removeFromBottom(24));
        knobs[i].setBounds(cell.reduced(2, 3));
    }

    // The on-screen MIDI keyboard is a fixed, full-width lower deck.
    auto keyboardBounds = area.reduced(20, 0).withY(getHeight() - 132).withHeight(104);
    if (keyboard)
        keyboard->setBounds(keyboardBounds.reduced(2, 2));
    layoutOverlays();
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
    const auto id = settings ? settings->getValue("skinId", "skin-51") : juce::String("skin-51");
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
 const auto& p=themeManager.getPalette();premiumLookAndFeel.setPalette(p);
 titleLabel.setColour(juce::Label::textColourId,p.text);subtitleLabel.setColour(juce::Label::textColourId,p.textSecondary);statusLabel.setColour(juce::Label::textColourId,p.textSecondary);
 ampTitle.setColour(juce::Label::textColourId,p.accent);fxTitle.setColour(juce::Label::textColourId,p.accent);playbackTitle.setColour(juce::Label::textColourId,p.accent);
 for(auto* b:{&presetBox,&categoryBox,&modeBox,&startModeBox}){b->setColour(juce::ComboBox::backgroundColourId,p.surfaceSecondary);b->setColour(juce::ComboBox::textColourId,p.text);b->setColour(juce::ComboBox::outlineColourId,p.border);b->setColour(juce::ComboBox::arrowColourId,p.accent);}
 presetSearch.setColour(juce::TextEditor::backgroundColourId,p.surfaceSecondary);presetSearch.setColour(juce::TextEditor::textColourId,p.text);presetSearch.setColour(juce::TextEditor::outlineColourId,p.border);presetSearch.setColour(juce::TextEditor::focusedOutlineColourId,p.focus);presetSearch.setColour(juce::TextEditor::highlightColourId,p.accent.withAlpha(.3f));
 loadButton.setColour(juce::TextButton::buttonColourId,p.accent);loadButton.setColour(juce::TextButton::textColourOffId,p.canvas);skinButton.setColour(juce::TextButton::buttonColourId,p.surfaceRaised);skinButton.setColour(juce::TextButton::textColourOffId,p.text);skinButton.setTooltip("Active skin: "+p.name+"  /  Ctrl+K");
 for(auto* b:{&melodyTab,&patternTab}){b->setColour(juce::TextButton::buttonColourId,p.surfaceSecondary);b->setColour(juce::TextButton::buttonOnColourId,p.accent);b->setColour(juce::TextButton::textColourOffId,p.text);b->setColour(juce::TextButton::textColourOnId,p.canvas);}
 for(auto& s:knobs){s.setColour(juce::Slider::rotarySliderFillColourId,p.accent);s.setColour(juce::Slider::rotarySliderOutlineColourId,p.border);s.setColour(juce::Slider::thumbColourId,p.text);s.setColour(juce::Slider::textBoxTextColourId,p.text);}
 for(auto& l:knobLabels)l.setColour(juce::Label::textColourId,p.textSecondary);
 for(auto& b:fxButtons){b.setColour(juce::TextButton::buttonColourId,p.surfaceSecondary);b.setColour(juce::TextButton::buttonOnColourId,p.accent);b.setColour(juce::TextButton::textColourOffId,p.text);b.setColour(juce::TextButton::textColourOnId,p.canvas);}
 if(keyboard){keyboard->setColour(juce::MidiKeyboardComponent::whiteNoteColourId,juce::Colour(0xffeaf0f2));keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId,p.surfaceSecondary);keyboard->setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId,p.border);keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId,p.accentSecondary.withAlpha(.3f));keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId,p.accent.withAlpha(.88f));}
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

void PopPunkGuitarUltimateAudioProcessorEditor::openOverlay(ppgu::ui::OverlayPanelId id)
{
    const auto index = id == ppgu::ui::OverlayPanelId::AutoChords ? 0u : 1u;
    if (!overlayPanels[index])
    {
        overlayPanels[index] = std::make_unique<ppgu::ui::OverlayPanel>(
            id, processor, [this, index] {
                persistOverlayState(index, false);
                if (overlayPanels[index])
                {
                    removeChildComponent(overlayPanels[index].get());
                    overlayPanels[index].reset();
                    layoutOverlays();
                }
            }, [this, index](bool compact) {
                if (restoringOverlayState) return;
                if (auto* settings = uiProperties.getUserSettings())
                {
                    settings->setValue("overlay." + juce::String(index) + ".compact", compact);
                    settings->saveIfNeeded();
                }
            });
        addAndMakeVisible(*overlayPanels[index]);
    }
    overlayPanels[index]->toFront(false);
    if (!restoringOverlayState) persistOverlayState(index, true);
    layoutOverlays();
}
void PopPunkGuitarUltimateAudioProcessorEditor::layoutOverlays()
{
    std::vector<ppgu::ui::PanelRequest> requests;
    for (size_t i = 0; i < overlayPanels.size(); ++i)
        if (overlayPanels[i])
            requests.push_back({ static_cast<int>(i + 1), { 620, 440 }, { 300, 210 } });
    if (requests.empty()) return;
    const auto work = getLocalBounds().reduced(28, 112).withTrimmedBottom(132);
    const auto scale = 1.0f; // DPI-aware metrics are introduced in O2.
    const auto result = ppgu::ui::calculatePanelLayout(
        { work.getX(), work.getY(), work.getWidth(), work.getHeight() }, requests, scale, 12);
    for (const auto& placement : result.panels)
    {
        const auto index = static_cast<size_t>(placement.id - 1);
        if (index < overlayPanels.size() && overlayPanels[index])
        {
            overlayPanels[index]->setBounds(placement.bounds.x, placement.bounds.y,
                                            placement.bounds.width, placement.bounds.height);
            if (result.compact || result.overflow)
                overlayPanels[index]->setCompact(true);
            persistOverlayState(index, true);
        }
    }
}

void PopPunkGuitarUltimateAudioProcessorEditor::persistOverlayState(size_t index, bool open)
{
    if (index >= overlayPanels.size() || restoringOverlayState) return;
    if (auto* settings = uiProperties.getUserSettings())
    {
        const auto prefix = "overlay." + juce::String(static_cast<int>(index)) + ".";
        settings->setValue(prefix + "open", open);
        if (overlayPanels[index])
        {
            const auto bounds = overlayPanels[index]->getBounds();
            settings->setValue(prefix + "x", bounds.getX());
            settings->setValue(prefix + "y", bounds.getY());
            settings->setValue(prefix + "width", bounds.getWidth());
            settings->setValue(prefix + "height", bounds.getHeight());
            settings->setValue(prefix + "compact", overlayPanels[index]->isCompact());
        }
        settings->saveIfNeeded();
    }
}

void PopPunkGuitarUltimateAudioProcessorEditor::restoreOverlayState()
{
    auto* settings = uiProperties.getUserSettings();
    if (settings == nullptr) return;
    restoringOverlayState = true;
    for (size_t index = 0; index < overlayPanels.size(); ++index)
    {
        const auto prefix = "overlay." + juce::String(static_cast<int>(index)) + ".";
        if (settings->getBoolValue(prefix + "open", false))
            openOverlay(index == 0 ? ppgu::ui::OverlayPanelId::AutoChords
                                   : ppgu::ui::OverlayPanelId::FXEdit);
        if (overlayPanels[index])
        {
            overlayPanels[index]->setCompact(settings->getBoolValue(prefix + "compact", false));
            const auto x = settings->getIntValue(prefix + "x", -1);
            const auto y = settings->getIntValue(prefix + "y", -1);
            const auto w = settings->getIntValue(prefix + "width", 0);
            const auto h = settings->getIntValue(prefix + "height", 0);
            if (w >= 300 && h >= 180)
                overlayPanels[index]->setBounds(juce::Rectangle<int>(x, y, w, h)
                    .constrainedWithin(getLocalBounds().reduced(12)));
        }
    }
    restoringOverlayState = false;
    layoutOverlays();
}
