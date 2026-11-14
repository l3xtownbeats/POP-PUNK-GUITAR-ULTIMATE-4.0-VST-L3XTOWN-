#!/usr/bin/env python3
"""Generate deterministic REV.18 NEON STAGE master control-state artwork."""
from __future__ import annotations

import argparse
import hashlib
import json
import zlib
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

STATES = ("default", "hover", "pressed", "disabled", "dragged")
SIZE = (512, 512)


def rgb(value: str) -> tuple[int, int, int]:
    value = value.lstrip("#")
    return tuple(int(value[i:i + 2], 16) for i in (0, 2, 4))


def mix(a, b, amount):
    return tuple(round(x + (y - x) * amount) for x, y in zip(a, b))


def render(control_id: str, state: str, palette: dict) -> Image.Image:
    bg = rgb(palette["window_bg"])
    panel = rgb(palette["panel"])
    raised = rgb(palette["panel_elevated"])
    border = rgb(palette["control_border"])
    primary = rgb(palette["text_primary"])
    secondary = rgb(palette["text_secondary"])
    accent = rgb(palette["accent"])
    if state == "hover":
        accent = rgb(palette["accent_glow"])
    elif state == "pressed":
        accent = mix(accent, bg, 0.28)
    elif state == "disabled":
        accent = rgb(palette["text_disabled"])
        primary = rgb(palette["text_disabled"])
    elif state == "dragged":
        accent = rgb(palette["warning"])

    image = Image.new("RGB", SIZE, bg)
    draw = ImageDraw.Draw(image)
    draw.rounded_rectangle((18, 18, 494, 494), radius=42, fill=panel,
                           outline=border, width=4)
    draw.rounded_rectangle((30, 30, 482, 482), radius=34,
                           outline=mix(border, accent, 0.35), width=2)
    font = ImageFont.load_default()
    name = control_id.replace("_", " ").upper()
    label = state.upper()
    cx = cy = 256

    if control_id.startswith("knob"):
        draw.ellipse((92, 72, 420, 400), fill=bg, outline=border, width=10)
        draw.arc((106, 86, 406, 386), 135, 405, fill=border, width=18)
        draw.arc((106, 86, 406, 386), 135, 305 if state != "disabled" else 210,
                 fill=accent, width=18)
        draw.ellipse((142, 122, 370, 350), fill=raised, outline=border, width=5)
        draw.ellipse((166, 146, 346, 326), fill=panel, outline=mix(border, accent, 0.25), width=3)
        draw.line((cx, 164, cx + 4, 218), fill=accent, width=12)
        draw.ellipse((cx - 13, cy - 13, cx + 13, cy + 13), fill=accent)
        for angle in range(0, 360, 30):
            import math
            x1 = cx + int(184 * math.cos(math.radians(angle)))
            y1 = cy + int(184 * math.sin(math.radians(angle)))
            x2 = cx + int(202 * math.cos(math.radians(angle)))
            y2 = cy + int(202 * math.sin(math.radians(angle)))
            draw.line((x1, y1, x2, y2), fill=secondary, width=4)
    elif control_id.startswith("fx_switch"):
        draw.rounded_rectangle((68, 170, 444, 342), radius=84, fill=bg, outline=border, width=8)
        on = state not in ("disabled",)
        track = accent if on else border
        draw.rounded_rectangle((92, 194, 420, 318), radius=62, fill=track)
        knob_x = 350 if state != "pressed" else 160
        draw.ellipse((knob_x - 54, 202, knob_x + 54, 310), fill=primary, outline=raised, width=7)
        draw.ellipse((knob_x - 18, 238, knob_x + 18, 274), fill=accent)
    elif control_id == "preset_browser":
        draw.rounded_rectangle((62, 74, 450, 414), radius=22, fill=bg, outline=border, width=5)
        draw.rounded_rectangle((62, 74, 450, 136), radius=22, fill=raised)
        for row in range(4):
            y = 158 + row * 58
            draw.rounded_rectangle((82, y, 430, y + 42), radius=9,
                                   fill=raised if row == 0 else panel,
                                   outline=accent if row == 0 else border, width=3)
            draw.rectangle((98, y + 13, 116, y + 29), fill=accent if row == 0 else secondary)
            draw.line((134, y + 21, 360, y + 21), fill=primary if row == 0 else secondary, width=5)
    elif control_id == "mode_selector":
        draw.rounded_rectangle((52, 190, 460, 322), radius=32, fill=bg, outline=border, width=6)
        for i, text in enumerate(("MELODY", "PATTERN", "CHORDS")):
            x = 62 + i * 130
            if i == (1 if state == "pressed" else 0):
                draw.rounded_rectangle((x, 200, x + 122, 312), radius=24, fill=accent)
            draw.text((x + 24, 250), text, font=font, fill=bg if i == 0 else primary)
    elif control_id == "midi_keyboard":
        draw.rounded_rectangle((48, 184, 464, 350), radius=18, fill=bg, outline=border, width=6)
        key_w = 48
        for i in range(8):
            x = 58 + i * key_w
            draw.rounded_rectangle((x, 202, x + key_w - 3, 338), radius=5,
                                   fill=primary, outline=border, width=2)
        for i in (0, 1, 3, 4, 5):
            x = 58 + i * key_w + 30
            draw.rounded_rectangle((x, 202, x + 25, 286), radius=4, fill=bg, outline=border, width=2)
        draw.rounded_rectangle((58 + 3 * key_w, 202, 58 + 4 * key_w - 3, 338),
                               radius=5, outline=accent, width=5)
    elif control_id == "LCD_display":
        draw.rounded_rectangle((54, 158, 458, 354), radius=20, fill=bg, outline=border, width=7)
        draw.rounded_rectangle((72, 176, 440, 336), radius=12, fill=rgb(palette["lcd_bg"]))
        draw.text((96, 204), "NEON STAGE", font=font, fill=rgb(palette["lcd_text"]))
        for i in range(18):
            x = 96 + i * 17
            height = 20 + ((i * 19) % 74)
            draw.line((x, 300 - height // 2, x, 300 + height // 2), fill=accent, width=5)
    elif control_id == "LED_indicator":
        draw.ellipse((104, 104, 408, 408), fill=bg, outline=border, width=8)
        draw.ellipse((154, 154, 358, 358), fill=accent, outline=primary, width=6)
        draw.ellipse((192, 182, 258, 242), fill=mix(primary, accent, 0.35))
    else:
        draw.rounded_rectangle((92, 116, 420, 376), radius=42, fill=raised, outline=accent, width=7)
        # A compact, original geometric glyph keeps each action tile distinct.
        if control_id == "Settings":
            draw.ellipse((204, 196, 308, 300), outline=primary, width=13)
            draw.ellipse((236, 228, 276, 268), fill=accent)
            for angle in range(0, 360, 45):
                import math
                x1 = cx + int(72 * math.cos(math.radians(angle)))
                y1 = cy + int(72 * math.sin(math.radians(angle)))
                x2 = cx + int(106 * math.cos(math.radians(angle)))
                y2 = cy + int(106 * math.sin(math.radians(angle)))
                draw.line((x1, y1, x2, y2), fill=primary, width=11)
        elif control_id == "About":
            draw.ellipse((184, 132, 328, 276), outline=primary, width=12)
            draw.ellipse((242, 166, 270, 194), fill=accent)
            draw.rounded_rectangle((244, 210, 268, 264), radius=8, fill=accent)
        elif control_id == "Import_Guitar":
            draw.ellipse((154, 212, 260, 320), outline=primary, width=12)
            draw.line((220, 240, 342, 118), fill=primary, width=16)
            draw.polygon([(326, 110), (370, 70), (408, 108), (366, 150)], outline=accent)
            draw.line((302, 318, 392, 318), fill=accent, width=12)
            draw.line((348, 274, 348, 362), fill=accent, width=12)
        else:
            draw.line((172, 250, 250, 172, 330, 250), fill=primary, width=14)
            draw.line((196, 246, 196, 320, 304, 320, 304, 246), fill=primary, width=12)
            draw.ellipse((238, 238, 262, 262), fill=accent)

    draw.text((42, 438), name[:38], font=font, fill=primary)
    draw.text((42, 462), label, font=font, fill=accent)
    return image


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path("SKIN"))
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    skin = args.root / "Skin_001"
    theme = json.loads((skin / "theme.json").read_text(encoding="utf-8"))
    schema = json.loads((args.root / "control_schema.json").read_text(encoding="utf-8"))
    controls = list(schema["x-ppgu-contract"]["requiredControlIds"])
    if "sync_selector" not in controls:
        controls.append("sync_selector")
    palette = theme["palette"]
    folder = skin / "assets" / "master" / "controls"
    expected = {}
    records = []
    for control_id in controls:
        for state in STATES:
            rel = f"{control_id}_{state}.png"
            image = render(control_id, state, palette)
            from io import BytesIO
            buffer = BytesIO()
            image.save(buffer, format="PNG", optimize=False, compress_level=9)
            raw = buffer.getvalue()
            expected[rel] = raw
            records.append({
                "id": f"control.{control_id}.{state}",
                "path": f"assets/master/controls/{rel}",
                "format": "PNG",
                "bytes": len(raw),
                "dimensions": "512x512",
                "sha256": hashlib.sha256(raw).hexdigest(),
                "crc32": f"{zlib.crc32(raw) & 0xffffffff:08X}",
                "origin": "[DECISION] deterministic original NEON STAGE control-state artwork",
            })
    errors = []
    for rel, raw in expected.items():
        path = folder / rel
        if args.check:
            if not path.is_file() or path.read_bytes() != raw:
                errors.append(str(path))
        else:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(raw)
    manifest = {
        "schemaVersion": "1.0.0",
        "skinId": "skin-001",
        "theme": "NEON STAGE DEFAULT",
        "assetGroup": "master-control-state-frames",
        "states": list(STATES),
        "controlIds": controls,
        "records": records,
    }
    manifest_path = folder / "control_states_manifest.json"
    manifest_raw = (json.dumps(manifest, indent=2, ensure_ascii=False) + "\n").encode("utf-8")
    if args.check:
        if not manifest_path.is_file() or manifest_path.read_bytes() != manifest_raw:
            errors.append(str(manifest_path))
    else:
        folder.mkdir(parents=True, exist_ok=True)
        manifest_path.write_bytes(manifest_raw)
    if errors:
        print("REV18 control-state mismatch: " + ", ".join(errors))
        return 1
    print(f"REV18 control frames: {'verified' if args.check else 'generated'}; {len(expected)} PNGs")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
