#pragma once
#ifdef PPGU_CONTRACT_TEST
 #include <cstdint>
 #include <string>
 #include <vector>
#else
 #include <juce_gui_basics/juce_gui_basics.h>
#endif
#include <vector>
#include "AuxiliaryPanelLayout.h"
#include "ThemeManager.h"

namespace ppgu {

// S2 window shell: square X/restore buttons, persistence, palette propagation,
// tiling with reentrancy guard. Content survives close (state preserved).
class MultiWindowWorkspace {
public:
    struct WindowState { PanelId id; bool open=false; bool maximized=false; int w=640, h=480; };
    static std::vector<WindowState> defaults();
    // Tile plan for currently open windows (reentrancy-safe: pure function).
    static bool planTiling(const std::vector<WindowState>& state,
                           int screenW, int screenH, int dpi,
                           std::vector<AuxiliaryPanelLayout::Rect>& out);
    // Palette propagation: all open windows receive the same palette.
    static void propagatePalette(std::vector<WindowState>& ws, const Palette& p);
    static Palette lastPalette() { return Palette{}; }
};

} // namespace ppgu