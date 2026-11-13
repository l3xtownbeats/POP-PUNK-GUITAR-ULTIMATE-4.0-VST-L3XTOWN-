#pragma once
#include <cstdint>
#include <vector>

namespace ppgu {

// WAV metadata index with Hermite-safe boundaries (no reads outside [0, frames)).
class WavIndex {
public:
    struct Entry { uint64_t firstFrame = 0; uint64_t frameCount = 0; };
    void add(uint64_t firstFrame, uint64_t frameCount);
    size_t size() const { return entries_.size(); }
    uint64_t totalFrames() const { return total_; }
    // Safe read window with Hermite boundaries clamped to [0, total).
    void readBounds(uint64_t pos, size_t n, uint64_t& first, size_t& count) const;
private:
    std::vector<Entry> entries_;
    uint64_t total_ = 0;
};

} // namespace ppgu