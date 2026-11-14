#include "PluginEditor.h"
#include "PPGUData.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <set>
#include <limits>
#include <vector>
#if JUCE_WINDOWS
 #ifndef NOMINMAX
  #define NOMINMAX
 #endif
 #include <windows.h>
juce::String fromWidePath(const wchar_t* value, int length)
{
    if (value == nullptr || length <= 0)
        return {};
    const int required = WideCharToMultiByte(CP_UTF8, 0, value, length, nullptr, 0, nullptr, nullptr);
    if (required <= 0)
        return {};
    std::string utf8(static_cast<size_t>(required), '\0');
    const int written = WideCharToMultiByte(CP_UTF8, 0, value, length,
                                             utf8.data(), required, nullptr, nullptr);
    return written == required ? juce::String::fromUTF8(utf8.data(), written) : juce::String();
}
#endif

// Scrollable visual browser for the complete registry. Preview images are loaded
// lazily by visible cards so opening the gallery does not decode all 102 PNGs.
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
                         std::function<void(int)> applyIn)
        : skins(std::move(skinsIn)), palette(std::move(paletteIn)), apply(std::move(applyIn))
    {
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
        rebuildCards();
    }

    ~PPGUSkinGalleryPanel() override
    {
        viewport.setViewedComponent(nullptr, false);
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
        rebuildCards();
        repaint();
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(16);
        heading.setBounds(area.removeFromTop(24));
        subtitle.setBounds(area.removeFromTop(18));
        area.removeFromTop(4);
        auto filters = area.removeFromTop(32);
        search.setBounds(filters.removeFromLeft(juce::jmin(300, filters.getWidth() / 2)).reduced(1, 2));
        filters.removeFromLeft(8);
        familyFilter.setBounds(filters.removeFromLeft(190).reduced(1, 2));
        countLabel.setBounds(filters);
        area.removeFromTop(4);
        viewport.setBounds(area);
        rebuildCards();
    }

private:
    class Card final : public juce::Component
    {
    public:
        Card(Skin itemIn, PPGUThemeManager::Palette paletteIn,
             std::function<void(int)> applyIn)
            : item(std::move(itemIn)), palette(std::move(paletteIn)), apply(std::move(applyIn))
        {
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
        }

        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat().reduced(1.0f);
            g.setColour(hovered ? palette.token("panel_elevated") : palette.token("panel"));
            g.fillRoundedRectangle(bounds, 9.0f);
            g.setColour(item.active ? item.accent : palette.token("control_border"));
            g.drawRoundedRectangle(bounds, 9.0f, item.active ? 1.8f : 1.0f);

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

            if (item.active)
            {
                g.setColour(item.accent);
                g.fillRoundedRectangle(getWidth() - 58.0f, 10.0f, 48.0f, 17.0f, 5.0f);
                g.setColour(palette.token("window_bg"));
                g.setFont(juce::Font(8.0f, juce::Font::bold));
                g.drawText("ACTIVE", getWidth() - 58, 10, 48, 17, juce::Justification::centred);
            }
            else
            {
                g.setColour(palette.token("accent"));
                g.fillRoundedRectangle(getWidth() - 54.0f, 10.0f, 44.0f, 17.0f, 5.0f);
                g.setColour(palette.token("window_bg"));
                g.setFont(juce::Font(8.0f, juce::Font::bold));
                g.drawText("APPLY", getWidth() - 54, 10, 44, 17, juce::Justification::centred);
            }
        }

        void setPalette(const PPGUThemeManager::Palette& value) { palette = value; repaint(); }

        void mouseEnter(const juce::MouseEvent&) override { hovered = true; repaint(); }
        void mouseExit(const juce::MouseEvent&) override { hovered = false; repaint(); }
        void mouseUp(const juce::MouseEvent& event) override
        {
            if (event.mouseWasClicked() && apply)
                apply(item.index);
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
        std::function<void(int)> apply;
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
            grid.addCard(std::make_unique<Card>(item, palette, apply));
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
    juce::Label heading, subtitle, countLabel;
    juce::TextEditor search;
    juce::ComboBox familyFilter;
    Grid grid;
    juce::Viewport viewport;
};

// UI-only settings panel: controls the auxiliary workspace and theme preference.
class PPGUSettingsPanel final : public juce::Component, public PPGUThemeAwarePanel
{
public:
    PPGUSettingsPanel(PPGUThemeManager::Palette paletteIn, float panelScaleIn, int panelGapIn,
                      std::function<void(float)> scaleChangedIn,
                      std::function<void(int)> gapChangedIn,
                      std::function<void()> restoreThemeIn,
                      std::function<void()> openGalleryIn)
        : palette(std::move(paletteIn)), scaleChanged(std::move(scaleChangedIn)),
          gapChanged(std::move(gapChangedIn)), restoreTheme(std::move(restoreThemeIn)),
          openGallery(std::move(openGalleryIn))
    {
        heading.setText("WORKSPACE SETTINGS", juce::dontSendNotification);
        heading.setFont(juce::Font(16.0f, juce::Font::bold));
        heading.setColour(juce::Label::textColourId, palette.token("text_primary"));
        addAndMakeVisible(heading);

        description.setText("These options affect auxiliary panels and visual preferences only. "
                             "Audio processing, MIDI and instrument parameters are not changed.",
                             juce::dontSendNotification);
        description.setColour(juce::Label::textColourId, palette.token("text_secondary"));
        description.setFont(juce::Font(10.0f));
        description.setJustificationType(juce::Justification::topLeft);
        addAndMakeVisible(description);

        scaleLabel.setText("AUXILIARY PANEL SCALE", juce::dontSendNotification);
        gapLabel.setText("SPACE BETWEEN PANELS", juce::dontSendNotification);
        for (auto* label : { &scaleLabel, &gapLabel })
        {
            label->setFont(juce::Font(9.5f, juce::Font::bold));
            label->setColour(juce::Label::textColourId, palette.token("accent"));
            addAndMakeVisible(*label);
        }

        configureSlider(scaleSlider, 0.5, 1.5, 0.05, panelScaleIn);
        scaleSlider.setTextValueSuffix(" x");
        scaleSlider.onValueChange = [this]
        {
            if (scaleChanged)
                scaleChanged(static_cast<float>(scaleSlider.getValue()));
        };
        addAndMakeVisible(scaleSlider);

        configureSlider(gapSlider, 0.0, 32.0, 1.0, panelGapIn);
        gapSlider.setTextValueSuffix(" px");
        gapSlider.onValueChange = [this]
        {
            if (gapChanged)
                gapChanged(static_cast<int>(std::lround(gapSlider.getValue())));
        };
        addAndMakeVisible(gapSlider);

        restoreButton.setButtonText("RESTORE SIGNAL FORGE DEFAULT");
        restoreButton.setColour(juce::TextButton::buttonColourId, palette.token("panel_elevated"));
        restoreButton.setColour(juce::TextButton::textColourOffId, palette.token("text_primary"));
        restoreButton.onClick = [this] { if (restoreTheme) restoreTheme(); };
        addAndMakeVisible(restoreButton);

        galleryButton.setButtonText("OPEN SKIN GALLERY");
        galleryButton.setColour(juce::TextButton::buttonColourId, palette.token("accent"));
        galleryButton.setColour(juce::TextButton::textColourOffId, palette.token("window_bg"));
        galleryButton.onClick = [this] { if (openGallery) openGallery(); };
        addAndMakeVisible(galleryButton);
    }

    void setThemePalette(const PPGUThemeManager::Palette& value) override
    {
        palette = value;
        heading.setColour(juce::Label::textColourId, palette.token("text_primary"));
        description.setColour(juce::Label::textColourId, palette.token("text_secondary"));
        scaleLabel.setColour(juce::Label::textColourId, palette.token("accent"));
        gapLabel.setColour(juce::Label::textColourId, palette.token("accent"));
        configureSlider(scaleSlider, 0.5, 1.5, 0.05, scaleSlider.getValue());
        configureSlider(gapSlider, 0.0, 32.0, 1.0, gapSlider.getValue());
        restoreButton.setColour(juce::TextButton::buttonColourId, palette.token("panel_elevated"));
        restoreButton.setColour(juce::TextButton::textColourOffId, palette.token("text_primary"));
        galleryButton.setColour(juce::TextButton::buttonColourId, palette.token("accent"));
        galleryButton.setColour(juce::TextButton::textColourOffId, palette.token("window_bg"));
        repaint();
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(12);
        heading.setBounds(area.removeFromTop(20));
        description.setBounds(area.removeFromTop(24));
        area.removeFromTop(2);
        scaleLabel.setBounds(area.removeFromTop(14));
        scaleSlider.setBounds(area.removeFromTop(20));
        area.removeFromTop(2);
        gapLabel.setBounds(area.removeFromTop(14));
        gapSlider.setBounds(area.removeFromTop(20));
        area.removeFromTop(2);
        restoreButton.setBounds(area.removeFromTop(22).removeFromLeft(270));
        area.removeFromTop(2);
        galleryButton.setBounds(area.removeFromTop(22).removeFromLeft(220));
    }

private:
    void configureSlider(juce::Slider& slider, double minimum, double maximum,
                         double interval, double value)
    {
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 52, 18);
        slider.setRange(minimum, maximum, interval);
        slider.setValue(juce::jlimit(minimum, maximum, value), juce::dontSendNotification);
        slider.setColour(juce::Slider::trackColourId, palette.token("control_border"));
        slider.setColour(juce::Slider::backgroundColourId, palette.token("control_bg"));
        slider.setColour(juce::Slider::thumbColourId, palette.token("accent"));
        slider.setColour(juce::Slider::textBoxTextColourId, palette.token("text_primary"));
        slider.setColour(juce::Slider::textBoxBackgroundColourId, palette.token("control_bg"));
        slider.setColour(juce::Slider::textBoxOutlineColourId, palette.token("control_border"));
    }

    PPGUThemeManager::Palette palette;
    std::function<void(float)> scaleChanged;
    std::function<void(int)> gapChanged;
    std::function<void()> restoreTheme, openGallery;
    juce::Label heading, description, scaleLabel, gapLabel;
    juce::Slider scaleSlider, gapSlider;
    juce::TextButton restoreButton, galleryButton;
};

// Import panel delegates to the existing asynchronous SFZ loader and reports its real result.
class PPGUImportGuitarPanel final : public juce::Component, public PPGUThemeAwarePanel
{
public:
    explicit PPGUImportGuitarPanel(std::function<void(juce::Label*, juce::Button*)> loadActionIn)
        : loadAction(std::move(loadActionIn))
    {
        heading.setText("LICENSED GUITAR LIBRARY", juce::dontSendNotification);
        heading.setFont(juce::Font(18.0f, juce::Font::bold));
        heading.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(heading);
        description.setText("Load an SFZ guitar instrument and its referenced WAV samples. "
                            "The existing background loader validates and registers the bank.",
                            juce::dontSendNotification);
        description.setJustificationType(juce::Justification::topLeft);
        description.setColour(juce::Label::textColourId, juce::Colour(0xffb5c4cb));
        addAndMakeVisible(description);
        status.setText("No SFZ library loaded in this session.", juce::dontSendNotification);
        status.setJustificationType(juce::Justification::topLeft);
        status.setColour(juce::Label::textColourId, juce::Colour(0xffd7ffb2));
        addAndMakeVisible(status);
        loadButton.setButtonText("LOAD SFZ");
        loadButton.onClick = [this] { if (loadAction) loadAction(&status, &loadButton); };
        addAndMakeVisible(loadButton);
    }
    void setThemePalette(const PPGUThemeManager::Palette& value) override
    {
        palette = value;
        heading.setColour(juce::Label::textColourId, palette.token("text_primary"));
        description.setColour(juce::Label::textColourId, palette.token("text_secondary"));
        status.setColour(juce::Label::textColourId, palette.token("lcd_text"));
        loadButton.setColour(juce::TextButton::buttonColourId, palette.token("accent"));
        loadButton.setColour(juce::TextButton::textColourOffId, palette.token("window_bg"));
        repaint();
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(18);
        heading.setBounds(area.removeFromTop(34));
        area.removeFromTop(12);
        description.setBounds(area.removeFromTop(64));
        area.removeFromTop(18);
        loadButton.setBounds(area.removeFromTop(42).removeFromLeft(180));
        area.removeFromTop(16);
        status.setBounds(area.removeFromTop(90));
    }
private:
    PPGUThemeManager::Palette palette;
    std::function<void(juce::Label*, juce::Button*)> loadAction;
    juce::Label heading, description, status;
    juce::TextButton loadButton;
};

// Compact live harmony monitor. It observes the existing MIDI keyboard state and
// scale parameters only; it never writes to APVTS or changes processor behaviour.
class PPGUPerformanceDisplay final : public juce::Component, private juce::Timer
{
public:
    explicit PPGUPerformanceDisplay(PopPunkGuitarUltimateAudioProcessor& processorIn)
        : processor(processorIn), parameters(processorIn.parameters)
    {
        setOpaque(false);
        refreshState();
        startTimerHz(20);
    }

    void setPalette(const PPGUThemeManager::Palette& value)
    {
        palette = value;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.setGradientFill(juce::ColourGradient(palette.tokens.panelElevated,
                                               bounds.getX(), bounds.getY(),
                                               palette.tokens.panel,
                                               bounds.getX(), bounds.getBottom(), false));
        g.fillRoundedRectangle(bounds, 9.0f);
        g.setColour(palette.tokens.controlBorder.withAlpha(0.9f));
        g.drawRoundedRectangle(bounds.reduced(0.5f), 9.0f, 1.0f);
        g.setColour(palette.tokens.accent);
        g.fillRoundedRectangle(bounds.getX() + 1.0f, bounds.getY() + 8.0f, 3.0f,
                               juce::jmax(8.0f, bounds.getHeight() - 16.0f), 1.5f);

        auto area = getLocalBounds().reduced(14, 5);
        auto harmony = area.removeFromLeft(190);
        g.setColour(palette.tokens.textSecondary);
        g.setFont(juce::Font(8.5f, juce::Font::bold));
        g.drawText("LIVE HARMONY", harmony.removeFromTop(13), juce::Justification::left);
        g.setColour(palette.tokens.accent);
        g.setFont(juce::Font(23.0f, juce::Font::bold));
        g.drawFittedText(chordText, harmony, juce::Justification::centredLeft, 1);

        area.removeFromLeft(10);
        auto keyArea = area.removeFromRight(148);
        g.setColour(palette.tokens.textSecondary);
        g.setFont(juce::Font(8.5f, juce::Font::bold));
        g.drawText("SCALE / KEY", keyArea.removeFromTop(13), juce::Justification::left);
        g.setColour(palette.tokens.textPrimary);
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        g.drawFittedText(keyText, keyArea, juce::Justification::centredLeft, 1);

        area.removeFromRight(8);
        g.setColour(palette.tokens.textSecondary);
        g.setFont(juce::Font(8.5f, juce::Font::bold));
        g.drawText("NOTES HELD", area.removeFromTop(13), juce::Justification::left);
        g.setColour(palette.tokens.textPrimary);
        g.setFont(juce::Font(12.0f));
        g.drawFittedText(notesText, area, juce::Justification::centredLeft, 1);
    }

private:
    struct ChordShape
    {
        const char* suffix;
        std::vector<int> intervals;
        int priority;
    };

    static juce::String pitchName(int pitchClass)
    {
        static const char* names[] = {
            "C", "C#", "D", "D#", "E", "F",
            "F#", "G", "G#", "A", "A#", "B"
        };
        return names[juce::jlimit(0, 11, pitchClass)];
    }

    static juce::String noteName(int midiNote)
    {
        return pitchName(midiNote % 12) + juce::String(midiNote / 12 - 1);
    }

    juce::String detectChord(const std::vector<int>& notes) const
    {
        if (notes.empty())
            return "-";

        if (notes.size() == 1)
            return noteName(notes.front());

        std::array<bool, 12> present {};
        int pitchClassCount = 0;
        for (const auto note : notes)
        {
            const int pc = note % 12;
            if (!present[static_cast<size_t>(pc)])
            {
                present[static_cast<size_t>(pc)] = true;
                ++pitchClassCount;
            }
        }

        static const std::vector<ChordShape> shapes {
            { "m",    { 0, 3, 7 },       12 },
            { "maj",  { 0, 4, 7 },       11 },
            { "m7",   { 0, 3, 7, 10 },  10 },
            { "7",    { 0, 4, 7, 10 },   9 },
            { "maj7", { 0, 4, 7, 11 },   9 },
            { "sus2", { 0, 2, 7 },       8 },
            { "sus4", { 0, 5, 7 },       8 },
            { "dim",  { 0, 3, 6 },       7 },
            { "aug",  { 0, 4, 8 },       7 },
            { "m7b5", { 0, 3, 6, 10 },   6 },
            { "dim7", { 0, 3, 6, 9 },    6 },
            { "6",    { 0, 4, 7, 9 },    5 },
            { "m6",   { 0, 3, 7, 9 },    5 },
            { "mMaj7",{ 0, 3, 7, 11 },  4 },
            { "5",    { 0, 7 },          2 }
        };

        const int bassPc = notes.front() % 12;
        int bestScore = std::numeric_limits<int>::min();
        juce::String best;

        for (int root = 0; root < 12; ++root)
        {
            for (const auto& shape : shapes)
            {
                bool complete = true;
                for (const auto interval : shape.intervals)
                    if (!present[static_cast<size_t>((root + interval) % 12)])
                    {
                        complete = false;
                        break;
                    }

                if (!complete)
                    continue;

                const int extras = pitchClassCount - static_cast<int>(shape.intervals.size());
                const int score = 100 - extras * 18 + shape.priority + (root == bassPc ? 4 : 0);
                if (score > bestScore)
                {
                    bestScore = score;
                    best = pitchName(root) + shape.suffix;
                }
            }
        }

        if (best.isNotEmpty())
            return best;

        return pitchName(bassPc) + "  /  " + juce::String(pitchClassCount) + " tones";
    }

    void refreshState()
    {
        std::vector<int> held;
        held.reserve(8);
        for (int note = 0; note < 128; ++note)
        {
            bool isHeld = false;
            isHeld = processor.isMidiNoteHeld(note);
            if (isHeld)
                held.push_back(note);
        }

        juce::StringArray noteNames;
        for (size_t i = 0; i < held.size() && i < 6; ++i)
            noteNames.add(noteName(held[i]));
        if (held.size() > 6)
            noteNames.add("+" + juce::String(static_cast<int>(held.size() - 6)));

        const auto newChord = detectChord(held);
        const auto newNotes = held.empty() ? juce::String("Waiting for MIDI input")
                                           : noteNames.joinIntoString("   /   ");

        auto* rootParameter = parameters.getRawParameterValue("scaleRoot");
        auto* modeParameter = parameters.getRawParameterValue("scaleMode");
        const int root = rootParameter != nullptr
            ? juce::jlimit(0, 11, static_cast<int>(rootParameter->load(std::memory_order_relaxed))) : 0;
        const int mode = modeParameter != nullptr
            ? juce::jlimit(0, 2, static_cast<int>(modeParameter->load(std::memory_order_relaxed))) : 0;
        const auto newKey = mode == 1 ? pitchName(root) + " MAJOR"
                         : mode == 2 ? pitchName(root) + " MINOR"
                                     : juce::String("CHROMATIC");

        if (newChord != chordText || newNotes != notesText || newKey != keyText)
        {
            chordText = newChord;
            notesText = newNotes;
            keyText = newKey;
            repaint();
        }
    }

    void timerCallback() override { refreshState(); }

    PopPunkGuitarUltimateAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& parameters;
    PPGUThemeManager::Palette palette;
    juce::String chordText { "—" };
    juce::String notesText { "Waiting for MIDI input" };
    juce::String keyText { "CHROMATIC" };
};


namespace
{
class PPGUFXEditPanel final : public juce::Component, public PPGUThemeAwarePanel
{
public:
    explicit PPGUFXEditPanel(juce::AudioProcessorValueTreeState& state, int initialEffect)
        : apvts(state)
    {
        groups = {
            { "COMPRESSOR", "compressorOn", { { "THRESHOLD", "compressorThreshold" }, { "RATIO", "compressorRatio" }, { "ATTACK", "compressorAttack" }, { "RELEASE", "compressorRelease" }, { "AMOUNT", "compressorAmount" } } },
            { "EQ", "eqOn", { { "LOW", "eqLow" }, { "MID", "eqMid" }, { "HIGH", "eqHigh" }, { "LOW FREQ", "eqLowFreq" }, { "MID FREQ", "eqMidFreq" }, { "MID Q", "eqMidQ" }, { "HIGH FREQ", "eqHighFreq" } } },
            { "DISTORTION", "distortionOn", { { "DRIVE", "distortionDrive" }, { "TONE", "distortionTone" }, { "MIX", "distortionMix" } } },
            { "CHORUS", "chorusOn", { { "RATE", "chorusRate" }, { "DEPTH", "chorusDepth" }, { "MIX", "chorusMix" } } },
            { "FLANGER", "flangerOn", { { "RATE", "flangerRate" }, { "DEPTH", "flangerDepth" }, { "FEEDBACK", "flangerFeedback" }, { "MIX", "flangerMix" } } },
            { "DELAY", "delayOn", { { "TIME", "delayTime" }, { "FEEDBACK", "delayFeedback" }, { "MIX", "delayMix" } } },
            { "REVERB", "reverbOn", { { "ROOM", "reverbRoomSize" }, { "DAMPING", "reverbDamping" }, { "MIX", "reverbMix" } } },
            { "CABINET IR", "cabinetOn", { { "MIX", "cabinetMix" }, { "LOW CUT", "cabinetLowCut" }, { "HIGH CUT", "cabinetHighCut" } } }
        };

        title.setText("SIGNAL FORGE  /  FX EDIT", juce::dontSendNotification);
        title.setFont(juce::Font(20.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(0xffb7f34a));
        addAndMakeVisible(title);

        for (size_t i = 0; i < groups.size(); ++i)
            effectSelector.addItem(groups[i].name, static_cast<int>(i + 1));
        effectSelector.setSelectedId(juce::jlimit(1, static_cast<int>(groups.size()), initialEffect + 1), juce::dontSendNotification);
        effectSelector.onChange = [this] { selectGroup(effectSelector.getSelectedId() - 1); };
        addAndMakeVisible(effectSelector);

        powerButton.setButtonText("POWER");
        powerButton.setClickingTogglesState(true);
        addAndMakeVisible(powerButton);
        graphCaption.setText("PROCESS RESPONSE  /  VISUAL GUIDE", juce::dontSendNotification);
        graphCaption.setColour(juce::Label::textColourId, juce::Colour(0xff91a5b0));
        addAndMakeVisible(graphCaption);
        status.setText("Controls are attached to the existing APVTS parameters.", juce::dontSendNotification);
        status.setColour(juce::Label::textColourId, juce::Colour(0xff91a5b0));
        addAndMakeVisible(status);

        for (size_t groupIndex = 0; groupIndex < groups.size(); ++groupIndex)
        {
            for (const auto& item : groups[groupIndex].parameters)
            {
                auto label = std::make_unique<juce::Label>();
                label->setText(item.first, juce::dontSendNotification);
                label->setColour(juce::Label::textColourId, juce::Colour(0xffb5c2ca));
                label->setFont(juce::Font(11.0f, juce::Font::bold));
                addAndMakeVisible(*label);

                auto slider = std::make_unique<juce::Slider>();
                slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
                slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
                slider->setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffb7f34a));
                slider->setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff43545e));
                slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xffe7f0f4));
                slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffe7f0f4));
                addAndMakeVisible(*slider);

                auto attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, item.second, *slider);
                controls.push_back({ static_cast<int>(groupIndex), std::move(label), std::move(slider), std::move(attachment) });
            }
        }
        selectGroup(effectSelector.getSelectedId() - 1);
    }

    void setThemePalette(const PPGUThemeManager::Palette& value) override
    {
        palette = value;
        title.setColour(juce::Label::textColourId, palette.token("text_primary"));
        graphCaption.setColour(juce::Label::textColourId, palette.token("text_secondary"));
        status.setColour(juce::Label::textColourId, palette.token("text_secondary"));
        effectSelector.setColour(juce::ComboBox::backgroundColourId, palette.token("control_bg"));
        effectSelector.setColour(juce::ComboBox::textColourId, palette.token("text_primary"));
        effectSelector.setColour(juce::ComboBox::outlineColourId, palette.token("control_border"));
        effectSelector.setColour(juce::ComboBox::arrowColourId, palette.token("accent"));
        powerButton.setColour(juce::TextButton::buttonColourId, palette.token("panel_elevated"));
        powerButton.setColour(juce::TextButton::buttonOnColourId, palette.token("accent"));
        powerButton.setColour(juce::TextButton::textColourOffId, palette.token("text_primary"));
        powerButton.setColour(juce::TextButton::textColourOnId, palette.token("window_bg"));
        for (auto& control : controls)
        {
            control.label->setColour(juce::Label::textColourId, palette.token("text_secondary"));
            control.slider->setColour(juce::Slider::rotarySliderFillColourId, palette.token("accent"));
            control.slider->setColour(juce::Slider::rotarySliderOutlineColourId, palette.token("control_border"));
            control.slider->setColour(juce::Slider::thumbColourId, palette.token("text_primary"));
            control.slider->setColour(juce::Slider::textBoxTextColourId, palette.token("text_primary"));
            control.slider->setColour(juce::Slider::textBoxBackgroundColourId, palette.token("control_bg"));
            control.slider->setColour(juce::Slider::textBoxOutlineColourId, palette.token("control_border"));
        }
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(palette.token("window_bg"));
        auto area = getLocalBounds().toFloat().reduced(12.0f);
        g.setColour(palette.token("control_border"));
        g.drawRoundedRectangle(area, 10.0f, 1.0f);
        if (!graphArea.isEmpty())
        {
            auto graph = graphArea.toFloat();
            g.setColour(palette.token("control_bg"));
            g.fillRoundedRectangle(graph, 8.0f);
            g.setColour(palette.token("control_border"));
            for (int i = 1; i < 5; ++i)
                g.drawHorizontalLine(graphArea.getY() + graphArea.getHeight() * i / 5, graphArea.getX(), graphArea.getRight());
            for (int i = 1; i < 9; ++i)
                g.drawVerticalLine(graphArea.getX() + graphArea.getWidth() * i / 9, graphArea.getY(), graphArea.getBottom());
            juce::Path curve;
            curve.startNewSubPath(static_cast<float>(graphArea.getX()), static_cast<float>(graphArea.getCentreY()));
            curve.cubicTo(graphArea.getX() + graphArea.getWidth() * 0.28f, graphArea.getY() + graphArea.getHeight() * 0.78f,
                          graphArea.getX() + graphArea.getWidth() * 0.62f, graphArea.getY() + graphArea.getHeight() * 0.22f,
                          static_cast<float>(graphArea.getRight()), static_cast<float>(graphArea.getCentreY()));
            g.setColour(palette.token("accent"));
            g.strokePath(curve, juce::PathStrokeType(2.0f));
        }
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(20);
        title.setBounds(area.removeFromTop(34));
        auto top = area.removeFromTop(44);
        effectSelector.setBounds(top.removeFromLeft(230).reduced(2, 4));
        powerButton.setBounds(top.removeFromLeft(110).reduced(2, 4));
        graphCaption.setBounds(top);
        const int graphHeight = getHeight() >= 600 ? 140 : (getHeight() >= 500 ? 90 : 0);
        graphCaption.setVisible(graphHeight > 0);
        graphArea = graphHeight > 0
            ? area.removeFromBottom(graphHeight).reduced(4, 8)
            : juce::Rectangle<int>{};
        status.setBounds(area.removeFromBottom(24));
        area.removeFromBottom(8);
        const int visibleCount = static_cast<int>(std::count_if(controls.begin(), controls.end(),
            [this](const Control& c) { return c.group == selectedGroup; }));
        const int columns = visibleCount > 5 ? 4 : juce::jmax(1, visibleCount);
        const int rows = (visibleCount + columns - 1) / columns;
        const int cellWidth = juce::jmax(1, area.getWidth() / columns);
        const int cellHeight = juce::jmax(1, area.getHeight() / juce::jmax(1, rows));
        int index = 0;
        for (auto& control : controls)
        {
            if (control.group != selectedGroup)
            {
                control.label->setVisible(false);
                control.slider->setVisible(false);
                continue;
            }
            control.label->setVisible(true);
            control.slider->setVisible(true);
            const int column = index % columns;
            const int row = index / columns;
            auto cell = juce::Rectangle<int>(area.getX() + column * cellWidth,
                area.getY() + row * cellHeight, cellWidth, cellHeight).reduced(6, 2);
            control.label->setBounds(cell.removeFromTop(20));
            control.slider->setBounds(cell);
            ++index;
        }
    }

private:
    struct Group
    {
        juce::String name, enabledParameter;
        std::vector<std::pair<juce::String, juce::String>> parameters;
    };
    struct Control
    {
        int group;
        std::unique_ptr<juce::Label> label;
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    void selectGroup(int index)
    {
        selectedGroup = juce::jlimit(0, static_cast<int>(groups.size()) - 1, index);
        powerAttachment.reset();
        powerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, groups[static_cast<size_t>(selectedGroup)].enabledParameter, powerButton);
        resized();
        repaint();
    }
    PPGUThemeManager::Palette palette;
    juce::AudioProcessorValueTreeState& apvts;
    std::vector<Group> groups;
    std::vector<Control> controls;
    juce::Label title, graphCaption, status;
    juce::ComboBox effectSelector;
    juce::TextButton powerButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
    juce::Rectangle<int> graphArea;
    int selectedGroup = 0;
};

class PPGUAutoChordsPanel final : public juce::Component, public PPGUThemeAwarePanel
{
public:
    explicit PPGUAutoChordsPanel(juce::AudioProcessorValueTreeState& state) : apvts(state)
    {
        title.setText("AUTOCHORDS  /  HARMONY SETUP", juce::dontSendNotification);
        title.setFont(juce::Font(20.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(0xffb7f34a));
        addAndMakeVisible(title);
        stateLabel.setText("SETUP ONLY — MIDI CHORD GENERATION / CUSTOM MODE / PREVIEW ENGINE NOT CONNECTED", juce::dontSendNotification);
        stateLabel.setColour(juce::Label::textColourId, juce::Colour(0xffffc66d));
        addAndMakeVisible(stateLabel);
        offButton.setButtonText("AUTOCHORDS OFF");
        offButton.setEnabled(false);
        offButton.setTooltip("The AutoChords MIDI-generation engine is not implemented yet.");
        addAndMakeVisible(offButton);

        keyBox.addItemList({ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 1);
        modeBox.addItemList({ "Chromatic", "Major", "Minor" }, 1);
        patternBox.addItemList({ "Pattern 1", "Pattern 2", "Pattern 3", "Pattern 4",
                                   "Pattern 5", "Pattern 6", "Pattern 7", "Pattern 8" }, 1);
        addAndMakeVisible(keyBox);
        addAndMakeVisible(modeBox);
        addAndMakeVisible(patternBox);
        keyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "scaleRoot", keyBox);
        modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "scaleMode", modeBox);
        patternAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "patternSlot", patternBox);
        for (auto* label : { &keyLabel, &modeLabel, &patternLabel })
        {
            label->setColour(juce::Label::textColourId, juce::Colour(0xffb5c2ca));
            addAndMakeVisible(*label);
        }
        keyLabel.setText("KEY", juce::dontSendNotification);
        modeLabel.setText("SCALE / QUALITY", juce::dontSendNotification);
        patternLabel.setText("RHYTHM PATTERN SLOT", juce::dontSendNotification);
    }
    void setThemePalette(const PPGUThemeManager::Palette& value) override
    {
        palette = value;
        title.setColour(juce::Label::textColourId, palette.token("text_primary"));
        stateLabel.setColour(juce::Label::textColourId, palette.token("warning"));
        for (auto* label : { &keyLabel, &modeLabel, &patternLabel })
            label->setColour(juce::Label::textColourId, palette.token("text_secondary"));
        offButton.setColour(juce::TextButton::buttonColourId, palette.token("panel_elevated"));
        offButton.setColour(juce::TextButton::textColourOffId, palette.token("text_primary"));
        for (auto* box : { &keyBox, &modeBox, &patternBox })
        {
            box->setColour(juce::ComboBox::backgroundColourId, palette.token("control_bg"));
            box->setColour(juce::ComboBox::textColourId, palette.token("text_primary"));
            box->setColour(juce::ComboBox::outlineColourId, palette.token("control_border"));
            box->setColour(juce::ComboBox::arrowColourId, palette.token("accent"));
        }
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(palette.token("window_bg"));
        g.setColour(palette.token("control_border"));
        g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(12), 10, 1);
    }
    void resized() override
    {
        auto area = getLocalBounds().reduced(24);
        title.setBounds(area.removeFromTop(38));
        stateLabel.setBounds(area.removeFromTop(32));
        offButton.setBounds(area.removeFromTop(44).removeFromLeft(210).reduced(2));
        area.removeFromTop(24);
        keyLabel.setBounds(area.removeFromTop(22));
        keyBox.setBounds(area.removeFromTop(44).removeFromLeft(260));
        area.removeFromTop(20);
        modeLabel.setBounds(area.removeFromTop(22));
        modeBox.setBounds(area.removeFromTop(44).removeFromLeft(260));
        area.removeFromTop(20);
        patternLabel.setBounds(area.removeFromTop(22));
        patternBox.setBounds(area.removeFromTop(44).removeFromLeft(360));
    }
private:
    PPGUThemeManager::Palette palette;
    juce::AudioProcessorValueTreeState& apvts;
    juce::Label title, stateLabel, keyLabel, modeLabel, patternLabel;
    juce::TextButton offButton;
    juce::ComboBox keyBox, modeBox, patternBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> keyAttachment, modeAttachment, patternAttachment;
};
}

// Premium SIGNAL FORGE shell. Visual theme code never touches processor/audio state.

namespace { void getPluginModuleAnchor() {} }

void PPGUPremiumLookAndFeel::drawRotarySlider(juce::Graphics& g,int x,int y,int w,int h,float pos,float start,float end,juce::Slider& slider)
{
 const float d=juce::jmin((float)w,(float)h)*.76f,r=d*.5f,cx=x+w*.5f,cy=y+h*.5f-2,angle=start+pos*(end-start);
 g.setColour(juce::Colours::black.withAlpha(.34f));g.fillEllipse(cx-r-1,cy-r+3,d+2,d+2);
 juce::Path arc;arc.addCentredArc(cx,cy,r+5,r+5,0,start,end,true);g.setColour(palette.border);g.strokePath(arc,juce::PathStrokeType(4,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
 juce::Path value;value.addCentredArc(cx,cy,r+5,r+5,0,start,angle,true);g.setColour(slider.isEnabled()?palette.accent:palette.textSecondary.withAlpha(.4f));g.strokePath(value,juce::PathStrokeType(4,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
 g.setGradientFill(juce::ColourGradient(palette.surfaceRaised.brighter(.12f),cx,cy-r,palette.canvasInset.darker(.12f),cx,cy+r,false));g.fillEllipse(cx-r+4,cy-r+4,d-8,d-8);g.setColour(palette.border);g.drawEllipse(cx-r+4,cy-r+4,d-8,d-8,1.2f);
 juce::Path pointer;pointer.startNewSubPath(cx,cy);pointer.lineTo(cx+std::sin(angle)*r*.56f,cy-std::cos(angle)*r*.56f);g.setColour(palette.text);g.strokePath(pointer,juce::PathStrokeType(2.6f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));g.setColour(palette.accentSecondary);g.fillEllipse(cx-2.5f,cy-2.5f,5,5);
}
void PPGUPremiumLookAndFeel::drawButtonBackground(juce::Graphics& g,juce::Button& b,const juce::Colour& base,bool hi,bool down)
{
 auto r=b.getLocalBounds().toFloat().reduced(.5f);auto c=b.getToggleState()?palette.accent:base;if(!b.isEnabled())c=palette.surfaceSecondary;else if(down)c=c.darker(.18f);else if(hi)c=c.brighter(.12f);
 g.setGradientFill(juce::ColourGradient(c.brighter(.08f),r.getX(),r.getY(),c.darker(.16f),r.getX(),r.getBottom(),false));g.fillRoundedRectangle(r,7);g.setColour(palette.border);g.drawRoundedRectangle(r,7,1);
 if(b.getToggleState()&&b.isEnabled()){g.setColour(palette.accentSecondary);g.fillRoundedRectangle(r.removeFromLeft(3),1.5f);}
}
void PPGUPremiumLookAndFeel::drawComboBox(juce::Graphics& g,int w,int h,bool down,int,int,int,int,juce::ComboBox& box)
{
 auto r=juce::Rectangle<float>(.5f,.5f,(float)w-1,(float)h-1);auto c=box.findColour(juce::ComboBox::backgroundColourId);g.setColour(down?c.brighter(.08f):c);g.fillRoundedRectangle(r,7);g.setColour(palette.border);g.drawRoundedRectangle(r,7,1);
 float cx=r.getRight()-17,cy=r.getCentreY();juce::Path p;p.startNewSubPath(cx-4,cy-2);p.lineTo(cx,cy+2);p.lineTo(cx+4,cy-2);g.setColour(palette.accent);g.strokePath(p,juce::PathStrokeType(1.8f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
}

PopPunkGuitarUltimateAudioProcessorEditor::PopPunkGuitarUltimateAudioProcessorEditor(PopPunkGuitarUltimateAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setWantsKeyboardFocus(true);
    setLookAndFeel(&premiumLookAndFeel);
    runtimeSkinRoot = getRuntimeSkinRoot();
    // Medium first-launch footprint for FL Studio; the JUCE resize corner remains enabled.
    // The responsive layout below keeps the main controls visible down to the minimum size.
    setSize(1024, 700);
    setResizable(true, true);
    setResizeLimits(900, 640, 1600, 1100);
    titleLabel.setText("PPGU", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, themeManager.getPalette().token("text_primary"));
    addAndMakeVisible(titleLabel);
    subtitleLabel.setText("L3XTOWN AUDIO LAB", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(9.5f, juce::Font::bold));
    subtitleLabel.setColour(juce::Label::textColourId, themeManager.getPalette().token("accent"));
    addAndMakeVisible(subtitleLabel);
    presetBox.setTextWhenNothingSelected("FACTORY PRESET CATALOG");
    presetBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    presetBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    presetBox.setColour(juce::ComboBox::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.5f));
    addAndMakeVisible(presetBox);
    categoryBox.addItem("ALL PRESETS", 1);
    categoryBox.addItem("MELODY", 2);
    categoryBox.addItem("PATTERN", 3);
    categoryBox.setSelectedId(1, juce::dontSendNotification);
    categoryBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    categoryBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    addAndMakeVisible(categoryBox);
    presetSearch.setTextToShowWhenEmpty("SEARCH PRESETS...", themeManager.getPalette().token("text_secondary"));
    presetSearch.setColour(juce::TextEditor::backgroundColourId, themeManager.getPalette().token("control_bg"));
    presetSearch.setColour(juce::TextEditor::textColourId, themeManager.getPalette().token("text_primary"));
    presetSearch.setColour(juce::TextEditor::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.4f));
    addAndMakeVisible(presetSearch);
    loadFactoryPresetCatalog();
    presetBox.onChange = [this] { if (presetBox.getSelectedId() > 0 && presetBox.getSelectedId() <= static_cast<int>(visiblePresetIndices.size())) applyFactoryPreset(visiblePresetIndices[static_cast<size_t>(presetBox.getSelectedId()-1)]); };
    auto refreshPresetList = [this] {
        // Keep the current sound selected when it remains in the filtered list.
        // Filtering must never silently load a different preset.
        int previouslySelectedPreset = -1;
        const int oldSelectedId = presetBox.getSelectedId();
        if (oldSelectedId > 0 && oldSelectedId <= static_cast<int>(visiblePresetIndices.size()))
            previouslySelectedPreset = visiblePresetIndices[static_cast<size_t>(oldSelectedId - 1)];

        const auto query = presetSearch.getText().trim().toLowerCase();
        const int category = categoryBox.getSelectedId();
        visiblePresetIndices.clear();
        presetBox.clear(juce::dontSendNotification);
        for (size_t i = 0; i < factoryPresets.size(); ++i) {
            const auto& item = factoryPresets[i];
            if (category == 2 && item.mode != "MELODY") continue;
            if (category == 3 && item.mode != "PATTERN") continue;

            const auto matchesQuery = query.isEmpty()
                || item.name.toLowerCase().contains(query)
                || item.category.toLowerCase().contains(query)
                || item.mode.toLowerCase().contains(query)
                || item.soundCharacter.toLowerCase().contains(query)
                || item.groove.toLowerCase().contains(query)
                || item.articulation.toLowerCase().contains(query);
            if (!matchesQuery) continue;

            visiblePresetIndices.push_back(static_cast<int>(i));
            presetBox.addItem(item.name, static_cast<int>(visiblePresetIndices.size()));
        }

        const auto selected = std::find(visiblePresetIndices.begin(), visiblePresetIndices.end(),
                                        previouslySelectedPreset);
        if (previouslySelectedPreset >= 0 && selected != visiblePresetIndices.end())
            presetBox.setSelectedId(static_cast<int>(std::distance(visiblePresetIndices.begin(), selected)) + 1,
                                    juce::dontSendNotification);
        else
            presetBox.setSelectedId(0, juce::dontSendNotification);
    };
    categoryBox.onChange = refreshPresetList;
    presetSearch.onTextChange = refreshPresetList;
    refreshPresetList();
    loadButton.setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("accent"));
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    loadButton.onClick = [this] { openImportGuitarPanel(); };
    addAndMakeVisible(loadButton);
    auxiliaryWorkspace = std::make_unique<PPGUMultiWindowWorkspace>();
    auxiliaryWorkspace->setSurfaceColours(themeManager.getPalette().token("window_bg"),
        themeManager.getPalette().token("panel"), themeManager.getPalette().token("control_border"),
        themeManager.getPalette().token("accent"), themeManager.getPalette().token("text_primary"));
    addAndMakeVisible(*auxiliaryWorkspace);
    auxiliaryWorkspace->setVisible(false);
    settingsButton.setTooltip("Workspace scale, panel spacing and visual defaults.");
    settingsButton.setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("panel_elevated"));
    settingsButton.setColour(juce::TextButton::textColourOffId, themeManager.getPalette().token("text_primary"));
    settingsButton.onClick = [this] { openSettingsPanel(); };
    addAndMakeVisible(settingsButton);
    skinButton.setTooltip("Open the premium visual skin gallery. Ctrl+K.");
    skinButton.onClick = [this] { openSkinGalleryPanel(); };
    addAndMakeVisible(skinButton);
    autoChordsButton.setTooltip("Open chord and harmony configuration workspace.");
    autoChordsButton.setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("control_bg"));
    autoChordsButton.setColour(juce::TextButton::textColourOffId, themeManager.getPalette().token("text_primary"));
    autoChordsButton.onClick = [this] { openAutoChordsPanel(); };
    addAndMakeVisible(autoChordsButton);
    statusLabel.setText("NO LIBRARY LOADED  /  LOAD A LICENSED SFZ INSTRUMENT", juce::dontSendNotification);
    const auto savedSfzPath = processor.getCurrentSfzFilePath();
    if (savedSfzPath.isNotEmpty())
    {
        const juce::File savedSfzFile(savedSfzPath);
        statusLabel.setText(savedSfzFile.existsAsFile()
            ? "SAVED LIBRARY  /  " + savedSfzFile.getFileName()
            : "LIBRARY FILE NOT FOUND  /  " + savedSfzFile.getFileName(),
            juce::dontSendNotification);
    }
    statusLabel.setColour(juce::Label::textColourId, themeManager.getPalette().token("text_secondary"));
    statusLabel.setFont(juce::Font(11.0f));
    addAndMakeVisible(statusLabel);
    melodyTab.setClickingTogglesState(false);
    patternTab.setClickingTogglesState(false);
    for (auto* b : { &melodyTab, &patternTab }) {
        b->setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("control_bg"));
        b->setColour(juce::TextButton::textColourOffId, themeManager.getPalette().token("text_primary"));
        addAndMakeVisible(*b);
    }
    modeBox.addItem("MELODY", 1);
    modeBox.addItem("PATTERN", 2);
    modeBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    modeBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    modeBox.setColour(juce::ComboBox::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.5f));
    addAndMakeVisible(modeBox);
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "playMode", modeBox);
    startModeBox.addItem("HOST", 1);
    startModeBox.addItem("HOST START", 2);
    startModeBox.addItem("INTERNAL", 3);
    startModeBox.setColour(juce::ComboBox::backgroundColourId, themeManager.getPalette().token("control_bg"));
    startModeBox.setColour(juce::ComboBox::textColourId, themeManager.getPalette().token("text_primary"));
    startModeBox.setColour(juce::ComboBox::outlineColourId, themeManager.getPalette().token("accent").withAlpha(0.5f));
    addAndMakeVisible(startModeBox);
    startModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "syncMode", startModeBox);
    keyboard = std::make_unique<juce::MidiKeyboardComponent>(processor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard);
    keyboard->setAvailableRange(36, 84);
    keyboard->setKeyWidth(18.0f);
    keyboard->setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xffe8e7eb));
    keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff17171d));
    keyboard->setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colour(0xff34343d));
    keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, themeManager.getPalette().token("accent").withAlpha(0.35f));
    keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, themeManager.getPalette().token("accent").withAlpha(0.85f));
    addAndMakeVisible(*keyboard);
    performanceDisplay = std::make_unique<PPGUPerformanceDisplay>(processor);
    addAndMakeVisible(*performanceDisplay);
    melodyTab.onClick = [this] { modeBox.setSelectedId(1); };
    patternTab.onClick = [this] { modeBox.setSelectedId(2); };
    for (size_t i = 0; i < knobs.size(); ++i) {
        configureKnob(knobs[i], knobLabels[i], knobNames[i]);
        knobAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, parameterIDs[i], knobs[i]);
    }
    for (size_t i = 0; i < fxButtons.size(); ++i) {
        auto& b = fxButtons[i];
        b.setClickingTogglesState(true);
        b.setColour(juce::TextButton::buttonColourId, themeManager.getPalette().token("control_bg"));
        b.setColour(juce::TextButton::buttonOnColourId, themeManager.getPalette().token("accent"));
        b.setColour(juce::TextButton::textColourOffId, themeManager.getPalette().token("text_primary"));
        b.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        fxAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters, fxParameterIDs[i], b);
        b.onClick = [this, i] { openFxEditPanel(static_cast<int>(i)); };
        addAndMakeVisible(b);
    }
    ampTitle.setText("AMP & TONE", juce::dontSendNotification);
    fxTitle.setText("FX CHAIN", juce::dontSendNotification);
    playbackTitle.setText("PLAYBACK", juce::dontSendNotification);
    for (auto* l : { &ampTitle, &fxTitle, &playbackTitle }) {
        l->setFont(juce::Font(15.0f, juce::Font::bold));
        l->setColour(juce::Label::textColourId, themeManager.getPalette().token("accent"));
        addAndMakeVisible(*l);
    }
    juce::PropertiesFile::Options options;
    options.applicationName = "PPGU";
    options.filenameSuffix = "settings";
    options.folderName = "L3XTOWN";
    uiProperties.setStorageParameters(options);
    int restoredWidth = 1024;
    int restoredHeight = 700;
    if (auto* settings = uiProperties.getUserSettings())
    {
        auxiliaryWorkspace->setPanelScale(static_cast<float>(settings->getDoubleValue("workspacePanelScale", 1.0)));
        auxiliaryWorkspace->setPanelGap(settings->getIntValue("workspacePanelGap", 12));
        restoredWidth = settings->getIntValue("editorWidth", restoredWidth);
        restoredHeight = settings->getIntValue("editorHeight", restoredHeight);
    }
    loadSavedSkin();
    applyPalette();
    uiSettingsReady = true;
    setSize(juce::jlimit(900, 1600, restoredWidth),
            juce::jlimit(640, 1100, restoredHeight));
}

PopPunkGuitarUltimateAudioProcessorEditor::~PopPunkGuitarUltimateAudioProcessorEditor()
{
    stopTimer();
    timerCallback();
    setLookAndFeel(nullptr);
}

void PopPunkGuitarUltimateAudioProcessorEditor::timerCallback()
{
    stopTimer();
    if (!uiSettingsReady)
        return;
    if (auto* settings = uiProperties.getUserSettings())
    {
        settings->setValue("editorWidth", getWidth());
        settings->setValue("editorHeight", getHeight());
        settings->saveIfNeeded();
    }
}

void PopPunkGuitarUltimateAudioProcessorEditor::configureKnob(juce::Slider& s, juce::Label& l, const juce::String& name)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 46, 16);
    s.setColour(juce::Slider::rotarySliderFillColourId, themeManager.getPalette().token("accent"));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff44444e));
    s.setColour(juce::Slider::thumbColourId, themeManager.getPalette().token("text_primary"));
    s.setColour(juce::Slider::textBoxTextColourId, themeManager.getPalette().token("text_primary"));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(s);
    l.setText(name, juce::dontSendNotification);
    l.setFont(juce::Font(9.5f, juce::Font::bold));
    l.setColour(juce::Label::textColourId, themeManager.getPalette().token("text_secondary"));
    l.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(l);
}

void PopPunkGuitarUltimateAudioProcessorEditor::drawPanel(
    juce::Graphics& g, juce::Rectangle<int> area, const juce::String& sectionTitle)
{
    const auto& p = themeManager.getPalette();
    const auto bounds = area.toFloat();

    g.setGradientFill(juce::ColourGradient(p.token("panel_elevated"),
                                           bounds.getX(), bounds.getY(),
                                           p.token("panel"),
                                           bounds.getX(), bounds.getBottom(), false));
    g.fillRoundedRectangle(bounds, 12.0f);
    g.setColour(p.token("control_border").withAlpha(0.78f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 12.0f, 1.0f);

    // Section chrome belongs to the visual shell; it does not create or
    // reassign controls and follows the active theme's semantic palette.
    g.setColour(p.token("text_secondary"));
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    auto heading = area.reduced(15, 7).removeFromTop(19);
    g.drawText(sectionTitle, heading, juce::Justification::centredLeft, true);

    g.setColour(p.token("accent").withAlpha(0.9f));
    g.fillRoundedRectangle(bounds.getX() + 15.0f, bounds.getY() + 29.0f,
                           34.0f, 2.0f, 1.0f);
    g.setColour(p.token("control_border").withAlpha(0.45f));
    g.drawHorizontalLine(static_cast<int>(bounds.getY() + 33.0f),
                         bounds.getX() + 15.0f, bounds.getRight() - 15.0f);
}

void PopPunkGuitarUltimateAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto& p = themeManager.getPalette();
    g.fillAll(p.token("window_bg"));
    auto area = getLocalBounds().reduced(14);
    area.removeFromTop(48);
    area.removeFromBottom(132);
    area.removeFromTop(8);
    const int gap = 10;
    const int browserWidth = juce::jlimit(190, 250, area.getWidth() / 5);
    const int rightWidth = juce::jlimit(230, 300, area.getWidth() / 4);
    auto browser = area.removeFromLeft(browserWidth);
    area.removeFromLeft(gap);
    auto right = area.removeFromRight(rightWidth);
    area.removeFromRight(gap);
    drawPanel(g, browser, "PRESET LIBRARY");
    drawPanel(g, area, "GUITAR ENGINE  /  PERFORMANCE");
    drawPanel(g, right, "SIGNAL CHAIN");

    // Subtle SIGNAL FORGE center identity; controls remain live JUCE components.
    auto badge = area.removeFromTop(3);
    g.setColour(p.token("accent"));
    g.fillRoundedRectangle(badge.toFloat(), 1.5f);
    g.setColour(p.token("text_secondary").withAlpha(0.12f));
    for (int x = area.getX() + 16; x < area.getRight(); x += 32)
        g.drawVerticalLine(x, (float)area.getY() + 8.0f, (float)area.getBottom() - 8.0f);
}

void PopPunkGuitarUltimateAudioProcessorEditor::resized()
{
    // Debounce disk persistence while the user drags the resize corner.
    if (uiSettingsReady)
        startTimer(450);

    auto bounds = getLocalBounds().reduced(14);
    if (auxiliaryWorkspace != nullptr)
        auxiliaryWorkspace->setBounds(bounds);

    // SIGNAL FORGE three-zone shell: browser / instrument / amp+FX.
    // Lower performance strip remains full width. APVTS attachments are untouched.
    auto header = bounds.removeFromTop(48);
    titleLabel.setBounds(header.removeFromLeft(105).reduced(2, 4));
    subtitleLabel.setBounds(18, 47, 150, 14);
    header.removeFromLeft(8);
    skinButton.setBounds(header.removeFromRight(78).reduced(2, 5));
    settingsButton.setBounds(header.removeFromRight(92).reduced(2, 5));
    autoChordsButton.setBounds(header.removeFromRight(112).reduced(2, 5));
    loadButton.setBounds(header.removeFromRight(112).reduced(2, 5));
    header.removeFromRight(8);

    auto footer = bounds.removeFromBottom(132);
    statusLabel.setBounds(footer.removeFromBottom(22).reduced(3, 1));
    footer.removeFromBottom(4);
    if (performanceDisplay != nullptr)
        performanceDisplay->setBounds(footer.removeFromTop(42).reduced(2, 1));
    footer.removeFromTop(5);
    if (keyboard != nullptr)
        keyboard->setBounds(footer.reduced(2, 2));

    bounds.removeFromTop(8);
    const int gap = 10;
    const int browserWidth = juce::jlimit(190, 250, bounds.getWidth() / 5);
    const int rightWidth = juce::jlimit(230, 300, bounds.getWidth() / 4);
    auto browser = bounds.removeFromLeft(browserWidth);
    bounds.removeFromLeft(gap);
    auto right = bounds.removeFromRight(rightWidth);
    bounds.removeFromRight(gap);
    auto centre = bounds;

    // Left preset browser: section chrome is reserved above live controls.
    browser.removeFromTop(34);
    presetBox.setBounds(browser.removeFromTop(42).reduced(3, 3));
    browser.removeFromTop(5);
    categoryBox.setBounds(browser.removeFromTop(34).reduced(3, 2));
    browser.removeFromTop(5);
    presetSearch.setBounds(browser.removeFromTop(34).reduced(3, 2));

    // Center instrument zone: tabs, mode and all nine performance controls.
    auto modeRow = centre.removeFromTop(42);
    melodyTab.setBounds(modeRow.removeFromLeft(modeRow.getWidth() / 3).reduced(2, 3));
    patternTab.setBounds(modeRow.removeFromLeft(modeRow.getWidth() / 2).reduced(2, 3));
    modeBox.setBounds(modeRow.reduced(2, 3));
    centre.removeFromTop(6);
    auto knobArea = centre;
    knobArea.removeFromBottom(42);
    const int cols = 3;
    const int rows = 3;
    const int cellW = juce::jmax(1, knobArea.getWidth() / cols);
    const int cellH = juce::jmax(1, knobArea.getHeight() / rows);
    for (size_t i = 0; i < knobs.size(); ++i)
    {
        const int col = static_cast<int>(i) % cols;
        const int row = static_cast<int>(i) / cols;
        auto cell = juce::Rectangle<int>(knobArea.getX() + col * cellW,
            knobArea.getY() + row * cellH, cellW, cellH).reduced(3, 2);
        knobLabels[i].setBounds(cell.removeFromTop(20));
        knobs[i].setBounds(cell.reduced(2, 1));
    }
    startModeBox.setBounds(centre.removeFromBottom(36).reduced(3, 2));

    // Right rack: section chrome is reserved above amplifier and FX headings.
    right.removeFromTop(34);
    ampTitle.setBounds(right.removeFromTop(32).reduced(5, 2));
    fxTitle.setBounds(right.removeFromTop(28).reduced(5, 1));
    for (size_t i = 0; i < fxButtons.size(); ++i)
    {
        auto row = right.removeFromTop(juce::jmax(1, right.getHeight()
            / static_cast<int>(fxButtons.size() - i)));
        fxButtons[i].setBounds(row.reduced(5, 2));
    }

    // Legacy title is no longer part of the visible main shell.
    playbackTitle.setVisible(false);
}

juce::File PopPunkGuitarUltimateAudioProcessorEditor::getRuntimeSkinRoot() const
{
#if JUCE_WINDOWS
 HMODULE module=nullptr;auto address=reinterpret_cast<LPCWSTR>(reinterpret_cast<std::uintptr_t>(&getPluginModuleAnchor));
 DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
 if(GetModuleHandleExW(flags,address,&module)!=0&&module){std::array<wchar_t,32768> path{};DWORD n=GetModuleFileNameW(module,path.data(),(DWORD)path.size());
  if(n>0&&n<path.size()){juce::File f(fromWidePath(path.data(), static_cast<int>(n)));for(int i=0;i<8;++i){auto d=f.isDirectory()?f:f.getParentDirectory();if(d.getFileName().endsWithIgnoreCase(".vst3"))return d.getParentDirectory().getChildFile("Skins");f=d.getParentDirectory();}}}
#endif
 auto exe=juce::File::getSpecialLocation(juce::File::currentExecutableFile);auto beside=exe.getParentDirectory().getChildFile("Skins");if(beside.isDirectory())return beside;
 auto dev=juce::File::getCurrentWorkingDirectory().getChildFile("SKIN");return dev.isDirectory()?dev:beside;
}
int PopPunkGuitarUltimateAudioProcessorEditor::getSkinCount() const
{
    const auto registryFile = runtimeSkinRoot.getChildFile("theme_manifest.json");
    if (!registryFile.existsAsFile())
        return 0;

    const auto registry = juce::JSON::parse(registryFile.loadFileAsString());
    if (!registry.isObject())
        return 0;

    const auto totalValue = registry.getProperty("totalSkins", juce::var());
    const auto themesValue = registry.getProperty("themes", juce::var());
    const auto* themes = themesValue.getArray();
    if ((!totalValue.isInt() && !totalValue.isInt64()) || themes == nullptr)
        return 0;

    const auto total64 = totalValue.toString().getLargeIntValue();
    if (total64 < 1 || total64 > std::numeric_limits<int>::max()
        || total64 != static_cast<std::int64_t>(themes->size()))
        return 0;
    const auto total = static_cast<int>(total64);

    std::set<int> indices;
    std::set<std::string> ids;
    for (const auto& entry : *themes)
    {
        if (!entry.isObject())
            return 0;

        const auto indexValue = entry.getProperty("index", juce::var());
        const auto id = entry.getProperty("id", "").toString();
        const auto path = entry.getProperty("path", "").toString();
        if ((!indexValue.isInt() && !indexValue.isInt64())
            || id.isEmpty() || path.isEmpty()
            || path.startsWith("/") || path.contains("..")
            || !path.startsWith("Skin_") || !path.endsWith("/theme.json"))
            return 0;

        const auto index64 = indexValue.toString().getLargeIntValue();
        if (index64 < 1 || index64 > total || !id.startsWith("skin-")
            || id.substring(5).getLargeIntValue() != index64
            || !indices.insert(static_cast<int>(index64)).second
            || !ids.insert(id.toStdString()).second)
            return 0;
    }

    return static_cast<int>(indices.size()) == total ? total : 0;
}

juce::File PopPunkGuitarUltimateAudioProcessorEditor::getSkinManifest(int index) const
{
    if (index < 1)
        return {};

    const auto registryFile = runtimeSkinRoot.getChildFile("theme_manifest.json");
    if (!registryFile.existsAsFile())
        return {};

    const auto registry = juce::JSON::parse(registryFile.loadFileAsString());
    const auto themesValue = registry.getProperty("themes", juce::var());
    const auto* themes = themesValue.getArray();
    if (themes == nullptr)
        return {};

    for (const auto& entry : *themes)
    {
        if (!entry.isObject())
            continue;
        const auto indexValue = entry.getProperty("index", juce::var());
        if ((!indexValue.isInt() && !indexValue.isInt64())
            || indexValue.toString().getLargeIntValue() != index)
            continue;

        const auto relativePath = entry.getProperty("path", "").toString();
        if (relativePath.isEmpty() || relativePath.startsWith("/")
            || relativePath.contains("..") || !relativePath.startsWith("Skin_")
            || !relativePath.endsWith("/theme.json"))
            return {};

        const auto manifest = runtimeSkinRoot.getChildFile(relativePath);
        return manifest.isAChildOf(runtimeSkinRoot) ? manifest : juce::File{};
    }
    return {};
}

void PopPunkGuitarUltimateAudioProcessorEditor::loadSavedSkin()
{
    auto* settings = uiProperties.getUserSettings();
    const auto id = settings ? settings->getValue("skinId", "skin-053") : juce::String("skin-053");
    const auto index = id.startsWith("skin-") ? id.substring(5).getIntValue() : 0;
    if (index < 1 || index > getSkinCount() || !loadSkinByIndex(index, false))
    {
        themeManager.resetToDefault();
        applyPalette();
    }
}

bool PopPunkGuitarUltimateAudioProcessorEditor::loadSkinByIndex(int index, bool save)
{
    if (index < 1 || index > getSkinCount())
        return false;

    const auto manifest = getSkinManifest(index);
    const bool loaded = manifest.existsAsFile() && themeManager.loadFromFile(manifest);
    if (!loaded)
        themeManager.resetToDefault();
    applyPalette();
    if (save && loaded)
        persistSkinPreference();
    return loaded;
}

void PopPunkGuitarUltimateAudioProcessorEditor::openSkinGalleryPanel()
{
    if (auxiliaryWorkspace == nullptr)
        return;

    constexpr int galleryPanelId = 5;
    if (!auxiliaryWorkspace->isPanelOpen(galleryPanelId))
    {
        std::vector<PPGUSkinGalleryPanel::Skin> skins;
        const auto activeId = themeManager.getPalette().id;
        const auto count = getSkinCount();
        skins.reserve(static_cast<size_t>(juce::jmax(0, count)));

        for (int index = 1; index <= count; ++index)
        {
            const auto manifest = getSkinManifest(index);
            if (!manifest.existsAsFile())
                continue;

            PPGUThemeManager candidate;
            if (!candidate.loadFromFile(manifest))
                continue;

            const auto root = juce::JSON::parse(manifest.loadFileAsString());
            const auto familyKey = root.getProperty("family", juce::var()).toString();
            const auto legacyCluster = root.getProperty("cluster", juce::var()).toString();
            const auto family = familyKey.isNotEmpty()
                ? familyKey.replace("_", " ").toUpperCase()
                : "LEGACY / CLUSTER " + legacyCluster;

            juce::File preview;
            const auto assets = root.getProperty("assets", juce::var());
            auto previewRelative = assets.getProperty("preview", juce::var()).toString();
            if (previewRelative.isEmpty())
                previewRelative = "assets/preview.png";
            if (!previewRelative.startsWith("/") && !previewRelative.contains(".."))
            {
                const auto candidatePreview = manifest.getParentDirectory().getChildFile(previewRelative);
                if (candidatePreview.isAChildOf(manifest.getParentDirectory())
                    && candidatePreview.existsAsFile())
                    preview = candidatePreview;
            }

            PPGUSkinGalleryPanel::Skin item;
            item.index = index;
            item.id = candidate.getPalette().id;
            item.name = candidate.getPalette().name;
            item.family = family;
            item.familyKey = familyKey.isNotEmpty() ? familyKey : "legacy";
            item.layout = root.getProperty("layout", "CLASSIC").toString().replace("_", " ");
            item.preview = preview;
            item.accent = candidate.getPalette().token("accent");
            item.active = candidate.getPalette().id == activeId;
            skins.push_back(std::move(item));
        }

        juce::Component::SafePointer<PopPunkGuitarUltimateAudioProcessorEditor> safe(this);
        const auto gallerySkinCount = static_cast<int>(skins.size());
        auto gallery = std::make_unique<PPGUSkinGalleryPanel>(
            std::move(skins), themeManager.getPalette(),
            [safe](int index)
            {
                if (safe == nullptr)
                    return;
                safe->loadSkinByIndex(index, true);
                if (safe->auxiliaryWorkspace != nullptr)
                {
                    safe->auxiliaryWorkspace->closePanel(5);
                    if (safe->auxiliaryWorkspace->getOpenPanelCount() == 0)
                        safe->auxiliaryWorkspace->setVisible(false);
                }
            });

        PPGUMultiWindowWorkspace::PanelSpec spec;
        spec.id = galleryPanelId;
        spec.title = "SKIN GALLERY  /  " + juce::String(gallerySkinCount) + " THEMES";
        spec.desired = { 940, 700 };
        spec.minimum = { 500, 320 };
        spec.content = std::move(gallery);
        auxiliaryWorkspace->openPanel(std::move(spec));
    }

    auxiliaryWorkspace->setVisible(true);
    auxiliaryWorkspace->toFront(false);
}

void PopPunkGuitarUltimateAudioProcessorEditor::openSettingsPanel()
{
    if (auxiliaryWorkspace == nullptr)
        return;

    constexpr int settingsPanelId = 6;
    if (!auxiliaryWorkspace->isPanelOpen(settingsPanelId))
    {
        auto* properties = uiProperties.getUserSettings();
        const auto panelScale = properties != nullptr
            ? static_cast<float>(properties->getDoubleValue("workspacePanelScale", 1.0)) : 1.0f;
        const auto panelGap = properties != nullptr
            ? properties->getIntValue("workspacePanelGap", 12) : 12;

        juce::Component::SafePointer<PopPunkGuitarUltimateAudioProcessorEditor> safe(this);
        auto content = std::make_unique<PPGUSettingsPanel>(
            themeManager.getPalette(), panelScale, panelGap,
            [safe](float value)
            {
                if (safe == nullptr || safe->auxiliaryWorkspace == nullptr)
                    return;
                safe->auxiliaryWorkspace->setPanelScale(value);
                if (auto* settings = safe->uiProperties.getUserSettings())
                {
                    settings->setValue("workspacePanelScale", static_cast<double>(value));
                    settings->saveIfNeeded();
                }
            },
            [safe](int value)
            {
                if (safe == nullptr || safe->auxiliaryWorkspace == nullptr)
                    return;
                safe->auxiliaryWorkspace->setPanelGap(value);
                if (auto* settings = safe->uiProperties.getUserSettings())
                {
                    settings->setValue("workspacePanelGap", value);
                    settings->saveIfNeeded();
                }
            },
            [safe]
            {
                if (safe != nullptr)
                    safe->loadSkinByIndex(52, true);
            },
            [safe]
            {
                if (safe != nullptr)
                    safe->openSkinGalleryPanel();
            });

        PPGUMultiWindowWorkspace::PanelSpec spec;
        spec.id = settingsPanelId;
        spec.title = "SETTINGS  /  WORKSPACE";
        spec.desired = { 700, 520 };
        spec.minimum = { 420, 260 };
        spec.content = std::move(content);
        auxiliaryWorkspace->openPanel(std::move(spec));
    }

    auxiliaryWorkspace->setVisible(true);
    auxiliaryWorkspace->toFront(false);
}

void PopPunkGuitarUltimateAudioProcessorEditor::applyPalette()
{
 const auto& p=themeManager.getPalette();premiumLookAndFeel.setPalette(p);if(performanceDisplay)performanceDisplay->setPalette(p);
 if (auxiliaryWorkspace != nullptr)
 {
     auxiliaryWorkspace->setSurfaceColours(p.token("window_bg"), p.token("panel"),
         p.token("control_border"), p.token("accent"), p.token("text_primary"));
     auxiliaryWorkspace->setThemePalette(p);
 }
 titleLabel.setColour(juce::Label::textColourId,p.token("text_primary"));subtitleLabel.setColour(juce::Label::textColourId,p.token("text_secondary"));statusLabel.setColour(juce::Label::textColourId,p.token("text_secondary"));
 ampTitle.setColour(juce::Label::textColourId,p.token("accent"));fxTitle.setColour(juce::Label::textColourId,p.token("accent"));playbackTitle.setColour(juce::Label::textColourId,p.token("accent"));
 for(auto* b:{&presetBox,&categoryBox,&modeBox,&startModeBox}){b->setColour(juce::ComboBox::backgroundColourId,p.token("control_bg"));b->setColour(juce::ComboBox::textColourId,p.token("text_primary"));b->setColour(juce::ComboBox::outlineColourId,p.token("control_border"));b->setColour(juce::ComboBox::arrowColourId,p.token("accent"));}
 presetSearch.setColour(juce::TextEditor::backgroundColourId,p.token("control_bg"));presetSearch.setColour(juce::TextEditor::textColourId,p.token("text_primary"));presetSearch.setColour(juce::TextEditor::outlineColourId,p.token("control_border"));presetSearch.setColour(juce::TextEditor::focusedOutlineColourId,p.token("highlight"));presetSearch.setColour(juce::TextEditor::highlightColourId,p.token("accent").withAlpha(.3f));
 loadButton.setColour(juce::TextButton::buttonColourId,p.token("accent"));loadButton.setColour(juce::TextButton::textColourOffId,p.token("window_bg"));settingsButton.setColour(juce::TextButton::buttonColourId,p.token("panel_elevated"));settingsButton.setColour(juce::TextButton::textColourOffId,p.token("text_primary"));skinButton.setColour(juce::TextButton::buttonColourId,p.token("panel_elevated"));skinButton.setColour(juce::TextButton::textColourOffId,p.token("text_primary"));skinButton.setTooltip("Active skin: "+p.name+"  /  Ctrl+K");
 for(auto* b:{&melodyTab,&patternTab}){b->setColour(juce::TextButton::buttonColourId,p.token("control_bg"));b->setColour(juce::TextButton::buttonOnColourId,p.token("accent"));b->setColour(juce::TextButton::textColourOffId,p.token("text_primary"));b->setColour(juce::TextButton::textColourOnId,p.token("window_bg"));}
 for(auto& s:knobs){s.setColour(juce::Slider::rotarySliderFillColourId,p.token("accent"));s.setColour(juce::Slider::rotarySliderOutlineColourId,p.token("control_border"));s.setColour(juce::Slider::thumbColourId,p.token("text_primary"));s.setColour(juce::Slider::textBoxTextColourId,p.token("text_primary"));}
 for(auto& l:knobLabels)l.setColour(juce::Label::textColourId,p.token("text_secondary"));
 for(auto& b:fxButtons){b.setColour(juce::TextButton::buttonColourId,p.token("control_bg"));b.setColour(juce::TextButton::buttonOnColourId,p.token("accent"));b.setColour(juce::TextButton::textColourOffId,p.token("text_primary"));b.setColour(juce::TextButton::textColourOnId,p.token("window_bg"));}
 if(keyboard){keyboard->setColour(juce::MidiKeyboardComponent::whiteNoteColourId,juce::Colour(0xffeaf0f2));keyboard->setColour(juce::MidiKeyboardComponent::blackNoteColourId,p.token("control_bg"));keyboard->setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId,p.token("control_border"));keyboard->setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId,p.token("accent_glow").withAlpha(.3f));keyboard->setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId,p.token("accent").withAlpha(.88f));}
 repaint();
}
void PopPunkGuitarUltimateAudioProcessorEditor::persistSkinPreference(){if(auto* s=uiProperties.getUserSettings()){s->setValue("skinId",themeManager.getPalette().id);s->saveIfNeeded();}}
bool PopPunkGuitarUltimateAudioProcessorEditor::keyPressed(const juce::KeyPress& k){if(k.getKeyCode()=='K'&&k.getModifiers().isCtrlDown()){openSkinGalleryPanel();return true;}return juce::AudioProcessorEditor::keyPressed(k);}

void PopPunkGuitarUltimateAudioProcessorEditor::loadFactoryPresetCatalog()
{
    factoryPresets.clear();
    const auto jsonText = juce::String::fromUTF8(BinaryData::factory_bank_300_json, BinaryData::factory_bank_300_jsonSize);
    const auto root = juce::JSON::parse(jsonText);
    if (!root.isObject()) { statusLabel.setText("FACTORY PRESET CATALOG ERROR", juce::dontSendNotification); return; }
    const auto append = [this](const juce::var& array, const juce::String& mode) {
        if (auto* values = array.getArray()) for (const auto& value : *values) {
            if (!value.isObject()) continue;
            FactoryPreset preset;
            auto reference = value.getProperty("referenceArtist", "").toString();
            auto displayName = value.getProperty("name", "Unnamed").toString();
            if (reference.isNotEmpty() && displayName.startsWithIgnoreCase(reference))
                displayName = displayName.substring(reference.length()).trim();
            preset.name = displayName;
            preset.category = "Original";
            preset.mode = mode;
            preset.soundCharacter = value.getProperty("soundCharacter", "Electric").toString();
            preset.groove = value.getProperty("groove", "").toString();
            preset.articulation = value.getProperty("articulation", "").toString();
            const auto params = value.getProperty("parameters", juce::var());
            preset.drive = static_cast<float>(params.getProperty("drive", 0.0));
            preset.tone = static_cast<float>(params.getProperty("tone", 5.0));
            preset.reverb = static_cast<float>(params.getProperty("reverb", 0.0));
            preset.outputDb = static_cast<float>(params.getProperty("outputDb", 0.0));
            factoryPresets.push_back(std::move(preset));
        }
    };
    append(root.getProperty("melodyPresets", juce::var()), "MELODY");
    append(root.getProperty("patternPresets", juce::var()), "PATTERN");
    statusLabel.setText("FACTORY CATALOG: "+juce::String(static_cast<int>(factoryPresets.size()))+" PRESETS", juce::dontSendNotification);
}

void PopPunkGuitarUltimateAudioProcessorEditor::applyFactoryPreset(int index)
{
    if (index < 0 || index >= static_cast<int>(factoryPresets.size())) return;
    const auto& preset = factoryPresets[static_cast<size_t>(index)];
    const auto setParameter = [this](const juce::String& id, float value) {
        if (auto* parameter = processor.parameters.getParameter(id))
            parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
    };
    setParameter("distortionDrive", juce::jlimit(0.0f, 1.0f, preset.drive));
    setParameter("tone", juce::jlimit(0.0f, 10.0f, preset.tone));
    setParameter("reverbMix", juce::jlimit(0.0f, 1.0f, preset.reverb));
    setParameter("output", juce::jlimit(-24.0f, 12.0f, preset.outputDb));
    modeBox.setSelectedId(preset.mode == "PATTERN" ? 2 : 1, juce::sendNotification);
    if (preset.mode == "PATTERN")
    {
        const auto detail = preset.soundCharacter + (preset.groove.isNotEmpty() ? "  /  " + preset.groove : juce::String{});
        statusLabel.setText("PATTERN STYLE: " + detail + "  /  " + preset.articulation + "  /  PERFORMANCE ENGINE REQUIRED", juce::dontSendNotification);
    }
    else
        statusLabel.setText("MELODY TONE: " + preset.soundCharacter + "  /  ONE KEY = ONE NOTE  /  LOAD A LICENSED SFZ BANK", juce::dontSendNotification);
}
void PopPunkGuitarUltimateAudioProcessorEditor::openFxEditPanel(int selectedEffect)
{
    if (auxiliaryWorkspace == nullptr)
        return;
    constexpr int fxEditPanelId = 2101;
    if (!auxiliaryWorkspace->isPanelOpen(fxEditPanelId))
    {
        PPGUMultiWindowWorkspace::PanelSpec spec;
        spec.id = fxEditPanelId;
        spec.title = "FX EDIT  /  SIGNAL FORGE";
        spec.desired = { 1000, 680 };
        spec.minimum = { 620, 440 };
        spec.content = std::make_unique<PPGUFXEditPanel>(processor.parameters, selectedEffect);
        if (!auxiliaryWorkspace->openPanel(std::move(spec)))
            return;
    }
    auxiliaryWorkspace->setVisible(true);
    auxiliaryWorkspace->toFront(false);
}

void PopPunkGuitarUltimateAudioProcessorEditor::openAutoChordsPanel()
{
    if (auxiliaryWorkspace == nullptr)
        return;
    constexpr int autoChordsPanelId = 2102;
    if (!auxiliaryWorkspace->isPanelOpen(autoChordsPanelId))
    {
        PPGUMultiWindowWorkspace::PanelSpec spec;
        spec.id = autoChordsPanelId;
        spec.title = "AUTOCHORDS  /  HARMONY WORKSPACE";
        spec.desired = { 900, 620 };
        spec.minimum = { 560, 420 };
        spec.content = std::make_unique<PPGUAutoChordsPanel>(processor.parameters);
        if (!auxiliaryWorkspace->openPanel(std::move(spec)))
            return;
    }
    auxiliaryWorkspace->setVisible(true);
    auxiliaryWorkspace->toFront(false);
}

void PopPunkGuitarUltimateAudioProcessorEditor::openImportGuitarPanel()
{
    if (auxiliaryWorkspace == nullptr)
        return;
    constexpr int importGuitarPanelId = 1001;
    if (!auxiliaryWorkspace->isPanelOpen(importGuitarPanelId))
    {
        juce::Component::SafePointer<PopPunkGuitarUltimateAudioProcessorEditor> safeThis(this);
        PPGUMultiWindowWorkspace::PanelSpec spec;
        spec.id = importGuitarPanelId;
        spec.title = "IMPORT GUITAR";
        spec.desired = { 560, 360 };
        spec.minimum = { 300, 220 };
        spec.content = std::make_unique<PPGUImportGuitarPanel>(
            [safeThis](juce::Label* targetStatus, juce::Button* targetButton)
            {
                if (safeThis != nullptr)
                    safeThis->chooseSfzFile(targetStatus, targetButton);
            });
        if (!auxiliaryWorkspace->openPanel(std::move(spec)))
            return;
    }
    auxiliaryWorkspace->setVisible(true);
    auxiliaryWorkspace->toFront(false);
    resized();
}

void PopPunkGuitarUltimateAudioProcessorEditor::chooseSfzFile(juce::Label* panelStatus, juce::Button* panelLoadButton)
{
    fileChooser=std::make_unique<juce::FileChooser>("Choose an SFZ guitar instrument",juce::File::getSpecialLocation(juce::File::userHomeDirectory),"*.sfz");
    juce::Component::SafePointer<PopPunkGuitarUltimateAudioProcessorEditor> safeThis(this);
    juce::Component::SafePointer<juce::Label> safePanelStatus(panelStatus);
    juce::Component::SafePointer<juce::Button> safePanelLoadButton(panelLoadButton);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safeThis, safePanelStatus, safePanelLoadButton](const juce::FileChooser& chooser){
        if(safeThis==nullptr) return; const auto selected=chooser.getResult(); if(selected==juce::File{}) return;
        safeThis->loadButton.setEnabled(false);
        if (safePanelLoadButton != nullptr)
            safePanelLoadButton->setEnabled(false);
        safeThis->statusLabel.setText("LOADING SFZ IN BACKGROUND...", juce::dontSendNotification);
        if (safePanelStatus != nullptr)
            safePanelStatus->setText("Loading SFZ in background...", juce::dontSendNotification);
        safeThis->processor.loadSfzFileAsync(selected, [safeThis, safePanelStatus, safePanelLoadButton](ppgu::sampler::LoadReport report)
        {
            if (safeThis == nullptr)
                return;
            safeThis->loadButton.setEnabled(true);
            if (safePanelLoadButton != nullptr)
                safePanelLoadButton->setEnabled(true);
            if (report.success)
            {
                const auto readyText = "READY  /  " + juce::String(static_cast<int>(report.loadedRegions))
                    + " REGIONS  /  " + juce::String(static_cast<int>(report.loadedSamples)) + " WAV SAMPLES";
                safeThis->statusLabel.setText(readyText, juce::dontSendNotification);
                if (safePanelStatus != nullptr)
                    safePanelStatus->setText(readyText, juce::dontSendNotification);
            }
            else
            {
                const auto errorText = report.errors.empty()
                    ? juce::String("SFZ LOAD FAILED") : juce::String(report.errors.front());
                safeThis->statusLabel.setText(errorText, juce::dontSendNotification);
                if (safePanelStatus != nullptr)
                    safePanelStatus->setText(errorText, juce::dontSendNotification);
            }
        });
    });
}
