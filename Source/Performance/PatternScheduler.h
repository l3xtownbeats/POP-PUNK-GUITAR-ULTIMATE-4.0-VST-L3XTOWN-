#pragma once

#include <array>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <limits>

namespace ppgu::performance
{
    // Immutable transport snapshot for one audio block. PPQ is the position
    // at the first sample of the block; no host pointers cross the scheduler.
    struct TransportSnapshot
    {
        bool hostValid = false;
        bool playing = false;
        double bpm = 120.0;
        double ppq = 0.0;
    };

    struct PatternEvent
    {
        double beat = 0.0;       // Offset inside the pattern, in quarter notes.
        double duration = 0.25;  // Gate length in quarter notes.
        std::uint8_t note = 60;
        std::uint8_t velocity = 100;
        std::uint8_t channel = 1;
    };

    enum class EventKind : std::uint8_t { NoteOff = 0, NoteOn = 1 };

    struct ScheduledEvent
    {
        int sampleOffset = 0;
        EventKind kind = EventKind::NoteOn;
        int note = 60;
        int velocity = 100;
        int channel = 1;
    };

    struct BlockClock
    {
        bool running = false;
        bool usingHost = false;
        bool transportStopped = false;
        bool transportDiscontinuity = false;
        double bpm = 120.0;
        double startPpq = 0.0;
        double endPpq = 0.0;
    };

    class PatternScheduler final
    {
    public:
        static constexpr std::size_t maximumEventsPerBlock = 512;

        // Host mode follows the DAW transport. Internal mode advances a
        // deterministic PPQ clock. Host Start waits for host playback, then
        // follows host tempo/position. Invalid host data falls back safely.
        BlockClock beginBlock(const TransportSnapshot& host,
                              bool useInternalClock,
                              double fallbackBpm,
                              double sampleRate,
                              int blockSamples) noexcept
        {
            BlockClock result;
            result.bpm = clampBpm(fallbackBpm);
            result.usingHost = host.hostValid && !useInternalClock;
            const bool clockSourceChanged = wasRunning_ && result.usingHost != previousUsingHost_;

            if (result.usingHost)
            {
                result.running = host.playing;
                result.bpm = clampBpm(host.bpm);
                result.startPpq = finiteOr(host.ppq, internalPpq_);
                result.endPpq = result.startPpq
                    + (result.running ? samplesToBeats(blockSamples, sampleRate, result.bpm) : 0.0);
                result.transportStopped = wasRunning_ && !result.running;
                if (result.running)
                {
                    const double expectedSpan = samplesToBeats(blockSamples, sampleRate, result.bpm);
                    const double tolerance = std::max(0.01, expectedSpan * 2.0 + 0.001);
                    result.transportDiscontinuity = clockSourceChanged
                        || (hostWasRunning_ && haveHostPosition_
                            && std::abs(result.startPpq - lastHostEndPpq_) > tolerance);
                    if (result.transportDiscontinuity)
                        pendingCount_ = 0;
                    lastHostEndPpq_ = result.endPpq;
                    haveHostPosition_ = true;
                    hostWasRunning_ = true;
                    internalPpq_ = result.endPpq;
                }
                else
                {
                    result.transportDiscontinuity = hostWasRunning_;
                    pendingCount_ = 0;
                    hostWasRunning_ = false;
                    haveHostPosition_ = false;
                    internalPpq_ = result.startPpq;
                }
                wasRunning_ = result.running;
                previousUsingHost_ = result.usingHost;
                return result;
            }

            result.usingHost = false;
            if (hostWasRunning_ || clockSourceChanged)
            {
                result.transportDiscontinuity = true;
                pendingCount_ = 0;
            }
            hostWasRunning_ = false;
            haveHostPosition_ = false;
            result.running = true;
            result.startPpq = internalPpq_;
            result.endPpq = internalPpq_ + samplesToBeats(blockSamples, sampleRate, result.bpm);
            internalPpq_ = result.endPpq;
            wasRunning_ = true;
            previousUsingHost_ = false;
            return result;
        }

        // Schedule events whose absolute pattern-cycle PPQ falls within this
        // block. Fixed storage only: safe for the audio thread.
        template <std::size_t N>
        std::size_t schedule(const std::array<PatternEvent, N>& pattern,
                             double patternLengthBeats,
                             double cycleStartPpq,
                             const BlockClock& clock,
                             double sampleRate,
                             int blockSamples,
                             double quantizeBeats,
                             int transpose,
                             std::array<ScheduledEvent, maximumEventsPerBlock>& output) noexcept
        {
            if (!clock.running)
            {
                pendingCount_ = 0;
                return 0;
            }
            if (blockSamples <= 0 || !std::isfinite(sampleRate) || sampleRate <= 0.0
                || !std::isfinite(patternLengthBeats) || patternLengthBeats <= 0.0
                || !std::isfinite(cycleStartPpq) || !std::isfinite(quantizeBeats)
                || !std::isfinite(clock.startPpq) || !std::isfinite(clock.endPpq)
                || !std::isfinite(clock.bpm) || clock.bpm < 20.0 || clock.bpm > 300.0
                || clock.endPpq <= clock.startPpq)
                return 0;

            std::size_t count = 0;
            const double samplesPerBeat = sampleRate * 60.0 / clock.bpm;
            const auto emit = [&](double eventPpq, EventKind kind, int note, int velocity, int channel)
            {
                if (count >= output.size() || eventPpq < clock.startPpq || eventPpq >= clock.endPpq)
                    return false;

                // Dispatch at the first sample at or after the event. A
                // fractional event may map to sample blockSamples (the first
                // sample of the NEXT block); never clamp it back to the last
                // sample of this block, because that would play it early.
                const double exactOffset = (eventPpq - clock.startPpq) * samplesPerBeat;
                const int offset = static_cast<int>(std::ceil(exactOffset - 1.0e-9));
                if (offset < 0 || offset >= blockSamples)
                    return false;

                output[count++] = { offset, kind, clampInt(note, 0, 127),
                                    clampInt(velocity, 0, 127), clampInt(channel, 1, 16) };
                return true;
            };

            const auto retainPending = [&](double ppq, EventKind kind, int note,
                                           int velocity, int channel)
            {
                if (pendingCount_ < pendingEvents_.size())
                    pendingEvents_[pendingCount_++] = { ppq, kind, note, velocity, channel };
                else
                    pendingOverflow_ = true;
            };

            std::size_t retained = 0;
            for (std::size_t i = 0; i < pendingCount_; ++i)
            {
                const auto pending = pendingEvents_[i];
                const double duePpq = pending.ppq < clock.startPpq ? clock.startPpq : pending.ppq;
                if (pending.ppq < clock.endPpq
                    && emit(duePpq, pending.kind, pending.note, pending.velocity, pending.channel))
                    continue;

                pendingEvents_[retained++] = pending;
            }
            pendingCount_ = retained;

            // Treat cycleStartPpq as the pattern's PPQ origin. Walk every
            // cycle intersecting this block so a block that straddles a bar
            // boundary cannot skip the first step of the next repetition.
            // The loop is bounded by the block's PPQ span and fixed output
            // capacity; no dynamic storage or host calls occur here.
            const double firstCycleValue = std::floor((clock.startPpq - cycleStartPpq) / patternLengthBeats);
            const double lastCycleValue = std::floor((clock.endPpq - cycleStartPpq) / patternLengthBeats);
            constexpr long double maxCycle = static_cast<long double>(std::numeric_limits<long long>::max() - 1);
            if (!std::isfinite(firstCycleValue) || !std::isfinite(lastCycleValue)
                || static_cast<long double>(firstCycleValue) > maxCycle
                || static_cast<long double>(lastCycleValue) > maxCycle)
            {
                outputOverflow_ = true;
                return 0;
            }
            if (lastCycleValue - firstCycleValue > static_cast<double>(maximumEventsPerBlock))
            {
                outputOverflow_ = true;
                return 0;
            }
            const auto firstCycle = static_cast<long long>(firstCycleValue < 0.0 ? 0.0 : firstCycleValue);
            const auto lastCycle = static_cast<long long>(lastCycleValue < 0.0 ? 0.0 : lastCycleValue);

            for (long long cycle = firstCycle; cycle <= lastCycle; ++cycle)
            {
                const double cyclePpq = cycleStartPpq + static_cast<double>(cycle) * patternLengthBeats;
                if (cyclePpq >= clock.endPpq)
                    break;

                for (const auto& event : pattern)
                {
                    if (count + 2 > output.size())
                    {
                        outputOverflow_ = true;
                        break;
                    }
                    if (!std::isfinite(event.beat) || !std::isfinite(event.duration)
                        || event.beat < 0.0 || event.beat >= patternLengthBeats || event.duration <= 0.0)
                        continue;

                    double onset = cyclePpq + event.beat;
                    if (quantizeBeats > 0.0)
                        onset = cyclePpq + quantize(onset - cyclePpq, quantizeBeats);
                    if (!std::isfinite(onset))
                        continue;
                    const double noteOffPpq = onset + event.duration;
                    if (!std::isfinite(noteOffPpq))
                        continue;
                    // Only the cycle whose onset lies in this block is emitted.
                    // Long gates retain their NoteOff in the bounded queue.
                    if (onset < clock.startPpq || onset >= clock.endPpq)
                        continue;

                    const int note = clampInt(static_cast<int>(event.note) + transpose, 0, 127);
                    const int noteVelocity = clampInt(event.velocity, 1, 127);
                    if (!emit(onset, EventKind::NoteOn, note, noteVelocity, event.channel))
                        retainPending(onset, EventKind::NoteOn, note, noteVelocity, event.channel);

                    // NoteOffs are queued by PPQ, not by a clamped sample
                    // index. This also covers a gate whose endpoint lies
                    // inside the PPQ block but rounds to the next block's
                    // first sample.
                    if (noteOffPpq >= clock.endPpq
                        || (noteOffPpq - clock.startPpq) * samplesPerBeat >= blockSamples - 1.0e-9)
                        retainPending(noteOffPpq, EventKind::NoteOff, note, 0, event.channel);
                    else if (!emit(noteOffPpq, EventKind::NoteOff, note, 0, event.channel))
                        retainPending(noteOffPpq, EventKind::NoteOff, note, 0, event.channel);
                }
            }

            // Insertion sort is bounded and stable; NoteOff precedes NoteOn
            // at an identical sample so repeated notes retrigger cleanly.
            for (std::size_t i = 1; i < count; ++i)
            {
                const auto value = output[i];
                std::size_t j = i;
                while (j > 0 && (output[j - 1].sampleOffset > value.sampleOffset
                    || (output[j - 1].sampleOffset == value.sampleOffset
                        && output[j - 1].kind > value.kind)))
                {
                    output[j] = output[j - 1];
                    --j;
                }
                output[j] = value;
            }
            return count;
        }

        void clearPendingNoteOffs() noexcept
        {
            pendingCount_ = 0;
            pendingOverflow_ = false;
        }

        // Treat output truncation like pending NoteOff exhaustion: the
        // processor must release voices rather than risk a stuck note.
        bool consumePendingOverflow() noexcept
        {
            const bool overflowed = pendingOverflow_ || outputOverflow_;
            pendingOverflow_ = false;
            outputOverflow_ = false;
            return overflowed;
        }

        void reset(double ppq = 0.0) noexcept
        {
            internalPpq_ = finiteOr(ppq, 0.0);
            wasRunning_ = false;
            hostWasRunning_ = false;
            haveHostPosition_ = false;
            previousUsingHost_ = false;
            pendingCount_ = 0;
            pendingOverflow_ = false;
            outputOverflow_ = false;
        }

        double getInternalPpq() const noexcept { return internalPpq_; }
        bool wasRunning() const noexcept { return wasRunning_; }

        static double clampBpm(double bpm) noexcept
        {
            return bpm >= 20.0 && bpm <= 300.0 ? bpm : 120.0;
        }

    private:
        static double samplesToBeats(int samples, double sampleRate, double bpm) noexcept
        {
            if (samples <= 0 || !std::isfinite(sampleRate) || sampleRate <= 0.0)
                return 0.0;
            return static_cast<double>(samples) * clampBpm(bpm) / (60.0 * sampleRate);
        }

        static double quantize(double value, double grid) noexcept
        {
            if (!std::isfinite(value) || !std::isfinite(grid) || grid <= 0.0)
                return value;
            const double steps = value / grid + 0.5;
            constexpr double limit = static_cast<double>(std::numeric_limits<long long>::max());
            if (!std::isfinite(steps) || steps >= limit || steps <= -limit)
                return value;
            return static_cast<double>(static_cast<long long>(steps)) * grid;
        }

    public:
        static double quantizePpqForward(double ppq, double gridBeats) noexcept
        {
            if (!std::isfinite(ppq) || !std::isfinite(gridBeats) || gridBeats <= 0.0)
                return 0.0;
            const double steps = ppq / gridBeats;
            if (!std::isfinite(steps)) return ppq;
            return std::ceil(steps - 1.0e-9) * gridBeats;
        }

    private:

        static double finiteOr(double value, double fallback) noexcept
        {
            return value == value && value > -1.0e12 && value < 1.0e12 ? value : fallback;
        }

        static int clampInt(int value, int low, int high) noexcept
        {
            return value < low ? low : (value > high ? high : value);
        }

        struct PendingEvent
        {
            double ppq = 0.0;
            EventKind kind = EventKind::NoteOff;
            int note = 60;
            int velocity = 0;
            int channel = 1;
        };
        std::array<PendingEvent, maximumEventsPerBlock * 4> pendingEvents_ {};
        std::size_t pendingCount_ = 0;
        double internalPpq_ = 0.0;
        double lastHostEndPpq_ = 0.0;
        bool wasRunning_ = false;
        bool hostWasRunning_ = false;
        bool haveHostPosition_ = false;
        bool previousUsingHost_ = false;
        bool pendingOverflow_ = false;
        bool outputOverflow_ = false;
    };
}
