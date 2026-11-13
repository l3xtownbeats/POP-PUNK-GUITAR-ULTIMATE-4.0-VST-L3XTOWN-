#include "SamplerEngine.h"
#include <cmath>

namespace ppgu {

void SamplerEngine::setSnapshot(std::shared_ptr<const SampleSnapshot> s) {
    snap_ = std::move(s); // atomic shared_ptr swap; audio thread picks it up on next block
}

int SamplerEngine::pickVoiceRR() {
    for (int i = 0; i < kVoiceSlots; ++i) {
        int idx = (int)((rrCounter_ + (uint64_t)i) % kVoiceSlots);
        if (!voices_[idx].active) { ++rrCounter_; return idx; }
    }
    // voice stealing: oldest slot wins (deterministic)
    int idx = (int)(rrCounter_ % kVoiceSlots);
    ++rrCounter_;
    return idx;
}

void SamplerEngine::noteOn(int key, float velocity, int channel) {
    auto snap = std::atomic_load(&snap_);
    if (!snap) return;
    const SampleSnapshot::Voice* chosen = nullptr;
    int rr = (int)(rrCounter_ % (snap->voices.empty() ? 1 : snap->voices.size()));
    for (size_t i = 0; i < snap->voices.size(); ++i) {
        const auto& v = snap->voices[(i + (size_t)rr) % snap->voices.size()];
        if (key < v.lokey || key > v.hikey) continue;
        float vel = velocity * 127.0f;
        if (vel < (float)v.lovel || vel > (float)v.hivel) continue;
        if (v.seqPosition == (int)(1 + (rrCounter_ % (uint64_t)std::max(1, v.seqLength)))) { chosen = &v; break; }
    }
    if (!chosen) return; // cache miss: never block; voice waits (cursor stays)
    int slot = pickVoiceRR();
    Voice& vc = voices_[slot];
    bool wasActive = vc.active;
    vc.active = true; vc.key = key; vc.velocity = velocity; vc.cursor = 0;
    vc.data = chosen->data; vc.frameCount = chosen->region.frameCount;
    vc.loop = chosen->region.loop; vc.oneShot = chosen->region.oneShot;
    vc.channel = channel; vc.snapshotGen = snap->generation;
    if (!wasActive) active_.fetch_add(1, std::memory_order_relaxed);
    if (pool_) pool_->setVoiceCount(active_.load(std::memory_order_relaxed));
}

void SamplerEngine::noteOff(int key, int channel) {
    for (auto& v : voices_) {
        if (v.active && v.key == key && v.channel == channel) {
            v.active = false;
            active_.fetch_sub(1, std::memory_order_relaxed);
        }
    }
    if (pool_) pool_->setVoiceCount(active_.load(std::memory_order_relaxed));
}

void SamplerEngine::process(float* outL, float* outR, int frames, double sampleRate) {
    sr_ = sampleRate;
    auto snap = std::atomic_load(&snap_);
    for (int f = 0; f < frames; ++f) {
        float l = 0.0f, r = 0.0f;
        for (auto& v : voices_) {
            if (!v.active || !v.data) continue;
            if (v.cursor >= v.frameCount) {
                if (v.loop && v.frameCount > 0) v.cursor = 0;
                else { v.active = false; active_.fetch_sub(1, std::memory_order_relaxed); continue; }
            }
            float s = v.data[v.cursor] * v.velocity * 0.3f;
            l += s; r += s;
            ++v.cursor;
        }
        outL[f] = l; outR[f] = r;
    }
}

} // namespace ppgu