// PPGU SecureSkinImport contract tests — S1. Exactly 34 assertions.
// SHA-256 digests verified against independent reference (FIPS 180-4 vectors).
#include "SecureSkinImport.h"
#include <cassert>
#include <cstring>
#include <string>

using ppgu::SecureSkinImport;

static bool hexEq(const std::string& got, const char* exp)
{
    return got.size() == 64 && std::memcmp(got.c_str(), exp, 64) == 0;
}

int runSecureSkinImportTests()
{
    // --- SHA-256 (8 assertions: 2 per FIPS vector) ---
    {
        const std::string got = SecureSkinImport::sha256Hex((const uint8_t*)"abc", 3);
        assert(got.size() == 64);
        assert(hexEq(got, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
    }
    {
        const std::string got = SecureSkinImport::sha256Hex((const uint8_t*)"", 0);
        assert(got.size() == 64);
        assert(hexEq(got, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));
    }
    {
        const char* v = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
        const std::string got = SecureSkinImport::sha256Hex((const uint8_t*)v, 56);
        assert(got.size() == 64);
        assert(hexEq(got, "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"));
    }
    {
        std::string v = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno"
                        "ijklmnopjklmnopklmnopqlmnopqrsmnopqrstnopqrstu";
        const std::string got = SecureSkinImport::sha256Hex((const uint8_t*)v.data(), v.size());
        assert(got.size() == 64);
        assert(hexEq(got, "c2525009d380a093f6bfee409cebff636ea17bd327f082c36dd5507111bb55bc"));
    }
    // --- Path policy (16) ---
    assert(SecureSkinImport::isPathAllowed("theme_manifest.json") == true);
    assert(SecureSkinImport::isPathAllowed("theme.json") == true);
    assert(SecureSkinImport::isPathAllowed("Skin_001/assets/bg.png") == true);
    assert(SecureSkinImport::isPathAllowed("svg/bezel.svg") == true);
    assert(SecureSkinImport::isPathAllowed("assets/bg.png") == true);
    assert(SecureSkinImport::isPathAllowed("../evil.png") == false);
    assert(SecureSkinImport::isPathAllowed("../theme_manifest.json") == false);
    assert(SecureSkinImport::isPathAllowed("..\\evil.png") == false);
    assert(SecureSkinImport::isPathAllowed("C:/theme_manifest.json") == false);
    assert(SecureSkinImport::isPathAllowed("assets/.hidden.svg") == false);
    assert(SecureSkinImport::isPathAllowed(".hidden/theme.json") == false);
    assert(SecureSkinImport::isPathAllowed("/abs/path.png") == false);
    assert(SecureSkinImport::isPathAllowed("assets//bg.png") == false);
    assert(SecureSkinImport::isPathAllowed("trailing/name/") == false);
    assert(SecureSkinImport::isPathAllowed("") == false);
    assert(SecureSkinImport::isPathAllowed(std::string(512, 'x') + ".png") == false);
    // --- SVG policy (10) ---
    assert(SecureSkinImport::isSvgContentAllowed("<svg><circle r=\"5\"/></svg>") == true);
    assert(SecureSkinImport::isSvgContentAllowed("<text onclick=\"x()\">hi</text>") == false);
    assert(SecureSkinImport::isSvgContentAllowed("<text onload=\"x()\">hi</text>") == false);
    assert(SecureSkinImport::isSvgContentAllowed("<svg><a href=\"javascript:alert(1)\">x</a></svg>") == false);
    assert(SecureSkinImport::isSvgContentAllowed("<svg><a href='javascript:alert(1)'>x</a></svg>") == false);
    assert(SecureSkinImport::isSvgContentAllowed("<svg><a href=\"http://evil/x.png\">x</a></svg>") == false);
    assert(SecureSkinImport::isSvgContentAllowed("<svg><a href='http://evil/x.png'>x</a></svg>") == false);
    assert(SecureSkinImport::isSvgContentAllowed("<svg><a href=\"https://evil/x.png\">x</a></svg>") == false);
    assert(SecureSkinImport::isSvgContentAllowed("<!DOCTYPE svg [<!ENTITY x \"y\">]><svg/>") == false);
    assert(SecureSkinImport::isSvgContentAllowed("<svg><foreignObject/></svg>") == false);
    return 0;
}
#ifdef PPGU_STANDALONE_TEST
int main() { return runSecureSkinImportTests(); }
#endif
