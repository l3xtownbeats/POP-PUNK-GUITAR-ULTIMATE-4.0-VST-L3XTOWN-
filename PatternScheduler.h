#pragma once
#include <array>
#include <cstdint>
#include <vector>

namespace ppgu {

enum class PlayMode { MELODY, PATTERN, PUNK_GROUP };

struct PatternStep {
    bool hit = false;
    int  key = 48;       // MIDI note (E2 ..)
    float velocity = 0.8f; // 0..2 (1 = nominal)
    bool accent = false;
    bool palmMute = false; // PUNK GROUP stabs
};

struct SchedulerParams {
    float swing = 0.0f;    // 0..1
    float humanize = 0.0f; // 0..1 timing jitter
    float dynamics = 1.0f; // velocity scaling
    float accentAmt = 1.0f;
};

// Deterministic 16-step pattern engine (like Session Guitarist pattern modes).
class PatternScheduler {
public:
    void setPattern(const std::vector<PatternStep>& steps) { steps_ = steps; }
    void setParams(const SchedulerParams& p) { params_ = p; }

    // Renders step events for a 16th-note grid; deterministic (seeded LCG, no time-dependence).
    std::vector<PatternStep> render(PlayMode mode, uint32_t seed = 1u) const;

private:
    static uint32_t next(uint32_t& s) { s = s * 1664525u + 1013904223u; return s; }
    std::vector<PatternStep> steps_;
    SchedulerParams params_;
};

// Factory patterns (approved in journal): punk downstroke verse, palm-muted stab bar.
std::vector<PatternStep> factoryPunkPattern();
std::vector<PatternStep> factoryPunkGroupStabs();

} // namespace ppgu