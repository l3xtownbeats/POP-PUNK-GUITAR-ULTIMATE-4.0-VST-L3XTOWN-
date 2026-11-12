#pragma once

#include "../Sampler/BoundedSpscQueue.h"
#include "PatternScheduler.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace ppgu::performance
{
enum class PreviewCommandKind : std::uint8_t { PlayChord, Stop };

struct PreviewCommand
{
    PreviewCommandKind kind = PreviewCommandKind::Stop;
    std::array<std::uint8_t, 6> notes {};
    std::uint8_t noteCount = 0;
    std::uint8_t velocity = 100;
    std::uint8_t channel = 1;
    std::uint32_t gateSamples = 1;
};

struct PreviewEvent
{
    int sampleOffset = 0;
    EventKind kind = EventKind::NoteOn;
    int note = 60;
    int velocity = 100;
    int channel = 1;
};

// Single-producer (message/UI thread), single-consumer (audio thread) preview
// command bridge. It is a transport contract only: the host processor remains
// responsible for routing emitted events to its one shared sampler.
class MidiPreviewScheduler final
{
public:
    static constexpr std::size_t commandQueueCapacity = 16;
    static constexpr std::size_t maximumEventsPerBlock = 24;
    static constexpr std::size_t maximumChordNotes = 6;

    [[nodiscard]] bool enqueueChord(const int* notes, std::size_t count,
                                    int velocity, std::uint32_t gateSamples,
                                    int channel = 1) noexcept
    {
        if (notes == nullptr || count == 0 || count > maximumChordNotes
            || gateSamples == 0 || channel < 1 || channel > 16
            || velocity < 1 || velocity > 127)
            return false;

        PreviewCommand command;
        command.kind = PreviewCommandKind::PlayChord;
        command.noteCount = static_cast<std::uint8_t>(count);
        command.velocity = static_cast<std::uint8_t>(velocity);
        command.channel = static_cast<std::uint8_t>(channel);
        command.gateSamples = gateSamples;
        for (std::size_t i = 0; i < count; ++i)
        {
            if (notes[i] < 0 || notes[i] > 127)
                return false;
            command.notes[i] = static_cast<std::uint8_t>(notes[i]);
        }
        return commands.tryPush(command);
    }

    [[nodiscard]] bool enqueueStop() noexcept
    {
        PreviewCommand command;
        command.kind = PreviewCommandKind::Stop;
        return commands.tryPush(command);
    }

    // Audio-thread only. Fixed arrays, no locks, allocation, waits or callbacks.
    std::size_t processBlock(int blockSamples,
        std::array<PreviewEvent, maximumEventsPerBlock>& output) noexcept
    {
        if (blockSamples <= 0)
            return 0;

        std::size_t count = 0;
        const auto emitOff = [&](const ActiveNote& note, int offset)
        {
            if (count < output.size())
                output[count++] = { offset, EventKind::NoteOff, note.note, 0, note.channel };
        };

        // Advance existing gates before applying commands received for this block.
        std::size_t retained = 0;
        for (std::size_t i = 0; i < activeCount; ++i)
        {
            auto note = active[i];
            if (note.remainingSamples < static_cast<std::uint32_t>(blockSamples))
                emitOff(note, static_cast<int>(note.remainingSamples));
            else
            {
                note.remainingSamples -= static_cast<std::uint32_t>(blockSamples);
                active[retained++] = note;
            }
        }
        activeCount = retained;

        // Coalesce a burst of UI commands to the latest intent. This bounds
        // each block to at most 6 old NoteOffs + 6 NoteOns + 6 NoteOffs.
        PreviewCommand command, latestCommand;
        bool haveCommand = false;
        while (commands.tryPop(command))
        {
            latestCommand = command;
            haveCommand = true;
        }

        if (haveCommand)
        {
            for (std::size_t i = 0; i < activeCount; ++i)
                emitOff(active[i], 0);
            activeCount = 0;

            if (latestCommand.kind == PreviewCommandKind::PlayChord)
            {
                for (std::size_t i = 0; i < latestCommand.noteCount && i < maximumChordNotes; ++i)
                {
                    const auto note = latestCommand.notes[i];
                    output[count++] = { 0, EventKind::NoteOn, note,
                                        latestCommand.velocity, latestCommand.channel };
                    if (latestCommand.gateSamples < static_cast<std::uint32_t>(blockSamples))
                    {
                        output[count++] = { static_cast<int>(latestCommand.gateSamples),
                                            EventKind::NoteOff, note, 0, latestCommand.channel };
                    }
                    else
                    {
                        active[activeCount++] = { note, latestCommand.channel,
                            latestCommand.gateSamples - static_cast<std::uint32_t>(blockSamples) };
                    }
                }
            }
        }

        // Stable bounded insertion sort; NoteOff precedes NoteOn at equal offsets.
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

    // Audio-thread or stopped-host lifecycle only; drains queued commands.
    void reset() noexcept
    {
        PreviewCommand discarded;
        while (commands.tryPop(discarded)) {}
        activeCount = 0;
    }

    [[nodiscard]] std::size_t activeNoteCount() const noexcept { return activeCount; }
    [[nodiscard]] bool commandQueueEmpty() const noexcept { return commands.empty(); }

private:
    struct ActiveNote
    {
        std::uint8_t note = 60;
        std::uint8_t channel = 1;
        std::uint32_t remainingSamples = 0;
    };

    ppgu::sampler::BoundedSpscQueue<PreviewCommand, commandQueueCapacity> commands;
    std::array<ActiveNote, maximumChordNotes> active {};
    std::size_t activeCount = 0;
};
} // namespace ppgu::performance
