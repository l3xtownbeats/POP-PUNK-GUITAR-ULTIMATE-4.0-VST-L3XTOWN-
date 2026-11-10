#include "Sampler/SamplePool.h"
#include "Sampler/SfzParser.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <thread>

static bool audition(const juce::File& sfz, const char* label)
{
    if (!sfz.existsAsFile()) { std::cerr << label << ": missing SFZ " << sfz.getFullPathName() << "\n"; return false; }
    const auto parsed = ppgu::sfz::parseFile(std::filesystem::path(sfz.getFullPathName().toStdString()));
    if (!parsed.succeeded()) { std::cerr << label << ": SFZ parse failed\n"; return false; }
    const auto missing = ppgu::sfz::validateSampleFiles(parsed);
    if (!missing.empty()) { std::cerr << label << ": missing mapped samples=" << missing.size() << "\n"; return false; }

    ppgu::sampler::SamplerEngine sampler;
    sampler.prepare(44100.0);
    const auto report = sampler.loadSfz(sfz);
    if (!report.success || report.loadedRegions == 0 || report.loadedSamples == 0) {
        std::cerr << label << ": bank load failed, regions=" << report.loadedRegions << ", samples=" << report.loadedSamples << "\n";
        for (const auto& error : report.errors) std::cerr << error << "\n";
        return false;
    }

    const int note = parsed.regions.front().rootKey;
    if (!sampler.noteOn(note, 112)) { std::cerr << label << ": note-on failed for root " << note << "\n"; return false; }
    juce::AudioBuffer<float> audio(2, 256);
    bool audible = false;
    for (int attempt = 0; attempt < 150 && !audible; ++attempt) {
        audio.clear();
        sampler.render(audio, 0, audio.getNumSamples());
        const auto magnitude = juce::jmax(audio.getMagnitude(0, 0, audio.getNumSamples()),
                                          audio.getMagnitude(1, 0, audio.getNumSamples()));
        audible = std::isfinite(magnitude) && magnitude > 0.0001f;
        if (!audible) std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    sampler.noteOff(note);
    if (!audible) { std::cerr << label << ": streamed note produced silence\n"; return false; }
    std::cout << label << " PASS: regions=" << report.loadedRegions << ", samples=" << report.loadedSamples << ", note=" << note << "\n";
    return true;
}

int main(int argc, char** argv)
{
    if (argc != 3) { std::cerr << "Usage: ppgu_factory_soundbank_tests <electric.sfz> <acoustic.sfz>\n"; return 2; }
    const bool electric = audition(juce::File(argv[1]), "ELECTRIC");
    const bool acoustic = audition(juce::File(argv[2]), "ACOUSTIC");
    return electric && acoustic ? 0 : 1;
}
