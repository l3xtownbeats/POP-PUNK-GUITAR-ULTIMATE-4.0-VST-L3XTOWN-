#include "AuxiliaryPanelWindowManager.h"

#include <algorithm>

PPGUAuxiliaryPanelWindowManager::PPGUAuxiliaryPanelWindowManager(
    const PPGUThemeManager::Palette& initialPalette)
    : palette(initialPalette)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    for (auto* button : { &previousPageButton, &nextPageButton })
    {
        button->setColour(juce::TextButton::buttonColourId, palette.token("control_bg"));
        button->setColour(juce::TextButton::textColourOffId, palette.token("text_primary"));
        addAndMakeVisible(*button);
    }
    previousPageButton.setTooltip("Previous panel page");
    nextPageButton.setTooltip("Next panel page");
    previousPageButton.onClick = [this] { changePage(-1); };
    nextPageButton.onClick = [this] { changePage(1); };
    pageIndicator.setJustificationType(juce::Justification::centred);
    pageIndicator.setColour(juce::Label::textColourId, palette.token("text_secondary"));
    pageIndicator.setFont(juce::Font(10.0f, juce::Font::bold));
    addAndMakeVisible(pageIndicator);
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

    if (!window->open)
    {
        window->open = true;
        addAndMakeVisible(*window->overlay);
        currentPage = (getOpenPanelCount() - 1) / juce::jmax(1, pageCapacity);
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
    {
        currentPage = 0;
        setVisible(false);
    }
    else
        currentPage = juce::jmin(currentPage, getPageCount() - 1);
    updateLayout();
}

void PPGUAuxiliaryPanelWindowManager::closeAll()
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    expandedId = 0;
    currentPage = 0;
    for (auto& window : windows)
    {
        window.open = false;
        window.overlay->setVisible(false);
    }
    setVisible(false);
    updateLayout();
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

juce::String PPGUAuxiliaryPanelWindowManager::getOpenPanelIds() const
{
    juce::StringArray ids;
    for (const auto& window : windows)
        if (window.open) ids.add(juce::String(window.id));
    return ids.joinIntoString(",");
}

void PPGUAuxiliaryPanelWindowManager::restoreOpenPanels(const juce::String& ids)
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    closeAll();
    juce::StringArray values;
    values.addTokens(ids, ",", "");
    for (const auto& value : values) { const int id = value.getIntValue(); if (id > 0) openPanel(id); }
    currentPage = 0;
    updateLayout();
}

int PPGUAuxiliaryPanelWindowManager::getPageCount() const noexcept
{
    const auto range = ppgu::ui::calculatePanelPageRange(static_cast<std::size_t>(getOpenPanelCount()), 0, static_cast<std::size_t>(juce::jmax(1, pageCapacity)));
    return juce::jmax(1, static_cast<int>(range.pageCount));
}

void PPGUAuxiliaryPanelWindowManager::changePage(int delta)
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    const int pages = getPageCount();
    if (pages <= 1) return;
    currentPage = (currentPage + delta + pages) % pages;
    expandedId = 0;
    updateLayout();
}

void PPGUAuxiliaryPanelWindowManager::setPalette(
    const PPGUThemeManager::Palette& value)
{
    palette = value;
    for (auto& window : windows)
        window.overlay->setPalette(palette);
    for (auto* button : { &previousPageButton, &nextPageButton })
    {
        button->setColour(juce::TextButton::buttonColourId, palette.token("control_bg"));
        button->setColour(juce::TextButton::textColourOffId, palette.token("text_primary"));
    }
    pageIndicator.setColour(juce::Label::textColourId, palette.token("text_secondary"));
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
    if (updatingLayout) return;
    updatingLayout = true;
    std::vector<Window*> openWindows; openWindows.reserve(windows.size());
    for (auto& window : windows) if (window.open) openWindows.push_back(&window);
    const auto fullArea = getLocalBounds().reduced(18, 18);
    const int total = static_cast<int>(openWindows.size());
    pageCapacity = juce::jmin(panelsPerPage, juce::jmax(1, total));
    std::vector<ppgu::ui::PanelRequest> probeRequests;
    auto makeRequests = [&openWindows](int first, int count)
    {
        std::vector<ppgu::ui::PanelRequest> requests; requests.reserve(static_cast<std::size_t>(count));
        for (int i = 0; i < count; ++i) requests.push_back({ openWindows[static_cast<std::size_t>(first + i)]->id, { 520, 360 }, { 260, 190 } });
        return requests;
    };
    // Reduce page capacity until the current viewport can show every panel on the page.
    while (pageCapacity > 1)
    {
        auto testArea = fullArea;
        if (total > pageCapacity) testArea.removeFromBottom(34);
        probeRequests = makeRequests(0, pageCapacity);
        const auto probe = ppgu::ui::calculatePanelLayout({ testArea.getX(), testArea.getY(), testArea.getWidth(), testArea.getHeight() }, probeRequests, 1.0f, 12);
        if (!probe.invalidInput && !probe.overflow) break;
        --pageCapacity;
    }
    const int pageCount = getPageCount(); currentPage = juce::jlimit(0, pageCount - 1, currentPage);
    const bool hasPaging = pageCount > 1;
    previousPageButton.setVisible(hasPaging); nextPageButton.setVisible(hasPaging); pageIndicator.setVisible(hasPaging);
    auto area = fullArea;
    if (hasPaging)
    {
        auto footer = area.removeFromBottom(34);
        previousPageButton.setBounds(footer.removeFromLeft(42).reduced(2));
        nextPageButton.setBounds(footer.removeFromRight(42).reduced(2));
        pageIndicator.setBounds(footer);
        pageIndicator.setText("PAGE " + juce::String(currentPage + 1) + " / " + juce::String(pageCount), juce::dontSendNotification);
    }
    const auto pageRange = ppgu::ui::calculatePanelPageRange(openWindows.size(), currentPage, static_cast<std::size_t>(pageCapacity));
    const auto requests = makeRequests(static_cast<int>(pageRange.begin), static_cast<int>(pageRange.end - pageRange.begin));
    for (auto& window : windows) if (window.open) window.overlay->setVisible(false);
    if (expandedId != 0 && isOpen(expandedId))
    {
        const bool onPage = std::any_of(requests.begin(), requests.end(), [this](const ppgu::ui::PanelRequest& request) { return request.id == expandedId; });
        if (!onPage) expandedId = 0;
    }
    if (expandedId != 0 && !requests.empty())
    {
        const auto expandedLayout = ppgu::ui::calculateExpandedPanelLayout({ area.getX(), area.getY(), area.getWidth(), area.getHeight() }, requests, expandedId, 1.0f, 8);
        if (!expandedLayout.invalidInput && !expandedLayout.overflow)
        {
            for (const auto& placement : expandedLayout.panels)
            {
                auto* window = findWindow(placement.id); if (!window) continue;
                const bool isExpanded = placement.id == expandedId;
                window->overlay->setExpanded(isExpanded); window->overlay->setVisible(placement.visible);
                const auto& b = placement.bounds; window->overlay->setBounds(b.x,b.y,b.width,b.height);
                if (isExpanded) window->overlay->toBack(); else window->overlay->toFront(false);
            }
            updatingLayout = false; repaint(); return;
        }
        expandedId = 0; for (auto& window : windows) window.overlay->setExpanded(false);
    }
    const auto layout = ppgu::ui::calculatePanelLayout({ area.getX(), area.getY(), area.getWidth(), area.getHeight() }, requests, 1.0f, 12);
    for (const auto& placement : layout.panels)
    {
        auto* window = findWindow(placement.id); if (!window) continue;
        window->overlay->setVisible(placement.visible);
        if (placement.visible) { const auto& b = placement.bounds; window->overlay->setBounds(b.x,b.y,b.width,b.height); }
    }
    updatingLayout = false; repaint();
}
