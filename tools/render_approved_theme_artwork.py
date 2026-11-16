#!/usr/bin/env python3
"""Render original gallery artwork for approved PPGU factory themes SKIN-2 through SKIN-5."""
from __future__ import annotations
import argparse, json, re, sys
from pathlib import Path
try:
    from PIL import Image, ImageDraw, ImageFont, ImageFilter
except ImportError as exc:
    raise SystemExit("Pillow required: python -m pip install pillow") from exc

THEMES = {
    "SKIN-2": {"stable_id": "skin-103", "index": 103, "folder": "Skin_103",
               "title": "OBSIDIAN GOLD", "tagline": "BLACK GLASS  •  CHAMPAGNE METAL"},
    "SKIN-3": {"stable_id": "skin-104", "index": 104, "folder": "Skin_104",
               "title": "CARBON FORGE", "tagline": "FORGED CARBON  •  SATIN TITANIUM"},
    "SKIN-4": {"stable_id": "skin-105", "index": 105, "folder": "Skin_105",
               "title": "MIDNIGHT CRIMSON", "tagline": "MIDNIGHT ENAMEL  •  RUBY METAL"},
    "SKIN-5": { "stable_id": "skin-106", "index": 106, "folder": "Skin_106",
               "title": "ARCTIC PLATINUM", "tagline": "FROSTED ARCTIC GLASS  •  SATIN PLATINUM" },
}

def rgb(value):
    value=value.lstrip("#")
    if len(value)!=6: raise ValueError(f"Invalid RGB token: {value!r}")
    return tuple(int(value[i:i+2],16) for i in (0,2,4))

def mix(a,b,t):
    return tuple(round(x+(y-x)*t) for x,y in zip(a,b))

def get_font(size,bold=False):
    candidates=([r"C:\Windows\Fonts\seguisb.ttf",r"C:\Windows\Fonts\arialbd.ttf"] if bold
                else [r"C:\Windows\Fonts\segoeui.ttf",r"C:\Windows\Fonts\arial.ttf"])
    for path in candidates:
        try: return ImageFont.truetype(path,size)
        except OSError: pass
    return ImageFont.load_default()

def guitar(d,w,h,accent,metal,carbon=False):
    cx,cy=int(w*.755),int(h*.53); s=min(w/800,h/450)
    bw,bh=int(118*s),int(155*s); nw=max(7,int(25*s))
    body=[(cx-int(bw*.48),cy-int(bh*.12)),(cx-int(bw*.34),cy-int(bh*.48)),
          (cx-int(bw*.05),cy-int(bh*.53)),(cx+int(bw*.18),cy-int(bh*.31)),
          (cx+int(bw*.42),cy-int(bh*.45)),(cx+int(bw*.50),cy-int(bh*.10)),
          (cx+int(bw*.37),cy+int(bh*.38)),(cx+int(bw*.08),cy+int(bh*.52)),
          (cx-int(bw*.28),cy+int(bh*.42)),(cx-int(bw*.50),cy+int(bh*.10))]
    body_fill=(13,20,24) if carbon else (18,16,20)
    d.polygon(body,fill=body_fill,outline=accent)
    d.ellipse((cx-int(bw*.28),cy-int(bh*.25),cx+int(bw*.28),cy+int(bh*.25)),outline=metal,width=max(1,int(2*s)))
    nx,ny=cx+int(bw*.1),cy-int(bh*.28); tx,ty=cx+int(160*s),cy-int(225*s)
    d.polygon([(nx-nw//2,ny),(nx+nw//2,ny),(tx+nw//2,ty),(tx-nw//2,ty)],fill=(49,67,74) if carbon else (82,68,52),outline=metal)
    for n in range(1,9):
        t=n/9; fx=nx+(tx-nx)*t; fy=ny+(ty-ny)*t
        half=max(3,int(nw*.45*(1-t*.25)))
        d.line((fx-half,fy,fx+half,fy),fill=(210,225,229) if carbon else (210,190,150),width=1)
    for off in (-.32,-.16,0,.16,.32):
        d.line((nx+int(nw*off),ny,tx+int(nw*off*.7),ty),fill=(232,242,245) if carbon else (232,224,205),width=1)
    for off in (-.17,.17):
        py=cy+int(bh*off)
        d.rounded_rectangle((cx-int(30*s),py-int(5*s),cx+int(30*s),py+int(5*s)),radius=max(2,int(3*s)),fill=metal)
    d.line((cx-int(38*s),cy+int(bh*.28),cx+int(38*s),cy+int(bh*.28)),fill=accent,width=max(2,int(3*s)))

def render(size,p,theme):
    w,h=size
    bg,panel=rgb(p["window_bg"]),rgb(p["panel"])
    accent,glow=rgb(p["accent"]),rgb(p["accent_glow"])
    primary,secondary,border=rgb(p["text_primary"]),rgb(p["text_secondary"]),rgb(p["control_border"])
    strip=Image.new("RGB",(1,h)); strip.putdata([mix(mix(bg,panel,.42),(3,3,6),.25*y/max(1,h-1)) for y in range(h)])
    im=strip.resize((w,h),Image.Resampling.BILINEAR)
    mask=Image.new("L",(w,h),0); md=ImageDraw.Draw(mask); bx,by=int(w*.77),int(h*.48)
    for n in range(18,0,-1):
        r=n/18; md.ellipse((bx-int(w*.42*r),by-int(h*.55*r),bx+int(w*.42*r),by+int(h*.55*r)),fill=round(20*(1-r*r)))
    mask=mask.filter(ImageFilter.GaussianBlur(max(2,w//90)))
    im=Image.composite(Image.new("RGB",(w,h),accent),im,mask)
    d=ImageDraw.Draw(im,"RGBA"); pad=max(8,int(w*.025))
    if theme=="SKIN-3":
        for k in range(-h,w,max(9,w//42)):
            d.line((k,0,k+h,h),fill=(*glow,17),width=max(1,w//900))
        for y in range(int(h*.23),int(h*.79),max(8,h//22)):
            d.line((int(w*.06),y,int(w*.93),y),fill=(*border,28),width=1)
    elif theme=="SKIN-4":
        for k in range(-h,w,max(14,w//30)):
            d.line((k,0,k+h,h),fill=(*glow,19),width=max(1,w//850))
        d.arc((int(w*.53),int(h*.12),int(w*.96),int(h*.92)),210,510,fill=(*accent,70),width=max(1,w//240))
        d.arc((int(w*.57),int(h*.16),int(w*.92),int(h*.88)),210,510,fill=(*glow,35),width=max(1,w//360))
    elif theme=="SKIN-5":
        for k in range(-h,w,max(22,w//26)):
            d.line((k,0,k+h,h),fill=(*glow,24),width=max(1,w//950))
        for n in range(5):
            x=int(w*(.58+n*.075)); d.line((x,int(h*.23),x-int(w*.12),int(h*.82)),fill=(*border,42),width=max(1,w//500))
        d.arc((int(w*.60),int(h*.10),int(w*.98),int(h*.88)),195,520,fill=(*accent,82),width=max(1,w//260))
        d.arc((int(w*.64),int(h*.16),int(w*.94),int(h*.82)),195,520,fill=(*glow,48),width=max(1,w//380))
    else:
        for k in range(-h,w,max(18,w//24)): d.line((k,0,k+h,h),fill=(*glow,10),width=1)
    d.rounded_rectangle((pad,pad,w-pad,h-pad),radius=max(8,int(w*.025)),outline=(*border,220),width=max(1,w//360))
    d.rounded_rectangle((pad+4,pad+4,w-pad-4,h-pad-4),radius=max(6,int(w*.02)),outline=(*glow,48),width=1)
    left=int(w*.065)
    d.line((left,int(h*.19),int(w*.93),int(h*.19)),fill=(*accent,150),width=max(1,w//400))
    d.text((left,int(h*.075)),"L3XTOWN AUDIO LAB  /  NEON STAGE",font=get_font(max(9,int(h*.035)),True),fill=(*secondary,255))
    title=THEMES[theme]["title"].split()
    d.text((left,int(h*.30)),title[0],font=get_font(max(18,int(h*.085)),True),fill=(*primary,255))
    d.text((left,int(h*.405)),title[1],font=get_font(max(18,int(h*.085)),True),fill=(*accent,255))
    d.text((left,int(h*.57)),f"{theme}  /  FACTORY THEME",font=get_font(max(9,int(h*.032)),True),fill=(*secondary,245))
    d.text((left,int(h*.84)),THEMES[theme]["tagline"],font=get_font(max(8,int(h*.025))),fill=(*glow,235))
    guitar(d,w,h,accent,glow,carbon=(theme in ("SKIN-3","SKIN-4","SKIN-5")))
    for x,y in ((pad+12,pad+12),(w-pad-12,pad+12),(pad+12,h-pad-12),(w-pad-12,h-pad-12)):
        d.ellipse((x-2,y-2,x+2,y+2),fill=(*glow,220))
    return im.convert("RGB")

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root",type=Path,default=Path("SKIN"))
    ap.add_argument("--theme",required=True,choices=sorted(THEMES))
    ap.add_argument("--check",action="store_true")
    a=ap.parse_args(); spec=THEMES[a.theme]; folder=a.root/spec["folder"]
    try:
        seq=json.loads((a.root/"approved_theme_sequence.json").read_text(encoding="utf-8"))
        approved=next((x for x in seq["themes"] if x.get("label")==a.theme),None)
        if not approved or approved.get("stableId")!=spec["stable_id"] or approved.get("index")!=spec["index"] or approved.get("path")!=f'{spec["folder"]}/theme.json':
            raise ValueError(f"{a.theme} identity differs from the immutable approved sequence")
        m=json.loads((folder/"theme.json").read_text(encoding="utf-8"))
        if m.get("id")!=spec["stable_id"] or m.get("index")!=spec["index"]:
            raise ValueError(f'{spec["folder"]} manifest identity mismatch')
        targets=[(folder/"assets/preview.png",(360,240)),(folder/"assets/hero.png",(800,450))]
        if a.check:
            for path,size in targets:
                if not path.is_file(): raise ValueError(f"Missing artwork: {path}")
                with Image.open(path) as im: im.verify()
                with Image.open(path) as im:
                    if im.size!=size: raise ValueError(f"{path}: {im.size}, expected {size}")
            print(f"PASS: {a.theme} preview and hero PNGs verified"); return 0
        (folder/"assets").mkdir(parents=True,exist_ok=True)
        for path,size in targets:
            render(size,m["palette"],a.theme).save(path,format="PNG",optimize=True)
            with Image.open(path) as im:
                im.verify()
                if im.size!=size: raise ValueError(f"Unexpected output dimensions for {path}")
        print(f"PASS: rendered original {a.theme} {spec['title']} artwork"); return 0
    except Exception as exc:
        print(f"FAIL: {exc}",file=sys.stderr); return 1

if __name__=="__main__": raise SystemExit(main())
