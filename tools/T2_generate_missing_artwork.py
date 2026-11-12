#!/usr/bin/env python3
"""Create missing original artwork for legacy PPGU skins (1..51)."""
from __future__ import annotations
import argparse, json, re, sys
from pathlib import Path
try:
    from PIL import Image, ImageDraw, ImageFont, ImageFilter
except ImportError as exc:
    raise SystemExit("Pillow required: python -m pip install pillow") from exc

HEX = re.compile(r"^#[0-9A-Fa-f]{6}$")

def pick(obj, keys, fallback):
    if isinstance(obj, dict):
        for key in keys:
            value = obj.get(key)
            if isinstance(value, str) and HEX.fullmatch(value):
                return value
        for value in obj.values():
            found = pick(value, keys, None)
            if found:
                return found
    elif isinstance(obj, list):
        for value in obj:
            found = pick(value, keys, None)
            if found:
                return found
    return fallback

def rgb(value):
    return tuple(int(value[i:i+2], 16) for i in (1, 3, 5))

def mix(a, b, t):
    return tuple(round(x + (y-x)*t) for x,y in zip(a,b))

def render(size, title, index, accent_hex, secondary_hex, bg_hex, mood):
    """Render a consistent skin artwork card using Pillow's native image ops.

    Build-time artwork generation runs for all legacy skins. A one-column
    gradient + blurred accent mask avoids the previous Python per-pixel loop,
    keeping Windows CI responsive while preserving the original composition.
    """
    w, h = size
    accent, secondary, bg = map(rgb, (accent_hex, secondary_hex, bg_hex))
    top = mix(bg, secondary, .12)
    bottom = mix(bg, (4, 7, 12), .35)

    gradient = Image.new("RGB", (1, h))
    gradient.putdata([
        mix(top, bottom, y / max(1, h - 1))
        for y in range(h)
    ])
    resampling = getattr(Image, "Resampling", Image)
    image = gradient.resize((w, h), resampling.BILINEAR)

    # Approximate the subtle off-centre accent bloom with a blurred mask.
    glow_mask = Image.new("L", (w, h), 0)
    glow_draw = ImageDraw.Draw(glow_mask)
    cx, cy = int(w * .76), int(h * .48)
    rx, ry = int(w * .90), int(h * .90)
    glow_draw.ellipse((cx - rx, cy - ry, cx + rx, cy + ry), fill=28)
    glow_mask = glow_mask.filter(ImageFilter.GaussianBlur(radius=max(2, w // 18)))
    accent_layer = Image.new("RGB", (w, h), accent)
    image = Image.composite(accent_layer, image, glow_mask)

    d = ImageDraw.Draw(image, "RGBA")
    font = ImageFont.load_default()
    for x in range(0, w, max(24, w // 24)):
        d.line((x, 0, x, h), fill=(180, 220, 240, 12), width=1)
    for y in range(0, h, max(24, h // 12)):
        d.line((0, y, w, y), fill=(180, 220, 240, 10), width=1)

    d.rounded_rectangle((10, 10, w - 11, h - 11), radius=max(7, w // 70),
                        outline=(*accent, 180), width=max(2, w // 500))
    cx, cy = int(w * .76), int(h * .55)
    scale = min(w / 1200, h / 420)
    bw, bh = int(145 * scale), int(190 * scale)
    nw, nh = int(34 * scale), int(245 * scale)
    d.ellipse((cx - bw, cy - bh // 2, cx + bw, cy + bh // 2),
              fill=(17, 22, 30, 255), outline=(*accent, 255),
              width=max(2, int(4 * scale)))
    d.ellipse((cx - int(bw * .62), cy - int(bh * .32),
               cx + int(bw * .62), cy + int(bh * .32)),
              fill=(*secondary, 100), outline=(230, 240, 250, 180),
              width=max(1, int(2 * scale)))
    d.polygon([(cx - nw // 2, cy - bh // 3), (cx + nw // 2, cy - bh // 3),
               (cx + nw // 2, cy - bh // 3 - nh),
               (cx - nw // 2, cy - bh // 3 - nh)],
              fill=(105, 116, 128, 255), outline=(230, 238, 245, 230))
    for fret in range(1, 9):
        fy = cy - bh // 3 - int(nh * fret / 9)
        d.line((cx - nw // 2, fy, cx + nw // 2, fy),
               fill=(20, 25, 31, 230), width=1)
    for string in range(-2, 3):
        sx = cx + string * max(1, int(2 * scale))
        d.line((sx, cy - bh // 3 - nh, sx, cy + bh // 3),
               fill=(245, 248, 250, 185), width=1)
    for py in (cy - int(24 * scale), cy + int(24 * scale)):
        d.rounded_rectangle((cx - int(42 * scale), py - max(2, int(5 * scale)),
                             cx + int(42 * scale), py + max(2, int(5 * scale))),
                            radius=3, fill=(215, 225, 235, 240))

    left, ty = int(w * .055), int(h * .31)
    d.text((left, ty), "POP PUNK GUITAR", font=font, fill=(240, 246, 250, 255))
    d.text((left, ty + max(18, int(h * .09))), "ULTIMATE",
           font=font, fill=(*accent, 255))
    d.text((left, ty + max(40, int(h * .21))), title[:38],
           font=font, fill=(205, 218, 228, 250))
    d.text((left, h - max(28, int(h * .12))),
           f"SKIN {index:02d} / {mood[:28]}", font=font,
           fill=(*secondary, 240))
    return image

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root",type=Path,default=Path("SKIN"))
    parser.add_argument("--force",action="store_true")
    parser.add_argument("--check",action="store_true")
    args=parser.parse_args()
    try:
        registry=json.loads((args.root/"theme_manifest.json").read_text(encoding="utf-8"))
        entries=registry["themes"]
        if not isinstance(entries,list) or registry.get("totalSkins")!=len(entries):
            raise ValueError("registry totalSkins does not match themes array")
        made=verified=0
        errors=[]
        for entry in entries:
            index=entry.get("index")
            if not isinstance(index,int) or isinstance(index,bool) or index>=52:
                continue
            folder=args.root/f"Skin_{index:02d}"
            manifest=folder/"theme.json"
            if not manifest.is_file():
                errors.append(f"missing {manifest}"); continue
            theme=json.loads(manifest.read_text(encoding="utf-8"))
            if theme.get("id")!=entry.get("id") or theme.get("index")!=index:
                errors.append(f"{manifest}: registry identity mismatch"); continue
            tokens=theme.get("tokens",{})
            accent=pick(tokens,("primary","accent","focus","value"),"#2DD4FF")
            secondary=pick(tokens,("secondary","indicator","glow"),"#FF2D9E")
            background=pick(tokens,("base","background"),"#10141B")
            mood=str(theme.get("identity",{}).get("mood","DREAMING IN NEON"))
            targets=((folder/"assets"/"preview.png",(480,300),"PNG"),
                     (folder/"assets"/"hero.webp",(1200,420),"WEBP"))
            for target,size,fmt in targets:
                if target.exists() and not args.force:
                    try:
                        with Image.open(target) as im:
                            im.verify()
                        verified+=1
                    except Exception as exc:
                        errors.append(f"{target}: corrupt existing artwork: {exc}")
                    continue
                if args.check:
                    errors.append(f"missing artwork: {target}"); continue
                target.parent.mkdir(parents=True,exist_ok=True)
                art=render(size,str(theme.get("name",f"Skin {index:02d}")),index,accent,secondary,background,mood)
                art.save(target,format=fmt,quality=91,method=6,optimize=True)
                with Image.open(target) as im:
                    im.verify()
                made+=1
        if errors:
            raise ValueError("\n".join(errors))
        print(f"PASS: generated {made} artwork file(s); verified {verified} existing file(s)")
        return 0
    except Exception as exc:
        print(f"FAIL: {exc}",file=sys.stderr)
        return 1

if __name__=="__main__":
    raise SystemExit(main())
