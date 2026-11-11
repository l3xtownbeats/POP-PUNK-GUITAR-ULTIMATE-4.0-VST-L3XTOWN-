#include "ThemeManager.h"
#include <cmath>
#include <algorithm>

namespace ppgu
{
namespace
{
juce::Colour parseThemeColour (juce::String value, juce::Colour fallback)
{
    value = value.trim().removeCharacters("#");
    if (value.length() == 6) value = "ff" + value;
    if (value.length() != 8) return fallback;
    return juce::Colour::fromString("0x" + value);
}

juce::var propertyOrEmpty (const juce::var& object, const juce::Identifier& key)
{
    return object.isObject() ? object.getProperty(key, juce::var()) : juce::var();
}
}

juce::Colour ThemePalette::get (const juce::String& key, juce::Colour fallback) const
{
    const auto it = colours.find(key);
    return it != colours.end() ? it->second : fallback;
}

bool ThemeManager::loadCatalogText (const juce::String& jsonText, const juce::File& assetRoot)
{
    themes.clear();
    currentTheme = nullptr;

    const auto root = juce::JSON::parse(jsonText);
    if (! root.isObject()) return false;

    defaultId = root.getProperty("default_theme_id", "neon-riot").toString();
    const auto themeArray = root.getProperty("themes", juce::var());
    const auto* entries = themeArray.getArray();
    if (entries == nullptr) return false;

    for (const auto& entry : *entries)
    {
        if (! entry.isObject()) continue;
        auto theme = std::make_unique<Theme>();
        theme->id = entry.getProperty("id", "").toString().trim();
        theme->name = entry.getProperty("name", theme->id).toString();
        theme->author = entry.getProperty("author", "Unknown").toString();
        theme->version = entry.getProperty("version", "1.0").toString();
        if (theme->id.isEmpty()) continue;

        const auto tags = propertyOrEmpty(entry, "style_tags");
        if (const auto* tagArray = tags.getArray())
            for (const auto& tag : *tagArray) theme->styleTags.add(tag.toString());

        // Accept both the original catalog schema ("palette") and the shipped
        // per-folder schema ("colors").
        auto palette = propertyOrEmpty(entry, "palette");
        if (! palette.isObject()) palette = propertyOrEmpty(entry, "colors");
        if (const auto* object = palette.getDynamicObject())
            for (const auto& item : object->getProperties())
                theme->palette.colours[item.name.toString()] =
                    parseThemeColour(item.value.toString(), juce::Colours::transparentBlack);

        theme->rootDir = assetRoot.getChildFile(theme->id);
        const auto assets = propertyOrEmpty(entry, "assets");
        if (const auto* object = assets.getDynamicObject())
            for (const auto& item : object->getProperties())
            {
                const auto relative = item.value.toString();
                if (relative.isEmpty() || juce::File::isAbsolutePath(relative)
                    || relative.contains("..")) continue;
                const auto file = theme->rootDir.getChildFile(relative);
                if (file.existsAsFile()) theme->assets[item.name.toString()] = file;
            }

        const auto fonts = propertyOrEmpty(entry, "fonts");
        if (const auto* object = fonts.getDynamicObject())
            for (const auto& item : object->getProperties())
                theme->fonts[item.name.toString()] = item.value.toString();

        themes.push_back(std::move(theme));
    }

    if (themes.empty()) return false;
    applyTheme(defaultId);
    if (currentTheme == nullptr) applyTheme(0);
    return true;
}

bool ThemeManager::loadFromDirectory (const juce::File& themesDirectory)
{
    themes.clear();
    currentTheme = nullptr;
    if (! themesDirectory.isDirectory()) return false;

    const auto folders = themesDirectory.findChildFiles(juce::File::findDirectories, false);
    for (const auto& folder : folders)
    {
        const auto config = folder.getChildFile("theme.json");
        if (! config.existsAsFile()) continue;

        const auto parsed = juce::JSON::parse(config.loadFileAsString());
        if (! parsed.isObject()) continue;

        auto theme = std::make_unique<Theme>();
        theme->id = folder.getFileName();
        theme->name = parsed.getProperty("name", theme->id).toString();
        theme->author = parsed.getProperty("author", "Unknown").toString();
        theme->version = parsed.getProperty("version", "1.0").toString();
        theme->rootDir = folder;

        auto colors = propertyOrEmpty(parsed, "colors");
        if (! colors.isObject()) colors = propertyOrEmpty(parsed, "palette");
        if (const auto* object = colors.getDynamicObject())
            for (const auto& item : object->getProperties())
                theme->palette.colours[item.name.toString()] =
                    parseThemeColour(item.value.toString(), juce::Colours::transparentBlack);

        const auto assets = propertyOrEmpty(parsed, "assets");
        if (const auto* object = assets.getDynamicObject())
            for (const auto& item : object->getProperties())
            {
                const auto relative = item.value.toString();
                if (relative.isEmpty() || juce::File::isAbsolutePath(relative)
                    || relative.contains("..")) continue;
                const auto file = folder.getChildFile(relative);
                if (file.existsAsFile()) theme->assets[item.name.toString()] = file;
            }

        const auto fonts = propertyOrEmpty(parsed, "fonts");
        if (const auto* object = fonts.getDynamicObject())
            for (const auto& item : object->getProperties())
                theme->fonts[item.name.toString()] = item.value.toString();

        if (! theme->palette.colours.empty())
            themes.push_back(std::move(theme));
    }

    std::sort(themes.begin(), themes.end(), [](const auto& a, const auto& b)
    {
        return a->name.compareIgnoreCase(b->name) < 0;
    });
    if (themes.empty()) return false;

    defaultId = "neon-riot";
    applyTheme(defaultId);
    if (currentTheme == nullptr) applyTheme(0);
    return true;
}

int ThemeManager::currentIndex() const noexcept
{
    return currentTheme == nullptr ? -1 : indexOf(currentTheme->id);
}

int ThemeManager::indexOf (const juce::String& id) const noexcept
{
    for (size_t i = 0; i < themes.size(); ++i)
        if (themes[i]->id == id) return static_cast<int>(i);
    return -1;
}

void ThemeManager::applyTheme (const juce::String& id)
{
    const auto index = indexOf(id);
    applyTheme(index >= 0 ? index : indexOf(defaultId));
}

void ThemeManager::applyTheme (int index)
{
    if (themes.empty()) return;
    if (index < 0 || index >= static_cast<int>(themes.size())) index = 0;
    currentTheme = themes[static_cast<size_t>(index)].get();
    triggerAsyncUpdate();
}

void ThemeManager::handleAsyncUpdate()
{
    if (onThemeChanged) onThemeChanged();
}

juce::Colour PpguLookAndFeel::colour (const juce::String& key, juce::Colour fallback) const
{
    const auto* theme = manager != nullptr ? manager->current() : nullptr;
    return theme != nullptr ? theme->palette.get(key, fallback) : fallback;
}

void PpguLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                                         float sliderPos, float startAngle, float endAngle,
                                         juce::Slider&)
{
    const auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)w, (float)h).reduced(3.0f);
    const auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();
    const auto angle = startAngle + sliderPos * (endAngle - startAngle);
    const auto face = juce::Rectangle<float>(radius * 1.7f, radius * 1.7f).withCentre(centre);

    g.setColour(colour("knob.face", juce::Colour(0xff292932)));
    g.fillEllipse(face);
    g.setColour(colour("panel.border", juce::Colour(0xff454550)));
    g.drawEllipse(face, 1.0f);

    juce::Path arc;
    arc.addCentredArc(centre.x, centre.y, radius * 0.88f, radius * 0.88f, 0.0f,
                      startAngle, angle, true);
    g.setColour(colour("knob.ring", colour("accent.primary", juce::Colours::hotpink)));
    g.strokePath(arc, juce::PathStrokeType(3.0f));

    const auto pointerEnd = juce::Point<float>(
        centre.x + std::cos(angle - juce::MathConstants<float>::halfPi) * radius * 0.66f,
        centre.y + std::sin(angle - juce::MathConstants<float>::halfPi) * radius * 0.66f);
    g.setColour(colour("knob.pointer", juce::Colours::white));
    g.drawLine(centre.x, centre.y, pointerEnd.x, pointerEnd.y, 2.0f);
}

void PpguLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                             const juce::Colour&, bool highlighted, bool down)
{
    const auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    const auto background = button.getToggleState()
        ? colour("button.active", colour("accent.primary", juce::Colours::hotpink))
        : (down || highlighted ? colour("button.hover", juce::Colour(0xff34343d))
                               : colour("button.normal", juce::Colour(0xff25252b)));
    g.setColour(background);
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(colour("panel.border", juce::Colour(0xff41414a)));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
}

void PpguLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h,
                                         float sliderPos, float, float,
                                         juce::Slider::SliderStyle style, juce::Slider&)
{
    if (style != juce::Slider::LinearVertical && style != juce::Slider::LinearHorizontal)
        return;
    const bool vertical = style == juce::Slider::LinearVertical;
    const auto track = vertical
        ? juce::Rectangle<float>((float)x + (float)w * 0.5f - 3.0f, (float)y, 6.0f, (float)h)
        : juce::Rectangle<float>((float)x, (float)y + (float)h * 0.5f - 3.0f, (float)w, 6.0f);
    g.setColour(colour("slider.track", juce::Colour(0xff33333a)));
    g.fillRoundedRectangle(track, 3.0f);
    auto fill = track;
    if (vertical) fill = fill.withTop(sliderPos);
    else fill = fill.withRight(sliderPos);
    g.setColour(colour("slider.fill", colour("accent.primary", juce::Colours::hotpink)));
    g.fillRoundedRectangle(fill, 3.0f);
}
} // namespace ppgu
