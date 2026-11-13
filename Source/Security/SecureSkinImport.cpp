#include "SecureSkinImport.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <unordered_set>
#include <vector>

namespace ppgu
{
namespace
{
constexpr std::uint32_t rotateRight(std::uint32_t value, int bits) noexcept
{
    return (value >> bits) | (value << (32 - bits));
}

constexpr std::uint32_t kRoundConstants[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

void sha256Block(const std::uint8_t* block, std::uint32_t state[8])
{
    std::uint32_t words[64]{};
    for (int i = 0; i < 16; ++i)
    {
        const auto offset = static_cast<std::size_t>(4 * i);
        words[i] = (std::uint32_t(block[offset]) << 24)
                 | (std::uint32_t(block[offset + 1]) << 16)
                 | (std::uint32_t(block[offset + 2]) << 8)
                 | std::uint32_t(block[offset + 3]);
    }
    for (int i = 16; i < 64; ++i)
    {
        const auto s0 = rotateRight(words[i - 15], 7)
                      ^ rotateRight(words[i - 15], 18) ^ (words[i - 15] >> 3);
        const auto s1 = rotateRight(words[i - 2], 17)
                      ^ rotateRight(words[i - 2], 19) ^ (words[i - 2] >> 10);
        words[i] = words[i - 16] + s0 + words[i - 7] + s1;
    }

    auto a = state[0], b = state[1], c = state[2], d = state[3];
    auto e = state[4], f = state[5], g = state[6], h = state[7];
    for (int i = 0; i < 64; ++i)
    {
        const auto sum1 = rotateRight(e, 6) ^ rotateRight(e, 11) ^ rotateRight(e, 25);
        const auto choose = (e & f) ^ (~e & g);
        const auto t1 = h + sum1 + choose + kRoundConstants[i] + words[i];
        const auto sum0 = rotateRight(a, 2) ^ rotateRight(a, 13) ^ rotateRight(a, 22);
        const auto majority = (a & b) ^ (a & c) ^ (b & c);
        const auto t2 = sum0 + majority;
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }

    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

std::string lowerAscii(const std::string& input)
{
    std::string result;
    result.reserve(input.size());
    for (const unsigned char ch : input)
        result.push_back(static_cast<char>(std::tolower(ch)));
    return result;
}

bool isSpace(char ch) noexcept
{
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

bool isAttributeBoundary(char ch) noexcept
{
    return isSpace(ch) || ch == '<' || ch == '/';
}

bool hasEventHandler(const std::string& text)
{
    for (std::size_t i = 0; i + 2 < text.size(); ++i)
    {
        if (text[i] != 'o' || text[i + 1] != 'n'
            || (i != 0 && !isAttributeBoundary(text[i - 1])))
            continue;

        auto end = i + 2;
        if (!std::isalpha(static_cast<unsigned char>(text[end])))
            continue;
        while (end < text.size())
        {
            const auto ch = static_cast<unsigned char>(text[end]);
            if (!std::isalnum(ch) && ch != '_' && ch != '-')
                break;
            ++end;
        }
        while (end < text.size() && isSpace(text[end]))
            ++end;
        if (end < text.size() && text[end] == '=')
            return true;
    }
    return false;
}

bool hasExternalReferenceAttribute(const std::string& text)
{
    std::size_t position = 0;
    while ((position = text.find("href", position)) != std::string::npos)
    {
        const auto before = position == 0 ? '\0' : text[position - 1];
        if (std::isalnum(static_cast<unsigned char>(before))
            || before == '_' || before == '-')
        {
            position += 4;
            continue;
        }

        auto cursor = position + 4;
        while (cursor < text.size() && isSpace(text[cursor]))
            ++cursor;
        if (cursor >= text.size() || text[cursor] != '=')
        {
            position += 4;
            continue;
        }

        ++cursor;
        while (cursor < text.size() && isSpace(text[cursor]))
            ++cursor;
        if (cursor >= text.size())
            return true;

        std::string value;
        if (text[cursor] == '"' || text[cursor] == '\'')
        {
            const auto quote = text[cursor++];
            const auto end = text.find(quote, cursor);
            if (end == std::string::npos)
                return true;
            value = text.substr(cursor, end - cursor);
            position = end + 1;
        }
        else
        {
            const auto end = text.find_first_of(" \t\r\n>", cursor);
            value = text.substr(cursor, end == std::string::npos ? end : end - cursor);
            position = end == std::string::npos ? text.size() : end;
        }

        const auto first = value.find_first_not_of(" \t\r\n");
        if (first == std::string::npos || value[first] != '#')
            return true; // only same-document fragment references are allowed
    }
    return false;
}

bool hasExternalCssUrl(const std::string& text)
{
    std::size_t position = 0;
    while ((position = text.find("url(", position)) != std::string::npos)
    {
        auto cursor = position + 4;
        while (cursor < text.size() && isSpace(text[cursor]))
            ++cursor;
        if (cursor < text.size() && (text[cursor] == '"' || text[cursor] == '\''))
            ++cursor;
        if (cursor >= text.size() || text[cursor] != '#')
            return true;
        position = cursor + 1;
    }
    return false;
}

bool fail(std::string* error, const char* message)
{
    if (error != nullptr)
        *error = message;
    return false;
}
} // namespace

bool SecureSkinImport::isPathAllowed(const std::string& path)
{
    if (path.empty() || path.size() > kMaxPathLen
        || path.front() == '/' || path.front() == '\\'
        || path.back() == '/' || path.find('\\') != std::string::npos
        || path.find(':') != std::string::npos)
        return false;

    for (const unsigned char ch : path)
        if (ch < 0x20 || ch == 0x7f)
            return false;

    std::size_t start = 0;
    while (start < path.size())
    {
        const auto end = path.find('/', start);
        const auto length = (end == std::string::npos ? path.size() : end) - start;
        if (length == 0)
            return false;
        const auto segment = path.substr(start, length);
        if (segment == "." || segment == ".." || segment.front() == '.')
            return false;
        if (end == std::string::npos)
            break;
        start = end + 1;
    }
    return true;
}

bool SecureSkinImport::isExtensionAllowed(const std::string& path)
{
    if (!isPathAllowed(path))
        return false;

    const auto lowered = lowerAscii(path);
    if (lowered == "theme_manifest.json" || lowered == "theme.json")
        return true;

    const auto slash = lowered.find_last_of('/');
    const auto dot = lowered.find_last_of('.');
    if (dot == std::string::npos || (slash != std::string::npos && dot < slash))
        return false;
    const auto extension = lowered.substr(dot);
    return extension == ".png" || extension == ".svg";
}

bool SecureSkinImport::isSvgContentAllowed(const std::string& text)
{
    if (text.empty() || text.size() > kMaxEntryBytes)
        return false;

    const auto lowered = lowerAscii(text);
    if (lowered.find("<script") != std::string::npos
        || lowered.find("<!entity") != std::string::npos
        || lowered.find("<!doctype") != std::string::npos
        || lowered.find("<foreignobject") != std::string::npos
        || lowered.find("javascript:") != std::string::npos
        || hasEventHandler(lowered)
        || hasExternalReferenceAttribute(lowered)
        || hasExternalCssUrl(lowered))
        return false;
    return true;
}

bool SecureSkinImport::isEntrySizeOk(std::size_t bytes) noexcept
{
    return bytes <= kMaxEntryBytes;
}

bool SecureSkinImport::validateEntries(const std::vector<SkinEntry>& entries,
                                       std::uint64_t archiveBytes,
                                       std::string* error)
{
    if (archiveBytes > kMaxArchiveBytes)
        return fail(error, "archive exceeds 64 MiB");
    if (entries.empty() || entries.size() > kMaxEntries)
        return fail(error, "entry count is empty or exceeds 512");

    std::unordered_set<std::string> seen;
    seen.reserve(entries.size());
    bool hasManifest = false;

    for (const auto& entry : entries)
    {
        if (!isPathAllowed(entry.path))
            return fail(error, "entry path is not allowed");
        if (!isExtensionAllowed(entry.path))
            return fail(error, "entry extension is not allowed");
        if (!isEntrySizeOk(entry.data.size()))
            return fail(error, "entry exceeds 8 MiB");

        const auto canonical = lowerAscii(entry.path);
        if (!seen.insert(canonical).second)
            return fail(error, "duplicate entry path");

        if (canonical == "theme_manifest.json" || canonical == "theme.json")
            hasManifest = true;

        if (canonical.size() >= 4 && canonical.compare(canonical.size() - 4, 4, ".svg") == 0)
        {
            std::string svg;
            if (!entry.data.empty())
                svg.assign(reinterpret_cast<const char*>(entry.data.data()), entry.data.size());
            if (!isSvgContentAllowed(svg))
                return fail(error, "SVG content is not allowed");
        }
    }

    if (!hasManifest)
        return fail(error, "skin package manifest is missing");
    return true;
}

void SecureSkinImport::sha256(const std::uint8_t* data, std::size_t length,
                              std::uint8_t output[32])
{
    if (output == nullptr)
        return;

    if (data == nullptr && length != 0)
    {
        std::memset(output, 0, 32);
        return;
    }

    static constexpr std::uint32_t initialState[8] = {
        0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
    };

    std::uint32_t state[8];
    for (int i = 0; i < 8; ++i)
        state[i] = initialState[i];

    const auto bitLength = static_cast<std::uint64_t>(length) * 8u;
    std::vector<std::uint8_t> message;
    message.reserve(length + 72u);
    if (length != 0)
        message.insert(message.end(), data, data + length);
    message.push_back(0x80u);
    while (message.size() % 64u != 56u)
        message.push_back(0u);
    for (int i = 7; i >= 0; --i)
        message.push_back(static_cast<std::uint8_t>((bitLength >> (8 * i)) & 0xffu));

    for (std::size_t offset = 0; offset < message.size(); offset += 64u)
        sha256Block(message.data() + offset, state);

    for (int i = 0; i < 8; ++i)
    {
        output[4 * i] = static_cast<std::uint8_t>(state[i] >> 24);
        output[4 * i + 1] = static_cast<std::uint8_t>(state[i] >> 16);
        output[4 * i + 2] = static_cast<std::uint8_t>(state[i] >> 8);
        output[4 * i + 3] = static_cast<std::uint8_t>(state[i]);
    }
}

std::string SecureSkinImport::sha256Hex(const std::uint8_t* data, std::size_t length)
{
    if (data == nullptr && length != 0)
        return {};

    static constexpr char hex[] = "0123456789abcdef";
    std::uint8_t digest[32]{};
    sha256(data, length, digest);

    std::string result;
    result.reserve(64);
    for (const auto byte : digest)
    {
        result.push_back(hex[byte >> 4]);
        result.push_back(hex[byte & 0x0f]);
    }
    return result;
}
} // namespace ppgu
