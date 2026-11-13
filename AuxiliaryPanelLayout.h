#pragma once
#include <cstdint>
#include <vector>
#ifdef PPGU_CONTRACT_TEST
 #include <cstdint>
#else
 #include <juce_gui_basics/juce_gui_basics.h>
#endif

namespace ppgu {

enum class PanelId { FxEdit, AutoChords, ImportGuitar, Curator, Settings, Performance, About };

struct PanelSpec { PanelId id; int width; int height; };

// Hardened geometry: approved 1xN / 2x3 forms, int64 math, overflow paging,
// duplicates -> invalidInput, DPI-safe.
class AuxiliaryPanelLayout {
public:
    struct Rect { int x = 0, y = 0, w = 0, h = 0; };
    enum class Status { Ok, InvalidInput };

    // Screens: virtual work area (width, height). dpi: 100=1.0.
    static Status tileAll(std::vector<PanelSpec> panels, int screenW, int screenH,
                          int dpi, std::vector<Rect>& out);
    static Status single(PanelId id, int screenW, int screenH, int dpi, Rect& out);
};

} // namespace ppgu