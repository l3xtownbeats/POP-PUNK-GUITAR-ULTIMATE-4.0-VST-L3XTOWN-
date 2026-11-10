#!/usr/bin/env python3
"""Validate portable .ppgm/.ppgp preset examples before VST3 integration."""
import json
import sys
from pathlib import Path

ROOT = Path(sys.argv[1] if len(sys.argv) > 1 else "presets/examples")
errors = []
files = sorted([*ROOT.glob("*.ppgm"), *ROOT.glob("*.ppgp")])
if not files:
    errors.append(f"No .ppgm/.ppgp examples found in {ROOT}")

seen = set()
for path in files:
    try:
        item = json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        errors.append(f"{path}: invalid JSON: {exc}")
        continue
    expected = "MELODY" if path.suffix == ".ppgm" else "PATTERN"
    required = ("format", "formatVersion", "id", "name", "mode", "parameters", "sampleProfile")
    for key in required:
        if key not in item:
            errors.append(f"{path}: missing {key}")
    if item.get("format") != "l3xtown-pop-punk-preset":
        errors.append(f"{path}: unsupported format identifier")
    if item.get("mode") != expected:
        errors.append(f"{path}: extension {path.suffix} requires mode {expected}")
    if not item.get("id") or item.get("id") in seen:
        errors.append(f"{path}: missing or duplicate preset id {item.get('id')!r}")
    seen.add(item.get("id"))
    params = item.get("parameters", {})
    for key in ("drive", "tone", "reverb"):
        if not isinstance(params.get(key), (int, float)):
            errors.append(f"{path}: parameters.{key} must be numeric")
    if expected == "MELODY" and not isinstance(item.get("melody"), dict):
        errors.append(f"{path}: missing melody object")
    if expected == "PATTERN":
        pattern = item.get("pattern")
        if not isinstance(pattern, dict) or not isinstance(pattern.get("steps"), list) or not pattern["steps"]:
            errors.append(f"{path}: pattern.steps must be a non-empty array")

if errors:
    print("Preset file examples validation FAILED:")
    for error in errors:
        print(f" - {error}")
    raise SystemExit(1)
print(f"Preset file examples OK: {len(files)} files ({len([p for p in files if p.suffix == '.ppgm'])} Melody, {len([p for p in files if p.suffix == '.ppgp'])} Pattern)")
