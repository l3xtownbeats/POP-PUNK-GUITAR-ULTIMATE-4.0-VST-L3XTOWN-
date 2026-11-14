"""Structural gate for the approved NEON STAGE references and canonical default skin.

This validates source-file integrity, canonical identity and metadata parity only.
It does NOT claim a pixel-perfect visual match; human review is still required.
"""
from __future__ import annotations

import json
from pathlib import Path
from PIL import Image

ROOT = Path(__file__).resolve().parent
SCHEMA = ROOT / "SKIN" / "control_schema.json"
REGISTRY = ROOT / "SKIN" / "theme_manifest.json"
ACTIVE_THEME = ROOT / "SKIN" / "Skin_052" / "theme.json"
LEGACY_THEME = ROOT / "SKIN" / "Skin_053" / "theme.json"
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
DEFAULT_ID = "neon-stage-default-skin-1"


def read_json(path: Path):
    if not path.is_file():
        raise FileNotFoundError(path)
    return json.loads(path.read_text(encoding="utf-8"))


def grid_map(theme):
    return {key: value.get("grid") for key, value in theme.get("controls", {}).items()}


def validate_grid(errors, controls, label):
    for control_id, box in controls.items():
        if not isinstance(box, dict) or not all(k in box for k in ("x", "y", "w", "h")):
            errors.append(f"{label}: invalid grid rectangle for {control_id}.")
            continue
        x, y, w, h = (box[k] for k in ("x", "y", "w", "h"))
        if not all(isinstance(v, int) and not isinstance(v, bool) for v in (x, y, w, h)):
            errors.append(f"{label}: non-integer grid rectangle for {control_id}.")
            continue
        if x < 0 or y < 0 or w < 1 or h < 1 or x + w > GRID[0] or y + h > GRID[1]:
            errors.append(f"{label}: out-of-bounds grid rectangle for {control_id}: {box}")

    entries = list(controls.items())
    for i, (name_a, a) in enumerate(entries):
        if not isinstance(a, dict) or not all(k in a for k in ("x", "y", "w", "h")):
            continue
        for name_b, b in entries[i + 1:]:
            if not isinstance(b, dict) or not all(k in b for k in ("x", "y", "w", "h")):
                continue
            if (a["x"] < b["x"] + b["w"] and b["x"] < a["x"] + a["w"]
                    and a["y"] < b["y"] + b["h"] and b["y"] < a["y"] + a["h"]):
                errors.append(f"{label}: grid overlap {name_a} / {name_b}")


def main():
    errors = []
    reference_info = []
    schema = read_json(SCHEMA)
    registry = read_json(REGISTRY)
    active = read_json(ACTIVE_THEME)
    legacy = read_json(LEGACY_THEME)

    if registry.get("defaultSkin") != DEFAULT_ID:
        errors.append(f"Registry defaultSkin must be {DEFAULT_ID}.")
    if registry.get("totalSkins") != len(registry.get("themes", [])):
        errors.append("Registry totalSkins does not match themes length.")

    active_entries = [item for item in registry.get("themes", [])
                      if item.get("id") == DEFAULT_ID]
    if len(active_entries) != 1:
        errors.append(f"Registry must contain exactly one {DEFAULT_ID} entry.")
    elif (active_entries[0].get("index") != 52
          or active_entries[0].get("path") != "Skin_052/theme.json"):
        errors.append("Canonical NEON STAGE registry entry must remain index 52 / Skin_052 for compatibility.")

    if (active.get("id") != DEFAULT_ID or active.get("index") != 52
            or active.get("name") != "NEON STAGE"
            or active.get("designStatus") != "USER_APPROVED_SINGLE_UNIFIED_IDENTITY_NEON_STAGE"):
        errors.append("Skin_052 must contain the canonical NEON STAGE DEFAULT SKIN-1 identity.")
    if (legacy.get("id") != "skin-053" or legacy.get("index") != 53
            or legacy.get("name") != "NEON STAGE — LEGACY ARCHIVE"
            or legacy.get("designStatus") != "LEGACY_ARCHIVE_NOT_ACTIVE"):
        errors.append("Skin_053 must remain the non-default NEON STAGE legacy archive.")

    required = schema.get("properties", {}).get("controls", {}).get("required", [])
    active_grid = grid_map(active)
    legacy_grid = grid_map(legacy)
    missing = sorted(set(required) - set(active_grid))
    if missing:
        errors.append("Canonical NEON STAGE is missing schema-required controls: " + ", ".join(missing))

    if set(active_grid) != set(legacy_grid):
        errors.append("Legacy Skin_053 control IDs differ from canonical Skin_052.")
    for control_id in sorted(set(active_grid) & set(legacy_grid)):
        if active_grid[control_id] != legacy_grid[control_id]:
            errors.append(f"Legacy grid differs from canonical Skin_052 for {control_id}.")

    validate_grid(errors, active_grid, "Canonical Skin_052")

    compatibility = active.get("compatibility", {})
    for key in ("apvtsChanges", "layoutChanges", "audioChanges"):
        if compatibility.get(key) is not False:
            errors.append(f"Visual-only contract violated: compatibility.{key} must be false.")

    for filename in REFERENCE_FILES:
        path = ROOT / filename
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
        "default_skin_id": DEFAULT_ID,
        "active_skin_directory": "Skin_052",
        "legacy_archive_directory": "Skin_053",
        "status": "PASS" if not errors else "FAIL",
        "checks": {
            "approved_reference_files_present_and_decodable": len(reference_info),
            "schema_required_controls": len(required),
            "canonical_controls": len(active_grid),
            "legacy_archive_controls": len(legacy_grid),
            "legacy_grid_matches_canonical": active_grid == legacy_grid,
            "grid_bounds": "12x16",
            "audio_midi_apvts_changes": False,
            "registry_default_matches_canonical_id": registry.get("defaultSkin") == DEFAULT_ID,
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
    OUT.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n",
                   encoding="utf-8")
    print(json.dumps(report, indent=2, ensure_ascii=False))
    if errors:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
