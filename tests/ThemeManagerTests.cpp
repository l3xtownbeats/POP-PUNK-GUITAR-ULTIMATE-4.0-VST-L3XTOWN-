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
    expect(manager.getPalette().id == "skin-01", "default theme ID");
    expect(manager.getPalette().usedFallback, "default palette is marked fallback");

    const auto root = juce::File::getCurrentWorkingDirectory();
    const auto skinRoot = root.getChildFile("SKIN");

    for (int index = 1; index <= 50; ++index)
    {
        const auto id = juce::String::formatted("skin-%02d", index);
        const auto manifest = skinRoot.getChildFile("Skin_" + juce::String(index))
                                       .getChildFile("theme.json");
        const bool loaded = manager.loadFromFile(manifest);
        expect(loaded, id + " manifest accepted" + (loaded ? juce::String{} : ": " + manager.getLastError()));
        if (loaded)
        {
            expect(manager.getPalette().id == id, id + " ID matches folder index");
            expect(!manager.getPalette().usedFallback, id + " does not use fallback");
        }
    }

    const auto defaultManifest = skinRoot.getChildFile("Skin_01").getChildFile("theme.json");
    const auto validJson = defaultManifest.loadFileAsString();
    expect(validJson.isNotEmpty(), "Skin_01 manifest readable");

    expect(!manager.loadFromJson("{"), "malformed JSON rejected");
    expect(manager.getPalette().id == "skin-01" && manager.getPalette().usedFallback,
           "malformed JSON restores Neon Riot fallback");

    expect(!manager.loadFromJson("{}"), "empty object rejected");
    expect(manager.getPalette().id == "skin-01" && manager.getPalette().usedFallback,
           "incomplete manifest restores fallback");

    const auto badColour = validJson.replace("#111116", "#GG1116");
    expect(!manager.loadFromJson(badColour), "invalid HEX colour rejected");
    expect(manager.getPalette().usedFallback, "invalid HEX restores fallback");

    const auto badCompatibility = validJson.replace("\"audioChanges\": false",
                                                     "\"audioChanges\": true");
    expect(!manager.loadFromJson(badCompatibility), "audio-changing theme rejected");
    expect(manager.getPalette().usedFallback, "compatibility failure restores fallback");

    const auto mismatchedIndex = validJson.replace("\"index\": 1", "\"index\": 2");
    expect(!manager.loadFromJson(mismatchedIndex), "ID/index mismatch rejected");
    expect(manager.getPalette().usedFallback, "ID/index failure restores fallback");

    if (failures != 0)
    {
        std::cerr << failures << " theme manager test(s) failed." << std::endl;
        return 1;
    }

    std::cout << "ThemeManager contract tests passed: 50 manifests + fallback/error cases." << std::endl;
    return 0;
}
