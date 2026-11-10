#include "SampleMetadataIndex.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <system_error>

namespace ppgu::sampler
{
namespace
{
bool isWithinRoot(const std::filesystem::path& root,
                  const std::filesystem::path& candidate)
{
    const auto relative = candidate.lexically_relative(root);
    if (relative.empty()) return candidate == root;
    if (relative.is_absolute()) return false;
    for (const auto& part : relative)
        if (part == "..") return false;
    return true;
}

bool isWav(const std::filesystem::path& path)
{
    return juce::String(path.extension().string()).equalsIgnoreCase(".wav");
}
}

SampleIndexReport SampleMetadataIndex::scanDirectory(const juce::File& directory) const
{
    SampleIndexReport report;
    if (!directory.isDirectory())
    {
        report.errors.emplace_back("Sample library directory does not exist or is not a directory.");
        return report;
    }

    std::error_code ec;
    const auto root = std::filesystem::canonical(
        std::filesystem::path(directory.getFullPathName().toStdString()), ec);
    if (ec)
    {
        report.errors.emplace_back("Could not resolve sample library directory: " + ec.message());
        return report;
    }

    juce::AudioFormatManager formats;
    formats.registerBasicFormats();

    // Do not follow directory symlinks: a library scan must not escape its
    // selected root or unexpectedly traverse mounted/network locations.
    const auto options = std::filesystem::directory_options::skip_permission_denied;
    std::filesystem::recursive_directory_iterator it(root, options, ec), end;
    for (; it != end; it.increment(ec))
    {
        if (ec)
        {
            report.warnings.emplace_back("Directory entry could not be inspected: " + ec.message());
            ec.clear();
            continue;
        }

        const auto entryPath = it->path();
        const auto symlinkStatus = it->symlink_status(ec);
        if (ec)
        {
            report.warnings.emplace_back("Could not inspect entry: " + entryPath.string());
            ec.clear();
            continue;
        }

        if (std::filesystem::is_symlink(symlinkStatus))
        {
            if (std::filesystem::is_directory(it->status(ec)))
                it.disable_recursion_pending();
            ec.clear();
            continue;
        }

        if (!std::filesystem::is_regular_file(symlinkStatus) || !isWav(entryPath))
            continue;

        const auto canonical = std::filesystem::canonical(entryPath, ec);
        if (ec || !isWithinRoot(root, canonical))
        {
            report.warnings.emplace_back("Skipped sample outside library root: " + entryPath.string());
            ec.clear();
            continue;
        }

        const auto bytes = std::filesystem::file_size(canonical, ec);
        if (ec)
        {
            report.warnings.emplace_back("Could not read sample file size: " + canonical.string());
            ec.clear();
            continue;
        }

        auto reader = std::unique_ptr<juce::AudioFormatReader>(
            formats.createReaderFor(juce::File(canonical.string())));
        if (reader == nullptr)
        {
            report.warnings.emplace_back("Unreadable WAV header: " + canonical.string());
            continue;
        }

        if (!std::isfinite(reader->sampleRate) || reader->sampleRate < 8000.0
            || reader->sampleRate > 384000.0 || reader->lengthInSamples <= 0
            || reader->numChannels == 0 || reader->numChannels > 64)
        {
            report.warnings.emplace_back("Invalid WAV metadata: " + canonical.string());
            continue;
        }

        SampleMetadata metadata;
        metadata.file = juce::File(canonical.string());
        metadata.frames = static_cast<std::uint64_t>(reader->lengthInSamples);
        metadata.sampleRate = reader->sampleRate;
        metadata.channels = reader->numChannels;
        metadata.bitsPerSample = reader->bitsPerSample;
        metadata.floatingPoint = reader->usesFloatingPointData;
        metadata.fileBytes = bytes;
        metadata.modificationTimeMs = juce::File(canonical.string()).getLastModificationTime().toMilliseconds();

        report.totalFileBytes += bytes;
        report.totalFrames += metadata.frames;
        report.samples.push_back(std::move(metadata));
    }

    std::sort(report.samples.begin(), report.samples.end(),
        [](const SampleMetadata& a, const SampleMetadata& b)
        {
            return a.file.getFullPathName().compareIgnoreCase(b.file.getFullPathName()) < 0;
        });

    report.success = !report.samples.empty() && report.errors.empty();
    if (report.samples.empty())
        report.warnings.emplace_back("No readable WAV samples were found in this directory.");
    return report;
}
}
