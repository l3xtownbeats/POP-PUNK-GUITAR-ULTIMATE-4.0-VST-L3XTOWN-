#pragma once

#include <JuceHeader.h>
#include "ThemeManager.h"
#include "PPGUMultiWindowWorkspace.h"

#include <functional>
#include <vector>

// Production gallery component extracted from PluginEditor.cpp.
// Selection, preview and application remain separate actions; this component
// owns visual state only and never accesses the processor or audio engine.
class PPGUSkinGalleryPanel final : public juce::Component, public PPGUThemeAwarePanel
{
public:
    struct Skin
    {
        int index = 0;
        juce::String id, name, family, familyKey, layout;
        juce::File preview;
        juce::Colour accent { 0xffd5ff45 };
        bool active = false;
    };

    PPGUSkinGalleryPanel(std::vector<Skin> skinsIn,
                         PPGUThemeManager::Palette paletteIn,
                         std::function<void(int)> applyIn,
                         std::function<void()> backIn)
        : skins(std::move(skinsIn)), palette(std::move(paletteIn)),
          apply(std::move(applyIn)), back(std::move(backIn))
    {
        for (const auto& skin : skins)
            if (skin.active) { selectedIndex = skin.index; break; }
        if (selectedIndex == 0 && !skins.empty())
            selectedIndex = skins.front().index;
        heading.setText("VISUAL SKIN GALLERY", juce::dontSendNotification);
        heading.setFont(juce::Font(17.0f, juce::Font::bold));
        heading.setColour(juce::Label::textColourId, palette.token("text_primary"));
        addAndMakeVisible(heading);

        subtitle.setText("Browse the complete PPGU collection. Select a card to apply its visual system.",
                         juce::dontSendNotification);
        subtitle.setFont(juce::Font(10.0f));
        subtitle.setColour(juce::Label::textColourId, palette.token("text_secondary"));
        addAndMakeVisible(subtitle);

        search.setTextToShowWhenEmpty("SEARCH THEMES...", palette.token("text_secondary"));
        search.setColour(juce::TextEditor::backgroundColourId, palette.token("control_bg"));
        search.setColour(juce::TextEditor::textColourId, palette.token("text_primary"));
        search.setColour(juce::TextEditor::outlineColourId, palette.token("control_border"));
        search.setColour(juce::TextEditor::focusedOutlineColourId, palette.token("accent"));
        search.onTextChange = [this] { rebuildCards(); };
        addAndMakeVisible(search);

        familyFilter.addItem("ALL FAMILIES", 1);
        familyFilter.addItem("CARBON STUDIO", 2);
        familyFilter.addItem("NEON BASEMENT", 3);
        familyFilter.addItem("CHROME PARADE", 4);
        familyFilter.addItem("WOOD SUMMER", 5);
        familyFilter.addItem("VINTAGE TAPE", 6);
        familyFilter.addItem("SUNSET SUBURB", 7);
        familyFilter.addItem("SPECIAL", 8);
        familyFilter.addItem("LEGACY THEMES", 9);
        familyFilter.setSelectedId(1, juce::dontSendNotification);
        familyFilter.setColour(juce::ComboBox::backgroundColourId, palette.token("control_bg"));
        familyFilter.setColour(juce::ComboBox::textColourId, palette.token("text_primary"));
        familyFilter.setColour(juce::ComboBox::outlineColourId, palette.token("control_border"));
        familyFilter.onChange = [this] { rebuildCards(); };
        addAndMakeVisible(familyFilter);

        countLabel.setColour(juce::Label::textColourId, palette.token("accent"));
        countLabel.setFont(juce::Font(10.5f, juce::Font::bold));
        countLabel.setJustificationType(juce::Justification::centredRight);
        addAndMakeVisible(countLabel);

        viewport.setViewedComponent(&grid, false);
        viewport.setOpaque(false);
        viewport.setScrollBarsShown(true, false);
        viewport.setScrollBarThickness(10);
        addAndMakeVisible(viewport);

        const auto styleAction = [this](juce::TextButton& button, bool primary)
        {
            button.setColour(juce::TextButton::buttonColourId,
                             primary ? palette.token("accent") : palette.token("panel_elevated"));
            button.setColour(juce::TextButton::textColourOffId,
                             primary ? palette.token("window_bg") : palette.token("text_primary"));
            button.setColour(juce::TextButton::buttonOnColourId, palette.token("accent_glow"));
            button.setColour(juce::TextButton::textColourOnId, palette.token("window_bg"));
        };
        styleAction(previewButton, false);
        styleAction(applyButton, true);
        styleAction(backButton, false);
        previewButton.onClick = [this]
        {
            if (findSelectedSkin() != nullptr)
            {
                showingPreview = true;
                resized();
                repaint();
            }
        };
        applyButton.onClick = [this]
        {
            if (selectedIndex > 0 && apply)
                apply(selectedIndex);
        };
        backButton.onClick = [this]
        {
            if (showingPreview)
            {
                showingPreview = false;
                resized();
                repaint();
            }
            else if (back)
                back();
        };
        addAndMakeVisible(previewButton);
        addAndMakeVisible(applyButton);
        addAndMakeVisible(backButton);

        previewTitle.setFont(juce::Font(16.0f, juce::Font::bold));
        previewTitle.setColour(juce::Label::textColourId, palette.token("text_primary"));
        addChildComponent(previewTitle);
        previewDetails.setFont(juce::Font(10.0f));
        previewDetails.setColour(juce::Label::textColourId, palette.token("text_secondary"));
        addChildComponent(previewDetails);
        updateSelectionControls();
        rebuildCards();
    }

    ~PPGUSkinGalleryPanel() override
    {
        viewport.setViewedComponent(nullptr, false);
    }

    void paint(juce::Graphics& g) override
    {
        if (!showingPreview)
            return;
        auto area = previewBounds.toFloat();
        g.setColour(palette.token("panel"));
        g.fillRoundedRectangle(area, 12.0f);
        g.setColour(palette.token("control_border"));
        g.drawRoundedRectangle(area, 12.0f, 1.2f);
        if (const auto* selected = findSelectedSkin())
        {
            const auto image = selected->preview.existsAsFile()
                ? juce::ImageCache::getFromFile(selected->preview) : juce::Image{};
            if (image.isValid())
                g.drawImageWithin(image, previewBounds.getX() + 12, previewBounds.getY() + 12,
                                  previewBounds.getWidth() - 24, previewBounds.getHeight() - 24,
                                  juce::RectanglePlacement::centred);
            else
            {
                auto art = area.reduced(12.0f);
                g.setGradientFill(juce::ColourGradient(palette.token("panel_elevated"),
                    art.getX(), art.getY(), selected->accent.withAlpha(0.72f),
                    art.getRight(), art.getBottom(), false));
                g.fillRoundedRectangle(art, 8.0f);
                g.setColour(palette.token("text_primary"));
                g.setFont(juce::Font(18.0f, juce::Font::bold));
                g.drawText(selected->name, art, juce::Justification::centred, true);
            }
        }
    }

    void setThemePalette(const PPGUThemeManager::Palette& value) override
    {
        palette = value;
        for (auto& skin : skins)
            skin.active = skin.id == palette.id;
        heading.setColour(juce::Label::textColourId, palette.token("text_primary"));
        subtitle.setColour(juce::Label::textColourId, palette.token("text_secondary"));
        search.setColour(juce::TextEditor::backgroundColourId, palette.token("control_bg"));
        search.setColour(juce::TextEditor::textColourId, palette.token("text_primary"));
        search.setColour(juce::TextEditor::outlineColourId, palette.token("control_border"));
        search.setColour(juce::TextEditor::focusedOutlineColourId, palette.token("accent"));
        search.setTextToShowWhenEmpty("SEARCH THEMES...", palette.token("text_secondary"));
        familyFilter.setColour(juce::ComboBox::backgroundColourId, palette.token("control_bg"));
        familyFilter.setColour(juce::ComboBox::textColourId, palette.token("text_primary"));
        familyFilter.setColour(juce::ComboBox::outlineColourId, palette.token("control_border"));
        countLabel.setColour(juce::Label::textColourId, palette.token("accent"));
        previewTitle.setColour(juce::Label::textColourId, palette.token("text_primary"));
        previewDetails.setColour(juce::Label::textColourId, palette.token("text_secondary"));
        const auto styleAction = [this](juce::TextButton& button, bool primary)
        {
            button.setColour(juce::TextButton::buttonColourId,
                             primary ? palette.token("accent") : palette.token("panel_elevated"));
            button.setColour(juce::TextButton::textColourOffId,
                             primary ? palette.token("window_bg") : palette.token("text_primary"));
            button.setColour(juce::TextButton::buttonOnColourId, palette.token("accent_glow"));
            button.setColour(juce::TextButton::textColourOnId, palette.token("window_bg"));
        };
        styleAction(previewButton, false);
        styleAction(applyButton, true);
        styleAction(backButton, false);
        updateSelectionControls();
        rebuildCards();
        repaint();
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(16);
        heading.setBounds(area.removeFromTop(24));
        subtitle.setBounds(area.removeFromTop(18));
        area.removeFromTop(4);
        if (showingPreview)
        {
            search.setVisible(false);
            familyFilter.setVisible(false);
            countLabel.setVisible(false);
            viewport.setVisible(false);
            previewButton.setVisible(false);
            previewTitle.setVisible(true);
            previewDetails.setVisible(true);
            previewTitle.setBounds(area.removeFromTop(24));
            previewDetails.setBounds(area.removeFromTop(18));
            area.removeFromTop(8);
            auto actions = area.removeFromBottom(38);
            backButton.setButtonText("BACK");
            backButton.setVisible(true);
            backButton.setBounds(actions.removeFromLeft(112).reduced(1, 2));
            actions.removeFromLeft(8);
            applyButton.setVisible(true);
            applyButton.setBounds(actions.removeFromRight(132).reduced(1, 2));
            previewBounds = area.reduced(2);
            updateSelectionControls();
            return;
        }
        search.setVisible(true);
        familyFilter.setVisible(true);
        countLabel.setVisible(true);
        viewport.setVisible(true);
        previewTitle.setVisible(false);
        previewDetails.setVisible(false);
        auto filters = area.removeFromTop(32);
        search.setBounds(filters.removeFromLeft(juce::jmin(300, filters.getWidth() / 2)).reduced(1, 2));
        filters.removeFromLeft(8);
        familyFilter.setBounds(filters.removeFromLeft(190).reduced(1, 2));
        countLabel.setBounds(filters);
        area.removeFromTop(4);
        auto actions = area.removeFromBottom(38);
        previewButton.setVisible(true);
        previewButton.setBounds(actions.removeFromLeft(112).reduced(1, 2));
        actions.removeFromLeft(8);
        applyButton.setVisible(true);
        applyButton.setBounds(actions.removeFromLeft(112).reduced(1, 2));
        actions.removeFromLeft(8);
        backButton.setButtonText("CLOSE");
        backButton.setVisible(true);
        backButton.setBounds(actions.removeFromRight(112).reduced(1, 2));
        viewport.setBounds(area);
        rebuildCards();
        updateSelectionControls();
    }

private:
    class Card final : public juce::Component
    {
    public:
        Card(Skin itemIn, PPGUThemeManager::Palette paletteIn,
             std::function<void(int)> selectIn, bool selectedIn)
            : item(std::move(itemIn)), palette(std::move(paletteIn)),
              select(std::move(selectIn)), selected(selectedIn)
        {
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
        }

        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat().reduced(1.0f);
            g.setColour(hovered ? palette.token("panel_elevated") : palette.token("panel"));
            g.fillRoundedRectangle(bounds, 9.0f);
            g.setColour((selected || item.active) ? item.accent : palette.token("control_border"));
            g.drawRoundedRectangle(bounds, 9.0f, selected ? 2.4f : (item.active ? 1.8f : 1.0f));

            auto previewArea = bounds.reduced(8.0f, 8.0f).removeFromTop(72.0f);
            if (item.preview.existsAsFile())
            {
                const auto image = juce::ImageCache::getFromFile(item.preview);
                if (image.isValid())
                    g.drawImageWithin(image, (int)previewArea.getX(), (int)previewArea.getY(),
                                      (int)previewArea.getWidth(), (int)previewArea.getHeight(),
                                      juce::RectanglePlacement::centred);
                else
                    paintFallback(g, previewArea);
            }
            else
                paintFallback(g, previewArea);

            auto textArea = bounds.reduced(10.0f, 8.0f);
            textArea.removeFromTop(78);
            g.setColour(palette.token("text_primary"));
            g.setFont(juce::Font(12.5f, juce::Font::bold));
            g.drawText(item.name, textArea.removeFromTop(18.0f), juce::Justification::centredLeft, true);
            g.setColour(palette.token("text_secondary"));
            g.setFont(juce::Font(9.0f));
            g.drawText(item.family + "  /  " + item.layout,
                       textArea.removeFromTop(14.0f), juce::Justification::centredLeft, true);

            if (item.active || selected)
            {
                const auto badge = item.active ? juce::String("ACTIVE") : juce::String("SELECTED");
                const int badgeWidth = item.active ? 48 : 58;
                g.setColour(item.accent);
                g.fillRoundedRectangle(getWidth() - (float) badgeWidth - 10.0f, 10.0f,
                                        (float) badgeWidth, 17.0f, 5.0f);
                g.setColour(palette.token("window_bg"));
                g.setFont(juce::Font(8.0f, juce::Font::bold));
                g.drawText(badge, getWidth() - badgeWidth - 10, 10,
                           badgeWidth, 17, juce::Justification::centred);
            }
        }

        void setPalette(const PPGUThemeManager::Palette& value) { palette = value; repaint(); }

        void mouseEnter(const juce::MouseEvent&) override { hovered = true; repaint(); }
        void mouseExit(const juce::MouseEvent&) override { hovered = false; repaint(); }
        void mouseUp(const juce::MouseEvent& event) override
        {
            if (event.mouseWasClicked() && select)
                select(item.index);
        }

    private:
        void paintFallback(juce::Graphics& g, juce::Rectangle<float> area)
        {
            g.setGradientFill(juce::ColourGradient(palette.token("panel_elevated"),
                area.getX(), area.getY(), item.accent.withAlpha(0.75f),
                area.getRight(), area.getBottom(), false));
            g.fillRoundedRectangle(area, 6.0f);
            g.setColour(palette.token("text_primary").withAlpha(0.72f));
            g.setFont(juce::Font(15.0f, juce::Font::bold));
            g.drawText("PPGU  /  " + item.family, area, juce::Justification::centred, true);
        }

        Skin item;
        PPGUThemeManager::Palette palette;
        std::function<void(int)> select;
        bool selected = false;
        bool hovered = false;
    };

    class Grid final : public juce::Component
    {
    public:
        void setPalette(const PPGUThemeManager::Palette& value)
        {
            for (auto& card : cards)
                card->setPalette(value);
        }
        void clearCards() { cards.clear(); }
        void addCard(std::unique_ptr<Card> card)
        {
            addAndMakeVisible(*card);
            cards.push_back(std::move(card));
        }
        int getCardCount() const noexcept { return static_cast<int>(cards.size()); }

        void resized() override
        {
            constexpr int gap = 12;
            constexpr int minCardWidth = 205;
            constexpr int cardHeight = 136;
            const int columns = juce::jmax(1, (getWidth() - gap) / (minCardWidth + gap));
            const int cardWidth = juce::jmax(160, (getWidth() - gap * (columns + 1)) / columns);
            for (int i = 0; i < static_cast<int>(cards.size()); ++i)
            {
                const int column = i % columns;
                const int row = i / columns;
                cards[static_cast<size_t>(i)]->setBounds(
                    gap + column * (cardWidth + gap), gap + row * (cardHeight + gap),
                    cardWidth, cardHeight);
            }
        }

    private:
        std::vector<std::unique_ptr<Card>> cards;
    };

    const Skin* findSelectedSkin() const
    {
        const auto it = std::find_if(skins.begin(), skins.end(),
            [this](const Skin& item) { return item.index == selectedIndex; });
        return it == skins.end() ? nullptr : &(*it);
    }

    void updateSelectionControls()
    {
        const auto* selected = findSelectedSkin();
        const bool valid = selected != nullptr;
        previewButton.setEnabled(valid);
        applyButton.setEnabled(valid);
        if (valid)
        {
            previewTitle.setText(selected->name, juce::dontSendNotification);
            previewDetails.setText(selected->family + "  /  " + selected->layout
                + "  /  " + selected->id, juce::dontSendNotification);
        }
    }

    void selectSkin(int index)
    {
        selectedIndex = index;
        updateSelectionControls();
        rebuildCards();
    }

    void rebuildCards()
    {
        grid.clearCards();
        const auto query = search.getText().trim().toLowerCase();
        const int selectedFamily = familyFilter.getSelectedId();
        const juce::StringArray families {
            "", "carbon_studio", "neon_basement", "chrome_parade", "wood_summer",
            "vintage_tape", "sunset_suburb", "special", "legacy"
        };
        const auto familyKey = selectedFamily > 1 && selectedFamily <= families.size()
            ? families[selectedFamily - 1] : juce::String{};

        for (const auto& item : skins)
        {
            if (familyKey.isNotEmpty() && item.familyKey != familyKey)
                continue;
            const auto haystack = (item.name + " " + item.family + " " + item.layout).toLowerCase();
            if (query.isNotEmpty() && !haystack.contains(query))
                continue;
            grid.addCard(std::make_unique<Card>(item, palette,
                [this](int index) { selectSkin(index); }, item.index == selectedIndex));
        }

        countLabel.setText(juce::String(grid.getCardCount()) + " / "
                           + juce::String(skins.size()) + " THEMES", juce::dontSendNotification);
        const int gap = 12;
        const int minCardWidth = 205;
        const int columns = juce::jmax(1, (viewport.getWidth() - gap) / (minCardWidth + gap));
        const int rows = (grid.getCardCount() + columns - 1) / columns;
        const int cardHeight = 136;
        grid.setSize(juce::jmax(1, viewport.getWidth()),
                     juce::jmax(viewport.getHeight(), gap + rows * (cardHeight + gap)));
        grid.resized();
    }

    std::vector<Skin> skins;
    PPGUThemeManager::Palette palette;
    std::function<void(int)> apply;
    std::function<void()> back;
    juce::Label heading, subtitle, countLabel, previewTitle, previewDetails;
    juce::TextEditor search;
    juce::ComboBox familyFilter;
    juce::TextButton previewButton { "PREVIEW" }, applyButton { "APPLY" }, backButton { "CLOSE" };
    juce::Rectangle<int> previewBounds;
    int selectedIndex = 0;
    bool showingPreview = false;
    Grid grid;
    juce::Viewport viewport;
};
