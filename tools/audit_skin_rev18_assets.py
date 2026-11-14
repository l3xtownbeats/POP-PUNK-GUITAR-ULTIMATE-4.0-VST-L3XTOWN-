#!/usr/bin/env python3
"""Read-only REV.18 asset completeness audit for a single PPGU skin.

This is deliberately separate from the legacy G1-G4 pack validator: it checks
REV.18 master/runtime deliverables and never modifies or generates assets.
Canonical path conventions are documented in REV18_ASSET_AUDIT_RU.md.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import struct
import sys
import zlib
import xml.etree.ElementTree as ET
from pathlib import Path

STATES = ("default", "hover", "pressed", "disabled", "dragged")
TOPBAR_ICONS = (
    "skins", "import_guitar", "auto_discover", "curator", "settings", "about",
)
MASTER_SIZES = {
    "hero": (1600, 900),
    "preview": (720, 480),
    "background": (2560, 1440),
}
RUNTIME_SIZES = {
    "hero": (800, 450),
    "preview": (360, 240),
}
REQUIRED_PALETTE_TOKENS = (
    "window_bg", "panel", "panel_elevated", "control_bg", "control_face",
    "control_border", "text_primary", "text_secondary", "text_disabled",
    "accent", "accent_glow", "danger", "warning", "success", "lcd_bg",
    "lcd_text", "led_on", "led_off", "shadow", "highlight", "lowlight",
)


def png_info(path: Path) -> tuple[int, int]:
    raw = path.read_bytes()
    if len(raw) < 33 or raw[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError(f"{path}: invalid PNG signature")
    offset = 8
    width = height = bit_depth = colour_type = None
    idat = bytearray()
    saw_iend = False
    while offset + 12 <= len(raw):
        length = struct.unpack(">I", raw[offset:offset + 4])[0]
        kind = raw[offset + 4:offset + 8]
        end = offset + 12 + length
        if end > len(raw):
            raise ValueError(f"{path}: truncated chunk")
        payload = raw[offset + 8:offset + 8 + length]
        crc = struct.unpack(">I", raw[offset + 8 + length:end])[0]
        if zlib.crc32(kind + payload) & 0xFFFFFFFF != crc:
            raise ValueError(f"{path}: CRC mismatch in {kind.decode('ascii', 'replace')}")
        if kind == b"IHDR":
            if width is not None or length != 13:
                raise ValueError(f"{path}: invalid IHDR")
            width, height, bit_depth, colour_type, comp, filt, interlace = struct.unpack(
                ">IIBBBBB", payload
            )
            if (comp, filt, interlace) != (0, 0, 0):
                raise ValueError(f"{path}: unsupported PNG encoding")
        elif kind == b"IDAT":
            idat.extend(payload)
        elif kind == b"IEND":
            if length:
                raise ValueError(f"{path}: invalid IEND")
            saw_iend = True
            offset = end
            break
        offset = end
    if not saw_iend or width is None or height is None or not idat:
        raise ValueError(f"{path}: incomplete PNG")
    channels = {2: 3, 6: 4}.get(colour_type)
    if bit_depth != 8 or channels is None:
        raise ValueError(f"{path}: expected 8-bit RGB/RGBA")
    decoded = zlib.decompress(bytes(idat))
    if len(decoded) != height * (1 + width * channels):
        raise ValueError(f"{path}: invalid decoded pixel payload")
    return width, height


def validate_svg(path: Path) -> None:
    """Reject active content and external references in theme SVG assets."""
    raw = path.read_bytes()
    upper = raw.upper()
    if b"<!DOCTYPE" in upper or b"<!ENTITY" in upper:
        raise ValueError(f"{path}: DOCTYPE/ENTITY is forbidden in SVG assets")
    try:
        root = ET.fromstring(raw)
    except ET.ParseError as exc:
        raise ValueError(f"{path}: invalid SVG XML: {exc}") from exc
    if root.tag not in ("svg", "{http://www.w3.org/2000/svg}svg"):
        raise ValueError(f"{path}: root element must be SVG")
    for element in root.iter():
        tag = element.tag.rsplit("}", 1)[-1].lower()
        if tag in {"script", "foreignobject", "iframe", "object", "embed"}:
            raise ValueError(f"{path}: forbidden SVG element <{tag}>")
        for key, value in element.attrib.items():
            attr = key.rsplit("}", 1)[-1].lower()
            value_lower = value.strip().lower()
            if attr.startswith("on"):
                raise ValueError(f"{path}: event handler attribute {attr!r} is forbidden")
            if attr in {"href", "src"} and value_lower and not value_lower.startswith("#"):
                raise ValueError(f"{path}: external href/src reference is forbidden")
            if "url(" in value_lower and not re.search(r"url\(\s*#[a-z0-9_.:-]+\s*\)", value_lower):
                raise ValueError(f"{path}: external CSS url() reference is forbidden")


def validate_icon_manifest(theme_dir: Path, icon_names: tuple[str, ...]) -> None:
    """Validate registered top-bar icon files against their SHA-256 and CRC32."""
    manifest_path = theme_dir / "assets/asset_manifest.json"
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ValueError(f"{manifest_path}: missing or invalid asset manifest: {exc}") from exc
    records = manifest.get("records")
    if not isinstance(records, list):
        raise ValueError(f"{manifest_path}: records must be an array")
    indexed = {record.get("id"): record for record in records if isinstance(record, dict)}
    if manifest.get("skinId") != "skin-053" or manifest.get("assetGroup") != "topbar-icons":
        raise ValueError(f"{manifest_path}: wrong skinId or assetGroup")
    for icon in icon_names:
        for suffix, rel in (("master_svg", f"assets/master/icons/{icon}.svg"),
                            ("runtime_png", f"assets/icons/{icon}.png")):
            asset_id = f"topbar.{icon}.{suffix}"
            record = indexed.get(asset_id)
            if not isinstance(record, dict) or record.get("path") != rel:
                raise ValueError(f"{manifest_path}: missing or mismatched record {asset_id}")
            if "[DECISION]" not in str(record.get("origin", "")):
                raise ValueError(f"{manifest_path}: {asset_id} lacks [DECISION] provenance")
            asset_path = theme_dir / rel
            raw = asset_path.read_bytes()
            if record.get("bytes") != len(raw):
                raise ValueError(f"{manifest_path}: byte count mismatch for {asset_id}")
            if record.get("sha256") != hashlib.sha256(raw).hexdigest():
                raise ValueError(f"{manifest_path}: SHA-256 mismatch for {asset_id}")
            if record.get("crc32") != f"{zlib.crc32(raw) & 0xFFFFFFFF:08X}":
                raise ValueError(f"{manifest_path}: CRC32 mismatch for {asset_id}")
            if suffix == "master_svg":
                validate_svg(asset_path)
            elif png_info(asset_path) != (256, 256):
                raise ValueError(f"{manifest_path}: runtime icon {asset_id} must be 256x256")


def luminance(hex_colour: str) -> float:
    if not isinstance(hex_colour, str) or not re.fullmatch(r"#[0-9A-Fa-f]{6}", hex_colour):
        raise ValueError(f"invalid HEX token: {hex_colour!r}")
    rgb = [int(hex_colour[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    linear = [v / 12.92 if v <= 0.04045 else ((v + 0.055) / 1.055) ** 2.4 for v in rgb]
    return .2126 * linear[0] + .7152 * linear[1] + .0722 * linear[2]


def contrast(a: str, b: str) -> float:
    hi, lo = sorted((luminance(a), luminance(b)), reverse=True)
    return (hi + .05) / (lo + .05)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path("SKIN"))
    parser.add_argument("--skin-id", default="skin-053")
    parser.add_argument("--report", type=Path, help="Optional JSON report path")
    args = parser.parse_args()
    root = args.root.resolve()
    findings: list[dict[str, str]] = []

    def add(level: str, code: str, message: str) -> None:
        findings.append({"level": level, "code": code, "message": message})

    try:
        schema = json.loads((root / "control_schema.json").read_text(encoding="utf-8"))
        registry = json.loads((root / "theme_manifest.json").read_text(encoding="utf-8"))
        contract = schema["x-ppgu-contract"]
        entry = next(x for x in registry["themes"] if x.get("id") == args.skin_id)
        theme_dir = root / Path(entry["path"]).parent
        theme = json.loads((theme_dir / "theme.json").read_text(encoding="utf-8"))
    except (OSError, ValueError, KeyError, StopIteration, json.JSONDecodeError) as exc:
        print(f"FATAL: cannot load skin contract/registry/theme: {exc}", file=sys.stderr)
        return 2

    control_ids = contract.get("requiredControlIds", [])
    tokens = contract.get("paletteTokens", [])
    if len(control_ids) != 27 or len(set(control_ids)) != 27:
        add("FAIL", "CONTROL_SCHEMA", "Expected exactly 27 unique requiredControlIds.")
    if tuple(tokens) != REQUIRED_PALETTE_TOKENS:
        add("FAIL", "PALETTE_SCHEMA", "Palette token inventory differs from REV.18's 21 canonical tokens.")
    if theme.get("id") != args.skin_id or entry.get("id") != args.skin_id:
        add("FAIL", "IDENTITY", "Registry/theme ID mismatch.")
    if args.skin_id == "skin-053" and (
        theme.get("name") != "NEON STAGE DEFAULT" or registry.get("defaultSkin") != "skin-053"
    ):
        add("FAIL", "DEFAULT_IDENTITY", "SKIN-1 must remain NEON STAGE DEFAULT / skin-053.")

    palette = theme.get("palette", {})
    missing_tokens = [token for token in REQUIRED_PALETTE_TOKENS if token not in palette]
    if missing_tokens:
        add("FAIL", "PALETTE_MISSING", "Missing theme palette tokens: " + ", ".join(missing_tokens))
    else:
        for token, bg_token in (
            ("text_primary", "window_bg"), ("text_secondary", "window_bg"),
            ("text_disabled", "window_bg"), ("lcd_text", "lcd_bg"), ("accent", "panel"),
        ):
            try:
                ratio = contrast(palette[token], palette[bg_token])
                strict = token == "text_primary"
                level = ("PASS" if ratio >= 7.0 else "FAIL") if strict else "INFO"
                target = "required >= 7:1" if strict else "measured; no universal 7:1 gate"
                add(level, "CONTRAST_" + token.upper(),
                    f"{token}/{bg_token} contrast = {ratio:.2f}:1 ({target}).")
            except ValueError as exc:
                add("FAIL", "CONTRAST_INVALID", str(exc))

    # Master and runtime artwork use separate files; runtime exports never replace masters.
    image_paths = {
        "master_hero": (theme_dir / "assets/master/hero/hero.png", MASTER_SIZES["hero"]),
        "runtime_hero": (theme_dir / "assets/hero/hero.png", RUNTIME_SIZES["hero"]),
        "master_preview": (theme_dir / "assets/master/preview/preview.png", MASTER_SIZES["preview"]),
        "runtime_preview": (theme_dir / "assets/preview/preview.png", RUNTIME_SIZES["preview"]),
        "master_background": (theme_dir / "assets/master/background/main.png", MASTER_SIZES["background"]),
        "runtime_background": (theme_dir / "assets/background/main.png", MASTER_SIZES["background"]),
    }
    for label, (path, minimum) in image_paths.items():
        if not path.is_file():
            add("FAIL", "MISSING_" + label.upper(), f"Missing required image: {path.relative_to(root)}")
            continue
        try:
            width, height = png_info(path)
            if width < minimum[0] or height < minimum[1]:
                add("FAIL", "SIZE_" + label.upper(),
                    f"{path.relative_to(root)} is {width}x{height}; minimum is {minimum[0]}x{minimum[1]}.")
            else:
                add("PASS", "PNG_" + label.upper(), f"{path.relative_to(root)}: {width}x{height}, CRC valid.")
        except (OSError, ValueError, zlib.error) as exc:
            add("FAIL", "PNG_" + label.upper(), str(exc))

    for control_id in control_ids:
        for state in STATES:
            path = theme_dir / f"assets/master/controls/{control_id}_{state}.png"
            if not path.is_file():
                add("FAIL", "CONTROL_FRAME_MISSING", f"Missing {path.relative_to(root)}")
            else:
                try:
                    w, h = png_info(path)
                    if w < 512 or h < 512:
                        add("FAIL", "CONTROL_FRAME_SIZE", f"{path.relative_to(root)} is {w}x{h}; minimum 512x512.")
                except (OSError, ValueError, zlib.error) as exc:
                    add("FAIL", "CONTROL_FRAME_PNG", str(exc))

    for folder, label in (
        ("assets/master/textures", "MASTER_TEXTURES"),
        ("assets/textures", "RUNTIME_TEXTURES"),
        ("assets/master/fx_layers", "MASTER_FX"),
        ("assets/fx_layers", "RUNTIME_FX"),
        ("assets/master/icons", "MASTER_ICONS"),
        ("assets/icons", "RUNTIME_ICONS"),
        ("fonts", "LICENSED_FONTS"),
        ("atlas", "TEXTURE_ATLAS"),
    ):
        path = theme_dir / folder
        if not path.is_dir() or not any(p.is_file() for p in path.rglob("*")):
            add("FAIL", "MISSING_" + label, f"Missing or empty required directory: {path.relative_to(root)}")

    for folder in ("assets/master/textures", "assets/textures"):
        directory = theme_dir / folder
        if directory.is_dir():
            for path in directory.glob("*.png"):
                try:
                    width, height = png_info(path)
                    if width < 1024 or height < 1024:
                        add("FAIL", "TEXTURE_SIZE",
                            f"{path.relative_to(root)} is {width}x{height}; minimum is 1024x1024.")
                except (OSError, ValueError, zlib.error) as exc:
                    add("FAIL", "TEXTURE_PNG", str(exc))

    for folder in ("assets/master/fx_layers", "assets/fx_layers"):
        directory = theme_dir / folder
        if directory.is_dir():
            for path in directory.glob("*.png"):
                try:
                    png_info(path)
                except (OSError, ValueError, zlib.error) as exc:
                    add("FAIL", "FX_LAYER_PNG", str(exc))

    for icon in TOPBAR_ICONS:
        svg = theme_dir / f"assets/master/icons/{icon}.svg"
        png = theme_dir / f"assets/icons/{icon}.png"
        if not svg.is_file():
            add("FAIL", "TOPBAR_SVG_MISSING", f"Missing {svg.relative_to(root)}")
        else:
            try:
                validate_svg(svg)
                add("PASS", "TOPBAR_SVG_SAFE", f"{svg.relative_to(root)}: valid, non-active SVG.")
            except (OSError, ValueError) as exc:
                add("FAIL", "TOPBAR_SVG_INVALID", str(exc))
        if not png.is_file():
            add("FAIL", "TOPBAR_PNG_MISSING", f"Missing {png.relative_to(root)}")
        else:
            try:
                png_info(png)
            except (OSError, ValueError, zlib.error) as exc:
                add("FAIL", "TOPBAR_PNG_INVALID", str(exc))

    icon_manifest = theme_dir / "assets/asset_manifest.json"
    if not icon_manifest.is_file():
        add("FAIL", "ASSET_MANIFEST_MISSING", f"Missing {icon_manifest.relative_to(root)}")
    else:
        try:
            validate_icon_manifest(theme_dir, TOPBAR_ICONS)
            add("PASS", "ASSET_MANIFEST_VALID", "TOP BAR asset manifest SHA-256/CRC32 records are valid.")
        except (OSError, ValueError, zlib.error) as exc:
            add("FAIL", "ASSET_MANIFEST_INVALID", str(exc))

    atlas_json = theme_dir / "atlas/atlas.json"
    if not atlas_json.is_file():
        add("FAIL", "ATLAS_INDEX_MISSING", f"Missing {atlas_json.relative_to(root)}")
    else:
        try:
            atlas = json.loads(atlas_json.read_text(encoding="utf-8"))
            if not isinstance(atlas.get("frames"), dict) or not atlas["frames"]:
                add("FAIL", "ATLAS_INDEX_INVALID", "atlas/atlas.json must contain a non-empty frames object.")
        except (OSError, json.JSONDecodeError) as exc:
            add("FAIL", "ATLAS_INDEX_INVALID", str(exc))

    errors = sum(item["level"] == "FAIL" for item in findings)
    passes = sum(item["level"] == "PASS" for item in findings)
    report = {
        "audit": "PPGU UI THEME SPEC REV.18 v2.0",
        "skinId": args.skin_id,
        "themeName": theme.get("name"),
        "result": "FAIL" if errors else "PASS",
        "passCount": passes,
        "failCount": errors,
        "findings": findings,
    }
    output = json.dumps(report, indent=2, ensure_ascii=False) + "\n"
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(output, encoding="utf-8")
    print(output, end="")
    return 1 if errors else 0


if __name__ == "__main__":
    raise SystemExit(main())
