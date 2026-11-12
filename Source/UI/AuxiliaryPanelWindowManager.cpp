#include "AuxiliaryPanelWindowManager.h"

#include <algorithm>

PPGUAuxiliaryPanelWindowManager::PPGUAuxiliaryPanelWindowManager(
    const PPGUThemeManager::Palette& initialPalette)
    : palette(initialPalette)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setVisible(false);
}

bool PPGUAuxiliaryPanelWindowManager::addPanel(
    int id, const juce::String& title, juce::Component& content)
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    if (id <= 0 || title.isEmpty() || findWindow(id) != nullptr)
        return false;

    Window window;
    window.id = id;
    window.content = &content;
    window.overlay = std::make_unique<PPGUEditorOverlayPanel>(
        title, content,
        [this, id] { closePanel(id); },
        [this, id](bool expanded) {
            if (expanded)
                expandedId = id;
            else if (expandedId == id)
                expandedId = 0;
            updateLayout();
        },
        palette);
    windows.push_back(std::move(window));
    return true;
}

bool PPGUAuxiliaryPanelWindowManager::openPanel(int id)
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    auto* window = findWindow(id);
    if (window == nullptr)
        return false;

    if (!window->open && getOpenPanelCount() >= 6)
        return false;

    if (!window->open)
    {
        window->open = true;
        addAndMakeVisible(*window->overlay);
    }
    window->overlay->setVisible(true);
    setVisible(true);
    window->overlay->toFront(false);
    updateLayout();
    return true;
}

void PPGUAuxiliaryPanelWindowManager::closePanel(int id)
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    auto* window = findWindow(id);
    if (window == nullptr)
        return;

    window->open = false;
    window->overlay->setVisible(false);
    if (expandedId == id)
        expandedId = 0;

    if (getOpenPanelCount() == 0)
        setVisible(false);
    updateLayout();
}

void PPGUAuxiliaryPanelWindowManager::closeAll()
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    expandedId = 0;
    for (auto& window : windows)
    {
        window.open = false;
        window.overlay->setVisible(false);
    }
    setVisible(false);
    repaint();
}

bool PPGUAuxiliaryPanelWindowManager::isOpen(int id) const noexcept
{
    const auto* window = findWindow(id);
    return window != nullptr && window->open;
}

int PPGUAuxiliaryPanelWindowManager::getOpenPanelCount() const noexcept
{
    return static_cast<int>(std::count_if(windows.begin(), windows.end(),
        [](const Window& window) { return window.open; }));
}

void PPGUAuxiliaryPanelWindowManager::setPalette(
    const PPGUThemeManager::Palette& value)
{
    palette = value;
    for (auto& window : windows)
        window.overlay->setPalette(palette);
    repaint();
}

void PPGUAuxiliaryPanelWindowManager::paint(juce::Graphics& g)
{
    if (getOpenPanelCount() > 0)
        g.fillAll(palette.token("shadow").withAlpha(0.58f));
}

void PPGUAuxiliaryPanelWindowManager::resized()
{
    updateLayout();
}

PPGUAuxiliaryPanelWindowManager::Window*
PPGUAuxiliaryPanelWindowManager::findWindow(int id) noexcept
{
    const auto it = std::find_if(windows.begin(), windows.end(),
        [id](const Window& window) { return window.id == id; });
    return it == windows.end() ? nullptr : &*it;
}

const PPGUAuxiliaryPanelWindowManager::Window*
PPGUAuxiliaryPanelWindowManager::findWindow(int id) const noexcept
{
    const auto it = std::find_if(windows.begin(), windows.end(),
        [id](const Window& window) { return window.id == id; });
    return it == windows.end() ? nullptr : &*it;
}

void PPGUAuxiliaryPanelWindowManager::updateLayout()
{
    if (updatingLayout)
        return;
    updatingLayout = true;

    const auto area = getLocalBounds().reduced(18, 18);
    if (expandedId != 0 && isOpen(expandedId))
    {
        // Keep the selected panel large while every other open panel remains
        // visible in a non-overlapping dock. The former implementation placed
        // all compact panels at the same coordinates and sent them behind the
        // expanded panel, making them inaccessible.
        const auto expanded = std::find_if(windows.begin(), windows.end(),
            [this](const Window& window) { return window.id == expandedId; });
        const int dockedCount = getOpenPanelCount() - 1;
        const int gap = 8;
        const int dockWidth = dockedCount > 0
            ? juce::jlimit(240, juce::jmax(240, area.getWidth() / 3), area.getWidth() / 2)
            : 0;
        const int mainWidth = dockedCount > 0
            ? juce::jmax(260, area.getWidth() - dockWidth - gap)
            : area.getWidth();

        if (expanded != windows.end())
        {
            expanded->overlay->setExpanded(true);
            expanded->overlay->setVisible(true);
            expanded->overlay->setBounds(area.getX(), area.getY(), mainWidth, area.getHeight());
            expanded->overlay->toBack();
        }

        int dockIndex = 0;
        for (auto& window : windows)
        {
            if (!window.open || window.id == expandedId)
                continue;

            window.overlay->setExpanded(false);
            window.overlay->setVisible(true);
            const int availableHeight = area.getHeight() - gap * (dockedCount - 1);
            const int cellHeight = dockedCount > 0 ? availableHeight / dockedCount : area.getHeight();
            const int y = area.getY() + dockIndex * (cellHeight + gap);
            const int height = dockIndex == dockedCount - 1
                ? area.getBottom() - y : cellHeight;
            window.overlay->setBounds(area.getRight() - dockWidth, y, dockWidth, height);
            window.overlay->toFront(false);
            ++dockIndex;
        }

        updatingLayout = false;
        repaint();
        return;
    }

    std::vector<ppgu::ui::PanelRequest> requests;
    requests.reserve(windows.size());
    for (const auto& window : windows)
    {
        if (!window.open)
            continue;
        requests.push_back({ window.id, { 520, 360 }, { 260, 190 } });
    }

    const auto layout = ppgu::ui::calculatePanelLayout(
        { area.getX(), area.getY(), area.getWidth(), area.getHeight() },
        requests, 1.0f, 12);

    for (auto& window : windows)
    {
        if (!window.open)
            continue;
        const auto placement = std::find_if(layout.panels.begin(), layout.panels.end(),
            [&window](const ppgu::ui::PanelPlacement& item) { return item.id == window.id; });
        if (placement == layout.panels.end() || !placement->visible)
        {
            window.overlay->setVisible(false);
            continue;
        }
        const auto& b = placement->bounds;
        window.overlay->setBounds(b.x, b.y, b.width, b.height);
    }

    updatingLayout = false;
    repaint();
}
