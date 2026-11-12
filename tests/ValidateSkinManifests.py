#!/usr/bin/env python3
"""Structural and accessibility checks for the registry-driven PPGU skin data contract.

Uses only the Python standard library so it can run on Windows CI without
installing packages. This is a focused contract validator, not a complete
JSON-Schema Draft 2020-12 implementation.
"""
from __future__ import annotations

import json
import re
import struct
import sys
import xml.etree.ElementTree as ET
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


def png_dimensions(path: Path) -> tuple[int, int]:
    raw = path.read_bytes()
    if len(raw) < 24 or raw[:8] != b"\\x89PNG\\r\\n\\x1a\\n" or raw[12:16] != b"IHDR":
        raise ValueError(f"{path}: invalid PNG header")
    return struct.unpack(">II", raw[16:24])


def safe_svg(path: Path) -> None:
    raw = path.read_text(encoding="utf-8")
    if re.search(r"<\\s*script|\\bon[a-z]+\\s*=|(?:href|src)\\s*=\\s*['\\\"]https?://", raw, re.I):
        raise ValueError(f"{path}: active script/event/external URL is forbidden")
    root = ET.fromstring(raw)
    if not root.tag.endswith("svg"):
        raise ValueError(f"{path}: root element is not SVG")


def validate_colour_tree(value, where: str, errors: list[str]) -> None:
    if isinstance(value, dict):
        for key, child in value.items():
            if key in COLOR_KEYS and isinstance(child, str) and not HEX.fullmatch(child):
                fail(errors, where, f"{key} must be #RRGGBB, got {child!r}")
            validate_colour_tree(child, where, errors)
    elif isinstance(value, list):
        for child in value:
            validate_colour_tree(child, where, errors)


def validate_skin_studio_theme(index: int, registry_entry: dict, errors: list[str]) -> None:
    folder = f"Skin_{index:03d}"
    path = SKIN_ROOT / folder / "theme.json"
    data = load_json(path, errors)
    if not isinstance(data, dict):
        return
    where = f"SKIN/{folder}/theme.json"
    expected_id = f"skin-{index:03d}"

    for key, expected in (("schemaVersion", "1.0.0"), ("id", expected_id), ("index", index)):
        if data.get(key) != expected:
            fail(errors, where, f"{key} must equal {expected!r}")
    if not isinstance(data.get("name"), str) or not data["name"].strip():
        fail(errors, where, "name must be a non-empty string")
    if not isinstance(data.get("description"), str) or not data["description"].strip():
        fail(errors, where, "description must be a non-empty string")
    if data.get("family") not in ("carbon_studio", "neon_basement", "chrome_parade",
                                  "wood_summer", "vintage_tape", "sunset_suburb", "special"):
        fail(errors, where, "invalid family")
    if data.get("layout") not in ("STUDIO_CONSOLE", "WORKSTATION_TOPBAR",
                                  "RACK_SIDE", "COMPACT_MINI"):
        fail(errors, where, "invalid layout")

    schema = load_json(SKIN_ROOT / "control_schema.json", errors)
    contract = schema.get("x-ppgu-contract", {}) if isinstance(schema, dict) else {}
    required = contract.get("requiredControlIds", [])
    palette_tokens = contract.get("paletteTokens", [])
    if len(palette_tokens) != 21 or len(set(palette_tokens)) != 21:
        fail(errors, "SKIN/control_schema.json", "paletteTokens must contain 21 unique tokens")

    palette = data.get("palette")
    if not isinstance(palette, dict):
        fail(errors, where, "palette must be an object")
        palette = {}
    if set(palette) != set(palette_tokens):
        fail(errors, where, "palette must contain exactly the 21 control_schema tokens")
    for token in palette_tokens:
        value = palette.get(token)
        if not isinstance(value, str) or not HEX.fullmatch(value):
            fail(errors, where, f"palette.{token} must be #RRGGBB")

    if isinstance(palette, dict):
        for fg, bg, minimum in (
            ("text_primary", "window_bg", 4.0),
            ("text_secondary", "window_bg", 4.0),
            ("text_disabled", "window_bg", 4.0),
            ("lcd_text", "lcd_bg", 4.0),
            ("accent", "panel", 2.2),
        ):
            if HEX.fullmatch(str(palette.get(fg, ""))) and HEX.fullmatch(str(palette.get(bg, ""))):
                ratio = contrast(palette[fg], palette[bg])
                if ratio < minimum:
                    fail(errors, where, f"{fg}/{bg} contrast {ratio:.2f}:1 is below {minimum:.1f}:1")

    controls = data.get("controls")
    if not isinstance(controls, dict):
        fail(errors, where, "controls must be an object")
        controls = {}
    missing = sorted(set(required) - set(controls))
    if missing:
        fail(errors, where, "missing control IDs: " + ", ".join(missing))
    for control_id, control in controls.items():
        grid = control.get("grid", {}) if isinstance(control, dict) else {}
        x, y, w, h = (grid.get(key) for key in ("x", "y", "w", "h")) if isinstance(grid, dict) else (None, None, None, None)
        if not all(isinstance(value, int) and not isinstance(value, bool) for value in (x, y, w, h)):
            fail(errors, where, f"invalid grid coordinates for {control_id}")
        elif min(x, y) < 0 or min(w, h) < 1 or x + w > 12 or y + h > 16:
            fail(errors, where, f"grid bounds outside 12x16 for {control_id}")

    if data.get("reduceInEco") is not True or data.get("hotSwapFadeMs") != 150:
        fail(errors, where, "ECO/hot-swap contract mismatch")
    compatibility = data.get("compatibility")
    if not isinstance(compatibility, dict) or any(compatibility.get(key) is not False
                                                   for key in ("apvtsChanges", "layoutChanges", "audioChanges")):
        fail(errors, where, "compatibility must declare APVTS/layout/audio changes false")

    assets = data.get("assets")
    expected_assets = {
        "preview": "assets/preview.png",
        "hero": "assets/hero.png",
        "texture": "svg/texture.svg",
        "bezel": "svg/bezel.svg",
        "crest": "svg/crest.svg",
    }
    if not isinstance(assets, dict):
        fail(errors, where, "assets must be an object")
        assets = {}
    for key, expected in expected_assets.items():
        if assets.get(key) != expected:
            fail(errors, where, f"assets.{key} must be {expected!r}")

    preview = SKIN_ROOT / folder / expected_assets["preview"]
    hero = SKIN_ROOT / folder / expected_assets["hero"]
    if not preview.is_file() or png_dimensions(preview) != (360, 240):
        fail(errors, where, "preview must exist and be a 360x240 PNG")
    if not hero.is_file() or png_dimensions(hero) != (800, 450):
        fail(errors, where, "hero must exist and be an 800x450 PNG")
    for key in ("texture", "bezel", "crest"):
        svg_path = SKIN_ROOT / folder / expected_assets[key]
        if not svg_path.is_file():
            fail(errors, where, f"missing {svg_path.relative_to(ROOT)}")
        else:
            try:
                safe_svg(svg_path)
            except Exception as exc:
                fail(errors, where, str(exc))

    if registry_entry.get("id") != expected_id or registry_entry.get("index") != index:
        fail(errors, where, "registry id/index does not match manifest")
    if registry_entry.get("path") != f"{folder}/theme.json":
        fail(errors, where, "registry path does not match canonical manifest path")
    if registry_entry.get("preview") != f"{folder}/assets/preview.png":
        fail(errors, where, "registry preview path does not match manifest")
    if registry_entry.get("hero") != f"{folder}/assets/hero.png":
        fail(errors, where, "registry hero path does not match manifest")


def validate_theme(index: int, registry_entry: dict, errors: list[str]) -> None:
    if index >= 52:
        validate_skin_studio_theme(index, registry_entry, errors)
        return

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

    registered_folders = {
        Path(entry.get("path", "")).parent.name
        for entry in entries if isinstance(entry, dict) and isinstance(entry.get("path"), str)
    }
    if len(registered_folders) != expected_count:
        fail(errors, "SKIN/theme_manifest.json", "registry paths must resolve to unique skin folders")
    actual_folders = {path.name for path in SKIN_ROOT.glob("Skin_*") if path.is_dir()}
    if actual_folders != registered_folders:
        missing = sorted(registered_folders - actual_folders)
        unregistered = sorted(actual_folders - registered_folders)
        if missing:
            fail(errors, "SKIN", "registered skin folders missing: " + ", ".join(missing))
        if unregistered:
            fail(errors, "SKIN", "unregistered skin folders present: " + ", ".join(unregistered))

    if errors:
        print(f"SKIN CONTRACT: FAIL ({len(errors)} issue(s))")
        for error in errors:
            print(" - " + error)
        return 1

    print("SKIN CONTRACT: PASS")
    print(f"{expected_count} registry manifests, exact folder parity, legacy and Skin Studio contracts, control maps, assets and contrast pairs verified.")
    print("Note: this is a focused project-contract validator, not a full JSON Schema Draft 2020-12 engine.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
