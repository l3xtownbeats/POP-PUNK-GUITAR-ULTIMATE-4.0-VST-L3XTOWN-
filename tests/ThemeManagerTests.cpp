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

    for (int index = 1; index <= 51; ++index)
    {
        const auto id = juce::String::formatted("skin-%02d", index);
        const auto manifest = skinRoot.getChildFile(juce::String::formatted("Skin_%02d", index))
                                       .getChildFile("theme.json");
        const bool loaded = manager.loadFromFile(manifest);
        expect(loaded, id + " manifest accepted" + (loaded ? juce::String{} : ": " + manager.getLastError()));
        if (loaded)
        {
            expect(manager.getPalette().id == id, id + " ID matches folder index");
            expect(!manager.getPalette().usedFallback, id + " does not use fallback");
        }
    }

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
        expect(manager.getPalette().canvas == juce::Colour(0xff0b0f12), "Skin_052 window background is mapped");
    }

    const auto badStudioColour = skin52Json.replace("#D5FF45", "#GG5FF45");
    expect(!manager.loadFromJson(badStudioColour), "invalid Skin Studio HEX token rejected");
    expect(manager.getPalette().id == "skin-51" && manager.getPalette().usedFallback,
           "invalid Skin Studio manifest restores default fallback");

    const auto defaultManifest = skinRoot.getChildFile("Skin_51").getChildFile("theme.json");
    const auto validJson = defaultManifest.loadFileAsString();
    expect(validJson.isNotEmpty(), "Skin_51 manifest readable");

    expect(!manager.loadFromJson("{"), "malformed JSON rejected");
    expect(manager.getPalette().id == "skin-51" && manager.getPalette().usedFallback,
           "malformed JSON restores Blackout Voltage fallback");

    expect(!manager.loadFromJson("{}"), "empty object rejected");
    expect(manager.getPalette().id == "skin-51" && manager.getPalette().usedFallback,
           "incomplete manifest restores fallback");

    const auto badColour = validJson.replace("#090C10", "#GG0C10");
    expect(!manager.loadFromJson(badColour), "invalid HEX colour rejected");
    expect(manager.getPalette().usedFallback, "invalid HEX restores fallback");

    const auto badCompatibility = validJson.replace("\"audioChanges\": false",
                                                     "\"audioChanges\": true");
    expect(!manager.loadFromJson(badCompatibility), "audio-changing theme rejected");
    expect(manager.getPalette().usedFallback, "compatibility failure restores fallback");

    const auto mismatchedIndex = validJson.replace("\"index\": 51", "\"index\": 50");
    expect(!manager.loadFromJson(mismatchedIndex), "ID/index mismatch rejected");
    expect(manager.getPalette().usedFallback, "ID/index failure restores fallback");

    if (failures != 0)
    {
        std::cerr << failures << " theme manager test(s) failed." << std::endl;
        return 1;
    }

    std::cout << "ThemeManager contract tests passed: 51 manifests + fallback/error cases." << std::endl;
    return 0;
}
