#include "PluginEditor.h"

namespace ppgu {

static const char* kApvtsKnobs[9] = { "ampGain","eqLow","eqMid","eqHigh","velocity","dynamics","swing","humanize","accent" };
static const char* kApvtsFx[8] = { "fxDistortion","fxChorus","fxDelay","fxReverb","fxCompressor","fxLimiter","fxCabSim","fxFlangerPower" };

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(p), proc_(p)
{
    setOpaque(true);
    addAndMakeVisible(keyboard);
    // Zone A
    for (auto* b : { &btnPrev, &btnNext, &btnFav, &btnSkins, &btnImport, &btnAuto,
                     &btnCurator, &btnSettings, &btnAbout })
        addAndMakeVisible(*b);
    btnPrev.onClick = [this] { /* preset prev */ };
    btnNext.onClick = [this] { /* preset next */ };
    btnFav.onClick  = [this] { /* favourite */ };
    btnSkins.onClick = [this] { applyThemeIndex(themeRandom_.nextInt(52)); };
    btnImport.onClick = [this] { togglePanelWindow(PanelId::ImportGuitar); };
    btnAuto.onClick   = [this] { togglePanelWindow(PanelId::Curator); };
    btnCurator.onClick= [this] { togglePanelWindow(PanelId::Curator); };
    btnSettings.onClick = [this] { togglePanelWindow(PanelId::Settings); };
    btnAbout.onClick   = [this] { togglePanelWindow(PanelId::About); };
    addAndMakeVisible(presetName); addAndMakeVisible(presetCategory);
    addAndMakeVisible(presetSearch);
    // Zone B
    addAndMakeVisible(browser); addAndMakeVisible(categories);
    // Zone C
    for (int i = 0; i < 9; ++i) {
        addAndMakeVisible(knobs[i]);
        knobs[i].setSliderStyle(juce::Slider::RotaryVerticalDrag);
        knobs[i].setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 14);
        knobAtt[i] = new juce::AudioProcessorValueTreeState::SliderAttachment(
            proc_.apvts, kApvtsKnobs[i], knobs[i]);
    }
    addAndMakeVisible(playModeBox);
    playModeBox.addItemList({ "MELODY", "PATTERN", "PUNK GROUP" }, 1);
    playModeBox.onChange = [this] {
        auto* p = proc_.apvts.getRawParameterValue("playMode");
        if (p) p->store(playModeBox.getSelectedId() - 1);
    };
    // Zone D
    for (int i = 0; i < 8; ++i) {
        addAndMakeVisible(fxToggles[i]);
        fxToggles[i].setButtonText(juce::StringArray { "Dist", "Cho", "Del", "Rev", "Comp", "Lim", "Cab", "Flg" }[i]);
        fxAtt[i] = new juce::AudioProcessorValueTreeState::ButtonAttachment(
            proc_.apvts, kApvtsFx[i], fxToggles[i]);
    }
    addAndMakeVisible(btnFxEdit);   btnFxEdit.onClick   = [this] { togglePanelWindow(PanelId::FxEdit); };
    addAndMakeVisible(btnAutochords); btnAutochords.onClick = [this] { togglePanelWindow(PanelId::AutoChords); };
    addAndMakeVisible(btnLoadSfz);
    btnLoadSfz.onClick = [this] {
        sfzChooser.launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc) {
                auto f = fc.getResult();
                if (f.existsAsFile()) { /* SFZ loader: parse and publish snapshot */ }
            });
    };
    keyboard.setAvailableRange(36, 84);
    setResizeLimits(1100, 640, 3840, 2160);
    startTimerHz(30);
}

void PluginEditor::timerCallback() { repaint(); }

bool PluginEditor::keyPressed(const juce::KeyPress& k) {
    if (k == juce::KeyPress('K', juce::ModifierKeys::ctrlModifier, 0)) {
        applyThemeIndex(themeRandom_.nextInt(52));
        return true;
    }
    return Component::keyPressed(k);
}

void PluginEditor::applyThemeIndex(int i) {
    themeIndex_ = juce::jlimit(0, 51, i);
    repaint();
}

void PluginEditor::togglePanelWindow(PanelId id) {
    for (auto& w : windows_)
        if (w.id == id) w.open = !w.open;
    repaint();
}

void PluginEditor::paint(juce::Graphics& g) {
    Palette pal;
    g.fillAll(ppgu::PaletteHolder::c(pal.window_bg));
    g.setColour(ppgu::PaletteHolder::c(pal.panel));
    auto r = getLocalBounds();
    g.fillRect(r.removeFromTop(40)); // zone A bg
    g.fillRect(r.removeFromBottom(64)); // zone H bg
    g.setColour(ppgu::PaletteHolder::c(pal.text_primary));
    g.setFont(juce::FontOptions(15.0f, juce::Font::bold));
    g.drawText("POP PUNK GUITAR ULTIMATE - SIGNAL FORGE REV.17", r.removeFromTop(18), juce::Justification::centred);
    g.setFont(juce::FontOptions(12.0f));
    g.drawText("Skin " + juce::String(themeIndex_ + 1) + " / 52   (SKINS button or Ctrl+K)", r.removeFromTop(16), juce::Justification::centredLeft);
}

void PluginEditor::resized() {
    auto r = getLocalBounds();
    auto top = r.removeFromTop(40); // Zone A
    auto x = top.getX();
    btnPrev.setBounds(x, 6, 24, 28); x += 28;
    btnNext.setBounds(x, 6, 24, 28); x += 28;
    btnFav.setBounds(x, 6, 24, 28); x += 28;
    presetName.setBounds(x, 6, 180, 28); x += 184;
    presetCategory.setBounds(x, 6, 120, 28); x += 124;
    presetSearch.setBounds(x, 6, 140, 28); x += 144;
    btnSkins.setBounds(x, 6, 64, 28); x += 68;
    btnImport.setBounds(x, 6, 110, 28); x += 114;
    btnAuto.setBounds(x, 6, 100, 28); x += 104;
    btnCurator.setBounds(x, 6, 76, 28); x += 80;
    btnSettings.setBounds(x, 6, 76, 28); x += 80;
    btnAbout.setBounds(top.getRight() - 70, 6, 64, 28);
    auto kb = r.removeFromBottom(64); // Zone H
    keyboard.setBounds(kb.reduced(4));
    auto body = r;
    auto brow = body.removeFromLeft(260); // Zone B
    categories.setBounds(brow.removeFromTop(28).reduced(4));
    browser.setBounds(brow.reduced(4));
    auto knobsRow = body.removeFromTop(150); // Zone C
    playModeBox.setBounds(knobsRow.removeFromLeft(120).reduced(4));
    int kw = knobsRow.getWidth() / 9;
    for (int i = 0; i < 9; ++i) knobs[i].setBounds(knobsRow.getX() + i * kw, 8, kw - 8, knobsRow.getHeight() - 16);
    auto fxRow = body.removeFromTop(48); // Zone D
    int fw = fxRow.getWidth() / 9;
    for (int i = 0; i < 8; ++i) fxToggles[i].setBounds(fxRow.getX() + i * fw, 8, fw - 8, 24);
    btnFxEdit.setBounds(fxRow.getX() + 8 * fw, 4, fw - 10, 30);
    btnAutochords.setBounds(fxRow.getX() + 8 * fw, 36, fw - 10, 22);
    btnLoadSfz.setBounds(body.removeFromTop(30).removeFromLeft(120).reduced(4));
}

} // namespace ppgu