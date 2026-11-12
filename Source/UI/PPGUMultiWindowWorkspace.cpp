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
          content(std::move(spec.content))
    {
        title.setText(spec.title, juce::dontSendNotification);
        title.setFont(juce::Font(14.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, heading);
        title.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(title);

        closeButton.setButtonText(juce::String::charToString(0x00d7));
        closeButton.setTooltip("Close panel");
        closeButton.setColour(juce::TextButton::buttonColourId, panel.darker(0.2f));
        closeButton.setColour(juce::TextButton::textColourOffId, text);
        closeButton.setColour(juce::TextButton::buttonOnColourId, border);
        closeButton.onClick = [this] { owner.handleClose(panelId); };
        addAndMakeVisible(closeButton);

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

    void setColours(juce::Colour panel, juce::Colour border,
                    juce::Colour heading, juce::Colour text)
    {
        panelColour = panel;
        borderColour = border;
        title.setColour(juce::Label::textColourId, heading);
        closeButton.setColour(juce::TextButton::buttonColourId, panel.darker(0.2f));
        closeButton.setColour(juce::TextButton::textColourOffId, text);
        closeButton.setColour(juce::TextButton::buttonOnColourId, border);
        repaint();
    }

    void setPanelBounds(juce::Rectangle<int> bounds)
    {
        setBounds(bounds);
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
        title.setBounds(header);
        area.removeFromTop(10);
        if (content != nullptr)
            content->setBounds(area);
    }

private:
    PPGUMultiWindowWorkspace& owner;
    int panelId;
    ppgu::ui::PanelSize desired;
    ppgu::ui::PanelSize minimum;
    std::unique_ptr<juce::Component> content;
    juce::Label title;
    juce::TextButton closeButton;
    juce::Colour panelColour, borderColour;
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
    viewport.setScrollBarsShown(true, true);
    viewport.setScrollBarThickness(10);
    viewport.setColour(juce::Viewport::backgroundColourId, backgroundColour);
    addAndMakeVisible(viewport);
    tileSurface = std::make_unique<TileSurface>();
    tileSurface->setOpaque(false);
    viewport.setViewedComponent(tileSurface.get(), false);
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
    viewport.setColour(juce::Viewport::backgroundColourId, backgroundColour);
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

void PPGUMultiWindowWorkspace::handleClose(int id)
{
    closePanel(id);
}

void PPGUMultiWindowWorkspace::layoutPanels()
{
    if (tileSurface == nullptr)
        return;

    const auto count = static_cast<int>(windows.size());
    if (count == 0)
    {
        tileSurface->setSize(1, 1);
        return;
    }

    const auto view = viewport.getLocalBounds();
    const ppgu::ui::PanelRect area { 0, 0,
        juce::jmax(1, view.getWidth()), juce::jmax(1, view.getHeight()) };

    std::vector<ppgu::ui::PanelRequest> requests;
    requests.reserve(windows.size());
    for (const auto& window : windows)
    {
        requests.push_back({ window->getPanelId(),
            window->getDesiredSize(), window->getMinimumSize() });
    }

    auto layout = ppgu::ui::calculatePanelLayout(area, requests, panelScale, panelGap);
    if (!layout.overflow && !layout.invalidInput)
    {
        tileSurface->setSize(area.width, area.height);
        for (const auto& placement : layout.panels)
        {
            const auto it = std::find_if(windows.begin(), windows.end(),
                [&placement](const auto& item) { return item->getPanelId() == placement.id; });
            if (it != windows.end())
                (*it)->setPanelBounds({ placement.bounds.x, placement.bounds.y,
                                       placement.bounds.width, placement.bounds.height });
        }
        return;
    }

    // When the viewport cannot fit every minimum-size panel, keep every panel
    // alive in a scrollable grid instead of hiding all but the first panel.
    int minWidth = 1, minHeight = 1;
    for (const auto& window : windows)
    {
        minWidth = juce::jmax(minWidth, static_cast<int>(std::ceil(window->getMinimumSize().width * panelScale)));
        minHeight = juce::jmax(minHeight, static_cast<int>(std::ceil(window->getMinimumSize().height * panelScale)));
    }

    const int gap = static_cast<int>(std::ceil(panelGap * panelScale));
    const int columns = juce::jmax(1, (area.width + gap) / (minWidth + gap));
    const int rows = (count + columns - 1) / columns;
    const int surfaceWidth = juce::jmax(area.width, columns * minWidth + (columns - 1) * gap);
    const int surfaceHeight = juce::jmax(area.height, rows * minHeight + (rows - 1) * gap);
    tileSurface->setSize(surfaceWidth, surfaceHeight);

    for (int i = 0; i < count; ++i)
    {
        const int column = i % columns;
        const int row = i / columns;
        const int x = column * (minWidth + gap);
        const int y = row * (minHeight + gap);
        const int width = column == columns - 1 ? surfaceWidth - x : minWidth;
        const int height = row == rows - 1 ? surfaceHeight - y : minHeight;
        windows[static_cast<std::size_t>(i)]->setPanelBounds({ x, y, width, height });
    }
}
