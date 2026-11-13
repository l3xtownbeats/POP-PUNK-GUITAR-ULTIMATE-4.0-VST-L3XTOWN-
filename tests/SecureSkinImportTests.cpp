// O2 contract tests. Exactly 34 checks: 8 SHA, 16 path/entry, 10 SVG.
#include "SecureSkinImport.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{
int checks = 0;
int failures = 0;
#define CHECK(expression) do { \
    ++checks; \
    if (!(expression)) { \
        ++failures; \
        std::cerr << "FAIL line " << __LINE__ << ": " #expression "\n"; \
    } \
} while (false)
}

int main()
{
    using ppgu::SecureSkinImport;
    using ppgu::SkinEntry;

    const std::string empty;
    const std::string abc = "abc";
    const std::string vector56 =
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    const std::string vector112 =
        "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno"
        "ijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

    CHECK(SecureSkinImport::sha256Hex(nullptr, 0).size() == 64);
    CHECK(SecureSkinImport::sha256Hex(nullptr, 0) ==
          "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    CHECK(SecureSkinImport::sha256Hex(
          reinterpret_cast<const std::uint8_t*>(abc.data()), abc.size()).size() == 64);
    CHECK(SecureSkinImport::sha256Hex(
          reinterpret_cast<const std::uint8_t*>(abc.data()), abc.size()) ==
          "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    CHECK(vector56.size() == 56);
    CHECK(SecureSkinImport::sha256Hex(
          reinterpret_cast<const std::uint8_t*>(vector56.data()), vector56.size()) ==
          "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
    CHECK(vector112.size() == 112);
    CHECK(SecureSkinImport::sha256Hex(
          reinterpret_cast<const std::uint8_t*>(vector112.data()), vector112.size()) ==
          "cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1");

    CHECK(SecureSkinImport::isPathAllowed("theme_manifest.json")
          && SecureSkinImport::isExtensionAllowed("theme_manifest.json"));
    CHECK(SecureSkinImport::isPathAllowed("theme.json")
          && SecureSkinImport::isExtensionAllowed("theme.json"));
    CHECK(SecureSkinImport::isPathAllowed("Skin_001/assets/bg.png")
          && SecureSkinImport::isExtensionAllowed("Skin_001/assets/bg.png"));
    CHECK(SecureSkinImport::isPathAllowed("svg/bezel.svg")
          && SecureSkinImport::isExtensionAllowed("svg/bezel.svg"));
    CHECK(SecureSkinImport::isEntrySizeOk(SecureSkinImport::kMaxEntryBytes)
          && !SecureSkinImport::isEntrySizeOk(SecureSkinImport::kMaxEntryBytes + 1));

    std::vector<SkinEntry> validEntries {
        { "theme_manifest.json", { '{', '}' } },
        { "assets/bg.png", { 0x89, 'P', 'N', 'G' } }
    };
    CHECK(SecureSkinImport::validateEntries(validEntries,
          SecureSkinImport::kMaxArchiveBytes)
          && !SecureSkinImport::validateEntries(validEntries,
          SecureSkinImport::kMaxArchiveBytes + 1));

    std::vector<SkinEntry> duplicateEntries {
        { "theme_manifest.json", { '{', '}' } },
        { "THEME_MANIFEST.JSON", { '{', '}' } }
    };
    CHECK(!SecureSkinImport::validateEntries(duplicateEntries, 128));

    CHECK(!SecureSkinImport::isPathAllowed("../evil.png"));
    CHECK(!SecureSkinImport::isPathAllowed("../theme_manifest.json"));
    CHECK(!SecureSkinImport::isPathAllowed("..\\evil.png"));
    CHECK(!SecureSkinImport::isPathAllowed("C:/theme_manifest.json"));
    CHECK(!SecureSkinImport::isPathAllowed("assets/.hidden.svg"));
    CHECK(!SecureSkinImport::isPathAllowed(".hidden/theme.json"));
    CHECK(!SecureSkinImport::isPathAllowed("/abs/path.png"));
    CHECK(!SecureSkinImport::isPathAllowed("assets//bg.png"));
    CHECK(!SecureSkinImport::isPathAllowed("trailing/name/")
          && !SecureSkinImport::isPathAllowed("")
          && !SecureSkinImport::isPathAllowed(std::string(513, 'x')));

    CHECK(SecureSkinImport::isSvgContentAllowed("<svg><circle r=\"5\"/></svg>"));
    CHECK(!SecureSkinImport::isSvgContentAllowed("<text onclick=\"x()\">hi</text>"));
    CHECK(!SecureSkinImport::isSvgContentAllowed("<text onload=\"x()\">hi</text>"));
    CHECK(!SecureSkinImport::isSvgContentAllowed("<svg><a href=\"javascript:alert(1)\">x</a></svg>"));
    CHECK(!SecureSkinImport::isSvgContentAllowed("<svg><a href='javascript:alert(1)'>x</a></svg>"));
    CHECK(!SecureSkinImport::isSvgContentAllowed("<svg><a href=\"http://evil/x.png\">x</a></svg>"));
    CHECK(!SecureSkinImport::isSvgContentAllowed("<svg><a href='http://evil/x.png'>x</a></svg>"));
    CHECK(!SecureSkinImport::isSvgContentAllowed("<svg><a href=https://evil/x.png>x</a></svg>"));
    CHECK(!SecureSkinImport::isSvgContentAllowed("<!DOCTYPE svg [<!ENTITY x \"y\">]><svg/>"));
    CHECK(!SecureSkinImport::isSvgContentAllowed("<svg><foreignObject/></svg>"));

    if (failures != 0)
    {
        std::cerr << failures << " SecureSkinImport check(s) failed out of "
                  << checks << ".\n";
        return 1;
    }
    std::cout << checks << "/" << checks << " PASS\n";
    return checks == 34 ? 0 : 2;
}
