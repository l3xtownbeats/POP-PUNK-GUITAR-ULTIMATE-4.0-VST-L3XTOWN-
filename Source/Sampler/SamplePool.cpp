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
constexpr double maximumSampleSeconds = 120.0;
constexpr int maximumSampleChannels = 2;

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
float hermiteSample(const float* data, int frame, float fraction, int regionStart, int regionEnd) noexcept
{
    const int i0 = juce::jlimit(regionStart, regionEnd - 1, frame - 1);
    const int i1 = juce::jlimit(regionStart, regionEnd - 1, frame);
    const int i2 = juce::jlimit(regionStart, regionEnd - 1, frame + 1);
    const int i3 = juce::jlimit(regionStart, regionEnd - 1, frame + 2);
    const float y0 = data[i0];
    const float y1 = data[i1];
    const float y2 = data[i2];
    const float y3 = data[i3];
    const float t2 = fraction * fraction;
    const float t3 = t2 * fraction;
    return 0.5f * ((2.0f * y1)
        + (-y0 + y2) * fraction
        + (2.0f * y0 - 5.0f * y1 + 4.0f * y2 - y3) * t2
        + (-y0 + 3.0f * y1 - 3.0f * y2 + y3) * t3);
}
}

SamplePool::SamplePool() = default;

LoadReport SamplePool::loadSfz(const juce::File& sfzFile)
{
    LoadReport report;
    std::lock_guard<std::mutex> guard(loaderMutex);

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
    if (!parsed.succeeded())
        return report;

    if (snapshots.size() >= maximumSnapshots)
    {
        report.errors.emplace_back("Sampler snapshot limit reached. Restart the plugin before loading more banks.");
        return report;
    }

    const auto root = std::filesystem::weakly_canonical(std::filesystem::path(sfzFile.getParentDirectory().getFullPathName().toStdString()));
    auto candidateSnapshot = std::make_unique<Snapshot>();
    candidateSnapshot->regions.reserve(parsed.regions.size());
    using RoundRobinKey = std::tuple<int, int, int, int, int, int, int, int>;
    std::map<RoundRobinKey, int> roundRobinSetIndices;

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::set<std::string> samplesUsed;
    // Keep newly decoded assets local until the entire SFZ map validates. A
    // later bad region must not leave orphaned buffers in the long-lived pool.
    std::map<std::string, std::unique_ptr<SampleAsset>> pendingSampleStorage;
    std::size_t pendingSampleBytes = 0;
    for (const auto& region : parsed.regions)
    {
        std::error_code ec;
        auto sourceSamplePath = region.samplePath;

        // Meta Open Sound's official SFZ maps refer to OGG preview filenames,
        // while the production WAV release contains same-stem WAV files under
        // a sibling wav/ directory. Prefer the original WAV release without
        // modifying or discarding the upstream SFZ map.
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

            // Some repackaged libraries place WAVs in a nested directory
            // other than wav/. Search by exact filename only and reject
            // ambiguous matches rather than silently choosing the wrong take.
            if (!resolvedWav)
            {
                std::filesystem::path uniqueMatch;
                bool ambiguous = false;
                ec.clear();
                for (std::filesystem::recursive_directory_iterator it(
                         root, std::filesystem::directory_options::skip_permission_denied, ec), end;
                     it != end && !ec; it.increment(ec))
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
        const SampleAsset* assetPointer = nullptr;

        if (const auto existing = sampleStorage.find(key); existing != sampleStorage.end())
            assetPointer = existing->second.get();
        else if (const auto pending = pendingSampleStorage.find(key); pending != pendingSampleStorage.end())
            assetPointer = pending->second.get();
        else
        {
            if (juce::String(canonicalSample.extension().string()).toLowerCase() != ".wav")
            {
                report.errors.push_back("Only WAV samples are accepted in this production loader: " + canonicalSample.string());
                continue;
            }

            auto reader = std::unique_ptr<juce::AudioFormatReader>(
                formatManager.createReaderFor(juce::File(canonicalSample.string())));
            if (reader == nullptr)
            {
                report.errors.push_back("Could not decode WAV sample: " + canonicalSample.string());
                continue;
            }

            if (!std::isfinite(reader->sampleRate) || reader->sampleRate < 8000.0
                || reader->sampleRate > 384000.0 || reader->lengthInSamples <= 0
                || reader->lengthInSamples > static_cast<juce::int64>(reader->sampleRate * maximumSampleSeconds)
                || reader->numChannels == 0 || reader->numChannels > static_cast<unsigned int>(maximumSampleChannels))
            {
                report.errors.push_back("WAV sample must be mono or stereo, have a valid sample rate, and be non-empty and no longer than 120 seconds: "
                                        + canonicalSample.string());
                continue;
            }

            // Never silently discard channels from multichannel source material.
            // The current sampler DSP is explicitly mono/stereo; unsupported
            // layouts must fail validation until a channel-routing policy exists.
            const int channels = static_cast<int>(reader->numChannels);
            const int frames = static_cast<int>(reader->lengthInSamples);
            const auto assetBytes = static_cast<std::size_t>(channels)
                * static_cast<std::size_t>(frames) * sizeof(float);
            if (sampleStorageBytes > maximumSampleStorageBytes
                || pendingSampleBytes > maximumSampleStorageBytes - sampleStorageBytes
                || assetBytes > maximumSampleStorageBytes - sampleStorageBytes - pendingSampleBytes)
            {
                report.errors.push_back("Sample bank exceeds the 1 GiB per-instance decoded WAV cache limit.");
                continue;
            }

            auto asset = std::make_unique<SampleAsset>();
            asset->audio.setSize(channels, frames, false, true, false);
            if (!reader->read(&asset->audio, 0, frames, 0, true, channels > 1))
            {
                report.errors.push_back("Failed while reading WAV sample: " + canonicalSample.string());
                continue;
            }
            asset->sampleRate = reader->sampleRate;
            const auto inserted = pendingSampleStorage.emplace(key, std::move(asset));
            pendingSampleBytes += assetBytes;
            assetPointer = inserted.first->second.get();
        }

        const auto& asset = *assetPointer;
        const int totalFrames = asset.audio.getNumSamples();
        const int start = static_cast<int>(juce::jmin<std::uint64_t>(region.offset, static_cast<std::uint64_t>(totalFrames)));
        const int end = region.end == 0
            ? totalFrames
            : static_cast<int>(juce::jmin<std::uint64_t>(region.end, static_cast<std::uint64_t>(totalFrames)));
        if (start >= end)
        {
            report.errors.push_back("SFZ offset/end produce an empty sample region: " + canonicalSample.string());
            continue;
        }
        const auto loopStart = start + static_cast<int>(juce::jmin<std::uint64_t>(region.loopStart, static_cast<std::uint64_t>(end - start - 1)));
        const auto loopEnd = region.loopEnd == 0 ? end
            : start + static_cast<int>(juce::jmin<std::uint64_t>(region.loopEnd, static_cast<std::uint64_t>(end - start)));
        if (region.loopMode != "no_loop" && region.loopMode != "one_shot"
            && (loopStart >= loopEnd || loopEnd - loopStart < 2))
        {
            report.errors.push_back("SFZ loop points are invalid or shorter than two frames: " + canonicalSample.string());
            continue;
        }

        SamplePool::RegionView view;
        view.audio = &asset.audio;
        view.sourceSampleRate = asset.sampleRate;
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
        view.loopStartFrame = start + static_cast<int>(juce::jmin<std::uint64_t>(region.loopStart, static_cast<std::uint64_t>(end - start - 1)));
        view.loopEndFrame = region.loopEnd == 0 ? end : start + static_cast<int>(juce::jmin<std::uint64_t>(region.loopEnd, static_cast<std::uint64_t>(end - start)));
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

    if (!report.errors.empty() || candidateSnapshot->regions.empty())
        return report;

    // Allocate all RR cursors during loading; findRegion() performs no allocation.
    constexpr std::size_t cursorsPerSet = 16U * 128U;
    candidateSnapshot->roundRobinCounters.assign(roundRobinSetIndices.size() * cursorsPerSet, 0U);

    // Commit decoded buffers only after every region has passed validation.
    // std::map::merge transfers nodes without moving the SampleAsset objects,
    // preserving the audio pointers already stored in the candidate snapshot.
    sampleStorage.merge(pendingSampleStorage);
    sampleStorageBytes += pendingSampleBytes;

    report.loadedRegions = candidateSnapshot->regions.size();
    report.loadedSamples = samplesUsed.size();
    const auto* published = candidateSnapshot.get();
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

std::size_t SamplePool::getRegionCount() const noexcept
{
    const auto* snapshot = activeSnapshot.load(std::memory_order_acquire);
    return snapshot != nullptr ? snapshot->regions.size() : 0;
}

std::size_t SamplePool::getSampleCount() const noexcept
{
    const auto* snapshot = activeSnapshot.load(std::memory_order_acquire);
    if (snapshot == nullptr) return 0;
    std::set<const juce::AudioBuffer<float>*> unique;
    for (const auto& region : snapshot->regions)
        unique.insert(region.audio);
    return unique.size();
}

void SamplerEngine::prepare(double sampleRate) noexcept
{
    outputSampleRate = juce::jmax(1.0, sampleRate);
    reset();
}

void SamplerEngine::reset() noexcept
{
    for (auto& voice : voices)
        voice = {};
}

LoadReport SamplerEngine::loadSfz(const juce::File& sfzFile)
{
    return pool.loadSfz(sfzFile);
}

bool SamplerEngine::noteOn(int midiNote, int velocity, int maximumActiveVoices, int midiChannel, double globalTuningSemitones) noexcept
{
    const auto* region = pool.findRegion(midiNote, velocity, midiChannel);
    if (region == nullptr || region->audio == nullptr || velocity <= 0)
        return false;

    // SFZ off_by groups implement guitar-string choking: a new note on the
    // same string gently releases the previously ringing voice.
    if (region->offBy > 0)
        for (auto& activeVoice : voices)
            if (activeVoice.region != nullptr
                && activeVoice.region->group == region->offBy
                && activeVoice.midiChannel == midiChannel)
                releaseVoice(activeVoice);

    const int voiceIndex = chooseVoice(juce::jlimit(1, maximumVoices, maximumActiveVoices));
    auto& voice = voices[static_cast<std::size_t>(voiceIndex)];
    voice = {};
    voice.region = region;
    voice.note = midiNote;
    voice.midiChannel = juce::jlimit(1, 16, midiChannel);
    voice.direction = 1;
    voice.position = static_cast<double>(region->startFrame);
    const double safeGlobalTuning = std::isfinite(globalTuningSemitones)
        ? juce::jlimit(-48.0, 48.0, globalTuningSemitones) : 0.0;
    const double semitones = static_cast<double>(midiNote - region->rootKey)
        + static_cast<double>(region->tuneCents) / 100.0 + safeGlobalTuning;
    voice.increment = (region->sourceSampleRate / outputSampleRate) * std::pow(2.0, semitones / 12.0);
    voice.gain = (static_cast<float>(velocity) / 127.0f) * region->gain;
    voice.age = nextAge++;
    return true;
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
            voice.releaseStep = voice.gain / samples;
            voice.releasing = true;
        }
}

void SamplerEngine::releaseVoice(Voice& voice) noexcept
{
    if (voice.region == nullptr) return;
    const int releaseSamples = juce::jmax(1, static_cast<int>(voice.region->releaseSeconds * outputSampleRate));
    voice.releaseStep = voice.gain / static_cast<float>(releaseSamples);
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
    for (auto& voice : voices)
    {
        if (voice.region == nullptr || voice.region->audio == nullptr)
            continue;

        const auto& source = *voice.region->audio;
        const int sourceChannels = source.getNumChannels();
        const int sourceEnd = voice.region->endFrame;
        const auto* left = source.getReadPointer(0);
        const auto* right = sourceChannels > 1 ? source.getReadPointer(1) : left;

        for (int outIndex = startSample; outIndex < endSample; ++outIndex)
        {
            const bool loopActive = voice.region->loopMode == 1
                || ((voice.region->loopMode == 2 || voice.region->loopMode == 3) && !voice.releasing);
            if (loopActive && voice.region->loopMode == 3)
            {
                if (voice.direction > 0 && voice.position >= voice.region->loopEndFrame)
                {
                    voice.direction = -1;
                    voice.position = static_cast<double>(voice.region->loopEndFrame - 1);
                }
                else if (voice.direction < 0 && voice.position < voice.region->loopStartFrame)
                {
                    voice.direction = 1;
                    voice.position = static_cast<double>(voice.region->loopStartFrame);
                }
            }
            else if (loopActive && voice.position >= voice.region->loopEndFrame)
                voice.position = static_cast<double>(voice.region->loopStartFrame);

            const int frame = static_cast<int>(voice.position);
            if ((voice.direction > 0 && frame >= sourceEnd) || frame < voice.region->startFrame)
            {
                voice = {};
                break;
            }

            const float fraction = static_cast<float>(voice.position - static_cast<double>(frame));
            const float sampleL = hermiteSample(left, frame, fraction, voice.region->startFrame, sourceEnd);
            const float sampleR = hermiteSample(right, frame, fraction, voice.region->startFrame, sourceEnd);
            const float panLeft = voice.region->pan > 0.0f ? 1.0f - voice.region->pan : 1.0f;
            const float panRight = voice.region->pan < 0.0f ? 1.0f + voice.region->pan : 1.0f;
            const float envelope = voice.gain;

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
                voice.gain -= voice.releaseStep;
                if (voice.gain <= 0.0f)
                {
                    voice = {};
                    break;
                }
            }
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
