#include "SecureSkinImport.h"
#include <cctype>
#include <cstring>

namespace ppgu
{
namespace
{
inline uint32_t ror(uint32_t x, int n)
{
    return (x >> n) | (x << (32 - n));
}

const uint32_t kK[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2 };

void sha256Block(const uint8_t* p, uint32_t h[8])
{
    uint32_t w[64];
    for (int i = 0; i < 16; ++i)
        w[i] = (uint32_t(p[4*i]) << 24) | (uint32_t(p[4*i+1]) << 16)
             | (uint32_t(p[4*i+2]) << 8) | uint32_t(p[4*i+3]);
    for (int i = 16; i < 64; ++i)
    {
        const uint32_t s0 = ror(w[i-15], 7) ^ ror(w[i-15], 18) ^ (w[i-15] >> 3);
        const uint32_t s1 = ror(w[i-2], 17) ^ ror(w[i-2], 19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
    uint32_t e = h[4], f = h[5], g = h[6], hh = h[7];
    for (int i = 0; i < 64; ++i)
    {
        const uint32_t S1 = ror(e, 6) ^ ror(e, 11) ^ ror(e, 25);
        const uint32_t ch = (e & f) ^ (~e & g);
        const uint32_t t1 = hh + S1 + ch + kK[i] + w[i];
        const uint32_t S0 = ror(a, 2) ^ ror(a, 13) ^ ror(a, 22);
        const uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        const uint32_t t2 = S0 + maj;
        hh = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
    }
    h[0] += a; h[1] += b; h[2] += c; h[3] += d;
    h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
}
} // namespace

bool SecureSkinImport::isPathAllowed(const std::string& path)
{
    if (path.empty()) return false;
    if (path.size() > kMaxPathLen) return false;
    if (path.find('\\') != std::string::npos) return false;      // backslash rejected
    if (path[0] == '/') return false;                            // absolute rejected
    if (path.find(':') != std::string::npos) return false;       // drive letter rejected
    size_t start = 0;
    while (start <= path.size())
    {
        size_t end = path.find('/', start);
        if (end == std::string::npos) end = path.size();
        const std::string seg = path.substr(start, end - start);
        if (seg.empty()) return false;                           // empty segment fail-closed
        if (seg == "." || seg == "..") return false;             // traversal
        if (seg[0] == '.') return false;                         // hidden segment
        if (end == path.size()) break;
        start = end + 1;
    }
    return true;
}

bool SecureSkinImport::isExtensionAllowed(const std::string& path)
{
    if (path == "theme_manifest.json" || path == "theme.json") return true;
    const size_t dot = path.rfind('.');
    if (dot == std::string::npos || dot + 1 >= path.size()) return false;
    const std::string ext = path.substr(dot + 1);
    if (ext == "png" || ext == "svg") return true;
    return false;
}

bool SecureSkinImport::isSvgContentAllowed(const std::string& text)
{
    const auto lower = [&text](const char* tok)
    {
        const size_t n = std::strlen(tok);
        for (size_t i = 0; i + n <= text.size(); ++i)
        {
            size_t j = 0;
            while (j < n && std::tolower((unsigned char)text[i + j]) == tok[j]) ++j;
            if (j == n) return true;
        }
        return false;
    };
    if (lower("<script")) return false;
    if (lower("<!entity")) return false;
    if (lower("<foreignobject")) return false;
    if (lower("javascript:")) return false;
    // inline handlers: on<token>= (covers onclick=, onload=, on*= in any quoting)
    for (size_t i = 0; i + 3 <= text.size(); ++i)
    {
        if (text[i] == 'o' && text[i+1] == 'n')
        {
            size_t j = i + 2;
            while (j < text.size() && std::isalpha((unsigned char)text[j])) ++j;
            if (j > i + 2 && j < text.size() && text[j] == '=') return false;
        }
    }
    // external href: href="http..." / href='http...' / bare href=http...
    for (size_t i = 0; i + 5 <= text.size(); ++i)
    {
        if (std::tolower((unsigned char)text[i]) == 'h'
         && std::tolower((unsigned char)text[i+1]) == 'r'
         && std::tolower((unsigned char)text[i+2]) == 'e'
         && std::tolower((unsigned char)text[i+3]) == 'f')
        {
            size_t j = i + 4;
            while (j < text.size() && std::isspace((unsigned char)text[j])) ++j;
            if (j >= text.size()) continue;
            if (text[j] == '=')
            {
                ++j;
                while (j < text.size() && std::isspace((unsigned char)text[j])) ++j;
                if (j < text.size() && (text[j] == '"' || text[j] == '\''))
                {
                    ++j;
                    while (j < text.size() && std::isspace((unsigned char)text[j])) ++j;
                }
                if (j + 6 < text.size()
                 && std::tolower((unsigned char)text[j]) == 'h'
                 && std::tolower((unsigned char)text[j+1]) == 't'
                 && std::tolower((unsigned char)text[j+2]) == 't'
                 && std::tolower((unsigned char)text[j+3]) == 'p')
                    return false;
            }
        }
    }
    return true;
}

void SecureSkinImport::sha256(const uint8_t* data, std::size_t len, uint8_t out32[32])
{
    static const uint32_t H0[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
    uint32_t h[8];
    for (int i = 0; i < 8; ++i) h[i] = H0[i];
    const uint64_t bits = uint64_t(len) * 8;
    std::vector<uint8_t> msg(data, data + len);
    msg.push_back(0x80);
    while (msg.size() % 64 != 56) msg.push_back(0);
    for (int i = 7; i >= 0; --i) msg.push_back(uint8_t((bits >> (8*i)) & 0xFF));
    for (size_t off = 0; off < msg.size(); off += 64)
        sha256Block(msg.data() + off, h);
    for (int i = 0; i < 8; ++i)
    {
        out32[4*i+0] = uint8_t(h[i] >> 24);
        out32[4*i+1] = uint8_t(h[i] >> 16);
        out32[4*i+2] = uint8_t(h[i] >> 8);
        out32[4*i+3] = uint8_t(h[i]);
    }
}

std::string SecureSkinImport::sha256Hex(const uint8_t* data, std::size_t len)
{
    static const char* kHex = "0123456789abcdef";
    uint8_t d[32];
    sha256(data, len, d);
    std::string s;
    s.reserve(64);
    for (int i = 0; i < 32; ++i)
    {
        s += kHex[d[i] >> 4];
        s += kHex[d[i] & 0xF];
    }
    return s;
}
} // namespace ppgu
