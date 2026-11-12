#pragma once

#include <string>
#include <vector>

namespace ppgu::ui
{
struct PanelBounds
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

struct PanelRequest
{
    std::string id;
    int preferredWidth = 900;
    int preferredHeight = 620;
    int minimumWidth = 420;
    int minimumHeight = 300;
};

struct PanelPlacement
{
    std::string id;
    PanelBounds bounds;
    bool compact = false;
    bool belowMinimum = false;
};

struct PanelLayout
{
    std::vector<PanelPlacement> panels;
    int columns = 0;
    int rows = 0;
    bool compact = false;
    bool overflow = false;
};

// Pure geometry only: no JUCE ownership, window creation, or audio-thread access.
// The returned rectangles are deterministic, non-overlapping tiles inside workArea.
PanelLayout layoutAuxiliaryPanels(const PanelBounds& workArea,
                                  const std::vector<PanelRequest>& requests);
}
