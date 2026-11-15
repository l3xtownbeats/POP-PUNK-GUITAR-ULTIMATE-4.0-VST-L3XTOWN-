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
    expect(manager.getPalette().id == "skin-053", "default theme ID");
    expect(manager.getPalette().usedFallback, "default palette is marked fallback");
    expect(manager.getPalette().name == "NEON FORGE (DEFAULT)", "fallback identity matches canonical SKIN-1");
    const auto& fallbackTokens = manager.getPalette().tokens;
    expect(fallbackTokens.windowBg == juce::Colour(0xff080b14), "fallback window background matches NEON STAGE");
    expect(fallbackTokens.panel == juce::Colour(0xff101827), "fallback panel matches NEON STAGE");
    expect(fallbackTokens.panelElevated == juce::Colour(0xff17243a), "fallback elevated panel matches NEON STAGE");
    expect(fallbackTokens.controlBg == juce::Colour(0xff0c1422), "fallback control background matches NEON STAGE");
    expect(fallbackTokens.controlFace == juce::Colour(0xff1a2940), "fallback control face matches NEON STAGE");
    expect(fallbackTokens.controlBorder == juce::Colour(0xff31506f), "fallback control border matches NEON STAGE");
    expect(fallbackTokens.textPrimary == juce::Colour(0xfff4f7ff), "fallback primary text matches NEON STAGE");
    expect(fallbackTokens.textSecondary == juce::Colour(0xffb5c4dc), "fallback secondary text matches NEON STAGE");
    expect(fallbackTokens.textDisabled == juce::Colour(0xff71829d), "fallback disabled text matches NEON STAGE");
    expect(fallbackTokens.accent == juce::Colour(0xff00d9ff), "fallback accent matches NEON STAGE");
    expect(fallbackTokens.accentGlow == juce::Colour(0xff6b8cff), "fallback accent glow matches NEON STAGE");
    expect(fallbackTokens.danger == juce::Colour(0xffff557a), "fallback danger matches NEON STAGE");
    expect(fallbackTokens.warning == juce::Colour(0xffffbe55), "fallback warning matches NEON STAGE");
    expect(fallbackTokens.success == juce::Colour(0xff42e6b0), "fallback success matches NEON STAGE");
    expect(fallbackTokens.lcdBg == juce::Colour(0xff050d1b), "fallback LCD background matches NEON STAGE");
    expect(fallbackTokens.lcdText == juce::Colour(0xff8debff), "fallback LCD text matches NEON STAGE");
    expect(fallbackTokens.ledOn == juce::Colour(0xff00e5ff), "fallback LED on matches NEON STAGE");
    expect(fallbackTokens.ledOff == juce::Colour(0xff263b55), "fallback LED off matches NEON STAGE");
    expect(fallbackTokens.shadow == juce::Colour(0xff03060b), "fallback shadow matches NEON STAGE");
    expect(fallbackTokens.highlight == juce::Colour(0xff8dcbff), "fallback highlight matches NEON STAGE");
    expect(fallbackTokens.lowlight == juce::Colour(0xff080f1b), "fallback lowlight matches NEON STAGE");
    expect(manager.getPalette().focus == fallbackTokens.highlight, "fallback focus alias matches canonical highlight token");
    expect(manager.getPalette().uiTypeface == "Segoe UI", "fallback UI typeface is NEON STAGE");
    expect(manager.getPalette().displayTypeface == "Consolas", "fallback display typeface is NEON STAGE");
    expect(manager.getPalette().fontScale == 1.0f, "fallback font scale is NEON STAGE");
    expect(manager.getPalette().letterSpacing == 0.35f, "fallback letter spacing is NEON STAGE");


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

    const auto defaultManifest = skinRoot.getChildFile("isolated_legacy/assets/Skin_51/theme.json");
    const auto validJson = defaultManifest.loadFileAsString();
    expect(validJson.isNotEmpty(), "isolated legacy Skin_51 manifest readable");

    const auto wrappedWidth = validJson.replace("\"width\": 480",
                                                "\"width\": 4294967776");
    expect(!manager.loadFromJson(wrappedWidth),
           "int64 asset width that wraps to 480 after narrowing is rejected");
    expect(manager.getPalette().usedFallback,
           "overflowing asset dimensions restore default fallback");

    const auto skin53Manifest = skinRoot.getChildFile("Skin_053").getChildFile("theme.json");
    const bool skin53Loaded = skin53Manifest.existsAsFile() && manager.loadFromFile(skin53Manifest);
    expect(skin53Loaded, "NEON STAGE Skin_053 manifest accepted"
           + (skin53Loaded ? juce::String{} : ": " + manager.getLastError()));
    if (skin53Loaded)
    {
        expect(manager.getPalette().id == "skin-053", "NEON STAGE stable ID preserved");
        expect(manager.getPalette().name == "NEON FORGE (DEFAULT)", "NEON FORGE display identity decoded");
        expect(manager.getPalette().uiTypeface == "Segoe UI", "NEON STAGE UI typeface decoded");
        expect(manager.getPalette().displayTypeface == "Consolas", "NEON STAGE display typeface decoded");
        expect(manager.getPalette().typographyWeight == "semibold", "NEON STAGE typography weight decoded");
        expect(manager.getPalette().fontScale == 1.0f, "NEON STAGE font scale decoded");
        expect(manager.getPalette().letterSpacing == 0.35f, "NEON STAGE letter spacing decoded");
    }

    const auto canonicalJson = skin53Manifest.loadFileAsString();

    // The active runtime registry intentionally contains only the approved
    // canonical default while the remaining approved themes pass artwork gates.
    // Do not load retired SIGNAL FORGE / Skin_052 as a runtime theme.
    auto missingControl = juce::JSON::parse(canonicalJson);
    auto* missingControls = missingControl.getProperty("controls", juce::var()).getDynamicObject();
    expect(missingControls != nullptr, "canonical controls object is mutable for contract tests");
    if (missingControls != nullptr)
        missingControls->removeProperty("About");
    expect(!manager.loadFromJson(juce::JSON::toString(missingControl)),
           "canonical manifest rejects a missing mandatory control ID");
    expect(manager.getPalette().usedFallback,
           "missing mandatory control ID restores default fallback");

    auto unexpectedControl = juce::JSON::parse(validJson);
    auto* unexpectedControls = unexpectedControl.getProperty("controls", juce::var()).getDynamicObject();
    expect(unexpectedControls != nullptr, "canonical controls object is mutable for extra-key test");
    if (unexpectedControls != nullptr)
        unexpectedControls->setProperty("unapproved_control",
                                         unexpectedControls->getProperty("About"));
    expect(!manager.loadFromJson(juce::JSON::toString(unexpectedControl)),
           "canonical manifest rejects an unapproved extra control ID");
    expect(manager.getPalette().usedFallback,
           "extra control ID restores default fallback");

    const auto badStudioColour = canonicalJson.replace("#00D9FF", "#GGD9FF");
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
           "incomplete manifest restores NEON STAGE fallback");

    expect(manager.loadFromJson(validJson), "isolated legacy manifest remains loadable after token migration");
    expect(manager.getPalette().token("window_bg") == manager.getPalette().canvas,
           "legacy window background maps to canonical token");
    expect(manager.getPalette().token("accent") == manager.getPalette().accent,
           "legacy accent maps to canonical token");
    expect(manager.getPalette().token("danger").isOpaque(),
           "legacy semantic token receives a deterministic fallback");

    const auto badColour = validJson.replace("#080B14", "#GG0B14");
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
