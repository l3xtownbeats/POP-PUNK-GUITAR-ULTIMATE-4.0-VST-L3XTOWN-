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
// only the first panel visible; the window manager owns page navigation.
// No JUCE/UI ownership or audio-thread state.
PanelLayout calculatePanelLayout(PanelRect workArea,
                                 const std::vector<PanelRequest>& requests,
                                 float scale,
                                 int gap = 12);

// Places one selected panel in the main workspace and every remaining panel
// in a vertically stacked dock. Returns overflow instead of clipping.
PanelLayout calculateExpandedPanelLayout(PanelRect workArea,
                                         const std::vector<PanelRequest>& requests,
                                         int expandedId,
                                         float scale = 1.0f,
                                         int gap = 8);
} // namespace ppgu::ui
