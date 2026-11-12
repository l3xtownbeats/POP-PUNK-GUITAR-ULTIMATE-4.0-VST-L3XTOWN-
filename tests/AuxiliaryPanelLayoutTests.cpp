#include "AuxiliaryPanelLayout.h"

#include <cassert>
#include <set>
#include <vector>

using namespace ppgu::ui;

static std::vector<PanelRequest> makePanels(int count)
{
    std::vector<PanelRequest> panels;
    for (int i = 0; i < count; ++i)
        panels.push_back({ i + 1, { 520, 360 }, { 280, 190 } });
    return panels;
}

static bool overlaps(const PanelRect& a, const PanelRect& b)
{
    return a.x < b.x + b.width && b.x < a.x + a.width
        && a.y < b.y + b.height && b.y < a.y + a.height;
}

int main()
{
    const PanelRect desktop { 100, 80, 1600, 900 };
    assert(calculatePanelLayout(desktop, {}, 1.0f).panels.empty());

    auto one = calculatePanelLayout(desktop, makePanels(1), 1.0f);
    assert(one.panels.size() == 1 && one.panels[0].visible);
    assert(!one.compact && !one.overflow);

    for (int count : { 2, 3, 4, 6 })
    {
        auto layout = calculatePanelLayout(desktop, makePanels(count), 1.0f);
        assert(layout.panels.size() == static_cast<std::size_t>(count));
        assert(!layout.overflow);
        for (const auto& p : layout.panels)
        {
            assert(p.visible);
            assert(p.bounds.x >= desktop.x && p.bounds.y >= desktop.y);
            assert(p.bounds.x + p.bounds.width <= desktop.x + desktop.width);
            assert(p.bounds.y + p.bounds.height <= desktop.y + desktop.height);
            assert(p.bounds.width >= 280 && p.bounds.height >= 190);
        }
        for (std::size_t i = 0; i < layout.panels.size(); ++i)
            for (std::size_t j = i + 1; j < layout.panels.size(); ++j)
                assert(!overlaps(layout.panels[i].bounds, layout.panels[j].bounds));
    }

    auto compact = calculatePanelLayout({ 0, 0, 900, 520 }, makePanels(4), 1.0f);
    assert(compact.compact && !compact.overflow);

    auto portrait = calculatePanelLayout({ 0, 0, 520, 1100 }, makePanels(3), 1.0f);
    assert(!portrait.overflow);
    for (const auto& p : portrait.panels)
        assert(p.bounds.width >= 280 && p.bounds.height >= 190);

    auto overflow = calculatePanelLayout({ 0, 0, 420, 260 }, makePanels(4), 1.0f);
    assert(overflow.overflow && overflow.panels.size() == 4);
    int visible = 0;
    for (const auto& p : overflow.panels) visible += p.visible ? 1 : 0;
    assert(visible == 1);

    auto dpi = calculatePanelLayout({ 0, 0, 1800, 1000 }, makePanels(4), 1.5f);
    assert(!dpi.overflow);
    for (const auto& p : dpi.panels)
        assert(p.bounds.width >= 420 && p.bounds.height >= 285);

    auto repeated = calculatePanelLayout(desktop, makePanels(4), 1.0f);
    auto repeatedAgain = calculatePanelLayout(desktop, makePanels(4), 1.0f);
    assert(repeated.columns == repeatedAgain.columns && repeated.rows == repeatedAgain.rows);
    for (std::size_t i = 0; i < repeated.panels.size(); ++i)
    {
        assert(repeated.panels[i].bounds.x == repeatedAgain.panels[i].bounds.x);
        assert(repeated.panels[i].bounds.y == repeatedAgain.panels[i].bounds.y);
        assert(repeated.panels[i].bounds.width == repeatedAgain.panels[i].bounds.width);
        assert(repeated.panels[i].bounds.height == repeatedAgain.panels[i].bounds.height);
    }
    return 0;
}
