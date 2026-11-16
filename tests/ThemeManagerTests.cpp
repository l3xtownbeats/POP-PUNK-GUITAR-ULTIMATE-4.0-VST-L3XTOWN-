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
    expect(manager.getPalette().name == "NEON STAGE DEFAULT",
           "fallback identity matches the canonical default skin");

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
               "registry totalSkins matches active entry count");

        // REV.19 intentionally exposes only themes that have passed runtime acceptance.
        // The approved 16-theme sequence is not a claim that all 16 are already active.
        expect(themes->size() == 1, "only the accepted NEON STAGE theme is active");
        for (const auto& entry : *themes)
        {
            expect(entry.isObject(), "registry entry is an object");
            if (!entry.isObject())
                continue;

            const auto id = entry.getProperty("id", "").toString();
            const auto indexValue = entry.getProperty("index", juce::var());
            const auto relativePath = entry.getProperty("path", "").toString();
            expect(id == "skin-053", "active registry contains only canonical skin-053");
            expect(indexValue.isInt() || indexValue.isInt64(), id + " index is an integer");
            expect(relativePath == "Skin_053/theme.json",
                   id + " points to the canonical manifest path");
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
                expect(id.substring(5).getLargeIntValue() == index,
                       id + " ID matches stable registry index");
                expect(!manager.getPalette().usedFallback, id + " does not use fallback");
            }
        }
    }

    const auto skin53Manifest = skinRoot.getChildFile("Skin_053").getChildFile("theme.json");
    const auto validJson = skin53Manifest.loadFileAsString();
    expect(validJson.isNotEmpty(), "NEON STAGE Skin_053 manifest is readable");
    const bool skin53Loaded = skin53Manifest.existsAsFile() && manager.loadFromFile(skin53Manifest);
    expect(skin53Loaded, "NEON STAGE Skin_053 manifest accepted"
           + (skin53Loaded ? juce::String{} : ": " + manager.getLastError()));
    if (skin53Loaded)
    {
        expect(manager.getPalette().id == "skin-053", "NEON STAGE stable ID preserved");
        expect(manager.getPalette().name == "NEON STAGE DEFAULT",
               "NEON STAGE display identity decoded");
        expect(manager.getPalette().uiTypeface == "Segoe UI",
               "NEON STAGE UI typeface decoded");
        expect(manager.getPalette().displayTypeface == "Consolas",
               "NEON STAGE display typeface decoded");
        expect(manager.getPalette().typographyWeight == "semibold",
               "NEON STAGE typography weight decoded");
        expect(manager.getPalette().fontScale == 1.0f, "NEON STAGE font scale decoded");
        expect(manager.getPalette().letterSpacing == 0.35f,
               "NEON STAGE letter spacing decoded");
    }

    const auto badColour = validJson.replace("#080B14", "#GG0B14");
    expect(!manager.loadFromJson(badColour), "invalid Skin Studio HEX colour rejected");
    expect(manager.getPalette().id == "skin-053" && manager.getPalette().usedFallback,
           "invalid colour restores NEON STAGE fallback");

    const auto badCompatibility = validJson.replace("\"audioChanges\": false",
                                                     "\"audioChanges\": true");
    expect(!manager.loadFromJson(badCompatibility),
           "audio-changing Skin Studio theme rejected");
    expect(manager.getPalette().usedFallback,
           "compatibility failure restores canonical fallback");

    auto missingControl = juce::JSON::parse(validJson);
    auto* missingControls = missingControl.getProperty("controls", juce::var()).getDynamicObject();
    expect(missingControls != nullptr, "Skin_053 controls object is mutable for contract tests");
    if (missingControls != nullptr)
        missingControls->removeProperty("About");
    expect(!manager.loadFromJson(juce::JSON::toString(missingControl)),
           "Skin_053 rejects a missing mandatory control ID");
    expect(manager.getPalette().usedFallback,
           "missing mandatory control ID restores default fallback");

    auto unexpectedControl = juce::JSON::parse(validJson);
    auto* unexpectedControls = unexpectedControl.getProperty("controls", juce::var()).getDynamicObject();
    expect(unexpectedControls != nullptr, "Skin_053 controls object is mutable for extra-key test");
    if (unexpectedControls != nullptr)
        unexpectedControls->setProperty("unapproved_control",
                                         unexpectedControls->getProperty("About"));
    expect(!manager.loadFromJson(juce::JSON::toString(unexpectedControl)),
           "Skin_053 rejects an unapproved extra control ID");
    expect(manager.getPalette().usedFallback,
           "extra control ID restores default fallback");

    expect(!manager.loadFromJson("{"), "malformed JSON rejected");
    expect(manager.getPalette().id == "skin-053" && manager.getPalette().usedFallback,
           "malformed JSON restores NEON STAGE fallback");

    expect(!manager.loadFromJson("{}"), "empty object rejected");
    expect(manager.getPalette().id == "skin-053" && manager.getPalette().usedFallback,
           "incomplete manifest restores NEON STAGE fallback");

    if (failures != 0)
    {
        std::cerr << failures << " theme manager test(s) failed." << std::endl;
        return 1;
    }

    std::cout << "ThemeManager contract tests passed: active REV.19 registry + canonical manifest + fallback/error cases."
              << std::endl;
    return 0;
}
