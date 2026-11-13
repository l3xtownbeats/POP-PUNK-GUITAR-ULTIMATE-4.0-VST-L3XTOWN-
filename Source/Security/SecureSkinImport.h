#pragma once
// PPGU SecureSkinImport — O2 contract. JUCE-free security checks for .ppguskin
// entries. ZIP extraction, symlink inspection and quarantine remain in the
// caller; validateEntries checks the extracted entry set before publication.
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace ppgu
{
struct SkinEntry
{
    std::string path;            // normalized '/' separators, no leading slash
    std::vector<std::uint8_t> data;
};

class SecureSkinImport final
{
public:
    static constexpr std::size_t kMaxArchiveBytes = 64u * 1024u * 1024u;
    static constexpr std::size_t kMaxEntryBytes = 8u * 1024u * 1024u;
    static constexpr std::size_t kMaxEntries = 512;
    static constexpr std::size_t kMaxPathLen = 512;

    static bool isPathAllowed(const std::string& path);
    static bool isExtensionAllowed(const std::string& path);
    static bool isSvgContentAllowed(const std::string& text);
    static bool isEntrySizeOk(std::size_t bytes) noexcept;

    static bool validateEntries(const std::vector<SkinEntry>& entries,
                                std::uint64_t archiveBytes,
                                std::string* error = nullptr);

    // FIPS 180-4 SHA-256. Import-thread operation; never call from processBlock.
    static void sha256(const std::uint8_t* data, std::size_t length,
                       std::uint8_t output[32]);
    static std::string sha256Hex(const std::uint8_t* data, std::size_t length);
};
} // namespace ppgu
