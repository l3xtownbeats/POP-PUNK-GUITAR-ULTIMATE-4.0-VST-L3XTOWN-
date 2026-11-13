#pragma once
#include <atomic>
#include <functional>
#include <string>
#include <vector>

namespace ppgu {

// Import Guitar: single files or whole libraries; WAV/FLAC/OGG/AIFF/MP3.
// Off-audio-thread; SHA-256 provenance; SNR/guitar gate; engine distributes
// results into MELODY and PATTERN/PUNK GROUP mappings.
class ImportPipeline {
public:
    enum class Format { WAV, FLAC, OGG, AIFF, MP3 };
    struct Item {
        std::string path;
        Format format = Format::WAV;
        std::string sha256;
        float snrDb = 0.0f;
        bool guitarConfirmed = false;
        bool pass = false;
        std::string rejectReason;
    };
    struct Progress { float fraction = 0.0f; std::string stage; };

    static Format detectFormat(const std::string& filename);
    static bool sniffHeader(const unsigned char* d, size_t n, Format& out);

    // Blocking conversion/screening; called from UI/worker thread only.
    std::vector<Item> screen(const std::vector<std::string>& paths,
                             std::function<void(const Progress&)> onProgress = {});
};

} // namespace ppgu