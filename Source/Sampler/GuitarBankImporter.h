#pragma once
#include <juce_core/juce_core.h>
#include "SampleMetadataIndex.h"

namespace ppgu::sampler
{
struct GuitarBankImportReport
{
    bool success = false;
    juce::String bankId;
    juce::File bankDirectory;
    juce::File selectedSfz;
    int indexedWavFiles = 0;
    std::uint64_t totalBytes = 0;
    juce::StringArray warnings;
    juce::StringArray errors;
};

// Transactional WAV/SFZ folder importer. Call only from a worker thread.
// Original files are preserved; the JSON index is metadata-only and rebuildable.
class GuitarBankImporter final
{
public:
    [[nodiscard]] GuitarBankImportReport importFolder(
        const juce::File& sourceDirectory,
        const juce::File& userLibraryRoot,
        const juce::String& requestedBankName) const;
};
}
