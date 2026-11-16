#!/usr/bin/env python3
"""Render the 15 approved PPGU skin artwork pairs. Existing PNGs are never overwritten by default."""
from __future__ import annotations
import argparse, json, math, random
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont, ImageFilter

def hx(v): return tuple(int(v.lstrip("#")[i:i+2],16) for i in (0,2,4))
def rgba(v,a=255): return (*hx(v),a)
def mix(a,b,t): return tuple(round(x+(y-x)*max(0,min(1,t))) for x,y in zip(a,b))
def getfont(size,bold=False):
    paths=["/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf" if bold else "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
           "/usr/share/fonts/truetype/liberation2/LiberationSans-Bold.ttf" if bold else "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf"]
    for p in paths:
        try: return ImageFont.truetype(p,size)
        except OSError: pass
    return ImageFont.load_default()

def motif(d,n,w,h,p):
    rng=random.Random(n*9173); a,g,hi=hx(p["accent"]),hx(p["accent_glow"]),hx(p["highlight"])
    if n==2: # Obsidian gold: faceted glass and radial engraving
        for x in range(-h,w,54):
            d.line((x,0,x+h,h),fill=(*a,45),width=2); d.line((x+18,0,x+h+18,h),fill=(*hi,20),width=1)
        for x in range(40,w,130):
            y=rng.randint(20,h-20); d.polygon([(x,y),(x+55,y-24),(x+105,y+5),(x+35,y+38)],outline=(*g,65))
        d.arc((w*.4,h*.08,w*1.1,h*1.1),200,335,fill=(*g,100),width=3)
    elif n==3: # Carbon weave and titanium rails
        for y in range(0,h,12):
            for x in range(0,w,16):
                c=25 if (x//16+y//12)%2 else 42
                d.rectangle((x,y,x+7,y+5),fill=(c,c+8,c+12,65))
        for x in range(-h,w,105): d.line((x,h,x+h,0),fill=(*hi,32),width=4)
    elif n==4: # Ruby lacquer shards
        for _ in range(10):
            x=rng.randrange(w); y=rng.randrange(h); s=rng.randrange(55,150)
            d.polygon([(x,y),(x+s,y-s//3),(x+s//2,y+s)],fill=(*a,15),outline=(*g,70))
        d.arc((w*.3,h*.1,w*1.1,h*1.1),190,330,fill=(*g,95),width=5)
    elif n==5: # Arctic platinum: faceted crystal prisms
        for _ in range(18):
            x=rng.randrange(w); y=rng.randrange(h); size=rng.randrange(45,125)
            pts=[(x,y-size),(x+size*.62,y-size*.12),(x+size*.18,y+size),(x-size*.58,y+size*.24)]
            d.polygon(pts,outline=(*hi,rng.randrange(28,78)),width=2)
            d.line((pts[0],pts[2]),fill=(*g,38),width=1)
            d.line((pts[1],pts[3]),fill=(*a,28),width=1)
    elif n==13: # Glacier teal: branching frost fractures
        for _ in range(24):
            x=rng.randrange(w); y=rng.randrange(h); length=rng.randrange(45,165)
            pts=[(x,y),(x+rng.randrange(-24,25),y+length*.35),(x+rng.randrange(-55,56),y+length)]
            d.line(pts,fill=(*g,rng.randrange(35,90)),width=2)
            branch=(pts[1][0]+rng.randrange(20,65),pts[1][1]+rng.randrange(8,38))
            d.line((pts[1],branch),fill=(*hi,55),width=1)
            d.line((pts[2],(pts[2][0]-rng.randrange(18,42),pts[2][1]-rng.randrange(8,32))),fill=(*a,35),width=1)
    elif n==6: # Violet noir: smoked-glass vortex and soft amethyst rings
        for i in range(9):
            box=(w*.34-i*17,h*.10-i*12,w*1.08+i*18,h*1.12+i*10)
            d.arc(box,195+i*3,338-i*2,fill=(*a,24+i*4),width=4 if i%2==0 else 2)
        for i in range(7):
            x=int(w*.68)+i*9; y=int(h*.48)-i*5; r=22+i*13
            d.ellipse((x-r,y-r,x+r,y+r),fill=(*g,7),outline=(*hi,18+i*3),width=2)
    elif n==15: # Ultraviolet drive: directional plasma speed trails
        for i in range(18):
            y=int(h*(.12+i*.045)); x0=int(w*(.05+(i%4)*.035))
            d.line((x0,y,x0+int(w*.72),y-int(h*.16)),fill=(*a,24+i*2),width=2 if i%3 else 4)
        for i in range(8):
            box=(w*.42-i*10,h*.20-i*7,w*1.08+i*14,h*.92+i*10)
            d.arc(box,205,295,fill=(*g,35+i*5),width=2)
    elif n==7: # Brushed titanium and storm cut
        for y in range(0,h,5): d.line((0,y,w,y),fill=(*hi,rng.randrange(8,28)),width=1)
        pts=[(w*.08,h*.2),(w*.25,h*.38),(w*.2,h*.5),(w*.43,h*.62),(w*.39,h*.74),(w*.65,h*.9)]
        d.line(pts,fill=(*g,105),width=4,joint="curve")
    elif n==8: # Emerald circuit etching
        for row in range(6):
            y=int(h*(.12+row*.14)); x0=rng.randrange(10,int(w*.32)); mid=int(w*rng.uniform(.35,.7)); x1=rng.randrange(int(w*.55),w-10)
            d.line((x0,y,mid,y,mid,y+22,x1,y+22),fill=(*a,85),width=3)
            for x,yy in ((x0,y),(mid,y+22),(x1,y+22)): d.ellipse((x-4,yy-4,x+4,yy+4),outline=(*g,120),width=2)
    elif n==9: # Solar bronze rays
        cx,cy=int(w*.7),int(h*.5)
        for angle in range(0,360,8):
            r1,r2=min(w,h)*.15,max(w,h)*.72
            d.line((cx+math.cos(math.radians(angle))*r1,cy+math.sin(math.radians(angle))*r1,
                    cx+math.cos(math.radians(angle))*r2,cy+math.sin(math.radians(angle))*r2),fill=(*a,35),width=2)
        d.ellipse((cx-85,cy-85,cx+85,cy+85),outline=(*g,85),width=4)
    elif n==10: # Deep ocean tidal contours
        for band in range(10):
            pts=[(x,h*(.15+band*.08)+math.sin(x*.012+band*.6)*17+math.sin(x*.026)*6) for x in range(-20,w+20,7)]
            d.line(pts,fill=(*a,30+band*4),width=3)
        for _ in range(40):
            x=rng.randrange(w); y=rng.randrange(h); r=rng.randrange(1,4); d.ellipse((x-r,y-r,x+r,y+r),outline=(*g,65))
    elif n==11: # Rose chrome rings
        for i in range(9): d.arc((w*.35-i*18,h*.1-i*10,w*1.1+i*15,h*1.1+i*8),205,342,fill=(*a,35+i*5),width=4 if i%2==0 else 2)
        d.line((w*.05,h*.8,w*.72,h*.18),fill=(*hi,55),width=4)
    elif n==12: # Desert dunes and sandblasted grain
        for band in range(9):
            pts=[(x,h*(.25+band*.085)+math.sin(x*.008+band*.8)*18+math.sin(x*.02)*5) for x in range(-10,w+10,6)]
            d.line(pts,fill=(*a,28+band*3),width=2)
        for _ in range(300):
            x=rng.randrange(w); y=rng.randrange(h); d.point((x,y),fill=(*hi,rng.randrange(8,28)))
    elif n==14: # Phantom graphite machined grid
        for x in range(0,w,26): d.line((x,0,x,h),fill=(*hi,14),width=1)
        for y in range(0,h,26): d.line((0,y,w,y),fill=(*hi,14),width=1)
        for _ in range(12):
            x=rng.randrange(w); y=rng.randrange(h); d.line((x,y,x+130,y-42),fill=(*a,28),width=2)
    elif n==16: # Volcanic basalt fissures
        for _ in range(22):
            x=rng.randrange(w); y=rng.randrange(h); length=rng.randrange(45,170); pts=[(x,y)]
            for _ in range(4): x+=rng.randrange(-25,26); y+=length/4+rng.randrange(-8,9); pts.append((x,y))
            d.line(pts,fill=(*a,70),width=5,joint="curve"); d.line(pts,fill=(*g,120),width=1,joint="curve")

def guitar(w,h,p):
    layer=Image.new("RGBA",(360,500),(0,0,0,0)); d=ImageDraw.Draw(layer)
    accent,glow,metal,edge,dark=(rgba(p["accent"],235),rgba(p["accent_glow"],190),rgba(p["control_face"],248),rgba(p["highlight"],235),rgba(p["control_bg"],255))
    d.polygon([(145,15),(210,15),(220,255),(135,255)],fill=metal,outline=edge)
    d.polygon([(140,18),(215,18),(238,0),(118,0)],fill=dark,outline=glow)
    for x in (128,225):
        for y in (12,35,58): d.ellipse((x-7,y-5,x+7,y+5),fill=edge,outline=accent,width=2)
    for y in range(68,260,18): d.line((140,y,215,y),fill=(*edge[:3],150),width=2)
    for x in (151,162,173,184,195,206): d.line((x,8,x,405),fill=(*glow[:3],190),width=1)
    body=[(143,235),(112,218),(82,230),(72,265),(92,295),(75,345),(92,405),(130,445),(178,458),(225,445),(263,405),(280,345),(263,295),(283,265),(270,230),(240,218),(209,235),(192,272),(160,272)]
    d.polygon(body,fill=metal,outline=edge,width=5); d.line(body+[body[0]],fill=accent,width=3,joint="curve")
    d.rounded_rectangle((145,285,215,310),radius=6,fill=dark,outline=glow,width=3)
    d.rounded_rectangle((145,330,215,355),radius=6,fill=dark,outline=edge,width=3)
    d.rounded_rectangle((142,375,218,388),radius=4,fill=dark,outline=accent,width=2)
    d.ellipse((228,330,247,349),fill=glow,outline=edge,width=2); d.ellipse((232,365,251,384),fill=glow,outline=edge,width=2)
    d.arc((92,250,264,445),205,335,fill=glow,width=3)
    target_h=int(h*.93); target_w=int(target_h*360/500)
    layer=layer.resize((target_w,target_h),Image.Resampling.LANCZOS).rotate(18,resample=Image.Resampling.BICUBIC,expand=True)
    out=Image.new("RGBA",(w,h),(0,0,0,0)); out.alpha_composite(layer,(int(w*.66-layer.width*.48),int(h*.48-layer.height*.48)))
    return out

def render(theme,ordinal,size):
    w,h=size; p=theme["palette"]; bg0,bg1,bg2=hx(p["window_bg"]),hx(p["panel"]),hx(p["panel_elevated"])
    im=Image.new("RGB",(w,h)); px=im.load(); cx,cy=w*.69,h*.48; md=math.hypot(w,h)
    for y in range(h):
        for x in range(w):
            t=y/max(1,h-1); radial=max(0,1-math.hypot(x-cx,y-cy)/md)
            px[x,y]=mix(mix(bg0,bg1,t*.55+radial*.20),bg2,max(0,radial-.3)*.30)
    glow=Image.new("RGBA",(w,h),(0,0,0,0)); gd=ImageDraw.Draw(glow); r=int(min(w,h)*.43); bx,by=int(w*.7),int(h*.48)
    gd.ellipse((bx-r,by-r,bx+r,by+r),fill=rgba(p["accent_glow"],55)); glow=glow.filter(ImageFilter.GaussianBlur(max(8,int(min(w,h)*.12))))
    im=Image.alpha_composite(im.convert("RGBA"),glow)
    tex=Image.new("RGBA",(w,h),(0,0,0,0)); motif(ImageDraw.Draw(tex),ordinal,w,h,p); im=Image.alpha_composite(im,tex)
    frame=Image.new("RGBA",(w,h),(0,0,0,0)); d=ImageDraw.Draw(frame); m=max(8,int(w*.025))
    d.rounded_rectangle((m,m,w-m,h-m),radius=max(6,int(w*.018)),outline=rgba(p["control_border"],185),width=max(1,int(w*.003)))
    d.line((m*2,m*2,w*.54,m*2),fill=rgba(p["highlight"],110),width=1)
    for x,y in ((m*2,m*2),(w-m*2,m*2),(m*2,h-m*2),(w-m*2,h-m*2)):
        q=max(2,int(w*.004)); d.ellipse((x-q,y-q,x+q,y+q),fill=rgba(p["accent_glow"],190))
    im=Image.alpha_composite(im,frame); im=Image.alpha_composite(im,guitar(w,h,p))
    text=Image.new("RGBA",(w,h),(0,0,0,0)); td=ImageDraw.Draw(text); s=min(w/800,h/450); x=int(w*.075); y=int(h*.34)
    title=theme["name"].upper(); tf=getfont(max(12,int(31*s)),True); sf=getfont(max(7,int(10*s)),True); sm=getfont(max(6,int(8*s)))
    td.text((x+2,y+2),title,font=tf,fill=(0,0,0,150)); td.text((x,y),title,font=tf,fill=rgba(p["text_primary"],245))
    td.rounded_rectangle((x,y+int(42*s),x+int(64*s),y+int(45*s)),radius=2,fill=rgba(p["accent"],235))
    td.text((x,y+int(58*s)),"SIGNAL FORGE",font=sf,fill=rgba(p["accent_glow"],235))
    td.text((x,y+int(78*s)),"POP PUNK GUITAR ULTIMATE",font=sm,fill=rgba(p["text_secondary"],190))
    td.text((w-int(100*s),int(24*s)),f"SKIN {ordinal:02d} / 16",font=sm,fill=rgba(p["text_secondary"],180))
    return Image.alpha_composite(im,text).convert("RGB")

def main():
    ap=argparse.ArgumentParser(description=__doc__); ap.add_argument("--root",type=Path,default=Path("SKIN"))
    ap.add_argument("--force",action="store_true"); ap.add_argument("--only",type=int,nargs="*")
    args=ap.parse_args(); seq=json.loads((args.root/"approved_theme_sequence.json").read_text(encoding="utf-8"))
    themes=seq.get("themes",[])
    if len(themes)!=16: raise SystemExit(f"Expected 16 approved themes, found {len(themes)}")
    selected=set(args.only or range(2,17))
    if any(n<2 or n>16 for n in selected): raise SystemExit("--only accepts ordinals 2..16")
    made=kept=0
    for item in themes:
        n=item["ordinal"]
        if n not in selected: continue
        folder=args.root/item["path"].split("/")[0]; theme=json.loads((folder/"theme.json").read_text(encoding="utf-8"))
        if theme.get("id")!=item["stableId"] or theme.get("name")!=item["name"]: raise SystemExit(f"{folder}: sequence identity/name mismatch")
        assets=folder/"assets"; assets.mkdir(parents=True,exist_ok=True)
        for key,size in (("preview",(360,240)),("hero",(800,450))):
            path=assets/f"{key}.png"
            if path.exists() and not args.force: kept+=1; continue
            render(theme,n,size).save(path,format="PNG",optimize=True,compress_level=9); made+=1
            print(f"GENERATED {path} {size[0]}x{size[1]} {path.stat().st_size} bytes")
    print(f"Artwork render complete: generated={made}, preserved={kept}")
if __name__=="__main__": main()
