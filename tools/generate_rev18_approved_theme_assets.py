#!/usr/bin/env python3
"""Generate deterministic, original REV18 premium theme asset packs.

This entry point is executed by the focused PR validation workflow.

The generator is visual-only. It reads approved theme tokens and the canonical
27-control schema; it never changes APVTS, layout geometry, DSP, MIDI or audio.
Existing different files are never overwritten.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import math
import random
import struct
import zlib
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter, ImageFont, ImageOps

STATES = ("default", "hover", "pressed", "disabled", "dragged")
ICON_NAMES = ("skins", "import_guitar", "auto_discover", "curator", "settings", "about")
FONT = ImageFont.load_default()


def rgb(value: str) -> tuple[int, int, int]:
    value = value.lstrip("#")
    return tuple(int(value[i:i + 2], 16) for i in (0, 2, 4))


def rgba(value: str, alpha: int = 255) -> tuple[int, int, int, int]:
    return (*rgb(value), alpha)


def mix(a, b, amount):
    return tuple(round(x + (y - x) * amount) for x, y in zip(a, b))


def png_bytes(image: Image.Image) -> bytes:
    from io import BytesIO
    stream = BytesIO()
    image.save(stream, format="PNG", optimize=True, compress_level=9)
    return stream.getvalue()


def safe_write(path: Path, raw: bytes, check: bool) -> None:
    if path.exists():
        if path.read_bytes() != raw:
            raise RuntimeError(f"refusing to overwrite different existing asset: {path}")
        return
    if check:
        raise RuntimeError(f"generated asset is missing: {path}")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(raw)


def add_record(records: list, rel: str, raw: bytes, category: str, dimensions=None):
    item = {
        "path": rel,
        "format": "PNG" if rel.lower().endswith(".png") else "SVG" if rel.lower().endswith(".svg") else "JSON",
        "category": category,
        "bytes": len(raw),
        "sha256": hashlib.sha256(raw).hexdigest(),
        "crc32": f"{zlib.crc32(raw) & 0xffffffff:08X}",
        "origin": "[DECISION] original deterministic PPGU REV18 artwork generated from approved theme tokens",
    }
    if dimensions:
        item["dimensions"] = f"{dimensions[0]}x{dimensions[1]}"
    records.append(item)


def write_metadata(path: Path, raw: bytes, check: bool) -> None:
    if check:
        if not path.is_file() or path.read_bytes() != raw:
            raise RuntimeError(f"generated metadata differs or is missing: {path}")
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(raw)


def gradient(size, top, bottom):
    mask = Image.linear_gradient("L").resize(size, Image.Resampling.BICUBIC)
    return ImageOps.colorize(mask, black=top, white=bottom).convert("RGBA")


def stage_background(size, palette, seed=0, guitar=False):
    w, h = size
    top, bottom = rgb(palette["window_bg"]), rgb(palette["panel"])
    im = gradient(size, top, bottom)
    overlay = Image.new("RGBA", size, (0, 0, 0, 0))
    d = ImageDraw.Draw(overlay)
    accent = rgb(palette["accent"])
    glow = rgb(palette["accent_glow"])
    d.ellipse((w * .35, -h * .58, w * 1.15, h * .72), fill=(*glow, 30))
    d.ellipse((-w * .32, h * .22, w * .48, h * 1.18), fill=(*accent, 17))
    for i in range(8):
        x = int(w * (.06 + i * .135))
        alpha = 17 if i == 3 else 8
        d.polygon([(x, 0), (x + w * .025, 0), (x - w * .20, h), (x - w * .27, h)],
                  fill=(*glow, alpha))
    overlay = overlay.filter(ImageFilter.GaussianBlur(max(8, w // 150)))
    im = Image.alpha_composite(im, overlay)
    grain = Image.effect_noise(size, 7).convert("L")
    grain_rgba = Image.new("RGBA", size, (255, 255, 255, 0))
    grain_rgba.putalpha(grain.point(lambda p: int(p * .07)))
    im = Image.alpha_composite(im, grain_rgba)
    d = ImageDraw.Draw(im)
    # Architectural stage rails: baked into the image, never animated at runtime.
    d.line((w * .04, h * .90, w * .96, h * .90), fill=(*rgb(palette["control_border"]), 80), width=max(2, w // 1000))
    d.line((w * .04, h * .925, w * .44, h * .925), fill=(*accent, 90), width=max(2, w // 1200))
    if guitar:
        im = draw_guitar(im, palette, seed)
    vignette = Image.new("L", size, 0)
    vd = ImageDraw.Draw(vignette)
    vd.ellipse((-w * .12, -h * .22, w * 1.12, h * 1.22), fill=145)
    vignette = vignette.filter(ImageFilter.GaussianBlur(max(12, w // 18)))
    shade = Image.new("RGBA", size, (0, 0, 0, 110))
    shade.putalpha(vignette.point(lambda p: 110 - p // 2))
    return Image.alpha_composite(im.convert("RGBA"), shade).convert("RGB")


def draw_guitar(base, palette, seed=0):
    w, h = base.size
    scale = min(w / 1600, h / 900)
    layer = Image.new("RGBA", base.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(layer)
    # Original electric-guitar silhouette, no brand marks or copied product artwork.
    body = [(w*.58,h*.54),(w*.64,h*.49),(w*.70,h*.50),(w*.73,h*.56),
            (w*.72,h*.62),(w*.77,h*.67),(w*.76,h*.75),(w*.70,h*.82),
            (w*.62,h*.84),(w*.55,h*.80),(w*.51,h*.73),(w*.52,h*.66),
            (w*.57,h*.62)]
    neck = [(w*.64,h*.56),(w*.70,h*.48),(w*.88,h*.15),(w*.92,h*.18),
            (w*.75,h*.56),(w*.69,h*.64)]
    shadow = [(x + w*.012, y + h*.018) for x, y in body]
    d.polygon(shadow, fill=(0,0,0,145))
    d.polygon(body, fill=(*rgb(palette["panel_elevated"]),255),
              outline=(*rgb(palette["highlight"]),235))
    d.line(body + [body[0]], fill=(*rgb(palette["control_border"]),255), width=max(3,int(5*scale)), joint="curve")
    d.polygon(neck, fill=(*rgb(palette["control_face"]),255),
              outline=(*rgb(palette["highlight"]),220))
    d.polygon([(w*.88,h*.15),(w*.92,h*.10),(w*.97,h*.13),(w*.95,h*.22),(w*.92,h*.25)],
              fill=(*rgb(palette["panel_elevated"]),255), outline=(*rgb(palette["highlight"]),235))
    # Pickguard, pickups, bridge and metallic hardware.
    d.polygon([(w*.59,h*.59),(w*.67,h*.53),(w*.72,h*.61),(w*.68,h*.72),(w*.59,h*.70)],
              fill=(*rgb(palette["lowlight"]),255), outline=(*rgb(palette["accent"]),170))
    for yy in (.62,.68):
        d.rounded_rectangle((w*.60,h*yy,w*.68,h*(yy+.025)), radius=max(2,int(5*scale)),
                            fill=(*rgb(palette["control_border"]),255),
                            outline=(*rgb(palette["highlight"]),190))
    d.rounded_rectangle((w*.57,h*.72,w*.69,h*.745), radius=max(2,int(4*scale)),
                        fill=(*rgb(palette["highlight"]),255))
    for i in range(6):
        x = w*(.645 + i*.006)
        d.line((x,h*.56,x+w*.22,h*.16), fill=(*rgb(palette["text_primary"]),170), width=max(1,int(1.5*scale)))
        # Fret bars.
    for t in (.25,.34,.43,.52,.61,.70,.79):
        x = w*(.70 + t*.18); y = h*(.48 - t*.31)
        d.line((x-w*.014,y-h*.008,x+w*.014,y+h*.008), fill=(*rgb(palette["control_border"]),230), width=max(1,int(2*scale)))
    for x,y in ((.57,.66),(.68,.66),(.62,.77)):
        r=w*.012
        d.ellipse((w*x-r,h*y-r,w*x+r,h*y+r), fill=(*rgb(palette["accent"]),255),
                  outline=(*rgb(palette["highlight"]),230), width=max(1,int(2*scale)))
    d.arc((w*.49,h*.49,w*.78,h*.88), 280, 70, fill=(*rgb(palette["accent_glow"]),150), width=max(2,int(4*scale)))
    return Image.alpha_composite(base.convert("RGBA"), layer).convert("RGB")


def material_texture(palette, seed, material=""):
    size = 128
    base = rgb(palette["lowlight"])
    accent = rgb(palette["control_border"])
    name = material.lower()
    tile = Image.new("RGB", (size, size))
    px = tile.load()
    rng = random.Random(seed)
    for y in range(size):
        for x in range(size):
            if "carbon" in name or "forged" in name:
                pattern = 9 if ((x // 8 + y // 8) % 2) else -5
                grain = 2 * math.sin(2 * math.pi * (x + y) / 16)
            elif "obsidian" in name or "glass" in name:
                pattern = 5 * math.sin(2 * math.pi * x / 32)
                grain = 3 * math.sin(2 * math.pi * (x + y) / 48)
            elif "crimson" in name or "enamel" in name:
                pattern = 7 * math.sin(2 * math.pi * y / 12)
                grain = 2 * math.sin(2 * math.pi * x / 64)
            elif "arctic" in name or "frost" in name or "platinum" in name:
                pattern = 5 * math.sin(2 * math.pi * (x + y) / 24)
                grain = 4 * math.cos(2 * math.pi * (x - y) / 40)
            elif "violet" in name or "smoked" in name:
                pattern = 5 * math.sin(2 * math.pi * x / 24)
                grain = 4 * math.cos(2 * math.pi * y / 48)
            else:
                pattern = 4 * math.sin(2 * math.pi * (x + y) / 24)
                grain = 2 * math.cos(2 * math.pi * x / 32)
            noise = rng.randint(-2, 2)
            amount = max(0.04, min(0.42, .16 + (pattern + grain + noise + 8) / 42))
            px[x, y] = tuple(max(0, min(255, round(base[i] * (1 - amount) + accent[i] * amount))) for i in range(3))
    # Force opposite borders to match exactly for a seamless repeated tile.
    for y in range(size):
        px[size - 1, y] = px[0, y]
    for x in range(size):
        px[x, size - 1] = px[x, 0]
    out = Image.new("RGB", (1024, 1024))
    for y in range(0, 1024, size):
        for x in range(0, 1024, size):
            out.paste(tile, (x, y))
    return out


def fx_layer(size, palette, kind):
    w,h=size
    im=Image.new("RGBA",size,(0,0,0,0))
    d=ImageDraw.Draw(im)
    accent=rgb(palette["accent"]); hi=rgb(palette["accent_glow"])
    if kind=="stage_glow":
        d.ellipse((w*.28,h*.02,w*.94,h*.96),fill=(*accent,40))
        d.ellipse((w*.43,h*.12,w*.82,h*.82),fill=(*hi,24))
        im=im.filter(ImageFilter.GaussianBlur(max(12,w//24)))
    elif kind=="edge_highlight":
        d.line((w*.02,h*.035,w*.98,h*.035),fill=(*hi,115),width=max(2,w//420))
        d.line((w*.02,h*.965,w*.98,h*.965),fill=(*accent,75),width=max(2,w//600))
        im=im.filter(ImageFilter.GaussianBlur(max(1,w//1200)))
    elif kind=="specular":
        d.polygon([(w*.16,0),(w*.23,0),(w*.56,h),(w*.49,h)],fill=(*hi,18))
        d.polygon([(w*.70,0),(w*.73,0),(w*.94,h),(w*.89,h)],fill=(*hi,13))
        im=im.filter(ImageFilter.GaussianBlur(max(4,w//140)))
    else:
        d.ellipse((w*.02,h*.02,w*.98,h*.98),fill=(0,0,0,105))
        im=im.filter(ImageFilter.GaussianBlur(max(10,w//20)))
    return im


def control_frame(control_id, state, palette):
    size=(512,512)
    bg=rgb(palette["window_bg"]); panel=rgb(palette["panel"])
    raised=rgb(palette["panel_elevated"]); border=rgb(palette["control_border"])
    primary=rgb(palette["text_primary"]); secondary=rgb(palette["text_secondary"])
    accent=rgb(palette["accent"])
    if state=="hover": accent=rgb(palette["accent_glow"])
    elif state=="pressed": accent=mix(accent,bg,.30)
    elif state=="disabled": accent=rgb(palette["text_disabled"]); primary=rgb(palette["text_disabled"])
    elif state=="dragged": accent=rgb(palette["warning"])
    im=gradient(size,bg,panel)
    d=ImageDraw.Draw(im)
    d.rounded_rectangle((15,15,497,497),radius=46,fill=panel,outline=border,width=5)
    d.rounded_rectangle((27,27,485,485),radius=36,outline=mix(border,accent,.30),width=2)
    cx=cy=256
    if control_id.startswith("knob"):
        d.ellipse((78,66,434,422),fill=bg,outline=border,width=13)
        d.arc((94,82,418,406),135,405,fill=mix(border,secondary,.35),width=18)
        d.arc((94,82,418,406),135,310 if state!="disabled" else 210,fill=accent,width=18)
        d.ellipse((126,114,386,374),fill=raised,outline=border,width=7)
        d.ellipse((148,136,364,352),fill=panel,outline=mix(border,accent,.25),width=4)
        d.ellipse((174,162,338,326),fill=raised,outline=border,width=3)
        d.arc((182,170,330,318),205,330,fill=primary,width=4)
        for angle in range(0,360,24):
            a=math.radians(angle)
            x1=cx+int(194*math.cos(a)); y1=cy+int(194*math.sin(a))
            x2=cx+int(210*math.cos(a)); y2=cy+int(210*math.sin(a))
            d.line((x1,y1,x2,y2),fill=secondary,width=3)
        d.line((cx,164,cx+8,220),fill=accent,width=12)
        d.ellipse((cx-13,cy-13,cx+13,cy+13),fill=accent,outline=primary,width=3)
    elif control_id.startswith("fx_switch"):
        d.rounded_rectangle((54,168,458,344),radius=88,fill=bg,outline=border,width=8)
        d.rounded_rectangle((75,188,437,324),radius=68,fill=mix(bg,accent,.35 if state!="disabled" else .08))
        thumb_x=348 if state!="pressed" else 164
        d.ellipse((thumb_x-65,194,thumb_x+65,324),fill=raised,outline=primary,width=5)
        d.ellipse((thumb_x-38,218,thumb_x+38,294),fill=panel,outline=border,width=3)
        d.ellipse((thumb_x-12,244,thumb_x+12,268),fill=accent)
    elif control_id=="preset_browser":
        d.rounded_rectangle((48,76,464,404),radius=24,fill=bg,outline=border,width=6)
        d.rounded_rectangle((62,90,450,148),radius=16,fill=raised,outline=border,width=2)
        for row in range(4):
            y=164+row*54
            selected=(row==1 and state!="disabled")
            fill=mix(raised,accent,.20) if selected else panel
            d.rounded_rectangle((68,y,444,y+42),radius=10,fill=fill,
                                outline=accent if selected else border,width=3)
            d.ellipse((84,y+13,100,y+29),fill=accent if selected else secondary)
            d.line((116,y+21,390,y+21),fill=primary if selected else secondary,width=5)
    elif control_id=="mode_selector":
        d.rounded_rectangle((44,176,468,336),radius=34,fill=bg,outline=border,width=7)
        labels=("MELODY","PATTERN","CHORDS")
        active=1 if state=="pressed" else 0
        for i,label in enumerate(labels):
            x=54+i*136
            fill=accent if i==active and state!="disabled" else raised
            d.rounded_rectangle((x,188,x+126,324),radius=25,fill=fill,
                                outline=primary if i==active else border,width=3)
            tw=d.textbbox((0,0),label,font=FONT)[2]
            d.text((x+(126-tw)//2,247),label,font=FONT,
                   fill=bg if i==active and state!="disabled" else primary)
    elif control_id=="midi_keyboard":
        d.rounded_rectangle((38,172,474,356),radius=18,fill=bg,outline=border,width=7)
        key_w=49
        for i in range(8):
            x=48+i*key_w
            d.rounded_rectangle((x,190,x+key_w-3,340),radius=5,fill=primary,outline=border,width=2)
        for i in (0,1,3,4,5):
            x=48+i*key_w+29
            d.rounded_rectangle((x,190,x+25,284),radius=4,fill=bg,outline=border,width=2)
        d.rounded_rectangle((48+3*key_w,190,48+4*key_w-3,340),radius=5,
                            outline=accent,width=5)
    elif control_id=="LCD_display":
        d.rounded_rectangle((42,142,470,370),radius=24,fill=bg,outline=border,width=8)
        d.rounded_rectangle((62,162,450,350),radius=14,fill=rgb(palette["lcd_bg"]),
                            outline=mix(border,accent,.25),width=2)
        d.text((88,188),"PATCH / STATUS",font=FONT,fill=rgb(palette["lcd_text"]))
        for i in range(22):
            x=92+i*15
            height=18+((i*23)%66)
            d.rounded_rectangle((x,300-height//2,x+5,300+height//2),radius=2,fill=accent)
    elif control_id=="LED_indicator":
        d.ellipse((90,90,422,422),fill=bg,outline=border,width=10)
        d.ellipse((136,136,376,376),fill=raised,outline=mix(border,accent,.35),width=6)
        d.ellipse((176,176,336,336),fill=panel,outline=border,width=4)
        d.ellipse((206,196,282,270),fill=(*accent,150),outline=primary,width=3)
        d.arc((154,154,358,358),205,320,fill=secondary,width=6)
    else:
        d.rounded_rectangle((54,94,458,398),radius=42,fill=raised,outline=accent,width=7)
        d.rounded_rectangle((70,110,442,382),radius=32,outline=mix(border,accent,.4),width=2)
        if control_id in ("SKIN_button","Settings"):
            d.ellipse((190,164,322,296),outline=primary,width=13)
            d.ellipse((232,206,280,254),fill=accent)
            for angle in range(0,360,45):
                a=math.radians(angle)
                d.line((cx+int(74*math.cos(a)),cy+int(74*math.sin(a)),
                        cx+int(108*math.cos(a)),cy+int(108*math.sin(a))),fill=primary,width=10)
        elif control_id=="Import_Guitar":
            d.ellipse((138,210,244,318),outline=primary,width=12)
            d.line((218,238,344,112),fill=primary,width=16)
            d.polygon([(330,102),(370,62),(414,106),(374,148)],outline=accent)
            d.line((286,326,390,326),fill=accent,width=12)
            d.line((338,274,338,378),fill=accent,width=12)
        elif control_id=="Auto_Discover":
            d.ellipse((136,126,322,312),outline=primary,width=13)
            d.line((294,286,390,382),fill=accent,width=18)
            d.line((228,158,228,280),fill=secondary,width=5)
            d.line((166,220,290,220),fill=secondary,width=5)
        elif control_id=="Curator":
            d.rounded_rectangle((144,130,368,350),radius=22,outline=primary,width=10)
            for y in (188,238,288): d.line((184,y,328,y),fill=secondary,width=7)
            d.polygon([(348,280),(364,310),(398,316),(372,340),(378,374),(348,356),(318,374),(324,340),(298,316),(332,310)],fill=accent)
        elif control_id=="About":
            d.ellipse((146,126,366,346),outline=primary,width=12)
            d.line((256,206,256,300),fill=accent,width=16)
            d.ellipse((244,158,268,182),fill=accent)
        else:
            d.rounded_rectangle((152,164,360,328),radius=20,outline=primary,width=9)
            d.line((184,214,328,214),fill=secondary,width=7)
            d.line((184,260,300,260),fill=accent,width=7)
    label=control_id.replace("_"," ").upper()[:28]
    d.text((42,432),label,font=FONT,fill=primary)
    d.text((42,462),state.upper(),font=FONT,fill=accent)
    return im


def icon_image(name, palette):
    im=Image.new("RGBA",(256,256),(0,0,0,0))
    d=ImageDraw.Draw(im)
    p=rgba(palette["text_primary"]); a=rgba(palette["accent"])
    sw=12
    if name=="skins":
        d.rounded_rectangle((36,36,220,220),radius=28,outline=p,width=sw)
        for x in (98,158): d.line((x,36,x,220),fill=p,width=sw)
        for y in (98,158): d.line((36,y,220,y),fill=p,width=sw)
        d.ellipse((58,58,82,82),fill=a); d.ellipse((116,116,140,140),fill=a); d.ellipse((174,174,198,198),fill=a)
    elif name=="import_guitar":
        d.ellipse((30,112,118,214),outline=p,width=sw)
        d.ellipse((70,94,146,176),outline=p,width=sw)
        d.line((116,130,184,60),fill=p,width=sw)
        d.polygon([(172,44),(194,22),(232,60),(210,82)],outline=p)
        d.line((160,174,226,174),fill=a,width=sw); d.line((193,141,193,207),fill=a,width=sw)
    elif name=="auto_discover":
        d.ellipse((32,32,184,184),outline=p,width=sw)
        d.line((108,32,108,184),fill=p,width=sw); d.line((32,108,184,108),fill=p,width=sw)
        d.line((54,54,162,162),fill=p,width=sw); d.line((162,54,54,162),fill=p,width=sw)
        d.line((166,166,228,228),fill=p,width=sw)
        d.polygon([(108,76),(130,108),(108,140),(86,108)],fill=a)
    elif name=="curator":
        d.rounded_rectangle((34,34,222,222),radius=24,outline=p,width=sw)
        for y,x in ((86,188),(128,188),(170,146)): d.line((72,y,x,y),fill=p,width=sw)
        d.polygon([(182,142),(194,164),(220,168),(200,186),(205,212),(182,198),(160,212),(164,186),(144,168),(170,164)],outline=a)
    elif name=="settings":
        d.ellipse((88,88,168,168),outline=p,width=sw)
        for x1,y1,x2,y2 in ((128,24,128,68),(128,188,128,232),(24,128,68,128),(188,128,232,128),(52,52,82,82),(174,174,204,204),(204,52,174,82),(82,174,52,204)):
            d.line((x1,y1,x2,y2),fill=p,width=sw)
        d.ellipse((112,112,144,144),fill=a)
    else:
        d.ellipse((34,34,222,222),outline=p,width=sw)
        d.line((128,104,128,174),fill=a,width=sw)
        d.ellipse((116,62,140,86),fill=a)
    return im


def icon_svg(name, palette):
    p=palette["text_primary"]; a=palette["accent"]
    # The SVG master is original, self-contained and uses only canonical palette tokens.
    shapes={
      "skins":'<rect x="34" y="34" width="188" height="188" rx="24"/><path d="M96 34v188M158 34v188M34 96h188M34 158h188"/><circle cx="64" cy="64" r="9" fill="'+a+'" stroke="none"/><circle cx="128" cy="128" r="9" fill="'+a+'" stroke="none"/><circle cx="192" cy="192" r="9" fill="'+a+'" stroke="none"/>',
      "import_guitar":'<path d="M50 145c-14 22-8 68 20 78 26 9 53-12 53-40 0-18-11-32-28-39l77-77 30 30-77 77"/><path d="M164 174h58m-29-29v58" stroke="'+a+'"/>',
      "auto_discover":'<circle cx="108" cy="108" r="76"/><path d="M108 32v152M32 108h152M54 54l108 108M162 54L54 162"/><path d="M108 76l24 32-24 32-24-32z" fill="'+a+'"/>',
      "curator":'<rect x="34" y="34" width="188" height="188" rx="24"/><path d="M70 84h118M70 128h118M70 172h70"/><path d="M182 142l12 22 26 4-20 18 5 26-23-14-22 14 4-26-20-18 26-4z" stroke="'+a+'"/>',
      "settings":'<circle cx="128" cy="128" r="42"/><path d="M128 24v48m0 112v48M24 128h48m112 0h48M54 54l34 34m80 80 34 34m0-148-34 34M88 168l-34 34"/>',
      "about":'<circle cx="128" cy="128" r="92"/><path d="M128 104v72" stroke="'+a+'"/><circle cx="128" cy="76" r="12" fill="'+a+'"/>'
    }
    return (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 256 256" width="256" height="256">'
            f'<g fill="none" stroke="{p}" stroke-width="12" stroke-linecap="round" stroke-linejoin="round">{shapes[name]}</g></svg>').encode()


def png_dimensions(raw):
    if raw[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError("invalid PNG signature")
    pos=8
    while pos+12<=len(raw):
        n=struct.unpack(">I",raw[pos:pos+4])[0]
        kind=raw[pos+4:pos+8]; data=raw[pos+8:pos+8+n]
        crc=struct.unpack(">I",raw[pos+8+n:pos+12+n])[0]
        if (zlib.crc32(kind+data)&0xffffffff)!=crc:
            raise ValueError(f"PNG internal CRC mismatch in {kind!r}")
        if kind==b"IHDR":
            return struct.unpack(">II",data[:8])
        pos += n+12
    raise ValueError("PNG IHDR missing")


def generate_theme(root: Path, item: dict, controls: list[str], check: bool):
    theme_path=root/item["path"]
    folder=theme_path.parent
    theme=json.loads(theme_path.read_text(encoding="utf-8"))
    palette=theme["palette"]
    stable_id=item["stableId"]
    if len(theme.get("controls",{})) != 27 or set(theme["controls"]) != set(controls):
        raise RuntimeError(f"{item['label']}: theme control map does not match canonical 27 IDs")
    records=[]; artwork=[]; control_records=[]
    generated={}

    def add(rel, image, category):
        raw=png_bytes(image)
        generated[rel]=raw
        add_record(records,rel,raw,category,image.size)

    def add_raw(rel, raw, category, dimensions):
        generated[rel]=raw
        add_record(records,rel,raw,category,dimensions)

    # SKIN-1 runtime artwork is an already-approved binary. Retain it byte-for-byte;
    # only create the missing master-resolution copies from that approved source.
    if stable_id == "skin-053":
        from io import BytesIO
        hero_path=folder/"assets/hero.png"
        preview_path=folder/"assets/preview.png"
        if not hero_path.is_file() or not preview_path.is_file():
            raise RuntimeError("approved SKIN-1 hero.png/preview.png source binaries are missing")
        hero_raw=hero_path.read_bytes()
        preview_raw=preview_path.read_bytes()
        hero=Image.open(BytesIO(hero_raw)).convert("RGB")
        preview=Image.open(BytesIO(preview_raw)).convert("RGB")
        if hero.size != (800,450) or preview.size != (360,240):
            raise RuntimeError(f"approved SKIN-1 source dimensions are unexpected: hero={hero.size}, preview={preview.size}")
        add_raw("assets/hero.png",hero_raw,"preserved-approved-hero-runtime",(800,450))
        records[-1]["origin"]="[PRESERVED] Existing approved SKIN-1 hero.png retained byte-for-byte."
        add_raw("assets/preview.png",preview_raw,"preserved-approved-preview-runtime",(360,240))
        records[-1]["origin"]="[PRESERVED] Existing approved SKIN-1 preview.png retained byte-for-byte."
        add_raw("assets/hero/hero.png",hero_raw,"hero-runtime-copy",(800,450))
        add_raw("assets/preview/preview.png",preview_raw,"preview-runtime-copy",(360,240))
        hero_master=hero.resize((1600,900),Image.Resampling.LANCZOS)
        preview_master=preview.resize((720,480),Image.Resampling.LANCZOS)
        add("assets/master/hero/hero.png",hero_master,"hero-master-upscaled-from-approved-runtime")
        add("assets/master/preview/preview.png",preview_master,"preview-master-upscaled-from-approved-runtime")
        artwork.extend([
            {"path":"assets/hero.png","dimensions":"800x450","category":"preserved-approved-hero-runtime"},
            {"path":"assets/preview.png","dimensions":"360x240","category":"preserved-approved-preview-runtime"},
            {"path":"assets/master/hero/hero.png","dimensions":"1600x900","category":"hero-master-upscaled-from-approved-runtime"},
            {"path":"assets/master/preview/preview.png","dimensions":"720x480","category":"preview-master-upscaled-from-approved-runtime"},
        ])
    else:
        hero=stage_background((1600,900),palette,item["index"],guitar=True)
        preview=ImageOps.fit(hero,(720,480),method=Image.Resampling.LANCZOS)
        hero_runtime=hero.resize((800,450),Image.Resampling.LANCZOS)
        preview_runtime=preview.resize((360,240),Image.Resampling.LANCZOS)
        for rel,img,cat in [
            ("assets/master/hero/hero.png",hero,"hero-master"),
            ("assets/hero/hero.png",hero_runtime,"hero-runtime"),
            ("assets/hero.png",hero_runtime,"hero-runtime-compat"),
            ("assets/master/preview/preview.png",preview,"preview-master"),
            ("assets/preview/preview.png",preview_runtime,"preview-runtime"),
            ("assets/preview.png",preview_runtime,"preview-runtime-compat"),
        ]:
            add(rel,img,cat); artwork.append({"path":rel,"dimensions":f"{img.width}x{img.height}","category":cat})

    bg=stage_background((2560,1440),palette,item["index"],guitar=False)
    texture=material_texture(palette,item["index"],theme.get("effects",{}).get("material",""))
    for rel,img,cat in [
        ("assets/master/background/main.png",bg,"background-master"),
        ("assets/background/main.png",bg,"background-runtime"),
        ("assets/master/textures/material.png",texture,"texture-master"),
        ("assets/textures/material.png",texture,"texture-runtime"),
    ]:
        add(rel,img,cat); artwork.append({"path":rel,"dimensions":f"{img.width}x{img.height}","category":cat})

    for kind in ("stage_glow","edge_highlight","specular","shadow"):
        master=fx_layer((1600,900),palette,kind)
        runtime=master.resize((800,450),Image.Resampling.LANCZOS)
        for rel,img,cat in [
            (f"assets/master/fx_layers/{kind}.png",master,f"{kind}-master"),
            (f"assets/fx_layers/{kind}.png",runtime,f"{kind}-runtime"),
        ]:
            add(rel,img,cat); artwork.append({"path":rel,"dimensions":f"{img.width}x{img.height}","category":cat})

    # Five states for every canonical control; master 512px, runtime 256px.
    atlas_icons=[]
    for control_id in controls:
        for state in STATES:
            master=control_frame(control_id,state,palette)
            rel=f"assets/master/controls/{control_id}_{state}.png"
            add(rel,master,"control-state-master")
            runtime=master.resize((256,256),Image.Resampling.LANCZOS)
            runtime_rel=f"assets/controls/{control_id}_{state}.png"
            add(runtime_rel,runtime,"control-state-runtime")
            control_records.append({
                "controlId":control_id,"state":state,
                "master":rel,"runtime":runtime_rel,
                "masterSize":"512x512","runtimeSize":"256x256"
            })

    # Original top-bar icon masters and raster exports.
    icon_frames={}
    for i,name in enumerate(ICON_NAMES):
        svg_rel=f"assets/master/icons/{name}.svg"
        svg=icon_svg(name,palette)
        generated[svg_rel]=svg
        add_record(records,svg_rel,svg,"topbar-icon-master")
        icon=icon_image(name,palette)
        png_rel=f"assets/icons/{name}.png"
        add(png_rel,icon,"topbar-icon-runtime")
        x=(i%3)*256; y=(i//3)*256
        icon_frames[name]={"x":x,"y":y,"w":256,"h":256}
        atlas_icons.append((name,icon))

    atlas=Image.new("RGBA",(768,512),(0,0,0,0))
    for i,(_,icon) in enumerate(atlas_icons):
        atlas.alpha_composite(icon,((i%3)*256,(i//3)*256))
    add("assets/atlas/topbar_icons.png",atlas,"topbar-icon-atlas")
    atlas_json={
        "schemaVersion":"1.0.0","skinId":stable_id,"image":"assets/atlas/topbar_icons.png",
        "width":768,"height":512,"frames":icon_frames
    }
    atlas_raw=(json.dumps(atlas_json,indent=2,ensure_ascii=False)+"\n").encode()
    generated["assets/atlas/topbar_icons.json"]=atlas_raw
    add_record(records,"assets/atlas/topbar_icons.json",atlas_raw,"topbar-icon-atlas-map")

    font_manifest={
        "schemaVersion":"1.0.0","skinId":stable_id,
        "fonts":[
            {"family":"Segoe UI","role":"ui","source":"Windows 10 system font","bundled":False,"redistribution":"not redistributed"},
            {"family":"Consolas","role":"display","source":"Windows 10 system font","bundled":False,"redistribution":"not redistributed"}
        ],
        "licenseNote":"No third-party font binary is bundled. Runtime resolves the named Windows system fonts."
    }
    font_raw=(json.dumps(font_manifest,indent=2,ensure_ascii=False)+"\n").encode()
    generated["fonts/font_manifest.json"]=font_raw
    add_record(records,"fonts/font_manifest.json",font_raw,"font-license-manifest")

    artwork_raw=(json.dumps({"schemaVersion":"1.0.0","skinId":stable_id,"theme":item["name"],
        "origin":"[DECISION] Original artwork generated from the approved palette/material brief; no third-party logos or product artwork.",
        "records":artwork},indent=2,ensure_ascii=False)+"\n").encode()
    generated["assets/artwork_manifest.json"]=artwork_raw
    add_record(records,"assets/artwork_manifest.json",artwork_raw,"artwork-manifest")

    control_raw=(json.dumps({"schemaVersion":"1.0.0","skinId":stable_id,"states":list(STATES),
        "controlIds":controls,"frameSize":"512x512","runtimeFrameSize":"256x256",
        "records":control_records},indent=2,ensure_ascii=False)+"\n").encode()
    generated["assets/controls/control_states_manifest.json"]=control_raw
    add_record(records,"assets/controls/control_states_manifest.json",control_raw,"control-state-manifest")

    # Include the three approved vector material masters in the checksum set.
    for rel in ("svg/texture.svg", "svg/bezel.svg", "svg/crest.svg"):
        source = folder / rel
        if not source.is_file():
            raise RuntimeError(f"approved vector source is missing: {source}")
        raw = source.read_bytes()
        add_record(records, rel, raw, "material-vector-source")
    # Combined manifest excludes itself to avoid a recursive checksum.
    asset_raw=(json.dumps({"schemaVersion":"1.0.0","skinId":stable_id,"theme":item["name"],
        "assetCount":len(records),"records":records},indent=2,ensure_ascii=False)+"\n").encode()
    generated["assets/asset_manifest.json"]=asset_raw

    # Refuse to replace any existing non-identical asset.
    for rel,raw in generated.items():
        safe_write(folder/rel,raw,check)

    # Keep top-level references compatible with the existing theme runtime contract.
    theme.setdefault("assets",{})
    theme["assets"].update({
        "preview":"assets/preview.png","hero":"assets/hero.png",
        "texture":"svg/texture.svg","bezel":"svg/bezel.svg","crest":"svg/crest.svg",
        "background":"assets/background/main.png",
        "textureRaster":"assets/textures/material.png",
        "iconAtlas":"assets/atlas/topbar_icons.png",
        "assetManifest":"assets/asset_manifest.json",
        "artworkManifest":"assets/artwork_manifest.json",
        "controlManifest":"assets/controls/control_states_manifest.json",
        "fontManifest":"fonts/font_manifest.json",
        "fxStageGlow":"assets/fx_layers/stage_glow.png",
        "fxEdgeHighlight":"assets/fx_layers/edge_highlight.png",
        "fxSpecular":"assets/fx_layers/specular.png",
        "fxShadow":"assets/fx_layers/shadow.png"
    })
    theme["assetPackStatus"]="GENERATED_G1_G4_RUNTIME_VALIDATION_PENDING"
    notes=theme.setdefault("notes",[])
    note="[DECISION] Original premium artwork/control-state pack generated from approved palette and material brief; runtime and G1-G4 validation remain separate gates."
    if note not in notes: notes.append(note)
    theme_raw=(json.dumps(theme,indent=2,ensure_ascii=False)+"\n").encode()
    write_metadata(theme_path,theme_raw,check)
    print(f"{item['label']} {stable_id}: generated {len(generated)} pack files")


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root",type=Path,default=Path("SKIN"))
    ap.add_argument("--check",action="store_true")
    args=ap.parse_args()
    sequence_path=args.root/"approved_theme_sequence.json"
    schema_path=args.root/"control_schema.json"
    sequence=json.loads(sequence_path.read_text(encoding="utf-8"))
    schema=json.loads(schema_path.read_text(encoding="utf-8"))
    controls=list(schema["x-ppgu-contract"]["requiredControlIds"])
    if len(controls)!=27 or len(set(controls))!=27:
        raise SystemExit("canonical control schema must contain 27 unique IDs")
    if len(sequence.get("themes",[]))!=6:
        raise SystemExit("approved sequence must contain the six currently named themes")
    for item in sequence["themes"]:
        generate_theme(args.root,item,controls,args.check)
    sequence["status"]="APPROVED_SIX_THEME_FULL_ASSET_PACK_GENERATED_G1_G4_RUNTIME_VALIDATION_PENDING"
    for item in sequence["themes"]:
        item["assetPackStatus"]="GENERATED_G1_G4_RUNTIME_VALIDATION_PENDING"
    sequence["pending"]=[
        "G1-G4 validation and PNG CRC checks.",
        "Canonical registry/gallery registration after asset validation.",
        "Runtime asset loading and exact-SHA Windows VST3 build, CTest, pluginval and FL Studio 20 host acceptance."
    ]
    sequence_raw=(json.dumps(sequence,indent=2,ensure_ascii=False)+"\n").encode()
    write_metadata(sequence_path,sequence_raw,args.check)
    print("REV18 approved theme asset generation complete; registry activation is intentionally a separate validated step.")


if __name__=="__main__":
    main()
