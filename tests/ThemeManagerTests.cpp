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
    expect(manager.getPalette().id == "skin-053", "default theme ID is NEON STAGE");
    expect(manager.getPalette().usedFallback, "default palette is marked fallback");
    expect(manager.getPalette().name == "NEON STAGE", "fallback identity matches the canonical NEON STAGE theme");
    const auto& fallbackTokens = manager.getPalette().tokens;
    expect(fallbackTokens.windowBg == juce::Colour(0xff050811), "fallback window background matches Skin_053");
    expect(fallbackTokens.panel == juce::Colour(0xff091321), "fallback panel matches Skin_053");
    expect(fallbackTokens.panelElevated == juce::Colour(0xff101f32), "fallback elevated panel matches Skin_053");
    expect(fallbackTokens.controlBg == juce::Colour(0xff07101c), "fallback control background matches Skin_053");
    expect(fallbackTokens.controlFace == juce::Colour(0xff14263a), "fallback control face matches Skin_053");
    expect(fallbackTokens.controlBorder == juce::Colour(0xff52718b), "fallback control border matches Skin_053");
    expect(fallbackTokens.textPrimary == juce::Colour(0xffffffff), "fallback primary text matches Skin_053");
    expect(fallbackTokens.textSecondary == juce::Colour(0xffd9e8f7), "fallback secondary text matches Skin_053");
    expect(fallbackTokens.textDisabled == juce::Colour(0xffaabbd0), "fallback disabled text matches Skin_053");
    expect(fallbackTokens.accent == juce::Colour(0xff66ffff), "fallback accent matches Skin_053");
    expect(fallbackTokens.accentGlow == juce::Colour(0xffb8ffff), "fallback accent glow matches Skin_053");
    expect(fallbackTokens.danger == juce::Colour(0xffff5577), "fallback danger matches Skin_053");
    expect(fallbackTokens.warning == juce::Colour(0xffffd166), "fallback warning matches Skin_053");
    expect(fallbackTokens.success == juce::Colour(0xff67f5b5), "fallback success matches Skin_053");
    expect(fallbackTokens.lcdBg == juce::Colour(0xff020a12), "fallback LCD background matches Skin_053");
    expect(fallbackTokens.lcdText == juce::Colour(0xffc9ffff), "fallback LCD text matches Skin_053");
    expect(fallbackTokens.ledOn == juce::Colour(0xff66ffff), "fallback LED on matches Skin_053");
    expect(fallbackTokens.ledOff == juce::Colour(0xff263c50), "fallback LED off matches Skin_053");
    expect(fallbackTokens.shadow == juce::Colour(0xff02040a), "fallback shadow matches Skin_053");
    expect(fallbackTokens.highlight == juce::Colour(0xffb8d9f0), "fallback highlight matches Skin_053");
    expect(fallbackTokens.lowlight == juce::Colour(0xff0a1421), "fallback lowlight matches Skin_053");
    expect(manager.getPalette().focus == fallbackTokens.highlight, "fallback focus alias matches canonical highlight token");


    const auto root = juce::File::getCurrentWorkingDirectory();
    const auto skinRoot = root.getChildFile("SKIN");
    const auto registryFile = skinRoot.getChildFile("theme_manifest.json");
    const auto registry = juce::JSON::parse(registryFile.loadFileAsString());
    const auto themesValue = registry.getProperty("themes", juce::var());
    const auto* themes = themesValue.getArray();
    const auto totalValue = registry.getProperty("totalSkins", juce::var());
    expect(registry.isObject(), "skin registry is a JSON object");
    expect(registry.getProperty("defaultSkin", "").toString() == "skin-053",
           "registry default points to NEON STAGE");
    expect(themes != nullptr, "skin registry contains a themes array");
    expect(totalValue.isInt() || totalValue.isInt64(), "skin registry totalSkins is an integer");

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
    expect(validJson.isNotEmpty(), "legacy Skin_51 manifest readable");

    const auto wrappedWidth = validJson.replace("\"width\": 480",
                                                "\"width\": 4294967776");
    expect(!manager.loadFromJson(wrappedWidth),
           "int64 asset width that wraps to 480 after narrowing is rejected");
    expect(manager.getPalette().usedFallback,
           "overflowing asset dimensions restore default fallback");

    const auto skin52Manifest = skinRoot.getChildFile("Skin_052").getChildFile("theme.json");
    const auto skin52Json = skin52Manifest.loadFileAsString();
    expect(skin52Json.isNotEmpty(), "Skin_052 manifest readable");
    const bool skin52Loaded = manager.loadFromFile(skin52Manifest);
    expect(skin52Loaded, "Skin Studio 052 manifest accepted"
           + (skin52Loaded ? juce::String{} : ": " + manager.getLastError()));
    if (skin52Loaded)
    {
        expect(manager.getPalette().id == "skin-052", "Skin_052 ID matches three-digit index");
        expect(manager.getPalette().name == "SIGNAL FORGE", "Skin_052 SIGNAL FORGE name is decoded");
        expect(!manager.getPalette().usedFallback, "Skin_052 does not use fallback");
        expect(manager.getPalette().canvas == juce::Colour(0xff0b0f12),
               "Skin_052 window background is mapped");

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

        auto missingControl = juce::JSON::parse(skin52Json);
        auto* missingControls = missingControl.getProperty("controls", juce::var()).getDynamicObject();
        expect(missingControls != nullptr, "Skin_052 controls object is mutable for contract tests");
        if (missingControls != nullptr)
            missingControls->removeProperty("About");
        expect(!manager.loadFromJson(juce::JSON::toString(missingControl)),
               "Skin_052 rejects a missing mandatory control ID");
        expect(manager.getPalette().usedFallback,
               "missing mandatory control ID restores default fallback");

        auto unexpectedControl = juce::JSON::parse(skin52Json);
        auto* unexpectedControls = unexpectedControl.getProperty("controls", juce::var()).getDynamicObject();
        expect(unexpectedControls != nullptr, "Skin_052 controls object is mutable for extra-key test");
        if (unexpectedControls != nullptr)
            unexpectedControls->setProperty("unapproved_control",
                                             unexpectedControls->getProperty("About"));
        expect(!manager.loadFromJson(juce::JSON::toString(unexpectedControl)),
               "Skin_052 rejects an unapproved extra control ID");
        expect(manager.getPalette().usedFallback,
               "extra control ID restores default fallback");
    }

    const auto skin53Manifest = skinRoot.getChildFile("Skin_053").getChildFile("theme.json");
    const auto skin53Json = skin53Manifest.loadFileAsString();
    expect(skin53Json.isNotEmpty(), "Skin_053 manifest readable");
    const bool skin53Loaded = manager.loadFromFile(skin53Manifest);
    expect(skin53Loaded, "NEON STAGE manifest accepted"
           + (skin53Loaded ? juce::String{} : ": " + manager.getLastError()));
    if (skin53Loaded)
    {
        expect(manager.getPalette().id == "skin-053", "NEON STAGE ID matches index 53");
        expect(manager.getPalette().name == "NEON STAGE", "NEON STAGE identity is decoded");
        expect(!manager.getPalette().usedFallback, "NEON STAGE manifest does not use fallback");
        const auto neonTokens = juce::JSON::parse(skin53Json).getProperty("palette", juce::var());
        const juce::StringArray tokenNames {
            "window_bg", "panel", "panel_elevated", "control_bg", "control_face",
            "control_border", "text_primary", "text_secondary", "text_disabled",
            "accent", "accent_glow", "danger", "warning", "success", "lcd_bg",
            "lcd_text", "led_on", "led_off", "shadow", "highlight", "lowlight"
        };
        for (const auto& tokenName : tokenNames)
        {
            const auto expected = juce::Colour::fromString(
                "ff" + neonTokens.getProperty(tokenName, juce::var()).toString().substring(1));
            expect(manager.getPalette().token(tokenName) == expected,
                   "NEON STAGE token is exposed exactly: " + tokenName);
        }
    }

    const auto badStudioColour = skin52Json.replace("#D5FF45", "#GG5FF45");
    expect(!manager.loadFromJson(badStudioColour), "invalid Skin Studio HEX token rejected");
    expect(manager.getPalette().id == "skin-053" && manager.getPalette().usedFallback,
           "invalid Skin Studio manifest restores default fallback");

    const auto badStudioCompatibility = skin52Json.replace("\"audioChanges\": false",
                                                            "\"audioChanges\": true");
    expect(!manager.loadFromJson(badStudioCompatibility),
           "audio-changing Skin Studio theme rejected");
    expect(manager.getPalette().usedFallback,
           "Skin Studio compatibility failure restores fallback");

    expect(!manager.loadFromJson("{"), "malformed JSON rejected");
    expect(manager.getPalette().id == "skin-053" && manager.getPalette().usedFallback,
           "malformed JSON restores NEON STAGE fallback");

    expect(!manager.loadFromJson("{}"), "empty object rejected");
    expect(manager.getPalette().id == "skin-053" && manager.getPalette().usedFallback,
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
