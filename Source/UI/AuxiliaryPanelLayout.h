#pragma once

#include <cstddef>
#include <vector>

namespace ppgu::ui
{
struct PanelSize { int width = 0; int height = 0; };
struct PanelRect { int x = 0; int y = 0; int width = 0; int height = 0; };
struct PanelRequest
{
    int id = 0;
    PanelSize desired;
    PanelSize minimum;
};
struct PanelPlacement
{
    int id = 0;
    PanelRect bounds;
    bool visible = true;
};
struct PanelLayout
{
    std::vector<PanelPlacement> panels;
    bool compact = false;
    bool overflow = false;
    bool invalidInput = false;
    int columns = 0;
    int rows = 0;
};

// Pure geometry only: coordinates are physical pixels; scale converts logical
// panel metrics to physical pixels. Invalid geometry or duplicate IDs return
// an empty layout with invalidInput=true. Overflow preserves all IDs and marks
// every panel visible on a minimum-size scrollable grid when the work area is too small.
// No JUCE/UI ownership or audio-thread state.
PanelLayout calculatePanelLayout(PanelRect workArea,
                                 const std::vector<PanelRequest>& requests,
                                 float scale,
                                 int gap = 12);
} // namespace ppgu::ui
