#include "DSP/BiquadFilter.h"

#include <cmath>
#include <complex>
#include <iostream>

namespace
{
double magnitudeAt(const ppgu::dsp::BiquadCoefficients& c, double frequency, double sampleRate)
{
    const auto z = std::exp(std::complex<double>(0.0, -6.2831853071795864769 * frequency / sampleRate));
    const auto z2 = z * z;
    const auto numerator = static_cast<double>(c.b0) + static_cast<double>(c.b1) * z
        + static_cast<double>(c.b2) * z2;
    const auto denominator = 1.0 + static_cast<double>(c.a1) * z
        + static_cast<double>(c.a2) * z2;
    return std::abs(numerator / denominator);
}

bool nearDb(double actual, double expected, double tolerance = 0.35)
{
    return std::abs(20.0 * std::log10(std::max(actual, 1.0e-12)) - expected) <= tolerance;
}
}

int main()
{
    constexpr float sampleRate = 48000.0f;
    const auto low = ppgu::dsp::makeLowShelf(sampleRate, 180.0f, 6.0f);
    const auto mid = ppgu::dsp::makePeaking(sampleRate, 1200.0f, 1.2f, 6.0f);
    const auto high = ppgu::dsp::makeHighShelf(sampleRate, 7000.0f, 6.0f);

    if (!nearDb(magnitudeAt(low, 30.0, sampleRate), 6.0)
        || !nearDb(magnitudeAt(low, 10000.0, sampleRate), 0.0)
        || !nearDb(magnitudeAt(mid, 1200.0, sampleRate), 6.0)
        || !nearDb(magnitudeAt(mid, 80.0, sampleRate), 0.0)
        || !nearDb(magnitudeAt(high, 16000.0, sampleRate), 6.0)
        || !nearDb(magnitudeAt(high, 100.0, sampleRate), 0.0))
    {
        std::cerr << "Biquad EQ frequency/gain response contract failed.\n";
        return 1;
    }

    const auto neutralLow = ppgu::dsp::makeLowShelf(sampleRate, 180.0f, 0.0f);
    const auto neutralMid = ppgu::dsp::makePeaking(sampleRate, 1200.0f, 1.2f, 0.0f);
    const auto neutralHigh = ppgu::dsp::makeHighShelf(sampleRate, 7000.0f, 0.0f);
    for (const auto* coefficients : { &neutralLow, &neutralMid, &neutralHigh })
        if (!nearDb(magnitudeAt(*coefficients, 1000.0, sampleRate), 0.0, 0.001))
        {
            std::cerr << "Zero-gain EQ must be neutral.\n";
            return 1;
        }

    ppgu::dsp::BiquadState state;
    const auto stressed = ppgu::dsp::makePeaking(sampleRate, 3000.0f, 0.2f, 18.0f);
    for (int i = 0; i < 200000; ++i)
    {
        const float input = 0.5f * std::sin(6.2831853071795864769f * 440.0f
                                            * static_cast<float>(i) / sampleRate);
        const float output = state.process(input, stressed);
        if (!std::isfinite(output) || std::abs(output) > 20.0f)
        {
            std::cerr << "Biquad state became unstable under sustained input.\n";
            return 1;
        }
    }

    std::cout << "DSP biquad filter contracts passed.\n";
    return 0;
}
