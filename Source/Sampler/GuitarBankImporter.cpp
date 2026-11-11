#include "GuitarBankImporter.h"
#include <algorithm>
#include <filesystem>

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
juce::String safeName(juce::String value)
{
    value = value.trim().retainCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_ ");
    value = value.replaceCharacter(' ', '-').substring(0, 48);
    return value.isEmpty() ? "Imported-Guitar-Bank" : value;
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

    juce::Array<juce::File> candidates;
    sourceDirectory.findChildFiles(candidates, juce::File::findFiles, true);
    juce::Array<juce::File> supported;
    for (const auto& file : candidates)
        if (isAudioOrInstrument(file))
            supported.add(file);
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
    juce::File selectedSfz;
    std::uint64_t totalBytes = 0;
    bool failed = false;
    for (const auto& source : supported)
    {
        const auto relative = source.getRelativePathFrom(sourceDirectory);
        const auto target = staging.getChildFile(relative);
        if (!target.getParentDirectory().createDirectory() || !source.copyFileTo(target))
        {
            report.errors.add("Could not preserve source file: " + source.getFileName());
            failed = true;
            break;
        }
        const auto bytes = target.getSize();
        if (bytes < 0 || static_cast<std::uint64_t>(bytes) > UINT64_MAX - totalBytes)
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
        asset->setProperty("sha256", juce::SHA256(target).toHexString());
        asset->setProperty("sourceName", source.getFileName());
        assets.add(juce::var(asset.get()));
        if (source.getFileExtension().equalsIgnoreCase(".sfz") && selectedSfz == juce::File{})
            selectedSfz = target;
    }

    if (!failed)
    {
        SampleMetadataIndex index;
        const auto indexed = index.scanDirectory(staging);
        report.warnings.addArray(juce::StringArray(indexed.warnings.begin(), indexed.warnings.end()));
        report.indexedWavFiles = static_cast<int>(indexed.samples.size());
        if (!indexed.success)
        {
            report.errors.addArray(juce::StringArray(indexed.errors.begin(), indexed.errors.end()));
            report.errors.add("The copied bank contains no valid WAV playback assets.");
            failed = true;
        }
        else
        {
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
