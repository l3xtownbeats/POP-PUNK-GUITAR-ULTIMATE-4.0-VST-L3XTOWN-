#include "ThemeManager.h"

namespace
{
    bool isAsciiHexDigit(juce_wchar character) noexcept
    {
        return (character >= '0' && character <= '9')
            || (character >= 'a' && character <= 'f')
            || (character >= 'A' && character <= 'F');
    }

    bool isHexColour(const juce::var& value)
    {
        const auto text = value.toString();
        if (text.length() != 7 || text[0] != '#')
            return false;

        for (int i = 1; i < text.length(); ++i)
            if (!isAsciiHexDigit(text[i]))
                return false;

        return true;
    }

    bool isNumber(const juce::var& value)
    {
        return value.isInt() || value.isInt64() || value.isDouble();
    }

    bool isNonEmptyString(const juce::var& value)
    {
        return value.isString() && value.toString().trim().isNotEmpty();
    }

    bool hasObject(const juce::var& parent, const juce::Identifier& key, juce::var& result)
    {
        result = parent.getProperty(key, juce::var());
        return result.isObject();
    }

    bool hasStringProperty(const juce::var& parent, const juce::Identifier& key)
    {
        return isNonEmptyString(parent.getProperty(key, juce::var()));
    }

    bool validateGroup(const juce::var& parent,
                       const juce::Identifier& groupName,
                       std::initializer_list<const char*> requiredKeys,
                       std::initializer_list<const char*> colourKeys,
                       juce::String& error)
    {
        juce::var group;
        if (!hasObject(parent, groupName, group))
        {
            error = "Missing token group: " + groupName.toString();
            return false;
        }

        for (const auto* key : requiredKeys)
        {
            const auto value = group.getProperty(key, juce::var());
            if (value.isVoid())
            {
                error = "Missing token: " + groupName.toString() + "." + key;
                return false;
            }
        }

        for (const auto* key : colourKeys)
        {
            if (!isHexColour(group.getProperty(key, juce::var())))
            {
                error = "Invalid HEX colour: " + groupName.toString() + "." + key;
                return false;
            }
        }

        return true;
    }

    bool validateButtonStyle(const juce::var& buttons,
                             const juce::Identifier& styleName,
                             juce::String& error)
    {
        juce::var style;
        if (!hasObject(buttons, styleName, style))
        {
            error = "Missing button style: " + styleName.toString();
            return false;
        }

        const juce::StringArray keys {
            "background", "foreground", "hover", "pressed", "disabled"
        };
        for (const auto& key : keys)
        {
            if (!isHexColour(style.getProperty(key, juce::var())))
            {
                error = "Invalid button colour: " + styleName.toString() + "." + key;
                return false;
            }
        }
        return true;
    }

    bool validateAssetSpec(const juce::var& assetSpec,
                           const juce::Identifier& name,
                           int width, int height, const juce::String& format,
                           juce::String& error)
    {
        juce::var spec;
        if (!hasObject(assetSpec, name, spec))
        {
            error = "Missing asset specification: " + name.toString();
            return false;
        }

        const auto widthValue = spec.getProperty("width", juce::var());
        const auto heightValue = spec.getProperty("height", juce::var());
        if ((!widthValue.isInt() && !widthValue.isInt64())
            || (!heightValue.isInt() && !heightValue.isInt64())
            || static_cast<int>(widthValue) != width
            || static_cast<int>(heightValue) != height
            || spec.getProperty("format", "").toString() != format)
        {
            error = "Invalid asset dimensions or format: " + name.toString();
            return false;
        }

        return true;
    }

    bool validateSkinStudioManifest(const juce::var& root,
                                    PPGUThemeManager::Palette& palette,
                                    juce::String& error)
    {
        if (root.getProperty("schemaVersion", "").toString() != "1.0.0")
        {
            error = "Unsupported Skin Studio schema version";
            return false;
        }

        const auto id = root.getProperty("id", "").toString();
        const auto indexValue = root.getProperty("index", juce::var());
        const auto index = indexValue.toString().getLargeIntValue();
        if ((!indexValue.isInt() && !indexValue.isInt64())
            || id.length() != 8 || !id.startsWith("skin-")
            || id.substring(5).length() != 3
            || !juce::CharacterFunctions::isDigit(id[5])
            || !juce::CharacterFunctions::isDigit(id[6])
            || !juce::CharacterFunctions::isDigit(id[7])
            || id.substring(5).getIntValue() != index
            || index < 52 || index > 999)
        {
            error = "Invalid Skin Studio ID/index pair";
            return false;
        }

        const auto name = root.getProperty("name", "").toString().trim();
        if (name.isEmpty())
        {
            error = "Skin Studio theme name is missing";
            return false;
        }

        const auto family = root.getProperty("family", "").toString();
        if (family != "carbon_studio" && family != "neon_basement"
            && family != "chrome_parade" && family != "wood_summer"
            && family != "vintage_tape" && family != "sunset_suburb"
            && family != "special")
        {
            error = "Invalid Skin Studio theme family";
            return false;
        }

        const auto layout = root.getProperty("layout", "").toString();
        if (layout != "STUDIO_CONSOLE" && layout != "WORKSTATION_TOPBAR"
            && layout != "RACK_SIDE" && layout != "COMPACT_MINI")
        {
            error = "Invalid Skin Studio layout";
            return false;
        }

        juce::var paletteTokens;
        if (!hasObject(root, "palette", paletteTokens))
        {
            error = "Skin Studio palette object is missing";
            return false;
        }

        const juce::StringArray requiredColours {
            "window_bg", "panel", "panel_elevated", "control_bg", "control_face",
            "control_border", "text_primary", "text_secondary", "text_disabled",
            "accent", "accent_glow", "danger", "warning", "success", "lcd_bg",
            "lcd_text", "led_on", "led_off", "shadow", "highlight", "lowlight"
        };
        const auto* paletteObject = paletteTokens.getDynamicObject();
        if (paletteObject == nullptr || paletteObject->getProperties().size() != requiredColours.size())
        {
            error = "Skin Studio palette must contain exactly 21 tokens";
            return false;
        }

        for (const auto& key : requiredColours)
        {
            if (!isHexColour(paletteTokens.getProperty(key, juce::var())))
            {
                error = "Invalid Skin Studio HEX token: " + key;
                return false;
            }
        }

        juce::var controls;
        if (!hasObject(root, "controls", controls)
            || controls.getDynamicObject() == nullptr
            || controls.getDynamicObject()->getProperties().size() < 27)
        {
            error = "Skin Studio control map is incomplete";
            return false;
        }

        juce::var assets;
        if (!hasObject(root, "assets", assets)
            || assets.getProperty("preview", "").toString() != "assets/preview.png"
            || assets.getProperty("hero", "").toString() != "assets/hero.png"
            || !hasStringProperty(assets, "texture")
            || !hasStringProperty(assets, "bezel")
            || !hasStringProperty(assets, "crest"))
        {
            error = "Skin Studio asset paths are incomplete";
            return false;
        }

        juce::var compatibility;
        if (!hasObject(root, "compatibility", compatibility)
            || static_cast<bool>(compatibility.getProperty("apvtsChanges", true))
            || static_cast<bool>(compatibility.getProperty("layoutChanges", true))
            || static_cast<bool>(compatibility.getProperty("audioChanges", true))
            || !root.getProperty("reduceInEco", juce::var()).isBool()
            || !static_cast<bool>(root.getProperty("reduceInEco", false))
            || !root.getProperty("hotSwapFadeMs", juce::var()).isInt()
            || static_cast<int>(root.getProperty("hotSwapFadeMs", 0)) != 150)
        {
            error = "Skin Studio visual-only/ECO/hot-swap contract is invalid";
            return false;
        }

        palette = PPGUThemeManager::Palette{};
        palette.id = id;
        palette.name = name;
        palette.canvas = juce::Colour::fromString("ff" + paletteTokens.getProperty("window_bg").toString().substring(1));
        palette.canvasInset = juce::Colour::fromString("ff" + paletteTokens.getProperty("control_bg").toString().substring(1));
        palette.surface = juce::Colour::fromString("ff" + paletteTokens.getProperty("panel").toString().substring(1));
        palette.surfaceSecondary = juce::Colour::fromString("ff" + paletteTokens.getProperty("control_bg").toString().substring(1));
        palette.surfaceRaised = juce::Colour::fromString("ff" + paletteTokens.getProperty("panel_elevated").toString().substring(1));
        palette.text = juce::Colour::fromString("ff" + paletteTokens.getProperty("text_primary").toString().substring(1));
        palette.textSecondary = juce::Colour::fromString("ff" + paletteTokens.getProperty("text_secondary").toString().substring(1));
        palette.accent = juce::Colour::fromString("ff" + paletteTokens.getProperty("accent").toString().substring(1));
        palette.accentSecondary = juce::Colour::fromString("ff" + paletteTokens.getProperty("accent_glow").toString().substring(1));
        palette.border = juce::Colour::fromString("ff" + paletteTokens.getProperty("control_border").toString().substring(1));
        palette.focus = palette.accentSecondary;
        palette.usedFallback = false;
        return true;
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

    destination = juce::Colour::fromString("ff" + value.toString().substring(1));
    return destination.isOpaque();
}

bool PPGUThemeManager::validateManifest(const juce::var& root,
                                        Palette& palette,
                                        juce::String& error)
{
    if (!root.isObject())
    {
        error = "Theme manifest is not a JSON object";
        return false;
    }

    if (root.getProperty("schemaVersion", "").toString() != "1.0.0")
    {
        error = "Unsupported theme schema version";
        return false;
    }

    const auto id = root.getProperty("id", "").toString();
    const auto indexValue = root.getProperty("index", juce::var());
    const int index = static_cast<int>(indexValue);
    if (!indexValue.isInt() && !indexValue.isInt64())
    {
        error = "Theme index must be an integer";
        return false;
    }
    if (id.length() == 8)
        return validateSkinStudioManifest(root, palette, error);

    if (id.length() != 7 || !id.startsWith("skin-")
        || !juce::CharacterFunctions::isDigit(id[5])
        || !juce::CharacterFunctions::isDigit(id[6])
        || index < 1 || index > 51
        || id.substring(5).getIntValue() != index)
    {
        error = "Invalid theme ID/index pair";
        return false;
    }

    const auto name = root.getProperty("name", "").toString().trim();
    if (name.isEmpty() || !root.getProperty("description", juce::var()).isString())
    {
        error = "Theme name or description is invalid";
        return false;
    }

    const auto cluster = root.getProperty("cluster", "").toString();
    if (cluster != "A" && cluster != "B" && cluster != "C")
    {
        error = "Invalid theme cluster";
        return false;
    }

    juce::var identity;
    if (!hasObject(root, "identity", identity)
        || !hasStringProperty(identity, "material")
        || !hasStringProperty(identity, "lighting")
        || !hasStringProperty(identity, "mood"))
    {
        error = "Theme identity is incomplete";
        return false;
    }

    juce::var tokens;
    if (!hasObject(root, "tokens", tokens))
    {
        error = "Theme tokens object is missing";
        return false;
    }

    if (!validateGroup(tokens, "canvas", { "base", "inset" }, { "base", "inset" }, error)
        || !validateGroup(tokens, "surfaces", { "primary", "secondary", "raised", "overlay" }, { "primary", "secondary", "raised", "overlay" }, error)
        || !validateGroup(tokens, "borders", { "subtle", "strong", "focus" }, { "subtle", "strong", "focus" }, error)
        || !validateGroup(tokens, "typography", { "primary", "secondary", "disabled", "onAccent" }, { "primary", "secondary", "disabled", "onAccent" }, error)
        || !validateGroup(tokens, "rotary", { "body", "ring", "track", "value", "pointer", "disabled" }, { "body", "ring", "track", "value", "pointer", "disabled" }, error)
        || !validateGroup(tokens, "sliders", { "track", "fill", "thumb", "disabled" }, { "track", "fill", "thumb", "disabled" }, error)
        || !validateGroup(tokens, "tabs", { "activeBackground", "activeText", "inactiveBackground", "inactiveText", "indicator" }, { "activeBackground", "activeText", "inactiveBackground", "inactiveText", "indicator" }, error)
        || !validateGroup(tokens, "comboBoxes", { "background", "foreground", "border", "hover", "menuBackground", "selection" }, { "background", "foreground", "border", "hover", "menuBackground", "selection" }, error)
        || !validateGroup(tokens, "keyboard", { "whiteKey", "blackKey", "whitePressed", "blackPressed", "keyBorder", "octaveLabel" }, { "whiteKey", "blackKey", "whitePressed", "blackPressed", "keyBorder", "octaveLabel" }, error)
        || !validateGroup(tokens, "meters", { "low", "medium", "high", "background" }, { "low", "medium", "high", "background" }, error)
        || !validateGroup(tokens, "icons", { "primary", "secondary", "accent", "disabled" }, { "primary", "secondary", "accent", "disabled" }, error)
        || !validateGroup(tokens, "thumbnails", { "background", "border", "selected", "caption" }, { "background", "border", "selected", "caption" }, error)
        || !validateGroup(tokens, "windowChrome", { "background", "title", "border", "close" }, { "background", "title", "border", "close" }, error)
        || !validateGroup(tokens, "hero", { "background", "accent", "secondaryAccent", "highlight" }, { "background", "accent", "secondaryAccent", "highlight" }, error)
        || !validateGroup(tokens, "accents", { "primary", "secondary", "tertiary" }, { "primary", "secondary", "tertiary" }, error)
        || !validateGroup(tokens, "lighting", { "profile", "glow", "glowIntensity", "edgeHighlight" }, { "glow", "edgeHighlight" }, error))
        return false;

    juce::var buttons;
    if (!hasObject(tokens, "buttons", buttons)
        || !validateButtonStyle(buttons, "primary", error)
        || !validateButtonStyle(buttons, "secondary", error))
    {
        if (error.isEmpty())
            error = "Button token group is incomplete";
        return false;
    }

    juce::var texture;
    if (!hasObject(tokens, "texture", texture)
        || !hasStringProperty(texture, "material")
        || !texture.getProperty("enabled", juce::var()).isBool()
        || !isNumber(texture.getProperty("intensity", juce::var()))
        || static_cast<double>(texture.getProperty("intensity", 0.0)) < 0.0
        || static_cast<double>(texture.getProperty("intensity", 0.0)) > 1.0)
    {
        error = "Texture token group is invalid";
        return false;
    }

    const auto* textureObject = texture.getDynamicObject();
    if (textureObject == nullptr || !textureObject->hasProperty("asset"))
    {
        error = "Texture asset property is missing";
        return false;
    }
    const auto textureAsset = texture.getProperty("asset", juce::var());
    if (!textureAsset.isString() && !textureAsset.isVoid())
    {
        error = "Texture asset must be a string or null";
        return false;
    }

    juce::var lighting;
    if (!hasObject(tokens, "lighting", lighting)
        || !hasStringProperty(lighting, "profile")
        || !isNumber(lighting.getProperty("glowIntensity", juce::var()))
        || static_cast<double>(lighting.getProperty("glowIntensity", 0.0)) < 0.0
        || static_cast<double>(lighting.getProperty("glowIntensity", 0.0)) > 1.0)
    {
        error = "Lighting intensity/profile is invalid";
        return false;
    }

    juce::var assets;
    if (!hasObject(root, "assets", assets)
        || !hasStringProperty(assets, "preview")
        || !hasStringProperty(assets, "hero"))
    {
        error = "Theme asset paths are incomplete";
        return false;
    }

    const auto optionalTexturePath = assets.getProperty("texture", juce::var());
    if (!optionalTexturePath.isVoid() && !optionalTexturePath.isString())
    {
        error = "Theme texture path must be a string or null";
        return false;
    }

    juce::var assetSpec;
    if (!hasObject(root, "assetSpec", assetSpec)
        || !validateAssetSpec(assetSpec, "preview", 480, 300, "png", error)
        || !validateAssetSpec(assetSpec, "hero", 1200, 420, "webp", error))
        return false;

    juce::var compatibility;
    if (!hasObject(root, "compatibility", compatibility)
        || !hasStringProperty(compatibility, "minimumPluginVersion")
        || static_cast<bool>(compatibility.getProperty("apvtsChanges", true))
        || static_cast<bool>(compatibility.getProperty("layoutChanges", true))
        || static_cast<bool>(compatibility.getProperty("audioChanges", true)))
    {
        error = "Theme violates the visual-only compatibility contract";
        return false;
    }

    juce::var validation;
    if (!hasObject(root, "validation", validation)
        || !hasStringProperty(validation, "json")
        || !hasStringProperty(validation, "assets")
        || !hasStringProperty(validation, "runtime")
        || !hasStringProperty(validation, "flStudio20"))
    {
        error = "Theme validation metadata is incomplete";
        return false;
    }

    palette = Palette{};
    palette.id = id;
    palette.name = name;
    palette.usedFallback = false;

    if (!readColour(tokens.getProperty("canvas", juce::var()), "base", palette.canvas)
        || !readColour(tokens.getProperty("canvas", juce::var()), "inset", palette.canvasInset)
        || !readColour(tokens.getProperty("surfaces", juce::var()), "primary", palette.surface)
        || !readColour(tokens.getProperty("surfaces", juce::var()), "secondary", palette.surfaceSecondary)
        || !readColour(tokens.getProperty("surfaces", juce::var()), "raised", palette.surfaceRaised)
        || !readColour(tokens.getProperty("typography", juce::var()), "primary", palette.text)
        || !readColour(tokens.getProperty("typography", juce::var()), "secondary", palette.textSecondary)
        || !readColour(tokens.getProperty("accents", juce::var()), "primary", palette.accent)
        || !readColour(tokens.getProperty("accents", juce::var()), "secondary", palette.accentSecondary)
        || !readColour(tokens.getProperty("borders", juce::var()), "strong", palette.border)
        || !readColour(tokens.getProperty("borders", juce::var()), "focus", palette.focus))
    {
        error = "One or more palette colours could not be decoded";
        return false;
    }

    return true;
}

bool PPGUThemeManager::loadFromJson(const juce::String& jsonText)
{
    const auto root = juce::JSON::parse(jsonText);
    Palette candidate;
    juce::String error;

    if (!validateManifest(root, candidate, error))
    {
        lastError = error.isNotEmpty() ? error : "Theme manifest validation failed";
        resetToDefault();
        return false;
    }

    activePalette = candidate;
    lastError.clear();
    return true;
}

bool PPGUThemeManager::loadFromFile(const juce::File& manifestFile)
{
    if (!manifestFile.existsAsFile())
    {
        lastError = "Theme manifest file does not exist";
        resetToDefault();
        return false;
    }

    return loadFromJson(manifestFile.loadFileAsString());
}

void PPGUThemeManager::resetToDefault() noexcept
{
    activePalette = Palette{};
}
