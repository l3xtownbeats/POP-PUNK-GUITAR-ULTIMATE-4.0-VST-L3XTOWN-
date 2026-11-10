#!/usr/bin/env python3
"""Validate the authored factory preset catalog and its release contract."""
import json
import math
import sys
from pathlib import Path

path = Path(sys.argv[1] if len(sys.argv) > 1 else "presets/factory_bank_300.json")
try:
    data = json.loads(path.read_text(encoding="utf-8"))
except (OSError, json.JSONDecodeError) as exc:
    print(f"Factory preset catalog validation FAILED: {exc}")
    raise SystemExit(1)

errors = []
if not isinstance(data, dict):
    print("Factory preset catalog validation FAILED: root must be a JSON object")
    raise SystemExit(1)

artists_raw = data.get("artists", [])
artists = set()
if not isinstance(artists_raw, list):
    errors.append("artists must be an array")
else:
    artist_names = []
    for index, item in enumerate(artists_raw):
        name = item.get("name") if isinstance(item, dict) else None
        if not isinstance(name, str) or not name.strip():
            errors.append(f"artists[{index}] must contain a non-empty name")
        else:
            artist_names.append(name.strip())
    if len(artist_names) != len(set(artist_names)):
        errors.append("Artist names must be unique")
    artists = set(artist_names)

# Preserve the agreed core pop-punk palette in every factory-bank revision.
required_artists = {
    "Blink-182", "Sum 41", "Simple Plan", "New Found Glory",
    "Good Charlotte", "Mest", "Angels & Airwaves",
    "The All-American Rejects", "MGK", "Avril Lavigne",
}
missing_artists = sorted(required_artists - artists)
if missing_artists:
    errors.append("Missing required reference artists: " + ", ".join(missing_artists))

melody = data.get("melodyPresets", [])
pattern = data.get("patternPresets", [])
if not isinstance(melody, list):
    errors.append("melodyPresets must be an array")
    melody = []
if not isinstance(pattern, list):
    errors.append("patternPresets must be an array")
    pattern = []

minimums = data.get("minimumPresetCounts", {})
if not isinstance(minimums, dict):
    errors.append("minimumPresetCounts must be an object")
    minimums = {}
for key, actual, expected in (
    ("melody", len(melody), 150),
    ("pattern", len(pattern), 150),
    ("total", len(melody) + len(pattern), 300),
):
    declared = minimums.get(key)
    if declared != expected:
        errors.append(f"minimumPresetCounts.{key} must be {expected}")
    if actual < expected:
        errors.append(f"{key} bank has {actual} presets; expected at least {expected}")

all_presets = melody + pattern
ids = []
for index, preset in enumerate(all_presets):
    if not isinstance(preset, dict):
        errors.append(f"preset[{index}] must be an object")
        continue
    preset_id = preset.get("id")
    if not isinstance(preset_id, str) or not preset_id.strip():
        errors.append(f"preset[{index}] has an empty or non-string id")
    else:
        ids.append(preset_id)
if len(ids) != len(set(ids)):
    errors.append("Preset IDs are not unique")

for expected_mode, presets in (("MELODY", melody), ("PATTERN", pattern)):
    for index, preset in enumerate(presets):
        if not isinstance(preset, dict):
            continue
        preset_id = preset.get("id", f"{expected_mode}[{index}]")
        if preset.get("mode") != expected_mode:
            errors.append(f"{preset_id}: expected mode {expected_mode}")
        name = preset.get("name")
        if not isinstance(name, str) or not name.strip():
            errors.append(f"{preset_id}: name must be a non-empty string")
        if preset.get("referenceArtist") not in artists:
            errors.append(f"{preset_id}: unknown reference artist {preset.get('referenceArtist')!r}")

        params = preset.get("parameters")
        if not isinstance(params, dict):
            errors.append(f"{preset_id}: parameters must be an object")
            params = {}
        for key, upper in (("drive", 1.0), ("tone", 10.0), ("reverb", 1.0)):
            value = params.get(key)
            if isinstance(value, bool) or not isinstance(value, (int, float)):
                errors.append(f"{preset_id}: parameters.{key} must be numeric")
            elif not math.isfinite(value):
                errors.append(f"{preset_id}: parameters.{key} must be finite")
            elif not 0.0 <= value <= upper:
                errors.append(f"{preset_id}: parameters.{key} outside 0..{upper:g}")

        if expected_mode == "MELODY":
            sample_profile = preset.get("sampleProfile")
            if not isinstance(sample_profile, str) or not sample_profile.strip():
                errors.append(f"{preset_id}: missing sampleProfile")
            if not preset.get("noteBehavior"):
                errors.append(f"{preset_id}: missing noteBehavior")
        else:
            if not preset.get("groove"):
                errors.append(f"{preset_id}: missing groove")
            if not preset.get("performance"):
                errors.append(f"{preset_id}: missing performance metadata")
            if not preset.get("sampleRequirement"):
                errors.append(f"{preset_id}: missing sampleRequirement")

if errors:
    print("Factory preset catalog validation FAILED:")
    for error in errors:
        print(f" - {error}")
    raise SystemExit(1)

print(f"Factory catalog OK: {len(melody)} Melody + {len(pattern)} Pattern = {len(all_presets)} presets")
print(f"Artist coverage: {len(artists)} reference artists (required core palette present)")
