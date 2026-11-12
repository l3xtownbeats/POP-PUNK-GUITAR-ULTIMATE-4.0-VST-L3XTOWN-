#include "ThemeManager.h"

#include <iostream>

namespace
{
    int failures = 0;

    void expect(bool condition, const juce::String& message)
    {
        if (!condition)
        {
            ++failures;
            std::cerr << "FAIL: " << message << std::endl;
        }
    }
}

int main()
{
    PPGUThemeManager manager;
    expect(manager.getPalette().id == "skin-51", "default theme ID");
    expect(manager.getPalette().usedFallback, "default palette is marked fallback");

    const auto root = juce::File::getCurrentWorkingDirectory();
    const auto skinRoot = root.getChildFile("SKIN");
    const auto registryFile = skinRoot.getChildFile("theme_manifest.json");
    const auto registry = juce::JSON::parse(registryFile.loadFileAsString());
    const auto themesValue = registry.getProperty("themes", juce::var());
    const auto* themes = themesValue.getArray();
    const auto totalValue = registry.getProperty("totalSkins", juce::var());
    expect(registry.isObject(), "skin registry is a JSON object");
    expect(themes != nullptr, "skin registry contains a themes array");
    expect(totalValue.isInt() || totalValue.isInt64(), "skin registry totalSkins is an integer");
    const auto skinRange = registry.getProperty("skinRange", juce::var());
    expect(static_cast<int>(skinRange.getProperty("first", 0)) == 1,
           "registry skinRange begins at 1");
    expect(static_cast<int>(skinRange.getProperty("last", 0)) == static_cast<int>(totalValue),
           "registry skinRange last matches totalSkins");

    if (themes != nullptr && (totalValue.isInt() || totalValue.isInt64()))
    {
        const auto total = totalValue.toString().getLargeIntValue();
        expect(total == static_cast<long long>(themes->size()),
               "registry totalSkins matches entry count");

        for (const auto& entry : *themes)
        {
            expect(entry.isObject(), "registry entry is an object");
            if (!entry.isObject())
                continue;

            const auto id = entry.getProperty("id", "").toString();
            const auto indexValue = entry.getProperty("index", juce::var());
            const auto relativePath = entry.getProperty("path", "").toString();
            expect(indexValue.isInt() || indexValue.isInt64(), id + " index is an integer");
            expect(relativePath.startsWith("Skin_") && relativePath.endsWith("/theme.json"),
                   id + " has a canonical relative manifest path");
            if ((!indexValue.isInt() && !indexValue.isInt64()) || relativePath.isEmpty())
                continue;

            const auto index = indexValue.toString().getLargeIntValue();
            const auto expectedId = juce::String("skin-")
                + juce::String(static_cast<juce::int64>(index)).paddedLeft('0', index <= 51 ? 2 : 3);
            expect(id == expectedId, id + " uses canonical ID formatting");
            const auto manifest = skinRoot.getChildFile(relativePath);
            const bool loaded = manifest.existsAsFile() && manager.loadFromFile(manifest);
            expect(loaded, id + " manifest accepted"
                   + (loaded ? juce::String{} : ": " + manager.getLastError()));
            if (loaded)
            {
                expect(manager.getPalette().id == id, id + " ID matches registry");
                expect(id.substring(5).getLargeIntValue() == index, id + " ID matches registry index");
                expect(!manager.getPalette().usedFallback, id + " does not use fallback");
            }
        }
    }

    const auto defaultManifest = skinRoot.getChildFile("Skin_51").getChildFile("theme.json");
    const auto validJson = defaultManifest.loadFileAsString();
    expect(validJson.isNotEmpty(), "Skin_51 fallback manifest readable");

    const auto skin52Manifest = skinRoot.getChildFile("Skin_052").getChildFile("theme.json");
    const auto skin52Json = skin52Manifest.loadFileAsString();
    expect(skin52Json.isNotEmpty(), "Skin_052 manifest readable");
    const bool skin52Loaded = manager.loadFromFile(skin52Manifest);
    expect(skin52Loaded, "Skin Studio 052 manifest accepted"
           + (skin52Loaded ? juce::String{} : ": " + manager.getLastError()));
    if (skin52Loaded)
    {
        expect(manager.getPalette().id == "skin-052", "Skin_052 ID matches three-digit index");
        expect(manager.getPalette().name == "Carbon Relay", "Skin_052 name is decoded");
        expect(!manager.getPalette().usedFallback, "Skin_052 does not use fallback");
        expect(manager.getPalette().canvas == juce::Colour(0xff0b0f12),
               "Skin_052 window background is mapped");
        const auto studioRoot = juce::JSON::parse(skin52Json);
        const auto studioControls = studioRoot.getProperty("controls", juce::var());
        const auto* controlObject = studioControls.getDynamicObject();
        const juce::StringArray requiredControlIds {
            "preset_browser", "mode_selector",
            "knob01", "knob02", "knob03", "knob04", "knob05",
            "knob06", "knob07", "knob08", "knob09",
            "fx_switch01", "fx_switch02", "fx_switch03", "fx_switch04",
            "fx_switch05", "fx_switch06", "fx_switch07",
            "midi_keyboard", "LCD_display", "LED_indicator", "SKIN_button",
            "Import_Guitar", "Auto_Discover", "Curator", "Settings", "About"
        };
        expect(controlObject != nullptr, "Skin_052 controls is a JSON object");
        if (controlObject != nullptr)
            for (const auto& controlId : requiredControlIds)
                expect(controlObject->hasProperty(juce::Identifier(controlId)),
                       juce::String("Skin_052 contains required control ID: ") + controlId);

        const auto studioTokens = juce::JSON::parse(skin52Json).getProperty("palette", juce::var());
        const juce::StringArray tokenNames {
            "window_bg", "panel", "panel_elevated", "control_bg", "control_face",
            "control_border", "text_primary", "text_secondary", "text_disabled",
            "accent", "accent_glow", "danger", "warning", "success", "lcd_bg",
            "lcd_text", "led_on", "led_off", "shadow", "highlight", "lowlight"
        };
        for (const auto& tokenName : tokenNames)
        {
            const auto expected = juce::Colour::fromString(
                "ff" + studioTokens.getProperty(tokenName, juce::var()).toString().substring(1));
            expect(manager.getPalette().token(tokenName) == expected,
                   "Skin_052 token is exposed exactly: " + tokenName);
        }
        expect(!manager.getPalette().token("unknown_token").isOpaque(),
               "unknown token does not silently alias a valid colour");
    }

    const auto badStudioColour = skin52Json.replace("#D5FF45", "#GG5FF45");
    expect(!manager.loadFromJson(badStudioColour), "invalid Skin Studio HEX token rejected");
    expect(manager.getPalette().id == "skin-51" && manager.getPalette().usedFallback,
           "invalid Skin Studio manifest restores default fallback");

    const auto badStudioCompatibility = skin52Json.replace("\"audioChanges\": false",
                                                            "\"audioChanges\": true");
    expect(!manager.loadFromJson(badStudioCompatibility),
           "audio-changing Skin Studio theme rejected");
    expect(manager.getPalette().usedFallback,
           "Skin Studio compatibility failure restores fallback");

    const auto badStudioId = skin52Json.replace("\"id\": \"skin-052\"",
                                                "\"id\": \"skin-000052\"");
    expect(!manager.loadFromJson(badStudioId),
           "non-canonical Skin Studio ID formatting is rejected");
    expect(manager.getPalette().usedFallback,
           "non-canonical Skin Studio ID restores fallback");

    const auto badStudioControls = skin52Json.replace("\"preset_browser\": {",
                                                      "\"preset_browser_removed\": {");
    expect(!manager.loadFromJson(badStudioControls),
           "missing required Skin Studio control ID is rejected");
    expect(manager.getPalette().usedFallback,
           "missing Skin Studio control restores fallback");

    expect(!manager.loadFromJson("{"), "malformed JSON rejected");
    expect(manager.getPalette().id == "skin-51" && manager.getPalette().usedFallback,
           "malformed JSON restores Blackout Voltage fallback");

    expect(!manager.loadFromJson("{}"), "empty object rejected");
    expect(manager.getPalette().id == "skin-51" && manager.getPalette().usedFallback,
           "incomplete manifest restores fallback");

    expect(manager.loadFromJson(validJson), "legacy skin remains loadable after token migration");
    expect(manager.getPalette().token("window_bg") == manager.getPalette().canvas,
           "legacy window background maps to canonical token");
    expect(manager.getPalette().token("accent") == manager.getPalette().accent,
           "legacy accent maps to canonical token");
    expect(manager.getPalette().token("danger").isOpaque(),
           "legacy semantic token receives a deterministic fallback");

    const auto badColour = validJson.replace("#090C10", "#GG0C10");
    expect(!manager.loadFromJson(badColour), "invalid legacy HEX colour rejected");
    expect(manager.getPalette().usedFallback, "invalid legacy HEX restores fallback");

    const auto badCompatibility = validJson.replace("\"audioChanges\": false",
                                                     "\"audioChanges\": true");
    expect(!manager.loadFromJson(badCompatibility), "legacy audio-changing theme rejected");
    expect(manager.getPalette().usedFallback, "legacy compatibility failure restores fallback");

    const auto mismatchedIndex = validJson.replace("\"index\": 51", "\"index\": 50");
    expect(!manager.loadFromJson(mismatchedIndex), "legacy ID/index mismatch rejected");
    expect(manager.getPalette().usedFallback, "legacy ID/index failure restores fallback");

    if (failures != 0)
    {
        std::cerr << failures << " theme manager test(s) failed." << std::endl;
        return 1;
    }

    std::cout << "ThemeManager contract tests passed: registry-driven manifests + fallback/error cases."
              << std::endl;
    return 0;
}
