#include "SfzParser.h"

#include <filesystem>
#include <iostream>

using namespace ppgu::sfz;

#define CHECK(condition) do { if (!(condition)) { std::cerr << "CHECK failed: " #condition << " at " << __FILE__ << ":" << __LINE__ << "\n"; return 1; } } while (false)

int main()
{
    const auto base = std::filesystem::temp_directory_path() / "ppgu-sfz-parser-tests";
    const std::string sfz = R"SFZ(
<global> ampeg_release=0.25
<group> lokey=48 hikey=72 lovel=1 hivel=90 volume=-2
<region> sample="guitar samples/C4.wav" pitch_keycenter=C4
<region> sample=G4.wav key=67 tune=4 pan=-25 loop_mode=one_shot
<group> key=60 lovel=91 hivel=127
<region> sample=G4-hard.wav
)SFZ";

    auto parsed = parseText(sfz, base);
    CHECK(parsed.succeeded());
    CHECK(parsed.regions.size() == 3);
    CHECK(parsed.regions[0].lowKey == 48 && parsed.regions[0].highKey == 72);
    CHECK(parsed.regions[0].rootKey == 60);
    CHECK(parsed.regions[0].lowVelocity == 1 && parsed.regions[0].highVelocity == 90);
    CHECK(parsed.regions[0].releaseSeconds == 0.25f);
    CHECK(parsed.regions[0].samplePath.filename() == "C4.wav");
    CHECK(parsed.regions[1].lowKey == 67 && parsed.regions[1].rootKey == 67);
    CHECK(parsed.regions[1].tuneCents == 4.0f);
    CHECK(parsed.regions[1].pan == -0.25f);
    CHECK(parsed.regions[1].loopMode == "one_shot");
    CHECK(parsed.regions[2].lowVelocity == 91 && parsed.regions[2].highVelocity == 127);

    const auto rrMetadata = parseText(
        "<group> group=12 seq_length=3\n"
        "<region> sample=rr1.wav key=60 seq_position=1\n"
        "<region> sample=rr2.wav key=60 seq_position=2\n"
        "<region> sample=rr3.wav key=60 seq_position=3\n", base);
    CHECK(rrMetadata.succeeded());
    CHECK(rrMetadata.regions.size() == 3);
    CHECK(rrMetadata.regions[0].seqLength == 3 && rrMetadata.regions[0].seqPosition == 1);
    CHECK(rrMetadata.regions[1].seqLength == 3 && rrMetadata.regions[1].seqPosition == 2);
    CHECK(rrMetadata.regions[2].seqLength == 3 && rrMetadata.regions[2].seqPosition == 3);
    const auto velocityCrossfade = parseText(
        "<region> sample=xfade.wav key=60 lovel=1 hivel=127 "
        "xfin_lovel=20 xfin_hivel=50 xfout_lovel=80 xfout_hivel=110\\n", base);
    CHECK(velocityCrossfade.succeeded());
    CHECK(velocityCrossfade.regions[0].xfinLowVelocity == 20);
    CHECK(velocityCrossfade.regions[0].xfinHighVelocity == 50);
    CHECK(velocityCrossfade.regions[0].xfoutLowVelocity == 80);
    CHECK(velocityCrossfade.regions[0].xfoutHighVelocity == 110);
    const auto incompleteCrossfade = parseText(
        "<region> sample=xfade.wav key=60 xfin_lovel=20\\n", base);
    CHECK(!incompleteCrossfade.succeeded());
    const auto invertedCrossfade = parseText(
        "<region> sample=xfade.wav key=60 xfout_lovel=100 xfout_hivel=80\\n", base);
    CHECK(!invertedCrossfade.succeeded());

    const auto rrInvalidZero = parseText(
        "<region> sample=rr.wav key=60 seq_length=0 seq_position=1\n", base);
    CHECK(!rrInvalidZero.succeeded());
    const auto rrInvalidPosition = parseText(
        "<region> sample=rr.wav key=60 seq_length=2 seq_position=3\n", base);
    CHECK(!rrInvalidPosition.succeeded());

    auto traversal = parseText("<region> sample=../../outside.wav key=60\n", base);
    CHECK(!traversal.succeeded());
    CHECK(!traversal.errors.empty());

    auto invalid = parseText("<region> sample=x.wav lokey=70 hikey=40 pitch_keycenter=60\n", base);
    CHECK(!invalid.succeeded());

    const auto excessiveTune = parseText("<region> sample=x.wav key=60 tune=9601\n", base);
    CHECK(!excessiveTune.succeeded());
    const auto excessiveGain = parseText("<region> sample=x.wav key=60 volume=25\n", base);
    CHECK(!excessiveGain.succeeded());
    const auto nonFiniteTune = parseText("<region> sample=x.wav key=60 tune=nan\n", base);
    CHECK(!nonFiniteTune.succeeded());

    auto unsupported = parseText("<region> sample=x.wav key=60 cutoff=12000\n", base);
    CHECK(unsupported.succeeded());
    CHECK(!unsupported.warnings.empty());

    const std::string metaSteelChannelExcerpt = R"SFZ(
<global> ampeg_release=0.05
<master> lochan=1 hichan=1
<group> group=1 off_by=1 lovel=1 hivel=127
<region> key=60 sample=StringLowE.wav
<master> lochan=2 hichan=2
<group> group=2 off_by=2 lovel=1 hivel=127
<region> key=60 sample=StringA.wav
)SFZ";
    const auto channelMap = parseText(metaSteelChannelExcerpt, base);
    CHECK(channelMap.succeeded());
    CHECK(channelMap.regions.size() == 2);
    CHECK(channelMap.regions[0].lowChannel == 1 && channelMap.regions[0].highChannel == 1);
    CHECK(channelMap.regions[0].group == 1 && channelMap.regions[0].offBy == 1);
    CHECK(channelMap.regions[1].lowChannel == 2 && channelMap.regions[1].highChannel == 2);
    CHECK(channelMap.regions[1].group == 2 && channelMap.regions[1].offBy == 2);

    const auto loopMap = parseText("<region> sample=loop.wav key=60 loop_mode=loop_sustain loop_start=32 loop_end=400\n", base);
    CHECK(loopMap.succeeded());
    CHECK(loopMap.regions[0].loopMode == "loop_sustain");
    CHECK(loopMap.regions[0].loopStart == 32 && loopMap.regions[0].loopEnd == 400);
    const auto pingPongMap = parseText("<region> sample=pingpong.wav key=60 loop_mode=loop_pingpong loop_start=16 loop_end=480\n", base);
    CHECK(pingPongMap.succeeded());
    CHECK(pingPongMap.regions[0].loopMode == "loop_pingpong");
    CHECK(pingPongMap.regions[0].loopStart == 16 && pingPongMap.regions[0].loopEnd == 480);

    const auto defaultPathMap = parseText(
        "<control> default_path=\"samples folder/\"\n"
        "<region> sample=steel.wav key=60\n", base);
    CHECK(defaultPathMap.succeeded());
    CHECK(defaultPathMap.regions[0].samplePath
        == (std::filesystem::absolute(base) / "samples folder" / "steel.wav").lexically_normal());

    const auto defaultPathTraversal = parseText(
        "<control> default_path=../../outside\n"
        "<region> sample=steel.wav key=60\n", base);
    CHECK(!defaultPathTraversal.succeeded());
    CHECK(!defaultPathTraversal.errors.empty());

    std::cout << "SFZ parser tests passed: inheritance, note names, velocity zones, metadata, path safety, validation.\n";
    return 0;
}
