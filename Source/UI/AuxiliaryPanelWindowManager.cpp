#include "AuxiliaryPanelWindowManager.h"
#include "AuxiliaryPanelLayout.h"

#include <algorithm>
#include <utility>

class PPGUAuxiliaryDocumentWindow final : public juce::DocumentWindow
{
public:
    PPGUAuxiliaryDocumentWindow(PPGUAuxiliaryPanelWindowManager& ownerToUse,
                                int panelIdToUse,
                                const juce::String& title,
                                juce::Component& content,
                                const PPGUThemeManager::Palette& initialPalette)
        : juce::DocumentWindow(title, initialPalette.panel,
              juce::DocumentWindow::closeButton | juce::DocumentWindow::maximiseButton, true),
          owner(ownerToUse), panelId(panelIdToUse)
    {
        setUsingNativeTitleBar(false);
        setTitleBarHeight(36);
        setTitleBarButtonsRequired(
            juce::DocumentWindow::closeButton | juce::DocumentWindow::maximiseButton, false);
        setTitleBarTextCentred(false);
        setResizable(true, true);
        setResizeLimits(260, 190, 1800, 1200);
        setDropShadowEnabled(true);
        setContentNonOwned(&content, false);
        applyPalette(initialPalette);
        setVisible(false);
    }

    ~PPGUAuxiliaryDocumentWindow() override
    {
        clearContentComponent();
    }

    void closeButtonPressed() override
    {
        owner.closePanel(panelId);
    }

    void maximiseButtonPressed() override
    {
        owner.toggleExpanded(panelId);
    }

    void setExpandedState(bool expanded)
    {
        if (auto* button = getMaximiseButton())
            button->setTooltip(expanded ? "Restore panel workspace" : "Expand panel workspace");
    }

    void applyPalette(const PPGUThemeManager::Palette& value)
    {
        setBackgroundColour(value.panel);
        setColour(juce::DocumentWindow::textColourId, value.textPrimary);
        repaint();
    }

private:
    PPGUAuxiliaryPanelWindowManager& owner;
    const int panelId;
};

struct PPGUAuxiliaryPanelWindowManager::Entry
{
    int id = 0;
    bool open = false;
    juce::Rectangle<int> normalBounds;
    PaletteUpdater paletteUpdater;
    std::unique_ptr<PPGUAuxiliaryDocumentWindow> window;
};

PPGUAuxiliaryPanelWindowManager::PPGUAuxiliaryPanelWindowManager(
    const PPGUThemeManager::Palette& initialPalette,
    juce::PropertiesFile* persistentSettings,
    juce::Component* displayAnchorToUse)
    : palette(initialPalette), settings(persistentSettings), displayAnchor(displayAnchorToUse)
{
}

PPGUAuxiliaryPanelWindowManager::~PPGUAuxiliaryPanelWindowManager()
{
    closeAll();
    entries.clear();
}

bool PPGUAuxiliaryPanelWindowManager::addPanel(
    int id, const juce::String& title, juce::Component& content,
    PaletteUpdater paletteUpdater)
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    if (id <= 0 || title.isEmpty() || findEntry(id) != nullptr)
        return false;

    auto entry = std::make_unique<Entry>();
    entry->id = id;
    entry->paletteUpdater = std::move(paletteUpdater);
    entry->window = std::make_unique<PPGUAuxiliaryDocumentWindow>(
        *this, id, title, content, palette);
    entries.push_back(std::move(entry));
    return true;
}

bool PPGUAuxiliaryPanelWindowManager::openPanel(int id)
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    auto* entry = findEntry(id);
    if (entry == nullptr)
        return false;

    if (!entry->open && settings != nullptr)
    {
        const auto prefix = "auxPanel." + juce::String(id) + ".";
        const auto saved = settings->getValue(prefix + "windowState");
        if (saved.isNotEmpty())
            entry->window->restoreWindowStateFromString(saved);

        const int x = settings->getIntValue(prefix + "normalX", -1);
        const int y = settings->getIntValue(prefix + "normalY", -1);
        const int w = settings->getIntValue(prefix + "normalW", 0);
        const int h = settings->getIntValue(prefix + "normalH", 0);
        if (x >= 0 && y >= 0 && w >= 260 && h >= 190)
            entry->normalBounds = { x, y, w, h };
    }

    entry->open = true;
    focusedId = id;
    updateLayout();
    entry->window->setVisible(true);
    entry->window->toFront(true);
    return true;
}

void PPGUAuxiliaryPanelWindowManager::closePanel(int id)
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    auto* entry = findEntry(id);
    if (entry == nullptr || !entry->open)
        return;

    saveWindowState(*entry);
    entry->open = false;
    entry->window->setVisible(false);
    if (expandedId == id)
        expandedId = 0;
    if (focusedId == id)
    {
        focusedId = 0;
        for (const auto& candidate : entries)
            if (candidate->open)
            {
                focusedId = candidate->id;
                break;
            }
    }
    updateLayout();
}

void PPGUAuxiliaryPanelWindowManager::closeAll()
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    for (auto& entry : entries)
    {
        if (entry->open)
            saveWindowState(*entry);
        entry->open = false;
        if (entry->window)
        {
            entry->window->setExpandedState(false);
            entry->window->setVisible(false);
        }
    }
    expandedId = 0;
    focusedId = 0;
    if (settings != nullptr)
        settings->saveIfNeeded();
}

bool PPGUAuxiliaryPanelWindowManager::isOpen(int id) const noexcept
{
    const auto* entry = findEntry(id);
    return entry != nullptr && entry->open;
}

int PPGUAuxiliaryPanelWindowManager::getOpenPanelCount() const noexcept
{
    return static_cast<int>(std::count_if(entries.begin(), entries.end(),
        [](const auto& entry) { return entry->open; }));
}

void PPGUAuxiliaryPanelWindowManager::setPalette(
    const PPGUThemeManager::Palette& value)
{
    palette = value;
    for (auto& entry : entries)
    {
        entry->window->applyPalette(palette);
        if (entry->paletteUpdater)
            entry->paletteUpdater(palette);
    }
}

PPGUAuxiliaryPanelWindowManager::Entry*
PPGUAuxiliaryPanelWindowManager::findEntry(int id) noexcept
{
    const auto it = std::find_if(entries.begin(), entries.end(),
        [id](const auto& entry) { return entry->id == id; });
    return it == entries.end() ? nullptr : it->get();
}

const PPGUAuxiliaryPanelWindowManager::Entry*
PPGUAuxiliaryPanelWindowManager::findEntry(int id) const noexcept
{
    const auto it = std::find_if(entries.begin(), entries.end(),
        [id](const auto& entry) { return entry->id == id; });
    return it == entries.end() ? nullptr : it->get();
}

void PPGUAuxiliaryPanelWindowManager::toggleExpanded(int id)
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    if (!isOpen(id))
        return;

    if (expandedId == id)
    {
        expandedId = 0;
    }
    else
    {
        if (getOpenPanelCount() == 1)
            if (auto* entry = findEntry(id))
                entry->normalBounds = entry->window->getBounds();
        expandedId = id;
    }
    focusedId = id;
    updateLayout();
}

void PPGUAuxiliaryPanelWindowManager::saveWindowState(Entry& entry)
{
    if (settings == nullptr || entry.window == nullptr)
        return;

    if (expandedId == 0 && getOpenPanelCount() == 1)
        entry.normalBounds = entry.window->getBounds();

    const auto prefix = "auxPanel." + juce::String(entry.id) + ".";
    settings->setValue(prefix + "windowState", entry.window->getWindowStateAsString());
    if (!entry.normalBounds.isEmpty())
    {
        settings->setValue(prefix + "normalX", entry.normalBounds.getX());
        settings->setValue(prefix + "normalY", entry.normalBounds.getY());
        settings->setValue(prefix + "normalW", entry.normalBounds.getWidth());
        settings->setValue(prefix + "normalH", entry.normalBounds.getHeight());
    }
}

void PPGUAuxiliaryPanelWindowManager::updateLayout()
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    if (getOpenPanelCount() == 0)
        return;

    const auto& displays = juce::Desktop::getInstance().getDisplays();
    const auto* display = displayAnchor != nullptr
        ? displays.getDisplayForRect(displayAnchor->getScreenBounds()) : nullptr;
    if (display == nullptr)
        display = displays.getPrimaryDisplay();

    auto area = display != nullptr ? display->userArea : juce::Rectangle<int>(0, 0, 1280, 720);
    area = area.reduced(12);
    if (area.isEmpty())
        area = { 0, 0, 1280, 720 };

    constexpr float logicalScale = 1.0f; // userArea and top-level Component bounds are logical pixels.
    std::vector<ppgu::ui::PanelRequest> requests;
    requests.reserve(entries.size());
    for (const auto& entry : entries)
        if (entry->open)
            requests.push_back({ entry->id, { 760, 560 }, { 260, 190 } });

    if (expandedId != 0 && isOpen(expandedId))
    {
        const auto expanded = ppgu::ui::calculateExpandedPanelLayout(
            { area.getX(), area.getY(), area.getWidth(), area.getHeight() },
            requests, expandedId, logicalScale, 8);
        if (!expanded.invalidInput && !expanded.overflow)
        {
            for (const auto& placement : expanded.panels)
            {
                auto* entry = findEntry(placement.id);
                if (entry == nullptr)
                    continue;
                entry->window->setExpandedState(placement.id == expandedId);
                entry->window->setVisible(placement.visible);
                if (placement.visible)
                    entry->window->setBounds(placement.bounds.x, placement.bounds.y,
                                             placement.bounds.width, placement.bounds.height);
            }
            return;
        }
        expandedId = 0;
    }

    if (requests.size() == 1)
    {
        auto* entry = findEntry(requests.front().id);
        if (entry == nullptr)
            return;
        entry->window->setExpandedState(false);
        auto bounds = entry->normalBounds.isEmpty() ? entry->window->getBounds() : entry->normalBounds;
        if (bounds.getWidth() < 260 || bounds.getHeight() < 190
            || !area.contains(bounds.getCentre()))
            bounds = area.withSizeKeepingCentre(
                juce::jmin(760, area.getWidth()), juce::jmin(560, area.getHeight()));
        else
            bounds = bounds.constrainedWithin(area);
        entry->normalBounds = bounds;
        entry->window->setBounds(bounds);
        entry->window->setVisible(true);
        return;
    }

    const auto layout = ppgu::ui::calculatePanelLayout(
        { area.getX(), area.getY(), area.getWidth(), area.getHeight() },
        requests, logicalScale, 12);

    if (layout.invalidInput)
        return;

    if (layout.overflow)
    {
        auto* focused = findEntry(focusedId);
        if (focused == nullptr || !focused->open)
            focused = findEntry(requests.front().id);
        for (const auto& entry : entries)
        {
            const bool visible = entry->open && entry.get() == focused;
            entry->window->setExpandedState(false);
            entry->window->setVisible(visible);
            if (visible)
                entry->window->setBounds(area);
        }
        return;
    }

    for (const auto& placement : layout.panels)
    {
        auto* entry = findEntry(placement.id);
        if (entry == nullptr)
            continue;
        entry->window->setExpandedState(false);
        entry->window->setVisible(placement.visible);
        if (placement.visible)
            entry->window->setBounds(placement.bounds.x, placement.bounds.y,
                                     placement.bounds.width, placement.bounds.height);
    }
}
