#!/usr/bin/env python3
"""Static regression audit for the v33 M1 APVTS contract."""
from pathlib import Path
import re
import sys

source = Path("Source/PluginProcessor.cpp").read_text(encoding="utf-8")
ids = re.findall(r'(?:floatParameter|boolParameter)\(\s*"([^"]+)"', source)
ids += re.findall(r'ParameterID\s*\{\s*"([^"]+)"', source)

duplicates = sorted({item for item in ids if ids.count(item) > 1})
if duplicates:
    print("Duplicate APVTS parameter IDs:", ", ".join(duplicates))
    sys.exit(1)

required = {
    "master", "voices", "velocity", "palmMuteThreshold", "playMode",
    "articulation", "pickStyle", "pitchBendTarget", "monoMode", "legato",
    "hammerPull", "slideModifier", "modWheelVibrato", "patternSlot",
    "patternLength", "syncMode", "swingBase", "bpm", "swing", "humanize",
    "accent", "startOnFirstChord", "globalPpqOffset", "dnaChugDensity",
    "compressorOn", "eqOn", "distortionOn", "chorusOn", "delayOn",
    "reverbOn", "cabinetOn", "doubleTrackOn",
}
missing = sorted(required.difference(ids))
if missing:
    print("Missing required APVTS IDs:", ", ".join(missing))
    sys.exit(1)

if not re.search(r'for\s*\(int\s+slot\s*=\s*0;\s*slot\s*<\s*7;', source):
    print("Seven FX slot order parameters are not generated.")
    sys.exit(1)

print(f"PASS: {len(ids)} statically declared unique APVTS IDs; required M1 controls present; 7 FX slots generated.")
