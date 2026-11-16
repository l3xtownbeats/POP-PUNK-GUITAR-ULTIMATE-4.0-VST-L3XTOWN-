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
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SKIN_ROOT = ROOT / "SKIN"
REGISTRY_PATH = SKIN_ROOT / "theme_manifest.json"
DEFAULT_ID = "skin-053"

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
    signature = b"\x89PNG\r\n\x1a\n"
    if len(raw) < 33 or raw[:8] != signature:
        raise ValueError(f"{path}: invalid PNG signature")

    offset = 8
    width = height = bit_depth = colour_type = None
    image_data = bytearray()
    saw_end = False
    while offset + 12 <= len(raw):
        length = struct.unpack(">I", raw[offset:offset + 4])[0]
        chunk_type = raw[offset + 4:offset + 8]
        chunk_end = offset + 12 + length
        if chunk_end > len(raw):
            raise ValueError(f"{path}: truncated PNG chunk")
        chunk_data = raw[offset + 8:offset + 8 + length]
        expected_crc = struct.unpack(">I", raw[offset + 8 + length:chunk_end])[0]
        actual_crc = zlib.crc32(chunk_type + chunk_data) & 0xFFFFFFFF
        if actual_crc != expected_crc:
            raise ValueError(f"{path}: PNG chunk CRC mismatch")
        if chunk_type == b"IHDR":
            if width is not None or length != 13:
                raise ValueError(f"{path}: invalid PNG IHDR")
            width, height, bit_depth, colour_type, compression, filtering, interlace = struct.unpack(">IIBBBBB", chunk_data)
            if compression != 0 or filtering != 0 or interlace != 0:
                raise ValueError(f"{path}: unsupported PNG encoding")
        elif chunk_type == b"IDAT":
            image_data.extend(chunk_data)
        elif chunk_type == b"IEND":
            if length != 0:
                raise ValueError(f"{path}: invalid PNG IEND")
            saw_end = True
            offset = chunk_end
            break
        offset = chunk_end

    if not saw_end or width is None or height is None or not image_data:
        raise ValueError(f"{path}: incomplete PNG structure")
    channels = {2: 3, 6: 4}.get(colour_type)
    if bit_depth != 8 or channels is None:
        raise ValueError(f"{path}: expected 8-bit RGB/RGBA PNG")
    decoded = zlib.decompress(bytes(image_data))
    if len(decoded) != height * (1 + width * channels):
        raise ValueError(f"{path}: decoded PNG pixel payload has an invalid size")
    return width, height


def safe_svg(path: Path) -> None:
    raw = path.read_text(encoding="utf-8")
    if re.search(r"""<\s*script|\bon[a-z]+\s*=|(?:href|src)\s*=\s*['"]https?://""", raw, re.I):
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
    for image_path, expected_size, label in (
        (preview, (360, 240), "preview"),
        (hero, (800, 450), "hero"),
    ):
        try:
            if not image_path.is_file() or png_dimensions(image_path) != expected_size:
                fail(errors, where, f"{label} must be a valid {expected_size[0]}x{expected_size[1]} PNG")
        except Exception as exc:
            fail(errors, where, f"{label} asset validation failed: {exc}")
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


def main() -> int:
    errors: list[str] = []
    registry = load_json(REGISTRY_PATH, errors)
    if not isinstance(registry, dict):
        print("\n".join(errors))
        return 1

    if registry.get("defaultSkin") != DEFAULT_ID:
        fail(errors, "SKIN/theme_manifest.json", f"defaultSkin must be {DEFAULT_ID}")
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
    if len(set(indices)) != expected_count or any(not isinstance(i, int) or isinstance(i, bool) or i < 1 for i in indices):
        fail(errors, "SKIN/theme_manifest.json", "stable indices must be unique positive integers")

    # Validate active entries against the approved 16-theme sequence.
    sequence = load_json(SKIN_ROOT / "approved_theme_sequence.json", errors)
    sequence_entries = sequence.get("themes", []) if isinstance(sequence, dict) else []
    approved = {
        item.get("stableId"): item for item in sequence_entries
        if isinstance(item, dict) and isinstance(item.get("stableId"), str)
    }
    if len(approved) != 16 or len(sequence_entries) != 16:
        fail(errors, "SKIN/approved_theme_sequence.json", "the approved sequence must contain exactly 16 unique themes")

    registered_folders = set()
    for ordinal, entry in enumerate(entries, start=1):
        if not isinstance(entry, dict):
            fail(errors, "SKIN/theme_manifest.json", "registry entries must be objects")
            continue
        stable_id = entry.get("id")
        approved_entry = approved.get(stable_id)
        if approved_entry is None:
            fail(errors, "SKIN/theme_manifest.json", f"unapproved theme identity: {stable_id!r}")
            continue
        if (entry.get("index") != approved_entry.get("index")
            or entry.get("path") != approved_entry.get("path")
            or approved_entry.get("ordinal") != ordinal):
            fail(errors, "SKIN/theme_manifest.json", f"registry order/identity mismatch at ordinal {ordinal}")
        stable_index = entry.get("index")
        if isinstance(stable_index, int) and not isinstance(stable_index, bool):
            validate_skin_studio_theme(stable_index, entry, errors)
        registered_folders.add(Path(entry.get("path", "")).parent.name)

    expected_folders = {
        Path(item.get("path", "")).parent.name
        for item in sequence_entries if isinstance(item, dict) and isinstance(item.get("path"), str)
    }
    actual_folders = {path.name for path in SKIN_ROOT.glob("Skin_*") if path.is_dir()}
    if actual_folders != expected_folders:
        missing = sorted(expected_folders - actual_folders)
        unexpected = sorted(actual_folders - expected_folders)
        if missing:
            fail(errors, "SKIN", "approved theme folders missing: " + ", ".join(missing))
        if unexpected:
            fail(errors, "SKIN", "folders outside the approved 16-theme set: " + ", ".join(unexpected))
    if not registered_folders.issubset(expected_folders):
        fail(errors, "SKIN/theme_manifest.json", "runtime registry contains a folder outside the approved sequence")

    if errors:
        print(f"SKIN CONTRACT: FAIL ({len(errors)} issue(s))")
        for error in errors:
            print(" - " + error)
        return 1

    print("SKIN CONTRACT: PASS")
    print(f"{expected_count} active registry manifests, approved 16-theme folder set, stable identity/ordinal mapping, control maps, assets and contrast pairs verified.")
    print("Note: this is a focused project-contract validator, not a full JSON Schema Draft 2020-12 engine.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
