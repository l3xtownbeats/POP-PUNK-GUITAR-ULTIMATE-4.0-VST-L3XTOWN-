#include "SamplePool.h"
#include <algorithm>

namespace ppgu {

void SamplePool::beginLoad() {
    std::lock_guard<std::mutex> lk(mtx_);
    snapshots_.push_back(nullptr); // reserve slot; superseded load fills next commit
    if (snapshots_.size() > kMaxSnapshots) {
        // never drop entries referenced as current without quiescence
        snapshots_.erase(snapshots_.begin());
    }
}

bool SamplePool::commit(std::shared_ptr<const SampleSnapshot> snap) {
    if (!snap) return false;
    std::lock_guard<std::mutex> lk(mtx_);
    if (current_ && current_->generation >= snap->generation) return false; // superseded
    current_ = snap;
    if (!snapshots_.empty() && snapshots_.back() == nullptr) snapshots_.back() = snap;
    else snapshots_.push_back(snap);
    return true;
}

std::shared_ptr<const SampleSnapshot> SamplePool::current() const {
    return std::atomic_load(&current_);
}

void SamplePool::retireOld(uint64_t generation) {
    std::lock_guard<std::mutex> lk(mtx_);
    for (auto it = snapshots_.begin(); it != snapshots_.end();) {
        auto& s = *it;
        if (s && s->generation < generation && s != current_) {
            if (activeVoiceCount_.load(std::memory_order_relaxed) == 0) {
                graveyard_.push_back(s);
                it = snapshots_.erase(it);
                continue;
            }
        }
        ++it;
    }
    if (graveyard_.size() > 32) graveyard_.clear();
}

bool SamplePool::registerRoot(const std::string& root) {
    std::lock_guard<std::mutex> lk(mtx_);
    if (roots_.size() >= kMaxStreamingRoots) return false;
    return roots_.insert(root).second;
}

} // namespace ppgu