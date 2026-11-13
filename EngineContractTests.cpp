#define PPGU_CONTRACT_TEST 1
#include <cassert>
#include <cstdio>
#include "Engine/SfzParser.h"
#include "Engine/PatternScheduler.h"
#include "Engine/ChordEngine.h"

#define CHECK(cond) do { if (!(cond)) { std::fprintf(stderr, "CHECK FAIL %s:%d\n", __FILE__, __LINE__); return 1; } } while (0)

using namespace ppgu;

int main() {
    // SfzParser: clamps and mapping
    {
        auto doc = SfzParser::parse(
            "<region> key=36 sample=a.wav seq_length=2 seq_position=2\n"
            "<region> lokey=40 hikey=39 lovel=90 hivel=30 sample=b.wav\n"
            "<region> key=200 sample=c.wav transpose=99\n");
        CHECK(doc.ok);
        CHECK(doc.regions.size() == 3);
        CHECK(doc.regions[1].lokey == 39 && doc.regions[1].hikey == 40);
        CHECK(doc.regions[1].lovel == 30 && doc.regions[1].hivel == 90);
        CHECK(doc.regions[2].key == 127);
        CHECK(doc.regions[2].transpose == 48);
        CHECK(doc.regions[0].seqPosition == 2 && doc.regions[0].seqLength == 2);
    }
    // PatternScheduler: determinism + dynamics
    {
        PatternScheduler s;
        s.setPattern(factoryPunkPattern());
        SchedulerParams p; p.dynamics = 1.2f; p.accentAmt = 1.5f;
        s.setParams(p);
        auto a = s.render(PlayMode::PATTERN, 7);
        auto b = s.render(PlayMode::PATTERN, 7);
        CHECK(a.size() == b.size() && !a.empty());
        for (size_t i = 0; i < a.size(); ++i) {
            CHECK(a[i].hit == b[i].hit);
            if (a[i].hit) CHECK(a[i].velocity > 0.0f);
        }
        auto stabs = s.render(PlayMode::PUNK_GROUP, 3);
        CHECK(stabs[0].palmMute == true);
    }
    // ChordEngine: circle of fifths
    {
        auto c = ChordEngine::diatonic(0, 0);
        CHECK(c.name == "C");
        CHECK(c.midi.size() == 3);
        auto d = ChordEngine::diatonic(0, 1);
        CHECK(d.name == "Dm");
        auto pc = ChordEngine::powerChord(4);
        CHECK(pc.name == "E5");
        CHECK(ChordEngine::circleNeighbour(0, 1) == 7);
    }
    std::puts("ppgu_engine_contracts: PASS");
    return 0;
}