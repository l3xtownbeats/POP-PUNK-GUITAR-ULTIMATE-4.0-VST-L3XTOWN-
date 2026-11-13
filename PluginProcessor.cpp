#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace ppgu {

static juce::AudioParameterFloat* rotary(juce::AudioProcessorValueTreeState::ParameterLayout& l,
        const char* id, const char* name, float lo, float hi, float def, const char* suffix = "")
{
    auto p = std::make_unique<juce::AudioParameterFloat>(id, name,
        juce::NormalisableRange<float>(lo, hi), def, juce::String(), juce::AudioProcessorParameter::defaultCategory,
        suffix == nullptr ? juce::String() : juce::String(suffix));
    auto* raw = p.get(); l.add(std::move(p)); return raw;
}
static juce::AudioParameterBool* toggle(juce::AudioProcessorValueTreeState::ParameterLayout& l,
        const char* id, const char* name, bool def)
{
    auto p = std::make_unique<juce::AudioParameterBool>(id, name, def);
    auto* raw = p.get(); l.add(std::move(p)); return raw;
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::makeLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout l;
    rotary(l, "ampGain", "AMP Gain", 0.0f, 10.0f, 5.0f);
    rotary(l, "eqLow",  "EQ Low",  -12.0f, 12.0f, 0.0f, " dB");
    rotary(l, "eqMid",  "EQ Mid",  -12.0f, 12.0f, 0.0f, " dB");
    rotary(l, "eqHigh", "EQ High", -12.0f, 12.0f, 0.0f, " dB");
    rotary(l, "velocity", "Velocity", 0.0f, 2.0f, 1.0f);
    rotary(l, "dynamics", "Dynamics", 0.0f, 2.0f, 1.0f);
    rotary(l, "swing",    "Swing", 0.0f, 1.0f, 0.0f);
    rotary(l, "humanize", "Humanize", 0.0f, 1.0f, 0.15f);
    rotary(l, "accent",   "Accent", 0.0f, 2.0f, 1.0f);
    toggle(l, "fxDistortion", "Distortion", false);
    toggle(l, "fxChorus", "Chorus", false);
    toggle(l, "fxDelay",  "Delay", false);
    toggle(l, "fxReverb", "Reverb", false);
    toggle(l, "fxCompressor", "Compressor", false);
    toggle(l, "fxLimiter", "Limiter", false);
    toggle(l, "fxCabSim", "Cab Sim", false);
    toggle(l, "fxFlangerPower", "Flanger Power", false);
    juce::StringArray modes { "MELODY", "PATTERN", "PUNK GROUP" };
    l.add(std::make_unique<juce::AudioParameterChoice>("playMode", "Play Mode", modes, 0));
    rotary(l, "patternSpeed", "Pattern Speed", 0.25f, 4.0f, 1.0f, "x");
    rotary(l, "masterVolume", "Master Volume", 0.0f, 1.0f, 0.8f);
    return l;
}

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PPGU_PARAMS", makeLayout())
{
    engine.bindPool(&pool);
    scheduler.setPattern(factoryPunkPattern());
    streaming.start();
}

PlayMode PluginProcessor::currentMode() const {
    auto* p = apvts.getRawParameterValue("playMode");
    int idx = p ? (int)p->load() : 0;
    return idx == 1 ? PlayMode::PATTERN : (idx == 2 ? PlayMode::PUNK_GROUP : PlayMode::MELODY);
}

void PluginProcessor::applyFxParams() {
    FxParams fp;
    auto b = [&](const char* id) { auto* p = apvts.getRawParameterValue(id); return p && p->load() > 0.5f; };
    auto f = [&](const char* id) { auto* p = apvts.getRawParameterValue(id); return p ? p->load() : 0.0f; };
    fp.distortion = b("fxDistortion"); fp.chorus = b("fxChorus");
    fp.delay = b("fxDelay"); fp.reverb = b("fxReverb");
    fp.compressor = b("fxCompressor"); fp.limiter = b("fxLimiter");
    fp.cabSim = b("fxCabSim"); fp.flanger = b("fxFlangerPower");
    fp.eqLowDb = f("eqLow"); fp.eqMidDb = f("eqMid"); fp.eqHighDb = f("eqHigh");
    fx.setParams(fp);
}

void PluginProcessor::handleMidi(const juce::MidiBuffer& midi) {
    keyboardState.processNextMidiBuffer(nullptr, 0, midi, true);
    auto mode = currentMode();
    for (const auto meta : midi) {
        const auto m = meta.getMessage();
        if (m.isNoteOn()) {
            if (mode == PlayMode::MELODY) engine.noteOn(m.getNoteNumber(), m.getVelocity() / 127.0f, m.getChannel());
            else {
                auto steps = (mode == PlayMode::PATTERN) ? factoryPunkPattern() : factoryPunkGroupStabs();
                for (auto& st : steps) if (st.hit) engine.noteOn(st.key, st.velocity, m.getChannel());
            }
        } else if (m.isNoteOff()) {
            engine.noteOff(m.getNoteNumber(), m.getChannel());
        }
    }
}

void PluginProcessor::prepareToPlay(double sr, int) { engine.bindPool(&pool); }

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals;
    buf.clear();
    applyFxParams();
    handleMidi(midi);
    const int n = buf.getNumSamples();
    auto* l = buf.getWritePointer(0);
    auto* r = buf.getNumChannels() > 1 ? buf.getWritePointer(1) : l;
    engine.process(l, r, n, getSampleRate());
    fx.process(l, r, n);
    auto* mv = apvts.getRawParameterValue("masterVolume");
    float g = mv ? mv->load() : 0.8f;
    for (int i = 0; i < n; ++i) { l[i] *= g; r[i] *= g; }
}

juce::AudioProcessorEditor* PluginProcessor::createEditor() { return new PluginEditor(*this); }

void PluginProcessor::getStateInformation(juce::MemoryBlock& mb) {
    if (auto xml = apvts.copyState().createXml()) copyXmlToBinary(*xml, mb);
}
void PluginProcessor::setStateInformation(const void* d, int s) {
    if (auto xml = getXmlFromBinary(d, s))
        if (xml->hasTagName(apvts.state.getType())) apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

} // namespace ppgu

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new ppgu::PluginProcessor(); }