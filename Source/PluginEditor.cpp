#include "PluginEditor.h"
#include "PPGUData.h"

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
    setSize(1120, 840);
    setResizable(true, true);
    // The current fixed-height control stack needs ~788 px including outer margins.
    // Do not allow a window size that clips the keyboard or lower controls.
    setResizeLimits(980, 800, 1600, 1100);
    titleLabel.setText("PPGU", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, themeManager.getPalette().token("text_primary"));
    addAndMakeVisible(titleLabel);
    subtitleLabel.setText("L3XTOWN AUDIO LAB", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(9.5f, juce::Font::bold));
    subtitleLabel.setColour(juce::Label::textColourId, themeManager.getPalette().token("accent"));
    addAndMakeVisible(subtitleLabel);
    presetBox.setTextWhenNothingSelected("FACTORY PRESET CATALOG");
    presetBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    presetBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    presetBox.setColour(juce::ComboBox::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.5f));
    addAndMakeVisible(presetBox);
    categoryBox.addItem("ALL PRESETS", 1);
    categoryBox.addItem("MELODY", 2);
    categoryBox.addItem("PATTERN", 3);
    categoryBox.setSelectedId(1, juce::dontSendNotification);
    categoryBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    categoryBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    addAndMakeVisible(categoryBox);
    presetSearch.setTextToShowWhenEmpty("SEARCH PRESETS...", themeManager.getPalette().token("text_secondary"));
    presetSearch.setColour(juce::TextEditor::backgroundColourId, themeManager.getPalette().token("control_bg"));
    presetSearch.setColour(juce::TextEditor::textColourId, themeManager.getPalette().token("text_primary"));
    presetSearch.setColour(juce::TextEditor::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.4f));
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
    loadButton.setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("accent"));
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    loadButton.onClick = [this] { chooseSfzFile(); };
    addAndMakeVisible(loadButton);
    skinButton.setTooltip("Visual theme only. Ctrl+K opens the skin menu.");
    skinButton.onClick = [this] { showSkinMenu(); };
    addAndMakeVisible(skinButton);
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
    statusLabel.setColour(juce::Label::textColourId, themeManager.getPalette().token("text_secondary"));
    statusLabel.setFont(juce::Font(11.0f));
    addAndMakeVisible(statusLabel);
    melodyTab.setClickingTogglesState(false);
    patternTab.setClickingTogglesState(false);
    for (auto* b : { &melodyTab, &patternTab }) {
        b->setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("control_bg"));
        b->setColour(juce::TextButton::textColourOffId, themeManager.getPalette().token("text_primary"));
        addAndMakeVisible(*b);
    }
    modeBox.addItem("MELODY", 1);
    modeBox.addItem("PATTERN", 2);
    modeBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    modeBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    modeBox.setColour(juce::ComboBox::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.5f));
    addAndMakeVisible(modeBox);
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "playMode", modeBox);
    startModeBox.addItem("HOST", 1);
    startModeBox.addItem("HOST START", 2);
    startModeBox.addItem("INTERNAL", 3);
    startModeBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    startModeBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    startModeBox.setColour(juce::ComboBox::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.5f));
    addAndMakeVisible(startModeBox);
    startModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "syncMode", startModeBox);
    keyboard = std::make_unique<juce::MidiKeyboardComponent>(processor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard);
    keyboard->setAvailableRange(36, 84);
    keyboard->setKeyWidth(18.0f);
    keyboard->setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xffe8e7eb));
    keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff17171d));
    keyboard->setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colour(0xff34343d));
    keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, themeManager.getPalette().token("accent").withAlpha(0.35f));
    keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, themeManager.getPalette().token("accent").withAlpha(0.85f));
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
        b.setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("control_bg"));
        b.setColour(juce::TextButton::buttonOnColourId, themeManager.getPalette().token("accent"));
        b.setColour(juce::TextButton::textColourOffId, themeManager.getPalette().token("text_primary"));
        b.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        fxAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters, fxParameterIDs[i], b);
        addAndMakeVisible(b);
    }
    ampTitle.setText("AMP & TONE", juce::dontSendNotification);
    fxTitle.setText("FX CHAIN", juce::dontSendNotification);
    playbackTitle.setText("PLAYBACK", juce::dontSendNotification);
    for (auto* l : { &ampTitle, &fxTitle, &playbackTitle }) {
        l->setFont(juce::Font(15.0f, juce::Font::bold));
        l->setColour(juce::Label::textColourId, themeManager.getPalette().token("accent"));
        addAndMakeVisible(*l);
    }
    juce::PropertiesFile::Options options;
    options.applicationName = "PPGU";
    options.filenameSuffix = "settings";
    options.folderName = "L3XTOWN";
    uiProperties.setStorageParameters(options);
    loadSavedSkin();
    applyPalette();
}

PopPunkGuitarUltimateAudioProcessorEditor::~PopPunkGuitarUltimateAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void PopPunkGuitarUltimateAudioProcessorEditor::configureKnob(juce::Slider& s, juce::Label& l, const juce::String& name)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 46, 16);
    s.setColour(juce::Slider::rotarySliderFillColourId, themeManager.getPalette().token("accent"));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff44444e));
    s.setColour(juce::Slider::thumbColourId, themeManager.getPalette().token("text_primary"));
    s.setColour(juce::Slider::textBoxTextColourId, themeManager.getPalette().token("text_primary"));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(s);
    l.setText(name, juce::dontSendNotification);
    l.setFont(juce::Font(9.5f, juce::Font::bold));
    l.setColour(juce::Label::textColourId, themeManager.getPalette().token("text_secondary"));
    l.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(l);
}

void PopPunkGuitarUltimateAudioProcessorEditor::drawPanel(juce::Graphics& g, juce::Rectangle<int> area)
{
    const auto& p = themeManager.getPalette();
    auto bounds = area.toFloat();
    g.setGradientFill(juce::ColourGradient(p.token("panel_elevated"), bounds.getX(), bounds.getY(),
                                           p.token("panel"), bounds.getX(), bounds.getBottom(), false));
    g.fillRoundedRectangle(bounds, 12.0f);
    g.setColour(p.token("control_border").withAlpha(0.78f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 12.0f, 1.0f);
    g.setColour(p.token("accent"));
    g.fillRoundedRectangle(bounds.getX() + 16.0f, bounds.getY() + 1.0f, 42.0f, 2.0f, 1.0f);
}

void PopPunkGuitarUltimateAudioProcessorEditor::paint(juce::Graphics& g)
{
 const auto& p=themeManager.getPalette();g.fillAll(p.token("window_bg"));g.setColour(p.token("lowlight").withAlpha(.35f));
 for(int x=16;x<getWidth();x+=32)g.drawVerticalLine(x,0,(float)getHeight());
 for(int y=16;y<getHeight();y+=32)g.drawHorizontalLine(y,0,(float)getWidth());
 g.setGradientFill(juce::ColourGradient(p.token("control_bg"),0,0,p.token("window_bg"),0,74,false));g.fillRect(0,0,getWidth(),76);g.setColour(p.token("accent"));g.fillRect(0,0,getWidth(),3);
 auto hero=getLocalBounds().reduced(18);hero.removeFromTop(66);hero.setHeight(184);
 g.setGradientFill(juce::ColourGradient(p.token("panel_elevated").brighter(.08f),(float)hero.getX(),(float)hero.getY(),p.token("control_bg"),(float)hero.getRight(),(float)hero.getBottom(),false));
 g.fillRoundedRectangle(hero.toFloat(),13);g.setColour(p.token("control_border"));g.drawRoundedRectangle(hero.toFloat().reduced(.5f),13,1);
 auto t=hero.reduced(28,17);g.setColour(p.token("accent_glow"));g.setFont(juce::Font(9.5f,juce::Font::bold));g.drawText("L3XTOWN AUDIO LAB  /  SIGNATURE INSTRUMENT",t.removeFromTop(18),juce::Justification::left);
 g.setColour(p.token("text_primary"));g.setFont(juce::Font(33,juce::Font::bold));g.drawText("POP PUNK GUITAR",t.removeFromTop(43),juce::Justification::left);
 g.setColour(p.token("accent"));g.setFont(juce::Font(39,juce::Font::bold));g.drawText("ULTIMATE",t.removeFromTop(45),juce::Justification::left);
 g.setColour(p.token("text_secondary"));g.setFont(juce::Font(10.5f));g.drawText("RIFFS  /  HOOKS  /  POWER CHORDS  /  MELODY",t.removeFromTop(17),juce::Justification::left);
 auto gr=hero.removeFromRight((int)(hero.getWidth()*.39f)).reduced(24,12);float x=(float)gr.getX(),y=(float)gr.getY(),w=(float)gr.getWidth(),h=(float)gr.getHeight();
 juce::Path neck;neck.addQuadrilateral(x+w*.56f,y+h*.43f,x+w*.96f,y+h*.05f,x+w*.99f,y+h*.17f,x+w*.65f,y+h*.57f);
 g.setGradientFill(juce::ColourGradient(juce::Colour(0xff8e9299),x+w*.5f,y,juce::Colour(0xff292d35),x+w,y+h,false));g.fillPath(neck);
 juce::Path body;body.startNewSubPath(x+w*.48f,y+h*.48f);body.cubicTo(x+w*.28f,y+h*.24f,x+w*.12f,y+h*.44f,x+w*.27f,y+h*.67f);body.cubicTo(x+w*.37f,y+h*.86f,x+w*.61f,y+h*.83f,x+w*.70f,y+h*.62f);body.cubicTo(x+w*.83f,y+h*.58f,x+w*.84f,y+h*.40f,x+w*.68f,y+h*.39f);body.closeSubPath();
 g.setGradientFill(juce::ColourGradient(juce::Colour(0xffc5c8ce),x+w*.2f,y+h*.3f,juce::Colour(0xff353a43),x+w*.8f,y+h*.8f,false));g.fillPath(body);g.setColour(p.token("control_border"));g.strokePath(body,juce::PathStrokeType(1.2f));
 g.setColour(juce::Colour(0xff171a20));g.fillRoundedRectangle(x+w*.42f,y+h*.45f,w*.22f,h*.18f,4);g.setColour(juce::Colour(0xffaeb4bc));g.fillRoundedRectangle(x+w*.43f,y+h*.49f,w*.20f,h*.035f,2);g.fillRoundedRectangle(x+w*.43f,y+h*.56f,w*.20f,h*.035f,2);
 for(int i=0;i<6;++i){float o=(float)i*1.4f;g.setColour(juce::Colours::white.withAlpha(.72f));g.drawLine(x+w*.55f+o,y+h*.46f,x+w*.96f+o*.2f,y+h*.12f,.65f);}
 g.setColour(p.token("accent"));g.fillEllipse(x+w*.56f,y+h*.69f,7,7);g.fillEllipse(x+w*.64f,y+h*.66f,7,7);
 auto panels=getLocalBounds().reduced(18);panels.removeFromTop(356);panels.removeFromBottom(92);int gap=12,usable=panels.getWidth()-gap*2,aw=(int)(usable*.38f),fw=(int)(usable*.35f);
 auto amp=panels.removeFromLeft(aw);panels.removeFromLeft(gap);auto fx=panels.removeFromLeft(fw);panels.removeFromLeft(gap);drawPanel(g,amp);drawPanel(g,fx);drawPanel(g,panels);
}

void PopPunkGuitarUltimateAudioProcessorEditor::resized()
{
 auto a=getLocalBounds().reduced(18);auto head=a.removeFromTop(56);titleLabel.setBounds(head.removeFromLeft(150).removeFromTop(29));subtitleLabel.setBounds(18,47,150,16);head.removeFromLeft(10);
 presetBox.setBounds(head.removeFromLeft(205).reduced(2,8));presetSearch.setBounds(head.removeFromLeft(160).reduced(2,8));categoryBox.setBounds(head.removeFromLeft(128).reduced(2,8));
 skinButton.setBounds(head.removeFromRight(94).reduced(2,8));loadButton.setBounds(head.removeFromRight(106).reduced(2,8));
 a.removeFromTop(10);a.removeFromTop(184);a.removeFromTop(10);auto tabs=a.removeFromTop(52);melodyTab.setBounds(tabs.removeFromLeft(190).reduced(2,3));patternTab.setBounds(tabs.removeFromLeft(190).reduced(8,3));modeBox.setBounds(tabs.removeFromRight(145).reduced(2,7));
 a.removeFromTop(8);statusLabel.setBounds(a.removeFromTop(26));a.removeFromTop(10);auto panels=a.removeFromTop(356);
 int gap=12,usable=panels.getWidth()-gap*2,aw=(int)(usable*.38f),fw=(int)(usable*.35f);auto amp=panels.removeFromLeft(aw);panels.removeFromLeft(gap);auto fx=panels.removeFromLeft(fw);panels.removeFromLeft(gap);auto play=panels;
 ampTitle.setBounds(amp.removeFromTop(38).reduced(14,2));fxTitle.setBounds(fx.removeFromTop(38).reduced(14,2));playbackTitle.setBounds(play.removeFromTop(38).reduced(12,2));
 int kw=juce::jmax(1,amp.getWidth()/4);for(size_t i=0;i<4;++i){auto cell=amp.removeFromLeft(kw);knobLabels[i].setBounds(cell.removeFromBottom(23));knobs[i].setBounds(cell.reduced(5,8));}
 for(size_t i=0;i<fxButtons.size();++i){auto row=fx.removeFromTop(fx.getHeight()/(int)(fxButtons.size()-i));fxButtons[i].setBounds(row.reduced(10,3));}
 auto controls=play.removeFromTop(270);int pw=juce::jmax(1,controls.getWidth()/5);for(size_t i=4;i<9;++i){auto cell=controls.removeFromLeft(pw);knobLabels[i].setBounds(cell.removeFromBottom(23));knobs[i].setBounds(cell.reduced(1,8));}
 startModeBox.setBounds(play.removeFromTop(36).reduced(8,2));a.removeFromTop(10);auto keys=a.removeFromBottom(82);if(keyboard)keyboard->setBounds(keys.reduced(2,4));
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
 titleLabel.setColour(juce::Label::textColourId,p.token("text_primary"));subtitleLabel.setColour(juce::Label::textColourId,p.token("text_secondary"));statusLabel.setColour(juce::Label::textColourId,p.token("text_secondary"));
 ampTitle.setColour(juce::Label::textColourId,p.token("accent"));fxTitle.setColour(juce::Label::textColourId,p.token("accent"));playbackTitle.setColour(juce::Label::textColourId,p.token("accent"));
 for(auto* b:{&presetBox,&categoryBox,&modeBox,&startModeBox}){b->setColour(juce::ComboBox::backgroundColourId,p.token("control_bg"));b->setColour(juce::ComboBox::textColourId,p.token("text_primary"));b->setColour(juce::ComboBox::outlineColourId,p.token("control_border"));b->setColour(juce::ComboBox::arrowColourId,p.token("accent"));}
 presetSearch.setColour(juce::TextEditor::backgroundColourId,p.token("control_bg"));presetSearch.setColour(juce::TextEditor::textColourId,p.token("text_primary"));presetSearch.setColour(juce::TextEditor::outlineColourId,p.token("control_border"));presetSearch.setColour(juce::TextEditor::focusedOutlineColourId,p.token("highlight"));presetSearch.setColour(juce::TextEditor::highlightColourId,p.token("accent").withAlpha(.3f));
 loadButton.setColour(juce::TextButton::buttonColourId,p.token("accent"));loadButton.setColour(juce::TextButton::textColourOffId,p.token("window_bg"));skinButton.setColour(juce::TextButton::buttonColourId,p.token("panel_elevated"));skinButton.setColour(juce::TextButton::textColourOffId,p.token("text_primary"));skinButton.setTooltip("Active skin: "+p.name+"  /  Ctrl+K");
 for(auto* b:{&melodyTab,&patternTab}){b->setColour(juce::TextButton::buttonColourId,p.token("control_bg"));b->setColour(juce::TextButton::buttonOnColourId,p.token("accent"));b->setColour(juce::TextButton::textColourOffId,p.token("text_primary"));b->setColour(juce::TextButton::textColourOnId,p.token("window_bg"));}
 for(auto& s:knobs){s.setColour(juce::Slider::rotarySliderFillColourId,p.token("accent"));s.setColour(juce::Slider::rotarySliderOutlineColourId,p.token("control_border"));s.setColour(juce::Slider::thumbColourId,p.token("text_primary"));s.setColour(juce::Slider::textBoxTextColourId,p.token("text_primary"));}
 for(auto& l:knobLabels)l.setColour(juce::Label::textColourId,p.token("text_secondary"));
 for(auto& b:fxButtons){b.setColour(juce::TextButton::buttonColourId,p.token("control_bg"));b.setColour(juce::TextButton::buttonOnColourId,p.token("accent"));b.setColour(juce::TextButton::textColourOffId,p.token("text_primary"));b.setColour(juce::TextButton::textColourOnId,p.token("window_bg"));}
 if(keyboard){keyboard->setColour(juce::MidiKeyboardComponent::whiteNoteColourId,juce::Colour(0xffeaf0f2));keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId,p.token("control_bg"));keyboard->setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId,p.token("control_border"));keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId,p.token("accent_glow").withAlpha(.3f));keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId,p.token("accent").withAlpha(.88f));}
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
