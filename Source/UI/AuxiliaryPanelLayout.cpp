#include "AuxiliaryPanelLayout.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace ppgu::ui
{
PanelLayout layoutAuxiliaryPanels(const PanelBounds& workArea,
                                  const std::vector<PanelRequest>& requests)
{
    PanelLayout result;
    if (requests.empty() || workArea.width <= 0 || workArea.height <= 0)
        return result;

    const int count = static_cast<int>(requests.size());
    const double aspect = static_cast<double>(workArea.width) /
                          static_cast<double>(workArea.height);

    // Choose a stable grid close to the work-area aspect ratio.
    int columns = (count == 3 || count == 4)
        ? 2
        : std::max(1, static_cast<int>(std::ceil(std::sqrt(count * aspect))));
    columns = std::min(columns, count);
    int rows = (count + columns - 1) / columns;

    // If the final row would be excessively sparse, prefer a more balanced grid.
    while (columns > 1 && rows > 1 && (rows - 1) * columns >= count)
    {
        --columns;
        rows = (count + columns - 1) / columns;
    }

    result.columns = columns;
    result.rows = rows;

    const int baseWidth = workArea.width / columns;
    const int extraWidth = workArea.width % columns;
    const int baseHeight = workArea.height / rows;
    const int extraHeight = workArea.height % rows;

    result.panels.reserve(requests.size());
    for (std::size_t i = 0; i < requests.size(); ++i)
    {
        const int row = static_cast<int>(i) / columns;
        const int column = static_cast<int>(i) % columns;
        const int cellX = workArea.x + column * baseWidth + std::min(column, extraWidth);
        const int cellY = workArea.y + row * baseHeight + std::min(row, extraHeight);
        const int cellWidth = baseWidth + (column < extraWidth ? 1 : 0);
        const int cellHeight = baseHeight + (row < extraHeight ? 1 : 0);

        const auto& request = requests[i];
        const int preferredWidth = std::max(1, request.preferredWidth);
        const int preferredHeight = std::max(1, request.preferredHeight);
        const int minimumWidth = std::max(1, request.minimumWidth);
        const int minimumHeight = std::max(1, request.minimumHeight);

        // A single panel uses its preferred size, bounded by the available work area.
        // Multiple panels use the complete tile so no open panel is hidden underneath another.
        const bool tiled = count > 1;
        const int width = tiled ? cellWidth : std::min(preferredWidth, cellWidth);
        const int height = tiled ? cellHeight : std::min(preferredHeight, cellHeight);
        const int x = tiled ? cellX : cellX + (cellWidth - width) / 2;
        const int y = tiled ? cellY : cellY + (cellHeight - height) / 2;
        const bool belowMinimum = width < minimumWidth || height < minimumHeight;
        const bool compact = tiled || width < preferredWidth || height < preferredHeight;

        result.panels.push_back({ request.id, { x, y, width, height }, compact, belowMinimum });
        result.compact = result.compact || compact;
        result.overflow = result.overflow || belowMinimum;
    }

    return result;
}
