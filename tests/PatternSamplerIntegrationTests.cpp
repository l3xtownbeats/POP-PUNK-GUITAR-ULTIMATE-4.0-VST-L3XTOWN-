#include "Sampler/SamplePool.h"
#include "Performance/PatternScheduler.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>\n#include <thread>\n#include <chrono>

int main()
{
    using namespace ppgu::performance;
    using namespace ppgu::sampler;

    const auto root = std::filesystem::temp_directory_path() / "ppgu-pattern-sampler-integration";
    std::filesystem::create_directories(root);
    const juce::File directory(root.string());
    const auto wavFile = directory.getChildFile("pattern.wav");
    const auto sfzFile = directory.getChildFile("pattern.sfz");

    juce::AudioBuffer<float> source(1, 8192);
    for (int i = 0; i < source.getNumSamples(); ++i)
        source.setSample(0, i, static_cast<float>(0.4 * std::sin(
            2.0 * juce::MathConstants<double>::pi * 330.0 * i / 44100.0)));

    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::OutputStream> output(wavFile.createOutputStream());
    if (output == nullptr) return 1;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wavFormat.createWriterFor(output.get(), 44100.0, 1, 16, {}, 0));
    if (writer == nullptr) return 2;
    output.release();
    if (!writer->writeFromAudioSampleBuffer(source, 0, source.getNumSamples())) return 3;
    writer.reset();

    if (!sfzFile.replaceWithText("<region> sample=pattern.wav key=60 ampeg_release=0.02\n"))
        return 4;

    SamplerEngine sampler;
    sampler.prepare(44100.0);
    const auto loaded = sampler.loadSfz(sfzFile);
    if (!loaded.success || loaded.loadedRegions != 1)
    {
        for (const auto& error : loaded.errors) std::cerr << error << "\n";
        return 5;
    }

    // Warm the asynchronous page cache before measuring the sample-accurate
    // dispatch path. A cold cache miss intentionally stalls the voice cursor.
    if (!sampler.noteOn(60, 112)) return 6;
    juce::AudioBuffer<float> warmup(2, 128);
    bool warmed = false;
    for (int attempt = 0; attempt < 100 && !warmed; ++attempt)
    {
        warmup.clear();
        sampler.render(warmup, 0, warmup.getNumSamples());
        warmed = warmup.getMagnitude(0, 0, warmup.getNumSamples()) > 0.001f;
        if (!warmed) std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    sampler.allNotesOff();
    if (!warmed) return 7;

    PatternScheduler scheduler;
    TransportSnapshot host;
    host.hostValid = true;
    host.playing = true;
    host.bpm = 120.0;
    host.ppq = 0.0;

    constexpr int blockSamples = 512;
    constexpr double sampleRate = 44100.0;
    const auto clock = scheduler.beginBlock(host, false, 120.0, sampleRate, blockSamples);
    if (!clock.running || !clock.usingHost) return 6;

    const std::array<PatternEvent, 1> pattern {{
        PatternEvent { 0.0, 0.02, 60, 112, 1 }
    }};
    std::array<ScheduledEvent, PatternScheduler::maximumEventsPerBlock> events {};
    const auto count = scheduler.schedule(pattern, 4.0, 0.0, clock,
                                          sampleRate, blockSamples, 0.0, 0, events);
    if (count != 2 || events[0].kind != EventKind::NoteOn
        || events[1].kind != EventKind::NoteOff || events[0].sampleOffset != 0
        || events[1].sampleOffset <= events[0].sampleOffset)
        return 7;

    juce::AudioBuffer<float> rendered(2, blockSamples);
    rendered.clear();
    int cursor = 0;
    for (std::size_t i = 0; i < count; ++i)
    {
        const int eventSample = events[i].sampleOffset;
        if (eventSample > cursor)
            sampler.render(rendered, cursor, eventSample - cursor);
        cursor = eventSample;
        if (events[i].kind == EventKind::NoteOn)
        {
            if (!sampler.noteOn(events[i].note, events[i].velocity,
                                SamplerEngine::maximumVoices, events[i].channel))
                return 8;
        }
        else
            sampler.noteOff(events[i].note, events[i].channel);
    }
    if (cursor < blockSamples)
        sampler.render(rendered, cursor, blockSamples - cursor);

    const auto magnitude = rendered.getMagnitude(0, 0, blockSamples);
    if (!std::isfinite(magnitude) || magnitude <= 0.001f)
    {
        std::cerr << "Scheduled MIDI events did not produce audible streamed sampler output.\n";
        return 9;
    }

    // A stopped host block must stop scheduler dispatch and clear pending MIDI.
    host.playing = false;
    host.ppq = clock.endPpq;
    const auto stopped = scheduler.beginBlock(host, false, 120.0, sampleRate, blockSamples);
    if (!stopped.transportStopped || stopped.running)
        return 10;

    std::cout << "PatternScheduler -> SamplerEngine integration passed; magnitude="
              << magnitude << "\n";
    return 0;
}
