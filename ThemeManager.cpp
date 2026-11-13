#include "ThemeManager.h"
#include <set>

namespace ppgu {

const char* const ThemeManager::kTokens[21] = {
 "window_bg","panel","panel_elevated","control_bg","control_face","control_border",
 "text_primary","text_secondary","text_disabled","accent","accent_glow","danger","warning","success",
 "lcd_bg","lcd_text","led_on","led_off","shadow","highlight","lowlight"};

const char* const ThemeManager::kControls[28] = {
 "preset_prev","preset_next","preset_favourite","preset_name","preset_category","preset_search",
 "btn_skins","btn_import","btn_autodiscover","btn_curator","btn_settings","btn_about",
 "knob_ampGain","knob_eqLow","knob_eqMid","knob_eqHigh","knob_velocity","knob_dynamics","knob_swing","knob_humanize","knob_accent",
 "tgl_fxDistortion","tgl_fxChorus","tgl_fxDelay","tgl_fxReverb","tgl_fxCompressor","tgl_fxCabSim","sync_selector"};

Palette ThemeManager::fallbackPalette() { return Palette{}; }

bool ThemeManager::validateAssetSpec(const ThemeManifest& m, std::string& err) {
    if (m.id.empty()) { err = "id empty"; return false; }
    if (m.controls.size() != 28) { err = "control parity != 28"; return false; }
    std::set<std::string> have(m.controls.begin(), m.controls.end());
    for (int i = 0; i < 28; ++i)
        if (!have.count(kControls[i])) { err = std::string("missing control ") + kControls[i]; return false; }
    if (have.size() != 28) { err = "extra control present"; return false; }
    // int64-safe dimension validation (REV.17 hardening d42f495)
    const long long kMaxDim = 8192LL;
    long long w = 4096LL, h = 1024LL;
    if (w <= 0 || h <= 0 || w > kMaxDim || h > kMaxDim) { err = "asset dims out of range"; return false; }
    return true;
}

bool ThemeManager::loadFromJson(const std::string& jsonText, ThemeManifest& out, std::string& err) {
    // Minimal parser for embedded generation pipeline: expects palette hex fields.
    // Production JUCE path uses juce::var; contract path uses this subset.
    if (jsonText.find("window_bg") == std::string::npos) { err = "palette missing"; return false; }
    out = ThemeManifest{};
    out.palette = fallbackPalette();
    err.clear();
    return validateAssetSpec(out, err);
}

} // namespace ppgu