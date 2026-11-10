#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Performance/ChordTransitionTimeline.h"
#include "Performance/AutoChordScale.h"
#include <cmath>
#include <algorithm>
#include <tuple>

namespace
{
    constexpr int panicFadeMs = 30;

    std::unique_ptr<juce::RangedAudioParameter> floatParameter(
        const char* id, const char* name, float minimum, float maximum,
        float defaultValue, float step = 0.001f)
    {
        return std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { id, 1 }, name,
            juce::NormalisableRange<float>(minimum, maximum, step), defaultValue);
    }

    std::unique_ptr<juce::RangedAudioParameter> boolParameter(
        const char* id, const char* name, bool defaultValue)
    {
        return std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID { id, 1 }, name, defaultValue);
    }
}

PopPunkGuitarUltimateAudioProcessor::~PopPunkGuitarUltimateAudioProcessor()
{
    // Invalidate queued worker and message-thread completions before waiting
    // for any active bank scan/decode to finish.
    sampleLoadGeneration->fetch_add(1, std::memory_order_acq_rel);
    sampleLoadPool.removeAllJobs(true, -1);
}

PopPunkGuitarUltimateAudioProcessor::PopPunkGuitarUltimateAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PPGU_STATE", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
PopPunkGuitarUltimateAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout p;

    // Global / performance controls.
    p.add(floatParameter("master", "Master", 0.0f, 1.0f, 0.8f));
    p.add(floatParameter("voices", "Voices", 1.0f, 64.0f, 32.0f, 1.0f));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "powerProfile", 1 }, "Power Profile",
        juce::StringArray { "GOOD", "NORMAL", "NIGHT" }, 1));
    p.add(floatParameter("velocity", "Velocity Response", 0.0f, 2.0f, 1.0f));
    p.add(floatParameter("dynamics", "Velocity Dynamics", 0.0f, 1.0f, 0.5f));
    p.add(floatParameter("palmMuteThreshold", "Palm Mute Threshold", 1.0f, 127.0f, 55.0f, 1.0f));
    p.add(floatParameter("tuning", "Tuning", -12.0f, 12.0f, 0.0f, 0.01f));
    p.add(floatParameter("pan", "Pan", -1.0f, 1.0f, 0.0f));
    p.add(floatParameter("output", "Output", -24.0f, 12.0f, 0.0f, 0.01f));
    p.add( std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "playMode", 1 }, "Play Mode",
        juce::StringArray { "Melody", "Pattern" }, 0));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "articulation", 1 }, "Articulation",
        juce::StringArray { "Open", "Palm Mute", "Flageolet", "Tremolo" }, 0));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "pickStyle", 1 }, "Pick Style",
        juce::StringArray { "Pick", "Fingers" }, 0));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "pitchBendTarget", 1 }, "Pitch Bend Target",
        juce::StringArray { "All", "Lowest", "Highest", "Softest", "Loudest", "Latest" }, 0));
    p.add(floatParameter("vibrato", "Vibrato", 0.0f, 1.0f, 0.0f));
    p.add(floatParameter("slideTime", "Slide Time", 0.01f, 1.0f, 0.25f));
    p.add(boolParameter("legato", "Legato", true));
    p.add(boolParameter("hammerPull", "Hammer On Pull Off", true));
    p.add(boolParameter("autoChords", "AutoChords", true));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "scaleRoot", 1 }, "Scale Root",
        juce::StringArray { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 0));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "scaleMode", 1 }, "Scale Mode",
        juce::StringArray { "Chromatic", "Major", "Minor" }, 1));

    // Melody performance and articulation controls from v33 sections 8 and 13.
    p.add(boolParameter("monoMode", "Monophonic Mode", true));
    p.add(boolParameter("fretNoiseOnChordChange", "Fret Noise on Chord Change", true));
    p.add(boolParameter("slideModifier", "Slide Modifier (B1)", false));
    p.add(floatParameter("slideVelocitySensitivity", "Slide Velocity Sensitivity", 0.0f, 1.0f, 0.5f));
    p.add(floatParameter("modWheelVibrato", "Mod Wheel Vibrato Depth", 0.0f, 1.0f, 0.0f));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "melodyPatternSlot", 1 }, "Melody Pattern Slot",
        juce::StringArray { "Off", "1", "2", "3", "4" }, 0));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "endingLength", 1 }, "Ending Length",
        juce::StringArray { "Short", "Medium", "Long" }, 1));
    p.add(boolParameter("palmHitOnEnding", "Palm Hit Ending", false));
    p.add(boolParameter("stringSlapOnEnding", "String Slap Ending", false));

    // Pattern transport, playback feel, and chord interpretation (v33 section 9).
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "patternSlot", 1 }, "Pattern Slot",
        juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8" }, 0));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "patternLength", 1 }, "Pattern Length",
        juce::StringArray { "1 Bar", "2 Bars", "4 Bars" }, 0));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "syncMode", 1 }, "Sync Mode",
        juce::StringArray { "Host", "Host Start", "Internal" }, 0));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "swingBase", 1 }, "Swing Base",
        juce::StringArray { "8th", "16th" }, 0));
    p.add(floatParameter("globalPpqOffset", "Global PPQ Offset (CC111)", -16.0f, 16.0f, 0.0f, 0.001f));
    p.add(boolParameter("chordSlashBass", "Respect Slash Chord Bass", true));
    p.add(boolParameter("singleMidiNoteAsChordRoot", "Single MIDI Note as Chord Root", true));
    p.add(boolParameter("preserveHumanTiming", "Preserve Recorded Human Timing", true));

    // Editable Style DNA parameters (v33 section 7.4).
    p.add(floatParameter("dnaChugDensity", "DNA Chug Density", 0.0f, 1.0f, 0.5f));
    p.add(floatParameter("dnaHammerPullProbability", "DNA Hammer/Pull Probability", 0.0f, 1.0f, 0.25f));
    p.add(floatParameter("dnaStringSpread", "DNA String Spread", 0.0f, 1.0f, 0.5f));
    p.add(floatParameter("dnaBeatAccent", "DNA Beat Accent", 0.0f, 1.0f, 0.7f));
    p.add(floatParameter("dnaFxTaste", "DNA FX Taste", 0.0f, 1.0f, 0.5f));

    // Seven FX slots can be reordered without changing their parameter IDs.
    const juce::StringArray fxNames { "Compressor", "EQ", "Distortion", "Chorus", "Delay", "Reverb", "Cabinet IR" };
    for (int slot = 0; slot < 7; ++slot)
        p.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID { "fxSlot" + juce::String(slot + 1), 1 },
            "FX Slot " + juce::String(slot + 1), fxNames, slot));

    // Expanded core controls for each of the seven fixed effects.
    p.add(floatParameter("compressorThreshold", "Compressor Threshold", -60.0f, 0.0f, -18.0f, 0.1f));
    p.add(floatParameter("compressorRatio", "Compressor Ratio", 1.0f, 20.0f, 3.0f, 0.1f));
    p.add(floatParameter("compressorAttack", "Compressor Attack", 0.1f, 100.0f, 10.0f, 0.1f));
    p.add(floatParameter("compressorRelease", "Compressor Release", 5.0f, 1000.0f, 120.0f, 0.1f));
    p.add(floatParameter("eqLowFreq", "EQ Low Frequency", 20.0f, 500.0f, 100.0f, 1.0f));
    p.add(floatParameter("eqMidFreq", "EQ Mid Frequency", 100.0f, 8000.0f, 1000.0f, 1.0f));
    p.add(floatParameter("eqMidQ", "EQ Mid Q", 0.1f, 10.0f, 1.0f, 0.01f));
    p.add(floatParameter("eqHighFreq", "EQ High Frequency", 2000.0f, 20000.0f, 8000.0f, 1.0f));
    p.add(floatParameter("distortionMix", "Distortion Mix", 0.0f, 1.0f, 1.0f));
    p.add(floatParameter("chorusRate", "Chorus Rate", 0.05f, 10.0f, 0.8f, 0.01f));
    p.add(floatParameter("chorusDepth", "Chorus Depth", 0.0f, 1.0f, 0.35f));
    p.add(floatParameter("delayFeedback", "Delay Feedback", 0.0f, 0.95f, 0.25f, 0.001f));
    p.add(floatParameter("reverbRoomSize", "Reverb Room Size", 0.0f, 1.0f, 0.45f));
    p.add(floatParameter("reverbDamping", "Reverb Damping", 0.0f, 1.0f, 0.5f));
    p.add(floatParameter("cabinetMix", "Cabinet IR Mix", 0.0f, 1.0f, 1.0f));
    p.add(floatParameter("cabinetLowCut", "Cabinet Low Cut", 20.0f, 500.0f, 80.0f, 1.0f));
    p.add(floatParameter("cabinetHighCut", "Cabinet High Cut", 2000.0f, 20000.0f, 12000.0f, 1.0f));

    // Pattern playback.
    p.add(floatParameter("bpm", "Fallback BPM", 20.0f, 300.0f, 120.0f, 0.1f));
    p.add(floatParameter("swing", "Swing", 0.0f, 1.0f, 0.0f));
    p.add(floatParameter("humanize", "Humanize", 0.0f, 1.0f, 0.0f));
    p.add(floatParameter("accent", "Accent", 0.0f, 1.0f, 0.75f));
    p.add(boolParameter("startOnFirstChord", "Start on First Chord", true));
    p.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "patternQuantize", 1 }, "Pattern Quantize",
        juce::StringArray { "1/16", "1/32" }, 0));

    // Amp / tone and fixed FX rack.
    p.add(floatParameter("ampGain", "Amp Gain", 0.0f, 10.0f, 5.0f));
    p.add(floatParameter("tone", "Tone", 0.0f, 10.0f, 7.0f));
    p.add(boolParameter("compressorOn", "Compressor On", true));
    p.add(floatParameter("compressorAmount", "Compressor Amount", 0.0f, 1.0f, 0.25f));
    p.add(boolParameter("eqOn", "EQ On", true));
    p.add(floatParameter("eqLow", "EQ Low", -12.0f, 12.0f, 0.0f, 0.1f));
    p.add(floatParameter("eqMid", "EQ Mid", -12.0f, 12.0f, 0.0f, 0.1f));
    p.add(floatParameter("eqHigh", "EQ High", -12.0f, 12.0f, 0.0f, 0.1f));
    p.add(boolParameter("distortionOn", "Distortion On", true));
    p.add(floatParameter("distortionDrive", "Distortion Drive", 0.0f, 1.0f, 0.55f));
    p.add(floatParameter("distortionTone", "Distortion Tone", 0.0f, 1.0f, 0.65f));
    p.add(boolParameter("chorusOn", "Chorus On", false));
    p.add(boolParameter("flangerOn", "Flanger On", false));
    p.add(floatParameter("flangerMix", "Flanger Mix", 0.0f, 1.0f, 0.2f));
    p.add(floatParameter("flangerRate", "Flanger Rate", 0.05f, 5.0f, 0.25f, 0.01f));
    p.add(floatParameter("flangerDepth", "Flanger Depth", 0.0f, 1.0f, 0.5f));
    p.add(floatParameter("flangerFeedback", "Flanger Feedback", -0.7f, 0.7f, 0.15f, 0.01f));
    p.add(floatParameter("chorusMix", "Chorus Mix", 0.0f, 1.0f, 0.15f));
    p.add(boolParameter("delayOn", "Delay On", false));
    p.add(floatParameter("delayTime", "Delay Time", 1.0f, 2000.0f, 320.0f, 1.0f));
    p.add(floatParameter("delayMix", "Delay Mix", 0.0f, 1.0f, 0.15f));
    p.add(boolParameter("reverbOn", "Reverb On", true));
    p.add(floatParameter("reverbMix", "Reverb Mix", 0.0f, 1.0f, 0.12f));
    p.add(boolParameter("cabinetOn", "Cabinet IR On", true));
    p.add(boolParameter("doubleTrackOn", "Double Track On", false));
    p.add(floatParameter("doubleTrackMix", "Double Track Mix", 0.0f, 1.0f, 0.5f));

    return p;
}

void PopPunkGuitarUltimateAudioProcessor::prepareToPlay(double sampleRate, int expectedBlockSize)
{
    currentSampleRate = juce::jlimit(8000.0, 384000.0, sampleRate);
    maximumBlockSize = juce::jmax(1, expectedBlockSize);
    sampler.prepare(currentSampleRate);
    patternScheduler.reset();
    patternNoteTracker.clear();
    patternCycleStarted = false;
    previousPatternChordCount = 0;
    previousPatternChordNotes.fill(-1);
    previousPatternChordChannel = 1;
    previousPatternSlot = -1;
    previousPatternLengthChoice = -1;
    previousPlayMode = -1;

    const auto delayCapacity = static_cast<size_t>(std::ceil(currentSampleRate * 2.1)) + 4;
    const auto chorusCapacity = static_cast<size_t>(std::ceil(currentSampleRate * 0.04)) + 4;
    const auto flangerCapacity = static_cast<size_t>(std::ceil(currentSampleRate * 0.02)) + 4;
    const auto doubleTrackCapacity = static_cast<size_t>(std::ceil(currentSampleRate * 0.04)) + 4;
    for (int channel = 0; channel < 2; ++channel)
    {
        delayBuffers[static_cast<size_t>(channel)].assign(delayCapacity, 0.0f);
        chorusBuffers[static_cast<size_t>(channel)].assign(chorusCapacity, 0.0f);
        flangerBuffers[static_cast<size_t>(channel)].assign(flangerCapacity, 0.0f);
        doubleTrackBuffers[static_cast<size_t>(channel)].assign(doubleTrackCapacity, 0.0f);
        reverbDryBuffers[static_cast<size_t>(channel)].assign(static_cast<size_t>(maximumBlockSize), 0.0f);
    }
    delayWritePosition = 0;
    chorusWritePosition = 0;
    flangerWritePosition = 0;
    doubleTrackWritePosition = 0;
    flangerPhase = 0.0;
    chorusPhase = 0.0;
    eqLowState.fill(0.0f);
    eqHighState.fill(0.0f);
    compressorEnvelope.fill(0.0f);
    for (auto& history : cabinetHistory) history.fill(0.0f);
    cabinetWritePosition = 0;
    // Compact original cabinet-style impulse: damped resonant response, generated once off the audio thread.
    cabinetImpulse.fill(0.0f);
    for (size_t tap = 0; tap < cabinetImpulse.size(); ++tap)
    {
        const float t = static_cast<float>(tap);
        cabinetImpulse[tap] = std::exp(-t / 11.0f) * (0.72f * std::cos(0.31f * t) + 0.28f * std::cos(0.67f * t));
    }
    float impulseEnergy = 0.0f;
    for (const auto coefficient : cabinetImpulse) impulseEnergy += std::abs(coefficient);
    if (impulseEnergy > 0.0f) for (auto& coefficient : cabinetImpulse) coefficient /= impulseEnergy;

    reverb.setSampleRate(currentSampleRate);
    reverb.reset();
}

void PopPunkGuitarUltimateAudioProcessor::releaseResources()
{
    sampler.reset();
    reverb.reset();
    for (auto& line : delayBuffers)
        std::fill(line.begin(), line.end(), 0.0f);
    for (auto& line : chorusBuffers)
        std::fill(line.begin(), line.end(), 0.0f);
    for (auto& line : flangerBuffers)
        std::fill(line.begin(), line.end(), 0.0f);
    for (auto& line : doubleTrackBuffers)
        std::fill(line.begin(), line.end(), 0.0f);
    delayWritePosition = 0;
    chorusWritePosition = 0;
    flangerWritePosition = 0;
    doubleTrackWritePosition = 0;
    flangerPhase = 0.0;
    chorusPhase = 0.0;
    eqLowState.fill(0.0f);
    eqHighState.fill(0.0f);
    compressorEnvelope.fill(0.0f);
    for (auto& history : cabinetHistory) history.fill(0.0f);
    cabinetWritePosition = 0;
}

bool PopPunkGuitarUltimateAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PopPunkGuitarUltimateAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const int fadeLength = juce::jmax(1, static_cast<int>(currentSampleRate * panicFadeMs / 1000.0));
    const int samples = buffer.getNumSamples();
    keyboardState.processNextMidiBuffer(midi, 0, samples, true);
    const int requestedVoices = juce::jlimit(1, ppgu::sampler::SamplerEngine::maximumVoices,
        static_cast<int>(parameters.getRawParameterValue("voices")->load(std::memory_order_relaxed)));
    const int powerProfile = juce::jlimit(0, 2, static_cast<int>(
        parameters.getRawParameterValue("powerProfile")->load(std::memory_order_relaxed)));
    // GOOD prioritizes maximum polyphony; NORMAL balances load and headroom;
    // NIGHT caps simultaneous voices to reduce sustained CPU demand on laptops.
    const int profileVoiceLimit = powerProfile == 0 ? 64 : (powerProfile == 1 ? 32 : 12);
    const int maximumVoices = juce::jmin(requestedVoices, profileVoiceLimit);
    const float velocityScale = juce::jlimit(0.0f, 2.0f,
        parameters.getRawParameterValue("velocity")->load(std::memory_order_relaxed));
    const int playMode = juce::jlimit(0, 2,
        static_cast<int>(parameters.getRawParameterValue("playMode")->load(std::memory_order_relaxed)));
    const bool autoChordsEnabled = parameters.getRawParameterValue("autoChords")->load(std::memory_order_relaxed) > 0.5f;
    const int selectedScaleRoot = juce::jlimit(0, 11, static_cast<int>(parameters.getRawParameterValue("scaleRoot")->load(std::memory_order_relaxed)));
    const int selectedScaleMode = juce::jlimit(0, 2, static_cast<int>(parameters.getRawParameterValue("scaleMode")->load(std::memory_order_relaxed)));
    const auto mapPerformanceNote = [&](int note) noexcept
    {
        return autoChordsEnabled
            ? ppgu::performance::mapNoteToScale(note, selectedScaleRoot, selectedScaleMode)
            : note;
    };

    // Read one immutable host-position snapshot per block. Pattern timing is
    // derived from PPQ/sample-rate; the scheduler never calls the host from a
    // worker thread and never allocates or locks.
    ppgu::performance::TransportSnapshot hostTransport;
    if (auto* hostPlayHead = getPlayHead())
    {
        if (const auto position = hostPlayHead->getPosition())
        {
            const auto bpm = position->getBpm();
            const auto ppq = position->getPpqPosition();
            const bool playing = position->getIsPlaying();
            if (bpm.hasValue() && ppq.hasValue())
            {
                hostTransport.hostValid = true;
                hostTransport.bpm = *bpm;
                hostTransport.ppq = *ppq;
                hostTransport.playing = playing;
            }
        }
    }
    const int syncMode = juce::jlimit(0, 2,
        static_cast<int>(parameters.getRawParameterValue("syncMode")->load(std::memory_order_relaxed)));
    const bool useInternalClock = syncMode == 2;
    const double fallbackBpm = parameters.getRawParameterValue("bpm")->load(std::memory_order_relaxed);
    patternTransportClock = patternScheduler.beginBlock(
        hostTransport, useInternalClock, fallbackBpm, currentSampleRate, samples);

    if (playMode != previousPlayMode)
    {
        sampler.allNotesOff();
        patternScheduler.reset(patternTransportClock.startPpq);
        patternNoteTracker.clear();
        for (auto& channel : melodyMappedNotes) channel.fill(-1);
        patternCycleStarted = false;
        previousPatternChordCount = 0;
        previousPatternChordNotes.fill(-1);
        previousPatternChordChannel = 1;
        previousPlayMode = playMode;
    }

    // Transport stop/seek owns only Pattern-generated voices. In Melody
    // mode MIDI notes are played live and must not be cut merely because the
    // DAW transport stops or seeks while a performer is holding a key.
    if (playMode == 1
        && (patternTransportClock.transportStopped || patternTransportClock.transportDiscontinuity))
    {
        sampler.allNotesOff();
        patternScheduler.clearPendingNoteOffs();
        patternCycleStarted = false;
        previousPatternChordCount = 0;
        previousPatternChordNotes.fill(-1);
        previousPatternChordChannel = 1;
        if (patternTransportClock.transportStopped)
            patternNoteTracker.clear();
    }

    const double globalTuningSemitones = juce::jlimit(-12.0, 12.0,
        static_cast<double>(parameters.getRawParameterValue("tuning")->load(std::memory_order_relaxed)));

    auto renderSegment = [&](int start, int length)
    {
        if (length > 0)
            sampler.render(buffer, start, length);
    };

    if (playMode == 0)
    {
        // MELODY: one incoming MIDI key plays one note. AutoChords optionally snaps
        // that pitch to the selected key/scale; when disabled Piano Roll pitches pass through.
        int cursor = 0;
        for (const auto metadata : midi)
        {
            const auto message = metadata.getMessage();
            const int eventSample = juce::jlimit(cursor, samples, metadata.samplePosition);
            renderSegment(cursor, eventSample - cursor);
            cursor = eventSample;

            if (message.isAllNotesOff() || message.isAllSoundOff())
            {
                for (auto& mapped : melodyMappedNotes) mapped.fill(-1);
                panicRequested.store(true, std::memory_order_release);
                sampler.panic(fadeLength);
            }
            else if (message.isNoteOn() && velocityScale > 0.0f)
            {
                const float normalizedVelocity = juce::jlimit<float>(
                    0.0f, 1.0f, static_cast<float>(message.getVelocity()) / 127.0f);
                const float dynamics = juce::jlimit(0.0f, 1.0f,
                    parameters.getRawParameterValue("dynamics")->load(std::memory_order_relaxed));
                const float shapedVelocity = std::pow(normalizedVelocity, 2.0f - 1.5f * dynamics);
                const int scaledVelocity = juce::jlimit(1, 127,
                    juce::roundToInt(shapedVelocity * 127.0f * velocityScale));
                const int mappedNote = mapPerformanceNote(message.getNoteNumber());
                melodyMappedNotes[static_cast<size_t>(message.getChannel() - 1)][static_cast<size_t>(message.getNoteNumber())] = mappedNote;
                const bool triggered = sampler.noteOn(mappedNote, scaledVelocity, maximumVoices,
                                                      message.getChannel(), globalTuningSemitones);
                juce::ignoreUnused(triggered);
            }
            else if (message.isNoteOff())
            {
                const auto channel = static_cast<size_t>(message.getChannel() - 1);
                const auto sourceNote = static_cast<size_t>(message.getNoteNumber());
                const int mappedNote = melodyMappedNotes[channel][sourceNote] >= 0
                    ? melodyMappedNotes[channel][sourceNote] : message.getNoteNumber();
                melodyMappedNotes[channel][sourceNote] = -1;
                sampler.noteOff(mappedNote, message.getChannel());
            }
        }
        renderSegment(cursor, samples - cursor);
    }
    else if (playMode == 1)
    {
        // PATTERN: process every MIDI chord transition at its sample position.
        // Each interval is scheduled independently, preserving every chord change.
        const int patternSlot = juce::jlimit(0, 7, static_cast<int>(parameters.getRawParameterValue("patternSlot")->load(std::memory_order_relaxed)));
        const int patternLengthChoice = juce::jlimit(0, 2, static_cast<int>(parameters.getRawParameterValue("patternLength")->load(std::memory_order_relaxed)));
        const double patternLengthBeats = patternLengthChoice == 0 ? 4.0 : (patternLengthChoice == 1 ? 8.0 : 16.0);
        const float swing = juce::jlimit(0.0f, 1.0f, parameters.getRawParameterValue("swing")->load(std::memory_order_relaxed));
        const float accent = juce::jlimit(0.0f, 1.0f, parameters.getRawParameterValue("accent")->load(std::memory_order_relaxed));
        const std::array<std::uint16_t, 8> rhythmMasks {{ 0x5555, 0x1111, 0x3333, 0xffff, 0x9999, 0x1111, 0x4444, 0xffff }};
        const bool arpeggio = patternSlot == 7;
        const int quantizeChoice = juce::jlimit(0, 1, static_cast<int>(parameters.getRawParameterValue("patternQuantize")->load(std::memory_order_relaxed)));
        const double quantizeGridBeats = quantizeChoice == 0 ? 0.25 : 0.125;
        const bool startOnFirstChord = parameters.getRawParameterValue("startOnFirstChord")->load(std::memory_order_relaxed) > 0.5f;
        const double ppqPerSample = patternTransportClock.bpm / (60.0 * currentSampleRate);

        auto readChord = [&]()
        {
            std::array<int, 6> notes {};
            notes.fill(-1);
            int count = 0;
            int channel = 1;
            for (int ch = 0; ch < 16 && count < static_cast<int>(notes.size()); ++ch)
                for (int note = 0; note < 128 && count < static_cast<int>(notes.size()); ++note)
                    if (patternNoteTracker.isHeld(ch, note))
                    {
                        notes[static_cast<size_t>(count++)] = note;
                        channel = ch + 1;
                    }
            return std::make_tuple(notes, count, channel);
        };

        auto chordDiffers = [&](const std::array<int, 6>& notes, int count, int channel)
        {
            return count != previousPatternChordCount || channel != previousPatternChordChannel
                || !std::equal(notes.begin(), notes.begin() + count, previousPatternChordNotes.begin());
        };

        auto restartForChord = [&](int sampleOffset, const std::array<int, 6>& notes, int count, int channel)
        {
            sampler.allNotesOff();
            patternScheduler.clearPendingNoteOffs();
            patternCycleStarted = count > 0;
            if (count > 0)
            {
                const double changePpq = patternTransportClock.startPpq + static_cast<double>(sampleOffset) * ppqPerSample;
                const bool firstChord = previousPatternChordCount == 0;
                patternCycleStartPpq = firstChord && startOnFirstChord ? changePpq
                    : (firstChord ? std::ceil(changePpq / 4.0 - 1.0e-9) * 4.0
                                  : ppgu::performance::PatternScheduler::quantizePpqForward(changePpq, quantizeGridBeats));
            }
            previousPatternChordCount = count;
            previousPatternChordNotes = notes;
            previousPatternChordChannel = channel;
        };

        const auto [initialNotes, initialCount, initialChannel] = readChord();
        const bool patternSettingsChanged =
            (previousPatternSlot >= 0 && patternSlot != previousPatternSlot)
            || (previousPatternLengthChoice >= 0 && patternLengthChoice != previousPatternLengthChoice);
        if (chordDiffers(initialNotes, initialCount, initialChannel))
            restartForChord(0, initialNotes, initialCount, initialChannel);
        else if (patternSettingsChanged && initialCount > 0)
            restartForChord(0, initialNotes, initialCount, initialChannel);
        previousPatternSlot = patternSlot;
        previousPatternLengthChoice = patternLengthChoice;

        auto renderPatternInterval = [&](int intervalStart, int intervalEnd)
        {
            if (intervalEnd <= intervalStart) return;
            const auto [notes, count, channel] = readChord();
            if (count == 0)
            {
                renderSegment(intervalStart, intervalEnd - intervalStart);
                return;
            }

            patternEvents.fill(ppgu::performance::PatternEvent { -1.0, 0.0, 60, 0, 1 });
            std::size_t patternEventCount = 0;
            const int bars = static_cast<int>(patternLengthBeats / 4.0);
            for (int bar = 0; bar < bars; ++bar)
                for (int step = 0; step < 16; ++step)
                {
                    if ((rhythmMasks[static_cast<size_t>(patternSlot)] & (1u << step)) == 0) continue;
                    const double swingOffset = (step % 4 == 2) ? 0.125 * swing : 0.0;
                    const double beat = static_cast<double>(bar * 4) + static_cast<double>(step) * 0.25 + swingOffset;
                    const int notesThisStep = arpeggio ? 1 : count;
                    for (int strum = 0; strum < notesThisStep && patternEventCount < patternEvents.size(); ++strum)
                    {
                        const int chordIndex = arpeggio ? ((bar * 16 + step) % count)
                            : ((step % 2 == 0) ? strum : count - 1 - strum);
                        const int velocity = juce::jlimit(1, 127, juce::roundToInt(78.0f + accent * 38.0f - static_cast<float>(strum) * 2.0f));
                        patternEvents[patternEventCount++] = { beat + static_cast<double>(strum) * 0.012, 0.20,
                            static_cast<std::uint8_t>(notes[static_cast<size_t>(chordIndex)]),
                            static_cast<std::uint8_t>(velocity), static_cast<std::uint8_t>(channel) };
                    }
                }

            auto intervalClock = patternTransportClock;
            intervalClock.startPpq = patternTransportClock.startPpq + static_cast<double>(intervalStart) * ppqPerSample;
            intervalClock.endPpq = patternTransportClock.startPpq + static_cast<double>(intervalEnd) * ppqPerSample;
            const int intervalSamples = intervalEnd - intervalStart;
            const auto scheduledCount = patternScheduler.schedule(patternEvents, patternLengthBeats, patternCycleStartPpq,
                intervalClock, currentSampleRate, intervalSamples, 0.0, 0, scheduledPatternEvents);

            int cursor = intervalStart;
            for (std::size_t i = 0; i < scheduledCount; ++i)
            {
                const auto& event = scheduledPatternEvents[i];
                const int eventSample = juce::jlimit(cursor, intervalEnd, intervalStart + event.sampleOffset);
                renderSegment(cursor, eventSample - cursor);
                cursor = eventSample;
                if (event.kind == ppgu::performance::EventKind::NoteOn)
                {
                    if (velocityScale <= 0.0f) continue;
                    const float normalizedVelocity = juce::jlimit<float>(0.0f, 1.0f, static_cast<float>(event.velocity) / 127.0f);
                    const float dynamics = juce::jlimit(0.0f, 1.0f, parameters.getRawParameterValue("dynamics")->load(std::memory_order_relaxed));
                    const float shapedVelocity = std::pow(normalizedVelocity, 2.0f - 1.5f * dynamics);
                    const int scaledVelocity = juce::jlimit(1, 127, juce::roundToInt(shapedVelocity * 127.0f * velocityScale));
                    const bool triggered = sampler.noteOn(event.note, scaledVelocity, maximumVoices, event.channel, globalTuningSemitones);
                    juce::ignoreUnused(triggered);
                }
                else sampler.noteOff(event.note, event.channel);
            }
            renderSegment(cursor, intervalEnd - cursor);
        };

        // Render each interval before applying its MIDI event at the exact sample.
        // Same-sample events retain order without inventing time between chord notes.
        ppgu::performance::processSampleAccurateMidiTimeline(
            samples, midi,
            [&](int intervalStart, int intervalEnd)
            {
                renderPatternInterval(intervalStart, intervalEnd);
            },
            [&](const auto& metadata, int eventSample)
            {
                const auto message = metadata.getMessage();
                const int channelIndex = juce::jlimit(1, 16, message.getChannel()) - 1;
                if (message.isAllNotesOff() || message.isAllSoundOff())
                {
                    patternNoteTracker.clear();
                    panicRequested.store(true, std::memory_order_release);
                    sampler.panic(fadeLength);
                    patternScheduler.clearPendingNoteOffs();
                    patternCycleStarted = false;
                    previousPatternChordCount = 0;
                    previousPatternChordNotes.fill(-1);
                    previousPatternChordChannel = 1;
                }
                else if (message.isNoteOn() || message.isNoteOff())
                {
                    const int sourceNote = message.getNoteNumber();
                    const int channel = channelIndex;
                    if (message.isNoteOn())
                    {
                        const int mappedNote = mapPerformanceNote(sourceNote);
                        // Repeated NoteOn for the same source key replaces its
                        // previous ownership instead of leaking a reference.
                        patternNoteTracker.noteOn(channel, sourceNote, mappedNote);
                    }
                    else
                    {
                        const int mappedNote = patternNoteTracker.noteOff(channel, sourceNote);
                        juce::ignoreUnused(mappedNote);
                    }
                    const auto [nextNotes, nextCount, nextChannel] = readChord();
                    if (chordDiffers(nextNotes, nextCount, nextChannel))
                        restartForChord(eventSample, nextNotes, nextCount, nextChannel);
                }
            });
        if (patternScheduler.consumePendingOverflow()) sampler.allNotesOff();
    }

    // AMP & TONE: low/mid/high split with persistent one-pole filter states.
    const float ampGainDb = (juce::jlimit(0.0f, 10.0f, parameters.getRawParameterValue("ampGain")->load(std::memory_order_relaxed)) - 5.0f) * 2.0f;
    const float ampGain = juce::Decibels::decibelsToGain(ampGainDb);
    const bool eqEnabled = parameters.getRawParameterValue("eqOn")->load(std::memory_order_relaxed) > 0.5f;
    const float lowGain = juce::Decibels::decibelsToGain(juce::jlimit(-12.0f, 12.0f, parameters.getRawParameterValue("eqLow")->load(std::memory_order_relaxed)));
    const float midGain = juce::Decibels::decibelsToGain(juce::jlimit(-12.0f, 12.0f, parameters.getRawParameterValue("eqMid")->load(std::memory_order_relaxed)));
    const float highGain = juce::Decibels::decibelsToGain(juce::jlimit(-12.0f, 12.0f, parameters.getRawParameterValue("eqHigh")->load(std::memory_order_relaxed)));
    const float lowAlpha = 1.0f - std::exp(-juce::MathConstants<float>::twoPi * 250.0f / static_cast<float>(currentSampleRate));
    const float highAlpha = 1.0f - std::exp(-juce::MathConstants<float>::twoPi * 2500.0f / static_cast<float>(currentSampleRate));
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* data = buffer.getWritePointer(channel);
        auto& lowState = eqLowState[static_cast<size_t>(channel)];
        auto& highState = eqHighState[static_cast<size_t>(channel)];
        for (int sample = 0; sample < samples; ++sample)
        {
            const float input = data[sample] * ampGain;
            lowState += lowAlpha * (input - lowState);
            highState += highAlpha * (input - highState);
            if (eqEnabled)
            {
                const float low = lowState;
                const float high = input - highState;
                const float mid = input - low - high;
                data[sample] = low * lowGain + mid * midGain + high * highGain;
            }
            else data[sample] = input;
        }
    }

    // Feed-forward compressor with per-channel envelope follower. Attack/release
    // coefficients are calculated once per block; no allocation or locking.
    if (parameters.getRawParameterValue("compressorOn")->load(std::memory_order_relaxed) > 0.5f)
    {
        const float thresholdDb = juce::jlimit(-60.0f, 0.0f, parameters.getRawParameterValue("compressorThreshold")->load(std::memory_order_relaxed));
        const float ratio = juce::jlimit(1.0f, 20.0f, parameters.getRawParameterValue("compressorRatio")->load(std::memory_order_relaxed));
        const float attackMs = juce::jlimit(0.1f, 100.0f, parameters.getRawParameterValue("compressorAttack")->load(std::memory_order_relaxed));
        const float releaseMs = juce::jlimit(5.0f, 1000.0f, parameters.getRawParameterValue("compressorRelease")->load(std::memory_order_relaxed));
        const float attackCoeff = std::exp(-1.0f / (0.001f * attackMs * static_cast<float>(currentSampleRate)));
        const float releaseCoeff = std::exp(-1.0f / (0.001f * releaseMs * static_cast<float>(currentSampleRate)));
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            auto& envelope = compressorEnvelope[static_cast<size_t>(channel)];
            for (int sample = 0; sample < samples; ++sample)
            {
                const float level = std::abs(data[sample]);
                const float coefficient = level > envelope ? attackCoeff : releaseCoeff;
                envelope = coefficient * envelope + (1.0f - coefficient) * level;
                const float levelDb = juce::Decibels::gainToDecibels(juce::jmax(envelope, 1.0e-8f));
                const float compressedDb = levelDb > thresholdDb ? thresholdDb + (levelDb - thresholdDb) / ratio : levelDb;
                const float gain = juce::Decibels::decibelsToGain(compressedDb - levelDb);
                data[sample] *= gain;
            }
        }
    }

    // Flanger uses a dedicated preallocated ring buffer; it can run alongside chorus and delay.
    if (parameters.getRawParameterValue("flangerOn")->load(std::memory_order_relaxed) > 0.5f)
    {
        const float mix = juce::jlimit(0.0f, 1.0f, parameters.getRawParameterValue("flangerMix")->load(std::memory_order_relaxed));
        const float depth = juce::jlimit(0.0f, 1.0f, parameters.getRawParameterValue("flangerDepth")->load(std::memory_order_relaxed));
        const float feedback = juce::jlimit(-0.7f, 0.7f, parameters.getRawParameterValue("flangerFeedback")->load(std::memory_order_relaxed));
        const double rate = juce::jlimit(0.05, 5.0, static_cast<double>(parameters.getRawParameterValue("flangerRate")->load(std::memory_order_relaxed)));
        const auto length = flangerBuffers[0].size();
        if (mix > 0.0f && length > 2)
        {
            for (int sample = 0; sample < samples; ++sample)
            {
                for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                {
                    auto& line = flangerBuffers[static_cast<size_t>(channel)];
                    auto* data = buffer.getWritePointer(channel);
                    const double phase = flangerPhase + (channel == 0 ? 0.0 : juce::MathConstants<double>::pi);
                    const double delaySamples = currentSampleRate * (0.001 + 0.006 * depth * (0.5 + 0.5 * std::sin(phase)));
                    double readPosition = static_cast<double>(flangerWritePosition) - delaySamples;
                    while (readPosition < 0.0) readPosition += static_cast<double>(length);
                    const auto a = static_cast<size_t>(readPosition) % length;
                    const auto b = (a + 1) % length;
                    const float fraction = static_cast<float>(readPosition - std::floor(readPosition));
                    const float delayed = line[a] + (line[b] - line[a]) * fraction;
                    const float dry = data[sample];
                    line[flangerWritePosition] = dry + delayed * feedback;
                    data[sample] = dry + (delayed - dry) * mix;
                }
                flangerWritePosition = (flangerWritePosition + 1) % length;
                flangerPhase += juce::MathConstants<double>::twoPi * rate / currentSampleRate;
                if (flangerPhase >= juce::MathConstants<double>::twoPi) flangerPhase -= juce::MathConstants<double>::twoPi;
            }
        }
    }

    // DOUBLE: compact second, slightly offset stereo layer. This bounded
    // short-delay thickener uses preallocated memory; no allocation, locks,
    // or file I/O occur in the audio callback.
    if (parameters.getRawParameterValue("doubleTrackOn")->load(std::memory_order_relaxed) > 0.5f
        && samples <= maximumBlockSize && !doubleTrackBuffers[0].empty())
    {
        const float mix = juce::jlimit(0.0f, 1.0f,
            parameters.getRawParameterValue("doubleTrackMix")->load(std::memory_order_relaxed));
        if (mix > 0.0f)
        {
            const auto capacity = doubleTrackBuffers[0].size();
            for (int sample = 0; sample < samples; ++sample)
            {
                for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                {
                    auto& line = doubleTrackBuffers[static_cast<size_t>(channel)];
                    const auto delayFrames = juce::jlimit<size_t>(1, capacity - 1,
                        static_cast<size_t>(currentSampleRate * (channel == 0 ? 0.0137 : 0.0193)));
                    const auto readIndex = (doubleTrackWritePosition + capacity - delayFrames) % capacity;
                    auto* data = buffer.getWritePointer(channel);
                    const float dry = data[sample];
                    const float doubled = line[readIndex];
                    line[doubleTrackWritePosition] = dry;
                    data[sample] = dry * (1.0f - 0.18f * mix) + doubled * (0.42f * mix);
                }
                doubleTrackWritePosition = (doubleTrackWritePosition + 1) % doubleTrackBuffers[0].size();
            }
        }
    }

    // Built-in cabinet convolution using a compact, original damped IR.
    // This is a designed cabinet colour, not a third-party recorded cabinet capture.
    if (parameters.getRawParameterValue("cabinetOn")->load(std::memory_order_relaxed) > 0.5f)
    {
        const float mix = juce::jlimit(0.0f, 1.0f, parameters.getRawParameterValue("cabinetMix")->load(std::memory_order_relaxed));
        if (mix > 0.0f)
        {
            for (int sample = 0; sample < samples; ++sample)
            {
                for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                {
                    auto& history = cabinetHistory[static_cast<size_t>(channel)];
                    auto* data = buffer.getWritePointer(channel);
                    history[cabinetWritePosition] = data[sample];
                    float wet = 0.0f;
                    size_t read = cabinetWritePosition;
                    for (size_t tap = 0; tap < cabinetImpulse.size(); ++tap)
                    {
                        wet += history[read] * cabinetImpulse[tap];
                        read = read == 0 ? cabinetImpulse.size() - 1 : read - 1;
                    }
                    const float dry = data[sample];
                    data[sample] = dry + (wet - dry) * mix;
                }
                cabinetWritePosition = (cabinetWritePosition + 1) % cabinetImpulse.size();
            }
        }
    }

    // First production FX stage: bounded soft-clipping distortion. The
    // processor runs in-place and performs no allocation on the audio thread.
    if (parameters.getRawParameterValue("distortionOn")->load(std::memory_order_relaxed) > 0.5f)
    {
        const float drive = 1.0f + 19.0f * parameters.getRawParameterValue("distortionDrive")->load(std::memory_order_relaxed);
        const float wet = juce::jlimit(0.0f, 1.0f,
            parameters.getRawParameterValue("distortionMix")->load(std::memory_order_relaxed));
        const float normalizer = std::tanh(drive);
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            for (int sample = 0; sample < samples; ++sample)
            {
                const float dry = data[sample];
                const float shaped = std::tanh(dry * drive) / normalizer;
                data[sample] = dry + (shaped - dry) * wet;
            }
        }
    }

    // Chorus: short modulated stereo delay. Delay memories are allocated in
    // prepareToPlay; this loop performs no heap work on the audio thread.
    if (parameters.getRawParameterValue("chorusOn")->load(std::memory_order_relaxed) > 0.5f
        && buffer.getNumSamples() <= maximumBlockSize)
    {
        const float mix = juce::jlimit(0.0f, 1.0f,
            parameters.getRawParameterValue("chorusMix")->load(std::memory_order_relaxed));
        const float depth = juce::jlimit(0.0f, 1.0f,
            parameters.getRawParameterValue("chorusDepth")->load(std::memory_order_relaxed));
        const double rate = juce::jlimit(0.05, 10.0,
            static_cast<double>(parameters.getRawParameterValue("chorusRate")->load(std::memory_order_relaxed)));
        const auto chorusLength = chorusBuffers[0].size();
        if (mix > 0.0f && chorusLength > 2)
        {
            for (int sample = 0; sample < samples; ++sample)
            {
                for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                {
                    auto& line = chorusBuffers[static_cast<size_t>(channel)];
                    auto* data = buffer.getWritePointer(channel);
                    const double channelPhase = chorusPhase + (channel == 0 ? 0.0 : juce::MathConstants<double>::pi);
                    const double modulation = 0.5 + 0.5 * std::sin(channelPhase);
                    const double delaySamples = currentSampleRate * (0.008 + 0.012 * depth * modulation);
                    double readPosition = static_cast<double>(chorusWritePosition) - delaySamples;
                    while (readPosition < 0.0)
                        readPosition += static_cast<double>(chorusLength);
                    const auto indexA = static_cast<size_t>(readPosition) % chorusLength;
                    const auto indexB = (indexA + 1) % chorusLength;
                    const float fraction = static_cast<float>(readPosition - std::floor(readPosition));
                    const float delayed = line[indexA] + (line[indexB] - line[indexA]) * fraction;
                    const float dry = data[sample];
                    line[chorusWritePosition] = dry;
                    data[sample] = dry + (delayed - dry) * mix;
                }
                chorusWritePosition = (chorusWritePosition + 1) % chorusLength;
                chorusPhase += juce::MathConstants<double>::twoPi * rate / currentSampleRate;
                if (chorusPhase >= juce::MathConstants<double>::twoPi)
                    chorusPhase -= juce::MathConstants<double>::twoPi;
            }
        }
    }

    // Tempo-independent stereo delay with bounded feedback and a 2-second
    // maximum delay. The same preallocated ring buffers are reused each block.
    if (parameters.getRawParameterValue("delayOn")->load(std::memory_order_relaxed) > 0.5f)
    {
        const float mix = juce::jlimit(0.0f, 1.0f,
            parameters.getRawParameterValue("delayMix")->load(std::memory_order_relaxed));
        const float feedback = juce::jlimit(0.0f, 0.95f,
            parameters.getRawParameterValue("delayFeedback")->load(std::memory_order_relaxed));
        const double delayMs = juce::jlimit(1.0, 2000.0,
            static_cast<double>(parameters.getRawParameterValue("delayTime")->load(std::memory_order_relaxed)));
        const auto delayLength = delayBuffers[0].size();
        const auto delaySamples = juce::jlimit<size_t>(1, delayLength - 1,
            static_cast<size_t>(std::llround(delayMs * currentSampleRate / 1000.0)));
        for (int sample = 0; sample < samples; ++sample)
        {
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                auto& line = delayBuffers[static_cast<size_t>(channel)];
                auto* data = buffer.getWritePointer(channel);
                const auto readPosition = (delayWritePosition + line.size() - delaySamples) % line.size();
                const float dry = data[sample];
                const float delayed = line[readPosition];
                line[delayWritePosition] = dry + delayed * feedback;
                data[sample] = dry + delayed * mix;
            }
            delayWritePosition = (delayWritePosition + 1) % delayLength;
        }
    }

    // JUCE's Schroeder reverb is configured wet-only; the dry/wet blend is
    // applied from preallocated scratch buffers so bypass and mix=0 stay exact.
    const float reverbMix = juce::jlimit(0.0f, 1.0f,
        parameters.getRawParameterValue("reverbMix")->load(std::memory_order_relaxed));
    if (parameters.getRawParameterValue("reverbOn")->load(std::memory_order_relaxed) > 0.5f
        && reverbMix > 0.0f && samples <= maximumBlockSize)
    {
        juce::Reverb::Parameters reverbParameters;
        reverbParameters.roomSize = juce::jlimit(0.0f, 1.0f,
            parameters.getRawParameterValue("reverbRoomSize")->load(std::memory_order_relaxed));
        reverbParameters.damping = juce::jlimit(0.0f, 1.0f,
            parameters.getRawParameterValue("reverbDamping")->load(std::memory_order_relaxed));
        reverbParameters.wetLevel = 1.0f;
        reverbParameters.dryLevel = 0.0f;
        reverbParameters.width = 1.0f;
        reverbParameters.freezeMode = 0.0f;
        reverb.setParameters(reverbParameters);

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            std::copy_n(buffer.getReadPointer(channel), samples,
                        reverbDryBuffers[static_cast<size_t>(channel)].begin());

        if (buffer.getNumChannels() >= 2)
            reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), samples);
        else
            reverb.processMono(buffer.getWritePointer(0), samples);

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            const auto& dry = reverbDryBuffers[static_cast<size_t>(channel)];
            for (int sample = 0; sample < samples; ++sample)
                data[sample] = dry[static_cast<size_t>(sample)]
                    + (data[sample] - dry[static_cast<size_t>(sample)]) * reverbMix;
        }
    }

    // Global stereo balance and output trim are real post-FX controls.
    const float globalPan = juce::jlimit(-1.0f, 1.0f,
        parameters.getRawParameterValue("pan")->load(std::memory_order_relaxed));
    if (buffer.getNumChannels() >= 2)
    {
        if (globalPan < 0.0f)
            buffer.applyGain(1, 0, buffer.getNumSamples(), 1.0f + globalPan);
        else if (globalPan > 0.0f)
            buffer.applyGain(0, 0, buffer.getNumSamples(), 1.0f - globalPan);
    }

    const float masterGain = juce::jlimit(0.0f, 1.0f,
        parameters.getRawParameterValue("master")->load(std::memory_order_relaxed));
    const float outputDb = juce::jlimit(-24.0f, 12.0f,
        parameters.getRawParameterValue("output")->load(std::memory_order_relaxed));
    buffer.applyGain(masterGain * juce::Decibels::decibelsToGain(outputDb));
}

juce::AudioProcessorEditor* PopPunkGuitarUltimateAudioProcessor::createEditor()
{
    return new PopPunkGuitarUltimateAudioProcessorEditor(*this);
}

void PopPunkGuitarUltimateAudioProcessor::getStateInformation(juce::MemoryBlock& destinationData)
{
    auto state = parameters.copyState();
    {
        const juce::ScopedLock lock(sampleStateLock);
        state.setProperty("sfzFilePath", currentSfzFilePath, nullptr);
    }
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    if (xml != nullptr)
        copyXmlToBinary(*xml, destinationData);
}

void PopPunkGuitarUltimateAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml == nullptr || !xml->hasTagName(parameters.state.getType()))
        return;

    auto restoredState = juce::ValueTree::fromXml(*xml);
    if (!restoredState.isValid())
        return;

    const auto restoredSfzPath = restoredState.getProperty("sfzFilePath").toString();
    // Even a state with no bank path supersedes an older pending load.
    sampleLoadGeneration->fetch_add(1, std::memory_order_acq_rel);
    parameters.replaceState(restoredState);
    {
        const juce::ScopedLock lock(sampleStateLock);
        currentSfzFilePath = restoredSfzPath;
    }

    // Legacy sessions have no path property. Missing libraries are deliberately
    // not loaded; the saved path is retained so the user can locate the bank again.
    if (restoredSfzPath.isNotEmpty())
    {
        const juce::File restoredSfzFile(restoredSfzPath);
        if (restoredSfzFile.existsAsFile()
            && restoredSfzFile.getFileExtension().equalsIgnoreCase(".sfz"))
            loadSfzFileAsync(restoredSfzFile, {});
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PopPunkGuitarUltimateAudioProcessor();
}

ppgu::sampler::LoadReport PopPunkGuitarUltimateAudioProcessor::loadSfzFile(const juce::File& sfzFile)
{
    // Synchronous API for tests/tools only; supersede any queued async request.
    sampleLoadGeneration->fetch_add(1, std::memory_order_acq_rel);
    auto report = sampler.loadSfz(sfzFile);
    if (report.success)
    {
        const juce::ScopedLock lock(sampleStateLock);
        currentSfzFilePath = sfzFile.getFullPathName();
    }
    return report;
}

juce::String PopPunkGuitarUltimateAudioProcessor::getCurrentSfzFilePath() const
{
    const juce::ScopedLock lock(sampleStateLock);
    return currentSfzFilePath;
}

void PopPunkGuitarUltimateAudioProcessor::loadSfzFileAsync(
    const juce::File& sfzFile, std::function<void(ppgu::sampler::LoadReport)> completion)
{
    const auto generationState = sampleLoadGeneration;
    const auto generation = generationState->fetch_add(1, std::memory_order_acq_rel) + 1;
    sampleLoadPool.addJob([this, sfzFile, completion = std::move(completion),
                           generationState, generation]() mutable
    {
        // A newer request may arrive while this job waits in the single-worker
        // queue. Skip stale scans entirely; if superseded during decode, do
        // not publish its path or completion (the newer queued job follows).
        if (generationState->load(std::memory_order_acquire) != generation)
            return;

        auto report = sampler.loadSfz(sfzFile, generationState.get(), generation);
        if (generationState->load(std::memory_order_acquire) != generation)
            return;

        if (report.success)
        {
            const juce::ScopedLock lock(sampleStateLock);
            currentSfzFilePath = sfzFile.getFullPathName();
        }

        juce::MessageManager::callAsync(
            [completion = std::move(completion), report = std::move(report),
             generationState, generation]() mutable
            {
                if (generationState->load(std::memory_order_acquire) == generation && completion)
                    completion(std::move(report));
            });
    });
}
