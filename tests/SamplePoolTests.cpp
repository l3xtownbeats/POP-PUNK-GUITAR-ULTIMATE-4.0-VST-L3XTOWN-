#include "SamplePool.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>

int main()
{
    const auto root = std::filesystem::temp_directory_path() / "ppgu-sample-pool-tests";
    std::filesystem::create_directories(root);
    const juce::File directory(root.string());
    const auto wavFile = directory.getChildFile("test.wav");
    const auto sfzFile = directory.getChildFile("test.sfz");

    juce::AudioBuffer<float> source(1, 512);
    for (int i = 0; i < source.getNumSamples(); ++i)
        source.setSample(0, i, static_cast<float>(0.5 * std::sin(2.0 * juce::MathConstants<double>::pi * 440.0 * i / 44100.0)));

    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::OutputStream> output(wavFile.createOutputStream());
    if (output == nullptr)
    {
        std::cerr << "Could not create test WAV file.\n";
        return 1;
    }
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wavFormat.createWriterFor(output.get(), 44100.0, 1, 16, {}, 0));
    if (writer == nullptr)
    {
        std::cerr << "Could not create WAV writer.\n";
        return 1;
    }
    output.release(); // The writer owns the stream after successful creation.
    if (!writer->writeFromAudioSampleBuffer(source, 0, source.getNumSamples()))
    {
        std::cerr << "Could not write test WAV data.\n";
        return 1;
    }
    writer.reset();

    if (!sfzFile.replaceWithText("<master> lochan=1 hichan=1\n<group> group=1 off_by=1\n<region> sample=test.wav key=60 ampeg_release=0.01 loop_mode=loop_continuous loop_start=32 loop_end=400\n<master> lochan=2 hichan=2\n<group> group=2 off_by=2\n<region> sample=test.wav key=60 ampeg_release=0.01 loop_mode=loop_sustain loop_start=32 loop_end=400\n<master> lochan=3 hichan=3\n<region> sample=test.wav key=60 ampeg_release=0.01 loop_mode=loop_pingpong loop_start=32 loop_end=400\n<master> lochan=4 hichan=4\n<region> sample=test.wav key=60 ampeg_release=0.01 loop_mode=one_shot\n"))
    {
        std::cerr << "Could not write test SFZ file.\n";
        return 1;
    }

    ppgu::sampler::SamplerEngine engine;
    engine.prepare(44100.0);
    const auto loaded = engine.loadSfz(sfzFile);
    if (!loaded.success || loaded.loadedRegions != 4 || loaded.loadedSamples != 1)
    {
        std::cerr << "SamplePool failed to load generated WAV/SFZ fixture.\n";
        for (const auto& error : loaded.errors) std::cerr << error << "\n";
        return 1;
    }

    // SFZ seq_length/seq_position must rotate deterministically without
    // repeating a position until the complete round-robin cycle is consumed.
    const auto roundRobinSfz = directory.getChildFile("round-robin.sfz");
    if (!roundRobinSfz.replaceWithText(
        "<group> group=77\n"
        "<region> sample=test.wav key=64 seq_length=3 seq_position=1\n"
        "<region> sample=test.wav key=64 seq_length=3 seq_position=2\n"
        "<region> sample=test.wav key=64 seq_length=3 seq_position=3\n"))
    {
        std::cerr << "Could not write SFZ round-robin fixture.\n";
        return 1;
    }
    ppgu::sampler::SamplePool roundRobinPool;
    const auto rrLoad = roundRobinPool.loadSfz(roundRobinSfz);
    if (!rrLoad.success || rrLoad.loadedRegions != 3)
    {
        std::cerr << "SFZ round-robin regions failed to load.\n";
        return 1;
    }
    for (int expectedPosition = 1; expectedPosition <= 6; ++expectedPosition)
    {
        const auto* selected = roundRobinPool.findRegion(64, 100, 1);
        const int expected = ((expectedPosition - 1) % 3) + 1;
        if (selected == nullptr || selected->seqPosition != expected)
        {
            std::cerr << "SFZ round-robin repeated or skipped a sequence position.\n";
            return 1;
        }
    }
    // Independent RR sets with distinct SFZ group IDs must not advance
    // one another's cursors when their notes are interleaved.
    const auto independentRoundRobinSfz = directory.getChildFile("independent-round-robin.sfz");
    if (!independentRoundRobinSfz.replaceWithText(
        "<group> group=77\\n"
        "<region> sample=test.wav key=65 seq_length=3 seq_position=1\\n"
        "<region> sample=test.wav key=65 seq_length=3 seq_position=2\\n"
        "<region> sample=test.wav key=65 seq_length=3 seq_position=3\\n"
        "<group> group=78\\n"
        "<region> sample=test.wav key=65 seq_length=3 seq_position=1\\n"
        "<region> sample=test.wav key=65 seq_length=3 seq_position=2\\n"
        "<region> sample=test.wav key=65 seq_length=3 seq_position=3\\n"))
    {
        std::cerr << "Could not write independent SFZ round-robin fixture.\\n";
        return 1;
    }
    ppgu::sampler::SamplePool independentRoundRobinPool;
    const auto independentLoad = independentRoundRobinPool.loadSfz(independentRoundRobinSfz);
    if (!independentLoad.success || independentLoad.loadedRegions != 6)
    {
        std::cerr << "Independent SFZ round-robin sets failed to load.\\n";
        return 1;
    }
    const auto* firstSetFirst = independentRoundRobinPool.findRegion(65, 100, 1);
    const auto* secondSetFirst = independentRoundRobinPool.findRegion(65, 100, 1);
    const auto* firstSetSecond = independentRoundRobinPool.findRegion(65, 100, 1);
    if (firstSetFirst == nullptr || secondSetFirst == nullptr || firstSetSecond == nullptr
        || firstSetFirst->seqPosition != 1 || secondSetFirst->seqPosition != 1
        || firstSetSecond->seqPosition != 2)
    {
        std::cerr << "Independent SFZ round-robin groups shared a cursor.\\n";
        return 1;
    }

    const auto invalidRoundRobin = ppgu::sfz::parseText(
        "<region> sample=test.wav key=64 seq_length=2 seq_position=3\n", root);
    if (!invalidRoundRobin.errors.size())
    {
        std::cerr << "SFZ parser accepted seq_position greater than seq_length.\n";
        return 1;
    }

    // Meta Open Sound's official SFZ maps reference .ogg preview names,
    // while the redistributable WAV release stores same-stem files under wav/.
    // Verify the production loader resolves that map without rewriting it.
    const auto wavDirectory = root / "wav";
    std::filesystem::create_directories(wavDirectory);
    const auto metaWav = wavDirectory / "test.wav";
    std::filesystem::copy_file(wavFile.getFullPathName().toStdString(), metaWav,
                               std::filesystem::copy_options::overwrite_existing);
    const auto metaSfzFile = directory.getChildFile("meta-ogg-reference.sfz");
    if (!metaSfzFile.replaceWithText(
        "<control>\ndefault_path=ogg/\n"
        "<global> ampeg_release=0.05\n"
        "<group> group=1 off_by=1 lovel=1 hivel=127\n"
        "<region> key=60 sample=test.ogg\n"))
    {
        std::cerr << "Could not write Meta-style OGG-reference SFZ fixture.\n";
        return 1;
    }

    ppgu::sampler::SamplerEngine metaFallbackEngine;
    metaFallbackEngine.prepare(44100.0);
    const auto metaLoaded = metaFallbackEngine.loadSfz(metaSfzFile);
    if (!metaLoaded.success || metaLoaded.loadedRegions != 1 || metaLoaded.loadedSamples != 1)
    {
        std::cerr << "Sampler did not resolve an SFZ OGG reference to the same-name WAV release file.\n";
        for (const auto& error : metaLoaded.errors) std::cerr << error << "\n";
        return 1;
    }
    if (!metaFallbackEngine.noteOn(60, 100))
    {
        std::cerr << "Meta-style SFZ OGG-to-WAV fallback did not produce a playable region.\n";
        return 1;
    }
    juce::AudioBuffer<float> metaRendered(2, 128);
    metaRendered.clear();
    metaFallbackEngine.render(metaRendered, 0, metaRendered.getNumSamples());
    if (metaRendered.getMagnitude(0, 0, metaRendered.getNumSamples()) <= 0.001f)
    {
        std::cerr << "Meta-style SFZ fallback loaded but rendered silence.\n";
        return 1;
    }

    // Global tuning must alter playback rate by the expected octave.
    ppgu::sampler::SamplerEngine baseTuningEngine;
    baseTuningEngine.prepare(44100.0);
    if (!baseTuningEngine.loadSfz(sfzFile).success || !baseTuningEngine.noteOn(60, 100))
    {
        std::cerr << "Could not prepare base-pitch tuning regression fixture.\n";
        return 1;
    }

    ppgu::sampler::SamplerEngine octaveTuningEngine;
    octaveTuningEngine.prepare(44100.0);
    if (!octaveTuningEngine.loadSfz(sfzFile).success
        || !octaveTuningEngine.noteOn(60, 100, ppgu::sampler::SamplerEngine::maximumVoices, 1, 12.0))
    {
        std::cerr << "Could not prepare octave-tuning regression fixture.\n";
        return 1;
    }

    juce::AudioBuffer<float> basePitch(2, 256);
    juce::AudioBuffer<float> octavePitch(2, 256);
    basePitch.clear();
    octavePitch.clear();
    baseTuningEngine.render(basePitch, 0, basePitch.getNumSamples());
    octaveTuningEngine.render(octavePitch, 0, octavePitch.getNumSamples());

    const auto risingZeroCrossings = [](const juce::AudioBuffer<float>& audio)
    {
        int crossings = 0;
        float previous = audio.getSample(0, 0);
        for (int sample = 1; sample < audio.getNumSamples(); ++sample)
        {
            const float current = audio.getSample(0, sample);
            if (previous <= 0.0f && current > 0.0f)
                ++crossings;
            previous = current;
        }
        return crossings;
    };
    const int baseCrossings = risingZeroCrossings(basePitch);
    const int octaveCrossings = risingZeroCrossings(octavePitch);
    if (baseCrossings < 2 || octaveCrossings < baseCrossings + 2)
    {
        std::cerr << "Global tuning did not raise the sample playback pitch by one octave.\n";
        return 1;
    }

    // A failed reload must leave the previously published map intact. The
    // first new WAV is valid, the second is deliberately corrupt.
    const auto orphanWav = directory.getChildFile("orphan.wav");
    const auto corruptWav = directory.getChildFile("corrupt.wav");
    const auto failedReloadSfz = directory.getChildFile("failed-reload.sfz");
    std::filesystem::copy_file(wavFile.getFullPathName().toStdString(),
                               orphanWav.getFullPathName().toStdString(),
                               std::filesystem::copy_options::overwrite_existing);
    if (!corruptWav.replaceWithText("not a WAV file")
        || !failedReloadSfz.replaceWithText(
            "<region> sample=orphan.wav key=61\n"
            "<region> sample=corrupt.wav key=62\n"))
    {
        std::cerr << "Could not create failed-reload regression fixture.\n";
        return 1;
    }

    const auto failedReload = engine.loadSfz(failedReloadSfz);
    if (failedReload.success || failedReload.errors.empty()
        || engine.getRegionCount() != 4 || engine.getSampleCount() != 1)
    {
        std::cerr << "Failed SFZ reload changed the active sample map.\n";
        return 1;
    }

    // Snapshot retention is deliberately bounded because active voices keep
    // raw pointers into old immutable snapshots until the engine is destroyed.
    for (int reload = 0; reload < 127; ++reload)
    {
        const auto repeatedLoad = engine.loadSfz(sfzFile);
        if (!repeatedLoad.success)
        {
            std::cerr << "Sampler rejected a bank before the documented snapshot cap.\n";
            return 1;
        }
    }
    const auto cappedLoad = engine.loadSfz(sfzFile);
    if (cappedLoad.success || cappedLoad.errors.empty()
        || engine.getRegionCount() != 4 || engine.getSampleCount() != 1)
    {
        std::cerr << "Sampler snapshot cap did not reject the next reload safely.\n";
        return 1;
    }

    if (!engine.noteOn(60, 100, 2, 1) || !engine.noteOn(60, 110, 2, 2) || !engine.noteOn(60, 120, 2, 1))
    {
        std::cerr << "MIDI note did not trigger a loaded sample.\n";
        return 1;
    }
    if (engine.noteOn(60, 100, 2, 5))
    {
        std::cerr << "Region lookup ignored the MIDI channel map.\n";
        return 1;
    }
    if (engine.getActiveVoiceCount() != 2)
    {
        std::cerr << "Voice limit was not enforced.\n";
        return 1;
    }

    juce::AudioBuffer<float> rendered(2, 128);
    rendered.clear();
    engine.render(rendered, 0, rendered.getNumSamples());
    juce::AudioBuffer<float> longRender(2, 600);
    longRender.clear();
    engine.render(longRender, 0, longRender.getNumSamples());
    if (engine.getActiveVoiceCount() == 0)
    {
        std::cerr << "Continuous SFZ loop ended before note-off.\n";
        return 1;
    }

    float peak = 0.0f;
    for (int ch = 0; ch < rendered.getNumChannels(); ++ch)
        peak = juce::jmax(peak, rendered.getMagnitude(ch, 0, rendered.getNumSamples()));
    if (peak <= 0.001f)
    {
        std::cerr << "Sampler rendered silence for a valid MIDI note.\n";
        return 1;
    }

    engine.noteOff(60, 2);
    engine.render(longRender, 0, longRender.getNumSamples());
    if (engine.getActiveVoiceCount() != 1)
    {
        std::cerr << "Sustain loop did not release after MIDI note-off.\n";
        return 1;
    }
    engine.noteOff(60, 1);
    engine.render(longRender, 0, longRender.getNumSamples());
    if (engine.getActiveVoiceCount() != 0)
    {
        std::cerr << "Continuous loop did not fade out after MIDI note-off.\n";
        return 1;
    }
    if (!engine.noteOn(60, 100, 2, 3))
    {
        std::cerr << "Ping-pong region was not selected for MIDI channel 3.\n";
        return 1;
    }
    engine.render(longRender, 0, longRender.getNumSamples());
    if (engine.getActiveVoiceCount() != 1)
    {
        std::cerr << "Ping-pong loop did not remain active across loop boundaries.\n";
        return 1;
    }
    engine.noteOff(60, 3);
    engine.render(longRender, 0, longRender.getNumSamples());
    if (engine.getActiveVoiceCount() != 0)
    {
        std::cerr << "Ping-pong loop did not exit and release after note-off.\n";
        return 1;
    }

    if (!engine.noteOn(60, 100, 2, 4))
    {
        std::cerr << "One-shot region was not selected for MIDI channel 4.\n";
        return 1;
    }
    engine.noteOff(60, 4);
    engine.render(rendered, 0, rendered.getNumSamples());
    if (engine.getActiveVoiceCount() != 1)
    {
        std::cerr << "One-shot playback was incorrectly stopped by MIDI note-off.\n";
        return 1;
    }
    engine.render(longRender, 0, longRender.getNumSamples());
    if (engine.getActiveVoiceCount() != 0)
    {
        std::cerr << "One-shot sample did not finish at its sample end.\n";
        return 1;
    }


    // SFZ group/off_by choking must stay within the triggering MIDI channel.
    // Different guitar strings may be routed on separate MIDI channels; a new
    // note on channel 1 must not choke the same group number on channel 2.
    const auto channelChokeSfz = directory.getChildFile("channel-choke.sfz");
    if (!channelChokeSfz.replaceWithText(
        "<master> lochan=1 hichan=1\n"
        "<group> group=1 off_by=1\n"
        "<region> sample=test.wav key=60 ampeg_release=0.002 loop_mode=loop_continuous loop_start=32 loop_end=400\n"
        "<master> lochan=2 hichan=2\n"
        "<group> group=1 off_by=1\n"
        "<region> sample=test.wav key=60 ampeg_release=0.002 loop_mode=loop_continuous loop_start=32 loop_end=400\n"))
    {
        std::cerr << "Could not write MIDI-channel choke regression SFZ.\n";
        return 1;
    }

    ppgu::sampler::SamplerEngine channelChokeEngine;
    channelChokeEngine.prepare(44100.0);
    if (!channelChokeEngine.loadSfz(channelChokeSfz).success
        || !channelChokeEngine.noteOn(60, 100, 64, 2)
        || !channelChokeEngine.noteOn(60, 100, 64, 1))
    {
        std::cerr << "Could not prepare MIDI-channel choke regression.\n";
        return 1;
    }
    juce::AudioBuffer<float> chokeRendered(2, 128);
    chokeRendered.clear();
    channelChokeEngine.render(chokeRendered, 0, chokeRendered.getNumSamples());
    if (channelChokeEngine.getActiveVoiceCount() != 2)
    {
        std::cerr << "SFZ off_by incorrectly choked a voice on another MIDI channel.\n";
        return 1;
    }

    // M2 acceptance regression: 100 repeated guitar hits at changing
    // velocities must remain finite and avoid abrupt discontinuities.
    ppgu::sampler::SamplerEngine repeatedHitEngine;
    repeatedHitEngine.prepare(44100.0);
    if (!repeatedHitEngine.loadSfz(sfzFile).success)
    {
        std::cerr << "Could not load repeated-hit regression bank.\n";
        return 1;
    }
    juce::AudioBuffer<float> repeatedHits(2, 12800);
    repeatedHits.clear();
    for (int hit = 0; hit < 100; ++hit)
    {
        const int velocity = 24 + ((hit * 37) % 104);
        const int offset = hit * 128;
        if (!repeatedHitEngine.noteOn(60, velocity, 64, 1))
        {
            std::cerr << "Repeated-hit regression failed to trigger note " << hit << ".\n";
            return 1;
        }
        repeatedHitEngine.render(repeatedHits, offset, 64);
        repeatedHitEngine.noteOff(60, 1);
        repeatedHitEngine.render(repeatedHits, offset + 64, 64);
    }
    float maximumAdjacentDelta = 0.0f;
    float repeatedHitPeak = 0.0f;
    for (int sample = 0; sample < repeatedHits.getNumSamples(); ++sample)
    {
        const float value = repeatedHits.getSample(0, sample);
        if (!std::isfinite(value))
        {
            std::cerr << "Repeated-hit render produced a non-finite sample.\n";
            return 1;
        }
        repeatedHitPeak = juce::jmax(repeatedHitPeak, std::abs(value));
        if (sample > 0)
            maximumAdjacentDelta = juce::jmax(maximumAdjacentDelta,
                std::abs(value - repeatedHits.getSample(0, sample - 1)));
    }
    if (repeatedHitPeak <= 0.001f || maximumAdjacentDelta > 0.20f)
    {
        std::cerr << "100 repeated hits produced silence or an audible discontinuity; max delta="
                  << maximumAdjacentDelta << ".\n";
        return 1;
    }

    std::cout << "SamplePool tests passed: WAV decode, SFZ load, MIDI playback, and polyphony cap.\n";
    return 0;
}
