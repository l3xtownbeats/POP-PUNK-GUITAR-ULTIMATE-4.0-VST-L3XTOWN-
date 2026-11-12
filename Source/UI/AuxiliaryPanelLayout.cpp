#include "AuxiliaryPanelLayout.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace ppgu::ui
{
namespace
{
int scaled(int value, float scale)
{
    return std::max(1, static_cast<int>(std::lround(static_cast<double>(value) * scale)));
}
}

PanelLayout calculatePanelLayout(PanelRect area,
                                 const std::vector<PanelRequest>& requests,
                                 float scale,
                                 int gap)
{
    PanelLayout result;
    if (requests.empty() || area.width <= 0 || area.height <= 0)
        return result;

    scale = std::isfinite(scale) && scale > 0.0f ? scale : 1.0f;
    gap = std::max(0, scaled(gap, scale));
    int desiredW = 1, desiredH = 1, minimumW = 1, minimumH = 1;
    for (const auto& request : requests)
    {
        desiredW = std::max(desiredW, scaled(request.desired.width, scale));
        desiredH = std::max(desiredH, scaled(request.desired.height, scale));
        minimumW = std::max(minimumW, scaled(request.minimum.width, scale));
        minimumH = std::max(minimumH, scaled(request.minimum.height, scale));
    }

    const int count = static_cast<int>(requests.size());
    int bestColumns = 0, bestRows = 0;
    double bestScale = -1.0;
    for (int columns = 1; columns <= count; ++columns)
    {
        const int rows = (count + columns - 1) / columns;
        const int cellW = (area.width - gap * (columns - 1)) / columns;
        const int cellH = (area.height - gap * (rows - 1)) / rows;
        if (cellW < minimumW || cellH < minimumH)
            continue;
        const double fit = std::min(static_cast<double>(cellW) / desiredW,
                                    static_cast<double>(cellH) / desiredH);
        if (fit > bestScale)
        {
            bestScale = fit;
            bestColumns = columns;
            bestRows = rows;
        }
    }

    if (bestColumns == 0)
    {
        // The work area cannot hold every panel at its declared minimum size.
        // Expose one panel page at a time; the host manager supplies navigation.
        result.overflow = true;
        result.columns = result.rows = 1;
        result.panels.reserve(requests.size());
        for (std::size_t i = 0; i < requests.size(); ++i)
            result.panels.push_back({ requests[i].id, area, i == 0 });
        return result;
    }

    result.columns = bestColumns;
    result.rows = bestRows;
    result.compact = bestScale < 1.0;
    const int cellW = (area.width - gap * (bestColumns - 1)) / bestColumns;
    const int cellH = (area.height - gap * (bestRows - 1)) / bestRows;
    result.panels.reserve(requests.size());

    for (int i = 0; i < count; ++i)
    {
        const int column = i % bestColumns;
        const int row = i / bestColumns;
        const int x = area.x + column * (cellW + gap);
        const int y = area.y + row * (cellH + gap);
        const int width = column == bestColumns - 1
            ? area.x + area.width - x : cellW;
        const int height = row == bestRows - 1
            ? area.y + area.height - y : cellH;
        result.panels.push_back({ requests[static_cast<std::size_t>(i)].id,
                                  { x, y, width, height }, true });
    }
    return result;
}
} // namespace ppgu::ui
