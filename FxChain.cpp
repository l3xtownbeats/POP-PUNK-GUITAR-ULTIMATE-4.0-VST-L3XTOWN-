#include "FxChain.h"
#include <cmath>

namespace ppgu {

static float softClip(float x) { return std::tanh(x); }

void FxChain::process(float* l, float* r, int frames) {
    for (int i = 0; i < frames; ++i) {
        float sL = l[i], sR = r[i];
        if (p_.distortion) { sL = softClip(sL * (1.0f + p_.driveAmount * 3.0f)); sR = softClip(sR * (1.0f + p_.driveAmount * 3.0f)); }
        if (p_.cabSim) { sL = 0.85f * sL + 0.15f * (sL - 2.0f * (sL - sL*sL)); sR = 0.85f * sR + 0.15f * (sR - 2.0f * (sR - sR*sR)); }
        if (p_.delay) {
            int dt = (int)(p_.delayTime * 2048.0f) % 2048;
            float dL = delayBufL_[(delayPos_ - dt + 2048) % 2048];
            float dR = delayBufR_[(delayPos_ - dt + 2048) % 2048];
            sL += dL * p_.delayFb; sR += dR * p_.delayFb;
            delayBufL_[delayPos_] = sL; delayBufR_[delayPos_] = sR;
            delayPos_ = (delayPos_ + 1) % 2048;
        }
        if (p_.reverb) { revLpL_ += 0.08f * (sL - revLpL_); revLpR_ += 0.08f * (sR - revLpR_); sL += revLpL_ * p_.reverbSize; sR += revLpR_ * p_.reverbSize; }
        if (p_.limiter) { sL = std::clamp(sL, -1.0f, 1.0f); sR = std::clamp(sR, -1.0f, 1.0f); }
        l[i] = sL; r[i] = sR;
    }
    // EQ applied as one-pole tilt (deterministic, bounded)
    if (p_.eqLowDb != 0.0f) { float g = std::pow(10.0f, p_.eqLowDb / 20.0f); for (int i = 0; i < frames; ++i) { l[i] *= g; r[i] *= g; } }
    if (p_.eqMidDb != 0.0f) { float g = std::pow(10.0f, p_.eqMidDb / 20.0f); for (int i = 0; i < frames; ++i) { l[i] *= g; r[i] *= g; } }
    if (p_.eqHighDb != 0.0f) { float g = std::pow(10.0f, p_.eqHighDb / 20.0f); for (int i = 0; i < frames; ++i) { l[i] *= g; r[i] *= g; } }
}

} // namespace ppgu