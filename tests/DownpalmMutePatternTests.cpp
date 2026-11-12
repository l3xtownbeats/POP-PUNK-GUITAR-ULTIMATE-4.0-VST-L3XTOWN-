#include "DownpalmMutePattern.h"

#include <cmath>
#include <iostream>

using namespace ppgu::performance;

int main()
{
    DownpalmMuteBar bar;
    if (!DownpalmMutePattern::createFourFourBar(180, 108, 0.82, bar))
        return 1;
    if (bar.bpm != 180 || bar.strokes.size() != 8)
        return 2;

    for (std::size_t i = 0; i < bar.strokes.size(); ++i)
    {
        const auto& stroke = bar.strokes[i];
        if (std::abs(stroke.beat - static_cast<double>(i) * 0.5) > 1.0e-12)
            return 3;
        if (std::abs(stroke.gateBeats - 0.41) > 1.0e-12)
            return 4;
        if (stroke.velocity != 108 || stroke.direction != StrumDirection::Down
            || stroke.articulation != GuitarArticulation::PalmMute)
            return 5;
    }

    DownpalmMuteBar unchanged;
    unchanged.bpm = 177;
    if (DownpalmMutePattern::createFourFourBar(159, 100, 0.8, unchanged)
        || unchanged.bpm != 177)
        return 6;
    if (DownpalmMutePattern::createFourFourBar(201, 100, 0.8, unchanged))
        return 7;
    if (DownpalmMutePattern::createFourFourBar(180, 0, 0.8, unchanged))
        return 8;
    if (DownpalmMutePattern::createFourFourBar(180, 128, 0.8, unchanged))
        return 9;
    if (DownpalmMutePattern::createFourFourBar(180, 100, 0.0, unchanged))
        return 10;
    if (DownpalmMutePattern::createFourFourBar(180, 100, 1.01, unchanged))
        return 11;

    DownpalmMuteBar fast;
    if (!DownpalmMutePattern::createFourFourBar(200, 127, 1.0, fast)
        || fast.strokes[7].beat != 3.5 || fast.strokes[7].gateBeats != 0.5)
        return 12;

    std::cout << "Downpalm mute PME contract passed: 4/4, eight even downstrokes, palm mute, 160-200 BPM.\n";
    return 0;
}
