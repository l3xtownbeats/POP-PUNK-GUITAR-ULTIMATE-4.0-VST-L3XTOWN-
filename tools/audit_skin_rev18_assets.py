#!/usr/bin/env python3
"""Read-only REV18 audit for approved PPGU themes; never modifies registries."""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import struct
import sys
import xml.etree.ElementTree as ET
import zlib
from pathlib import Path

TOKENS = {
    "window_bg", "panel", "panel_elevated", "control_bg", "control_face",
    "control_border", "text_primary", "text_secondary", "text_disabled",
    "accent", "accent_glow", "danger", "warning", "success", "lcd_bg",
    "lcd_text", "led_on", "led_off", "shadow", "highlight", "lowlight",
}
HEX = re.compile(r"^#[0-9A-Fa-f]{6}$")


def png_info(path: Path) -> tuple[int, int]:
    raw = path.read_bytes()
    if len(raw) < 45 or raw[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError("invalid PNG signature or truncated file")
    pos, width, height, saw_ihdr, saw_iend, saw_idat = 8, 0, 0, False, False, False
    while pos + 12 <= len(raw):
        length = struct.unpack(">I", raw[pos:pos + 4])[0]
        kind = raw[pos + 4:pos + 8]
        end = pos + 12 + length
        if end > len(raw):
            raise ValueError("truncated PNG chunk")
        data = raw[pos + 8:pos + 8 + length]
        expected = struct.unpack(">I", raw[pos + 8 + length:end])[0]
        actual = zlib.crc32(kind + data) & 0xffffffff
        if actual != expected:
            raise ValueError(f"CRC mismatch in {kind.decode('ascii', 'replace')}")
        if not saw_ihdr:
            if kind != b"IHDR" or length != 13:
                raise ValueError("IHDR missing or malformed")
            width, height = struct.unpack(">II", data[:8])
            saw_ihdr = True
        if kind == b"IDAT":
            saw_idat = True
        if kind == b"IEND":
            if length != 0:
                raise ValueError("malformed IEND")
            saw_iend = True
            pos = end
            break
        pos = end
    if not (saw_ihdr and saw_idat and saw_iend) or pos != len(raw):
        raise ValueError("incomplete PNG structure or trailing bytes")
    return width, height


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", type=Path, default=Path("SKIN"))
    ap.add_argument("--strict-assets", action="store_true",
                    help="fail when referenced raster assets are absent")
    args = ap.parse_args()
    root = args.root
    errors: list[str] = []
    warnings: list[str] = []
    seq_path = root / "approved_theme_sequence.json"
    registry_path = root / "theme_manifest.json"
    try:
        sequence = json.loads(seq_path.read_text(encoding="utf-8"))
        registry = json.loads(registry_path.read_text(encoding="utf-8"))
    except Exception as exc:
        print(f"FATAL: cannot read sequence/registry: {exc}")
        return 2

    themes = sequence.get("themes", [])
    if len(themes) != 6:
        errors.append(f"approved sequence must contain 6 entries, got {len(themes)}")
    if registry.get("totalSkins") != len(registry.get("themes", [])):
        errors.append("registry totalSkins does not match themes array")
    registry_ids = {x.get("id") for x in registry.get("themes", [])}
    try:
        control_schema = json.loads((root / "control_schema.json").read_text(encoding="utf-8"))
        required_controls = set(control_schema["x-ppgu-contract"]["requiredControlIds"])
    except Exception as exc:
        print(f"FATAL: cannot read canonical control schema: {exc}")
        return 2
    if len(required_controls) != 27:
        errors.append(f"canonical control schema must define exactly 27 IDs, got {len(required_controls)}")

    for item in themes:
        label, stable_id, index = item.get("label"), item.get("stableId"), item.get("index")
        manifest_path = root / item.get("path", "")
        folder = manifest_path.parent
        try:
            theme = json.loads(manifest_path.read_text(encoding="utf-8"))
        except Exception as exc:
            errors.append(f"{label}: cannot read {manifest_path}: {exc}")
            continue
        prefix = f"{label}/{stable_id}"
        if theme.get("id") != stable_id or theme.get("index") != index:
            errors.append(f"{prefix}: manifest identity/index mismatch")
        if theme.get("name", "").find(item.get("name", "")) < 0:
            errors.append(f"{prefix}: approved name mismatch")
        palette = theme.get("palette", {})
        if set(palette) != TOKENS:
            errors.append(f"{prefix}: palette token mismatch (missing={sorted(TOKENS-set(palette))}, extra={sorted(set(palette)-TOKENS)})")
        for token, colour in palette.items():
            if not HEX.fullmatch(str(colour)):
                errors.append(f"{prefix}: invalid colour for {token}: {colour!r}")
        controls = set(theme.get("controls", {}))
        if controls != required_controls:
            missing = sorted(required_controls - controls)
            extra = sorted(controls - required_controls)
            errors.append(f"{prefix}: control-map must match the canonical 27 IDs (missing={missing}, extra={extra})")
        assets = theme.get("assets", {})
        for key, rel in assets.items():
            path = folder / rel
            if not path.is_file():
                message = f"{prefix}: missing asset {rel}"
                (errors if args.strict_assets and key in {"preview", "hero"} else warnings).append(message)
                continue
            if path.suffix.lower() == ".png":
                try:
                    dims = png_info(path)
                    if key == "preview" and dims != (360, 240):
                        errors.append(f"{prefix}: preview dimensions {dims}, expected 360x240")
                    if key == "hero" and dims != (800, 450):
                        errors.append(f"{prefix}: hero dimensions {dims}, expected 800x450")
                except Exception as exc:
                    errors.append(f"{prefix}: {rel}: {exc}")
            elif path.suffix.lower() == ".svg":
                try:
                    text = path.read_text(encoding="utf-8")
                    doc = ET.fromstring(text)
                    if re.search(r"<\s*script\b|javascript:|https?://", text, re.I):
                        errors.append(f"{prefix}: unsafe SVG content in {rel}")
                    for element in doc.iter():
                        for attr in ("href", "{http://www.w3.org/1999/xlink}href"):
                            value = element.attrib.get(attr, "")
                            if value and not value.startswith("#") and not value.startswith("data:image/"):
                                errors.append(f"{prefix}: external SVG reference in {rel}")
                except Exception as exc:
                    errors.append(f"{prefix}: invalid SVG {rel}: {exc}")
        # Full REV18 pack gate: raster masters/runtime exports, all 27x5 control
        # states, icon sources/atlas, licensed-font declaration and checksum manifest.
        required_rasters = {
            "assets/master/hero/hero.png": (1600, 900),
            "assets/hero/hero.png": (800, 450),
            "assets/hero.png": (800, 450),
            "assets/master/preview/preview.png": (720, 480),
            "assets/preview/preview.png": (360, 240),
            "assets/preview.png": (360, 240),
            "assets/master/background/main.png": (2560, 1440),
            "assets/background/main.png": (2560, 1440),
            "assets/master/textures/material.png": (1024, 1024),
            "assets/textures/material.png": (1024, 1024),
            "assets/master/fx_layers/stage_glow.png": (1600, 900),
            "assets/fx_layers/stage_glow.png": (800, 450),
            "assets/master/fx_layers/edge_highlight.png": (1600, 900),
            "assets/fx_layers/edge_highlight.png": (800, 450),
            "assets/master/fx_layers/specular.png": (1600, 900),
            "assets/fx_layers/specular.png": (800, 450),
            "assets/master/fx_layers/shadow.png": (1600, 900),
            "assets/fx_layers/shadow.png": (800, 450),
            "assets/atlas/topbar_icons.png": (768, 512),
        }

        def require_asset(rel: str, dimensions=None) -> None:
            asset_path = folder / rel
            if not asset_path.is_file():
                message = f"{prefix}: missing required pack asset {rel}"
                (errors if args.strict_assets else warnings).append(message)
                return
            if asset_path.suffix.lower() == ".png":
                try:
                    actual = png_info(asset_path)
                    if dimensions and actual != dimensions:
                        errors.append(f"{prefix}: {rel} dimensions {actual}, expected {dimensions}")
                except Exception as exc:
                    errors.append(f"{prefix}: {rel}: {exc}")
            elif asset_path.suffix.lower() == ".svg":
                try:
                    svg_text = asset_path.read_text(encoding="utf-8")
                    doc = ET.fromstring(svg_text)
                    if re.search(r"<\\s*script\\b|javascript:|https?://", svg_text, re.I):
                        errors.append(f"{prefix}: unsafe SVG content in {rel}")
                    for element in doc.iter():
                        for attr in ("href", "{http://www.w3.org/1999/xlink}href"):
                            value = element.attrib.get(attr, "")
                            if value and not value.startswith("#") and not value.startswith("data:image/"):
                                errors.append(f"{prefix}: external SVG reference in {rel}")
                except Exception as exc:
                    errors.append(f"{prefix}: invalid SVG {rel}: {exc}")

        for rel, dims in required_rasters.items():
            require_asset(rel, dims)
        for icon_name in ("skins", "import_guitar", "auto_discover", "curator", "settings", "about"):
            require_asset(f"assets/master/icons/{icon_name}.svg")
            require_asset(f"assets/icons/{icon_name}.png", (256, 256))
        require_asset("assets/atlas/topbar_icons.json")
        require_asset("fonts/font_manifest.json")
        require_asset("assets/artwork_manifest.json")
        require_asset("assets/controls/control_states_manifest.json")
        require_asset("assets/asset_manifest.json")

        for control_id in required_controls:
            for state in ("default", "hover", "pressed", "disabled", "dragged"):
                require_asset(f"assets/master/controls/{control_id}_{state}.png", (512, 512))
                require_asset(f"assets/controls/{control_id}_{state}.png", (256, 256))

        control_manifest_path = folder / "assets/controls/control_states_manifest.json"
        if control_manifest_path.is_file():
            try:
                control_manifest = json.loads(control_manifest_path.read_text(encoding="utf-8"))
                if set(control_manifest.get("controlIds", [])) != required_controls:
                    errors.append(f"{prefix}: control-state manifest does not contain the canonical 27 controls")
                if len(control_manifest.get("records", [])) != len(required_controls) * 5:
                    errors.append(f"{prefix}: control-state manifest must contain 135 control/state records")
                if control_manifest.get("skinId") != stable_id:
                    errors.append(f"{prefix}: control-state manifest stable ID mismatch")
            except Exception as exc:
                errors.append(f"{prefix}: invalid control-state manifest: {exc}")

        font_manifest_path = folder / "fonts/font_manifest.json"
        if font_manifest_path.is_file():
            try:
                font_manifest = json.loads(font_manifest_path.read_text(encoding="utf-8"))
                if font_manifest.get("skinId") != stable_id:
                    errors.append(f"{prefix}: font manifest stable ID mismatch")
                if any(font.get("bundled") for font in font_manifest.get("fonts", [])):
                    errors.append(f"{prefix}: unverified bundled font detected")
            except Exception as exc:
                errors.append(f"{prefix}: invalid font manifest: {exc}")

        asset_manifest_path = folder / "assets/asset_manifest.json"
        if asset_manifest_path.is_file():
            try:
                asset_manifest = json.loads(asset_manifest_path.read_text(encoding="utf-8"))
                records = asset_manifest.get("records", [])
                if asset_manifest.get("skinId") != stable_id:
                    errors.append(f"{prefix}: asset manifest stable ID mismatch")
                if asset_manifest.get("assetCount") != len(records):
                    errors.append(f"{prefix}: asset manifest assetCount mismatch")
                seen_paths = set()
                for record in records:
                    rel = record.get("path", "")
                    if not rel or rel in seen_paths:
                        errors.append(f"{prefix}: duplicate/empty asset manifest path {rel!r}")
                        continue
                    seen_paths.add(rel)
                    asset_path = folder / rel
                    if not asset_path.is_file():
                        errors.append(f"{prefix}: asset manifest references missing file {rel}")
                        continue
                    raw = asset_path.read_bytes()
                    if len(raw) != record.get("bytes"):
                        errors.append(f"{prefix}: byte-size mismatch for {rel}")
                    if hashlib.sha256(raw).hexdigest() != record.get("sha256"):
                        errors.append(f"{prefix}: SHA-256 mismatch for {rel}")
                    if f"{zlib.crc32(raw) & 0xffffffff:08X}" != record.get("crc32"):
                        errors.append(f"{prefix}: CRC32 mismatch for {rel}")
                    if asset_path.suffix.lower() == ".png":
                        try:
                            dims = png_info(asset_path)
                            expected = record.get("dimensions")
                            if expected and expected != f"{dims[0]}x{dims[1]}":
                                errors.append(f"{prefix}: manifest dimensions mismatch for {rel}")
                        except Exception as exc:
                            errors.append(f"{prefix}: invalid PNG in asset manifest {rel}: {exc}")
            except Exception as exc:
                errors.append(f"{prefix}: invalid asset manifest: {exc}")

        if stable_id not in registry_ids and item.get("ordinal", 0) == 1:
            errors.append(f"{prefix}: canonical default missing from registry")
        elif stable_id not in registry_ids:
            warnings.append(f"{prefix}: not yet promoted into canonical registry (expected before PNG acceptance)")

    print(f"REV18 audit: {len(themes)} approved theme specs")
    print(f"Registry: {registry.get('totalSkins')} entries; read-only, no migration performed")
    for message in warnings:
        print("WARN: " + message)
    for message in errors:
        print("FAIL: " + message)
    print(f"Result: {len(errors)} errors, {len(warnings)} warnings")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
