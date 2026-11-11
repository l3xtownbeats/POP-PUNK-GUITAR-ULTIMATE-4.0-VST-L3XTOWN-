#include "ThemeManager.h"

namespace
{
    const juce::StringArray requiredGroups {
        "canvas", "surfaces", "borders", "typography", "rotary", "sliders",
        "buttons", "tabs", "comboBoxes", "keyboard", "meters", "icons",
        "thumbnails", "windowChrome", "hero", "accents", "texture", "lighting"
    };

    bool isHexColour(const juce::var& value)
    {
        const auto text = value.toString();
        if (text.length() != 7 || text[0] != '#')
            return false;

        for (int i = 1; i < text.length(); ++i)
            if (!juce::CharacterFunctions::isHexDigit(text[i]))
                return false;

        return true;
    }

    juce::Colour parseHexColour(const juce::String& value)
    {
        return juce::Colour::fromString("ff" + value.substring(1));
    }
}

bool PPGUThemeManager::readColour(const juce::var& object,
                                  const juce::Identifier& key,
                                  juce::Colour& destination)
{
    if (!object.isObject())
        return false;

    const auto value = object.getProperty(key, juce::var());
    if (!isHexColour(value))
        return false;

    destination = parseHexColour(value.toString());
    return destination.isOpaque();
}

bool PPGUThemeManager::hasRequiredTokenGroups(const juce::var& tokens)
{
    if (!tokens.isObject())
        return false;

    for (const auto& group : requiredGroups)
        if (!tokens.getProperty(group, juce::var()).isObject())
            return false;

    return true;
}

bool PPGUThemeManager::loadFromJson(const juce::String& jsonText)
{
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());

    const auto root = juce::JSON::parse(jsonText);
    const auto fail = [this](const juce::String& reason)
    {
        lastError = reason;
        resetToDefault();
        return false;
    };

    if (!root.isObject())
        return fail("Theme manifest is not a JSON object");

    if (root.getProperty("schemaVersion", "").toString() != "1.0.0")
        return fail("Unsupported theme schema version");

    const auto id = root.getProperty("id", "").toString();
    const int index = static_cast<int>(root.getProperty("index", 0));
    const auto name = root.getProperty("name", "").toString().trim();

    if (!id.startsWith("skin-") || id.length() != 7 || index < 1 || index > 50 || name.isEmpty())
        return fail("Invalid theme identity");

    const auto tokens = root.getProperty("tokens", juce::var());
    if (!hasRequiredTokenGroups(tokens))
        return fail("Theme is missing one or more required token groups");

    const auto compatibility = root.getProperty("compatibility", juce::var());
    if (!compatibility.isObject()
        || static_cast<bool>(compatibility.getProperty("apvtsChanges", true))
        || static_cast<bool>(compatibility.getProperty("layoutChanges", true))
        || static_cast<bool>(compatibility.getProperty("audioChanges", true)))
        return fail("Theme violates the visual-only compatibility contract");

    Palette candidate;
    candidate.id = id;
    candidate.name = name;
    candidate.usedFallback = false;

    if (!readColour(tokens.getProperty("canvas", juce::var()), "base", candidate.canvas)
        || !readColour(tokens.getProperty("surfaces", juce::var()), "primary", candidate.surface)
        || !readColour(tokens.getProperty("surfaces", juce::var()), "secondary", candidate.surfaceSecondary)
        || !readColour(tokens.getProperty("typography", juce::var()), "primary", candidate.text)
        || !readColour(tokens.getProperty("typography", juce::var()), "secondary", candidate.textSecondary)
        || !readColour(tokens.getProperty("accents", juce::var()), "primary", candidate.accent)
        || !readColour(tokens.getProperty("accents", juce::var()), "secondary", candidate.accentSecondary)
        || !readColour(tokens.getProperty("borders", juce::var()), "strong", candidate.border))
        return fail("Theme contains an invalid required colour token");

    activePalette = candidate;
    lastError.clear();
    return true;
}

void PPGUThemeManager::resetToDefault() noexcept
{
    activePalette = Palette{};
}
