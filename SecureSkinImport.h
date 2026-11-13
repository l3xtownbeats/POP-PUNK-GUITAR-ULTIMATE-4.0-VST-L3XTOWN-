#pragma once
// PPGU SecureSkinImport — S1 contract: secure .ppguskin import (JUCE-free core).
// Allowlist paths/extensions, traversal & drive-letter rejection, hidden/empty
// segments rejected, duplicate entries rejected, limits 64 MiB archive /
// 8 MiB entry / 512 entries, SVG policy, SHA-256 provenance (FIPS 180-4).
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace ppgu
{
struct SkinEntry
{
    std::string path;            // normalized: '/' separators, no leading '/'
    std::vector<uint8_t> data;
};

class SecureSkinImport
{
public:
    static constexpr std::size_t kMaxArchiveBytes = 64u * 1024u * 1024u;
    static constexpr std::size_t kMaxEntryBytes   = 8u  * 1024u * 1024u;
    static constexpr std::size_t kMaxEntries      = 512;
    static constexpr std::size_t kMaxPathLen      = 512;

    // Path policy (fail-closed). Mirrors Tools/secure_skin_import_mirror.py.
    static bool isPathAllowed(const std::string& path);

    // Extension allowlist: theme_manifest.json / theme.json / .png / .svg
    static bool isExtensionAllowed(const std::string& path);

    // SVG content policy: script/ENTITY/foreignObject/javascript:/external
    // href (both quote styles, bare href=http://) and on<token>= handlers.
    static bool isSvgContentAllowed(const std::string& text);

    // Entry-level limits check.
    static bool isEntrySizeOk(std::size_t n) { return n <= kMaxEntryBytes; }

    // SHA-256 (FIPS 180-4). Verified: 4 FIPS vectors + lengths 0..259.
    static void sha256(const uint8_t* data, std::size_t len, uint8_t out32[32]);
    static std::string sha256Hex(const uint8_t* data, std::size_t len);
};
} // namespace ppgu
