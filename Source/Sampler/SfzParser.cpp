#include "SfzParser.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <sstream>
#include <system_error>

namespace ppgu::sfz
{
namespace
{
struct Opcodes
{
    Region values;
    bool hasSample = false;
};

std::vector<std::string> tokenize(const std::string& line)
{
    std::vector<std::string> result;
    std::string token;
    bool quoted = false;
    for (std::size_t i = 0; i < line.size(); ++i)
    {
        const char c = line[i];
        if (c == '"' && (i == 0 || line[i - 1] != '\\'))
        {
            quoted = !quoted;
            continue;
        }
        if (!quoted && std::isspace(static_cast<unsigned char>(c)))
        {
            if (!token.empty()) { result.push_back(token); token.clear(); }
            continue;
        }
        token.push_back(c);
    }
    if (!token.empty()) result.push_back(token);
    return result;
}

bool parseInteger(const std::string& text, int& value)
{
    try
    {
        std::size_t used = 0;
        const int parsed = std::stoi(text, &used);
        if (used != text.size()) return false;
        value = parsed;
        return true;
    }
    catch (...) { return false; }
}

bool parseUnsigned(const std::string& text, std::uint64_t& value)
{
    if (text.empty() || text.front() == '-') return false;
    try
    {
        std::size_t used = 0;
        const auto parsed = std::stoull(text, &used);
        if (used != text.size()) return false;
        value = parsed;
        return true;
    }
    catch (...) { return false; }
}

bool parseFloat(const std::string& text, float& value)
{
    try
    {
        std::size_t used = 0;
        const float parsed = std::stof(text, &used);
        if (used != text.size() || !std::isfinite(parsed)) return false;
        value = parsed;
        return true;
    }
    catch (...) { return false; }
}

bool parseKey(const std::string& text, int& value)
{
    if (parseInteger(text, value))
        return value >= 0 && value <= 127;

    // SFZ note names may use signed, multi-digit octave suffixes (for example
    // C-1 and G9). Splitting off only the final character rejects valid
    // negative-octave notes and can misread multi-digit octaves.
    static const std::string names[] = {
        "C#", "D#", "F#", "G#", "A#", "C", "D", "E", "F", "G", "A", "B"
    };

    for (const auto& noteName : names)
    {
        if (text.size() <= noteName.size()
            || text.compare(0, noteName.size(), noteName) != 0)
            continue;

        int octave = 0;
        if (!parseInteger(text.substr(noteName.size()), octave))
            return false;

        static const std::string chromatic[] = {
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
        };
        int noteIndex = -1;
        for (int i = 0; i < 12; ++i)
            if (noteName == chromatic[i])
            {
                noteIndex = i;
                break;
            }

        const long long midi = (static_cast<long long>(octave) + 1LL) * 12LL + noteIndex;
        if (noteIndex < 0 || midi < 0 || midi > 127)
            return false;

        value = static_cast<int>(midi);
        return true;
    }

    return false;
}

bool pathStaysInside(const std::filesystem::path& base, const std::filesystem::path& candidate)
{
    // Lexical checks alone do not stop a library from escaping through a
    // symlink inside the SFZ folder. Resolve every existing path component
    // before checking containment; weakly_canonical also supports not-yet-
    // created sample files while resolving existing parent symlinks.
    std::error_code error;
    const auto canonicalBase = std::filesystem::weakly_canonical(base, error);
    if (error)
        return false;

    error.clear();
    const auto canonicalCandidate = std::filesystem::weakly_canonical(candidate, error);
    if (error)
        return false;

    const auto relative = canonicalCandidate.lexically_relative(canonicalBase);
    if (relative.empty())
        return canonicalCandidate == canonicalBase;
    if (relative.is_absolute())
        return false;

    for (const auto& component : relative)
        if (component == "..")
            return false;

    return true;
}

void applyOpcode(Opcodes& target, const std::string& opcode, const std::string& value,
                 const std::filesystem::path& libraryRoot,
                 const std::filesystem::path& sampleBase,
                 std::size_t line, ParseResult& result)
{
    auto bad = [&] { result.errors.push_back({ line, "Invalid value for " + opcode + ": " + value }); };
    int integer = 0;
    float real = 0.0f;
    if (opcode == "sample")
    {
        if (value.empty()) { bad(); return; }
        const std::filesystem::path supplied(value);
        if (supplied.is_absolute()) { result.errors.push_back({ line, "Absolute sample paths are not portable." }); return; }
        const auto resolved = (sampleBase / supplied).lexically_normal();
        if (!pathStaysInside(libraryRoot.lexically_normal(), resolved))
        {
            result.errors.push_back({ line, "Sample path escapes the SFZ library directory." });
            return;
        }
        target.values.samplePath = resolved;
        target.hasSample = true;
    }
    else if (opcode == "key" || opcode == "lokey" || opcode == "hikey" || opcode == "pitch_keycenter")
    {
        if (!parseKey(value, integer)) { bad(); return; }
        if (opcode == "key") target.values.lowKey = target.values.highKey = target.values.rootKey = integer;
        if (opcode == "lokey") target.values.lowKey = integer;
        if (opcode == "hikey") target.values.highKey = integer;
        if (opcode == "pitch_keycenter") target.values.rootKey = integer;
    }
    else if (opcode == "lovel" || opcode == "hivel"
        || opcode == "xfin_lovel" || opcode == "xfin_hivel"
        || opcode == "xfout_lovel" || opcode == "xfout_hivel")
    {
        if (!parseInteger(value, integer) || integer < 0 || integer > 127) { bad(); return; }
        if (opcode == "lovel") target.values.lowVelocity = integer;
        else if (opcode == "hivel") target.values.highVelocity = integer;
        else if (opcode == "xfin_lovel") target.values.xfinLowVelocity = integer;
        else if (opcode == "xfin_hivel") target.values.xfinHighVelocity = integer;
        else if (opcode == "xfout_lovel") target.values.xfoutLowVelocity = integer;
        else target.values.xfoutHighVelocity = integer;
    }
    else if (opcode == "lochan" || opcode == "hichan" || opcode == "group" || opcode == "off_by"
        || opcode == "seq_length" || opcode == "seq_position")
    {
        if (!parseInteger(value, integer) || integer < 0
            || ((opcode == "lochan" || opcode == "hichan") && (integer < 1 || integer > 16)))
        {
            bad();
            return;
        }
        if (opcode == "lochan") target.values.lowChannel = integer;
        if (opcode == "hichan") target.values.highChannel = integer;
        if (opcode == "group") target.values.group = integer;
        if (opcode == "off_by") target.values.offBy = integer;
        if (opcode == "seq_length") target.values.seqLength = integer;
        if (opcode == "seq_position") target.values.seqPosition = integer;
    }
    else if (opcode == "tune" || opcode == "volume" || opcode == "pan" || opcode == "ampeg_release")
    {
        if (!parseFloat(value, real)) { bad(); return; }
        // Bound values before they reach pow()/Decibels::decibelsToGain()
        // in the audio engine. SFZ files are external input and must not be
        // able to create infinities or denormal/overflowing playback rates.
        if (opcode == "tune")
        {
            if (real < -9600.0f || real > 9600.0f) { bad(); return; }
            target.values.tuneCents = real;
        }
        if (opcode == "volume")
        {
            if (real < -144.0f || real > 24.0f) { bad(); return; }
            target.values.volumeDb = real;
        }
        if (opcode == "pan") target.values.pan = std::clamp(real, -100.0f, 100.0f) / 100.0f;
        if (opcode == "ampeg_release") target.values.releaseSeconds = std::clamp(real, 0.0f, 30.0f);
    }
    else if (opcode == "offset" || opcode == "end" || opcode == "loop_start" || opcode == "loop_end")
    {
        std::uint64_t frames = 0;
        if (!parseUnsigned(value, frames)) { bad(); return; }
        if (opcode == "offset") target.values.offset = frames;
        else if (opcode == "end") target.values.end = frames;
        else if (opcode == "loop_start") target.values.loopStart = frames;
        else target.values.loopEnd = frames;
    }
    else if (opcode == "loop_mode")
    {
        static const std::vector<std::string> allowed { "no_loop", "one_shot", "loop_continuous", "loop_sustain", "loop_pingpong" };
        if (std::find(allowed.begin(), allowed.end(), value) == allowed.end()) { bad(); return; }
        target.values.loopMode = value;
    }
    else
        result.warnings.push_back({ line, "Unsupported SFZ opcode ignored: " + opcode });
}
}

ParseResult parseText(const std::string& text, const std::filesystem::path& sfzDirectory)
{
    ParseResult result;
    if (text.size() > maximumSfzFileBytes)
    {
        result.errors.push_back({ 0, "SFZ text exceeds the 16 MiB parser safety limit." });
        return result;
    }
    const auto base = std::filesystem::absolute(sfzDirectory).lexically_normal();
    auto sampleBase = base;
    Opcodes global;
    Opcodes master = global;
    Opcodes group = master;
    Opcodes region;
    bool inRegion = false;
    bool inControl = false;
    bool inGlobal = false;
    bool inMaster = false;
    bool ignoreSection = false;
    bool regionLimitReported = false;
    std::size_t lineNumber = 0;
    auto flushRegion = [&]
    {
        if (inRegion && region.hasSample)
        {
            if (result.regions.size() >= maximumSfzRegions)
            {
                if (!regionLimitReported)
                    result.errors.push_back({ lineNumber, "SFZ region count exceeds the 65536-region safety limit." });
                regionLimitReported = true;
                inRegion = false;
                return;
            }
            const auto& item = region.values;
            if (item.lowKey > item.highKey || item.lowVelocity > item.highVelocity
                || item.seqLength < 1 || item.seqLength > 256
                || item.seqPosition < 1 || item.seqPosition > item.seqLength
                || ((item.xfinLowVelocity >= 0) != (item.xfinHighVelocity >= 0))
                || ((item.xfoutLowVelocity >= 0) != (item.xfoutHighVelocity >= 0))
                || (item.xfinLowVelocity >= 0 && item.xfinLowVelocity > item.xfinHighVelocity)
                || (item.xfoutLowVelocity >= 0 && item.xfoutLowVelocity > item.xfoutHighVelocity)
                || item.lowChannel > item.highChannel
                || item.rootKey < item.lowKey || item.rootKey > item.highKey
                || (item.loopEnd != 0 && item.loopStart >= item.loopEnd))
                result.errors.push_back({ item.sourceLine, "Region has invalid key/velocity ranges or root key." });
            else
                result.regions.push_back(item);
        }
        inRegion = false;
    };

    std::istringstream input(text);
    std::string line;
    while (std::getline(input, line))
    {
        ++lineNumber;
        if (lineNumber > maximumSfzLines)
        {
            result.errors.push_back({ lineNumber, "SFZ exceeds the 262144-line parser safety limit." });
            break;
        }
        if (line.size() > maximumSfzLineLength)
        {
            result.errors.push_back({ lineNumber, "SFZ line exceeds the 64 KiB parser safety limit." });
            continue;
        }
        bool quoted = false;
        std::size_t comment = std::string::npos;
        for (std::size_t i = 0; i + 1 < line.size(); ++i)
        {
            if (line[i] == '"' && (i == 0 || line[i - 1] != '\\')) quoted = !quoted;
            if (!quoted && line[i] == '/' && line[i + 1] == '/') { comment = i; break; }
        }
        if (comment != std::string::npos) line.erase(comment);
        auto tokens = tokenize(line);
        if (tokens.size() > maximumSfzTokensPerLine)
        {
            result.errors.push_back({ lineNumber, "SFZ line exceeds the 4096-token parser safety limit." });
            continue;
        }
        for (const auto& token : tokens)
        {
            if (token.size() >= 3 && token.front() == '<' && token.back() == '>')
            {
                flushRegion();
                const auto section = token.substr(1, token.size() - 2);
                if (section == "control")
                {
                    inRegion = false;
                    inControl = true;
                    inGlobal = false;
                    inMaster = false;
                    ignoreSection = false;
                }
                else if (section == "global")
                {
                    inControl = false;
                    global = {};
                    master = global;
                    group = master;
                    inRegion = false;
                    inGlobal = true;
                    inMaster = false;
                    ignoreSection = false;
                }
                else if (section == "master")
                {
                    inControl = false;
                    master = global;
                    group = master;
                    inRegion = false;
                    inGlobal = false;
                    inMaster = true;
                    ignoreSection = false;
                }
                else if (section == "group")
                {
                    inControl = false;
                    group = master;
                    inRegion = false;
                    inGlobal = false;
                    inMaster = false;
                    ignoreSection = false;
                }
                else if (section == "region")
                {
                    inControl = false;
                    if (result.regions.size() >= maximumSfzRegions)
                    {
                        if (!regionLimitReported)
                            result.errors.push_back({ lineNumber, "SFZ region count exceeds the 65536-region safety limit." });
                        regionLimitReported = true;
                        inRegion = false;
                        ignoreSection = true;
                        continue;
                    }
                    region = group;
                    region.values.sourceLine = lineNumber;
                    inRegion = true;
                    inGlobal = false;
                    inMaster = false;
                    ignoreSection = false;
                }
                else
                {
                    inControl = false;
                    inRegion = false;
                    inGlobal = false;
                    inMaster = false;
                    ignoreSection = true;
                    result.warnings.push_back({ lineNumber, "Unsupported SFZ section ignored: " + section });
                }
                continue;
            }

            const auto equals = token.find('=');
            if (equals == std::string::npos || equals == 0)
            {
                result.errors.push_back({ lineNumber, "Expected opcode=value, got: " + token });
                continue;
            }
            if (ignoreSection)
                continue;
            const auto opcode = token.substr(0, equals);
            const auto value = token.substr(equals + 1);
            if (inControl)
            {
                if (opcode == "default_path")
                {
                    const std::filesystem::path supplied(value);
                    if (supplied.empty() || supplied.is_absolute())
                    {
                        result.errors.push_back({ lineNumber, "default_path must be a non-empty relative directory." });
                        continue;
                    }
                    const auto resolved = (base / supplied).lexically_normal();
                    if (!pathStaysInside(base, resolved))
                        result.errors.push_back({ lineNumber, "default_path escapes the SFZ library directory." });
                    else
                        sampleBase = resolved;
                }
                continue;
            }

            Opcodes* destination = inRegion ? &region
                : (inGlobal ? &global : (inMaster ? &master : &group));
            applyOpcode(*destination, opcode, value, base, sampleBase, lineNumber, result);
            if (inGlobal)
            {
                master = global;
                group = master;
            }
            else if (inMaster)
                group = master;

        }
    }

    flushRegion();

    if (result.regions.empty() && result.errors.empty())
        result.errors.push_back({ 0, "No playable <region> with a sample opcode was found." });
    return result;
}

ParseResult parseFile(const std::filesystem::path& sfzPath)
{
    ParseResult result;
    std::error_code fileError;
    if (!std::filesystem::is_regular_file(sfzPath, fileError) || fileError)
    {
        result.errors.push_back({ 0, "SFZ path is not a readable regular file: " + sfzPath.string() });
        return result;
    }
    const auto fileBytes = std::filesystem::file_size(sfzPath, fileError);
    if (fileError)
    {
        result.errors.push_back({ 0, "Could not read SFZ file size: " + fileError.message() });
        return result;
    }
    if (fileBytes > maximumSfzFileBytes)
    {
        result.errors.push_back({ 0, "SFZ file exceeds the 16 MiB parser safety limit." });
        return result;
    }
    const auto initialWriteTime = std::filesystem::last_write_time(sfzPath, fileError);
    if (fileError)
    {
        result.errors.push_back({ 0, "Could not inspect SFZ modification time: " + fileError.message() });
        return result;
    }

    std::ifstream stream(sfzPath, std::ios::binary);
    if (!stream)
    {
        result.errors.push_back({ 0, "Could not open SFZ file: " + sfzPath.string() });
        return result;
    }

    // Read exactly the indexed size. A file that grows/shrinks during the
    // read must not bypass the parser's memory bound or produce a partial map.
    std::string contents(static_cast<std::size_t>(fileBytes), '\0');
    if (fileBytes > 0)
        stream.read(contents.data(), static_cast<std::streamsize>(fileBytes));
    if (stream.bad() || static_cast<std::uint64_t>(stream.gcount()) != fileBytes)
    {
        result.errors.push_back({ 0, "I/O error or concurrent truncation while reading SFZ file: " + sfzPath.string() });
        return result;
    }
    char unexpectedTrailingByte = 0;
    if (stream.get(unexpectedTrailingByte))
    {
        result.errors.push_back({ 0, "SFZ file grew while being read; retry the load." });
        return result;
    }

    fileError.clear();
    const auto finalBytes = std::filesystem::file_size(sfzPath, fileError);
    if (fileError || finalBytes != fileBytes)
    {
        result.errors.push_back({ 0, "SFZ file size changed while being read; retry the load." });
        return result;
    }
    fileError.clear();
    const auto finalWriteTime = std::filesystem::last_write_time(sfzPath, fileError);
    if (fileError || finalWriteTime != initialWriteTime)
    {
        result.errors.push_back({ 0, "SFZ file changed while being read; retry the load." });
        return result;
    }
    return parseText(contents, sfzPath.parent_path());
}

std::vector<Diagnostic> validateSampleFiles(const ParseResult& parsed)
{
    std::vector<Diagnostic> issues;
    for (const auto& region : parsed.regions)
    {
        std::error_code error;
        const bool regularFile = std::filesystem::is_regular_file(region.samplePath, error);
        if (error || !regularFile)
            issues.push_back({ region.sourceLine, "Sample file not found or could not be inspected: "
                + region.samplePath.string() });
    }
    return issues;
}
}
