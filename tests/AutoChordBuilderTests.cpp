#include "AutoChordBuilder.h"

#include <array>
#include <iostream>

namespace
{
bool expect(const char* label, const ppgu::music::AutoChordResult& result,
            const std::array<int, 3>& expected, bool snapped)
{
    if (result.count == 3 && result.notes == expected && result.snappedToScale == snapped)
        return true;
    std::cerr << label << " failed: got [" << result.notes[0] << ", "
              << result.notes[1] << ", " << result.notes[2] << "] count="
              << result.count << " snapped=" << result.snappedToScale << "\n";
    return false;
}
}

int main()
{
    using ppgu::music::buildAutoChord;
    if (!expect("C major tonic", buildAutoChord(60, 0, 1), { 60, 64, 67 }, false)) return 1;
    if (!expect("D major-scale degree", buildAutoChord(62, 0, 1), { 62, 65, 69 }, false)) return 1;
    if (!expect("A natural minor tonic", buildAutoChord(57, 9, 2), { 57, 60, 64 }, false)) return 1;
    if (!expect("chromatic major triad", buildAutoChord(61, 0, 0), { 61, 65, 68 }, false)) return 1;
    if (!expect("out-of-scale root snap", buildAutoChord(61, 0, 1), { 60, 64, 67 }, true)) return 1;
    if (!expect("high-note range safety", buildAutoChord(126, 0, 1), { 113, 117, 120 }, true)) return 1;
    if (!expect("low-note range safety", buildAutoChord(0, 11, 1), { 11, 15, 18 }, true)) return 1;

    if (buildAutoChord(-1, 0, 1).count != 0
        || buildAutoChord(60, 12, 1).count != 0
        || buildAutoChord(60, 0, 3).count != 0)
    {
        std::cerr << "Invalid AutoChords input was not rejected.\n";
        return 1;
    }

    std::cout << "AutoChords builder tests passed.\n";
    return 0;
}
