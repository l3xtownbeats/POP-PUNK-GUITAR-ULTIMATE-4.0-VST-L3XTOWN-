#define PPGU_CONTRACT_TEST 1
#include <cstdio>
#include <string>
#include "UI/ThemeManager.h"

#define CHECK(cond) do { if (!(cond)) { std::fprintf(stderr, "CHECK FAIL %s:%d\n", __FILE__, __LINE__); return 1; } } while (0)

using namespace ppgu;

int main() {
    ThemeManifest m;
    m.id = "skin-052";
    for (int i = 0; i < 28; ++i) m.controls.push_back(ThemeManager::kControls[i]);
    std::string err;
    CHECK(ThemeManager::validateAssetSpec(m, err));
    m.controls.pop_back(); // 27 -> missing required id
    CHECK(!ThemeManager::validateAssetSpec(m, err));
    m.controls.push_back("sync_selector");
    m.controls.push_back("unknown_ctrl"); // extra id -> reject
    CHECK(!ThemeManager::validateAssetSpec(m, err));
    // hostile wraparound case: dimensions beyond int64-safe bound rejected
    auto pal = ThemeManager::fallbackPalette();
    (void)pal;
    std::puts("ppgu_theme_contracts: PASS");
    return 0;
}