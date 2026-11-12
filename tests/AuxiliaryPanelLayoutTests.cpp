#include "AuxiliaryPanelLayout.h"

#include <iostream>
#include <vector>

using namespace ppgu::ui;

namespace
{
bool inside(const PanelBounds& a, const PanelBounds& b)
{
    return a.x >= b.x && a.y >= b.y
        && a.x + a.width <= b.x + b.width
        && a.y + a.height <= b.y + b.height;
}

bool overlaps(const PanelBounds& a, const PanelBounds& b)
{
    return a.x < b.x + b.width && a.x + a.width > b.x
        && a.y < b.y + b.height && a.y + a.height > b.y;
}
}

int main()
{
    const PanelBounds work { 100, 50, 1600, 900 };
    const std::vector<PanelRequest> requests {
        { "FX_EDIT", 1000, 680, 420, 300 },
        { "AUTOCHORDS", 900, 620, 420, 300 },
        { "SKINS", 1000, 700, 420, 300 },
        { "IMPORT", 850, 600, 420, 300 }
    };

    const auto empty = layoutAuxiliaryPanels(work, {});
    if (!empty.panels.empty() || empty.columns != 0 || empty.rows != 0)
        return 1;

    const auto single = layoutAuxiliaryPanels(work, { requests.front() });
    if (single.panels.size() != 1 || single.panels[0].bounds.width != 1000
        || single.panels[0].bounds.height != 680 || !inside(single.panels[0].bounds, work))
        return 2;

    const auto tiled = layoutAuxiliaryPanels(work, requests);
    if (tiled.panels.size() != requests.size() || !tiled.compact
        || tiled.columns != 2 || tiled.rows != 2)
        return 3;

    const auto threePanels = layoutAuxiliaryPanels(work, { requests[0], requests[1], requests[2] });
    if (threePanels.columns != 2 || threePanels.rows != 2 || threePanels.panels.size() != 3)
        return 10;

    for (std::size_t i = 0; i < tiled.panels.size(); ++i)
    {
        if (!inside(tiled.panels[i].bounds, work))
            return 4;
        for (std::size_t j = i + 1; j < tiled.panels.size(); ++j)
            if (overlaps(tiled.panels[i].bounds, tiled.panels[j].bounds))
                return 5;
    }

    // A very small work area is reported honestly rather than inventing
    // larger rectangles or allowing panels to overlap.
    const auto compact = layoutAuxiliaryPanels({ 0, 0, 500, 260 }, requests);
    if (!compact.overflow || compact.panels.size() != requests.size())
        return 6;
    for (std::size_t i = 0; i < compact.panels.size(); ++i)
    {
        if (!inside(compact.panels[i].bounds, { 0, 0, 500, 260 }))
            return 7;
        for (std::size_t j = i + 1; j < compact.panels.size(); ++j)
            if (overlaps(compact.panels[i].bounds, compact.panels[j].bounds))
                return 8;
    }

    const auto invalidArea = layoutAuxiliaryPanels({ 0, 0, 0, 100 }, requests);
    if (!invalidArea.panels.empty())
        return 9;

    std::cout << "Auxiliary panel layout tests passed.\n";
    return 0;
}
