#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace ppgu::sfz
{
struct Region
{
    std::filesystem::path samplePath;
    int lowKey = 0;
    int highKey = 127;
    int rootKey = 60;
    int lowVelocity = 0;
    int highVelocity = 127;
    int lowChannel = 1;
    int highChannel = 16;
    int group = 0;
    int offBy = 0;
    int seqLength = 1;
    int seqPosition = 1;
    float tuneCents = 0.0f;
    float volumeDb = 0.0f;
    float pan = 0.0f;
    std::uint64_t offset = 0;
    std::uint64_t end = 0;
    std::uint64_t loopStart = 0;
    std::uint64_t loopEnd = 0;
    std::string loopMode = "no_loop";
    float releaseSeconds = 0.15f;
    std::size_t sourceLine = 0;
};

struct Diagnostic
{
    std::size_t line = 0;
    std::string message;
};

struct ParseResult
{
    std::vector<Region> regions;
    std::vector<Diagnostic> errors;
    std::vector<Diagnostic> warnings;

    [[nodiscard]] bool succeeded() const noexcept { return errors.empty() && !regions.empty(); }
};

[[nodiscard]] ParseResult parseText(const std::string& text,
                                    const std::filesystem::path& sfzDirectory);
[[nodiscard]] ParseResult parseFile(const std::filesystem::path& sfzPath);
[[nodiscard]] std::vector<Diagnostic> validateSampleFiles(const ParseResult& parsed);
}
