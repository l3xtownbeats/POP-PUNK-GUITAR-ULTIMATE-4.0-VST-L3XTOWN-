#include "PPGUMultiWindowWorkspace.h"

#include <algorithm>
#include <cmath>

class PPGUMultiWindowWorkspace::Window final : public juce::Component
{
public:
    Window(PPGUMultiWindowWorkspace& ownerIn, PanelSpec&& spec,
           juce::Colour panel, juce::Colour border,
           juce::Colour heading, juce::Colour text)
        : owner(ownerIn), panelId(spec.id), desired(spec.desired), minimum(spec.minimum),
          content(std::move(spec.content)),
          resizeCorner(this, &boundsConstrainer)
    {
        title.setText(spec.title, juce::dontSendNotification);
        title.setFont(juce::Font(14.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, heading);
        title.setJustificationType(juce::Justification::centredLeft);
        title.setInterceptsMouseClicks(false, false);
        addAndMakeVisible(title);

        closeButton.setButtonText(juce::String::charToString(0x00d7));
        closeButton.setTooltip("Close panel");
        closeButton.setColour(juce::TextButton::buttonColourId, panel.darker(0.2f));
        closeButton.setColour(juce::TextButton::textColourOffId, text);
        closeButton.setColour(juce::TextButton::buttonOnColourId, border);
        closeButton.onClick = [this] { owner.handleClose(panelId); };
        addAndMakeVisible(closeButton);

        maximizeButton.setButtonText("□");
        maximizeButton.setTooltip("MAXIMIZE / RESTORE");
        maximizeButton.setColour(juce::TextButton::buttonColourId, panel.darker(0.2f));
        maximizeButton.setColour(juce::TextButton::textColourOffId, text);
        maximizeButton.onClick = [this] { toggleMaximize(); };
        addAndMakeVisible(maximizeButton);
        boundsConstrainer.setMinimumSize(static_cast<int>(minimum.width), static_cast<int>(minimum.height));
        resizeCorner.setAlwaysOnTop(true);
        addAndMakeVisible(resizeCorner);

        if (content != nullptr)
        {
            content->setBounds(0, 0, desired.width, desired.height);
            addAndMakeVisible(*content);
        }

        panelColour = panel;
        borderColour = border;
    }

    int getPanelId() const noexcept { return panelId; }
    ppgu::ui::PanelSize getDesiredSize() const noexcept { return desired; }
    ppgu::ui::PanelSize getMinimumSize() const noexcept { return minimum; }
    bool isMaximized() const noexcept { return maximized; }
    void restoreBounds(juce::Rectangle<int> bounds) { setBounds(bounds); maximized = false; }

    void setColours(juce::Colour panel, juce::Colour border,
                    juce::Colour heading, juce::Colour text)
    {
        panelColour = panel;
        borderColour = border;
        title.setColour(juce::Label::textColourId, heading);
        closeButton.setColour(juce::TextButton::buttonColourId, panel.darker(0.2f));
        closeButton.setColour(juce::TextButton::textColourOffId, text);
        closeButton.setColour(juce::TextButton::buttonOnColourId, border);
        maximizeButton.setColour(juce::TextButton::buttonColourId, panel.darker(0.2f));
        maximizeButton.setColour(juce::TextButton::textColourOffId, text);
        repaint();
    }

    void setPanelBounds(juce::Rectangle<int> bounds)
    {
        if (maximized)
        {
            maximized = false;
            maximizeButton.setButtonText("□");
        }
        setBounds(bounds);
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        const auto point = event.getPosition();
        draggingHeader = point.y < 42
            && !closeButton.getBounds().contains(point)
            && !maximizeButton.getBounds().contains(point);
        dragStartBounds = getBounds();
        dragStartScreen = event.getEventRelativeTo(getParentComponent()).getPosition();
    }

    void mouseDrag(const juce::MouseEvent& event) override
    {
        if (!event.mouseWasDraggedSinceMouseDown())
            return;
        const auto relative = event.getEventRelativeTo(getParentComponent()).getPosition();
        if (draggingHeader)
        {
            auto bounds = dragStartBounds.translated(relative.x - dragStartScreen.x, relative.y - dragStartScreen.y);
            bounds.setPosition(juce::jmax(0, bounds.getX()), juce::jmax(0, bounds.getY()));
            setBounds(bounds);
            owner.handleGeometryChanged(panelId);
        }
    }

    void mouseDoubleClick(const juce::MouseEvent& event) override
    {
        if (event.y < 42 && !closeButton.getBounds().contains(event.getPosition())
            && !maximizeButton.getBounds().contains(event.getPosition()))
            toggleMaximize();
    }

    void mouseUp(const juce::MouseEvent&) override
    {
        draggingHeader = false;
        owner.handleGeometryChanged(panelId);
    }

    void toggleMaximize()
    {
        if (!maximized)
        {
            restoreBoundsBeforeMaximize = getBounds();
            maximized = true;
            setBounds(owner.viewport.getViewArea());
        }
        else
        {
            maximized = false;
            setBounds(restoreBoundsBeforeMaximize);
        }
        maximizeButton.setButtonText(maximized ? "❐" : "□");
        owner.handleGeometryChanged(panelId);
    }

    void paint(juce::Graphics& g) override
    {
        auto area = getLocalBounds().toFloat();
        g.setColour(panelColour);
        g.fillRoundedRectangle(area, 8.0f);
        g.setColour(borderColour);
        g.drawRoundedRectangle(area.reduced(0.5f), 8.0f, 1.0f);
        g.setColour(borderColour.withAlpha(0.55f));
        g.drawLine(8.0f, 42.0f, static_cast<float>(getWidth() - 8), 42.0f, 1.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);
        auto header = area.removeFromTop(32);
        closeButton.setBounds(header.removeFromRight(28));
        header.removeFromRight(4);
        maximizeButton.setBounds(header.removeFromRight(28));
        title.setBounds(header);
        area.removeFromTop(10);
        if (content != nullptr)
            content->setBounds(area);
        resizeCorner.setBounds(getWidth() - 18, getHeight() - 18, 18, 18);
        owner.handleGeometryChanged(panelId);
    }

    void paintOverChildren(juce::Graphics& g) override
    {
        if (!maximized)
        {
            juce::Path grip;
            grip.startNewSubPath(static_cast<float>(getWidth() - 5), static_cast<float>(getHeight() - 16));
            grip.lineTo(static_cast<float>(getWidth() - 16), static_cast<float>(getHeight() - 5));
            g.setColour(borderColour.brighter(0.4f));
            g.strokePath(grip, juce::PathStrokeType(2.0f));
        }
    }

private:
    PPGUMultiWindowWorkspace& owner;
    int panelId;
    ppgu::ui::PanelSize desired;
    ppgu::ui::PanelSize minimum;
    std::unique_ptr<juce::Component> content;
    juce::Label title;
    juce::TextButton closeButton, maximizeButton;
    juce::ComponentBoundsConstrainer boundsConstrainer;
    juce::ResizableCornerComponent resizeCorner;
    juce::Colour panelColour, borderColour;
    juce::Rectangle<int> dragStartBounds, restoreBoundsBeforeMaximize;
    juce::Point<int> dragStartScreen;
    bool draggingHeader = false;
    bool maximized = false;
};

class PPGUMultiWindowWorkspace::TileSurface final : public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::transparentBlack);
    }
};

PPGUMultiWindowWorkspace::PPGUMultiWindowWorkspace()
{
    viewport.setViewedComponent(nullptr, false);
    viewport.setOpaque(false);
    viewport.setScrollBarsShown(true, true);
    viewport.setScrollBarThickness(10);
    addAndMakeVisible(viewport);
    tileSurface = std::make_unique<TileSurface>();
    tileSurface->setOpaque(false);
    viewport.setViewedComponent(tileSurface.get(), false);

    juce::PropertiesFile::Options options;
    options.applicationName = "PPGU";
    options.filenameSuffix = ".settings";
    options.folderName = "L3XTOWN";
    options.osxLibrarySubFolder = "Application Support";
    ownedProperties = std::make_unique<juce::PropertiesFile>(options);
    persistenceProperties = ownedProperties.get();
}

PPGUMultiWindowWorkspace::~PPGUMultiWindowWorkspace()
{
    viewport.setViewedComponent(nullptr, false);
    windows.clear();
    tileSurface.reset();
}

bool PPGUMultiWindowWorkspace::openPanel(PanelSpec spec)
{
    if (spec.id <= 0 || spec.title.trim().isEmpty() || spec.content == nullptr
        || spec.desired.width <= 0 || spec.desired.height <= 0
        || spec.minimum.width <= 0 || spec.minimum.height <= 0
        || spec.minimum.width > spec.desired.width
        || spec.minimum.height > spec.desired.height
        || isPanelOpen(spec.id))
        return false;

    auto window = std::make_unique<Window>(*this, std::move(spec),
        panelColour, borderColour, headingColour, textColour);
    const auto panelId = window->getPanelId();
    auto bounds = juce::Rectangle<int>(nextCascadeOffset, nextCascadeOffset,
        static_cast<int>(window->getDesiredSize().width), static_cast<int>(window->getDesiredSize().height));
    nextCascadeOffset = (nextCascadeOffset + 34) % 170;
    if (persistenceProperties != nullptr)
    {
        juce::StringArray values;
        values.addTokens(persistenceProperties->getValue(geometryKey(panelId, "bounds")), ",", "");
        if (values.size() == 4)
            bounds = { values[0].getIntValue(), values[1].getIntValue(), values[2].getIntValue(), values[3].getIntValue() };
    }
    window->setBounds(bounds);
    tileSurface->addAndMakeVisible(*window);
    windows.push_back(std::move(window));
    layoutPanels();
    return true;
}

bool PPGUMultiWindowWorkspace::closePanel(int id)
{
    const auto it = std::find_if(windows.begin(), windows.end(),
        [id](const auto& item) { return item->getPanelId() == id; });
    if (it == windows.end())
        return false;

    tileSurface->removeChildComponent(it->get());
    windows.erase(it);
    layoutPanels();
    return true;
}

void PPGUMultiWindowWorkspace::closeAllPanels()
{
    for (auto& window : windows)
        tileSurface->removeChildComponent(window.get());
    windows.clear();
    layoutPanels();
}

bool PPGUMultiWindowWorkspace::isPanelOpen(int id) const noexcept
{
    return std::any_of(windows.begin(), windows.end(),
        [id](const auto& item) { return item->getPanelId() == id; });
}

int PPGUMultiWindowWorkspace::getOpenPanelCount() const noexcept
{
    return static_cast<int>(windows.size());
}

void PPGUMultiWindowWorkspace::setPanelGap(int pixels)
{
    panelGap = juce::jlimit(0, 64, pixels);
    layoutPanels();
}

void PPGUMultiWindowWorkspace::setPanelScale(float scale)
{
    panelScale = std::isfinite(scale) ? juce::jlimit(0.5f, 3.0f, scale) : 1.0f;
    layoutPanels();
}

void PPGUMultiWindowWorkspace::setPersistenceProperties(juce::PropertiesFile* properties)
{
    persistenceProperties = properties;
}

void PPGUMultiWindowWorkspace::setSurfaceColours(juce::Colour background,
                                                  juce::Colour panel,
                                                  juce::Colour border,
                                                  juce::Colour heading,
                                                  juce::Colour text)
{
    backgroundColour = background;
    panelColour = panel;
    borderColour = border;
    headingColour = heading;
    textColour = text;
    for (auto& window : windows)
        window->setColours(panelColour, borderColour, headingColour, textColour);
    repaint();
    layoutPanels();
}

void PPGUMultiWindowWorkspace::resized()
{
    viewport.setBounds(getLocalBounds());
    layoutPanels();
}

void PPGUMultiWindowWorkspace::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);
}

juce::String PPGUMultiWindowWorkspace::geometryKey(int id, const juce::String& suffix) const
{
    return "workspace.panel." + juce::String(id) + "." + suffix;
}

void PPGUMultiWindowWorkspace::handleGeometryChanged(int id)
{
    if (layingOutPanels)
        return;

    const auto it = std::find_if(windows.begin(), windows.end(),
        [id](const auto& item) { return item->getPanelId() == id; });
    if (it == windows.end())
        return;

    const auto bounds = (*it)->getBounds();
    if (persistenceProperties != nullptr && !(*it)->isMaximized())
    {
        persistenceProperties->setValue(geometryKey(id, "bounds"),
            juce::String(bounds.getX()) + "," + juce::String(bounds.getY()) + ","
            + juce::String(bounds.getWidth()) + "," + juce::String(bounds.getHeight()));
        persistenceProperties->saveIfNeeded();
    }
    // Manual drag/resize/maximize is user geometry, not a request to retile.
    // Automatic tiling runs only when the panel set or workspace size changes.
}

void PPGUMultiWindowWorkspace::handleClose(int id)
{
    closePanel(id);
    if (windows.empty())
        setVisible(false);
}

void PPGUMultiWindowWorkspace::reorderPanels(int draggedId, int targetId)
{
    const auto dragged = std::find_if(windows.begin(), windows.end(),
        [draggedId](const auto& item) { return item->getPanelId() == draggedId; });
    const auto target = std::find_if(windows.begin(), windows.end(),
        [targetId](const auto& item) { return item->getPanelId() == targetId; });
    if (dragged == windows.end() || target == windows.end() || dragged == target)
        return;

    std::iter_swap(dragged, target);
    layoutPanels();
}

void PPGUMultiWindowWorkspace::layoutPanels()
{
    if (tileSurface == nullptr || layingOutPanels)
        return;

    juce::ScopedValueSetter<bool> layoutGuard(layingOutPanels, true);
    const auto view = viewport.getViewArea();
    const int viewWidth = juce::jmax(1, view.getWidth());
    const int viewHeight = juce::jmax(1, view.getHeight());

    std::vector<ppgu::ui::PanelRequest> requests;
    requests.reserve(windows.size());
    for (const auto& window : windows)
        requests.push_back({ window->getPanelId(), window->getDesiredSize(),
                             window->getMinimumSize() });

    const auto layout = ppgu::ui::calculatePanelLayout(
        { 0, 0, viewWidth, viewHeight }, requests, panelScale, panelGap);

    if (!layout.invalidInput && !layout.overflow
        && layout.panels.size() == windows.size())
    {
        for (const auto& placement : layout.panels)
        {
            const auto it = std::find_if(windows.begin(), windows.end(),
                [&placement](const auto& item)
                {
                    return item->getPanelId() == placement.id;
                });
            if (it != windows.end() && placement.visible)
            {
                const auto& b = placement.bounds;
                (*it)->setPanelBounds({ b.x, b.y, b.width, b.height });
            }
        }

        tileSurface->setSize(viewWidth, viewHeight);
        viewport.setViewPosition(0, 0);

        if (persistenceProperties != nullptr)
        {
            for (const auto& window : windows)
            {
                if (window->isMaximized())
                    continue;
                const auto bounds = window->getBounds();
                persistenceProperties->setValue(geometryKey(window->getPanelId(), "bounds"),
                    juce::String(bounds.getX()) + "," + juce::String(bounds.getY()) + ","
                    + juce::String(bounds.getWidth()) + "," + juce::String(bounds.getHeight()));
            }
            persistenceProperties->saveIfNeeded();
        }
        return;
    }

    // If the viewport cannot satisfy every panel's minimum size, retain every
    // panel on a scrollable surface instead of clipping or silently hiding it.
    int surfaceWidth = viewWidth;
    int surfaceHeight = viewHeight;
    for (auto& window : windows)
    {
        auto bounds = window->getBounds();
        bounds.setWidth(juce::jmax(static_cast<int>(window->getMinimumSize().width), bounds.getWidth()));
        bounds.setHeight(juce::jmax(static_cast<int>(window->getMinimumSize().height), bounds.getHeight()));
        bounds.setPosition(juce::jmax(0, bounds.getX()), juce::jmax(0, bounds.getY()));
        window->setPanelBounds(bounds);
        surfaceWidth = juce::jmax(surfaceWidth, bounds.getRight() + panelGap);
        surfaceHeight = juce::jmax(surfaceHeight, bounds.getBottom() + panelGap);
    }

    tileSurface->setSize(surfaceWidth, surfaceHeight);
}
