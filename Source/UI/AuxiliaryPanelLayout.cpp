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

    const int count = static_cast<int>(requests.size());
    int columns = 1;
    int rows = 1;
    if (count == 2)
    {
        columns = 2; rows = 1;
    }
    else if (count >= 3 && count <= 4)
    {
        columns = 2; rows = 2;
    }
    else if (count >= 5 && count <= 6)
    {
        columns = 2; rows = 3;
    }
    else if (count > 6)
    {
        result.overflow = true;
        result.columns = 2;
        result.rows = (count + 1) / 2;
        result.panels.reserve(requests.size());
        for (std::size_t i = 0; i < requests.size(); ++i)
            result.panels.push_back({ requests[i].id, area, i == 0 });
        return result;
    }

    const auto scaledGap = gap <= 0 ? 0 : scaled(gap, scale);
    const auto horizontalGaps = static_cast<std::int64_t>(scaledGap) * (columns - 1);
    const auto verticalGaps = static_cast<std::int64_t>(scaledGap) * (rows - 1);
    const auto availableW = static_cast<std::int64_t>(area.width) - horizontalGaps;
    const auto availableH = static_cast<std::int64_t>(area.height) - verticalGaps;
    const auto cellW = availableW > 0 ? availableW / columns : 0;
    const auto cellH = availableH > 0 ? availableH / rows : 0;

    if (cellW < minimumW || cellH < minimumH)
    {
        result.overflow = true;
        result.columns = columns;
        result.rows = rows;
        result.panels.reserve(requests.size());
        for (std::size_t i = 0; i < requests.size(); ++i)
            result.panels.push_back({ requests[i].id, area, i == 0 });
        return result;
    }

    result.columns = columns;
    result.rows = rows;
    result.compact = cellW < desiredW || cellH < desiredH;
    result.panels.reserve(requests.size());

    for (int i = 0; i < count; ++i)
    {
        const int column = i % columns;
        const int row = i / columns;
        const auto x = static_cast<std::int64_t>(area.x)
                     + static_cast<std::int64_t>(column) * (cellW + scaledGap);
        const auto y = static_cast<std::int64_t>(area.y)
                     + static_cast<std::int64_t>(row) * (cellH + scaledGap);
        const auto width = column == columns - 1
            ? static_cast<std::int64_t>(area.x) + area.width - x : cellW;
        const auto height = row == rows - 1
            ? static_cast<std::int64_t>(area.y) + area.height - y : cellH;

        result.panels.push_back({ requests[static_cast<std::size_t>(i)].id,
                                  { static_cast<int>(x), static_cast<int>(y),
                                    static_cast<int>(width), static_cast<int>(height) }, true });
    }

    return result;
}
PanelLayout calculateExpandedPanelLayout(PanelRect area,
                                         const std::vector<PanelRequest>& requests,
                                         int expandedId,
                                         float scale,
                                         int gap)
{
    PanelLayout result;
    if (requests.empty())
        return result;

    if (!validArea(area)
        || requests.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
        result.invalidInput = true;
        return result;
    }

    if (!std::isfinite(scale) || scale <= 0.0f)
        scale = 1.0f;

    std::set<int> ids;
    const PanelRequest* expanded = nullptr;
    int dockMinimumWidth = 1;
    int dockMinimumHeight = 1;
    int expandedMinimumWidth = 1;
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

        if (request.id == expandedId)
        {
            expanded = &request;
            expandedMinimumWidth = scaled(request.minimum.width, scale);
        }
        else
        {
            dockMinimumWidth = std::max(dockMinimumWidth, scaled(request.minimum.width, scale));
            dockMinimumHeight = std::max(dockMinimumHeight, scaled(request.minimum.height, scale));
        }
    }

    if (expanded == nullptr)
    {
        result.invalidInput = true;
        return result;
    }

    const int count = static_cast<int>(requests.size());
    const int dockedCount = count - 1;
    const int scaledGap = gap <= 0 ? 0 : scaled(gap, scale);
    const auto totalVerticalGap = static_cast<std::int64_t>(scaledGap) * std::max(0, dockedCount - 1);
    const auto dockCellHeight = dockedCount > 0
        ? (static_cast<std::int64_t>(area.height) - totalVerticalGap) / dockedCount
        : area.height;

    if (dockedCount > 0)
    {
        const int preferredDockWidth = std::max(dockMinimumWidth, scaled(240, scale));
        const int maximumDockWidth = area.width - scaledGap - expandedMinimumWidth;
        if (maximumDockWidth < preferredDockWidth || dockCellHeight < dockMinimumHeight)
        {
            result.overflow = true;
            result.panels.reserve(requests.size());
            for (std::size_t i = 0; i < requests.size(); ++i)
                result.panels.push_back({ requests[i].id, area, requests[i].id == expandedId });
            return result;
        }

        const int dockWidth = std::min(maximumDockWidth,
            std::max(preferredDockWidth, scaled(area.width / 3, scale)));
        const int mainWidth = area.width - dockWidth - scaledGap;
        result.columns = 2;
        result.rows = dockedCount;
        result.compact = true;
        result.panels.reserve(requests.size());

        int dockIndex = 0;
        for (const auto& request : requests)
        {
            if (request.id == expandedId)
            {
                result.panels.push_back({ request.id,
                    { area.x, area.y, mainWidth, area.height }, true });
                continue;
            }

            const auto y = static_cast<std::int64_t>(area.y)
                         + static_cast<std::int64_t>(dockIndex) * (static_cast<std::int64_t>(dockCellHeight) + scaledGap);
            const auto height = dockIndex == dockedCount - 1
                ? static_cast<std::int64_t>(area.y) + area.height - y
                : dockCellHeight;
            result.panels.push_back({ request.id,
                { area.x + mainWidth + scaledGap, static_cast<int>(y),
                  dockWidth, static_cast<int>(height) }, true });
            ++dockIndex;
        }
        return result;
    }

    result.columns = 1;
    result.rows = 1;
    result.panels.push_back({ expandedId, area, true });
    return result;
}

} // namespace ppgu::ui
