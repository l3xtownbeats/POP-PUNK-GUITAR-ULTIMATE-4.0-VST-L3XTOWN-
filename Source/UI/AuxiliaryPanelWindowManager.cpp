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

    if (!window->overlay->isVisible())
    {
        window->overlay->setVisible(true);
        addAndMakeVisible(*window->overlay);
    }
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
        window.overlay->setVisible(false);
    setVisible(false);
    repaint();
}

bool PPGUAuxiliaryPanelWindowManager::isOpen(int id) const noexcept
{
    const auto* window = findWindow(id);
    return window != nullptr && window->overlay->isVisible();
}

int PPGUAuxiliaryPanelWindowManager::getOpenPanelCount() const noexcept
{
    return static_cast<int>(std::count_if(windows.begin(), windows.end(),
        [](const Window& window) { return window.overlay->isVisible(); }));
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
        for (auto& window : windows)
        {
            const bool expanded = window.id == expandedId && window.overlay->isVisible();
            if (window.overlay->isVisible())
                window.overlay->setVisible(expanded);
            if (expanded)
                window.overlay->setBounds(area);
        }
        updatingLayout = false;
        repaint();
        return;
    }

    std::vector<ppgu::ui::PanelRequest> requests;
    requests.reserve(windows.size());
    for (const auto& window : windows)
    {
        if (!window.overlay->isVisible())
            continue;
        requests.push_back({ window.id, { 520, 360 }, { 260, 190 } });
    }

    const auto layout = ppgu::ui::calculatePanelLayout(
        { area.getX(), area.getY(), area.getWidth(), area.getHeight() },
        requests, 1.0f, 12);

    for (auto& window : windows)
    {
        if (!window.overlay->isVisible())
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
