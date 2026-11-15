#pragma once

#include <algorithm>
#include <cmath>

namespace ppgu::dsp
{
struct BiquadCoefficients
{
    float b0 = 1.0f;
    float b1 = 0.0f;
    float b2 = 0.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
};

struct BiquadState
{
    float z1 = 0.0f;
    float z2 = 0.0f;

    void reset() noexcept { z1 = z2 = 0.0f; }

    float process(float input, const BiquadCoefficients& c) noexcept
    {
        const float output = c.b0 * input + z1;
        z1 = c.b1 * input - c.a1 * output + z2;
        z2 = c.b2 * input - c.a2 * output;
        return output;
    }
};

namespace detail
{
inline float safeFrequency(float frequency, float sampleRate) noexcept
{
    const float safeRate = std::max(8000.0f, sampleRate);
    return std::clamp(frequency, 10.0f, safeRate * 0.45f);
}

inline BiquadCoefficients normalize(float b0, float b1, float b2,
                                    float a0, float a1, float a2) noexcept
{
    if (!std::isfinite(a0) || std::abs(a0) < 1.0e-12f)
        return {};
    const float inverseA0 = 1.0f / a0;
    return { b0 * inverseA0, b1 * inverseA0, b2 * inverseA0,
             a1 * inverseA0, a2 * inverseA0 };
}
}

// RBJ Audio EQ Cookbook filters. Coefficients are plain values: calculating
// them in processBlock performs no heap allocation and keeps state per channel.
inline BiquadCoefficients interpolate(const BiquadCoefficients& from,
                                           const BiquadCoefficients& to,
                                           float proportion) noexcept
{
    const float t = std::clamp(proportion, 0.0f, 1.0f);
    return {
        from.b0 + (to.b0 - from.b0) * t,
        from.b1 + (to.b1 - from.b1) * t,
        from.b2 + (to.b2 - from.b2) * t,
        from.a1 + (to.a1 - from.a1) * t,
        from.a2 + (to.a2 - from.a2) * t
    };
}

inline BiquadCoefficients makePeaking(float sampleRate, float frequency,
                                      float q, float gainDb) noexcept
{
    const float fs = std::max(8000.0f, sampleRate);
    const float f = detail::safeFrequency(frequency, fs);
    const float safeQ = std::clamp(q, 0.1f, 10.0f);
    const float a = std::pow(10.0f, std::clamp(gainDb, -24.0f, 24.0f) / 40.0f);
    const float w0 = 6.2831853071795864769f * f / fs;
    const float cosine = std::cos(w0);
    const float alpha = std::sin(w0) / (2.0f * safeQ);
    return detail::normalize(1.0f + alpha * a, -2.0f * cosine,
                             1.0f - alpha * a, 1.0f + alpha / a,
                             -2.0f * cosine, 1.0f - alpha / a);
}

inline BiquadCoefficients makeLowShelf(float sampleRate, float frequency,
                                       float gainDb) noexcept
{
    const float fs = std::max(8000.0f, sampleRate);
    const float f = detail::safeFrequency(frequency, fs);
    const float a = std::pow(10.0f, std::clamp(gainDb, -24.0f, 24.0f) / 40.0f);
    const float w0 = 6.2831853071795864769f * f / fs;
    const float cosine = std::cos(w0);
    const float sine = std::sin(w0);
    constexpr float slope = 1.0f;
    const float alpha = 0.5f * sine * std::sqrt((a + 1.0f / a) * (1.0f / slope - 1.0f) + 2.0f);
    const float beta = 2.0f * std::sqrt(a) * alpha;
    return detail::normalize(
        a * ((a + 1.0f) - (a - 1.0f) * cosine + beta),
        2.0f * a * ((a - 1.0f) - (a + 1.0f) * cosine),
        a * ((a + 1.0f) - (a - 1.0f) * cosine - beta),
        (a + 1.0f) + (a - 1.0f) * cosine + beta,
        -2.0f * ((a - 1.0f) + (a + 1.0f) * cosine),
        (a + 1.0f) + (a - 1.0f) * cosine - beta);
}

inline BiquadCoefficients makeHighShelf(float sampleRate, float frequency,
                                        float gainDb) noexcept
{
    const float fs = std::max(8000.0f, sampleRate);
    const float f = detail::safeFrequency(frequency, fs);
    const float a = std::pow(10.0f, std::clamp(gainDb, -24.0f, 24.0f) / 40.0f);
    const float w0 = 6.2831853071795864769f * f / fs;
    const float cosine = std::cos(w0);
    const float sine = std::sin(w0);
    constexpr float slope = 1.0f;
    const float alpha = 0.5f * sine * std::sqrt((a + 1.0f / a) * (1.0f / slope - 1.0f) + 2.0f);
    const float beta = 2.0f * std::sqrt(a) * alpha;
    return detail::normalize(
        a * ((a + 1.0f) + (a - 1.0f) * cosine + beta),
        -2.0f * a * ((a - 1.0f) + (a + 1.0f) * cosine),
        a * ((a + 1.0f) + (a - 1.0f) * cosine - beta),
        (a + 1.0f) - (a - 1.0f) * cosine + beta,
        2.0f * ((a - 1.0f) - (a + 1.0f) * cosine),
        (a + 1.0f) - (a - 1.0f) * cosine - beta);
}
} // namespace ppgu::dsp
