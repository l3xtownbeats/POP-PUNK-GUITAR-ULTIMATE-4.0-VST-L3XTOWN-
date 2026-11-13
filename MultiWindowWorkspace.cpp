#include "MultiWindowWorkspace.h"

namespace ppgu {

std::vector<MultiWindowWorkspace::WindowState> MultiWindowWorkspace::defaults() {
    return {
        {PanelId::FxEdit, false, false, 640, 480},
        {PanelId::AutoChords, false, false, 560, 420},
        {PanelId::ImportGuitar, false, false, 600, 460},
        {PanelId::Curator, false, false, 620, 440},
        {PanelId::Settings, false, false, 520, 400},
        {PanelId::Performance, false, false, 560, 380},
        {PanelId::About, false, false, 480, 360}};
}

bool MultiWindowWorkspace::planTiling(const std::vector<WindowState>& state,
        int screenW, int screenH, int dpi,
        std::vector<AuxiliaryPanelLayout::Rect>& out) {
    std::vector<PanelSpec> open;
    for (auto& w : state)
        if (w.open && !w.maximized)
            open.push_back({w.id, w.w, w.h});
    if (open.empty()) { out.clear(); return true; }
    return AuxiliaryPanelLayout::tileAll(open, screenW, screenH, dpi, out)
           == AuxiliaryPanelLayout::Status::Ok;
}

void MultiWindowWorkspace::propagatePalette(std::vector<WindowState>&, const Palette&) {
    // In JUCE build: each window component receives theme look-and-feel refresh.
    // Contract path: no-op (pure), asserted in tests via defaults().
}

} // namespace ppgu