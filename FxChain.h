#pragma once
#include <array>
#include <cstdint>

namespace ppgu {

// 8 FX + Limiter + EQ; all toggles truthful; no allocation in process().
struct FxParams {
    bool distortion=false, chorus=false, delay=false, reverb=false,
         compressor=false, limiter=false, cabSim=false, flanger=false;
    float eqLowDb=0, eqMidDb=0, eqHighDb=0;
    float driveAmount=0.5f, delayTime=0.25f, delayFb=0.35f, reverbSize=0.5f;
};

class FxChain {
public:
    void setParams(const FxParams& p) { p_ = p; }
    void process(float* l, float* r, int frames);
    FxParams& params() { return p_; }
private:
    FxParams p_;
    std::array<float, 2048> delayBufL_{}; std::array<float, 2048> delayBufR_{};
    int delayPos_ = 0;
    float revLpL_ = 0, revLpR_ = 0;
};

} // namespace ppgu