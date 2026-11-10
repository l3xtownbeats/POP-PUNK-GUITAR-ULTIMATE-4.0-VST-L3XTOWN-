#include "StyleEngine.h"
#include <cmath>
#include <iostream>
#include <limits>

using namespace ppgu::performance;

int main()
{
    StyleDNA dna;
    dna.velocityLow = 1.0f;
    dna.velocityHigh = 1.0f;
    dna.downbeatAccent = 1.5f;
    dna.backbeatAccent = 1.25f;
    dna.offbeatAccent = 0.5f;
    dna.chugDensity = 1.0f;
    dna.hammerOnProbability = 1.0f;
    dna.pullOffProbability = 1.0f;
    dna.stringSpreadMs = 50.0f;
    dna.fxTaste = 2.0f;

    const StyleEventInput downbeat { 0.0, 0.25, 64, 80, 1, 10, false, true, false };
    const auto accented = StyleEngine::process(dna, downbeat);
    if (accented.velocity != 120 || accented.articulation != StyleArticulation::Open) return 1;
    if (accented.strumSpreadMs != 35.0f || accented.fxTaste != 1.0f) return 2;

    const StyleEventInput offbeat { 1.0, 0.25, 64, 80, 1, 10, false, true, true };
    const auto muted = StyleEngine::process(dna, offbeat);
    if (muted.velocity != 40 || muted.articulation != StyleArticulation::PalmMute) return 3;

    const StyleEventInput ascending { 0.5, 0.5, 65, 90, 1, 2, true, true, false };
    if (StyleEngine::process(dna, ascending).articulation != StyleArticulation::HammerOn) return 4;
    const StyleEventInput descending { 0.5, 0.5, 64, 90, 1, 2, true, false, false };
    const auto pull = StyleEngine::process(dna, descending);
    if (pull.articulation != StyleArticulation::PullOff) return 5;
    const auto repeat = StyleEngine::process(dna, descending);
    if (repeat.velocity != pull.velocity || repeat.articulation != pull.articulation
        || repeat.note != pull.note) return 6;

    StyleDNA invalid;
    invalid.velocityLow = std::numeric_limits<float>::quiet_NaN();
    invalid.stringSpreadMs = std::numeric_limits<float>::infinity();
    invalid.fxTaste = -100.0f;
    const auto safe = StyleEngine::process(invalid,
        { std::numeric_limits<double>::quiet_NaN(), 0.25, 200, 0, 0, 0, false, true, false });
    if (safe.note != 127 || safe.velocity < 1 || safe.velocity > 127
        || safe.channel != 1 || safe.strumSpreadMs != 8.0f || safe.fxTaste != 0.0f) return 7;

    const auto same = StyleEngine::process(StyleDNA{}, { 0.0, 0.25, 60, 100, 1, 42, false, true, false });
    const auto sameAgain = StyleEngine::process(StyleDNA{}, { 0.0, 0.25, 60, 100, 1, 42, false, true, false });
    if (same.velocity != sameAgain.velocity || same.articulation != sameAgain.articulation) return 8;

    std::cout << "StyleEngine deterministic DNA tests passed\n";
    return 0;
}
