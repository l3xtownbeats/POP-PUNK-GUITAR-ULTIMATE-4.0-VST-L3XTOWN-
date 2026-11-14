#!/usr/bin/env python3
"""Validate PPGU REV14 Skin Studio packs (G1-G4) using only Python stdlib."""
from __future__ import annotations

import argparse
import json
import re
import struct
import sys
import xml.etree.ElementTree as ET
import zlib
from pathlib import Path


FAMILIES = {
    "carbon_studio", "neon_basement", "chrome_parade",
    "wood_summer", "vintage_tape", "sunset_suburb", "special",
}
LAYOUTS = {"STUDIO_CONSOLE", "WORKSTATION_TOPBAR", "RACK_SIDE", "COMPACT_MINI"}


def fail(message: str) -> None:
    raise ValueError(message)


def load_json(path: Path):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        fail(f"{path}: invalid JSON: {exc}")


def channel(hex_value: str) -> float:
    value = int(hex_value, 16) / 255.0
    return value / 12.92 if value <= 0.04045 else ((value + 0.055) / 1.055) ** 2.4


def luminance(colour: str) -> float:
    if not isinstance(colour, str) or not re.fullmatch(r"#[0-9A-Fa-f]{6}", colour):
        fail(f"invalid colour {colour!r}")
    value = colour[1:]
    r, g, b = (channel(value[i:i + 2]) for i in (0, 2, 4))
    return 0.2126 * r + 0.7152 * g + 0.0722 * b


def contrast(a: str, b: str) -> float:
    high, low = sorted((luminance(a), luminance(b)), reverse=True)
    return (high + 0.05) / (low + 0.05)


def png_dimensions(path: Path) -> tuple[int, int]:
    raw = path.read_bytes()
    if len(raw) < 33 or raw[:8] != b"\x89PNG\r\n\x1a\n":
        fail(f"{path}: invalid PNG signature")

    offset = 8
    width = height = bit_depth = colour_type = None
    image_data = bytearray()
    saw_end = False
    while offset + 12 <= len(raw):
        length = struct.unpack(">I", raw[offset:offset + 4])[0]
        kind = raw[offset + 4:offset + 8]
        end = offset + 12 + length
        if end > len(raw):
            fail(f"{path}: truncated PNG chunk")
        payload = raw[offset + 8:offset + 8 + length]
        expected_crc = struct.unpack(">I", raw[offset + 8 + length:end])[0]
        if (zlib.crc32(kind + payload) & 0xFFFFFFFF) != expected_crc:
            fail(f"{path}: PNG CRC mismatch")

        if kind == b"IHDR":
            if width is not None or length != 13:
                fail(f"{path}: invalid PNG IHDR")
            width, height, bit_depth, colour_type, compression, filtering, interlace = struct.unpack(
                ">IIBBBBB", payload
            )
            if compression != 0 or filtering != 0 or interlace != 0:
                fail(f"{path}: unsupported PNG encoding")
        elif kind == b"IDAT":
            image_data.extend(payload)
        elif kind == b"IEND":
            if length != 0:
                fail(f"{path}: invalid PNG IEND")
            saw_end = True
            offset = end
            break
        offset = end

    if not saw_end or width is None or height is None or not image_data:
        fail(f"{path}: incomplete PNG structure")
    channels = {2: 3, 6: 4}.get(colour_type)
    if bit_depth != 8 or channels is None:
        fail(f"{path}: expected 8-bit RGB/RGBA PNG")
    decoded = zlib.decompress(bytes(image_data))
    if len(decoded) != height * (1 + width * channels):
        fail(f"{path}: decoded pixel payload has invalid length")
    return width, height


def safe_svg(path: Path) -> None:
    raw = path.read_text(encoding="utf-8")
    if re.search(
        r"""<\s*(script|foreignObject)\b|<!DOCTYPE|<!ENTITY|\bon[a-z]+\s*=|\b(?:href|src)\s*=""",
        raw, re.I
    ):
        fail(f"{path}: active content, event handlers, entities or external references are forbidden")
    try:
        root = ET.fromstring(raw)
    except ET.ParseError as exc:
        fail(f"{path}: invalid SVG: {exc}")
    if not root.tag.endswith("svg"):
        fail(f"{path}: root element is not SVG")


def validate_registry_contract(registry: dict) -> int:
    """Validate no-loss historical registry plus the active SKIN-1..16 ordering."""
    total = registry.get("totalSkins")
    themes = registry.get("themes")
    if not isinstance(total, int) or isinstance(total, bool) or total < 1:
        fail("theme_manifest.json totalSkins is not a positive integer")
    if not isinstance(themes, list) or len(themes) != total:
        fail("theme registry totalSkins does not match its themes array")

    if registry.get("defaultSkin") != "skin-001":
        fail("theme registry defaultSkin must be canonical skin-001 / NEON STAGE DEFAULT")
    target = registry.get("releaseTargetActiveThemeCount")
    if target != 16:
        fail("theme registry releaseTargetActiveThemeCount must be exactly 16")

    declared_active = registry.get("activeThemeCount")
    if (not isinstance(declared_active, int) or isinstance(declared_active, bool)
            or declared_active < 1 or declared_active > 16):
        fail("theme registry activeThemeCount must be an integer from 1 to 16")

    seen_ids: set[str] = set()
    seen_indices: set[int] = set()
    seen_ordinals: set[int] = set()
    default_is_first = False
    legacy_names = {
        "neon riot", "dark metal", "modern studio", "carbon relay",
        "blackout voltage", "signal forge", "sound forge",
    }
    active_count = 0

    for entry in themes:
        if not isinstance(entry, dict):
            fail("theme registry contains a malformed entry")
        theme_id = entry.get("id")
        index = entry.get("index")
        name = entry.get("name")
        path = entry.get("path")
        active = entry.get("active")
        if (not isinstance(theme_id, str) or not theme_id.startswith("skin-")
                or not isinstance(index, int) or isinstance(index, bool)
                or index < 1 or theme_id != f"skin-{index:02d}" and theme_id != f"skin-{index:03d}"
                or not isinstance(name, str) or not name.strip()
                or not isinstance(path, str) or not path.startswith("Skin_")
                or not path.endswith("/theme.json") or path.startswith("/")
                or ".." in Path(path).parts or not isinstance(active, bool)):
            fail("theme registry entry has invalid ID/index/name/path/active fields")
        if theme_id in seen_ids or index in seen_indices:
            fail(f"theme registry has duplicate ID or index: {theme_id}")
        seen_ids.add(theme_id)
        seen_indices.add(index)

        if active:
            ordinal = entry.get("activeOrdinal")
            if (not isinstance(ordinal, int) or isinstance(ordinal, bool)
                    or ordinal < 1 or ordinal > declared_active or ordinal in seen_ordinals):
                fail(f"active theme {theme_id} has invalid/duplicate activeOrdinal")
            seen_ordinals.add(ordinal)
            active_count += 1
            if ordinal == 1 and theme_id == "skin-001":
                default_is_first = True
            if name.strip().casefold() in legacy_names:
                fail(f"legacy theme {name!r} cannot be active")
        elif "activeOrdinal" in entry:
            fail(f"inactive historical theme {theme_id} must not have activeOrdinal")

    if active_count != declared_active:
        fail("activeThemeCount does not match the number of active registry entries")
    if seen_ordinals != set(range(1, declared_active + 1)):
        fail("activeOrdinal values must be sequential from 1 without gaps")
    if not default_is_first:
        fail("skin-001 must be the first active theme (SKIN-1 / NEON STAGE DEFAULT)")
    return active_count


def validate(root: Path, pack_manifest_path: Path) -> list[str]:
    schema = load_json(root / "control_schema.json")
    registry = load_json(root / "theme_manifest.json")
    pack = load_json(pack_manifest_path)
    contract = schema.get("x-ppgu-contract", {}) if isinstance(schema, dict) else {}
    required = contract.get("requiredControlIds", [])
    tokens = contract.get("paletteTokens", [])
    if not isinstance(required, list) or not required or len(required) != len(set(required)):
        fail("control_schema.json must define unique requiredControlIds")
    if not isinstance(tokens, list) or len(tokens) != 21 or len(tokens) != len(set(tokens)):
        fail("control_schema.json must define exactly 21 unique palette tokens")

    existing_total = registry.get("totalSkins")
    if not isinstance(existing_total, int) or isinstance(existing_total, bool):
        fail("theme_manifest.json totalSkins is not an integer")
    if not isinstance(registry.get("themes"), list) or len(registry["themes"]) != existing_total:
        fail("theme registry totalSkins does not match its themes array")
    validate_registry_contract(registry)

    entries = pack.get("themes")
    if not isinstance(entries, list) or not entries:
        fail("theme_pack_manifest.json must contain a non-empty themes array")
    start, end = pack.get("startIndex"), pack.get("endIndex")
    if (not isinstance(start, int) or isinstance(start, bool)
            or not isinstance(end, int) or isinstance(end, bool)
            or pack.get("totalSkins") != len(entries)
            or end != start + len(entries) - 1):
        fail("pack range/count is internally inconsistent")

    pack_status = pack.get("status")
    if pack_status == "GENERATED_NOT_MERGED":
        if start != existing_total + 1:
            fail("unmerged pack must begin immediately after the current registry")
    elif pack_status == "MERGED":
        merged_total = pack.get("mergedRegistryTotal")
        if (start < 1 or end > existing_total
                or not isinstance(merged_total, int) or isinstance(merged_total, bool)
                or merged_total < end or merged_total > existing_total):
            fail("merged pack range/mergedRegistryTotal is outside the current registry")
        registry_by_index = {
            item.get("index"): item for item in registry["themes"] if isinstance(item, dict)
        }
        for packed in entries:
            registered = registry_by_index.get(packed.get("index"))
            if not isinstance(registered, dict):
                fail(f"merged pack index {packed.get('index')} is absent from registry")
            for key in ("id", "index", "name", "family", "layout", "path", "preview", "hero"):
                if registered.get(key) != packed.get(key):
                    fail(f"merged registry entry mismatch at index {packed.get('index')}: {key}")
    else:
        fail("pack status must be GENERATED_NOT_MERGED or MERGED")

    indices = []
    ids = set()
    for entry in entries:
        if not isinstance(entry, dict):
            fail("pack contains a malformed theme entry")
        index = entry.get("index")
        if not isinstance(index, int) or isinstance(index, bool):
            fail("pack theme index must be an integer")
        indices.append(index)
        expected_id = f"skin-{index:03d}"
        folder_name = f"Skin_{index:03d}"
        folder = root / folder_name
        theme_path = folder / "theme.json"
        if not theme_path.is_file():
            fail(f"missing {theme_path}")
        theme = load_json(theme_path)

        if theme.get("schemaVersion") != "1.0.0" or theme.get("index") != index or theme.get("id") != expected_id:
            fail(f"{theme_path}: schema version/ID/index mismatch")
        if not isinstance(theme.get("name"), str) or not theme["name"].strip():
            fail(f"{theme_path}: name is empty")
        if not isinstance(theme.get("description"), str) or not theme["description"].strip():
            fail(f"{theme_path}: description is empty")
        if theme.get("family") not in FAMILIES or theme.get("layout") not in LAYOUTS:
            fail(f"{theme_path}: invalid family or layout")
        if expected_id in ids:
            fail(f"{theme_path}: duplicate theme ID")
        ids.add(expected_id)

        for key in ("id", "index", "name", "family", "layout"):
            if entry.get(key) != theme.get(key):
                fail(f"{theme_path}: pack manifest {key} mismatch")
        for key, expected in (
            ("path", f"{folder_name}/theme.json"),
            ("preview", f"{folder_name}/assets/preview.png"),
            ("hero", f"{folder_name}/assets/hero.png"),
        ):
            if entry.get(key) != expected:
                fail(f"{theme_path}: pack manifest {key} path mismatch")

        palette = theme.get("palette")
        if not isinstance(palette, dict) or set(palette) != set(tokens):
            fail(f"{theme_path}: palette must contain exactly the 21 contract tokens")
        for token in tokens:
            luminance(palette[token])
        for foreground, background, minimum in (
            ("text_primary", "window_bg", 4.0),
            ("text_secondary", "window_bg", 4.0),
            ("text_disabled", "window_bg", 4.0),
            ("lcd_text", "lcd_bg", 4.0),
            ("accent", "panel", 2.2),
        ):
            ratio = contrast(palette[foreground], palette[background])
            if ratio < minimum:
                fail(f"{theme_path}: {foreground}/{background} contrast {ratio:.2f} < {minimum:.1f}")

        controls = theme.get("controls")
        if not isinstance(controls, dict):
            fail(f"{theme_path}: controls must be an object")
        missing = sorted(set(required) - set(controls))
        if missing:
            fail(f"{theme_path}: missing control IDs: {', '.join(missing)}")
        for control_id in required:
            control = controls[control_id]
            grid = control.get("grid", {}) if isinstance(control, dict) else {}
            x, y, w, h = (grid.get(key) for key in ("x", "y", "w", "h")) if isinstance(grid, dict) else (None, None, None, None)
            if not all(isinstance(value, int) and not isinstance(value, bool) for value in (x, y, w, h)):
                fail(f"{theme_path}: invalid grid coordinates for {control_id}")
            if min(x, y) < 0 or min(w, h) < 1 or x + w > 12 or y + h > 16:
                fail(f"{theme_path}: grid outside 12x16 for {control_id}")
            if (not isinstance(control.get("presentation"), str)
                    or not isinstance(control.get("runtimeBinding"), str)):
                fail(f"{theme_path}: presentation/runtimeBinding missing for {control_id}")

        if theme.get("reduceInEco") is not True or theme.get("hotSwapFadeMs") != 150:
            fail(f"{theme_path}: ECO/hotSwap contract mismatch")

        typography = theme.get("typography")
        if not isinstance(typography, dict):
            fail(f"{theme_path}: typography must be an object")
        if (not all(isinstance(typography.get(key), str) and typography[key].strip()
                    for key in ("ui", "display", "weight"))
                or not isinstance(typography.get("scale"), (int, float))
                or isinstance(typography.get("scale"), bool)
                or not 0.5 <= typography["scale"] <= 2.0
                or not isinstance(typography.get("letterSpacing"), (int, float))):
            fail(f"{theme_path}: typography values are invalid")

        effects = theme.get("effects")
        if not isinstance(effects, dict):
            fail(f"{theme_path}: effects must be an object")
        for key in ("material", "texture", "bezel", "crest", "glow"):
            if not isinstance(effects.get(key), str) or not effects[key].strip():
                fail(f"{theme_path}: effects.{key} must be a non-empty string")
        for key in ("shadowDepth", "edgeHighlight"):
            value = effects.get(key)
            if not isinstance(value, (int, float)) or isinstance(value, bool) or not 0.0 <= value <= 1.0:
                fail(f"{theme_path}: effects.{key} must be within 0..1")
        if any(effects.get(key) != expected for key, expected in (
            ("texture", "svg/texture.svg"), ("bezel", "svg/bezel.svg"), ("crest", "svg/crest.svg")
        )):
            fail(f"{theme_path}: effects asset paths must match the SVG asset contract")
        compatibility = theme.get("compatibility")
        if not isinstance(compatibility, dict) or any(
            compatibility.get(key) is not False
            for key in ("apvtsChanges", "layoutChanges", "audioChanges")
        ):
            fail(f"{theme_path}: compatibility must keep APVTS/layout/audio unchanged")

        assets = theme.get("assets")
        expected_assets = {
            "preview": "assets/preview.png",
            "hero": "assets/hero.png",
            "texture": "svg/texture.svg",
            "bezel": "svg/bezel.svg",
            "crest": "svg/crest.svg",
        }
        if not isinstance(assets, dict):
            fail(f"{theme_path}: assets must be an object")
        for key, expected in expected_assets.items():
            if assets.get(key) != expected:
                fail(f"{theme_path}: assets.{key} must be {expected!r}")

        for key, dimensions in (("preview", (360, 240)), ("hero", (800, 450))):
            asset = folder / expected_assets[key]
            if not asset.is_file() or png_dimensions(asset) != dimensions:
                fail(f"{theme_path}: {key} must be a valid {dimensions[0]}x{dimensions[1]} PNG")
        for key in ("texture", "bezel", "crest"):
            asset = folder / expected_assets[key]
            if not asset.is_file():
                fail(f"{theme_path}: missing {asset}")
            safe_svg(asset)

    if sorted(indices) != list(range(start, end + 1)) or len(indices) != len(set(indices)):
        fail("pack theme indices are not unique and continuous")
    return [
        f"PASS {len(entries)} theme(s)",
        "PASS G1 contrast",
        "PASS G2 control parity",
        "PASS G3 PNG/SVG structure and safety",
        "PASS G4 identity and continuous numbering",
    ]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path("SKIN"))
    parser.add_argument("--manifest", type=Path, default=Path("SKIN/theme_pack_manifest.json"))
    args = parser.parse_args()
    try:
        for line in validate(args.root, args.manifest):
            print(line)
    except Exception as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
