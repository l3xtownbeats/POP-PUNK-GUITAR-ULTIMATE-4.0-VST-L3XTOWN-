#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace ppgu::performance
{
enum class StyleArticulation : std::uint8_t
{
    Open, PalmMute, HammerOn, PullOff
};

// Style DNA is intentionally content-agnostic: presets inherit a group profile
// and may override these normalized controls without changing the phrase data.
struct StyleDNA
{
    float velocityLow = 0.90f;
    float velocityHigh = 1.08f;
    float chugDensity = 0.25f;
    float hammerOnProbability = 0.10f;
    float pullOffProbability = 0.10f;
    float stringSpreadMs = 8.0f;
    float downbeatAccent = 1.12f;
    float backbeatAccent = 1.04f;
    float offbeatAccent = 0.94f;
    float fxTaste = 0.5f;
};

struct StyleEventInput
{
    double beatInBar = 0.0;      // Quarter-note position in a 4/4 bar.
    double durationBeats = 0.25;
    int note = 60;
    int velocity = 100;
    int channel = 1;
    std::uint32_t eventIndex = 0; // Stable phrase-local index; no mutable RNG state.
    bool legatoCandidate = false;
    bool ascendingFromPrevious = true;
    bool shortRhythmicHit = false;
};

struct StyledEvent
{
    int note = 60;
    int velocity = 100;
    int channel = 1;
    float strumSpreadMs = 0.0f;
    float fxTaste = 0.5f;
    StyleArticulation articulation = StyleArticulation::Open;
};

class StyleEngine final
{
public:
    static StyleDNA sanitize(StyleDNA dna) noexcept
    {
        dna.velocityLow = clampFinite(dna.velocityLow, 0.0f, 2.0f, 0.9f);
        dna.velocityHigh = clampFinite(dna.velocityHigh, 0.0f, 2.0f, 1.08f);
        dna.chugDensity = clampFinite(dna.chugDensity, 0.0f, 1.0f, 0.25f);
        dna.hammerOnProbability = clampFinite(dna.hammerOnProbability, 0.0f, 1.0f, 0.1f);
        dna.pullOffProbability = clampFinite(dna.pullOffProbability, 0.0f, 1.0f, 0.1f);
        dna.stringSpreadMs = clampFinite(dna.stringSpreadMs, 0.0f, 35.0f, 8.0f);
        dna.downbeatAccent = clampFinite(dna.downbeatAccent, 0.0f, 2.0f, 1.12f);
        dna.backbeatAccent = clampFinite(dna.backbeatAccent, 0.0f, 2.0f, 1.04f);
        dna.offbeatAccent = clampFinite(dna.offbeatAccent, 0.0f, 2.0f, 0.94f);
        dna.fxTaste = clampFinite(dna.fxTaste, 0.0f, 1.0f, 0.5f);
        return dna;
    }

    // Pure, deterministic transform: same DNA + phrase event always yields the
    // same result. Safe for real-time use (fixed arithmetic, no allocation,
    // locks, I/O, or mutable global/random state).
    static StyledEvent process(const StyleDNA& rawDNA, const StyleEventInput& input) noexcept
    {
        const auto dna = sanitize(rawDNA);
        StyledEvent out;
        out.note = clampInt(input.note, 0, 127);
        out.channel = clampInt(input.channel, 1, 16);
        out.fxTaste = dna.fxTaste;
        out.strumSpreadMs = dna.stringSpreadMs;

        const int velocity = clampInt(input.velocity, 1, 127);
        const float normalized = static_cast<float>(velocity - 1) / 126.0f;
        const float velocityGain = dna.velocityLow
            + (dna.velocityHigh - dna.velocityLow) * normalized;
        float accent = dna.offbeatAccent;
        if (std::isfinite(input.beatInBar))
        {
            const double beat = positiveModulo(input.beatInBar, 4.0);
            if (beat < 0.125 || beat >= 3.875)
                accent = dna.downbeatAccent;
            else if (beat >= 1.875 && beat < 2.125)
                accent = dna.backbeatAccent;
        }
        const float scaled = static_cast<float>(velocity) * velocityGain * accent;
        out.velocity = clampInt(static_cast<int>(std::lround(scaled)), 1, 127);

        const auto roll = unitRoll(input.eventIndex);
        if (input.legatoCandidate)
        {
            const float probability = input.ascendingFromPrevious
                ? dna.hammerOnProbability : dna.pullOffProbability;
            if (roll < probability)
                out.articulation = input.ascendingFromPrevious
                    ? StyleArticulation::HammerOn : StyleArticulation::PullOff;
        }
        if (out.articulation == StyleArticulation::Open
            && input.shortRhythmicHit && roll < dna.chugDensity)
            out.articulation = StyleArticulation::PalmMute;
        return out;
    }

private:
    static float clampFinite(float value, float low, float high, float fallback) noexcept
    {
        return std::isfinite(value) ? (std::max)(low, (std::min)(high, value)) : fallback;
    }
    static int clampInt(int value, int low, int high) noexcept
    {
        return (std::max)(low, (std::min)(high, value));
    }
    static double positiveModulo(double value, double modulus) noexcept
    {
        const double result = std::fmod(value, modulus);
        return result < 0.0 ? result + modulus : result;
    }
    static float unitRoll(std::uint32_t value) noexcept
    {
        std::uint32_t x = value + 0x9e3779b9u;
        x ^= x >> 16;
        x *= 0x7feb352du;
        x ^= x >> 15;
        x *= 0x846ca68bu;
        x ^= x >> 16;
        return static_cast<float>(x >> 8) * (1.0f / 16777216.0f);
    }
};
} // namespace ppgu::performance
