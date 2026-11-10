#include "PatternScheduler.h"

#include <array>
#include <cmath>
#include <iostream>

using namespace ppgu::performance;

int main()
{
    PatternScheduler scheduler;
    std::array<PatternEvent, 4> pattern {{
        { 0.0, 0.25, 60, 110, 1 },
        { 0.5, 0.25, 64, 95, 1 },
        { 1.0, 0.5, 67, 105, 1 },
        { 1.5, 0.25, 72, 100, 1 }
    }};
    std::array<ScheduledEvent, PatternScheduler::maximumEventsPerBlock> events {};

    // Host transport maps PPQ to sample positions with no block-size assumptions.
    const TransportSnapshot host { true, true, 120.0, 4.0 };
    const auto clock = scheduler.beginBlock(host, false, 90.0, 48000.0, 12000);
    if (!clock.running || !clock.usingHost || std::abs(clock.endPpq - 4.5) > 1.0e-9)
        return 1;

    const auto count = scheduler.schedule(pattern, 2.0, 4.0, clock, 48000.0, 12000, 0.25, 0, events);
    if (count != 2 || events[0].kind != EventKind::NoteOn || events[0].sampleOffset != 0
        || events[1].kind != EventKind::NoteOff || events[1].sampleOffset != 6000)
        return 2;

    // Host stop freezes PPQ and returns no scheduled events.
    const auto stopped = scheduler.beginBlock({ true, false, 120.0, 4.5 }, false, 90.0, 48000.0, 12000);
    if (stopped.running || stopped.startPpq != stopped.endPpq || !stopped.transportStopped)
        return 3;
    if (scheduler.schedule(pattern, 2.0, 4.0, stopped, 48000.0, 12000, 0.25, 0, events) != 0)
        return 4;

    // Internal fallback advances at configured BPM even without a host playhead.
    scheduler.reset(8.0);
    const auto internal = scheduler.beginBlock({ false, false, 0.0, 0.0 }, true, 60.0, 48000.0, 24000);
    if (!internal.running || internal.usingHost || std::abs(internal.endPpq - 8.5) > 1.0e-9)
        return 5;

    // Transposition, quantization and stable note-off-before-note-on ordering.
    std::array<PatternEvent, 2> retrigger {{
        { 0.0, 0.25, 60, 100, 1 },
        { 0.25, 0.25, 60, 100, 1 }
    }};
    const auto retriggerClock = scheduler.beginBlock({ false, false, 0.0, 0.0 }, true, 120.0, 48000.0, 24000);
    const auto retriggerCount = scheduler.schedule(retrigger, 1.0, 8.5, retriggerClock,
                                                    48000.0, 24000, 0.25, 12, events);
    if (retriggerCount < 3)
        return 6;
    bool foundOrderedRetrigger = false;
    for (std::size_t i = 1; i < retriggerCount; ++i)
        if (events[i - 1].sampleOffset == events[i].sampleOffset
            && events[i - 1].kind == EventKind::NoteOff
            && events[i].kind == EventKind::NoteOn)
            foundOrderedRetrigger = true;
    if (!foundOrderedRetrigger)
        return 7;

    // A long gate must retain its NoteOff when it crosses multiple blocks.
    PatternScheduler longGateScheduler;
    std::array<PatternEvent, 1> longGate {{{ 0.0, 0.75, 48, 100, 1 }}};
    auto longBlock = longGateScheduler.beginBlock({ true, true, 120.0, 0.0 }, false, 120.0, 48000.0, 6000);
    auto longCount = longGateScheduler.schedule(longGate, 1.0, 0.0, longBlock, 48000.0, 6000, 0.0, 0, events);
    if (longCount != 1 || events[0].kind != EventKind::NoteOn)
        return 9;
    for (int block = 1; block <= 2; ++block)
    {
        const double ppq = 0.25 * block;
        const auto next = longGateScheduler.beginBlock({ true, true, 120.0, ppq }, false, 120.0, 48000.0, 6000);
        if (longGateScheduler.schedule(longGate, 1.0, 0.0, next, 48000.0, 6000, 0.0, 0, events) != 0)
            return 10;
    }
    const auto finalGateBlock = longGateScheduler.beginBlock({ true, true, 120.0, 0.75 }, false, 120.0, 48000.0, 6000);
    const auto finalGateCount = longGateScheduler.schedule(longGate, 1.0, 0.0, finalGateBlock,
                                                            48000.0, 6000, 0.0, 0, events);
    if (finalGateCount != 1 || events[0].kind != EventKind::NoteOff || events[0].sampleOffset != 0)
        return 11;

 
    // A block crossing a pattern-cycle boundary must schedule the first
    // event of the next cycle at its exact sample offset.
    PatternScheduler wrapScheduler;
    std::array<PatternEvent, 1> wrapPattern {{{ 0.0, 0.125, 55, 100, 1 }}}; 
    const auto wrapClock = wrapScheduler.beginBlock({ true, true, 120.0, 0.9 }, false,
                                                      120.0, 48000.0, 4800);
    const auto wrapCount = wrapScheduler.schedule(wrapPattern, 1.0, 0.0, wrapClock,
                                                    48000.0, 4800, 0.0, 0, events);
    if (wrapCount != 1 || events[0].kind != EventKind::NoteOn
        || events[0].sampleOffset != 2400)
        return 12;
    const auto wrapNextClock = wrapScheduler.beginBlock({ true, true, 120.0, 1.1 }, false,
                                                          120.0, 48000.0, 4800);
    const auto wrapNextCount = wrapScheduler.schedule(wrapPattern, 1.0, 0.0, wrapNextClock,
                                                       48000.0, 4800, 0.0, 0, events);
    if (wrapNextCount != 1 || events[0].kind != EventKind::NoteOff
        || events[0].sampleOffset != 600)
        return 13;

    // Invalid BPM cannot create a zero/negative or unbounded clock.
    if (PatternScheduler::clampBpm(-50.0) != 120.0
        || PatternScheduler::clampBpm(900.0) != 120.0)
        return 8;

    // A host seek must be reported as a discontinuity so the processor can
    // release voices and discard stale pending NoteOff events.
    PatternScheduler seekScheduler;
    const auto seekA = seekScheduler.beginBlock({ true, true, 120.0, 12.0 }, false, 120.0, 48000.0, 12000);
    if (!seekA.running || seekA.transportDiscontinuity) return 14;
    const auto seekB = seekScheduler.beginBlock({ true, true, 120.0, 2.0 }, false, 120.0, 48000.0, 12000);
    if (!seekB.transportDiscontinuity) return 15;

    // Tempo and host block-size changes are reflected immediately without
    // accumulating drift from the previous block's size.
    PatternScheduler resizeScheduler;
    const auto resizeA = resizeScheduler.beginBlock({ true, true, 100.0, 0.0 }, false, 120.0, 48000.0, 64);
    const auto resizeB = resizeScheduler.beginBlock({ true, true, 140.0, resizeA.endPpq }, false, 120.0, 48000.0, 1024);
    if (std::abs(resizeB.endPpq - (resizeB.startPpq + 1024.0 * 140.0 / (60.0 * 48000.0))) > 1.0e-9)
        return 16;

    // An event just before the block end must never be rounded backward to
    // the final sample. Its offset is the first sample at/after its PPQ.
    PatternScheduler edgeScheduler;
    std::array<PatternEvent, 1> edgePattern {{{ 0.0, 0.01, 60, 100, 1 }}};
    const auto edgeClock = edgeScheduler.beginBlock({ true, true, 120.0, 0.0 }, false, 120.0, 48000.0, 64);
    const auto edgeCount = edgeScheduler.schedule(edgePattern, 1.0, 0.0, edgeClock, 48000.0, 64, 0.0, 0, events);
    if (edgeCount != 2 || events[0].sampleOffset != 0 || events[1].sampleOffset < 0)
        return 17;

    std::cout << "PatternScheduler tests passed: transport, seek, tempo/block changes, gates, ordering.\n";
    return 0;
}
