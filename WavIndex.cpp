#include "WavIndex.h"
#include <algorithm>

namespace ppgu {

void WavIndex::add(uint64_t firstFrame, uint64_t frameCount) {
    entries_.push_back({firstFrame, frameCount});
    total_ += frameCount;
}

void WavIndex::readBounds(uint64_t pos, size_t n, uint64_t& first, size_t& count) const {
    if (total_ == 0 || n == 0) { first = 0; count = 0; return; }
    uint64_t a = pos;
    uint64_t b = pos + n;
    if (a >= total_) { first = 0; count = 0; return; }
    if (b > total_) b = total_;
    first = a;
    count = (size_t)(b - a);
}

} // namespace ppgu