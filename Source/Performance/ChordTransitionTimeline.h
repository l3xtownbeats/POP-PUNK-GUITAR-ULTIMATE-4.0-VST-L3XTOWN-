#pragma once

#include <algorithm>

namespace ppgu::performance
{
    // Conflict-check: all MIDI events at one sample position form one atomic
    // chord-state transition. This prevents note-off/note-on chord replacement
    // from exposing a transient empty chord and restarting the phrase clock.
    // MIDI is already sample-sorted by juce::MidiBuffer; storage remains bounded.
    template <typename MidiRange, typename RenderInterval, typename ApplyEvent,
              typename AfterEventGroup>
    void processSampleAccurateMidiTimeline(int blockSamples,
                                           const MidiRange& midi,
                                           RenderInterval&& renderInterval,
                                           ApplyEvent&& applyEvent,
                                           AfterEventGroup&& afterEventGroup)
    {
        const int boundedBlockSamples = (std::max)(0, blockSamples);
        int intervalStart = 0;
        auto iterator = midi.begin();
        const auto end = midi.end();

        while (iterator != end)
        {
            const int eventSample = (std::clamp)(static_cast<int>((*iterator).samplePosition),
                                                 intervalStart, boundedBlockSamples);
            renderInterval(intervalStart, eventSample);

            // Apply every event at this sample before publishing one resulting
            // chord snapshot. Same-sample note-off/note-on replacement is atomic.
            do
            {
                applyEvent(*iterator, eventSample);
                ++iterator;
            }
            while (iterator != end
                   && (std::clamp)(static_cast<int>(iterator->samplePosition),
                                   eventSample, boundedBlockSamples) == eventSample);

            afterEventGroup(eventSample);
            intervalStart = eventSample;
        }

        renderInterval(intervalStart, boundedBlockSamples);
    }

    // Compatibility overload for timeline-only callers that do not maintain
    // derived state after an event group.
    template <typename MidiRange, typename RenderInterval, typename ApplyEvent>
    void processSampleAccurateMidiTimeline(int blockSamples,
                                           const MidiRange& midi,
                                           RenderInterval&& renderInterval,
                                           ApplyEvent&& applyEvent)
    {
        processSampleAccurateMidiTimeline(
            blockSamples, midi,
            renderInterval, applyEvent,
            [](int) noexcept {});
    }
}
