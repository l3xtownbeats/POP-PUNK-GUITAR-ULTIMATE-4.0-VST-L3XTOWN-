"""Structural gate for the approved NEON STAGE references and Skin_052 geometry.

This validates source-file integrity and metadata parity only. It does NOT
claim a pixel-perfect visual match; that still requires human review.
"""
from __future__ import annotations

import json
from pathlib import Path
from PIL import Image

ROOT = Path(__file__).resolve().parent
SCHEMA = ROOT / "SKIN" / "control_schema.json"
REFERENCE_DIR = ROOT
SKIN_052 = ROOT / "SKIN" / "Skin_052" / "theme.json"
SKIN_053 = ROOT / "SKIN" / "Skin_053" / "theme.json"
OUT = ROOT / "dist" / "NEON_STAGE_REFERENCE_AUDIT.json"
REFERENCE_FILES = (
    "01 INTERFACE  NEON STAGE DEFAULT .png",
    "02 INTERFACE  NEON STAGE DEFAULT AUTOCHORDS .png",
    "03 INTERFACE  NEON STAGE DEFAULT .png",
    "04 INTERFACE  NEON STAGE DEFAULT .png",
    "05 INTERFACE  NEON STAGE DEFAULT .png",
    "06 INTERFACE  NEON STAGE DEFAULT .png",
    "07 INTERFACE  NEON STAGE DEFAULT .png",
)
GRID = (12, 16)


def read_json(path: Path):
    if not path.is_file():
        raise FileNotFoundError(path)
    return json.loads(path.read_text(encoding="utf-8"))


def grid_map(theme):
    return {key: value.get("grid") for key, value in theme.get("controls", {}).items()}


def main():
    errors = []
    reference_info = []
    schema = read_json(SCHEMA)
    old = read_json(SKIN_052)
    new = read_json(SKIN_053)

    required = schema.get("properties", {}).get("controls", {}).get("required", [])
    old_grid = grid_map(old)
    new_grid = grid_map(new)

    if new.get("id") != "skin-053" or new.get("index") != 53:
        errors.append("Skin_053 identity must remain skin-053 / index 53.")
    if new.get("name") != "NEON STAGE":
        errors.append("Skin_053 source name must be NEON STAGE.")
    if new.get("family") != "neon_basement":
        errors.append("Skin_053 source family must be neon_basement.")

    missing = sorted(set(required) - set(new_grid))
    if missing:
        errors.append("Missing schema-required controls: " + ", ".join(missing))

    if set(old_grid) != set(new_grid):
        errors.append("Skin_053 control IDs differ from the Skin_052 reference.")
    for control_id in sorted(set(old_grid) & set(new_grid)):
        if old_grid[control_id] != new_grid[control_id]:
            errors.append(f"Grid differs from Skin_052 for {control_id}: "
                          f"{new_grid[control_id]} != {old_grid[control_id]}")

    for control_id, box in new_grid.items():
        if not isinstance(box, dict) or not all(k in box for k in ("x", "y", "w", "h")):
            errors.append(f"Invalid grid rectangle for {control_id}.")
            continue
        x, y, w, h = (box[k] for k in ("x", "y", "w", "h"))
        if not all(isinstance(v, int) for v in (x, y, w, h)):
            errors.append(f"Non-integer grid rectangle for {control_id}.")
            continue
        if x < 0 or y < 0 or w < 1 or h < 1 or x + w > GRID[0] or y + h > GRID[1]:
            errors.append(f"Out-of-bounds grid rectangle for {control_id}: {box}")

    entries = list(new_grid.items())
    for i, (name_a, a) in enumerate(entries):
        if not isinstance(a, dict) or not all(k in a for k in ("x", "y", "w", "h")):
            continue
        for name_b, b in entries[i + 1:]:
            if not isinstance(b, dict) or not all(k in b for k in ("x", "y", "w", "h")):
                continue
            if (a["x"] < b["x"] + b["w"] and b["x"] < a["x"] + a["w"]
                    and a["y"] < b["y"] + b["h"] and b["y"] < a["y"] + a["h"]):
                errors.append(f"Grid overlap: {name_a} / {name_b}")

    compatibility = new.get("compatibility", {})
    for key in ("apvtsChanges", "layoutChanges", "audioChanges"):
        if compatibility.get(key) is not False:
            errors.append(f"Visual-only contract violated: compatibility.{key} must be false.")

    for filename in REFERENCE_FILES:
        path = REFERENCE_DIR / filename
        if not path.is_file():
            errors.append(f"Missing approved reference: {filename}")
            continue
        try:
            with Image.open(path) as im:
                im.verify()
            with Image.open(path) as im:
                if im.format != "PNG" or im.width < 1 or im.height < 1:
                    errors.append(f"Invalid PNG reference: {filename}")
                reference_info.append({"file": filename, "width": im.width,
                                       "height": im.height, "format": im.format})
        except Exception as exc:
            errors.append(f"Reference decode failed for {filename}: {exc}")

    report = {
        "project": "POP PUNK GUITAR ULTIMATE 4.0",
        "theme": "NEON STAGE",
        "skin": "Skin_053",
        "status": "PASS" if not errors else "FAIL",
        "checks": {
            "approved_reference_files_present_and_decodable": len(reference_info),
            "schema_required_controls": len(required),
            "skin_052_controls": len(old_grid),
            "skin_053_controls": len(new_grid),
            "grid_matches_skin_052": not any("Grid differs" in e for e in errors),
            "grid_bounds": "12x16",
            "audio_midi_apvts_changes": False,
        },
        "references": reference_info,
        "errors": errors,
        "limitations": [
            "This is a structural and image-integrity audit, not a pixel-level visual comparison.",
            "Human review of all seven mockups is still required before approving the final appearance.",
            "This script does not build the JUCE VST3 or test it in FL Studio."
        ],
    }
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\\n",
                   encoding="utf-8")
    print(json.dumps(report, indent=2, ensure_ascii=False))
    if errors:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
