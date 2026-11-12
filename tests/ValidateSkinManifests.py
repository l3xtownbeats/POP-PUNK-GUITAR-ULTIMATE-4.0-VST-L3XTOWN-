#!/usr/bin/env python3
"""Structural and accessibility checks for the registry-driven PPGU skin data contract.

Uses only the Python standard library so it can run on Windows CI without
installing packages. This is a focused contract validator, not a complete
JSON-Schema Draft 2020-12 implementation.
"""
from __future__ import annotations

import json
import re
import sys
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SKIN_ROOT = ROOT / "SKIN"
REGISTRY_PATH = SKIN_ROOT / "theme_manifest.json"
DEFAULT_ID = "skin-51"

GROUP_KEYS = {
    "canvas": ("base", "inset"),
    "surfaces": ("primary", "secondary", "raised", "overlay"),
    "borders": ("subtle", "strong", "focus"),
    "typography": ("primary", "secondary", "disabled", "onAccent"),
    "rotary": ("body", "ring", "track", "value", "pointer", "disabled"),
    "sliders": ("track", "fill", "thumb", "disabled"),
    "buttons": ("primary", "secondary"),
    "tabs": ("activeBackground", "activeText", "inactiveBackground", "inactiveText", "indicator"),
    "comboBoxes": ("background", "foreground", "border", "hover", "menuBackground", "selection"),
    "keyboard": ("whiteKey", "blackKey", "whitePressed", "blackPressed", "keyBorder", "octaveLabel"),
    "meters": ("low", "medium", "high", "background"),
    "icons": ("primary", "secondary", "accent", "disabled"),
    "thumbnails": ("background", "border", "selected", "caption"),
    "windowChrome": ("background", "title", "border", "close"),
    "hero": ("background", "accent", "secondaryAccent", "highlight"),
    "accents": ("primary", "secondary", "tertiary"),
    "texture": ("material", "enabled", "intensity", "asset"),
    "lighting": ("profile", "glow", "glowIntensity", "edgeHighlight"),
}
BUTTON_KEYS = ("background", "foreground", "hover", "pressed", "disabled")
HEX = re.compile(r"^#[0-9A-Fa-f]{6}$")
COLOR_KEYS = {
    "base", "inset", "primary", "secondary", "raised", "overlay", "subtle",
    "strong", "focus", "disabled", "onAccent", "body", "ring", "track",
    "value", "pointer", "fill", "thumb", "background", "foreground", "hover",
    "pressed", "activeBackground", "activeText", "inactiveBackground",
    "inactiveText", "indicator", "border", "menuBackground", "selection",
    "whiteKey", "blackKey", "whitePressed", "blackPressed", "keyBorder",
    "octaveLabel", "low", "medium", "high", "accent", "selected", "caption",
    "title", "close", "secondaryAccent", "highlight", "tertiary", "glow",
    "edgeHighlight",
}


def fail(errors: list[str], where: str, message: str) -> None:
    errors.append(f"{where}: {message}")


def load_json(path: Path, errors: list[str]):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        fail(errors, str(path.relative_to(ROOT)), f"invalid/unreadable JSON: {exc}")
        return None


def luminance(hex_colour: str) -> float:
    rgb = [int(hex_colour[i:i + 2], 16) / 255.0 for i in (1, 3, 5)]
    linear = [v / 12.92 if v <= 0.04045 else ((v + 0.055) / 1.055) ** 2.4 for v in rgb]
    return 0.2126 * linear[0] + 0.7152 * linear[1] + 0.0722 * linear[2]


def contrast(a: str, b: str) -> float:
    x, y = sorted((luminance(a), luminance(b)), reverse=True)
    return (x + 0.05) / (y + 0.05)


def validate_colour_tree(value, where: str, errors: list[str]) -> None:
    if isinstance(value, dict):
        for key, child in value.items():
            if key in COLOR_KEYS and isinstance(child, str) and not HEX.fullmatch(child):
                fail(errors, where, f"{key} must be #RRGGBB, got {child!r}")
            validate_colour_tree(child, where, errors)
    elif isinstance(value, list):
        for child in value:
            validate_colour_tree(child, where, errors)


def validate_theme(index: int, registry_entry: dict, errors: list[str]) -> None:
    folder = f"Skin_{index:02d}"
    path = SKIN_ROOT / folder / "theme.json"
    data = load_json(path, errors)
    if not isinstance(data, dict):
        return
    where = f"SKIN/{folder}/theme.json"

    expected_id = f"skin-{index:02d}"
    for key, expected in (("schemaVersion", "1.0.0"), ("id", expected_id), ("index", index)):
        if data.get(key) != expected:
            fail(errors, where, f"{key} must equal {expected!r}")

    for key in ("name", "description", "cluster", "identity", "tokens", "assets", "assetSpec", "compatibility", "validation"):
        if key not in data:
            fail(errors, where, f"missing top-level field {key}")

    if data.get("cluster") not in ("A", "B", "C"):
        fail(errors, where, "cluster must be A, B or C")

    identity = data.get("identity")
    if not isinstance(identity, dict) or any(not isinstance(identity.get(k), str) or not identity[k].strip()
                                             for k in ("material", "lighting", "mood")):
        fail(errors, where, "identity requires non-empty material, lighting and mood strings")

    tokens = data.get("tokens")
    if not isinstance(tokens, dict):
        fail(errors, where, "tokens must be an object")
        return

    for group, keys in GROUP_KEYS.items():
        value = tokens.get(group)
        if not isinstance(value, dict):
            fail(errors, where, f"missing token group/object {group}")
            continue
        missing = [key for key in keys if key not in value]
        if missing:
            fail(errors, where, f"{group} missing keys: {', '.join(missing)}")

    buttons = tokens.get("buttons", {})
    if isinstance(buttons, dict):
        for button_name in ("primary", "secondary"):
            button = buttons.get(button_name)
            if not isinstance(button, dict):
                fail(errors, where, f"buttons.{button_name} must be an object")
            else:
                missing = [key for key in BUTTON_KEYS if key not in button]
                if missing:
                    fail(errors, where, f"buttons.{button_name} missing: {', '.join(missing)}")

    validate_colour_tree(tokens, where, errors)

    compatibility = data.get("compatibility", {})
    for key in ("apvtsChanges", "layoutChanges", "audioChanges"):
        if compatibility.get(key) is not False:
            fail(errors, where, f"compatibility.{key} must be false")

    texture = tokens.get("texture", {})
    if isinstance(texture, dict):
        intensity = texture.get("intensity")
        if not isinstance(intensity, (int, float)) or not 0.0 <= intensity <= 1.0:
            fail(errors, where, "texture.intensity must be within 0..1")
    lighting = tokens.get("lighting", {})
    if isinstance(lighting, dict):
        intensity = lighting.get("glowIntensity")
        if not isinstance(intensity, (int, float)) or not 0.0 <= intensity <= 1.0:
            fail(errors, where, "lighting.glowIntensity must be within 0..1")

    assets = data.get("assets", {})
    if not isinstance(assets, dict):
        fail(errors, where, "assets must be an object")
    else:
        for key, expected in (("preview", "assets/preview.png"), ("hero", "assets/hero.webp")):
            if assets.get(key) != expected:
                fail(errors, where, f"assets.{key} must be {expected!r}")
        texture_asset = assets.get("texture")
        if texture_asset is not None and texture_asset != "assets/texture.png":
            fail(errors, where, "assets.texture must be null or assets/texture.png")

    specs = data.get("assetSpec", {})
    for key, width, height, fmt in (("preview", 480, 300, "png"), ("hero", 1200, 420, "webp")):
        spec = specs.get(key) if isinstance(specs, dict) else None
        if not isinstance(spec, dict) or (spec.get("width"), spec.get("height"), spec.get("format")) != (width, height, fmt):
            fail(errors, where, f"assetSpec.{key} must specify {width}x{height} {fmt}")

    # Text accessibility: the two primary reading combinations and primary button label.
    typography = tokens.get("typography", {})
    canvas = tokens.get("canvas", {})
    surfaces = tokens.get("surfaces", {})
    if isinstance(typography, dict) and isinstance(canvas, dict) and HEX.fullmatch(str(typography.get("primary", ""))) and HEX.fullmatch(str(canvas.get("base", ""))):
        ratio = contrast(typography["primary"], canvas["base"])
        if ratio < 4.5:
            fail(errors, where, f"primary text/canvas contrast {ratio:.2f}:1 is below 4.5:1")
    if isinstance(typography, dict) and isinstance(surfaces, dict) and HEX.fullmatch(str(typography.get("secondary", ""))) and HEX.fullmatch(str(surfaces.get("primary", ""))):
        ratio = contrast(typography["secondary"], surfaces["primary"])
        if ratio < 4.5:
            fail(errors, where, f"secondary text/card contrast {ratio:.2f}:1 is below 4.5:1")
    primary_button = buttons.get("primary", {}) if isinstance(buttons, dict) else {}
    if isinstance(primary_button, dict) and HEX.fullmatch(str(primary_button.get("foreground", ""))) and HEX.fullmatch(str(primary_button.get("background", ""))):
        ratio = contrast(primary_button["foreground"], primary_button["background"])
        if ratio < 4.5:
            fail(errors, where, f"primary button text contrast {ratio:.2f}:1 is below 4.5:1")

    # Registry entry must point to the corresponding canonical manifest.
    if registry_entry.get("id") != expected_id or registry_entry.get("index") != index:
        fail(errors, where, "registry id/index does not match manifest")
    if registry_entry.get("path") != f"{folder}/theme.json":
        fail(errors, where, "registry path does not match canonical manifest path")


def main() -> int:
    errors: list[str] = []
    registry = load_json(REGISTRY_PATH, errors)
    if not isinstance(registry, dict):
        print("\n".join(errors))
        return 1

    if registry.get("defaultSkin") != DEFAULT_ID:
        fail(errors, "SKIN/theme_manifest.json", "defaultSkin must be skin-51")
    if registry.get("selector") != "SKIN":
        fail(errors, "SKIN/theme_manifest.json", "selector must be SKIN")

    entries = registry.get("themes")
    if not isinstance(entries, list) or not entries:
        fail(errors, "SKIN/theme_manifest.json", "themes must be a non-empty registry array")
        entries = entries if isinstance(entries, list) else []

    expected_count = len(entries)
    if registry.get("totalSkins") != expected_count:
        fail(errors, "SKIN/theme_manifest.json", "totalSkins must equal registry entry count")

    ids = [entry.get("id") for entry in entries if isinstance(entry, dict)]
    indices = [entry.get("index") for entry in entries if isinstance(entry, dict)]
    if len(set(ids)) != expected_count:
        fail(errors, "SKIN/theme_manifest.json", "theme ids must be unique")
    if sorted(indices) != list(range(1, expected_count + 1)):
        fail(errors, "SKIN/theme_manifest.json", "indices must be unique and cover the registry range")

    by_index = {entry.get("index"): entry for entry in entries if isinstance(entry, dict)}
    for index in range(1, expected_count + 1):
        validate_theme(index, by_index.get(index, {}), errors)

    # A REV 14 pack may be staged beside the legacy runtime registry, but it
    # must remain explicitly non-runtime-integrated and pass its own full G1-G4
    # validator. Staged folders are not silently added to the runtime registry.
    pending_folders: set[str] = set()
    pack_path = SKIN_ROOT / "theme_pack_manifest.json"
    if pack_path.is_file():
        pack = load_json(pack_path, errors)
        if isinstance(pack, dict) and pack.get("status") == "DESIGNED_NOT_RUNTIME_INTEGRATED":
            pack_entries = pack.get("themes")
            if not isinstance(pack_entries, list) or not pack_entries:
                fail(errors, "SKIN/theme_pack_manifest.json", "staged pack must contain a non-empty themes array")
            else:
                expected_start = expected_count + 1
                pack_indices = [entry.get("index") for entry in pack_entries if isinstance(entry, dict)]
                if (pack.get("startIndex") != expected_start
                        or pack.get("endIndex") != expected_count + len(pack_entries)
                        or pack.get("totalSkins") != len(pack_entries)
                        or sorted(pack_indices) != list(range(expected_start, expected_count + len(pack_entries) + 1))):
                    fail(errors, "SKIN/theme_pack_manifest.json", "staged pack range/count is not continuous with the legacy registry")
                else:
                    result = subprocess.run(
                        [sys.executable, str(ROOT / "tools" / "validate_skin_pack.py"),
                         "--root", str(SKIN_ROOT), "--manifest", str(pack_path)],
                        capture_output=True, text=True, check=False)
                    if result.returncode != 0:
                        detail = (result.stdout + "\\n" + result.stderr).strip()
                        fail(errors, "SKIN/theme_pack_manifest.json",
                             "staged REV 14 pack failed G1-G4 validation: " + detail)
                    else:
                        pending_folders = {f"Skin_{index:03d}" for index in pack_indices}
                        print("STAGED REV 14 PACK: " + "; ".join(line for line in result.stdout.splitlines() if line.startswith("PASS ")))

    registered_folders = {f"Skin_{index:02d}" for index in range(1, expected_count + 1)}
    actual_folders = {path.name for path in SKIN_ROOT.glob("Skin_*") if path.is_dir()}
    expected_folders = registered_folders | pending_folders
    if actual_folders != expected_folders:
        missing = sorted(expected_folders - actual_folders)
        unregistered = sorted(actual_folders - expected_folders)
        if missing:
            fail(errors, "SKIN", "registered or staged skin folders missing: " + ", ".join(missing))
        if unregistered:
            fail(errors, "SKIN", "unregistered skin folders present: " + ", ".join(unregistered))

    if errors:
        print(f"SKIN CONTRACT: FAIL ({len(errors)} issue(s))")
        for error in errors:
            print(" - " + error)
        return 1

    print("SKIN CONTRACT: PASS")
    print(f"{expected_count} runtime registry manifests plus {len(pending_folders)} independently validated staged REV 14 manifest(s); no staged theme is treated as runtime-registered.")
    print("Note: this is a focused project-contract validator, not a full JSON Schema Draft 2020-12 engine.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
