#ifndef JucePlugin_Name
#define JucePlugin_Name "POP PUNK GUITAR ULTIMATE"
#endif

// Conflict-check: this integration test drives the real AudioProcessor path
// (MIDI -> held chord -> PatternScheduler -> SamplerEngine -> streamed WAV).
// It uses only temporary synthetic fixtures; no production sample assets or
// licensed third-party content are copied into the test.
#include "PluginProcessor.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <utility>

namespace
{
bool writeTone(const juce::File& file, float amplitude, double frequency)
{
    constexpr int sampleRate = 48000;
    constexpr int frames = sampleRate;
    juce::AudioBuffer<float> audio(1, frames);
    for (int i = 0; i < frames; ++i)
        audio.setSample(0, i, amplitude * std::sin(
            juce::MathConstants<double>::twoPi * frequency * static_cast<double>(i) / sampleRate));

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::OutputStream> output(file.createOutputStream());
    if (!output)
        return false;

    std::unique_ptr<juce::AudioFormatWriter> writer(
        wav.createWriterFor(output.get(), sampleRate, 1, 24, {}, 0));
    if (!writer)
        return false;

    output.release();
    return writer->writeFromAudioSampleBuffer(audio, 0, frames);
}

bool setParameter(PopPunkGuitarUltimateAudioProcessor& processor,
                  const char* id, float value)
{
    auto* parameter = processor.parameters.getRawParameterValue(id);
    if (parameter == nullptr)
    {
        std::cerr << "Missing APVTS parameter: " << id << "\n";
        return false;
    }
    parameter->store(value, std::memory_order_relaxed);
    return true;
}

double rms(const juce::AudioBuffer<float>& audio, int start, int length)
{
    const int boundedStart = juce::jlimit(0, audio.getNumSamples(), start);
    const int boundedEnd = juce::jlimit(boundedStart, audio.getNumSamples(), start + length);
    if (boundedEnd <= boundedStart)
        return 0.0;

    double sum = 0.0;
    std::uint64_t count = 0;
    for (int channel = 0; channel < audio.getNumChannels(); ++channel)
    {
        const auto* data = audio.getReadPointer(channel);
        for (int i = boundedStart; i < boundedEnd; ++i)
        {
            const double value = data[i];
            sum += value * value;
            ++count;
        }
    }
    return count == 0 ? 0.0 : std::sqrt(sum / static_cast<double>(count));
}

void addChordOn(juce::MidiBuffer& midi, int sample, const std::array<int, 3>& notes)
{
    for (const int note : notes)
        midi.addEvent(juce::MidiMessage::noteOn(1, note, static_cast<juce::uint8>(110)), sample);
}

void addChordOff(juce::MidiBuffer& midi, int sample, const std::array<int, 3>& notes)
{
    for (const int note : notes)
        midi.addEvent(juce::MidiMessage::noteOff(1, note), sample);
}
}

int main()
{
    juce::ScopedJuceInitialiser_GUI juceInitialiser;
    const auto root = std::filesystem::temp_directory_path() / "ppgu-processor-pattern-integration";
    std::error_code cleanupError;
    std::filesystem::remove_all(root, cleanupError);
    std::filesystem::create_directories(root);

    const juce::File directory(root.string());
    const auto firstTone = directory.getChildFile("first-chord.wav");
    const auto secondTone = directory.getChildFile("second-chord.wav");
    const auto thirdTone = directory.getChildFile("third-chord.wav");
    const auto sfz = directory.getChildFile("pattern-test.sfz");

    // Different source levels make the chord identity observable in rendered
    // audio: chord A is deliberately 3x louder than chord B.
    if (!writeTone(firstTone, 0.60f, 220.0)
        || !writeTone(secondTone, 0.40f, 330.0)
        || !writeTone(thirdTone, 0.20f, 440.0))
    {
        std::cerr << "Could not create deterministic WAV fixtures.\n";
        return 1;
    }

    const auto sfzText =
        "<region> sample=first-chord.wav key=60 ampeg_release=0.01\n"
        "<region> sample=first-chord.wav key=64 ampeg_release=0.01\n"
        "<region> sample=first-chord.wav key=67 ampeg_release=0.01\n"
        "<region> sample=second-chord.wav key=62 ampeg_release=0.01\n"
        "<region> sample=second-chord.wav key=66 ampeg_release=0.01\n"
        "<region> sample=second-chord.wav key=69 ampeg_release=0.01\n"
        "<region> sample=third-chord.wav key=55 ampeg_release=0.01\n"
        "<region> sample=third-chord.wav key=59 ampeg_release=0.01\n"
        "<region> sample=third-chord.wav key=62 ampeg_release=0.01\n";
    if (!sfz.replaceWithText(sfzText))
    {
        std::cerr << "Could not create SFZ fixture.\n";
        return 1;
    }

    int result = 0;
    {
        PopPunkGuitarUltimateAudioProcessor processor;
        processor.prepareToPlay(48000.0, 13000);
        const auto load = processor.loadSfzFile(sfz);
        if (!load.success)
        {
            std::cerr << "Processor failed to load SFZ fixture.\n";
            for (const auto& error : load.errors)
                std::cerr << error << "\n";
            result = 1;
        }

        const std::array<std::pair<const char*, float>, 17> settings {{
            { "playMode", 1.0f }, { "syncMode", 2.0f }, { "patternSlot", 0.0f },
            { "patternLength", 0.0f }, { "patternQuantize", 0.0f }, { "bpm", 120.0f },
            { "startOnFirstChord", 1.0f }, { "voices", 64.0f }, { "master", 1.0f },
            { "ampGain", 5.0f }, { "eqOn", 0.0f }, { "compressorOn", 0.0f },
            { "distortionOn", 0.0f }, { "chorusOn", 0.0f }, { "flangerOn", 0.0f },
            { "delayOn", 0.0f }, { "reverbOn", 0.0f }
        }};
        for (const auto& setting : settings)
            if (!setParameter(processor, setting.first, setting.second))
                result = 1;
        if (!setParameter(processor, "cabinetOn", 0.0f)
            || !setParameter(processor, "doubleTrackOn", 0.0f))
            result = 1;

        if (result == 0)
        {
            const std::array<int, 3> chordA { 60, 64, 67 };
            const std::array<int, 3> chordB { 62, 66, 69 };
            const std::array<int, 3> chordC { 55, 59, 62 };

            // One real processor block contains two distinct chord replacements.
            // The second replacement must not expose a transient empty chord or
            // restart the pattern cycle. The third chord should drive step 2.
            juce::AudioBuffer<float> blockA(2, 13000);
            juce::MidiBuffer midiA;
            addChordOn(midiA, 16, chordA);
            addChordOff(midiA, 6000, chordA);
            addChordOn(midiA, 6000, chordB);
            addChordOff(midiA, 9000, chordB);
            addChordOn(midiA, 9000, chordC);
            processor.processBlock(blockA, midiA);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));

            // The first step of the next 8th-note slot is at absolute sample
            // 12016. Its source page may have missed the cache in block A, so
            // the streaming reader must retain the cursor into this block.
            juce::AudioBuffer<float> blockB(2, 13000);
            juce::MidiBuffer midiB;
            processor.processBlock(blockB, midiB);

            const double firstChordRms = rms(blockA, 256, 4200);
            const double finalChordRms = rms(blockB, 1000, 7000);
            if (!(firstChordRms > 0.04))
            {
                std::cerr << "First chord did not reach the real sampler output; RMS="
                          << firstChordRms << "\n";
                result = 1;
            }
            if (!(finalChordRms > 0.008 && finalChordRms < firstChordRms * 0.65))
            {
                std::cerr << "Final chord snapshot was not rendered after two same-block chord replacements; "
                          << "first RMS=" << firstChordRms
                          << ", final RMS=" << finalChordRms << "\n";
                result = 1;
            }
        }
    }

    std::filesystem::remove_all(root, cleanupError);
    if (cleanupError)
    {
        std::cerr << "Could not remove temporary integration fixtures: "
                  << cleanupError.message() << "\n";
        result = 1;
    }

    if (result == 0)
        std::cout << "Processor Pattern MIDI integration test passed.\n";
    return result;
}
