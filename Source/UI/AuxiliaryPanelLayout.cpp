#include "AuxiliaryPanelLayout.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
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
    std::vector<int> desiredWidths, minimumWidths, desiredHeights, minimumHeights;
    desiredWidths.reserve(requests.size());
    minimumWidths.reserve(requests.size());
    desiredHeights.reserve(requests.size());
    minimumHeights.reserve(requests.size());

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

        desiredWidths.push_back(scaled(request.desired.width, scale));
        minimumWidths.push_back(scaled(request.minimum.width, scale));
        desiredHeights.push_back(scaled(request.desired.height, scale));
        minimumHeights.push_back(scaled(request.minimum.height, scale));
    }

    const int scaledGap = gap <= 0 ? 0 : scaled(gap, scale);
    const int count = static_cast<int>(requests.size());
    const int maximumColumns = std::min(count, 2);

    struct Candidate
    {
        int columns = 0;
        int rows = 0;
        double factor = -1.0;
        std::vector<int> widths;
        std::vector<int> heights;
        std::int64_t minimumOverflow = std::numeric_limits<std::int64_t>::max();
        bool feasible = false;
    };

    Candidate bestFit;
    Candidate bestOverflow;

    const auto roundedAtScale = [](int desired, int minimum, double factor)
    {
        const auto value = static_cast<std::int64_t>(std::llround(
            static_cast<long double>(desired) * static_cast<long double>(factor)));
        return static_cast<int>(std::max<std::int64_t>(minimum,
            std::min<std::int64_t>(std::numeric_limits<int>::max(), value)));
    };

    for (int columns = 1; columns <= maximumColumns; ++columns)
    {
        const int rows = count / columns + (count % columns == 0 ? 0 : 1);
        std::vector<int> colDesired(static_cast<std::size_t>(columns), 1);
        std::vector<int> colMinimum(static_cast<std::size_t>(columns), 1);
        std::vector<int> rowDesired(static_cast<std::size_t>(rows), 1);
        std::vector<int> rowMinimum(static_cast<std::size_t>(rows), 1);

        for (int i = 0; i < count; ++i)
        {
            const int column = i % columns;
            const int row = i / columns;
            const auto index = static_cast<std::size_t>(i);
            colDesired[static_cast<std::size_t>(column)] =
                std::max(colDesired[static_cast<std::size_t>(column)], desiredWidths[index]);
            colMinimum[static_cast<std::size_t>(column)] =
                std::max(colMinimum[static_cast<std::size_t>(column)], minimumWidths[index]);
            rowDesired[static_cast<std::size_t>(row)] =
                std::max(rowDesired[static_cast<std::size_t>(row)], desiredHeights[index]);
            rowMinimum[static_cast<std::size_t>(row)] =
                std::max(rowMinimum[static_cast<std::size_t>(row)], minimumHeights[index]);
        }

        const auto gapWidth = static_cast<std::int64_t>(scaledGap) * (columns - 1);
        const auto gapHeight = static_cast<std::int64_t>(scaledGap) * (rows - 1);
        const auto sum = [](const std::vector<int>& values)
        {
            std::int64_t total = 0;
            for (const auto value : values)
                total += value;
            return total;
        };

        const auto minWidth = sum(colMinimum) + gapWidth;
        const auto minHeight = sum(rowMinimum) + gapHeight;
        if (minWidth > std::numeric_limits<int>::max()
            || minHeight > std::numeric_limits<int>::max())
        {
            bestOverflow.minimumOverflow = -1;
            bestOverflow.columns = columns;
            bestOverflow.rows = rows;
            bestOverflow.widths = colMinimum;
            bestOverflow.heights = rowMinimum;
            continue;
        }

        Candidate overflowCandidate;
        overflowCandidate.columns = columns;
        overflowCandidate.rows = rows;
        overflowCandidate.widths = colMinimum;
        overflowCandidate.heights = rowMinimum;
        overflowCandidate.minimumOverflow =
            std::max<std::int64_t>(0, minWidth - area.width)
            + std::max<std::int64_t>(0, minHeight - area.height);
        if (overflowCandidate.minimumOverflow < bestOverflow.minimumOverflow
            || (overflowCandidate.minimumOverflow == bestOverflow.minimumOverflow
                && columns > bestOverflow.columns))
            bestOverflow = std::move(overflowCandidate);

        if (minWidth > area.width || minHeight > area.height)
            continue;

        const auto fits = [&](double factor)
        {
            std::int64_t width = gapWidth;
            std::int64_t height = gapHeight;
            for (std::size_t i = 0; i < colDesired.size(); ++i)
                width += roundedAtScale(colDesired[i], colMinimum[i], factor);
            for (std::size_t i = 0; i < rowDesired.size(); ++i)
                height += roundedAtScale(rowDesired[i], rowMinimum[i], factor);
            return width <= area.width && height <= area.height;
        };

        double low = 0.0;
        double high = 1.0;
        for (int iteration = 0; iteration < 48; ++iteration)
        {
            const double middle = (low + high) * 0.5;
            if (fits(middle))
                low = middle;
            else
                high = middle;
        }

        Candidate candidate;
        candidate.columns = columns;
        candidate.rows = rows;
        candidate.factor = low;
        candidate.feasible = true;
        for (std::size_t i = 0; i < colDesired.size(); ++i)
            candidate.widths.push_back(roundedAtScale(colDesired[i], colMinimum[i], low));
        for (std::size_t i = 0; i < rowDesired.size(); ++i)
            candidate.heights.push_back(roundedAtScale(rowDesired[i], rowMinimum[i], low));

        const auto currentBetter = candidate.factor > bestFit.factor + 1.0e-9
            || (std::abs(candidate.factor - bestFit.factor) <= 1.0e-9
                && candidate.columns > bestFit.columns);
        if (currentBetter)
            bestFit = std::move(candidate);
    }

    if (!bestFit.feasible)
    {
        result.overflow = true;
        if (bestOverflow.columns == 0 || bestOverflow.minimumOverflow == -1)
        {
            result.invalidInput = true;
            return result;
        }

        result.columns = bestOverflow.columns;
        result.rows = bestOverflow.rows;
        const auto totalWidth = std::accumulate(bestOverflow.widths.begin(),
            bestOverflow.widths.end(), static_cast<std::int64_t>(scaledGap)
                * (bestOverflow.columns - 1));
        const auto totalHeight = std::accumulate(bestOverflow.heights.begin(),
            bestOverflow.heights.end(), static_cast<std::int64_t>(scaledGap)
                * (bestOverflow.rows - 1));
        if (totalWidth > std::numeric_limits<int>::max()
            || totalHeight > std::numeric_limits<int>::max()
            || static_cast<std::int64_t>(area.x) + totalWidth > std::numeric_limits<int>::max()
            || static_cast<std::int64_t>(area.y) + totalHeight > std::numeric_limits<int>::max())
        {
            result.invalidInput = true;
            result.panels.clear();
            return result;
        }

        std::vector<int> xPositions(static_cast<std::size_t>(result.columns));
        std::vector<int> yPositions(static_cast<std::size_t>(result.rows));
        std::int64_t position = area.x;
        for (int column = 0; column < result.columns; ++column)
        {
            xPositions[static_cast<std::size_t>(column)] = static_cast<int>(position);
            position += bestOverflow.widths[static_cast<std::size_t>(column)] + scaledGap;
        }
        position = area.y;
        for (int row = 0; row < result.rows; ++row)
        {
            yPositions[static_cast<std::size_t>(row)] = static_cast<int>(position);
            position += bestOverflow.heights[static_cast<std::size_t>(row)] + scaledGap;
        }

        result.panels.reserve(requests.size());
        for (int i = 0; i < count; ++i)
        {
            const int column = i % result.columns;
            const int row = i / result.columns;
            result.panels.push_back({ requests[static_cast<std::size_t>(i)].id,
                { xPositions[static_cast<std::size_t>(column)],
                  yPositions[static_cast<std::size_t>(row)],
                  bestOverflow.widths[static_cast<std::size_t>(column)],
                  bestOverflow.heights[static_cast<std::size_t>(row)] }, true });
        }
        return result;
    }

    result.columns = bestFit.columns;
    result.rows = bestFit.rows;
    result.compact = bestFit.factor < 0.999999;

    const auto totalWidth = std::accumulate(bestFit.widths.begin(), bestFit.widths.end(),
        static_cast<std::int64_t>(scaledGap) * (result.columns - 1));
    const auto totalHeight = std::accumulate(bestFit.heights.begin(), bestFit.heights.end(),
        static_cast<std::int64_t>(scaledGap) * (result.rows - 1));
    const auto extraWidth = static_cast<std::int64_t>(area.width) - totalWidth;
    const auto extraHeight = static_cast<std::int64_t>(area.height) - totalHeight;
    for (int column = 0; column < result.columns; ++column)
        bestFit.widths[static_cast<std::size_t>(column)] += static_cast<int>(
            extraWidth / result.columns + (column == result.columns - 1 ? extraWidth % result.columns : 0));
    for (int row = 0; row < result.rows; ++row)
        bestFit.heights[static_cast<std::size_t>(row)] += static_cast<int>(
            extraHeight / result.rows + (row == result.rows - 1 ? extraHeight % result.rows : 0));

    std::vector<int> xPositions(static_cast<std::size_t>(result.columns));
    std::vector<int> yPositions(static_cast<std::size_t>(result.rows));
    std::int64_t position = area.x;
    for (int column = 0; column < result.columns; ++column)
    {
        xPositions[static_cast<std::size_t>(column)] = static_cast<int>(position);
        position += bestFit.widths[static_cast<std::size_t>(column)] + scaledGap;
    }
    position = area.y;
    for (int row = 0; row < result.rows; ++row)
    {
        yPositions[static_cast<std::size_t>(row)] = static_cast<int>(position);
        position += bestFit.heights[static_cast<std::size_t>(row)] + scaledGap;
    }

    result.panels.reserve(requests.size());
    for (int i = 0; i < count; ++i)
    {
        const int column = i % result.columns;
        const int row = i / result.columns;
        result.panels.push_back({ requests[static_cast<std::size_t>(i)].id,
            { xPositions[static_cast<std::size_t>(column)],
              yPositions[static_cast<std::size_t>(row)],
              bestFit.widths[static_cast<std::size_t>(column)],
              bestFit.heights[static_cast<std::size_t>(row)] }, true });
    }

    return result;
}
} // namespace ppgu::ui
