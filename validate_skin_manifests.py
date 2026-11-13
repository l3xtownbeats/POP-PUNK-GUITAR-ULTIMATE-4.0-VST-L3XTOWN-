#!/usr/bin/env python3
"""G3: skin pack validator G1-G4. totalSkins derived from registry (no magic constant)."""
import json, os, sys
TOKENS = ["window_bg","panel","panel_elevated","control_bg","control_face","control_border",
 "text_primary","text_secondary","text_disabled","accent","accent_glow","danger","warning","success",
 "lcd_bg","lcd_text","led_on","led_off","shadow","highlight","lowlight"]
CONTROLS = ["preset_prev","preset_next","preset_favourite","preset_name","preset_category","preset_search",
 "btn_skins","btn_import","btn_autodiscover","btn_curator","btn_settings","btn_about",
 "knob_ampGain","knob_eqLow","knob_eqMid","knob_eqHigh","knob_velocity","knob_dynamics","knob_swing","knob_humanize","knob_accent",
 "tgl_fxDistortion","tgl_fxChorus","tgl_fxDelay","tgl_fxReverb","tgl_fxCompressor","tgl_fxCabSim","sync_selector"]
def lum(c):
    r, g, b = int(c[1:3],16)/255, int(c[3:5],16)/255, int(c[5:7],16)/255
    f = lambda x: x/12.92 if x <= 0.03928 else ((x+0.055)/1.055)**2.4
    return 0.2126*f(r)+0.7152*f(g)+0.0722*f(b)
def contrast(a, b):
    la, lb = lum(a), lum(b)
    return (max(la,lb)+0.05)/(min(la,lb)+0.05)
def main(skin_root):
    reg = json.load(open(os.path.join(skin_root, "registry.json"), encoding="utf-8"))
    total = reg["totalSkins"]
    entries = reg["skins"]
    if len(entries) != total:
        print(f"FAIL: registry totalSkins={total} but {len(entries)} entries"); return 1
    if sorted(e["index"] for e in entries) != list(range(1, total+1)):
        print("FAIL: skin index range not continuous"); return 1
    if len(set(e["id"] for e in entries)) != total: print("FAIL: duplicate skin ids"); return 1
    validated = 0
    for e in entries:
        mf = os.path.join(skin_root, e["folder"], "theme_manifest.json")
        if not os.path.isfile(mf): print(f"FAIL: missing manifest {mf}"); return 1
        m = json.load(open(mf, encoding="utf-8"))
        pal = m.get("palette", {})
        for t in TOKENS:
            c = pal.get(t, "")
            if not (isinstance(c, str) and len(c) == 7 and c[0] == "#"):
                print(f"FAIL: {e['id']}: token {t} invalid"); return 1
        if contrast(pal["text_primary"], pal["window_bg"]) < 4.5:
            print(f"FAIL: {e['id']}: text/window contrast < 4.5:1"); return 1
        if contrast(pal["lcd_text"], pal["lcd_bg"]) < 4.5:
            print(f"FAIL: {e['id']}: lcd contrast < 4.5:1"); return 1
        if set(m.get("controls", [])) != set(CONTROLS):
            print(f"FAIL: {e['id']}: control parity != 28 exact"); return 1
        png = os.path.join(skin_root, e["folder"], "assets", "bg.png")
        if not os.path.isfile(png): print(f"FAIL: {e['id']}: missing bg.png"); return 1
        if open(png, "rb").read(8) != b"\x89PNG\r\n\x1a\n":
            print(f"FAIL: {e['id']}: bg.png not valid PNG"); return 1
        validated += 1
    if reg.get("defaultSkin") != "skin-052":
        print("FAIL: defaultSkin must be skin-052"); return 1
    print(f"PASS G1-G4: {validated} themes validated")
    print(f"PASS: skin registry consistent ({total} themes, continuous range, default skin-052)")
    return 0
if __name__ == "__main__": sys.exit(main(sys.argv[1]))