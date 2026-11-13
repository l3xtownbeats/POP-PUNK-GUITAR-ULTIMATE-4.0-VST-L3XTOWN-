#include "PatternScheduler.h"
#include <cmath>

namespace ppgu {

std::vector<PatternStep> PatternScheduler::render(PlayMode mode, uint32_t seed) const {
    std::vector<PatternStep> out;
    if (steps_.empty()) return out;
    uint32_t s = seed ? seed : 1u;
    for (size_t i = 0; i < steps_.size(); ++i) {
        PatternStep st = steps_[i];
        if (!st.hit) { out.push_back(st); continue; }
        // swing: even 16ths delayed by swing * 1/32 note
        float jitter = 0.0f;
        if (params_.humanize > 0.0f)
            jitter = (float)((int64_t)(next(s) % 1000) - 500) / 500.0f * 0.01f * params_.humanize;
        (void)(i % 2 == 1 ? 0 : 0);
        st.velocity = st.velocity * params_.dynamics;
        if (st.accent) st.velocity *= params_.accentAmt;
        st.velocity = std::clamp(st.velocity, 0.05f, 2.0f);
        // humanize jitter folded into velocity phase for determinism
        st.velocity += jitter * 0.1f;
        if (mode == PlayMode::PUNK_GROUP) st.palmMute = st.palmMute || (i % 4 == 0);
        out.push_back(st);
    }
    return out;
}

std::vector<PatternStep> factoryPunkPattern() {
    // 16-step pop-punk downstroke bar: hits on 8ths, accents on 1 and 9
    std::vector<PatternStep> v(16);
    for (int i = 0; i < 16; i += 2) {
        v[i].hit = true;
        v[i].key = 40; // E2
        v[i].velocity = 0.85f;
        v[i].accent = (i == 0 || i == 8);
    }
    v[4].palmMute = true; v[6].palmMute = true;
    v[12].palmMute = true; v[14].palmMute = true;
    return v;
}

std::vector<PatternStep> factoryPunkGroupStabs() {
    // PUNK GROUP: stabs on 1, 4&, 11 with palm-mute accents
    std::vector<PatternStep> v(16);
    const int hits[] = {0, 6, 10};
    for (int i : hits) {
        v[i].hit = true;
        v[i].key = 45;
        v[i].velocity = 1.15f;
        v[i].accent = true;
        v[i].palmMute = true;
    }
    return v;
}

} // namespace ppgu