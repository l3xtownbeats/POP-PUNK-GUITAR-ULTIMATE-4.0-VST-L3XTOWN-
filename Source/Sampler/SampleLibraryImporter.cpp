#include "SampleLibraryImporter.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <vector>
#include <limits>
#include <regex>
#include <system_error>

namespace ppgu::sampler
{
namespace
{
bool isWithin(const std::filesystem::path& root, const std::filesystem::path& candidate)
{
    const auto relative = candidate.lexically_relative(root);
    if (relative.empty()) return candidate == root;
    if (relative.is_absolute()) return false;
    for (const auto& part : relative)
        if (part == "..") return false;
    return true;
}

std::string jsonEscape(const std::string& value)
{
    std::string result;
    result.reserve(value.size() + 8);
    for (const unsigned char c : value)
    {
        switch (c)
        {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: if (c >= 0x20) result.push_back(static_cast<char>(c)); break;
        }
    }
    return result;
}

std::string safeName(std::string name)
{
    for (auto& c : name)
    {
        const auto u = static_cast<unsigned char>(c);
        if (!(std::isalnum(u) || c == '-' || c == '_' || c == ' '))
            c = '_';
    }
    while (!name.empty() && name.front() == ' ') name.erase(name.begin());
    while (!name.empty() && name.back() == ' ') name.pop_back();
    return name.empty() ? "Imported_Guitar_Library" : name;
}

int noteFromFilename(const std::string& filename)
{
    static const std::regex notePattern(R"(([A-Ga-g])([#b]?)(-?[0-9]))");
    static const std::array<int, 7> natural { 9, 11, 0, 2, 4, 5, 7 };
    std::smatch match;
    if (!std::regex_search(filename, match, notePattern))
        return -1;

    const char letter = static_cast<char>(std::toupper(static_cast<unsigned char>(match[1].str()[0])));
    const int letterIndex = letter - 'A';
    int semitone = natural[static_cast<std::size_t>(letterIndex)];
    const auto accidental = match[2].str();
    if (accidental == "#") ++semitone;
    if (accidental == "b") --semitone;
    const int octave = std::stoi(match[3].str());
    const int midi = (octave + 1) * 12 + semitone;
    return midi >= 0 && midi <= 127 ? midi : -1;
}

enum class ImportedPlayMode { melody, pattern };

ImportedPlayMode classifyPlayMode(const std::filesystem::path& relativePath)
{
    // Explicit naming hints classify riffs, loops and strumming material as PATTERN.
    // Unlabelled one-shots default to MELODY and are marked for user review in the manifest.
    std::string name = relativePath.generic_string();
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    static const std::array<const char*, 12> terms {
        "pattern", "riff", "strum", "strumming", "chord", "chords",
        "loop", "groove", "phrase", "articulation", "rhythm", "beat"
    };
    for (const auto* term : terms)
        if (name.find(term) != std::string::npos) return ImportedPlayMode::pattern;
    return ImportedPlayMode::melody;
}

const char* playModeName(ImportedPlayMode mode)
{
    return mode == ImportedPlayMode::pattern ? "PATTERN" : "MELODY";
}
bool convertToFloatWav(juce::AudioFormatReader& reader, const juce::File& output,
                       std::string& error)
{
    if (reader.lengthInSamples <= 0 || reader.numChannels == 0
        || reader.numChannels > SampleLibraryImporter::maximumChannels
        || !std::isfinite(reader.sampleRate)
        || reader.sampleRate < SampleLibraryImporter::minimumSampleRate
        || reader.sampleRate > SampleLibraryImporter::maximumSampleRate)
    {
        error = "Unsupported audio metadata (duration, sample rate or channel count).";
        return false;
    }

    const double duration = static_cast<double>(reader.lengthInSamples) / reader.sampleRate;
    if (!std::isfinite(duration) || duration > SampleLibraryImporter::maximumSampleDurationSeconds)
    {
        error = "Audio file exceeds the 15-minute per-file import limit.";
        return false;
    }

    const auto channels = static_cast<std::uint64_t>(reader.numChannels);
    const auto frames = static_cast<std::uint64_t>(reader.lengthInSamples);
    if (frames > (std::numeric_limits<std::uint32_t>::max() - 128ULL) / (channels * sizeof(float)))
    {
        error = "Converted WAV would exceed the classic RIFF/WAV size limit.";
        return false;
    }

    if (!output.getParentDirectory().createDirectory())
    {
        error = "Could not create output sample directory.";
        return false;
    }

    const auto sampleRate = static_cast<std::uint32_t>(std::llround(reader.sampleRate));
    const auto channelCount = static_cast<std::uint16_t>(reader.numChannels);
    const auto dataBytes = static_cast<std::uint32_t>(frames * channels * sizeof(float));
    const auto byteRate = sampleRate * static_cast<std::uint32_t>(channelCount) * 4U;
    const auto blockAlign = static_cast<std::uint16_t>(channelCount * 4U);

    // JUCE 7.0.12 does not expose the newer floating-point WAV writer option.
    // Write a standards-compliant WAVE_FORMAT_IEEE_FLOAT (format tag 3) file
    // directly so imported samples are genuinely 32-bit float, not PCM32.
    std::ofstream file(output.getFullPathName().toStdString(),
                       std::ios::binary | std::ios::trunc);
    if (!file)
    {
        error = "Could not create converted WAV file.";
        return false;
    }

    const auto writeU16 = [&file](std::uint16_t value)
    {
        const char bytes[2] { static_cast<char>(value & 0xffU),
                              static_cast<char>((value >> 8U) & 0xffU) };
        file.write(bytes, 2);
    };
    const auto writeU32 = [&file](std::uint32_t value)
    {
        const char bytes[4] { static_cast<char>(value & 0xffU),
                              static_cast<char>((value >> 8U) & 0xffU),
                              static_cast<char>((value >> 16U) & 0xffU),
                              static_cast<char>((value >> 24U) & 0xffU) };
        file.write(bytes, 4);
    };

    file.write("RIFF", 4);
    writeU32(36U + dataBytes);
    file.write("WAVEfmt ", 8);
    writeU32(16U);
    writeU16(3U); // WAVE_FORMAT_IEEE_FLOAT
    writeU16(channelCount);
    writeU32(sampleRate);
    writeU32(byteRate);
    writeU16(blockAlign);
    writeU16(32U);
    file.write("data", 4);
    writeU32(dataBytes);

    juce::AudioBuffer<float> chunk(static_cast<int>(reader.numChannels),
                                   static_cast<int>(SampleLibraryImporter::conversionChunkFrames));
    std::vector<std::uint8_t> interleaved(
        static_cast<std::size_t>(SampleLibraryImporter::conversionChunkFrames)
        * static_cast<std::size_t>(reader.numChannels) * sizeof(float));
    juce::int64 position = 0;
    while (position < reader.lengthInSamples)
    {
        const int count = static_cast<int>(std::min<juce::int64>(
            SampleLibraryImporter::conversionChunkFrames, reader.lengthInSamples - position));
        chunk.clear(0, count);
        if (!reader.read(&chunk, 0, count, position, true, true))
        {
            error = "Decoder failed while reading source audio.";
            file.close();
            output.deleteFile();
            return false;
        }

        std::size_t offset = 0;
        for (int frame = 0; frame < count; ++frame)
        {
            for (unsigned int channel = 0; channel < reader.numChannels; ++channel)
            {
                const auto bits = std::bit_cast<std::uint32_t>(
                    chunk.getSample(static_cast<int>(channel), frame));
                interleaved[offset++] = static_cast<std::uint8_t>(bits & 0xffU);
                interleaved[offset++] = static_cast<std::uint8_t>((bits >> 8U) & 0xffU);
                interleaved[offset++] = static_cast<std::uint8_t>((bits >> 16U) & 0xffU);
                interleaved[offset++] = static_cast<std::uint8_t>((bits >> 24U) & 0xffU);
            }
        }
        file.write(reinterpret_cast<const char*>(interleaved.data()),
                   static_cast<std::streamsize>(offset));
        if (!file)
        {
            error = "WAV encoder failed while writing converted audio.";
            file.close();
            output.deleteFile();
            return false;
        }
        position += count;
    }

    file.close();
    return file.good() && output.existsAsFile() && output.getSize() == 44 + dataBytes;
}
}

LibraryImportReport SampleLibraryImporter::importDirectory(const juce::File& sourceDirectory,
                                                            const juce::File& librariesRoot) const
{
    LibraryImportReport report;
    if (!sourceDirectory.isDirectory())
    {
        report.errors.emplace_back("Selected source is not a directory.");
        return report;
    }

    std::error_code ec;
    const auto sourceRoot = std::filesystem::canonical(
        std::filesystem::path(sourceDirectory.getFullPathName().toStdString()), ec);
    if (ec)
    {
        report.errors.emplace_back("Could not resolve source directory: " + ec.message());
        return report;
    }

    if (!librariesRoot.createDirectory())
    {
        report.errors.emplace_back("Could not create PPGU Libraries directory.");
        return report;
    }

    juce::AudioFormatManager formats;
    formats.registerBasicFormats();

    std::vector<std::filesystem::path> candidates;
    const auto options = std::filesystem::directory_options::skip_permission_denied;
    std::filesystem::recursive_directory_iterator it(sourceRoot, options, ec), end;
    for (; it != end; it.increment(ec))
    {
        if (ec)
        {
            report.warnings.emplace_back("Skipped inaccessible directory entry: " + ec.message());
            ec.clear();
            continue;
        }
        const auto status = it->symlink_status(ec);
        if (ec) { ec.clear(); continue; }
        if (std::filesystem::is_symlink(status))
        {
            if (std::filesystem::is_directory(it->status(ec)))
                it.disable_recursion_pending();
            ec.clear();
            continue;
        }
        if (!std::filesystem::is_regular_file(status))
            continue;

        const auto canonical = std::filesystem::canonical(it->path(), ec);
        if (ec || !isWithin(sourceRoot, canonical))
        {
            report.warnings.emplace_back("Skipped file outside source root.");
            ec.clear();
            continue;
        }
        candidates.push_back(canonical);
        if (candidates.size() > maximumFiles)
        {
            report.errors.emplace_back("Library exceeds the 4096-file import safety limit.");
            return report;
        }
    }

    std::sort(candidates.begin(), candidates.end());
    if (candidates.empty())
    {
        report.errors.emplace_back("No regular files were found in the selected directory.");
        return report;
    }

    const auto displayName = safeName(sourceDirectory.getFileName().toStdString());
    juce::File finalDirectory = librariesRoot.getChildFile(juce::String::fromUTF8(displayName.c_str()));
    for (int suffix = 2; finalDirectory.exists(); ++suffix)
        finalDirectory = librariesRoot.getChildFile(juce::String::fromUTF8((displayName + "_" + std::to_string(suffix)).c_str()));

    const auto stagingDirectory = librariesRoot.getChildFile(
        juce::String(".") + juce::String::fromUTF8(displayName.c_str()) + ".staging-" + juce::Uuid().toString());
    if (!stagingDirectory.createDirectory())
    {
        report.errors.emplace_back("Could not create isolated import staging directory.");
        return report;
    }
    const auto samplesDirectory = stagingDirectory.getChildFile("samples");
    if (!samplesDirectory.createDirectory())
    {
        stagingDirectory.deleteRecursively();
        report.errors.emplace_back("Could not create staging samples directory.");
        return report;
    }

    std::string sfz = "// PPGU imported library map. Unmapped files remain in manifest for manual mapping.\n";
    std::string manifest = "{\n  \"schemaVersion\": 1,\n  \"displayName\": \"" +
        jsonEscape(displayName) + "\",\n  \"internalFormat\": \"WAV IEEE Float 32-bit\",\n"
        "  \"sampleRatePolicy\": \"preserve source rate\",\n"
        "  \"licenseStatus\": \"USER PROVIDED - VERIFY RIGHTS BEFORE REDISTRIBUTION\",\n"
        "  \"samples\": [\n";
    bool firstManifestEntry = true;

    for (const auto& candidate : candidates)
    {
        const juce::File source(candidate.string());
        std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(source));
        if (!reader)
        {
            auto extension = source.getFileExtension().toLowerCase();
            const auto filename = source.getFileName().toStdString();
            if (extension == ".ncw")
                report.warnings.emplace_back(filename + ": NCW is not supported and was skipped.");
            else
                report.warnings.emplace_back(filename
                    + ": unsupported or unreadable audio. Enabled import formats: WAV, AIFF/AIF, FLAC, OGG Vorbis, MP3.");
            continue;
        }

        const auto relative = std::filesystem::relative(candidate, sourceRoot, ec);
        if (ec || relative.empty() || relative.is_absolute())
        {
            report.warnings.emplace_back("Could not safely derive relative path for " + source.getFileName().toStdString());
            ec.clear();
            continue;
        }

        auto outputRelative = relative;
        outputRelative.replace_extension(".wav");
        juce::File output = samplesDirectory.getChildFile(outputRelative.generic_string());
        // A collision can occur when source files differ only by extension.
        if (output.existsAsFile())
        {
            output = output.getSiblingFile(output.getFileNameWithoutExtension()
                + "_" + juce::String(static_cast<int>(report.importedFiles + 1)) + ".wav");
        }

        std::string conversionError;
        if (!convertToFloatWav(*reader, output, conversionError))
        {
            report.warnings.emplace_back(source.getFileName().toStdString() + ": " + conversionError);
            continue;
        }

        const auto sourceBytes = static_cast<std::uint64_t>(source.getSize());
        const auto outputBytes = static_cast<std::uint64_t>(output.getSize());
        report.sourceBytes += sourceBytes;
        report.convertedBytes += outputBytes;
        ++report.importedFiles;

        const auto playMode = classifyPlayMode(relative);
        const int rootKey = noteFromFilename(source.getFileNameWithoutExtension().toStdString());
        if (rootKey >= 0)
        {
            const auto sfzPath = std::filesystem::relative(
                std::filesystem::path(output.getFullPathName().toStdString()),
                std::filesystem::path(stagingDirectory.getFullPathName().toStdString()), ec);
            if (!ec)
            {
                sfz += "<region> sample=\"" + sfzPath.generic_string()
                    + "\" key=" + std::to_string(rootKey)
                    + " lokey=" + std::to_string(rootKey)
                    + " hikey=" + std::to_string(rootKey) + "\n";
                ++report.mappedFiles;
            }
            else ec.clear();
        }
        else
        {
            ++report.needsMapping;
            report.warnings.emplace_back(source.getFileName().toStdString()
                + ": note/root key not found in filename; imported but left for manual mapping.");
        }

        if (!firstManifestEntry) manifest += ",\n";
        firstManifestEntry = false;
        manifest += "    {\"source\": \"" + jsonEscape(relative.generic_string())
            + "\", \"converted\": \"" + jsonEscape(
                std::filesystem::relative(std::filesystem::path(output.getFullPathName().toStdString()),
                                          std::filesystem::path(stagingDirectory.getFullPathName().toStdString()), ec).generic_string())
            + "\", \"mode\": \"" + playModeName(playMode) + "\""
            + ", \"classification\": \"" + (playMode == ImportedPlayMode::pattern ? "name_hint" : "default_review") + "\""
            + ", \"rootKey\": " + (rootKey >= 0 ? std::to_string(rootKey) : "null")
            + ", \"sourceBytes\": " + std::to_string(sourceBytes)
            + ", \"convertedBytes\": " + std::to_string(outputBytes) + "}";
        ec.clear();
    }
    manifest += "\n  ]\n}\n";

    if (report.importedFiles == 0)
    {
        stagingDirectory.deleteRecursively();
        report.errors.emplace_back("No supported audio files could be converted. Supported formats depend on the JUCE audio decoders enabled in this build.");
        return report;
    }

    const auto manifestFile = stagingDirectory.getChildFile("manifest.json");
    if (!manifestFile.replaceWithText(manifest))
    {
        stagingDirectory.deleteRecursively();
        report.errors.emplace_back("Could not write library manifest.");
        return report;
    }

    if (report.mappedFiles > 0)
    {
        const auto sfzFile = stagingDirectory.getChildFile("Imported_Instrument.sfz");
        if (!sfzFile.replaceWithText(sfz))
        {
            stagingDirectory.deleteRecursively();
            report.errors.emplace_back("Could not write generated SFZ mapping.");
            return report;
        }
        report.generatedSfz = sfzFile;
    }

    const auto stagingPath = std::filesystem::path(stagingDirectory.getFullPathName().toStdString());
    const auto finalPath = std::filesystem::path(finalDirectory.getFullPathName().toStdString());
    std::filesystem::rename(stagingPath, finalPath, ec);
    if (ec)
    {
        stagingDirectory.deleteRecursively();
        report.errors.emplace_back("Could not atomically publish imported library: " + ec.message());
        return report;
    }

    report.libraryDirectory = finalDirectory;
    // The staging path is invalid after the atomic directory rename. Rebuild the
    // published SFZ path from the final library directory instead of probing the
    // now-moved staging file (which would leave callers with a stale path).
    if (report.mappedFiles > 0)
        report.generatedSfz = finalDirectory.getChildFile("Imported_Instrument.sfz");
    report.success = true;
    return report;
}
}
