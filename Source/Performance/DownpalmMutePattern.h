#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace ppgu::performance
{
    enum class StrumDirection : std::uint8_t { Down = 0 };
    enum class GuitarArticulation : std::uint8_t { PalmMute = 0 };

    struct DownpalmMuteStroke
    {
        double beat = 0.0;
        double gateBeats = 0.0;
        std::uint8_t velocity = 100;
        StrumDirection direction = StrumDirection::Down;
        GuitarArticulation articulation = GuitarArticulation::PalmMute;
    };

    struct DownpalmMuteBar
    {
        static constexpr std::size_t strokeCount = 8;
        static constexpr double beatsPerBar = 4.0;
        static constexpr double eighthNoteBeats = 0.5;
        int bpm = 180;
        std::array<DownpalmMuteStroke, strokeCount> strokes {};
    };

    // Canonical PME groove: eight mechanically even eighth-note downstrokes
    // per 4/4 bar. Every stroke is palm-muted and down-only by construction.
    // Sampler articulation mapping and chord/fretboard voicing are separate
    // integration work; this contract alone does not produce audio.
    class DownpalmMutePattern final
    {
    public:
        static constexpr int minimumBpm = 160;
        static constexpr int maximumBpm = 200;

        [[nodiscard]] static bool createFourFourBar(int bpm, int velocity,
                                                    double gateRatio,
                                                    DownpalmMuteBar& destination) noexcept
        {
            if (bpm < minimumBpm || bpm > maximumBpm
                || velocity < 1 || velocity > 127
                || !std::isfinite(gateRatio)
                || gateRatio <= 0.0 || gateRatio > 1.0)
                return false;

            DownpalmMuteBar candidate;
            candidate.bpm = bpm;
            const auto gate = DownpalmMuteBar::eighthNoteBeats * gateRatio;
            for (std::size_t i = 0; i < candidate.strokes.size(); ++i)
                candidate.strokes[i] = {
                    static_cast<double>(i) * DownpalmMuteBar::eighthNoteBeats,
                    gate, static_cast<std::uint8_t>(velocity),
                    StrumDirection::Down, GuitarArticulation::PalmMute
                };
            destination = candidate;
            return true;
        }
    };
}
