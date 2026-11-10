#include "SampleLibraryImporter.h"
#include "SfzParser.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <array>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

namespace
{
bool writeFixture(const juce::File& file, juce::AudioFormat& format, int bits)
{
    if (!file.getParentDirectory().createDirectory())
        return false;
    std::unique_ptr<juce::OutputStream> stream(file.createOutputStream());
    if (!stream) return false;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        format.createWriterFor(stream.get(), 44100.0, 1, bits, {}, 0));
    if (!writer) return false;
    stream.release();

    juce::AudioBuffer<float> audio(1, 4096);
    for (int i = 0; i < audio.getNumSamples(); ++i)
        audio.setSample(0, i, 0.3f * std::sin(
            2.0 * juce::MathConstants<double>::pi * 330.0 * i / 44100.0));
    const bool ok = writer->writeFromAudioSampleBuffer(audio, 0, audio.getNumSamples());
    writer.reset();
    return ok;
}
}

int main()
{
    // Ensure all advertised common interchange formats are registered in this build.
    juce::AudioFormatManager registeredFormats;
    registeredFormats.registerBasicFormats();
    for (const auto* extension : { "wav", "aiff", "aif", "flac", "ogg", "mp3" })
    {
        if (registeredFormats.findFormatForFileExtension(extension) == nullptr)
        {
            std::cerr << "Required audio decoder is not registered for ." << extension << "\\n";
            return 1;
        }
    }
    const auto root = std::filesystem::temp_directory_path() / "ppgu-library-import-tests";
    std::filesystem::remove_all(root);
    const juce::File source((root / "User Guitar Pack").string());
    const juce::File libraries((root / "PPGU Libraries").string());
    if (!source.createDirectory() || !libraries.createDirectory())
        return 1;

    juce::WavAudioFormat wav;
    juce::AiffAudioFormat aiff;
    if (!writeFixture(source.getChildFile("Guitar_C3_Vel100.wav"), wav, 24)
        || !writeFixture(source.getChildFile("Clean_D4.aiff"), aiff, 16)
        || !writeFixture(source.getChildFile("Unknown_take.wav"), wav, 24)
        || !source.getChildFile("Unsupported.ncw").replaceWithText("NCW placeholder"))
    {
        std::cerr << "Could not create audio import fixtures.\n";
        return 1;
    }

    ppgu::sampler::SampleLibraryImporter importer;
    const auto report = importer.importDirectory(source, libraries);
    if (!report.success || report.importedFiles != 3 || report.mappedFiles != 2
        || report.needsMapping != 1 || !report.generatedSfz.existsAsFile())
    {
        std::cerr << "Import report did not match expected files. Imported="
                  << report.importedFiles << " mapped=" << report.mappedFiles
                  << " needsMapping=" << report.needsMapping << "\n";
        for (const auto& warning : report.warnings) std::cerr << warning << "\n";
        for (const auto& error : report.errors) std::cerr << error << "\n";
        return 1;
    }

    const auto parsed = ppgu::sfz::parseFile(
        std::filesystem::path(report.generatedSfz.getFullPathName().toStdString()));
    if (!parsed.succeeded() || parsed.regions.size() != 2
        || !ppgu::sfz::validateSampleFiles(parsed).empty())
    {
        std::cerr << "Generated SFZ did not resolve converted sample files.\n";
        for (const auto& error : parsed.errors) std::cerr << error.message << "\n";
        return 1;
    }

    bool sawC3 = false, sawD4 = false;
    for (const auto& region : parsed.regions)
    {
        sawC3 = sawC3 || region.rootKey == 48;
        sawD4 = sawD4 || region.rootKey == 62;
        juce::AudioFormatManager formats;
        formats.registerBasicFormats();
        std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(juce::File(region.samplePath.string())));
        if (!reader || reader->bitsPerSample != 32 || !reader->usesFloatingPointData
            || reader->numChannels != 1 || std::abs(reader->sampleRate - 44100.0) > 0.01)
        {
            std::cerr << "Converted sample is not the expected 32-bit IEEE float WAV.\n";
            return 1;
        }
    }
    if (!sawC3 || !sawD4)
    {
        std::cerr << "Filename root-key auto mapping failed.\n";
        return 1;
    }

    const auto manifest = report.libraryDirectory.getChildFile("manifest.json");
    const auto manifestText = manifest.loadFileAsString();
    const auto sourceHash = juce::SHA256(source.getChildFile("Guitar_C3_Vel100.wav"))
                                .toHexString().toStdString();
    const auto convertedHash = juce::SHA256(
        juce::File(parsed.regions.front().samplePath.string())).toHexString().toStdString();
    bool warnedAboutNcw = false;
    for (const auto& warning : report.warnings)
        warnedAboutNcw = warnedAboutNcw || warning.find("NCW is not supported") != std::string::npos;
    if (!manifest.existsAsFile()
        || !manifestText.contains("USER PROVIDED - VERIFY RIGHTS")
        || !manifestText.contains("SHA-256")
        || !manifestText.contains(sourceHash)
        || !manifestText.contains(convertedHash)
        || !manifestText.contains("sourceSampleRate")
        || !manifestText.contains("durationSeconds")
        || !warnedAboutNcw)
    {
        std::cerr << "Manifest checksums/metadata, license warning or NCW rejection missing.\\n";
        return 1;
    }

    // Re-import creates a new unique library directory instead of overwriting
    // an existing bank, keeping the currently installed library untouched.
    const auto second = importer.importDirectory(source, libraries);
    if (!second.success || second.libraryDirectory == report.libraryDirectory)
    {
        std::cerr << "Repeated import overwrote or failed to isolate the existing bank.\n";
        return 1;
    }

    std::filesystem::remove_all(root);
    std::cout << "Sample library import tests passed.\n";
    return 0;
}
