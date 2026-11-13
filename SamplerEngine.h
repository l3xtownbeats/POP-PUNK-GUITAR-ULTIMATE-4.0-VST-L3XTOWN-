#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>
#include "SamplePool.h"

namespace ppgu {

// 64 preallocated voice slots; RR selection; cache miss never blocks audio thread.
class SamplerEngine {
public:
    static constexpr int kVoiceSlots = 64;

    struct Voice {
        bool active = false;
        int key = 0;
        float velocity = 1.0f;
        uint64_t cursor = 0;
        const float* data = nullptr;
        uint64_t frameCount = 0;
        bool loop = false, oneShot = false;
        int channel = 0;
        uint64_t snapshotGen = 0;
    };

    void setSnapshot(std::shared_ptr<const SampleSnapshot> snap);
    void noteOn(int key, float velocity, int channel);
    void noteOff(int key, int channel);
    int activeCount() const { return active_.load(std::memory_order_relaxed); }
    void setPoolVoiceCount(int n) { if (pool_) pool_->setVoiceCount(n); }
    void bindPool(SamplePool* p) { pool_ = p; }

    // Renders one block; pure, no allocation, no locks on audio path.
    void process(float* outL, float* outR, int frames, double sampleRate);

private:
    int pickVoiceRR();
    std::array<Voice, kVoiceSlots> voices_{};
    std::shared_ptr<const SampleSnapshot> snap_;
    uint64_t rrCounter_ = 0;
    std::atomic<int> active_{0};
    SamplePool* pool_ = nullptr;
    double sr_ = 44100.0;
};

} // namespace ppgu