#!/usr/bin/env python3
"""Validate the authored 300-preset catalog before packaging the VST3 build."""
import json
import math
import sys
from pathlib import Path

path = Path(sys.argv[1] if len(sys.argv) > 1 else "presets/factory_bank_300.json")
data = json.loads(path.read_text(encoding="utf-8"))
errors = []

artists = {item["name"] for item in data.get("artists", [])}
melody = data.get("melodyPresets", [])
pattern = data.get("patternPresets", [])
if len(melody) < 150:
    errors.append(f"Melody bank has {len(melody)} presets; expected at least 150")
if len(pattern) < 150:
    errors.append(f"Pattern bank has {len(pattern)} presets; expected at least 150")
if len(melody) + len(pattern) < 300:
    errors.append("Combined bank contains fewer than 300 presets")

all_presets = melody + pattern
ids = [p.get("id") for p in all_presets]
if len(ids) != len(set(ids)):
    errors.append("Preset IDs are not unique")
if any(not p.get("name") for p in all_presets):
    errors.append("One or more presets have an empty name")

for expected_mode, presets in (("MELODY", melody), ("PATTERN", pattern)):
    for preset in presets:
        if preset.get("mode") != expected_mode:
            errors.append(f'{preset.get("id")}: expected mode {expected_mode}')
        artist = preset.get("referenceArtist")
        if artist not in artists:
            errors.append(f'{preset.get("id")}: unknown reference artist {artist!r}')
        params = preset.get("parameters", {})
        for key in ("drive", "tone", "reverb"):
            value = params.get(key)
            if not isinstance(value, (int, float)):
                errors.append(f'{preset.get("id")}: missing numeric {key}')
        if isinstance(params.get("drive"), (int, float)) and not 0 <= params["drive"] <= 1:
            errors.append(f'{preset.get("id")}: drive outside 0..1')
        if isinstance(params.get("reverb"), (int, float)) and not 0 <= params["reverb"] <= 1:
            errors.append(f'{preset.get("id")}: reverb outside 0..1')
        for key in ("drive", "tone", "reverb"):
            value = params.get(key)
            if isinstance(value, bool) or not isinstance(value, (int, float)):
                errors.append(f'{preset.get("id")}: {key} must be numeric')
            elif not math.isfinite(value):
                errors.append(f'{preset.get("id")}: {key} must be finite')
        for key in ("drive", "reverb"):
            value = params.get(key)
            if isinstance(value, (int, float)) and not isinstance(value, bool) and math.isfinite(value) and not 0 <= value <= 1:
                errors.append(f'{preset.get("id")}: {key} outside 0..1')
        tone = params.get("tone")
        if isinstance(tone, (int, float)) and not isinstance(tone, bool) and math.isfinite(tone) and not 0 <= tone <= 10:
            errors.append(f'{preset.get("id")}: tone outside 0..10')
        sample_profile = preset.get("sampleProfile")
        if expected_mode == "MELODY" and (not isinstance(sample_profile, str) or not sample_profile.strip()):
            errors.append(f'{preset.get("id")}: missing sampleProfile')
        if expected_mode == "MELODY" and not preset.get("noteBehavior"):
            errors.append(f'{preset.get("id")}: missing noteBehavior')
        if expected_mode == "PATTERN" and not preset.get("groove"):
            errors.append(f'{preset.get("id")}: missing groove')

if errors:
    print("Factory preset catalog validation FAILED:")
    for error in errors:
        print(f" - {error}")
    raise SystemExit(1)

print(f"Factory catalog OK: {len(melody)} Melody + {len(pattern)} Pattern = {len(all_presets)} presets")
print(f"Artist coverage: {len(artists)} reference artists")
