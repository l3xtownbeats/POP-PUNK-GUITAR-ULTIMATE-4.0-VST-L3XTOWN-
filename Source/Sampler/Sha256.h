#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

namespace ppgu::sampler::crypto
{
class Sha256 final
{
public:
    void update(const std::uint8_t* data, std::size_t size)
    {
        bitCount_ += static_cast<std::uint64_t>(size) * 8U;
        for (std::size_t i = 0; i < size; ++i)
        {
            buffer_[bufferSize_++] = data[i];
            if (bufferSize_ == buffer_.size())
            {
                transform(buffer_.data());
                bufferSize_ = 0;
            }
        }
    }

    std::string finishHex()
    {
        const auto originalBitCount = bitCount_;
        const std::uint8_t marker = 0x80U;
        update(&marker, 1);
        const std::uint8_t zero = 0;
        while (bufferSize_ != 56U)
            update(&zero, 1);
        std::array<std::uint8_t, 8> lengthBytes {};
        for (unsigned int i = 0; i < 8; ++i)
            lengthBytes[7U - i] = static_cast<std::uint8_t>(originalBitCount >> (i * 8U));
        update(lengthBytes.data(), lengthBytes.size());

        std::ostringstream out;
        out << std::hex << std::setfill('0');
        for (const auto word : state_)
            out << std::setw(8) << word;
        return out.str();
    }

private:
    static constexpr std::array<std::uint32_t, 64> constants_ {
        0x428a2f98U,0x71374491U,0xb5c0fbcfU,0xe9b5dba5U,0x3956c25bU,0x59f111f1U,0x923f82a4U,0xab1c5ed5U,
        0xd807aa98U,0x12835b01U,0x243185beU,0x550c7dc3U,0x72be5d74U,0x80deb1feU,0x9bdc06a7U,0xc19bf174U,
        0xe49b69c1U,0xefbe4786U,0x0fc19dc6U,0x240ca1ccU,0x2de92c6fU,0x4a7484aaU,0x5cb0a9dcU,0x76f988daU,
        0x983e5152U,0xa831c66dU,0xb00327c8U,0xbf597fc7U,0xc6e00bf3U,0xd5a79147U,0x06ca6351U,0x14292967U,
        0x27b70a85U,0x2e1b2138U,0x4d2c6dfcU,0x53380d13U,0x650a7354U,0x766a0abbU,0x81c2c92eU,0x92722c85U,
        0xa2bfe8a1U,0xa81a664bU,0xc24b8b70U,0xc76c51a3U,0xd192e819U,0xd6990624U,0xf40e3585U,0x106aa070U,
        0x19a4c116U,0x1e376c08U,0x2748774cU,0x34b0bcb5U,0x391c0cb3U,0x4ed8aa4aU,0x5b9cca4fU,0x682e6ff3U,
        0x748f82eeU,0x78a5636fU,0x84c87814U,0x8cc70208U,0x90befffaU,0xa4506cebU,0xbef9a3f7U,0xc67178f2U
    };

    static std::uint32_t rotateRight(std::uint32_t value, unsigned int amount)
    {
        return (value >> amount) | (value << (32U - amount));
    }

    void transform(const std::uint8_t* block)
    {
        std::array<std::uint32_t, 64> words {};
        for (unsigned int i = 0; i < 16; ++i)
            words[i] = (static_cast<std::uint32_t>(block[i * 4U]) << 24U)
                     | (static_cast<std::uint32_t>(block[i * 4U + 1U]) << 16U)
                     | (static_cast<std::uint32_t>(block[i * 4U + 2U]) << 8U)
                     | static_cast<std::uint32_t>(block[i * 4U + 3U]);
        for (unsigned int i = 16; i < 64; ++i)
        {
            const auto s0 = rotateRight(words[i - 15U], 7U)
                          ^ rotateRight(words[i - 15U], 18U) ^ (words[i - 15U] >> 3U);
            const auto s1 = rotateRight(words[i - 2U], 17U)
                          ^ rotateRight(words[i - 2U], 19U) ^ (words[i - 2U] >> 10U);
            words[i] = words[i - 16U] + s0 + words[i - 7U] + s1;
        }

        auto a = state_[0]; auto b = state_[1]; auto c = state_[2]; auto d = state_[3];
        auto e = state_[4]; auto f = state_[5]; auto g = state_[6]; auto h = state_[7];
        for (unsigned int i = 0; i < 64; ++i)
        {
            const auto sum1 = rotateRight(e, 6U) ^ rotateRight(e, 11U) ^ rotateRight(e, 25U);
            const auto choose = (e & f) ^ (~e & g);
            const auto temp1 = h + sum1 + choose + constants_[i] + words[i];
            const auto sum0 = rotateRight(a, 2U) ^ rotateRight(a, 13U) ^ rotateRight(a, 22U);
            const auto majority = (a & b) ^ (a & c) ^ (b & c);
            const auto temp2 = sum0 + majority;
            h = g; g = f; f = e; e = d + temp1;
            d = c; c = b; b = a; a = temp1 + temp2;
        }
        state_[0] += a; state_[1] += b; state_[2] += c; state_[3] += d;
        state_[4] += e; state_[5] += f; state_[6] += g; state_[7] += h;
    }

    std::array<std::uint32_t, 8> state_ {
        0x6a09e667U,0xbb67ae85U,0x3c6ef372U,0xa54ff53aU,
        0x510e527fU,0x9b05688cU,0x1f83d9abU,0x5be0cd19U
    };
    std::array<std::uint8_t, 64> buffer_ {};
    std::size_t bufferSize_ = 0;
    std::uint64_t bitCount_ = 0;
};

inline std::string sha256String(std::string_view value)
{
    Sha256 hash;
    hash.update(reinterpret_cast<const std::uint8_t*>(value.data()), value.size());
    return hash.finishHex();
}

inline bool sha256File(const std::filesystem::path& path, std::string& result)
{
    std::ifstream input(path, std::ios::binary);
    if (!input)
        return false;
    Sha256 hash;
    std::array<char, 65536> chunk {};
    while (input)
    {
        input.read(chunk.data(), static_cast<std::streamsize>(chunk.size()));
        const auto count = input.gcount();
        if (count > 0)
            hash.update(reinterpret_cast<const std::uint8_t*>(chunk.data()),
                         static_cast<std::size_t>(count));
    }
    if (!input.eof())
        return false;
    result = hash.finishHex();
    return true;
}
}
