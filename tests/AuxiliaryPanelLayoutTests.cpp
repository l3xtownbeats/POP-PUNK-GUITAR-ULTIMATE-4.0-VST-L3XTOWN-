#include "AuxiliaryPanelLayout.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <set>
#include <vector>

using namespace ppgu::ui;

namespace
{
int failures = 0;

void check(bool condition, const char* expression, int line)
{
    if (!condition)
    {
        ++failures;
        std::cerr << "FAIL line " << line << ": " << expression << '\n';
    }
}

#define CHECK(expression) check((expression), #expression, __LINE__)

std::vector<PanelRequest> makePanels(int count)
{
    std::vector<PanelRequest> panels;
    for (int i = 0; i < count; ++i)
        panels.push_back({ i + 1, { 520, 360 }, { 280, 190 } });
    return panels;
}

bool overlaps(const PanelRect& a, const PanelRect& b)
{
    const auto ar = static_cast<long long>(a.x) + a.width;
    const auto ab = static_cast<long long>(a.y) + a.height;
    const auto br = static_cast<long long>(b.x) + b.width;
    const auto bb = static_cast<long long>(b.y) + b.height;
    return a.x < br && b.x < ar && a.y < bb && b.y < ab;
}

void checkInsideAndDisjoint(const PanelLayout& layout, const PanelRect& area)
{
    for (const auto& p : layout.panels)
    {
        CHECK(p.visible);
        CHECK(p.bounds.x >= area.x && p.bounds.y >= area.y);
        CHECK(static_cast<long long>(p.bounds.x) + p.bounds.width
              <= static_cast<long long>(area.x) + area.width);
        CHECK(static_cast<long long>(p.bounds.y) + p.bounds.height
              <= static_cast<long long>(area.y) + area.height);
    }
    for (std::size_t i = 0; i < layout.panels.size(); ++i)
        for (std::size_t j = i + 1; j < layout.panels.size(); ++j)
            CHECK(!overlaps(layout.panels[i].bounds, layout.panels[j].bounds));
}
}

int main()
{
    const PanelRect desktop { 100, 80, 1600, 900 };
    CHECK(calculatePanelLayout(desktop, {}, 1.0f).panels.empty());

    auto one = calculatePanelLayout(desktop, makePanels(1), 1.0f);
    CHECK(one.panels.size() == 1 && one.panels[0].visible);
    CHECK(!one.compact && !one.overflow && !one.invalidInput);
    checkInsideAndDisjoint(one, desktop);

    CHECK(one.columns == 1 && one.rows == 1);

    for (int count : { 2, 3, 4, 5, 6 })
    {
        auto layout = calculatePanelLayout(desktop, makePanels(count), 1.0f);
        CHECK(layout.panels.size() == static_cast<std::size_t>(count));
        CHECK(!layout.overflow && !layout.invalidInput);
        CHECK(layout.columns == 2);
        CHECK(layout.rows == (count + 1) / 2);
        for (const auto& p : layout.panels)
        {
            CHECK(p.bounds.width >= 280 && p.bounds.height >= 190);
        }
        checkInsideAndDisjoint(layout, desktop);
    }

    const PanelRect compactArea { 0, 0, 900, 520 };
    auto compact = calculatePanelLayout(compactArea, makePanels(4), 1.0f);
    CHECK(compact.compact && !compact.overflow);
    checkInsideAndDisjoint(compact, compactArea);

    const PanelRect portraitArea { 0, 0, 520, 1100 };
    auto portrait = calculatePanelLayout(portraitArea, makePanels(3), 1.0f);
    CHECK(!portrait.overflow);
    for (const auto& p : portrait.panels)
        CHECK(p.bounds.width >= 280 && p.bounds.height >= 190);
    checkInsideAndDisjoint(portrait, portraitArea);

    const PanelRect narrowArea { 0, 0, 420, 260 };
    auto overflow = calculatePanelLayout(narrowArea, makePanels(4), 1.0f);
    CHECK(overflow.overflow && !overflow.invalidInput && overflow.panels.size() == 4);
    CHECK(overflow.columns == 1 && overflow.rows == 4);
    for (const auto& p : overflow.panels)
    {
        CHECK(p.visible);
        CHECK(p.bounds.width == 280 && p.bounds.height == 190);
    }
    for (std::size_t i = 0; i < overflow.panels.size(); ++i)
        for (std::size_t j = i + 1; j < overflow.panels.size(); ++j)
            CHECK(!overlaps(overflow.panels[i].bounds, overflow.panels[j].bounds));

    const PanelRect shortWideArea { 0, 0, 700, 260 };
    auto twoColumnOverflow = calculatePanelLayout(shortWideArea, makePanels(4), 1.0f);
    CHECK(twoColumnOverflow.overflow && twoColumnOverflow.columns == 2
          && twoColumnOverflow.rows == 2);
    for (const auto& p : twoColumnOverflow.panels)
        CHECK(p.visible);
    for (std::size_t i = 0; i < twoColumnOverflow.panels.size(); ++i)
        for (std::size_t j = i + 1; j < twoColumnOverflow.panels.size(); ++j)
            CHECK(!overlaps(twoColumnOverflow.panels[i].bounds, twoColumnOverflow.panels[j].bounds));

    // Mixed-size panels must use per-row/per-column minimums instead of
    // imposing the largest minimum on every window. This keeps all six
    // approved auxiliary panels visible at the default editor workspace size.
    std::vector<PanelRequest> mixedPanels {
        { 1, { 520, 360 }, { 280, 190 } },
        { 2, { 520, 360 }, { 280, 190 } },
        { 3, { 520, 360 }, { 280, 190 } },
        { 4, { 940, 700 }, { 500, 320 } },
        { 5, { 700, 520 }, { 420, 260 } },
        { 6, { 520, 360 }, { 280, 190 } }
    };
    auto mixed = calculatePanelLayout({ 0, 0, 1092, 812 }, mixedPanels, 1.0f, 12);
    CHECK(!mixed.invalidInput && !mixed.overflow);
    CHECK(mixed.panels.size() == 6 && mixed.columns == 2 && mixed.rows == 3);
    for (std::size_t i = 0; i < mixed.panels.size(); ++i)
    {
        CHECK(mixed.panels[i].visible);
        const auto& request = mixedPanels[i];
        CHECK(mixed.panels[i].bounds.width >= request.minimum.width);
        CHECK(mixed.panels[i].bounds.height >= request.minimum.height);
    }
    checkInsideAndDisjoint(mixed, { 0, 0, 1092, 812 });

    const PanelRect dpiArea { 0, 0, 1800, 1000 };
    auto dpi = calculatePanelLayout(dpiArea, makePanels(4), 1.5f);
    CHECK(!dpi.overflow && !dpi.invalidInput);
    for (const auto& p : dpi.panels)
        CHECK(p.bounds.width >= 420 && p.bounds.height >= 285);
    checkInsideAndDisjoint(dpi, dpiArea);

    auto repeated = calculatePanelLayout(desktop, makePanels(4), 1.0f);
    auto repeatedAgain = calculatePanelLayout(desktop, makePanels(4), 1.0f);
    CHECK(repeated.columns == repeatedAgain.columns && repeated.rows == repeatedAgain.rows);
    for (std::size_t i = 0; i < repeated.panels.size(); ++i)
    {
        CHECK(repeated.panels[i].id == repeatedAgain.panels[i].id);
        CHECK(repeated.panels[i].bounds.x == repeatedAgain.panels[i].bounds.x);
        CHECK(repeated.panels[i].bounds.y == repeatedAgain.panels[i].bounds.y);
        CHECK(repeated.panels[i].bounds.width == repeatedAgain.panels[i].bounds.width);
        CHECK(repeated.panels[i].bounds.height == repeatedAgain.panels[i].bounds.height);
    }

    auto duplicateIds = makePanels(2);
    duplicateIds[1].id = duplicateIds[0].id;
    auto duplicateResult = calculatePanelLayout(desktop, duplicateIds, 1.0f);
    CHECK(duplicateResult.invalidInput && duplicateResult.panels.empty());

    auto invalidSize = makePanels(1);
    invalidSize[0].minimum.width = 0;
    auto invalidSizeResult = calculatePanelLayout(desktop, invalidSize, 1.0f);
    CHECK(invalidSizeResult.invalidInput && invalidSizeResult.panels.empty());

    auto invertedSize = makePanels(1);
    invertedSize[0].minimum.width = invertedSize[0].desired.width + 1;
    CHECK(calculatePanelLayout(desktop, invertedSize, 1.0f).invalidInput);

    CHECK((calculatePanelLayout(PanelRect{ 0, 0, 0, 100 }, makePanels(1), 1.0f).invalidInput));
    CHECK((calculatePanelLayout(PanelRect{ std::numeric_limits<int>::max() - 2, 0, 10, 10 },
                                 makePanels(1), 1.0f).invalidInput));

    auto negativeGap = calculatePanelLayout(desktop, makePanels(2), 1.0f, -50);
    CHECK(!negativeGap.invalidInput && !negativeGap.overflow);
    checkInsideAndDisjoint(negativeGap, desktop);

    auto badScale = calculatePanelLayout(desktop, makePanels(2),
                                         std::numeric_limits<float>::quiet_NaN());
    CHECK(!badScale.invalidInput && !badScale.overflow);

    auto extremeScale = calculatePanelLayout(desktop, makePanels(2),
                                              std::numeric_limits<float>::max());
    CHECK(extremeScale.overflow && extremeScale.invalidInput && extremeScale.panels.empty());

    if (failures != 0)
    {
        std::cerr << failures << " AuxiliaryPanelLayout check(s) failed.\n";
        return 1;
    }

    std::cout << "AuxiliaryPanelLayout contract tests passed.\n";
    return 0;
}
