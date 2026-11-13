#pragma once
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

namespace ppgu {

struct SampleRegion {
    uint64_t firstFrame = 0;
    uint64_t frameCount = 0;
    bool loop = false;
    bool oneShot = false;
};

// Immutable snapshot: audio thread reads raw pointers only after atomic publish.
struct SampleSnapshot {
    struct Voice {
        std::string sampleId;
        const float* data = nullptr;   // owned by pool block
        uint64_t frameCount = 0;
        int lokey = 0, hikey = 127;
        int lovel = 0, hivel = 127;
        int transpose = 0;
        int seqLength = 1, seqPosition = 1;
        SampleRegion region;
    };
    uint64_t generation = 0;
    std::vector<Voice> voices;
};

// Transactional pool with graveyard quiescence release; caps 128 snapshots / 8 roots.
class SamplePool {
public:
    static constexpr size_t kMaxSnapshots = 128;
    static constexpr size_t kMaxStreamingRoots = 8;

    void beginLoad();
    bool commit(std::shared_ptr<const SampleSnapshot> snap);
    std::shared_ptr<const SampleSnapshot> current() const;

    // Preset runtime: retire previous preset bank after quiescence check.
    void retireOld(uint64_t generation);
    size_t graveyardSize() const { return graveyard_.size(); }
    size_t activeVoices() const { return activeVoiceCount_.load(std::memory_order_relaxed); }
    void setVoiceCount(int n) { activeVoiceCount_.store(n, std::memory_order_relaxed); }

    size_t snapshotCount() const { return snapshots_.size(); }
    bool registerRoot(const std::string& root);
private:
    std::vector<std::shared_ptr<const SampleSnapshot>> graveyard_;
    std::vector<std::shared_ptr<const SampleSnapshot>> snapshots_;
    std::shared_ptr<const SampleSnapshot> current_;
    std::unordered_set<std::string> roots_;
    std::atomic<int> activeVoiceCount_{0};
    std::mutex mtx_;
};

} // namespace ppgu