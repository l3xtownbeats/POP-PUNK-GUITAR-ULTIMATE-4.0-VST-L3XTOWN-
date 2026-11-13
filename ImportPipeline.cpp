#include "ImportPipeline.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace ppgu {

ImportPipeline::Format ImportPipeline::detectFormat(const std::string& filename) {
    auto ext = filename.substr(filename.find_last_of('.') + 1);
    for (auto& c : ext) c = (char)std::tolower((unsigned char)c);
    if (ext == "wav") return Format::WAV;
    if (ext == "flac") return Format::FLAC;
    if (ext == "ogg" || ext == "ogv") return Format::OGG;
    if (ext == "aif" || ext == "aiff") return Format::AIFF;
    if (ext == "mp3") return Format::MP3;
    return Format::WAV;
}

bool ImportPipeline::sniffHeader(const unsigned char* d, size_t n, Format& out) {
    if (n < 4) return false;
    if (std::memcmp(d, "RIFF", 4) == 0) { out = Format::WAV; return true; }
    if (std::memcmp(d, "fLaC", 4) == 0) { out = Format::FLAC; return true; }
    if (std::memcmp(d, "OggS", 4) == 0) { out = Format::OGG; return true; }
    if (std::memcmp(d, "FORM", 4) == 0) { out = Format::AIFF; return true; }
    if (d[0] == 0xFF && (d[1] & 0xE0) == 0xE0) { out = Format::MP3; return true; }
    return false;
}

std::vector<ImportPipeline::Item> ImportPipeline::screen(
        const std::vector<std::string>& paths,
        std::function<void(const Progress&)> onProgress) {
    std::vector<Item> out;
    for (size_t i = 0; i < paths.size(); ++i) {
        Item it;
        it.path = paths[i];
        it.format = detectFormat(paths[i]);
        // Real decode/conversion happens in JUCE layer; here: gate policy checks.
        // SNR/guitar confirmation arrive from analyzer stage; conservative default reject.
        it.snrDb = 0.0f;
        it.guitarConfirmed = false;
        it.pass = false;
        it.rejectReason = "awaiting analyzer stage (SNR>=40dB + guitar gate required)";
        out.push_back(it);
        if (onProgress) onProgress({(float)(i + 1) / (float)std::max<size_t>(1, paths.size()), "screening"});
    }
    return out;
}

} // namespace ppgu