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

    // Frequency and Q controls must change the response, not merely retain
    // valid coefficients. Probe away from the centre frequencies so that
    // moving a control produces an observable response difference.
    const auto lowAt80 = ppgu::dsp::makeLowShelf(sampleRate, 80.0f, 6.0f);
    const auto lowAt400 = ppgu::dsp::makeLowShelf(sampleRate, 400.0f, 6.0f);
    const auto midAt800 = ppgu::dsp::makePeaking(sampleRate, 800.0f, 1.2f, 6.0f);
    const auto midAt2400 = ppgu::dsp::makePeaking(sampleRate, 2400.0f, 1.2f, 6.0f);
    const auto narrowMid = ppgu::dsp::makePeaking(sampleRate, 1200.0f, 5.0f, 6.0f);
    const auto wideMid = ppgu::dsp::makePeaking(sampleRate, 1200.0f, 0.3f, 6.0f);
    const auto highAt4000 = ppgu::dsp::makeHighShelf(sampleRate, 4000.0f, 6.0f);
    const auto highAt10000 = ppgu::dsp::makeHighShelf(sampleRate, 10000.0f, 6.0f);
    if (std::abs(magnitudeAt(lowAt80, 150.0, sampleRate)
                 - magnitudeAt(lowAt400, 150.0, sampleRate)) < 0.05
        || std::abs(magnitudeAt(midAt800, 1600.0, sampleRate)
                    - magnitudeAt(midAt2400, 1600.0, sampleRate)) < 0.05
        || std::abs(magnitudeAt(narrowMid, 1800.0, sampleRate)
                    - magnitudeAt(wideMid, 1800.0, sampleRate)) < 0.05
        || std::abs(magnitudeAt(highAt4000, 7000.0, sampleRate)
                    - magnitudeAt(highAt10000, 7000.0, sampleRate)) < 0.05)
    {
        std::cerr << "EQ frequency/Q controls did not produce distinct filter responses.\\n";
        return 1;
    }

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

    const auto stressed = ppgu::dsp::makePeaking(sampleRate, 3000.0f, 0.2f, 18.0f);
    const auto interpolatedStart = ppgu::dsp::interpolate(neutralMid, stressed, 0.0f);
    const auto interpolatedEnd = ppgu::dsp::interpolate(neutralMid, stressed, 1.0f);
    const auto interpolatedHalf = ppgu::dsp::interpolate(neutralMid, stressed, 0.5f);
    if (std::abs(interpolatedStart.b0 - neutralMid.b0) > 1.0e-7f
        || std::abs(interpolatedEnd.b0 - stressed.b0) > 1.0e-7f
        || !std::isfinite(interpolatedHalf.a1)
        || !std::isfinite(interpolatedHalf.a2))
    {
        std::cerr << "Biquad coefficient interpolation contract failed.\\n";
        return 1;
    }

    ppgu::dsp::BiquadState state;
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
