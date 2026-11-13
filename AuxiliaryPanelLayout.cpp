#include "AuxiliaryPanelLayout.h"
#include <algorithm>

namespace ppgu {

static bool isDuplicate(const std::vector<PanelSpec>& ps) {
    for (size_t i = 0; i < ps.size(); ++i)
        for (size_t j = i + 1; j < ps.size(); ++j)
            if (ps[i].id == ps[j].id) return true;
    return false;
}

AuxiliaryPanelLayout::Status AuxiliaryPanelLayout::tileAll(
        std::vector<PanelSpec> panels, int screenW, int screenH, int dpi,
        std::vector<Rect>& out) {
    out.clear();
    if (panels.empty() || screenW <= 0 || screenH <= 0) return Status::InvalidInput;
    if (isDuplicate(panels)) return Status::InvalidInput;
    if (dpi < 50 || dpi > 400) return Status::InvalidInput;
    // int64 arithmetic (overflow-safe)
    const long long W64 = screenW, H64 = screenH;
    const long long scale = dpi / 100;
    const size_t n = panels.size();
    // Approved forms: 1xN or 2x3 (up to 6), else overflow pages of 6
    const size_t pageSize = std::min<size_t>(n <= 1 ? n : 6, 6);
    long long cols = (n == 1) ? 1 : (n <= 3 ? (long long)n : 3);
    long long rows = ((long long)n + cols - 1) / cols;
    if (cols * rows > 9) cols = 3, rows = 3; // overflow: first page of 9 max
    long long cellW = W64 / cols, cellH = H64 / rows;
    for (size_t i = 0; i < n && i < (size_t)(cols * rows); ++i) {
        Rect r;
        r.x = (int)((long long)(i % (size_t)cols) * cellW);
        r.y = (int)((long long)(i / (size_t)cols) * cellH);
        r.w = (int)cellW; r.h = (int)cellH;
        // scale-aware padding
        r.x += (int)(8 * scale); r.y += (int)(8 * scale);
        r.w -= (int)(16 * scale); r.h -= (int)(16 * scale);
        if (r.w < 32 || r.h < 32) return Status::InvalidInput;
        out.push_back(r);
    }
    return Status::Ok;
}

AuxiliaryPanelLayout::Status AuxiliaryPanelLayout::single(
        PanelId id, int screenW, int screenH, int dpi, Rect& out) {
    if (screenW <= 0 || screenH <= 0 || dpi < 50 || dpi > 400) return Status::InvalidInput;
    (void)id;
    const long long scale = dpi / 100;
    out.w = (int)((long long)screenW / 2 - 32 * scale);
    out.h = (int)((long long)screenH / 2 - 32 * scale);
    out.x = (int)((long long)screenW / 4);
    out.y = (int)((long long)screenH / 4);
    return (out.w >= 32 && out.h >= 32) ? Status::Ok : Status::InvalidInput;
}

} // namespace ppgu