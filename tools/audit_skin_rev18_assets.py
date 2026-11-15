#!/usr/bin/env python3
"""Read-only REV18 audit for approved PPGU themes; never modifies registries."""
from __future__ import annotations

import argparse
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
URL_FUNCTION = re.compile(r"url\(\s*([^)]+?)\s*\)", re.I)


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


def svg_safety_errors(source: str, label: str = "SVG") -> list[str]:
    """Reject active content/external loads; allow W3C namespaces and url(#id)."""
    errors: list[str] = []
    if re.search(r"<!\s*(?:DOCTYPE|ENTITY)\b", source, re.I):
        errors.append(f"{label}: DTD/entity declarations are forbidden")
        return errors
    try:
        root = ET.fromstring(source)
    except ET.ParseError as exc:
        return [f"{label}: invalid XML: {exc}"]

    for element in root.iter():
        tag = str(element.tag).rsplit("}", 1)[-1].lower()
        if tag == "script":
            errors.append(f"{label}: script element is forbidden")
        elif tag == "foreignobject":
            errors.append(f"{label}: foreignObject element is forbidden")

        for raw_name, raw_value in element.attrib.items():
            name = str(raw_name).rsplit("}", 1)[-1].lower()
            value = str(raw_value).strip()
            lowered = value.lower()
            if name.startswith("on"):
                errors.append(f"{label}: event-handler attribute {name!r} is forbidden")
            if name == "base":
                errors.append(f"{label}: xml:base is forbidden")
            if "javascript:" in lowered:
                errors.append(f"{label}: javascript URL is forbidden")
            if name == "href" and value and not value.startswith("#") and not lowered.startswith("data:image/"):
                errors.append(f"{label}: external href is forbidden: {value!r}")
            for match in URL_FUNCTION.finditer(value):
                target = match.group(1).strip().strip("\"'")
                if target and not target.startswith("#") and not target.lower().startswith("data:image/"):
                    errors.append(f"{label}: external url() reference is forbidden: {target!r}")

        if tag == "style":
            css = element.text or ""
            if re.search(r"@import|javascript:|https?://|url\(\s*//", css, re.I):
                errors.append(f"{label}: external CSS/import content is forbidden")
            for match in URL_FUNCTION.finditer(css):
                target = match.group(1).strip().strip("\"'")
                if target and not target.startswith("#") and not target.lower().startswith("data:image/"):
                    errors.append(f"{label}: external CSS url() reference is forbidden: {target!r}")
    return errors


def safe_relative_path(root: Path, folder: Path, relative: str) -> Path:
    rel = Path(relative)
    if rel.is_absolute() or ".." in rel.parts:
        raise ValueError(f"unsafe relative asset path: {relative!r}")
    resolved_root = root.resolve()
    resolved = (folder / rel).resolve()
    if resolved != resolved_root and resolved_root not in resolved.parents:
        raise ValueError(f"asset path escapes SKIN root: {relative!r}")
    return resolved


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", type=Path, default=Path("SKIN"))
    ap.add_argument("--strict-assets", action="store_true",
                    help="fail when referenced preview/hero raster assets are absent")
    args = ap.parse_args()
    root = args.root
    errors: list[str] = []
    warnings: list[str] = []
    try:
        sequence = json.loads((root / "approved_theme_sequence.json").read_text(encoding="utf-8"))
        registry = json.loads((root / "theme_manifest.json").read_text(encoding="utf-8"))
        schema = json.loads((root / "control_schema.json").read_text(encoding="utf-8"))
    except Exception as exc:
        print(f"FATAL: cannot read sequence/registry/control schema: {exc}")
        return 2

    themes = sequence.get("themes", [])
    registry_entries = registry.get("themes", [])
    if len(themes) != 16:
        errors.append(f"approved sequence must contain 16 entries, got {len(themes)}")
    if registry.get("totalSkins") != len(registry_entries):
        errors.append("registry totalSkins does not match themes array")
    registry_ids = {entry.get("id") for entry in registry_entries}
    ordinals = [item.get("ordinal") for item in themes if isinstance(item, dict)]
    if ordinals != list(range(1, 17)):
        errors.append(f"approved sequence ordinals must be exactly 1..16, got {ordinals}")

    stable_ids = [item.get("stableId") for item in themes if isinstance(item, dict)]
    indices = [item.get("index") for item in themes if isinstance(item, dict)]
    if len(set(stable_ids)) != len(stable_ids):
        errors.append("approved sequence contains duplicate stable IDs")
    if len(set(indices)) != len(indices):
        errors.append("approved sequence contains duplicate stable indices")
    if (not themes or themes[0].get("stableId") != "skin-053"
            or themes[0].get("label") != "SKIN-1"
            or themes[0].get("name") != "NEON FORGE"):
        errors.append("SKIN-1 must be the first entry: label SKIN-1, name NEON FORGE, stable ID skin-053")

    contract = schema.get("x-ppgu-contract", {})
    required_control_ids = set(contract.get("requiredControlIds", []))
    schema_tokens = set(contract.get("paletteTokens", []))
    if schema_tokens != TOKENS:
        errors.append("control_schema paletteTokens do not match the 21 canonical tokens")

    reference_controls = None
    reference_typography = None
    for item in themes:
        label = str(item.get("label", "<unnamed>"))
        stable_id = str(item.get("stableId", ""))
        index = item.get("index")
        relative_manifest = str(item.get("path", ""))
        prefix = f"{label}/{stable_id}"
        try:
            manifest_path = safe_relative_path(root, root, relative_manifest)
            folder = manifest_path.parent
            theme = json.loads(manifest_path.read_text(encoding="utf-8"))
        except Exception as exc:
            errors.append(f"{prefix}: cannot read safe manifest path {relative_manifest!r}: {exc}")
            continue

        if theme.get("id") != stable_id or theme.get("index") != index:
            errors.append(f"{prefix}: manifest identity/index mismatch")
        expected_name = item.get("name", "") if item.get("ordinal") == 1 else f"{label} — {item.get('name', '')}"
        if theme.get("name") != expected_name:
            errors.append(f"{prefix}: approved name mismatch (manifest={theme.get('name')!r}, sequence={expected_name!r})")

        palette = theme.get("palette", {})
        if set(palette) != TOKENS:
            errors.append(f"{prefix}: palette token mismatch (missing={sorted(TOKENS-set(palette))}, extra={sorted(set(palette)-TOKENS)})")
        for token, colour in palette.items():
            if not HEX.fullmatch(str(colour)):
                errors.append(f"{prefix}: invalid colour for {token}: {colour!r}")

        controls = set(theme.get("controls", {}))
        if reference_controls is None:
            reference_controls = controls
            missing_required = required_control_ids - controls
            if missing_required:
                errors.append(f"{prefix}: missing required control IDs {sorted(missing_required)}")
        elif controls != reference_controls:
            errors.append(f"{prefix}: control-map keys differ from SKIN-1")

        typography = theme.get("typography", {})
        if reference_typography is None:
            reference_typography = typography
        elif typography != reference_typography:
            errors.append(f"{prefix}: typography differs from the shared NEON STAGE system")

        compatibility = theme.get("compatibility", {})
        for flag in ("apvtsChanges", "layoutChanges", "audioChanges"):
            if compatibility.get(flag) is not False:
                errors.append(f"{prefix}: compatibility.{flag} must be false for a visual-only theme")

        assets = theme.get("assets", {})
        effects = theme.get("effects", {})
        svg_paths = [assets.get(key) for key in ("texture", "bezel", "crest")]
        if any(not isinstance(path, str) or not path for path in svg_paths):
            errors.append(f"{prefix}: texture/bezel/crest SVG paths must all be declared")
        elif len(set(svg_paths)) != 3:
            errors.append(f"{prefix}: texture, bezel and crest must be separate resources")
        for key in ("texture", "bezel", "crest"):
            if effects.get(key) != assets.get(key):
                errors.append(f"{prefix}: effects.{key} must match assets.{key}")

        for key, rel in assets.items():
            if not isinstance(rel, str):
                errors.append(f"{prefix}: asset path for {key} must be a string")
                continue
            try:
                path = safe_relative_path(root, folder, rel)
            except Exception as exc:
                errors.append(f"{prefix}: unsafe asset path for {key}: {exc}")
                continue
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
                    svg_text = path.read_text(encoding="utf-8")
                    errors.extend(svg_safety_errors(svg_text, f"{prefix}: {rel}"))
                except Exception as exc:
                    errors.append(f"{prefix}: invalid SVG {rel}: {exc}")

        if stable_id not in registry_ids and item.get("ordinal", 0) == 1:
            errors.append(f"{prefix}: canonical default missing from registry")
        elif stable_id not in registry_ids:
            warnings.append(f"{prefix}: not yet promoted into canonical registry (expected before PNG acceptance)")

    if registry.get("defaultSkin") != "skin-053":
        warnings.append(f"registry default remains {registry.get('defaultSkin')!r}; no-loss SKIN-1 migration is still pending")

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
