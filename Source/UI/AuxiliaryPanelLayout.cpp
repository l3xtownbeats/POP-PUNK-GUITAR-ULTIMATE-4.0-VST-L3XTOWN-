#include "AuxiliaryPanelLayout.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <set>

namespace ppgu::ui
{
namespace
{
int scaled(int value, float scale)
{
    const long double product = static_cast<long double>(value)
                              * static_cast<long double>(scale);
    if (!std::isfinite(product) || product >= static_cast<long double>(std::numeric_limits<int>::max()))
        return std::numeric_limits<int>::max();
    return std::max(1, static_cast<int>(std::llround(product)));
}

bool validArea(const PanelRect& area)
{
    if (area.width <= 0 || area.height <= 0)
        return false;

    const auto right = static_cast<std::int64_t>(area.x) + area.width;
    const auto bottom = static_cast<std::int64_t>(area.y) + area.height;
    return right <= std::numeric_limits<int>::max()
        && right >= std::numeric_limits<int>::min()
        && bottom <= std::numeric_limits<int>::max()
        && bottom >= std::numeric_limits<int>::min();
}
}

PanelLayout calculatePanelLayout(PanelRect area,
                                 const std::vector<PanelRequest>& requests,
                                 float scale,
                                 int gap)
{
    PanelLayout result;
    if (requests.empty())
        return result;

    if (!validArea(area) || requests.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
        result.invalidInput = true;
        return result;
    }

    if (!std::isfinite(scale) || scale <= 0.0f)
        scale = 1.0f;

    std::set<int> ids;
    int desiredW = 1, desiredH = 1, minimumW = 1, minimumH = 1;
    for (const auto& request : requests)
    {
        if (!ids.insert(request.id).second
            || request.desired.width <= 0 || request.desired.height <= 0
            || request.minimum.width <= 0 || request.minimum.height <= 0
            || request.minimum.width > request.desired.width
            || request.minimum.height > request.desired.height)
        {
            result.invalidInput = true;
            return result;
        }

        desiredW = std::max(desiredW, scaled(request.desired.width, scale));
        desiredH = std::max(desiredH, scaled(request.desired.height, scale));
        minimumW = std::max(minimumW, scaled(request.minimum.width, scale));
        minimumH = std::max(minimumH, scaled(request.minimum.height, scale));
    }

    const auto scaledGap = gap <= 0 ? 0 : scaled(gap, scale);
    const auto count = static_cast<int>(requests.size());
    int bestColumns = 0, bestRows = 0;
    double bestScale = -1.0;

    for (int columns = 1; columns <= count; ++columns)
    {
        const int rows = (count + columns - 1) / columns;
        const auto horizontalGaps = static_cast<std::int64_t>(scaledGap) * (columns - 1);
        const auto verticalGaps = static_cast<std::int64_t>(scaledGap) * (rows - 1);
        const auto availableW = static_cast<std::int64_t>(area.width) - horizontalGaps;
        const auto availableH = static_cast<std::int64_t>(area.height) - verticalGaps;
        if (availableW <= 0 || availableH <= 0)
            continue;

        const auto cellW = availableW / columns;
        const auto cellH = availableH / rows;
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
    const auto horizontalGaps = static_cast<std::int64_t>(scaledGap) * (bestColumns - 1);
    const auto verticalGaps = static_cast<std::int64_t>(scaledGap) * (bestRows - 1);
    const auto cellW = (static_cast<std::int64_t>(area.width) - horizontalGaps) / bestColumns;
    const auto cellH = (static_cast<std::int64_t>(area.height) - verticalGaps) / bestRows;
    result.panels.reserve(requests.size());

    for (int i = 0; i < count; ++i)
    {
        const int column = i % bestColumns;
        const int row = i / bestColumns;
        const auto x = static_cast<std::int64_t>(area.x)
                     + static_cast<std::int64_t>(column) * (cellW + scaledGap);
        const auto y = static_cast<std::int64_t>(area.y)
                     + static_cast<std::int64_t>(row) * (cellH + scaledGap);
        const auto width = column == bestColumns - 1
            ? static_cast<std::int64_t>(area.x) + area.width - x : cellW;
        const auto height = row == bestRows - 1
            ? static_cast<std::int64_t>(area.y) + area.height - y : cellH;

        result.panels.push_back({ requests[static_cast<std::size_t>(i)].id,
                                  { static_cast<int>(x), static_cast<int>(y),
                                    static_cast<int>(width), static_cast<int>(height) }, true });
    }

    return result;
}
} // namespace ppgu::ui
