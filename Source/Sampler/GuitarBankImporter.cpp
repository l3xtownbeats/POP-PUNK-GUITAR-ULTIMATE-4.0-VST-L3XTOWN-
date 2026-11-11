#include "GuitarBankImporter.h"
#include <algorithm>
#include <filesystem>
#include <limits>
#include <juce_core/juce_core.h>

namespace ppgu::sampler
{
namespace
{
bool isAudioOrInstrument(const juce::File& file)
{
    const auto ext = file.getFileExtension().toLowerCase();
    return ext == ".wav" || ext == ".sfz" || ext == ".aiff" || ext == ".aif"
        || ext == ".flac" || ext == ".ogg" || ext == ".mp3";
}

juce::String classifyCategory(const juce::File& file)
{
    const auto hint = (file.getParentDirectory().getFileName() + " "
        + file.getFileNameWithoutExtension()).toLowerCase();
    if (hint.contains("acoustic") || hint.contains("nylon") || hint.contains("steel-guitar")) return "Acoustic";
    if (hint.contains("palm") || hint.contains("mute") || hint.contains("muted")) return "Palm Mute";
    if (hint.contains("distortion") || hint.contains("dist") || hint.contains("high-gain")
        || hint.contains("metal") || hint.contains("heavy")) return "Distortion";
    if (hint.contains("crunch") || hint.contains("overdrive")) return "Crunch";
    if (hint.contains("drive") || hint.contains("lead") || hint.contains("amp")) return "Drive";
    if (hint.contains("clean") || hint.contains("direct")) return "Clean";
    if (hint.contains("bass")) return "Bass";
    return "Unsorted";
}
juce::String formatLabel(const juce::File& file)
{
    auto ext = file.getFileExtension().toLowerCase().removeCharacters(".");
    if (ext == "aif") ext = "aiff";
    return ext.toUpperCase();
}

juce::String safeName(juce::String value)
{
    value = value.trim().retainCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_ ");
    value = value.replaceCharacter(' ', '-').substring(0, 48);
    return value.isEmpty() ? "Imported-Guitar-Bank" : value;
}

bool isWithinDirectory(const std::filesystem::path& root, const std::filesystem::path& candidate)
{
    const auto relative = candidate.lexically_relative(root);
    if (relative.empty()) return candidate == root;
    if (relative.is_absolute()) return false;
    for (const auto& part : relative)
        if (part == "..") return false;
    return true;
}
}

GuitarBankImportReport GuitarBankImporter::importFolder(
    const juce::File& sourceDirectory, const juce::File& userLibraryRoot,
    const juce::String& requestedBankName) const
{
    GuitarBankImportReport report;
    if (!sourceDirectory.isDirectory() || !userLibraryRoot.isDirectory() && !userLibraryRoot.createDirectory())
    {
        report.errors.add("Source folder or user library directory is unavailable.");
        return report;
    }

    // Import only regular files physically contained by the selected root.
    // Do not recurse through directory symlinks or copy symlink targets.
    std::error_code scanError;
    const auto canonicalSourceRoot = std::filesystem::canonical(
        std::filesystem::path(sourceDirectory.getFullPathName().toStdString()), scanError);
    if (scanError)
    {
        report.errors.add("Could not resolve the selected source folder.");
        return report;
    }

    constexpr std::size_t maximumAssets = 50000;
    constexpr std::uint64_t maximumAssetBytes = 2ULL * 1024ULL * 1024ULL * 1024ULL;
    constexpr std::uint64_t maximumBankBytes = 20ULL * 1024ULL * 1024ULL * 1024ULL;
    juce::Array<juce::File> supported;
    std::uint64_t scannedTotalBytes = 0;
    const auto options = std::filesystem::directory_options::skip_permission_denied;
    std::filesystem::recursive_directory_iterator it(canonicalSourceRoot, options, scanError), end;
    for (; it != end; it.increment(scanError))
    {
        if (scanError)
        {
            report.warnings.add("A source folder entry could not be inspected and was skipped.");
            scanError.clear();
            continue;
        }

        const auto status = it->symlink_status(scanError);
        if (scanError)
        {
            report.warnings.add("A source asset could not be inspected and was skipped.");
            scanError.clear();
            continue;
        }
        if (std::filesystem::is_symlink(status))
        {
            if (std::filesystem::is_directory(it->status(scanError)))
                it.disable_recursion_pending();
            scanError.clear();
            continue;
        }
        if (!std::filesystem::is_regular_file(status))
            continue;

        const juce::File file(it->path().string());
        if (!isAudioOrInstrument(file))
            continue;
        if (supported.size() >= static_cast<int>(maximumAssets))
        {
            report.errors.add("Import stopped: the bank exceeds the 50,000 asset safety limit.");
            return report;
        }

        const auto canonicalAsset = std::filesystem::canonical(it->path(), scanError);
        if (scanError || !isWithinDirectory(canonicalSourceRoot, canonicalAsset))
        {
            report.warnings.add("An asset resolving outside the selected folder was skipped.");
            scanError.clear();
            continue;
        }
        const auto bytes = std::filesystem::file_size(canonicalAsset, scanError);
        if (scanError || bytes > maximumAssetBytes
            || bytes > maximumBankBytes - scannedTotalBytes)
        {
            report.errors.add("Import stopped: an asset is too large or the bank exceeds the 20 GiB safety limit.");
            return report;
        }
        scannedTotalBytes += bytes;
        supported.add(juce::File(canonicalAsset.string()));
    }
    if (supported.isEmpty())
    {
        report.errors.add("No supported guitar audio or SFZ files were found. Nothing was imported.");
        return report;
    }

    const auto bankId = juce::Uuid().toString().removeCharacters("{}-").toLowerCase();
    const auto name = safeName(requestedBankName);
    const auto staging = userLibraryRoot.getChildFile(".staging-" + bankId);
    const auto destination = userLibraryRoot.getChildFile(name + "-" + bankId.substring(0, 8));
    if (!staging.createDirectory())
    {
        report.errors.add("Could not create transactional staging directory.");
        return report;
    }

    juce::Array<juce::var> assets;
    int wavSourceCount = 0;
    juce::DynamicObject::Ptr categoryCounts = new juce::DynamicObject();
    juce::DynamicObject::Ptr formatCounts = new juce::DynamicObject();
    juce::File selectedSfz;
    std::uint64_t totalBytes = 0;
    bool failed = false;
    for (const auto& source : supported)
    {
        const auto sourcePath = std::filesystem::canonical(
            std::filesystem::path(source.getFullPathName().toStdString()), scanError);
        if (scanError || !isWithinDirectory(canonicalSourceRoot, sourcePath))
        {
            report.errors.add("An imported asset changed or escaped the selected source folder during import.");
            failed = true;
            scanError.clear();
            break;
        }
        const auto relativePath = sourcePath.lexically_relative(canonicalSourceRoot);
        if (relativePath.empty() || relativePath.is_absolute())
        {
            report.errors.add("Could not safely resolve an asset's relative path.");
            failed = true;
            break;
        }
        juce::String relative(relativePath.generic_string());
        const auto target = staging.getChildFile(relative);
        if (!target.getParentDirectory().createDirectory() || !source.copyFileTo(target))
        {
            report.errors.add("Could not preserve source file: " + source.getFileName());
            failed = true;
            break;
        }
        const auto bytes = target.getSize();
        if (bytes < 0 || static_cast<std::uint64_t>(bytes) > UINT64_MAX - totalBytes
            || static_cast<std::uint64_t>(bytes) > maximumBankBytes - totalBytes)
        {
            report.errors.add("Imported bank size overflow.");
            failed = true;
            break;
        }
        totalBytes += static_cast<std::uint64_t>(bytes);
        juce::DynamicObject::Ptr asset = new juce::DynamicObject();
        asset->setProperty("path", relative.replaceCharacter('\\', '/'));
        asset->setProperty("extension", source.getFileExtension().toLowerCase());
        asset->setProperty("bytes", static_cast<juce::int64>(bytes));
        asset->setProperty("md5", juce::MD5(target).toHexString());
        asset->setProperty("sourceName", source.getFileName());
        if (source.hasFileExtension("wav")) ++wavSourceCount;
        const auto category = classifyCategory(source);
        const auto format = formatLabel(source);
        asset->setProperty("category", category);
        asset->setProperty("format", format);
        const bool currentPlaybackFormat = source.hasFileExtension("wav") || source.hasFileExtension("sfz");
        asset->setProperty("playbackStatus", currentPlaybackFormat
            ? "supported-by-current-loader" : "indexed-decoder-capability-pending");
        categoryCounts->setProperty(category, static_cast<int>(categoryCounts->getProperty(category)) + 1);
        formatCounts->setProperty(format, static_cast<int>(formatCounts->getProperty(format)) + 1);
        assets.add(juce::var(asset.get()));
        if (source.getFileExtension().equalsIgnoreCase(".sfz") && selectedSfz == juce::File{})
            selectedSfz = target;
    }

    if (!failed)
    {
        SampleMetadataIndex index;
        const auto indexed = index.scanDirectory(staging);
        for (const auto& warning : indexed.warnings) report.warnings.add(warning);
        report.indexedWavFiles = static_cast<int>(indexed.samples.size());
        if (!indexed.success && wavSourceCount > 0)
        {
            for (const auto& error : indexed.errors) report.errors.add(error);
            report.errors.add("The bank contains WAV files, but none passed playback metadata validation.");
            failed = true;
        }
        else
        {
            if (wavSourceCount == 0)
                report.warnings.add("Assets were catalogued, but this build has no verified WAV playback asset in the imported bank.");
            juce::DynamicObject::Ptr manifest = new juce::DynamicObject();
            manifest->setProperty("schemaVersion", 1);
            manifest->setProperty("bankId", bankId);
            manifest->setProperty("name", name);
            manifest->setProperty("createdUtc", juce::Time::getCurrentTime().toISO8601(true));
            manifest->setProperty("sourceRootName", sourceDirectory.getFileName());
            manifest->setProperty("assetCount", assets.size());
            manifest->setProperty("wavCount", report.indexedWavFiles);
            manifest->setProperty("totalBytes", static_cast<juce::int64>(totalBytes));
            manifest->setProperty("assets", juce::var(assets));
            manifest->setProperty("categoryCounts", juce::var(categoryCounts.get()));
            manifest->setProperty("formatCounts", juce::var(formatCounts.get()));
            manifest->setProperty("organization", "logical-categories-and-formats; original-relative-paths-preserved");
            manifest->setProperty("categoryRuleVersion", 1);
            const auto manifestFile = staging.getChildFile("bank.json");
            if (!manifestFile.replaceWithText(juce::JSON::toString(juce::var(manifest.get()), true)))
            {
                report.errors.add("Could not write bank manifest.");
                failed = true;
            }
        }
    }

    if (failed)
    {
        staging.deleteRecursively();
        return report;
    }

    if (destination.exists() || !staging.moveFileTo(destination))
    {
        staging.deleteRecursively();
        report.errors.add("Could not atomically publish the imported guitar bank.");
        return report;
    }

    report.success = true;
    report.bankId = bankId;
    report.bankDirectory = destination;
    if (selectedSfz != juce::File{})
        report.selectedSfz = destination.getChildFile(selectedSfz.getRelativePathFrom(staging));
    report.totalBytes = totalBytes;
    return report;
}
}
