#include "MidiPreviewScheduler.h"
#include <array>
#include <iostream>

using namespace ppgu::performance;

static bool eventIs(const PreviewEvent& e, int offset, EventKind kind,
                    int note, int velocity, int channel)
{
    return e.sampleOffset == offset && e.kind == kind && e.note == note
        && e.velocity == velocity && e.channel == channel;
}

int main()
{
    MidiPreviewScheduler scheduler;
    std::array<PreviewEvent, MidiPreviewScheduler::maximumEventsPerBlock> events {};
    const std::array<int, 3> chord {{ 60, 64, 67 }};

    if (!scheduler.enqueueChord(chord.data(), chord.size(), 105, 100, 2)) return 1;
    auto count = scheduler.processBlock(64, events);
    if (count != 3 || scheduler.activeNoteCount() != 3) return 2;
    if (!eventIs(events[0], 0, EventKind::NoteOn, 60, 105, 2)
        || !eventIs(events[1], 0, EventKind::NoteOn, 64, 105, 2)
        || !eventIs(events[2], 0, EventKind::NoteOn, 67, 105, 2)) return 3;

    count = scheduler.processBlock(36, events);
    if (count != 0 || scheduler.activeNoteCount() != 3) return 4;
    count = scheduler.processBlock(1, events);
    if (count != 3 || scheduler.activeNoteCount() != 0) return 5;
    if (!eventIs(events[0], 0, EventKind::NoteOff, 60, 0, 2)
        || !eventIs(events[1], 0, EventKind::NoteOff, 64, 0, 2)
        || !eventIs(events[2], 0, EventKind::NoteOff, 67, 0, 2)) return 6;

    // A gate shorter than one block emits its NoteOff at the exact in-block offset.
    if (!scheduler.enqueueChord(chord.data(), chord.size(), 100, 12, 1)) return 20;
    count = scheduler.processBlock(32, events);
    if (count != 6 || scheduler.activeNoteCount() != 0) return 21;
    if (!eventIs(events[0], 0, EventKind::NoteOn, 60, 100, 1)
        || !eventIs(events[1], 0, EventKind::NoteOn, 64, 100, 1)
        || !eventIs(events[2], 0, EventKind::NoteOn, 67, 100, 1)
        || !eventIs(events[3], 12, EventKind::NoteOff, 60, 0, 1)
        || !eventIs(events[4], 12, EventKind::NoteOff, 64, 0, 1)
        || !eventIs(events[5], 12, EventKind::NoteOff, 67, 0, 1)) return 22;

    // A replacement chord must release the old notes before starting the new one.
    if (!scheduler.enqueueChord(chord.data(), chord.size(), 100, 500, 1)) return 7;
    count = scheduler.processBlock(32, events);
    if (count != 3) return 8;
    const std::array<int, 2> replacement {{ 62, 65 }};
    if (!scheduler.enqueueChord(replacement.data(), replacement.size(), 90, 80, 1)) return 9;
    count = scheduler.processBlock(16, events);
    if (count != 5 || scheduler.activeNoteCount() != 2) return 10;
    if (!eventIs(events[0], 0, EventKind::NoteOff, 60, 0, 1)
        || !eventIs(events[1], 0, EventKind::NoteOff, 64, 0, 1)
        || !eventIs(events[2], 0, EventKind::NoteOff, 67, 0, 1)
        || !eventIs(events[3], 0, EventKind::NoteOn, 62, 90, 1)
        || !eventIs(events[4], 0, EventKind::NoteOn, 65, 90, 1)) return 11;

    if (!scheduler.enqueueStop()) return 12;
    count = scheduler.processBlock(16, events);
    if (count != 2 || scheduler.activeNoteCount() != 0) return 13;
    if (!eventIs(events[0], 0, EventKind::NoteOff, 62, 0, 1)
        || !eventIs(events[1], 0, EventKind::NoteOff, 65, 0, 1)) return 14;

    if (scheduler.enqueueChord(nullptr, 1, 100, 100)
        || scheduler.enqueueChord(chord.data(), 0, 100, 100)
        || scheduler.enqueueChord(chord.data(), 7, 100, 100)
        || scheduler.enqueueChord(chord.data(), 3, 0, 100)
        || scheduler.enqueueChord(chord.data(), 3, 100, 0)
        || scheduler.enqueueChord(chord.data(), 3, 100, 100, 17)) return 15;
    const std::array<int, 1> invalid {{ 128 }};
    if (scheduler.enqueueChord(invalid.data(), invalid.size(), 100, 100)) return 16;

    // Queue overflow is explicit to the caller; no silent command overwrite.
    MidiPreviewScheduler bounded;
    for (std::size_t i = 0; i < MidiPreviewScheduler::commandQueueCapacity - 1; ++i)
        if (!bounded.enqueueChord(chord.data(), 3, 100, 100)) return 17;
    if (bounded.enqueueStop()) return 18;

    // A burst is coalesced to the latest explicit intent; capacity stays bounded.
    MidiPreviewScheduler coalesced;
    if (!coalesced.enqueueChord(chord.data(), 3, 100, 100)) return 23;
    if (!coalesced.enqueueChord(replacement.data(), 2, 90, 100)) return 24;
    if (!coalesced.enqueueStop()) return 25;
    count = coalesced.processBlock(64, events);
    if (count != 0 || coalesced.activeNoteCount() != 0) return 26;
    if (!coalesced.enqueueChord(replacement.data(), 2, 90, 100)) return 27;
    if (!coalesced.enqueueChord(chord.data(), 3, 100, 100)) return 28;
    count = coalesced.processBlock(64, events);
    if (count != 3 || coalesced.activeNoteCount() != 3) return 29;
    if (!eventIs(events[0], 0, EventKind::NoteOn, 60, 100, 1)
        || !eventIs(events[1], 0, EventKind::NoteOn, 64, 100, 1)
        || !eventIs(events[2], 0, EventKind::NoteOn, 67, 100, 1)) return 30;

    std::cout << "MIDI preview scheduler contract tests passed\n";
    return 0;
}
