#!/usr/bin/env python3
"""Generate one validated 50-theme PPGU Skin Studio batch using stdlib only."""
from __future__ import annotations

import argparse
import copy
import json
import math
import shutil
import struct
import subprocess
import sys
import tempfile
import zlib
from pathlib import Path

FAMILIES = (
    "carbon_studio", "neon_basement", "chrome_parade",
    "wood_summer", "vintage_tape", "sunset_suburb",
)
LAYOUTS = ("STUDIO_CONSOLE", "WORKSTATION_TOPBAR", "RACK_SIDE", "COMPACT_MINI")
ACCENTS = {
    "carbon_studio": ("#D5FF45", "#7CFFCB", "#B8F05A", "#E0FF70"),
    "neon_basement": ("#2DD4FF", "#FF2D9E", "#B45CFF", "#35E6D0"),
    "chrome_parade": ("#9BE7FF", "#C5B8FF", "#F1F5F9", "#72D6C8"),
    "wood_summer": ("#FFC857", "#FFB36B", "#E8D18A", "#A9D18E"),
    "vintage_tape": ("#FF7F50", "#F4D35E", "#E99A9A", "#A8C686"),
    "sunset_suburb": ("#FF6B6B", "#F72585", "#FF9F68", "#C77DFF"),
    "special": ("#FF453A", "#EAF2F7"),
}
BACKGROUNDS = {
    "carbon_studio": ("#0B0F12", "#141B20", "#1D2930"),
    "neon_basement": ("#100B18", "#1C1224", "#291936"),
    "chrome_parade": ("#0D131A", "#19232D", "#263442"),
    "wood_summer": ("#17100B", "#241A12", "#33261A"),
    "vintage_tape": ("#111014", "#21191B", "#302326"),
    "sunset_suburb": ("#170D15", "#25131F", "#351D2B"),
    "special": ("#111418", "#1C2329", "#2A343C"),
}


def rgb(value: str) -> tuple[int, int, int]:
    return tuple(int(value[i:i + 2], 16) for i in (1, 3, 5))


def hex_rgb(value: tuple[int, int, int]) -> str:
    return "#" + "".join(f"{max(0, min(255, c)):02X}" for c in value)


def mix(a: str, b: str, amount: float) -> str:
    aa, bb = rgb(a), rgb(b)
    return hex_rgb(tuple(round(x + (y - x) * amount) for x, y in zip(aa, bb)))


def luminance(value: str) -> float:
    channels = []
    for component in rgb(value):
        v = component / 255.0
        channels.append(v / 12.92 if v <= 0.04045 else ((v + 0.055) / 1.055) ** 2.4)
    return 0.2126 * channels[0] + 0.7152 * channels[1] + 0.0722 * channels[2]


def contrast(a: str, b: str) -> float:
    high, low = sorted((luminance(a), luminance(b)), reverse=True)
    return (high + 0.05) / (low + 0.05)


def ensure_contrast(foreground: str, background: str, minimum: float) -> str:
    if contrast(foreground, background) >= minimum:
        return foreground
    target = "#FFFFFF" if contrast("#FFFFFF", background) >= contrast("#000000", background) else "#000000"
    low, high = 0.0, 1.0
    for _ in range(24):
        middle = (low + high) * 0.5
        candidate = mix(foreground, target, middle)
        if contrast(candidate, background) >= minimum:
            high = middle
        else:
            low = middle
    return mix(foreground, target, high)


def make_palette(family: str, variant: int) -> dict[str, str]:
    bg, panel, elevated = BACKGROUNDS[family]
    accents = ACCENTS[family]
    accent = accents[variant % len(accents)]
    if family == "special" and variant == 1:
        accent = "#EAF2F7"
    accent = ensure_contrast(accent, panel, 2.2)
    text_disabled = ensure_contrast("#879791", bg, 4.0)
    text_secondary = ensure_contrast("#B7C5C2", bg, 4.0)
    return {
        "window_bg": bg,
        "panel": panel,
        "panel_elevated": elevated,
        "control_bg": mix(bg, panel, 0.48),
        "control_face": mix(panel, elevated, 0.58),
        "control_border": "#53656B",
        "text_primary": ensure_contrast("#F3F7F5", bg, 4.0),
        "text_secondary": text_secondary,
        "text_disabled": text_disabled,
        "accent": accent,
        "accent_glow": mix(accent, "#FFFFFF", 0.22),
        "danger": "#FF5A66",
        "warning": "#FFC857",
        "success": "#6EE7A8",
        "lcd_bg": "#08110E",
        "lcd_text": ensure_contrast("#D7FFB2", "#08110E", 4.0),
        "led_on": accent,
        "led_off": "#33443B",
        "shadow": "#050708",
        "highlight": "#90A7A0",
        "lowlight": "#0D1417",
    }


def png_chunk(kind: bytes, payload: bytes) -> bytes:
    body = kind + payload
    return struct.pack(">I", len(payload)) + body + struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)


def write_png(path: Path, width: int, height: int, palette: dict[str, str], seed: int) -> None:
    background = rgb(palette["window_bg"])
    panel = rgb(palette["panel"])
    accent = rgb(palette["accent"])
    rows = bytearray()
    center_x, center_y = int(width * 0.60), int(height * 0.53)
    body_rx, body_ry = max(8, int(width * 0.105)), max(10, int(height * 0.23))
    neck_x = center_x - int(width * 0.04)
    for y in range(height):
        rows.append(0)
        t = y / max(1, height - 1)
        base = tuple(round(background[i] + (panel[i] - background[i]) * t * 0.72) for i in range(3))
        for x in range(width):
            colour = base
            stripe = (x + y * (0.28 + (seed % 5) * 0.035) + seed * 17) % (max(48, width // 7))
            if stripe < max(2, width // 180):
                colour = tuple(round(colour[i] * 0.62 + accent[i] * 0.38) for i in range(3))
            nx = (x - center_x) / body_rx
            ny = (y - center_y) / body_ry
            if nx * nx + ny * ny <= 1.0:
                colour = tuple(round(accent[i] * 0.38 + panel[i] * 0.62) for i in range(3))
            if abs(x - neck_x) <= max(2, width // 100) and y < center_y + body_ry // 2:
                colour = tuple(round(accent[i] * 0.68 + 18 * 0.32) for i in range(3))
            if abs(x - center_x) <= max(1, width // 180) and abs(y - center_y) <= max(2, height // 80):
                colour = (238, 245, 245)
            rows.extend(colour)
    raw = bytes(rows)
    png = (b"\x89PNG\r\n\x1a\n"
           + png_chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0))
           + png_chunk(b"IDAT", zlib.compress(raw, 7))
           + png_chunk(b"IEND", b""))
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(png)


def write_svg(path: Path, accent: str, accent2: str, seed: int, layer: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    angle = 18 + (seed * 13) % 45
    svg = f'''<svg xmlns="http://www.w3.org/2000/svg" width="800" height="450" viewBox="0 0 800 450">
<defs><linearGradient id="g{seed}" x1="0" y1="0" x2="1" y2="1">
<stop offset="0" stop-color="{accent}" stop-opacity=".42"/><stop offset="1" stop-color="{accent2}" stop-opacity=".04"/>
</linearGradient></defs>
<rect width="800" height="450" fill="url(#g{seed})"/>
<path d="M-40 {100+seed%90} L840 {260+seed%70}" stroke="{accent}" stroke-opacity=".42" stroke-width="3"/>
<path d="M{80+seed%120} 450 L{420+seed%90} 0" stroke="{accent2}" stroke-opacity=".32" stroke-width="2"/>
<path d="M300 330 C{250+seed%30} 260 {320+seed%40} 205 390 235 C470 270 435 365 365 380 Z" fill="none" stroke="{accent}" stroke-width="8"/>
<path d="M380 270 L{500+seed%30} 100" stroke="{accent2}" stroke-width="13" stroke-linecap="round"/>
<text x="36" y="410" fill="{accent}" font-family="sans-serif" font-size="16" letter-spacing="{angle}">{layer.upper()}</text>
</svg>
'''
    path.write_text(svg, encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path("SKIN"))
    parser.add_argument("--manifest", type=Path, default=Path("SKIN/theme_pack_manifest.json"))
    parser.add_argument("--count", type=int, default=50)
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    root = args.root.resolve()
    manifest_path = args.manifest.resolve()
    if args.count != 50:
        print("This production batch is fixed at 50 themes (6 families x 8 + 2 special).", file=sys.stderr)
        return 2
    try:
        registry = json.loads((root / "theme_manifest.json").read_text(encoding="utf-8"))
        base = json.loads((root / "Skin_052" / "theme.json").read_text(encoding="utf-8"))
        schema = json.loads((root / "control_schema.json").read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        print(f"Cannot read generator inputs: {exc}", file=sys.stderr)
        return 2

    existing_total = registry.get("totalSkins")
    if not isinstance(existing_total, int) or isinstance(existing_total, bool) or existing_total != len(registry.get("themes", [])):
        print("Registry totalSkins does not match themes array.", file=sys.stderr)
        return 2
    start = existing_total + 1
    end = start + 49
    if base.get("index") != 52 or not schema.get("x-ppgu-contract", {}).get("requiredControlIds"):
        print("Skin_052 base or control schema is invalid.", file=sys.stderr)
        return 2

    if manifest_path.exists():
        try:
            old_pack = json.loads(manifest_path.read_text(encoding="utf-8"))
            if old_pack.get("startIndex", 0) > existing_total:
                print("An unmerged skin pack already occupies the next registry range.", file=sys.stderr)
                return 2
        except json.JSONDecodeError as exc:
            print(f"Existing pack manifest is invalid: {exc}", file=sys.stderr)
            return 2

    families = [family for family in FAMILIES for _ in range(8)] + ["special", "special"]
    names = {
        "carbon_studio": "Carbon Studio",
        "neon_basement": "Neon Basement",
        "chrome_parade": "Chrome Parade",
        "wood_summer": "Wood Summer",
        "vintage_tape": "Vintage Tape",
        "sunset_suburb": "Sunset Suburb",
        "special": ("Hazard Line", "Whiteout Bleach"),
    }
    layouts = list(LAYOUTS)
    entries = []
    for offset, family in enumerate(families):
        index = start + offset
        name = names[family][offset - 48] if family == "special" else f"{names[family]} {offset % 8 + 1:02d}"
        entries.append({
            "id": f"skin-{index:03d}",
            "index": index,
            "name": name,
            "family": family,
            "layout": layouts[offset % len(layouts)],
            "path": f"Skin_{index:03d}/theme.json",
            "preview": f"Skin_{index:03d}/assets/preview.png",
            "hero": f"Skin_{index:03d}/assets/hero.png",
        })

    pack = {
        "schemaVersion": "1.0.0",
        "product": "POP PUNK GUITAR ULTIMATE",
        "contract": "PPGU SKIN STUDIO REV 14",
        "status": "GENERATED_NOT_MERGED",
        "startIndex": start,
        "endIndex": end,
        "totalSkins": 50,
        "themes": entries,
    }
    if args.dry_run:
        print(f"DRY RUN: {len(entries)} themes, indices {start}..{end}, 4 layouts, 6 families + 2 special.")
        return 0

    collisions = [root / f"Skin_{entry['index']:03d}" for entry in entries
                  if (root / f"Skin_{entry['index']:03d}").exists()]
    if collisions:
        print("Refusing to overwrite existing skin folders: " + ", ".join(p.name for p in collisions), file=sys.stderr)
        return 2

    staging = Path(tempfile.mkdtemp(prefix=".ppgu_skin_batch_", dir=root))
    created: list[Path] = []
    previous_manifest = manifest_path.read_bytes() if manifest_path.exists() else None
    try:
        for offset, entry in enumerate(entries):
            family = entry["family"]
            index = entry["index"]
            palette = make_palette(family, offset)
            theme = copy.deepcopy(base)
            theme.update({
                "id": entry["id"],
                "index": index,
                "name": entry["name"],
                "family": family,
                "layout": entry["layout"],
                "description": f"{entry['name']}: a distinct {family.replace('_', ' ')} visual shell for PPGU.",
                "palette": palette,
                "designStatus": "GENERATED_NOT_MERGED",
                "notes": [
                    "Generated by tools/generate_skin_batch.py.",
                    "Visual-only theme: no APVTS, live layout, audio or MIDI changes.",
                    "Control coordinates remain metadata until the runtime layout engine consumes them."
                ],
            })
            folder = staging / f"Skin_{index:03d}"
            (folder / "assets").mkdir(parents=True)
            (folder / "svg").mkdir(parents=True)
            (folder / "theme.json").write_text(json.dumps(theme, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
            write_png(folder / "assets/preview.png", 360, 240, palette, index)
            write_png(folder / "assets/hero.png", 800, 450, palette, index + 17)
            write_svg(folder / "svg/texture.svg", palette["accent"], palette["accent_glow"], index, "texture")
            write_svg(folder / "svg/bezel.svg", palette["accent_glow"], palette["control_border"], index + 1, "bezel")
            write_svg(folder / "svg/crest.svg", palette["accent"], palette["text_primary"], index + 2, "crest")

        for entry in entries:
            source = staging / f"Skin_{entry['index']:03d}"
            destination = root / source.name
            shutil.move(str(source), str(destination))
            created.append(destination)

        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        temporary_manifest = manifest_path.with_suffix(manifest_path.suffix + ".tmp")
        temporary_manifest.write_text(json.dumps(pack, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        temporary_manifest.replace(manifest_path)

        validator = Path(__file__).resolve().parent / "validate_skin_pack.py"
        result = subprocess.run(
            [sys.executable, str(validator), "--root", str(root), "--manifest", str(manifest_path)],
            capture_output=True, text=True
        )
        if result.returncode:
            print(result.stdout, end="")
            print(result.stderr, end="", file=sys.stderr)
            raise RuntimeError("Generated skin batch failed G1-G4 validation.")
        print(result.stdout, end="")
        print(f"GENERATED AND VALIDATED {len(entries)} themes: {start}..{end}.")
        return 0
    except Exception as exc:
        for folder in created:
            shutil.rmtree(folder, ignore_errors=True)
        if previous_manifest is None:
            manifest_path.unlink(missing_ok=True)
        else:
            manifest_path.write_bytes(previous_manifest)
        print(f"Generation rolled back: {exc}", file=sys.stderr)
        return 1
    finally:
        shutil.rmtree(staging, ignore_errors=True)


if __name__ == "__main__":
    raise SystemExit(main())
