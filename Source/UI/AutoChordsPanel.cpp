#include "AutoChordsPanel.h"

PPGUAutoChordsPanel::PPGUAutoChordsPanel(const PPGUThemeManager::Palette& p) : palette(p)
{
    label(title, "♫  AUTOCHORDS  /  HARMONY WORKSPACE", 16.0f);
    label(status, "MELODY MODE  •  MIDI ROUTING PENDING", 9.0f);
    label(currentCaption, "CURRENT CHORD", 9.0f);
    label(currentValue, "—", 38.0f);
    label(chordInfo, "MODE  —     KEY  —     PATTERN  —     TEMPO  —", 10.0f);
    for (auto* l : { &title, &status, &currentCaption, &currentValue, &chordInfo }) addAndMakeVisible(*l);

    mode.addItem("MAJOR",1); mode.addItem("MINOR",2); mode.addItem("CUSTOM",3);
    const char* keys[] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
    for (int i=0;i<12;++i) key.addItem(keys[i],i+1);
    progression.addItem("I – V – vi – IV",1);
    progression.addItem("I – vi – IV – V",2);
    progression.addItem("vi – IV – I – V",3);
    progression.addItem("I – IV – V – IV",4);
    inversion.addItem("ROOT POSITION",1); inversion.addItem("1ST INVERSION",2);
    inversion.addItem("2ND INVERSION",3); inversion.addItem("3RD INVERSION",4);
    duration.addItem("1 BAR",1); duration.addItem("1/2 BAR",2); duration.addItem("1/4 BAR",3); duration.addItem("1/8 BAR",4);
    rhythm.addItem("STRAIGHT",1); rhythm.addItem("PUNK DOWNSTROKES",2); rhythm.addItem("UP / DOWN",3); rhythm.addItem("ARPEGGIATED",4);
    execution.addItem("BLOCK CHORDS",1); execution.addItem("STRUM",2); execution.addItem("ARP / PICK",3);
    mode.setSelectedId(1); key.setSelectedId(1); progression.setSelectedId(1);
    inversion.setSelectedId(1); duration.setSelectedId(1); rhythm.setSelectedId(1); execution.setSelectedId(1);

    label(scaleLabel,"CHORD MODE",9); label(keyLabel,"KEY",9); label(progressionLabel,"PATTERN / PROGRESSION",9);
    label(inversionLabel,"INVERSION",9); label(durationLabel,"DURATION",9); label(rhythmLabel,"RHYTHM",9); label(executionLabel,"PLAY STYLE",9);
    for (auto* l : { &scaleLabel,&keyLabel,&progressionLabel,&inversionLabel,&durationLabel,&rhythmLabel,&executionLabel }) addAndMakeVisible(*l);
    for (auto* c : { &mode,&key,&progression,&inversion,&duration,&rhythm,&execution }) { styleCombo(*c); addAndMakeVisible(*c); }

    for (auto* t : { &enabledToggle,&syncToTempo,&smartTranspose })
    {
        t->setClickingTogglesState(true);
        t->setEnabled(false);
        t->setColour(juce::ToggleButton::textColourId,palette.token("text_primary"));
        t->setColour(juce::ToggleButton::tickColourId,palette.token("accent"));
        addAndMakeVisible(*t);
    }
    for (auto& b : chordPads) { styleButton(b); b.setEnabled(false); addAndMakeVisible(b); }
    styleButton(preview); styleButton(reset);
    preview.setEnabled(false); reset.setEnabled(false);
    preview.setTooltip("Pending: preview uses the same MIDI/audio route as AutoChords playback.");
    enabledToggle.setTooltip("Pending backend: MELODY chord generation and note lifecycle are not connected yet.");
    for (auto* c : { &mode,&key,&progression,&inversion,&duration,&rhythm,&execution })
        c->setTooltip("Pending AutoChords backend integration.");
    addAndMakeVisible(preview); addAndMakeVisible(reset);
    setPalette(p);
}

void PPGUAutoChordsPanel::label(juce::Label& l,const juce::String& text,float size,bool bold)
{
    l.setText(text,juce::dontSendNotification);
    l.setFont(juce::Font(size,bold?juce::Font::bold:juce::Font::plain));
    l.setJustificationType(juce::Justification::centredLeft);
}
void PPGUAutoChordsPanel::styleCombo(juce::ComboBox& c)
{
    c.setColour(juce::ComboBox::backgroundColourId,palette.token("control_bg"));
    c.setColour(juce::ComboBox::textColourId,palette.token("text_primary"));
    c.setColour(juce::ComboBox::outlineColourId,palette.token("control_border"));
    c.setColour(juce::ComboBox::arrowColourId,palette.token("accent"));
}
void PPGUAutoChordsPanel::styleButton(juce::TextButton& b)
{
    b.setColour(juce::TextButton::buttonColourId,palette.token("control_bg"));
    b.setColour(juce::TextButton::buttonOnColourId,palette.token("accent"));
    b.setColour(juce::TextButton::textColourOffId,palette.token("text_primary"));
    b.setColour(juce::TextButton::textColourOnId,palette.token("window_bg"));
}
void PPGUAutoChordsPanel::setPalette(const PPGUThemeManager::Palette& p)
{
    palette=p;
    title.setColour(juce::Label::textColourId,palette.token("text_primary"));
    status.setColour(juce::Label::textColourId,palette.token("warning"));
    currentCaption.setColour(juce::Label::textColourId,palette.token("accent"));
    currentValue.setColour(juce::Label::textColourId,palette.token("text_primary"));
    chordInfo.setColour(juce::Label::textColourId,palette.token("text_secondary"));
    for (auto* l : { &scaleLabel,&keyLabel,&progressionLabel,&inversionLabel,&durationLabel,&rhythmLabel,&executionLabel })
        l->setColour(juce::Label::textColourId,palette.token("accent"));
    for (auto* c : { &mode,&key,&progression,&inversion,&duration,&rhythm,&execution }) styleCombo(*c);
    for (auto* b : { &preview,&reset }) styleButton(*b);
    for (auto& b : chordPads) styleButton(b);
    for (auto* t : { &enabledToggle,&syncToTempo,&smartTranspose })
    {
        t->setColour(juce::ToggleButton::textColourId,palette.token("text_primary"));
        t->setColour(juce::ToggleButton::tickColourId,palette.token("accent"));
    }
    repaint();
}
void PPGUAutoChordsPanel::paint(juce::Graphics& g)
{
    auto b=getLocalBounds().toFloat();
    g.fillAll(palette.token("window_bg"));
    auto card=b.reduced(2.0f);
    g.setColour(palette.token("panel")); g.fillRoundedRectangle(card,10.0f);
    g.setColour(palette.token("accent_glow").withAlpha(0.30f)); g.drawRoundedRectangle(card.reduced(1.0f),10.0f,2.0f);
    g.setColour(palette.token("control_border")); g.drawRoundedRectangle(card.reduced(3.0f),8.0f,1.0f);

    auto area=getLocalBounds().reduced(12);
    auto header=area.removeFromTop(38);
    g.setColour(palette.token("panel_elevated")); g.fillRoundedRectangle(header.toFloat(),7.0f);
    const bool expanded=getHeight()>=500;
    auto section=[&g,this](juce::Rectangle<int> r,const juce::String& name)
    {
        if(r.isEmpty()) return;
        g.setColour(palette.token("panel_elevated")); g.fillRoundedRectangle(r.toFloat(),7.0f);
        g.setColour(palette.token("control_border")); g.drawRoundedRectangle(r.toFloat(),7.0f,1.0f);
        g.setColour(palette.token("accent_glow").withAlpha(0.25f));
        g.fillRoundedRectangle(r.getX()+10.0f,r.getY()+1.0f,38.0f,2.0f,1.0f);
        g.setColour(palette.token("accent")); g.setFont(juce::Font(9.0f,juce::Font::bold));
        g.drawText(name,r.reduced(10,5).removeFromTop(14),juce::Justification::left);
    };
    area.removeFromTop(5);
    section(area.removeFromTop(expanded?76:62),"HARMONY CONFIGURATION");
    area.removeFromTop(7);
    section(area.removeFromTop(expanded?82:58),"LIVE CHORD MONITOR");
    area.removeFromTop(7);
    section(area.removeFromTop(expanded?78:54),"CHORD PROGRESSION  /  LOOP");
    area.removeFromTop(7);
    section(area.removeFromTop(expanded?104:42),expanded?"ADVANCED PERFORMANCE":"PERFORMANCE OPTIONS");
}
void PPGUAutoChordsPanel::resized()
{
    auto a=getLocalBounds().reduced(12);
    auto h=a.removeFromTop(38).reduced(8,3);
    title.setBounds(h.removeFromLeft(h.getWidth()-184));
    status.setBounds(h.removeFromRight(180));
    a.removeFromTop(5);
    const bool expanded=getHeight()>=500;
    auto config=a.removeFromTop(expanded?76:62).reduced(10,20);
    const int gap=8;
    auto c=config.removeFromLeft((config.getWidth()-2*gap)/3);
    scaleLabel.setBounds(c.removeFromTop(14)); mode.setBounds(c.removeFromTop(26));
    config.removeFromLeft(gap);
    c=config.removeFromLeft((config.getWidth()-gap)/2);
    keyLabel.setBounds(c.removeFromTop(14)); key.setBounds(c.removeFromTop(26));
    config.removeFromLeft(gap);
    progressionLabel.setBounds(config.removeFromTop(14)); progression.setBounds(config.removeFromTop(26));

    a.removeFromTop(7);
    auto live=a.removeFromTop(expanded?82:58).reduced(12,18);
    auto chord=live.removeFromLeft(expanded?150:92);
    currentCaption.setBounds(chord.removeFromTop(14)); currentValue.setBounds(chord);
    chordInfo.setBounds(live);

    a.removeFromTop(7);
    auto pads=a.removeFromTop(expanded?78:54).reduced(10,18);
    const int pgap=6, pw=(pads.getWidth()-3*pgap)/4;
    for(size_t i=0;i<chordPads.size();++i)
    {
        chordPads[i].setBounds(pads.removeFromLeft(pw));
        if(i+1<chordPads.size()) pads.removeFromLeft(pgap);
    }

    a.removeFromTop(7);
    auto options=a.removeFromTop(expanded?104:42).reduced(10,expanded?18:4);
    if(expanded)
    {
        auto toggles=options.removeFromTop(24);
        syncToTempo.setBounds(toggles.removeFromLeft(toggles.getWidth()/2));
        smartTranspose.setBounds(toggles);
        auto row=options.removeFromTop(42);
        auto col=row.removeFromLeft(row.getWidth()/4);
        inversionLabel.setBounds(col.removeFromTop(13)); inversion.setBounds(col);
        col=row.removeFromLeft(row.getWidth()/3);
        durationLabel.setBounds(col.removeFromTop(13)); duration.setBounds(col);
        col=row.removeFromLeft(row.getWidth()/2);
        rhythmLabel.setBounds(col.removeFromTop(13)); rhythm.setBounds(col);
        executionLabel.setBounds(row.removeFromTop(13)); execution.setBounds(row);
    }
    else
    {
        syncToTempo.setBounds(options.removeFromLeft(options.getWidth()/2));
        smartTranspose.setBounds(options);
    }
    auto footer=getLocalBounds().removeFromBottom(28).reduced(12,1);
    preview.setBounds(footer.removeFromRight(126));
    reset.setBounds(footer.removeFromRight(66));
    enabledToggle.setBounds(getLocalBounds().reduced(12).removeFromTop(38).removeFromRight(164).reduced(0,3));
}
