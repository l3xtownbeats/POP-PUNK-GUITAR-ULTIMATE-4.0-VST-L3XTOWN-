#!/usr/bin/env python3
"""G2: APVTS registry - 20 ids, playMode MELODY/PATTERN/PUNK GROUP, layout parity."""
import json, os, sys
REQUIRED_ROTARY = ["ampGain","eqLow","eqMid","eqHigh","velocity","dynamics","swing","humanize","accent"]
REQUIRED_TOGGLE = ["fxDistortion","fxChorus","fxDelay","fxReverb","fxCompressor","fxLimiter","fxCabSim","fxFlangerPower"]
PLAY_MODES = ["MELODY","PATTERN","PUNK GROUP"]
def main(root="."):
    reg = json.load(open(os.path.join(root,"assets","apvts_registry.json"), encoding="utf-8"))
    ids = set(reg["ids"])
    missing = [i for i in REQUIRED_ROTARY+REQUIRED_TOGGLE+["playMode","patternSpeed","masterVolume"] if i not in ids]
    if missing: print(f"FAIL: APVTS missing ids: {missing}"); return 1
    if reg.get("playModes") != PLAY_MODES: print("FAIL: playModes mismatch"); return 1
    src = open(os.path.join(root,"Source","PluginProcessor.cpp"), encoding="utf-8").read()
    for i in REQUIRED_ROTARY + REQUIRED_TOGGLE:
        if f'"{i}"' not in src: print(f"FAIL: APVTS id {i} not present in PluginProcessor.cpp"); return 1
    if reg.get("skinControlParity") != 28: print("FAIL: skin control parity != 28"); return 1
    print(f"PASS: APVTS registry consistent ({len(ids)} ids, playModes={PLAY_MODES}, parity=28)")
    return 0
if __name__ == "__main__": sys.exit(main())