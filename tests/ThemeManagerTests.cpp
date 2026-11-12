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
        const auto& studioPalette = manager.getPalette();
        expect(studioPalette.windowBg == juce::Colour(0xff0b0f12), "token window_bg is retained");
        expect(studioPalette.panel == juce::Colour(0xff141b20), "token panel is retained");
        expect(studioPalette.panelElevated == juce::Colour(0xff1d2930), "token panel_elevated is retained");
        expect(studioPalette.controlBg == juce::Colour(0xff10171b), "token control_bg is retained");
        expect(studioPalette.controlFace == juce::Colour(0xff26343a), "token control_face is retained");
        expect(studioPalette.controlBorder == juce::Colour(0xff53656b), "token control_border is retained");
        expect(studioPalette.textPrimary == juce::Colour(0xfff3f7f5), "token text_primary is retained");
        expect(studioPalette.textSecondary == juce::Colour(0xffb7c5c2), "token text_secondary is retained");
        expect(studioPalette.textDisabled == juce::Colour(0xff788984), "token text_disabled is retained");
        expect(studioPalette.accent == juce::Colour(0xffd5ff45), "token accent is retained");
        expect(studioPalette.accentGlow == juce::Colour(0xffb7ff3c), "token accent_glow is retained");
        expect(studioPalette.danger == juce::Colour(0xffff5a66), "token danger is retained");
        expect(studioPalette.warning == juce::Colour(0xffffc857), "token warning is retained");
        expect(studioPalette.success == juce::Colour(0xff6ee7a8), "token success is retained");
        expect(studioPalette.lcdBg == juce::Colour(0xff08110e), "token lcd_bg is retained");
        expect(studioPalette.lcdText == juce::Colour(0xffd7ffb2), "token lcd_text is retained");
        expect(studioPalette.ledOn == juce::Colour(0xffa8ff42), "token led_on is retained");
        expect(studioPalette.ledOff == juce::Colour(0xff33443b), "token led_off is retained");
        expect(studioPalette.shadow == juce::Colour(0xff050708), "token shadow is retained");
        expect(studioPalette.highlight == juce::Colour(0xff90a7a0), "token highlight is retained");
        expect(studioPalette.lowlight == juce::Colour(0xff0d1417), "token lowlight is retained");
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

    expect(!manager.loadFromJson("{"), "malformed JSON rejected");
    expect(manager.getPalette().id == "skin-51" && manager.getPalette().usedFallback,
           "malformed JSON restores Blackout Voltage fallback");

    expect(!manager.loadFromJson("{}"), "empty object rejected");
    expect(manager.getPalette().id == "skin-51" && manager.getPalette().usedFallback,
           "incomplete manifest restores fallback");

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
