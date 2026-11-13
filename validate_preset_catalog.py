#!/usr/bin/env python3
"""G1: preset catalog - 300 unique definitions, 8 categories, APVTS parity."""
import json, sys
CATS = {"Clean","Crunch","Drive","Lead","Ambient","Lo-Fi","Acoustic Textures","Punk Rhythm"}
def main(path):
    data = json.load(open(path, encoding="utf-8"))
    presets = data.get("presets")
    if not isinstance(presets, list) or len(presets) != 300:
        print(f"FAIL: expected 300 presets, got {len(presets) if presets else 0}"); return 1
    ids = [p.get("id") for p in presets]
    if len(set(ids)) != 300: print("FAIL: duplicate preset ids"); return 1
    cats = {p.get("category") for p in presets}
    if not cats <= CATS: print(f"FAIL: unknown categories {cats - CATS}"); return 1
    if len(cats) != 8: print(f"FAIL: expected all 8 categories, got {len(cats)}"); return 1
    allowed = {"ampGain","eqLow","eqMid","eqHigh","velocity","dynamics","swing","humanize","accent",
               "fxDistortion","fxChorus","fxDelay","fxReverb","fxCompressor","fxLimiter","fxCabSim",
               "fxFlangerPower","playMode","patternSpeed","masterVolume"}
    for p in presets:
        for k in p.get("params", {}):
            if k not in allowed: print(f"FAIL: preset {p['id']} has non-APVTS key {k}"); return 1
    print("PASS: preset catalog valid (300 definitions, unique ids, 8 categories, APVTS parity)")
    return 0
if __name__ == "__main__": sys.exit(main(sys.argv[1]))