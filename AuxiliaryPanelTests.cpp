#define PPGU_CONTRACT_TEST 1
#include <cstdio>
#include <vector>
#include "UI/AuxiliaryPanelLayout.h"
#include "UI/MultiWindowWorkspace.h"

#define CHECK(cond) do { if (!(cond)) { std::fprintf(stderr, "CHECK FAIL %s:%d\n", __FILE__, __LINE__); return 1; } } while (0)

using namespace ppgu;

int main() {
    using L = AuxiliaryPanelLayout;
    std::vector<L::Rect> out;
    std::vector<L::PanelSpec> ps { {PanelId::FxEdit, 640, 480}, {PanelId::Settings, 520, 400} };
    CHECK(L::tileAll(ps, 1920, 1080, 100, out) == L::Status::Ok);
    CHECK(out.size() == 2);
    // duplicate panel ids -> invalidInput
    ps.push_back({PanelId::FxEdit, 100, 100});
    CHECK(L::tileAll(ps, 1920, 1080, 100, out) == L::Status::InvalidInput);
    // int64-safe dpi bounds
    std::vector<L::PanelSpec> one { {PanelId::About, 480, 360} };
    CHECK(L::tileAll(one, 1920, 1080, 300, out) == L::Status::Ok);
    CHECK(L::tileAll(one, 1920, 1080, 10, out) == L::Status::InvalidInput);
    // multi-window plan
    auto ws = MultiWindowWorkspace::defaults();
    ws[0].open = true; ws[1].open = true; ws[2].open = true;
    std::vector<L::Rect> plan;
    CHECK(MultiWindowWorkspace::planTiling(ws, 1920, 1080, 100, plan));
    CHECK(plan.size() == 3);
    std::puts("ppgu_panel_contracts: PASS");
    return 0;
}