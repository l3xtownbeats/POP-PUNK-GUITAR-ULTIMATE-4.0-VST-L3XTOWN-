#pragma once
#include <array>
#include <string>
#include <vector>
#ifdef JUCE_MODULE_AVAILABLE_juce_gui_basics
 #include <juce_gui_basics/juce_gui_basics.h>
#else
 #include <cstdint>
 namespace juce { struct Colour { unsigned int v=0; }; }
#endif

namespace ppgu {

// Canonical 21-token palette (CODE.txt contract). No token() accessor —
// canonical fields only (lesson of run #1163).
struct Palette {
    unsigned int window_bg=0xFF101418, panel=0xFF181D23, panel_elevated=0xFF20262E;
    unsigned int control_bg=0xFF262D36, control_face=0xFF313A45, control_border=0xFF3D4754;
    unsigned int text_primary=0xFFE8EEF4, text_secondary=0xFFAEBBC9, text_disabled=0xFF6E7B88;
    unsigned int accent=0xFF2FA4FF, accent_glow=0xFF1E63A8, danger=0xFFE04848;
    unsigned int warning=0xFFE0A028, success=0xFF3FBF6A, lcd_bg=0xFF0A0E12, lcd_text=0xFF9FE6FF;
    unsigned int led_on=0xFF46FFA0, led_off=0xFF24313D, shadow=0xFF000000, highlight=0xFFFFFFFF, lowlight=0xFF08090B;
};

struct ThemeManifest {
    std::string id;
    std::string name;
    Palette palette;
    std::vector<std::string> controls; // exact 28 parity
};

class ThemeManager {
public:
    static const char* const kTokens[21];
    static const char* const kControls[28];

    static bool validateAssetSpec(const ThemeManifest& m, std::string& err);
    static bool loadFromJson(const std::string& jsonText, ThemeManifest& out, std::string& err);
    static Palette fallbackPalette(); // skin-51 Blackout Voltage legacy fallback
};

} // namespace ppgu