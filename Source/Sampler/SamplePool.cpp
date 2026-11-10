#include "SamplePool.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <limits>
#include <set>
#include <tuple>

namespace ppgu::sampler
{
namespace
{
std::string pathKey(const std::filesystem::path& path)
{
#if JUCE_WINDOWS
    return juce::String(path.generic_string()).toLowerCase().toStdString();
#else
    return path.generic_string();
#endif
}

bool isWithinDirectory(const std::filesystem::path& root, const std::filesystem::path& candidate)
{
    const auto relative = candidate.lexically_relative(root);
    if (relative.empty()) return candidate == root;
    if (relative.is_absolute()) return false;
    return *relative.begin() != "..";
}

// Кубическая интерполяция Hermite (Catmull–Rom). Индексы ограничены
// границами региона, поэтому чтение не выходит за декодированный WAV.
float hermiteSample(const float* data, std::uint64_t frame, float fraction,
                    std::uint64_t regionStart, std::uint64_t regionEnd) noexcept
{
    const auto lastFrame = regionEnd - 1;
    const auto i0 = frame > regionStart ? frame - 1 : regionStart;
    const auto i1 = std::clamp(frame, regionStart, lastFrame);
    const auto i2 = std::clamp(frame + 1, regionStart, lastFrame);
    const auto i3 = std::clamp(frame + 2, regionStart, lastFrame);
    const float y0 = data[static_cast<std::size_t>(i0)];
    const float y1 = data[static_cast<std::size_t>(i1)];
    const float y2 = data[static_cast<std::size_t>(i2)];
    const float y3 = data[static_cast<std::size_t>(i3)];
    const float t2 = fraction * fraction;
    const float t3 = t2 * fraction;
    return 0.5f * ((2.0f * y1)
        + (-y0 + y2) * fraction
        + (2.0f * y0 - 5.0f * y1 + 4.0f * y2 - y3) * t2
        + (-y0 + 3.0f * y1 - 3.0f * y2 + y3) * t3);
}
}

SamplePool::SamplePool() = default;

LoadReport SamplePool::loadSfz(const juce::File& sfzFile,
                               const std::atomic<std::uint64_t>* requestGeneration,
                               std::uint64_t expectedGeneration)
{
    LoadReport report;
    std::lock_guard<std::mutex> guard(loaderMutex);
    const auto requestWasSuperseded = [&]() noexcept
    {
        return requestGeneration != nullptr
            && requestGeneration->load(std::memory_order_acquire) != expectedGeneration;
    };
    const auto abortIfSuperseded = [&]() 
    {
        if (!requestWasSuperseded())
            return false;
        report.errors.emplace_back("SFZ load superseded by a newer bank request.");
        return true;
    };
    if (abortIfSuperseded())
        return report;

    if (!sfzFile.existsAsFile())
    {
        report.errors.emplace_back("SFZ file does not exist: " + sfzFile.getFullPathName().toStdString());
        return report;
    }

    const auto parsed = ppgu::sfz::parseFile(std::filesystem::path(sfzFile.getFullPathName().toStdString()));
    for (const auto& error : parsed.errors)
        report.errors.push_back("Line " + std::to_string(error.line) + ": " + error.message);
    for (const auto& warning : parsed.warnings)
        report.warnings.push_back("Line " + std::to_string(warning.line) + ": " + warning.message);
    if (!parsed.succeeded() || abortIfSuperseded())
        return report;

    if (snapshots.size() >= maximumSnapshots)
    {
        report.errors.emplace_back("Sampler snapshot limit reached. Restart the plugin before loading more banks.");
        return report;
    }

    std::error_code ec;
    const auto root = std::filesystem::weakly_canonical(
        std::filesystem::path(sfzFile.getParentDirectory().getFullPathName().toStdString()), ec);
    if (ec)
    {
        report.errors.emplace_back("Could not resolve SFZ library directory: " + ec.message());
        return report;
    }
    const auto rootKey = pathKey(root);

    StreamingLibrary* library = nullptr;
    for (const auto& existing : streamingLibraries)
        if (existing->rootKey == rootKey)
        {
            library = existing.get();
            break;
        }

    std::unique_ptr<StreamingLibrary> pendingLibrary;
    if (library == nullptr)
    {
        if (streamingLibraries.size() >= maximumStreamingLibraries)
        {
            report.errors.emplace_back("Streaming library limit reached (8 library roots). Close/restart the plugin before loading another library.");
            return report;
        }

        SampleMetadataIndex index;
        auto indexed = index.scanDirectory(juce::File(root.string()));
        if (abortIfSuperseded())
            return report;
        report.warnings.insert(report.warnings.end(), indexed.warnings.begin(), indexed.warnings.end());
        if (!indexed.success)
        {
            report.errors.insert(report.errors.end(), indexed.errors.begin(), indexed.errors.end());
            if (report.errors.empty())
                report.errors.emplace_back("No supported WAV files were found in the SFZ library directory.");
            return report;
        }
        if (indexed.samples.size() > static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max()))
        {
            report.errors.emplace_back("The sample library contains more WAV files than the streaming index can address.");
            return report;
        }

        pendingLibrary = std::make_unique<StreamingLibrary>();
        pendingLibrary->rootKey = rootKey;
        pendingLibrary->metadata = std::move(indexed.samples);
        for (std::size_t i = 0; i < pendingLibrary->metadata.size(); ++i)
            pendingLibrary->sampleIds.emplace(
                pathKey(std::filesystem::path(pendingLibrary->metadata[i].file.getFullPathName().toStdString())),
                static_cast<std::uint32_t>(i));

        // 256 planar pages * 4096 frames * stereo floats = 8 MiB per
        // library root. The eight-root limit therefore bounds these default
        // sampler caches to 64 MiB in aggregate.
        pendingLibrary->service = std::make_unique<SampleStreamingService>(
            pendingLibrary->metadata, streamingCacheSlots, streamingPageFrames);
        if (!pendingLibrary->service->isValid())
        {
            report.errors.emplace_back("Could not allocate the bounded streaming page cache for this library.");
            return report;
        }
        library = pendingLibrary.get();
    }

    auto candidateSnapshot = std::make_unique<Snapshot>();
    candidateSnapshot->regions.reserve(parsed.regions.size());
    using RoundRobinKey = std::tuple<int, int, int, int, int, int, int, int>;
    std::map<RoundRobinKey, int> roundRobinSetIndices;
    std::set<std::string> samplesUsed;

    for (const auto& region : parsed.regions)
    {
        ec.clear();
        auto sourceSamplePath = region.samplePath;

        // Meta Open Sound SFZ maps may refer to OGG preview names while the
        // production WAV release stores same-stem files in a sibling wav/.
        if (juce::String(sourceSamplePath.extension().string()).toLowerCase() == ".ogg")
        {
            auto wavName = sourceSamplePath.filename();
            wavName.replace_extension(".wav");
            const std::array<std::filesystem::path, 3> candidates {
                sourceSamplePath.parent_path() / wavName,
                root / "wav" / wavName,
                root / wavName
            };

            bool resolvedWav = false;
            for (const auto& candidate : candidates)
            {
                ec.clear();
                if (std::filesystem::is_regular_file(candidate, ec) && !ec)
                {
                    sourceSamplePath = candidate;
                    resolvedWav = true;
                    break;
                }
            }

            if (!resolvedWav)
            {
                std::filesystem::path uniqueMatch;
                bool ambiguous = false;
                ec.clear();
                for (std::filesystem::recursive_directory_iterator it(
                         root, std::filesystem::directory_options::skip_permission_denied, ec), finish;
                     it != finish && !ec; it.increment(ec))
                {
                    if (it->is_regular_file(ec) && !ec && it->path().filename() == wavName)
                    {
                        if (!uniqueMatch.empty() && uniqueMatch != it->path())
                        {
                            ambiguous = true;
                            break;
                        }
                        uniqueMatch = it->path();
                    }
                }
                if (!ambiguous && !uniqueMatch.empty())
                {
                    sourceSamplePath = uniqueMatch;
                    resolvedWav = true;
                }
                else if (ambiguous)
                {
                    report.errors.push_back("Ambiguous WAV fallback for SFZ OGG reference: " + region.samplePath.string());
                    continue;
                }
            }

            if (!resolvedWav)
            {
                report.errors.push_back("SFZ references an OGG sample but no same-name WAV source was found: "
                                        + region.samplePath.string());
                continue;
            }
        }

        ec.clear();
        const auto canonicalSample = std::filesystem::weakly_canonical(sourceSamplePath, ec);
        if (ec || !std::filesystem::is_regular_file(canonicalSample, ec)
            || ec || !isWithinDirectory(root, canonicalSample))
        {
            report.errors.push_back("Sample path is missing or escapes the library directory: " + sourceSamplePath.string());
            continue;
        }

        const auto key = pathKey(canonicalSample);
        const auto indexedSample = library->sampleIds.find(key);
        if (indexedSample == library->sampleIds.end())
        {
            report.errors.push_back("WAV sample is not present in the streaming metadata index: " + canonicalSample.string());
            continue;
        }

        const auto sampleId = indexedSample->second;
        const auto& metadata = library->metadata[sampleId];
        if (metadata.frames == 0 || !std::isfinite(metadata.sampleRate)
            || metadata.sampleRate < 8000.0 || metadata.sampleRate > 384000.0
            || metadata.frames > static_cast<std::uint64_t>(std::numeric_limits<juce::int64>::max()))
        {
            report.errors.push_back("WAV sample has invalid or unsupported 64-bit streaming metadata: " + canonicalSample.string());
            continue;
        }

        // Keep frame positions 64-bit throughout the streaming runtime. The
        // page reader decodes bounded chunks, so total file duration is not
        // constrained by JUCE AudioBuffer's int-sized in-memory sample count.
        const auto totalFrames = metadata.frames;
        const std::uint64_t start = std::min<std::uint64_t>(region.offset, totalFrames);
        const std::uint64_t end = region.end == 0
            ? totalFrames
            : std::min<std::uint64_t>(region.end, totalFrames);
        if (start >= end)
        {
            report.errors.push_back("SFZ offset/end produce an empty sample region: " + canonicalSample.string());
            continue;
        }

        const auto loopStartOffset = std::min<std::uint64_t>(
            region.loopStart, static_cast<std::uint64_t>(end - start - 1));
        const auto loopEndOffset = region.loopEnd == 0
            ? static_cast<std::uint64_t>(end - start)
            : std::min<std::uint64_t>(region.loopEnd, static_cast<std::uint64_t>(end - start));
        if (region.loopMode != "no_loop" && region.loopMode != "one_shot"
            && (loopStartOffset >= loopEndOffset || loopEndOffset - loopStartOffset < 2))
        {
            report.errors.push_back("SFZ loop points are invalid or shorter than two frames: " + canonicalSample.string());
            continue;
        }

        RegionView view;
        view.streaming = library->service.get();
        view.sampleId = sampleId;
        view.totalFrames = totalFrames;
        view.sourceSampleRate = metadata.sampleRate;
        view.lowKey = region.lowKey;
        view.highKey = region.highKey;
        view.rootKey = region.rootKey;
        view.lowVelocity = region.lowVelocity;
        view.highVelocity = region.highVelocity;
        view.lowChannel = region.lowChannel;
        view.highChannel = region.highChannel;
        view.group = region.group;
        view.offBy = region.offBy;
        view.seqLength = region.seqLength;
        view.seqPosition = region.seqPosition;
        view.xfinLowVelocity = region.xfinLowVelocity;
        view.xfinHighVelocity = region.xfinHighVelocity;
        view.xfoutLowVelocity = region.xfoutLowVelocity;
        view.xfoutHighVelocity = region.xfoutHighVelocity;
        view.velocityCrossfadePower = region.velocityCrossfadePower;
        if (region.seqLength > 1)
        {
            const RoundRobinKey rrKey { region.group, region.seqLength, region.lowKey, region.highKey,
                                        region.lowVelocity, region.highVelocity,
                                        region.lowChannel, region.highChannel };
            const auto nextIndex = static_cast<int>(roundRobinSetIndices.size());
            const auto inserted = roundRobinSetIndices.emplace(rrKey, nextIndex);
            view.roundRobinSetIndex = inserted.first->second;
        }
        view.startFrame = start;
        view.endFrame = end;
        view.loopStartFrame = start + loopStartOffset;
        view.loopEndFrame = start + loopEndOffset;
        view.loopMode = region.loopMode == "loop_continuous" ? 1
            : (region.loopMode == "loop_sustain" ? 2
                : (region.loopMode == "loop_pingpong" ? 3 : (region.loopMode == "one_shot" ? 4 : 0)));
        view.tuneCents = region.tuneCents;
        view.gain = juce::Decibels::decibelsToGain(region.volumeDb);
        view.pan = region.pan;
        view.releaseSeconds = region.releaseSeconds;
        candidateSnapshot->regions.push_back(view);
        samplesUsed.insert(key);
    }

    if (!report.errors.empty() || candidateSnapshot->regions.empty() || abortIfSuperseded())
        return report;

    constexpr std::size_t cursorsPerSet = 16U * 128U;
    candidateSnapshot->roundRobinCounters.assign(roundRobinSetIndices.size() * cursorsPerSet, 0U);

    if (abortIfSuperseded())
        return report;

    if (pendingLibrary != nullptr)
    {
        if (!pendingLibrary->service->start())
        {
            report.errors.emplace_back("Could not start the background WAV streaming worker.");
            return report;
        }
        // Keep ownership local until the final cancellation check so a
        // superseded load cannot consume one of the bounded library slots.
        if (abortIfSuperseded())
            return report;
    }
    else if (abortIfSuperseded())
        return report;

    report.loadedRegions = candidateSnapshot->regions.size();
    report.loadedSamples = samplesUsed.size();
    const auto* published = candidateSnapshot.get();
    if (pendingLibrary != nullptr)
        streamingLibraries.push_back(std::move(pendingLibrary));
    snapshots.push_back(std::move(candidateSnapshot));
    activeSnapshot.store(published, std::memory_order_release);
    report.success = true;
    return report;
}

const SamplePool::RegionView* SamplePool::findRegion(int midiNote, int velocity, int midiChannel) noexcept
{
    const auto* snapshot = activeSnapshot.load(std::memory_order_acquire);
    if (snapshot == nullptr) return nullptr;

    const RegionView* firstMatch = nullptr;
    const RegionView* firstSequenceMatch = nullptr;
    int sequenceLength = 1;
    int sequenceSetIndex = -1;
    for (const auto& region : snapshot->regions)
    {
        if (midiNote < region.lowKey || midiNote > region.highKey
            || velocity < region.lowVelocity || velocity > region.highVelocity
            || midiChannel < region.lowChannel || midiChannel > region.highChannel)
            continue;

        if (firstMatch == nullptr)
            firstMatch = &region;
        if (region.roundRobinSetIndex >= 0)
        {
            if (sequenceSetIndex < 0)
            {
                sequenceLength = region.seqLength;
                sequenceSetIndex = region.roundRobinSetIndex;
            }
            if (region.roundRobinSetIndex == sequenceSetIndex && firstSequenceMatch == nullptr)
                firstSequenceMatch = &region;
        }
    }

    if (firstMatch == nullptr || sequenceSetIndex < 0 || sequenceLength <= 1)
        return firstMatch;

    const auto channelIndex = static_cast<std::size_t>(juce::jlimit(1, 16, midiChannel) - 1);
    const auto noteIndex = static_cast<std::size_t>(juce::jlimit(0, 127, midiNote));
    constexpr std::size_t cursorsPerSet = 16U * 128U;
    const auto cursorIndex = static_cast<std::size_t>(sequenceSetIndex) * cursorsPerSet
        + channelIndex * 128U + noteIndex;
    if (cursorIndex >= snapshot->roundRobinCounters.size())
        return firstMatch;

    auto& cursor = snapshot->roundRobinCounters[cursorIndex];
    const int wantedPosition = static_cast<int>(cursor % static_cast<std::uint32_t>(sequenceLength)) + 1;
    ++cursor;

    for (const auto& region : snapshot->regions)
        if (region.roundRobinSetIndex == sequenceSetIndex
            && wantedPosition == region.seqPosition
            && midiNote >= region.lowKey && midiNote <= region.highKey
            && velocity >= region.lowVelocity && velocity <= region.highVelocity
            && midiChannel >= region.lowChannel && midiChannel <= region.highChannel)
            return &region;

    // Malformed/incomplete RR sets cannot silence a note: fall back within
    // the selected RR set, never to an unrelated non-RR region encountered
    // earlier in the SFZ file.
    return firstSequenceMatch != nullptr ? firstSequenceMatch : firstMatch;
}

std::size_t SamplePool::findRegions(
    int midiNote, int velocity, int midiChannel,
    std::array<RegionSelection, maximumVelocityLayers>& output) noexcept
{
    output.fill(RegionSelection {});
    const auto* snapshot = activeSnapshot.load(std::memory_order_acquire);
    if (snapshot == nullptr || velocity <= 0)
        return 0;

    struct RoundRobinChoice
    {
        int setIndex = -1;
        int wantedPosition = -1;
        const RegionView* fallback = nullptr;
        const RegionView* selected = nullptr;
    };
    std::array<RoundRobinChoice, maximumVelocityLayers> rrChoices {};
    std::size_t rrChoiceCount = 0;
    std::size_t outputCount = 0;

    const auto append = [&](const RegionView* region)
    {
        if (region == nullptr || outputCount >= output.size())
            return;

        float gain = 1.0f;
        constexpr float halfPi = 1.57079632679489661923f;
        if (region->xfinLowVelocity >= 0 && region->xfinHighVelocity >= 0)
        {
            const int low = region->xfinLowVelocity;
            const int high = region->xfinHighVelocity;
            float phase = high > low
                ? static_cast<float>(velocity - low) / static_cast<float>(high - low)
                : (velocity > low ? 1.0f : 0.0f);
            phase = juce::jlimit(0.0f, 1.0f, phase);
            if (region->velocityCrossfadePower)
                phase = std::sin(phase * halfPi);
            gain *= phase;
        }

        if (region->xfoutLowVelocity >= 0 && region->xfoutHighVelocity >= 0)
        {
            const int low = region->xfoutLowVelocity;
            const int high = region->xfoutHighVelocity;
            float phase = high > low
                ? static_cast<float>(velocity - low) / static_cast<float>(high - low)
                : (velocity > low ? 1.0f : 0.0f);
            phase = juce::jlimit(0.0f, 1.0f, phase);
            if (region->velocityCrossfadePower)
                phase = std::cos(phase * halfPi);
            else
                phase = 1.0f - phase;
            gain *= phase;
        }

        if (gain <= 0.000001f)
            return;
        output[outputCount++] = { region, gain };
    };

    // One bounded pass over the immutable region snapshot. RR choices are
    // accumulated in a fixed array, avoiding a second full scan per RR set.
    for (const auto& region : snapshot->regions)
    {
        if (midiNote < region.lowKey || midiNote > region.highKey
            || velocity < region.lowVelocity || velocity > region.highVelocity
            || midiChannel < region.lowChannel || midiChannel > region.highChannel)
            continue;

        if (region.roundRobinSetIndex < 0)
        {
            append(&region);
            continue;
        }

        std::size_t choiceIndex = rrChoiceCount;
        for (std::size_t i = 0; i < rrChoiceCount; ++i)
            if (rrChoices[i].setIndex == region.roundRobinSetIndex)
            {
                choiceIndex = i;
                break;
            }

        if (choiceIndex == rrChoiceCount)
        {
            if (rrChoiceCount >= rrChoices.size())
                continue;

            auto& choice = rrChoices[rrChoiceCount++];
            choice.setIndex = region.roundRobinSetIndex;
            const auto channelIndex = static_cast<std::size_t>(juce::jlimit(1, 16, midiChannel) - 1);
            const auto noteIndex = static_cast<std::size_t>(juce::jlimit(0, 127, midiNote));
            constexpr std::size_t cursorsPerSet = 16U * 128U;
            const auto cursorIndex = static_cast<std::size_t>(region.roundRobinSetIndex) * cursorsPerSet
                + channelIndex * 128U + noteIndex;
            if (cursorIndex < snapshot->roundRobinCounters.size())
            {
                auto& cursor = snapshot->roundRobinCounters[cursorIndex];
                choice.wantedPosition = static_cast<int>(
                    cursor % static_cast<std::uint32_t>(region.seqLength)) + 1;
                ++cursor;
            }
        }

        auto& choice = rrChoices[choiceIndex];
        if (choice.fallback == nullptr)
            choice.fallback = &region;
        if (choice.wantedPosition > 0 && region.seqPosition == choice.wantedPosition)
            choice.selected = &region;
    }

    for (std::size_t i = 0; i < rrChoiceCount; ++i)
    {
        const auto& choice = rrChoices[i];
        append(choice.selected != nullptr ? choice.selected : choice.fallback);
    }

    return outputCount;
}

std::size_t SamplePool::getRegionCount() const noexcept
{
    const auto* snapshot = activeSnapshot.load(std::memory_order_acquire);
    return snapshot != nullptr ? snapshot->regions.size() : 0;
}

std::size_t SamplePool::getSampleCount() const noexcept
{
    const auto* snapshot = activeSnapshot.load(std::memory_order_acquire);
    if (snapshot == nullptr) return 0;
    std::set<std::pair<std::uintptr_t, std::uint64_t>> unique;
    for (const auto& region : snapshot->regions)
    {
        if (region.streaming != nullptr)
            unique.emplace(reinterpret_cast<std::uintptr_t>(region.streaming), region.sampleId);
        else if (region.audio != nullptr)
            unique.emplace(0U, reinterpret_cast<std::uintptr_t>(region.audio));
    }
    return unique.size();
}

void SamplerEngine::prepare(double sampleRate) noexcept
{
    outputSampleRate = juce::jmax(1.0, sampleRate);
    reset();
}

void SamplerEngine::reset() noexcept
{
    for (std::size_t i = 0; i < voices.size(); ++i)
    {
        streamingReaders[i].release();
        voices[i] = {};
    }
}

LoadReport SamplerEngine::loadSfz(const juce::File& sfzFile,
                                  const std::atomic<std::uint64_t>* requestGeneration,
                                  std::uint64_t expectedGeneration)
{
    return pool.loadSfz(sfzFile, requestGeneration, expectedGeneration);
}

bool SamplerEngine::noteOn(int midiNote, int velocity, int maximumActiveVoices,
                           int midiChannel, double globalTuningSemitones) noexcept
{
    if (velocity <= 0)
        return false;

    std::array<SamplePool::RegionSelection, SamplePool::maximumVelocityLayers> selections {};
    const auto selectionCount = pool.findRegions(midiNote, velocity, midiChannel, selections);
    if (selectionCount == 0)
        return false;

    const int voiceLimit = juce::jlimit(1, maximumVoices, maximumActiveVoices);
    int activeVoices = 0;
    for (const auto& active : voices)
        if (active.region != nullptr)
            ++activeVoices;

    // If the polyphony budget is tight, retain the strongest crossfade layers
    // rather than letting later layers steal voices started for this same note.
    const std::size_t available = activeVoices < voiceLimit
        ? static_cast<std::size_t>(voiceLimit - activeVoices) : 1U;
    for (std::size_t i = 1; i < selectionCount; ++i)
    {
        const auto candidate = selections[i];
        std::size_t j = i;
        while (j > 0 && selections[j - 1].velocityGain < candidate.velocityGain)
        {
            selections[j] = selections[j - 1];
            --j;
        }
        selections[j] = candidate;
    }
    const auto layersToStart = std::min(selectionCount, available);

    const double safeGlobalTuning = std::isfinite(globalTuningSemitones)
        ? juce::jlimit(-48.0, 48.0, globalTuningSemitones) : 0.0;
    bool started = false;

    for (std::size_t layer = 0; layer < layersToStart; ++layer)
    {
        const auto* region = selections[layer].region;
        if (region == nullptr || (region->audio == nullptr && region->streaming == nullptr))
            continue;

        // SFZ off_by groups implement guitar-string choking.
        if (region->offBy > 0)
            for (auto& activeVoice : voices)
                if (activeVoice.region != nullptr
                    && activeVoice.region->group == region->offBy
                    && activeVoice.midiChannel == midiChannel)
                    releaseVoice(activeVoice);

        const int voiceIndex = chooseVoice(voiceLimit);
        auto& voice = voices[static_cast<std::size_t>(voiceIndex)];
        auto& streamReader = streamingReaders[static_cast<std::size_t>(voiceIndex)];
        streamReader.release();
        voice = {};
        voice.region = region;
        voice.note = midiNote;
        voice.midiChannel = juce::jlimit(1, 16, midiChannel);
        voice.direction = 1;
        voice.position = static_cast<double>(region->startFrame);
        const double semitones = static_cast<double>(midiNote - region->rootKey)
            + static_cast<double>(region->tuneCents) / 100.0 + safeGlobalTuning;
        voice.increment = (region->sourceSampleRate / outputSampleRate)
            * std::pow(2.0, semitones / 12.0);
        voice.gain = (static_cast<float>(velocity) / 127.0f) * region->gain
            * selections[layer].velocityGain;
        voice.envelopeLevel = 0.0f;
        // A short linear attack prevents sample-start discontinuities from
        // producing clicks while preserving a fast, playable guitar response.
        constexpr double attackSeconds = 0.003;
        voice.attackStep = static_cast<float>(1.0 / juce::jmax(1.0, attackSeconds * outputSampleRate));
        voice.age = nextAge++;
        if (region->streaming != nullptr)
        {
            streamReader.attach(region->streaming, region->sampleId, region->totalFrames);
            streamReader.prefetch(region->startFrame);
            if (region->loopMode == 1 || region->loopMode == 2 || region->loopMode == 3)
                streamReader.requestPrefetch(region->loopStartFrame);
            voice.prefetchedPage = region->startFrame / region->streaming->pageFrames();
        }
        started = true;
    }

    return started;
}

void SamplerEngine::noteOff(int midiNote, int midiChannel) noexcept
{
    for (auto& voice : voices)
        if (voice.region != nullptr && voice.note == midiNote && voice.midiChannel == midiChannel && !voice.releasing)
            if (voice.region->loopMode != 4)
            {
                if (voice.region->loopMode == 2 || voice.region->loopMode == 3)
                    voice.direction = 1;
                releaseVoice(voice);
            }
}

void SamplerEngine::allNotesOff() noexcept
{
    for (auto& voice : voices)
        if (voice.region != nullptr)
            releaseVoice(voice);
}

void SamplerEngine::panic(int fadeSamples) noexcept
{
    const float samples = static_cast<float>(juce::jmax(1, fadeSamples));
    for (auto& voice : voices)
        if (voice.region != nullptr)
        {
            voice.releaseStep = voice.envelopeLevel / samples;
            voice.releasing = true;
        }
}

void SamplerEngine::releaseVoice(Voice& voice) noexcept
{
    if (voice.region == nullptr) return;
    const int releaseSamples = juce::jmax(1, static_cast<int>(voice.region->releaseSeconds * outputSampleRate));
    voice.releaseStep = voice.envelopeLevel / static_cast<float>(releaseSamples);
    voice.releasing = true;
}

int SamplerEngine::chooseVoice(int maximumActiveVoices) noexcept
{
    int active = 0;
    int freeIndex = -1;
    int oldestIndex = 0;
    std::uint64_t oldestAge = std::numeric_limits<std::uint64_t>::max();
    for (int i = 0; i < maximumVoices; ++i)
    {
        const auto& voice = voices[static_cast<std::size_t>(i)];
        if (voice.region == nullptr)
        {
            if (freeIndex < 0) freeIndex = i;
            continue;
        }
        ++active;
        if (voice.age < oldestAge)
        {
            oldestAge = voice.age;
            oldestIndex = i;
        }
    }
    if (active < maximumActiveVoices && freeIndex >= 0)
        return freeIndex;
    return oldestIndex;
}

void SamplerEngine::render(juce::AudioBuffer<float>& output, int startSample, int numSamples) noexcept
{
    const int channels = output.getNumChannels();
    const int endSample = juce::jmin(output.getNumSamples(), startSample + numSamples);
    for (std::size_t voiceIndex = 0; voiceIndex < voices.size(); ++voiceIndex)
    {
        auto& voice = voices[voiceIndex];
        auto& streamReader = streamingReaders[voiceIndex];
        if (voice.region == nullptr)
            continue;

        const auto& region = *voice.region;
        const auto* source = region.audio;
        const int sourceChannels = source != nullptr ? source->getNumChannels() : 2;
        const std::uint64_t sourceEnd = region.endFrame;
        const auto* left = source != nullptr ? source->getReadPointer(0) : nullptr;
        const auto* right = source != nullptr && sourceChannels > 1
            ? source->getReadPointer(1) : left;

        for (int outIndex = startSample; outIndex < endSample; ++outIndex)
        {
            const bool loopActive = region.loopMode == 1
                || ((region.loopMode == 2 || region.loopMode == 3) && !voice.releasing);
            if (loopActive && region.loopMode == 3)
            {
                if (voice.direction > 0 && voice.position >= region.loopEndFrame)
                {
                    voice.direction = -1;
                    voice.position = static_cast<double>(region.loopEndFrame - 1);
                }
                else if (voice.direction < 0 && voice.position < region.loopStartFrame)
                {
                    voice.direction = 1;
                    voice.position = static_cast<double>(region.loopStartFrame);
                }
            }
            else if (loopActive && voice.position >= region.loopEndFrame)
                voice.position = static_cast<double>(region.loopStartFrame);

            if (!std::isfinite(voice.position) || voice.position < 0.0
                || voice.position >= static_cast<double>(sourceEnd))
            {
                streamReader.release();
                voice = {};
                break;
            }
            const auto frame = static_cast<std::uint64_t>(voice.position);
            if ((voice.direction > 0 && frame >= sourceEnd) || frame < region.startFrame)
            {
                streamReader.release();
                voice = {};
                break;
            }

            if (region.streaming != nullptr)
            {
                const auto currentPage = static_cast<std::uint64_t>(frame)
                    / region.streaming->pageFrames();
                if (currentPage != voice.prefetchedPage)
                {
                    streamReader.prefetch(static_cast<std::uint64_t>(frame));
                    voice.prefetchedPage = currentPage;
                }
            }

            const float fraction = static_cast<float>(voice.position - static_cast<double>(frame));
            float sampleL = 0.0f;
            float sampleR = 0.0f;
            bool sampleReady = false;
            if (source != nullptr)
            {
                sampleL = hermiteSample(left, frame, fraction, region.startFrame, sourceEnd);
                sampleR = hermiteSample(right, frame, fraction, region.startFrame, sourceEnd);
                sampleReady = true;
            }
            else if (region.streaming != nullptr)
            {
                sampleReady = streamReader.readHermite(
                    static_cast<std::uint64_t>(frame), fraction,
                    static_cast<std::uint64_t>(region.startFrame),
                    static_cast<std::uint64_t>(sourceEnd), sampleL, sampleR);
            }

            if (!sampleReady)
            {
                // A cache miss is not allowed to advance the sample cursor:
                // this preserves the attack/transient while the worker loads
                // the page. No wait or file access occurs on the audio thread.
                streamReader.prefetch(static_cast<std::uint64_t>(frame));

                // A failed/removed source must not pin a voice forever. If a
                // note is already releasing, continue its envelope while the
                // output is silent; otherwise retire a voice after a bounded
                // two-second page-unavailable timeout.
                if (voice.releasing)
                {
                    voice.envelopeLevel -= voice.releaseStep;
                    if (voice.envelopeLevel <= 0.0f)
                    {
                        streamReader.release();
                        voice = {};
                        break;
                    }
                }
                else if (++voice.streamingMissSamples
                         > static_cast<std::uint64_t>(outputSampleRate * 2.0))
                {
                    streamReader.release();
                    voice = {};
                    break;
                }
                continue;
            }
            voice.streamingMissSamples = 0;

            const float panLeft = region.pan > 0.0f ? 1.0f - region.pan : 1.0f;
            const float panRight = region.pan < 0.0f ? 1.0f + region.pan : 1.0f;
            const float envelope = voice.gain * voice.envelopeLevel;

            if (channels == 1)
                output.addSample(0, outIndex, 0.5f * (sampleL + sampleR) * envelope);
            else
            {
                output.addSample(0, outIndex, sampleL * envelope * panLeft);
                output.addSample(1, outIndex, sampleR * envelope * panRight);
                for (int ch = 2; ch < channels; ++ch)
                    output.addSample(ch, outIndex, 0.5f * (sampleL + sampleR) * envelope);
            }

            voice.position += voice.increment * static_cast<double>(voice.direction);
            if (voice.releasing)
            {
                voice.envelopeLevel -= voice.releaseStep;
                if (voice.envelopeLevel <= 0.0f)
                {
                    streamReader.release();
                    voice = {};
                    break;
                }
            }
            else if (voice.envelopeLevel < 1.0f)
                voice.envelopeLevel = juce::jmin(1.0f, voice.envelopeLevel + voice.attackStep);
        }
    }
}

bool SamplerEngine::hasSamples() const noexcept
{
    return pool.getRegionCount() > 0;
}

std::size_t SamplerEngine::getRegionCount() const noexcept
{
    return pool.getRegionCount();
}

std::size_t SamplerEngine::getSampleCount() const noexcept
{
    return pool.getSampleCount();
}

int SamplerEngine::getActiveVoiceCount() const noexcept
{
    int count = 0;
    for (const auto& voice : voices)
        if (voice.region != nullptr) ++count;
    return count;
}
}
