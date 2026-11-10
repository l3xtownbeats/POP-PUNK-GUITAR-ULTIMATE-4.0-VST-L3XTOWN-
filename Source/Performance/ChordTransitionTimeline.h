#pragma once

#include <algorithm>

namespace ppgu::performance
{
    // Walk a block in sample-accurate MIDI intervals. Render the interval
    // before applying each event so intermediate chord states cannot collapse.
    // The helper is bounded, allocation-free, and safe for the audio callback.
    template <typename MidiRange, typename RenderInterval, typename ApplyEvent>
    void processSampleAccurateMidiTimeline(int blockSamples,
                                           const MidiRange& midi,
                                           RenderInterval&& renderInterval,
                                           ApplyEvent&& applyEvent)
    {
        const int boundedBlockSamples = (std::max)(0, blockSamples);
        int intervalStart = 0;
        for (const auto& metadata : midi)
        {
            const int eventSample = (std::clamp)(static_cast<int>(metadata.samplePosition),
                                                 intervalStart, boundedBlockSamples);
            renderInterval(intervalStart, eventSample);
            applyEvent(metadata, eventSample);
            intervalStart = eventSample;
        }
        renderInterval(intervalStart, boundedBlockSamples);
    }
}
