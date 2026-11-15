#!/usr/bin/env python3
"""Generate deterministic REV18 SKIN-1 top-bar PNG exports and checksum manifest."""
from __future__ import annotations
import argparse
import hashlib
import io
import json
import zlib
from pathlib import Path
from PIL import Image, ImageDraw

SIZE = 256
BG = (0, 0, 0, 0)
PRIMARY = (243, 247, 245, 255)
ACCENT = (213, 255, 69, 255)
STROKE = 12
NAMES = ("skins", "import_guitar", "auto_discover", "curator", "settings", "about")


def render(name: str) -> Image.Image:
    im = Image.new("RGBA", (SIZE, SIZE), BG)
    d = ImageDraw.Draw(im)
    if name == "skins":
        d.rounded_rectangle((44, 44, 212, 212), radius=24, outline=PRIMARY, width=STROKE)
        for x in (100, 156): d.line((x, 44, x, 212), fill=PRIMARY, width=STROKE)
        for y in (100, 156): d.line((44, y, 212, y), fill=PRIMARY, width=STROKE)
        for x, y in ((72,72),(128,128),(184,184)):
            d.ellipse((x-7,y-7,x+7,y+7), fill=ACCENT)
    elif name == "import_guitar":
        d.ellipse((42, 112, 112, 202), outline=PRIMARY, width=STROKE)
        d.ellipse((68, 104, 132, 176), outline=PRIMARY, width=STROKE)
        d.line((104, 124, 170, 58), fill=PRIMARY, width=STROKE)
        d.polygon([(158,45),(180,23),(218,61),(196,83)], outline=PRIMARY, fill=BG)
        d.line((92, 134, 144, 82), fill=PRIMARY, width=5)
        d.line((82, 124, 134, 72), fill=PRIMARY, width=5)
        d.line((168, 164, 224, 164), fill=ACCENT, width=STROKE)
        d.line((196, 136, 196, 192), fill=ACCENT, width=STROKE)
    elif name == "auto_discover":
        d.ellipse((48,48,184,184), outline=PRIMARY, width=STROKE)
        d.line((116,48,116,184), fill=PRIMARY, width=STROKE)
        d.line((48,116,184,116), fill=PRIMARY, width=STROKE)
        d.line((68,68,164,164), fill=PRIMARY, width=STROKE)
        d.line((164,68,68,164), fill=PRIMARY, width=STROKE)
        d.polygon([(116,80),(130,116),(116,152),(102,116)], outline=ACCENT, fill=ACCENT)
        d.line((166,166,218,218), fill=PRIMARY, width=STROKE)
    elif name == "curator":
        d.rounded_rectangle((44,48,212,208), radius=20, outline=PRIMARY, width=STROKE)
        for y, x2 in ((88,180),(128,180),(168,132)):
            d.line((76,y,x2,y), fill=PRIMARY, width=STROKE)
        pts=[(176,150),(186,170),(208,173),(191,188),(195,210),(176,198),(157,210),(161,188),(144,173),(166,170)]
        d.line(pts+[pts[0]], fill=ACCENT, width=7, joint="curve")
    elif name == "settings":
        d.ellipse((94,94,162,162), outline=PRIMARY, width=STROKE)
        rays=[((128,36),(128,60)),((128,196),(128,220)),((36,128),(60,128)),((196,128),(220,128)),((62,62),(80,80)),((176,176),(194,194)),((194,62),(176,80)),((80,176),(62,194))]
        for a,b in rays: d.line((a,b), fill=PRIMARY, width=STROKE)
        for x,y in ((128,36),(220,128),(128,220),(36,128)):
            d.polygon([(x,y-14),(x+14,y),(x,y+14),(x-14,y)], outline=ACCENT, fill=BG)
    elif name == "about":
        d.ellipse((42,42,214,214), outline=PRIMARY, width=STROKE)
        d.line((128,106,128,168), fill=ACCENT, width=STROKE)
        d.ellipse((118,70,138,90), fill=ACCENT)
    else:
        raise ValueError(name)
    return im


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", type=Path, default=Path("SKIN"))
    ap.add_argument("--check", action="store_true")
    args = ap.parse_args()
    skin = args.root / "Skin_053"
    out = skin / "assets" / "icons"
    out.mkdir(parents=True, exist_ok=True)
    errors = []
    records = []
    for name in NAMES:
        png_path = out / f"{name}.png"
        svg_path = skin / "assets" / "master" / "icons" / f"{name}.svg"
        if not svg_path.is_file():
            errors.append(str(svg_path))
            continue
        buf = io.BytesIO()
        render(name).save(buf, format="PNG", optimize=False, compress_level=9)
        expected = buf.getvalue()
        if args.check:
            if not png_path.is_file() or png_path.read_bytes() != expected:
                errors.append(str(png_path))
        else:
            png_path.write_bytes(expected)
        for kind, path, rel in (
            ("master_svg", svg_path, f"assets/master/icons/{name}.svg"),
            ("runtime_png", png_path, f"assets/icons/{name}.png"),
        ):
            if not path.is_file():
                errors.append(str(path))
                continue
            raw = path.read_bytes()
            record = {
                "id": f"topbar.{name}.{kind}", "path": rel,
                "format": path.suffix[1:].upper(), "bytes": len(raw),
                "sha256": hashlib.sha256(raw).hexdigest(),
                "crc32": f"{zlib.crc32(raw) & 0xffffffff:08X}",
                "origin": "[DECISION] original PPGU NEON STAGE line icon",
            }
            if kind == "runtime_png":
                record.update({"dimensions": "256x256", "mode": "RGBA"})
            records.append(record)
    manifest_path = skin / "assets" / "asset_manifest.json"
    manifest = {
        "schemaVersion": "1.0.0", "skinId": "skin-053",
        "theme": "NEON STAGE DEFAULT", "assetGroup": "topbar-icons",
        "creationDecision": "[DECISION] Created as original neutral line icons because no approved source icon set existed in the supplied SKIN-1 package.",
        "runtimeBehavior": "Icons do not enable reserved actions. Auto Discover, Curator, Settings and About remain unavailable until their real functions are integrated.",
        "records": records,
    }
    expected_manifest = (json.dumps(manifest, indent=2, ensure_ascii=False) + "\n").encode("utf-8")
    if args.check:
        if not manifest_path.is_file() or manifest_path.read_bytes() != expected_manifest:
            errors.append(str(manifest_path))
    else:
        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        manifest_path.write_bytes(expected_manifest)
    if errors:
        print("REV18 icon export mismatch: " + ", ".join(errors))
        return 1
    print(f"REV18 SKIN-1 top-bar icons: {'verified' if args.check else 'generated'} ({len(NAMES)} x 256x256 RGBA; manifest SHA-256/CRC32 registered)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
