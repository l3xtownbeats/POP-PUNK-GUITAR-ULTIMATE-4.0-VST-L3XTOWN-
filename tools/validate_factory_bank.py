#!/usr/bin/env python3
"""Validate the 500-preset authored factory catalog before VST3 packaging."""
import json
import math
import sys
from pathlib import Path

path = Path(sys.argv[1] if len(sys.argv) > 1 else "presets/factory_bank_500.json")
data = json.loads(path.read_text(encoding="utf-8"))
errors = []

artists = {item["name"]: item for item in data.get("artists", [])}
melody = data.get("melodyPresets", [])
pattern = data.get("patternPresets", [])
expected = data.get("minimumPresetCounts", {})
if len(melody) != 250:
    errors.append(f"Melody bank has {len(melody)} presets; expected exactly 250")
if len(pattern) != 250:
    errors.append(f"Pattern bank has {len(pattern)} presets; expected exactly 250")
if len(melody) + len(pattern) != 500:
    errors.append("Combined bank must contain exactly 500 presets")
if expected != {"melody": 250, "pattern": 250, "total": 500}:
    errors.append(f"minimumPresetCounts mismatch: {expected!r}")

all_presets = melody + pattern
ids = [p.get("id") for p in all_presets]
names = [p.get("name") for p in all_presets]
if len(ids) != len(set(ids)):
    errors.append("Preset IDs are not unique")
if len(names) != len(set(names)):
    errors.append("Preset names are not unique")
if any(not isinstance(name, str) or not name.strip() for name in names):
    errors.append("One or more presets have an empty name")

allowed_pattern_categories = {
    "DOWNSTROKES", "DOWN/UP", "PALM MUTE",
    "POWER CHORDS", "ALTERNATE PICKING", "CUSTOM PATTERNS",
}
for expected_mode, presets in (("MELODY", melody), ("PATTERN", pattern)):
    for preset in presets:
        pid = preset.get("id", "<missing-id>")
        if preset.get("mode") != expected_mode:
            errors.append(f"{pid}: expected mode {expected_mode}")
        artist_name = preset.get("referenceArtist")
        artist = artists.get(artist_name)
        if artist is None:
            errors.append(f"{pid}: unknown reference artist {artist_name!r}")
        else:
            known_tracks = set(artist.get("tracks", []))
            refs = preset.get("referenceTracks", [])
            if not isinstance(refs, list) or any(track not in known_tracks for track in refs):
                errors.append(f"{pid}: referenceTracks do not belong to {artist_name}")
        if not preset.get("genreFamily"):
            errors.append(f"{pid}: missing genreFamily")

        params = preset.get("parameters", {})
        for key, low, high in (("drive", 0.0, 1.0), ("tone", 0.0, 10.0), ("reverb", 0.0, 1.0)):
            value = params.get(key)
            if isinstance(value, bool) or not isinstance(value, (int, float)) or not math.isfinite(value):
                errors.append(f"{pid}: {key} must be a finite number")
            elif not low <= value <= high:
                errors.append(f"{pid}: {key} outside {low}..{high}")
        output_db = params.get("outputDb")
        if output_db is not None and (isinstance(output_db, bool) or not isinstance(output_db, (int, float)) or not math.isfinite(output_db)):
            errors.append(f"{pid}: outputDb must be finite")

        if expected_mode == "MELODY":
            if not isinstance(preset.get("sampleProfile"), str) or not preset["sampleProfile"].strip():
                errors.append(f"{pid}: missing sampleProfile")
            if not preset.get("noteBehavior"):
                errors.append(f"{pid}: missing noteBehavior")
            if not preset.get("styleSubcategory"):
                errors.append(f"{pid}: missing styleSubcategory")
            performance = preset.get("melodyPerformance")
            if not isinstance(performance, dict) or performance.get("articulation") not in {"Open", "Palm Mute", "Flageolet", "Tremolo"}:
                errors.append(f"{pid}: missing/invalid melodyPerformance.articulation")
        else:
            if not preset.get("groove"):
                errors.append(f"{pid}: missing groove")
            if preset.get("patternCategory") not in allowed_pattern_categories:
                errors.append(f"{pid}: missing/invalid patternCategory")
            if not preset.get("patternSubcategory"):
                errors.append(f"{pid}: missing patternSubcategory")
            if preset.get("patternSequenceEncoding") != "tuple-v1":
                errors.append(f"{pid}: unsupported patternSequenceEncoding")
            sequence = preset.get("patternSequence")
            if not isinstance(sequence, list) or not sequence:
                errors.append(f"{pid}: patternSequence must contain events")
            else:
                seen_steps = set()
                for event in sequence:
                    if not isinstance(event, list) or len(event) != 7:
                        errors.append(f"{pid}: pattern event must be a 7-value tuple")
                        continue
                    step, stroke, velocity, gate, articulation, accent, palm_mute = event
                    if not isinstance(step, int) or not 0 <= step <= 15 or step in seen_steps:
                        errors.append(f"{pid}: invalid/duplicate pattern step {step!r}")
                    seen_steps.add(step)
                    if stroke not in (0, 1) or not isinstance(velocity, int) or not 0 <= velocity <= 127:
                        errors.append(f"{pid}: invalid stroke/velocity at step {step}")
                    if isinstance(gate, bool) or not isinstance(gate, (int, float)) or not math.isfinite(gate) or not 0 <= gate <= 1:
                        errors.append(f"{pid}: invalid gate at step {step}")
                    if articulation not in range(6) or accent not in (0, 1) or palm_mute not in (0, 1):
                        errors.append(f"{pid}: invalid articulation/accent/palmMute at step {step}")
            performance = preset.get("performance", {})
            if not isinstance(performance, dict) or not isinstance(performance.get("patternLengthBeats"), (int, float)) or performance["patternLengthBeats"] <= 0:
                errors.append(f"{pid}: invalid patternLengthBeats")

if errors:
    print("Factory preset catalog validation FAILED:")
    for error in errors:
        print(f" - {error}")
    raise SystemExit(1)

print(f"Factory catalog OK: {len(melody)} Melody + {len(pattern)} Pattern = {len(all_presets)} presets")
print(f"Coverage: {len(artists)} artist profiles, {len(data.get('presetCoverage', {}).get('genreFamilies', []))} genre families")
print("Pattern data: compact tuple-v1 events on a 16-step grid")
