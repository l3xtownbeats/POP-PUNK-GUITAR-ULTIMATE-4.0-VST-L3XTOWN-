#!/usr/bin/env python3
"""Validate the full 432-entry authored factory catalog before packaging the VST3 build."""
import json
import math
import sys
from pathlib import Path

path = Path(sys.argv[1] if len(sys.argv) > 1 else "presets/factory_bank_300.json")
data = json.loads(path.read_text(encoding="utf-8"))
errors = []

artist_items = data.get("artists", [])
artists = {item["name"] for item in artist_items}
artist_tracks = {item["name"]: set(item.get("tracks", [])) for item in artist_items}
melody = data.get("melodyPresets", [])
pattern = data.get("patternPresets", [])
minimums = data.get("minimumPresetCounts", {})
minimum_melody = int(minimums.get("melody", 216))
minimum_pattern = int(minimums.get("pattern", 216))
minimum_total = int(minimums.get("total", minimum_melody + minimum_pattern))
if len(melody) < minimum_melody:
    errors.append(f"Melody bank has {len(melody)} presets; expected at least {minimum_melody}")
if len(pattern) < minimum_pattern:
    errors.append(f"Pattern bank has {len(pattern)} presets; expected at least {minimum_pattern}")
if len(melody) + len(pattern) < minimum_total:
    errors.append(f"Combined bank has {len(melody) + len(pattern)} presets; expected at least {minimum_total}")

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
        else:
            allowed_tracks = artist_tracks.get(artist, set())
            for track in preset.get("referenceTracks", []):
                if track not in allowed_tracks:
                    errors.append(f'{preset.get("id")}: track {track!r} is not listed for {artist!r}')
            breakdown = preset.get("trackBreakdown")
            if breakdown:
                track_title = breakdown.get("trackTitle")
                if track_title not in allowed_tracks:
                    errors.append(f'{preset.get("id")}: track breakdown title {track_title!r} is not listed for {artist!r}')
                if track_title not in preset.get("referenceTracks", []):
                    errors.append(f'{preset.get("id")}: track breakdown title is missing from referenceTracks')
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

for artist in artist_items:
    if len(artist.get("tracks", [])) < 2:
        errors.append(f'{artist.get("name")}: expected at least two track references')
    for mode_name, presets in (("MELODY", melody), ("PATTERN", pattern)):
        if not any(p.get("referenceArtist") == artist.get("name") and p.get("trackBreakdown") for p in presets):
            errors.append(f'{artist.get("name")}: missing track-study preset in {mode_name}')

if errors:
    print("Factory preset catalog validation FAILED:")
    for error in errors:
        print(f" - {error}")
    raise SystemExit(1)

print(f"Factory catalog OK: {len(melody)} Melody + {len(pattern)} Pattern = {len(all_presets)} presets (minimum {minimum_total})")
print(f"Artist coverage: {len(artists)} reference artists")
