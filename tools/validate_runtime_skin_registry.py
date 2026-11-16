#!/usr/bin/env python3
"""Validate only the approved themes currently admitted to the REV.19 runtime registry."""
from __future__ import annotations
import json
import sys
import zlib
from pathlib import Path
from xml.etree import ElementTree as ET

def fail(message: str) -> None:
    raise ValueError(message)

def load_json(path: Path):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        fail(f"{path}: invalid JSON: {exc}")

def safe_asset(theme_dir: Path, relative: str, label: str) -> Path:
    if not isinstance(relative, str) or not relative or relative.startswith("/") or "\\" in relative:
        fail(f"{theme_dir}: invalid {label} asset path")
    if any(part in ("", ".", "..") for part in Path(relative).parts):
        fail(f"{theme_dir}: unsafe {label} asset path")
    candidate = (theme_dir / relative).resolve()
    try:
        candidate.relative_to(theme_dir.resolve())
    except ValueError:
        fail(f"{theme_dir}: {label} asset escapes its theme directory")
    if not candidate.is_file():
        fail(f"{theme_dir}: missing {label} asset: {relative}")
    return candidate

def validate_png(path: Path, expected: tuple[int, int]) -> None:
    data = path.read_bytes()
    if len(data) < 45 or data[:8] != b"\x89PNG\r\n\x1a\n":
        fail(f"{path}: invalid PNG signature or truncated file")
    offset, dimensions, saw_iend = 8, None, False
    while offset + 12 <= len(data):
        length = int.from_bytes(data[offset:offset + 4], "big")
        end = offset + 12 + length
        if end > len(data):
            fail(f"{path}: truncated PNG chunk")
        kind = data[offset + 4:offset + 8]
        payload = data[offset + 8:offset + 8 + length]
        expected_crc = int.from_bytes(data[offset + 8 + length:end], "big")
        actual_crc = zlib.crc32(payload, zlib.crc32(kind)) & 0xFFFFFFFF
        if actual_crc != expected_crc:
            fail(f"{path}: CRC mismatch in {kind.decode('ascii', 'replace')} chunk")
        if kind == b"IHDR":
            if length != 13:
                fail(f"{path}: invalid IHDR length")
            dimensions = (int.from_bytes(payload[0:4], "big"), int.from_bytes(payload[4:8], "big"))
        if kind == b"IEND":
            saw_iend = length == 0
            break
        offset = end
    if not saw_iend or dimensions != expected:
        fail(f"{path}: expected PNG {expected[0]}x{expected[1]}, got {dimensions}")

def validate_svg(path: Path) -> None:
    try:
        root = ET.parse(path).getroot()
    except Exception as exc:
        fail(f"{path}: invalid SVG/XML: {exc}")
    if root.tag.split("}")[-1].lower() != "svg":
        fail(f"{path}: root element is not SVG")
    for element in root.iter():
        tag = element.tag.split("}")[-1].lower()
        if tag in {"script", "foreignobject"}:
            fail(f"{path}: forbidden SVG element {tag}")
        for key, value in element.attrib.items():
            name, text = key.split("}")[-1].lower(), str(value).strip().lower()
            if name.startswith("on"):
                fail(f"{path}: event handler attribute is forbidden")
            if name in {"href", "src"} and text and not text.startswith("#"):
                fail(f"{path}: external SVG resource reference is forbidden")
            if any(token in text for token in ("http://", "https://", "file://", "data:")):
                fail(f"{path}: external SVG resource reference is forbidden")

def validate(root: Path) -> int:
    registry = load_json(root / "theme_manifest.json")
    sequence = load_json(root / "approved_theme_sequence.json")
    schema = load_json(root / "control_schema.json")
    contract = schema.get("x-ppgu-contract", {})
    required, tokens = contract.get("requiredControlIds"), contract.get("paletteTokens")
    approved, active = sequence.get("themes"), registry.get("themes")
    if not isinstance(required, list) or len(required) != 27 or len(required) != len(set(required)):
        fail("control_schema.json must define exactly 27 unique required control IDs")
    if not isinstance(tokens, list) or len(tokens) != 21 or len(tokens) != len(set(tokens)):
        fail("control_schema.json must define exactly 21 unique palette tokens")
    if not isinstance(approved, list) or len(approved) != 16:
        fail("approved_theme_sequence.json must define exactly 16 factory themes")
    if not isinstance(active, list) or not active or registry.get("totalSkins") != len(active):
        fail("runtime theme registry is empty or totalSkins does not match its themes array")
    if registry.get("defaultSkin") != sequence.get("defaultStableId") or registry.get("defaultOrdinal") != 1:
        fail("runtime default must be logical SKIN-1 / the approved default stable ID")
    approved_by_ordinal = {}
    for ordinal, item in enumerate(approved, start=1):
        if item.get("ordinal") != ordinal:
            fail("approved theme sequence ordinals must be contiguous from 1 to 16")
        approved_by_ordinal[ordinal] = item
    if len(active) > len(approved):
        fail("runtime registry contains more themes than the approved 16-theme sequence")
    seen_ids = set()
    for ordinal, entry in enumerate(active, start=1):
        if not isinstance(entry, dict) or entry.get("ordinal") != ordinal:
            fail("runtime themes must be admitted as a contiguous approved prefix")
        approved_item = approved_by_ordinal[ordinal]
        for key, seq_key in (("id", "stableId"), ("index", "index"), ("path", "path")):
            if entry.get(key) != approved_item.get(seq_key):
                fail(f"runtime registry mismatch for SKIN-{ordinal}: {key}")
        if entry.get("id") in seen_ids:
            fail(f"duplicate runtime theme ID: {entry.get('id')}")
        seen_ids.add(entry.get("id"))
        theme_path = safe_asset(root, entry.get("path"), f"SKIN-{ordinal} theme manifest")
        theme_dir, theme = theme_path.parent, load_json(theme_path)
        if theme.get("id") != entry.get("id") or theme.get("index") != entry.get("index"):
            fail(f"{theme_path}: stable ID/index mismatch")
        for key in ("name", "family", "layout"):
            if theme.get(key) != entry.get(key):
                fail(f"{theme_path}: runtime registry {key} mismatch")
        palette, controls = theme.get("palette"), theme.get("controls")
        if not isinstance(palette, dict) or set(palette) != set(tokens):
            fail(f"{theme_path}: palette must contain exactly the 21 contract tokens")
        if not isinstance(controls, dict) or set(controls) != set(required) | {"sync_selector"}:
            fail(f"{theme_path}: controls must contain exactly 27 canonical IDs plus sync_selector")
        compatibility = theme.get("compatibility", {})
        if compatibility.get("apvtsChanges") or compatibility.get("layoutChanges") or compatibility.get("audioChanges"):
            fail(f"{theme_path}: skin must remain visual-only")
        if theme.get("reduceInEco") is not True or theme.get("hotSwapFadeMs") != 150:
            fail(f"{theme_path}: ECO and 150 ms hot-swap contract mismatch")
        assets = theme.get("assets", {})
        validate_png(safe_asset(theme_dir, assets.get("preview"), f"SKIN-{ordinal} preview"), (360, 240))
        validate_png(safe_asset(theme_dir, assets.get("hero"), f"SKIN-{ordinal} hero"), (800, 450))
        effects = theme.get("effects", {})
        for key in ("texture", "bezel", "crest"):
            validate_svg(safe_asset(theme_dir, effects.get(key), f"SKIN-{ordinal} {key}"))
    if active[0].get("id") != sequence.get("defaultStableId"):
        fail("the first active runtime theme must be NEON STAGE DEFAULT")
    return len(active)

def main() -> int:
    root = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("SKIN")
    try:
        count = validate(root)
    except Exception as exc:
        print(f"RUNTIME SKIN REGISTRY: FAIL: {exc}", file=sys.stderr)
        return 1
    print(f"RUNTIME SKIN REGISTRY: PASS — {count} approved runtime theme(s); 27-control and asset gates verified.")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
