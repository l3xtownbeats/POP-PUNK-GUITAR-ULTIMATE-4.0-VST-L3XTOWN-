#!/usr/bin/env python3
"""Deterministic NEON STAGE REV ULTRA 3.0 asset generator.

All output is written below dist/neon_stage_staging. No runtime skin, JUCE
resource, registry, DSP, APVTS, MIDI or processor file is modified.
"""
from __future__ import annotations

import hashlib
import json
import math
import struct
import zlib
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont, ImageFilter, ImageOps

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "SKIN" / "Skin_052"
OUT_ROOT = ROOT / "dist" / "neon_stage_staging"
SKIN = OUT_ROOT / "Skin_052"
ASSETS = SKIN / "assets"
ATLAS = ASSETS / "atlas"
WINDOWS = ASSETS / "windows"
SHEETS = OUT_ROOT / "contact_sheets"
REF = ROOT / "01 INTERFACE  NEON STAGE DEFAULT .png"

TOKENS = {
    "bg_deep": "#0A0E18", "bg_panel": "#141B2A", "bg_panel_hi": "#1E2A40",
    "border": "#2A3A55", "border_glow": "#22CFFF", "text_primary": "#EAF6FF",
    "text_secondary": "#9FB6D4", "text_disabled": "#5A6B85",
    "accent_primary": "#22CFFF", "accent_secondary": "#FF3EA5",
    "accent_warning": "#FFB454", "accent_danger": "#FF5A5A",
    "accent_success": "#46E88C", "lcd_bg": "#061021", "lcd_text": "#7FE8FF",
    "knob_body": "#0C1220", "knob_ring": "#22CFFF", "led_on": "#46E8FF",
    "led_off": "#1A2436", "overlay_scrim": "#04080FCC", "focus_ring": "#7FE8FF",
}
P = TOKENS
def rgba(name):
    value = P[name].lstrip("#")
    if len(value) == 8:
        return tuple(int(value[i:i+2], 16) for i in (0, 2, 4, 6))
    return tuple(int(value[i:i+2], 16) for i in (0, 2, 4)) + (255,)
def rgb(name):
    return rgba(name)[:3]
def font(size=12):
    return ImageFont.load_default()
def ensure_dirs():
    for p in (ASSETS, ATLAS, WINDOWS, SHEETS, SKIN / "svg"):
        p.mkdir(parents=True, exist_ok=True)
def save_png8(image, path):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    if image.mode == "P":
        indexed = image
    else:
        indexed = image.convert("RGBA").quantize(colors=256, method=Image.Quantize.FASTOCTREE,
                                                  dither=Image.Dither.NONE)
    indexed.save(path, format="PNG", optimize=False, compress_level=9)
def write_json(path, data):
    Path(path).write_text(json.dumps(data, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
def frame_index(name, x, y, w, h, **extra):
    return {"name": name, "x": x, "y": y, "w": w, "h": h, **extra}
def luminance(hex_value):
    value = hex_value[1:7]
    channels = [int(value[i:i+2], 16) / 255 for i in (0, 2, 4)]
    linear = [v / 12.92 if v <= .04045 else ((v + .055) / 1.055) ** 2.4 for v in channels]
    return .2126 * linear[0] + .7152 * linear[1] + .0722 * linear[2]
def contrast(a, b):
    hi, lo = sorted((luminance(a), luminance(b)), reverse=True)
    return (hi + .05) / (lo + .05)

def render_knob(size, frame, state):
    im = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    cx = cy = size // 2
    radius = int(size * .37)
    body = rgba("knob_body")
    border = rgba("border")
    ring = rgba("knob_ring")
    glow = rgba("border_glow")
    if state == "disabled":
        body = rgba("bg_panel")
        border = rgba("text_disabled")
        ring = rgba("text_disabled")
        glow = rgba("text_disabled")
    d.ellipse((cx-radius, cy-radius, cx+radius, cy+radius),
              fill=body, outline=border, width=max(2, size//48))
    inset = int(size*.055)
    d.ellipse((cx-radius+inset, cy-radius+inset, cx+radius-inset, cy+radius-inset),
              fill=rgba("bg_panel_hi"), outline=rgba("text_secondary"), width=max(1,size//100))
    start = 135
    angle_deg = -135.0 + 22.5 * frame
    end = 135 + angle_deg + 135
    box = (int(size*.12), int(size*.12), int(size*.88), int(size*.88))
    d.arc(box, start=135, end=max(136, min(405, end)), fill=ring, width=max(3,size//22))
    if state == "highlight":
        d.ellipse((cx-radius-2,cy-radius-2,cx+radius+2,cy+radius+2),
                  outline=glow, width=max(2,size//36))
    a = math.radians(angle_deg)
    inner, outer = size*.08, size*.28
    d.line((cx+math.cos(a)*inner,cy+math.sin(a)*inner,
            cx+math.cos(a)*outer,cy+math.sin(a)*outer),
           fill=rgba("focus_ring") if state != "disabled" else rgba("text_disabled"),
           width=max(3,size//24))
    d.ellipse((cx-size*.045,cy-size*.045,cx+size*.045,cy+size*.045),
              fill=rgba("text_primary"))
    return im

def generate_knob_atlas(name, ids, frame_size):
    states = ("base", "highlight", "disabled")
    cols, rows = 13, len(ids)*len(states)
    atlas = Image.new("RGBA", (cols*frame_size, rows*frame_size), (0,0,0,0))
    frames = []
    row = 0
    for knob_id in ids:
        for state in states:
            for angle in range(13):
                x, y = angle*frame_size, row*frame_size
                atlas.alpha_composite(render_knob(frame_size, angle, state), (x,y))
                frames.append(frame_index(f"{knob_id}_angle{angle:02d}_{state}",
                                          x,y,frame_size,frame_size,
                                          control=knob_id,state=state,
                                          angle_degrees=-135+22.5*angle))
            row += 1
    save_png8(atlas, ATLAS / f"{name}.png")
    write_json(ATLAS / f"{name}.json", {"atlas":name,"frameSize":frame_size,
        "columns":cols,"rows":rows,"frames":frames})
    if name == "knobs_main":
        scaled = atlas.resize((atlas.width*2, atlas.height*2), Image.Resampling.NEAREST)
        save_png8(scaled, ATLAS / "knobs_main@2x.png")
        write_json(ATLAS / "knobs_main@2x.json", {"atlas":"knobs_main@2x",
            "frameSize":frame_size*2,"columns":cols,"rows":rows,
            "frames":[{**f,"x":f["x"]*2,"y":f["y"]*2,"w":f["w"]*2,"h":f["h"]*2}
                      for f in frames]})
    if name == "knobs_amp":
        scaled = atlas.resize((atlas.width*2, atlas.height*2), Image.Resampling.NEAREST)
        save_png8(scaled, ATLAS / "knobs_amp@2x.png")
        write_json(ATLAS / "knobs_amp@2x.json", {"atlas":"knobs_amp@2x",
            "frameSize":frame_size*2,"columns":cols,"rows":rows,
            "frames":[{**f,"x":f["x"]*2,"y":f["y"]*2,"w":f["w"]*2,"h":f["h"]*2}
                      for f in frames]})

def draw_fx_glyph(d, name, x, y, colour):
    cx, cy = x+18, y+18
    if name == "compressor":
        d.line((cx-11,cy+7,cx-5,cy-6,cx,cy+1,cx+6,cy-9,cx+12,cy+7),fill=colour,width=3)
    elif name == "eq":
        for i,h in enumerate((10,17,13,20,8)):
            xx=cx-10+i*5
            d.line((xx,cy+h//2,xx,cy-h//2),fill=colour,width=3)
    elif name == "distortion":
        d.line((cx-12,cy+8,cx-8,cy-8,cx-3,cy+7,cx+2,cy-8,cx+7,cy+8,cx+12,cy-8),fill=colour,width=3)
    elif name == "chorus":
        pts=[(cx-12,cy),(cx-9,cy-7),(cx-4,cy-9),(cx,cy),(cx+4,cy+9),(cx+9,cy+7),(cx+12,cy)]
        d.line(pts,fill=colour,width=3)
    elif name == "delay":
        d.ellipse((cx-10,cy-10,cx+10,cy+10),outline=colour,width=2)
        d.line((cx,cy,cx,cy-7),fill=colour,width=2);d.line((cx,cy,cx+6,cy+4),fill=colour,width=2)
    elif name == "reverb":
        d.rectangle((cx-9,cy-9,cx+9,cy+9),outline=colour,width=2)
        d.line((cx-9,cy-3,cx+9,cy-3),fill=colour,width=2)
        d.line((cx-3,cy-9,cx-3,cy+9),fill=colour,width=2)
    else:
        d.ellipse((cx-10,cy-10,cx+10,cy+10),outline=colour,width=2)
        d.ellipse((cx-3,cy-3,cx+3,cy+3),outline=colour,width=2)

def generate_fx():
    names=("compressor","eq","distortion","chorus","delay","reverb","cabinet_ir")
    colours=("accent_primary","accent_primary","accent_warning","accent_primary",
             "accent_secondary","accent_success","accent_warning")
    states=("off","on","bypass","disabled")
    w,h=140,84
    atlas=Image.new("RGBA",(len(names)*w,len(states)*h),(0,0,0,0))
    frames=[]
    for row,state in enumerate(states):
        for col,name in enumerate(names):
            x,y=col*w,row*h
            d=ImageDraw.Draw(atlas)
            accent=rgba(colours[col])
            if state=="disabled": accent=rgba("text_disabled")
            fill=rgba("bg_panel_hi") if state=="on" else rgba("bg_panel")
            edge=accent if state in ("on","off") else rgba("accent_warning" if state=="bypass" else "border")
            d.rounded_rectangle((x+3,y+3,x+w-4,y+h-4),radius=9,fill=fill,outline=edge,width=2)
            d.rounded_rectangle((x+10,y+8,x+w-11,y+12),radius=2,fill=edge)
            draw_fx_glyph(d,name,x+9,y+20,accent)
            d.text((x+42,y+31),name.replace("_"," ").upper(),font=font(),fill=rgba("text_primary" if state!="disabled" else "text_disabled"))
            if state=="bypass":
                d.text((x+42,y+49),"BYPASS",font=font(),fill=rgba("accent_warning"))
            elif state=="disabled":
                d.text((x+42,y+49),"DISABLED",font=font(),fill=rgba("text_disabled"))
            else:
                d.ellipse((x+w-20,y+26,x+w-11,y+35),fill=rgba("led_on" if state=="on" else "led_off"))
            frames.append(frame_index(f"{name}_{state}",x,y,w,h,module=name,state=state))
    save_png8(atlas,ATLAS/"fx_tiles.png")
    write_json(ATLAS/"fx_tiles.json",{"atlas":"fx_tiles","frameSize":{"w":w,"h":h},
        "columns":len(names),"rows":len(states),"frames":frames})

def generate_toggles():
    styles=("neon_pill","flat","metallic")
    states=("on","off","bypass")
    w,h=72,32
    atlas=Image.new("RGBA",(w*len(states),h*len(styles)),(0,0,0,0))
    frames=[]
    for r,style in enumerate(styles):
        for c,state in enumerate(states):
            x,y=c*w,r*h; d=ImageDraw.Draw(atlas)
            fill=rgba("accent_primary") if state=="on" else rgba("bg_panel_hi")
            edge=rgba("accent_warning") if state=="bypass" else rgba("border_glow" if state=="on" else "border")
            if style=="flat": fill=rgba("bg_panel") if state!="on" else rgba("bg_panel_hi")
            if style=="metallic": fill=rgba("knob_body")
            d.rounded_rectangle((x+2,y+2,x+w-3,y+h-3),radius=14,fill=fill,outline=edge,width=2)
            label={"on":"ON","off":"OFF","bypass":"BYPASS"}[state]
            d.text((x+7,y+11),label,font=font(),fill=rgba("bg_deep" if state=="on" and style=="neon_pill" else "text_primary"))
            d.ellipse((x+w-18,y+9,x+w-8,y+19),fill=rgba("led_on" if state=="on" else "accent_warning" if state=="bypass" else "led_off"))
            frames.append(frame_index(f"{style}_{state}",x,y,w,h,style=style,state=state))
    save_png8(atlas,ATLAS/"toggles.png")
    write_json(ATLAS/"toggles.json",{"atlas":"toggles","frameSize":{"w":w,"h":h},
        "columns":3,"rows":3,"frames":frames})

def generate_sliders():
    # H/V tracks, sync track, and three thumb states are independently indexed.
    items=[]
    w,h=360,28
    for state in ("normal","hover","dragged"):
        im=Image.new("RGBA",(w,h),(0,0,0,0));d=ImageDraw.Draw(im)
        d.rounded_rectangle((4,10,w-5,18),radius=4,fill=rgba("bg_deep"),outline=rgba("border"),width=1)
        d.rounded_rectangle((4,10,210,18),radius=4,fill=rgba("accent_primary"))
        x={"normal":210,"hover":210,"dragged":240}[state]
        edge=rgba("focus_ring" if state!="normal" else "border")
        d.ellipse((x-10,3,x+10,23),fill=rgba("knob_body"),outline=edge,width=2)
        d.line((x,6,x,20),fill=rgba("text_primary"),width=2)
        items.append((f"horizontal_{state}",im))
    for state in ("normal","hover","dragged"):
        im=Image.new("RGBA",(28,360),(0,0,0,0));d=ImageDraw.Draw(im)
        d.rounded_rectangle((10,4,18,355),radius=4,fill=rgba("bg_deep"),outline=rgba("border"),width=1)
        d.rounded_rectangle((10,150,18,355),radius=4,fill=rgba("accent_primary"))
        y={"normal":150,"hover":150,"dragged":180}[state]
        d.ellipse((3,y-10,23,y+10),fill=rgba("knob_body"),outline=rgba("focus_ring" if state!="normal" else "border"),width=2)
        items.append((f"vertical_{state}",im))
    for name in ("pitch","mod"):
        im=Image.new("RGBA",(48,180),(0,0,0,0));d=ImageDraw.Draw(im)
        d.rounded_rectangle((18,4,30,176),radius=5,fill=rgba("bg_deep"),outline=rgba("border"),width=2)
        d.rounded_rectangle((12,78,36,102),radius=5,fill=rgba("knob_body"),outline=rgba("border_glow"),width=2)
        d.line((18,90,30,90),fill=rgba("text_primary"),width=2)
        items.append((f"wheel_{name}",im))
    # Pack variable-sized sprites into a deterministic horizontal strip.
    width=sum(im.width+4 for _,im in items); height=max(im.height for _,im in items)
    atlas=Image.new("RGBA",(width,height),(0,0,0,0));frames=[];x=0
    for name,im in items:
        atlas.alpha_composite(im,(x,0))
        frames.append(frame_index(name,x,0,im.width,im.height))
        x+=im.width+4
    save_png8(atlas,ATLAS/"sliders.png")
    write_json(ATLAS/"sliders.json",{"atlas":"sliders","frames":frames})

def generate_keys():
    for colour,size,states in (
        ("white",(46,160),("normal","pressed","glow")),
        ("black",(28,96),("normal","pressed","glow")),
    ):
        w,h=size; atlas=Image.new("RGBA",(w*3,h),(0,0,0,0));frames=[]
        for i,state in enumerate(states):
            x=i*w;d=ImageDraw.Draw(atlas)
            fill=rgba("text_primary") if colour=="white" else rgba("knob_body")
            if state=="pressed": fill=rgba("bg_panel_hi")
            edge=rgba("border_glow" if state=="glow" else "border")
            d.rounded_rectangle((x+1,1,x+w-2,h-2),radius=3,fill=fill,outline=edge,width=2)
            if state=="glow":
                d.line((x+4,h-8,x+w-5,h-8),fill=rgba("led_on"),width=3)
            if state=="pressed":
                d.line((x+3,h-4,x+w-4,h-4),fill=rgba("accent_primary"),width=2)
            frames.append(frame_index(f"{colour}_{state}",x,0,w,h,state=state))
        save_png8(atlas,ATLAS/f"keys_{colour}.png")
        write_json(ATLAS/f"keys_{colour}.json",{"atlas":f"keys_{colour}",
            "frameSize":{"w":w,"h":h},"frames":frames})

def generate_meters():
    colours=("cyan","magenta","orange","green","red")
    token_map={"cyan":"led_on","magenta":"accent_secondary","orange":"accent_warning",
               "green":"accent_success","red":"accent_danger"}
    w,h=20,10; atlas=Image.new("RGBA",(w*5,h*2),(0,0,0,0));frames=[]
    for r,state in enumerate(("off","on")):
        for c,name in enumerate(colours):
            x,y=c*w,r*h;d=ImageDraw.Draw(atlas)
            fill=rgba("led_off") if state=="off" else rgba(token_map[name])
            d.rounded_rectangle((x+1,y+1,x+w-2,y+h-2),radius=3,fill=fill)
            frames.append(frame_index(f"segment_{name}_{state}",x,y,w,h,colour=name,state=state))
    save_png8(atlas,ATLAS/"meters.png")
    write_json(ATLAS/"meters.json",{"atlas":"meters","frameSize":{"w":w,"h":h},
        "columns":5,"rows":2,"frames":frames})
    for name,token in (("gr_column","accent_primary"),("output_column","accent_success")):
        im=Image.new("RGBA",(28,180),(0,0,0,0));d=ImageDraw.Draw(im)
        for i in range(18):
            y=174-i*9
            col=token if i<12 else ("accent_warning" if i<16 else "accent_danger")
            d.rounded_rectangle((3,y,25,y+6),radius=2,
                fill=rgba(col if i<8 else "led_off"))
        save_png8(im,ATLAS/f"{name}.png")
        write_json(ATLAS/f"{name}.json",{"atlas":name,"frames":[frame_index(name,0,0,28,180)]})

def generate_window_chrome():
    actions=("close","maximize");states=("normal","hover")
    size=24;atlas=Image.new("RGBA",(size*2,size*2),(0,0,0,0));frames=[]
    for r,action in enumerate(actions):
        for c,state in enumerate(states):
            x,y=c*size,r*size;d=ImageDraw.Draw(atlas)
            col=rgba("text_secondary" if state=="normal" else "accent_primary")
            if action=="close":
                d.line((x+7,y+7,x+17,y+17),fill=col,width=2)
                d.line((x+17,y+7,x+7,y+17),fill=col,width=2)
            else:
                d.rectangle((x+6,y+6,x+18,y+18),outline=col,width=2)
            frames.append(frame_index(f"{action}_{state}",x,y,size,size,action=action,state=state))
    save_png8(atlas,ATLAS/"window_chrome.png")
    write_json(ATLAS/"window_chrome.json",{"atlas":"window_chrome",
        "frameSize":{"w":size,"h":size},"columns":2,"rows":2,"frames":frames})

def generate_panels():
    # Each panel primitive is a separate indexed PNG so JUCE can scale it safely.
    d=Image.new("RGBA",(480,44),rgba("bg_panel"));draw=ImageDraw.Draw(d)
    draw.rectangle((0,0,479,43),outline=rgba("border"),width=2)
    draw.rounded_rectangle((8,6,472,38),radius=6,outline=rgba("border_glow"),width=1)
    draw.line((14,42,466,42),fill=rgba("accent_primary"),width=2)
    save_png8(d,ATLAS/"panel_header.png")
    lcd=Image.new("RGBA",(320,54),rgba("lcd_bg"));dl=ImageDraw.Draw(lcd)
    dl.rectangle((1,1,318,52),outline=rgba("border_glow"),width=2)
    dl.line((8,43,312,43),fill=rgba("led_off"),width=1)
    save_png8(lcd,ATLAS/"lcd_panel.png")
    badge=Image.new("RGBA",(112,24),(0,0,0,0));db=ImageDraw.Draw(badge)
    db.rounded_rectangle((1,1,110,22),radius=8,fill=rgba("bg_panel_hi"),outline=rgba("text_secondary"),width=1)
    db.text((12,8),"SETUP-ONLY",font=font(),fill=rgba("text_secondary"))
    save_png8(badge,ATLAS/"setup_only_badge.png")
    card=Image.new("RGBA",(260,160),rgba("bg_panel"));dc=ImageDraw.Draw(card)
    dc.rounded_rectangle((1,1,258,158),radius=8,outline=rgba("border"),width=2)
    dc.rectangle((8,8,251,112),fill=rgba("bg_deep"),outline=rgba("border"))
    dc.line((8,120,251,120),fill=rgba("border_glow"),width=2)
    save_png8(card,ATLAS/"gallery_tile.png")
    tabs=Image.new("RGBA",(112*3,36),(0,0,0,0));frames=[]
    for i,state in enumerate(("inactive","hover","active")):
        x=i*112;dt=ImageDraw.Draw(tabs)
        fill=rgba("bg_panel_hi" if state=="active" else "bg_panel")
        edge=rgba("border_glow" if state!="inactive" else "border")
        dt.rounded_rectangle((x+2,2,x+109,33),radius=6,fill=fill,outline=edge,width=2)
        frames.append(frame_index(f"tab_{state}",x,0,112,36,state=state))
    save_png8(tabs,ATLAS/"tabs.png")
    write_json(ATLAS/"tabs.json",{"atlas":"tabs","frames":frames})
    transport=Image.new("RGBA",(40*3,40),(0,0,0,0));frames=[]
    for i,name in enumerate(("play","pause","stop")):
        x=i*40;dt=ImageDraw.Draw(transport);col=rgba("accent_primary")
        if name=="play":dt.polygon((x+12,8,x+31,20,x+12,32),fill=col)
        elif name=="pause":
            dt.rectangle((x+10,9,x+17,31),fill=col);dt.rectangle((x+23,9,x+30,31),fill=col)
        else:dt.rectangle((x+10,9,x+30,31),fill=col)
        frames.append(frame_index(name,x,0,40,40))
    save_png8(transport,ATLAS/"transport.png")
    write_json(ATLAS/"transport.json",{"atlas":"transport","frames":frames})
    panel_frames=[
        frame_index("panel_header",0,0,480,44),
        frame_index("lcd_panel",0,0,320,54),
        frame_index("setup_only_badge",0,0,112,24),
        frame_index("gallery_tile",0,0,260,160),
    ]
    write_json(ATLAS/"panels.json",{"atlas":"panels","individualAssets":panel_frames,
        "notes":"Individual PNGs are named in the frame index; tabs and transport have their own atlases."})

def generate_icons():
    names=("folder","star","search","clear","palette","guitar","radar","curator","settings","about",
           "note","grid","group","play","pause","stop","loop","dice","info","crown")
    cols,rows=5,4;size=32
    atlas=Image.new("RGBA",(cols*size,rows*size),(0,0,0,0));frames=[]
    for i,name in enumerate(names):
        x=(i%cols)*size;y=(i//cols)*size;d=ImageDraw.Draw(atlas)
        col=rgba("accent_primary")
        if name=="folder":
            d.rectangle((x+4,y+10,x+27,y+24),outline=col,width=2);d.rectangle((x+5,y+7,x+15,y+12),outline=col,width=2)
        elif name=="star":
            pts=[]
            for k in range(10):
                a=-math.pi/2+k*math.pi/5;r=12 if k%2==0 else 5
                pts.append((x+16+math.cos(a)*r,y+16+math.sin(a)*r))
            d.polygon(pts,outline=col)
        elif name=="search":
            d.ellipse((x+5,y+4,x+21,y+20),outline=col,width=2);d.line((x+19,y+19,x+28,y+28),fill=col,width=3)
        elif name=="clear":
            d.line((x+7,y+7,x+25,y+25),fill=col,width=3);d.line((x+25,y+7,x+7,y+25),fill=col,width=3)
        elif name=="palette":
            d.ellipse((x+4,y+4,x+28,y+28),outline=col,width=2);d.ellipse((x+10,y+10,x+13,y+13),fill=col);d.ellipse((x+18,y+9,x+21,y+12),fill=col)
        elif name=="guitar":
            d.ellipse((x+7,y+15,x+21,y+28),outline=col,width=2);d.line((x+16,y+17,x+25,y+5),fill=col,width=3);d.line((x+21,y+9,x+27,y+13),fill=col,width=2)
        elif name=="radar":
            d.ellipse((x+4,y+4,x+28,y+28),outline=col,width=2);d.line((x+16,y+16,x+25,y+7),fill=col,width=2);d.arc((x+8,y+8,x+24,y+24),0,270,fill=col,width=1)
        elif name=="curator":
            d.ellipse((x+11,y+5,x+21,y+15),outline=col,width=2);d.arc((x+5,y+12,x+27,y+30),180,360,fill=col,width=2)
        elif name=="settings":
            d.ellipse((x+7,y+7,x+25,y+25),outline=col,width=3);d.ellipse((x+13,y+13,x+19,y+19),outline=col,width=2)
        elif name=="about" or name=="info":
            d.ellipse((x+4,y+4,x+28,y+28),outline=col,width=2);d.text((x+13,y+9),"i",font=font(15),fill=col)
        elif name=="note":
            d.line((x+19,y+5,x+19,y+22),fill=col,width=3);d.line((x+19,y+5,x+27,y+3),fill=col,width=2);d.ellipse((x+8,y+18,x+19,y+27),outline=col,width=2)
        elif name=="grid":
            for xx in (6,16,26):
                for yy in (6,16,26):d.rectangle((x+xx-3,y+yy-3,x+xx+3,y+yy+3),outline=col,width=1)
        elif name=="group":
            for xx in (9,16,23):d.ellipse((x+xx-3,y+7,x+xx+3,y+13),outline=col,width=1)
            d.arc((x+4,y+12,x+28,y+29),180,360,fill=col,width=2)
        elif name=="play":d.polygon((x+9,y+5,x+26,y+16,x+9,y+27),fill=col)
        elif name=="pause":d.rectangle((x+8,y+6,x+13,y+26),fill=col);d.rectangle((x+19,y+6,x+24,y+26),fill=col)
        elif name=="stop":d.rectangle((x+7,y+7,x+25,y+25),fill=col)
        elif name=="loop":
            d.arc((x+5,y+6,x+27,y+26),30,300,fill=col,width=2);d.polygon((x+23,y+4,x+28,y+9,x+20,y+10),fill=col)
        elif name=="dice":
            d.rounded_rectangle((x+5,y+5,x+27,y+27),radius=4,outline=col,width=2)
            for xx,yy in ((10,10),(22,10),(16,16),(10,22),(22,22)):d.ellipse((x+xx-1,y+yy-1,x+xx+1,y+yy+1),fill=col)
        elif name=="crown":
            d.polygon((x+4,y+10,x+10,y+16,x+16,y+7,x+22,y+16,x+28,y+10,x+25,y+25,x+7,y+25),outline=rgba("accent_warning"))
        frames.append(frame_index(name,x,y,size,size))
    save_png8(atlas,ATLAS/"icons.png")
    write_json(ATLAS/"icons.json",{"atlas":"icons","frameSize":{"w":size,"h":size},
        "columns":cols,"rows":rows,"frames":frames})

def svg_assets():
    palette=P
    texture=f'''<svg xmlns="http://www.w3.org/2000/svg" width="128" height="128" viewBox="0 0 128 128">
<rect width="128" height="128" fill="{palette['bg_deep']}"/>
<path d="M0 16H128 M0 64H128 M0 112H128 M16 0V128 M64 0V128 M112 0V128" stroke="{palette['border']}" stroke-opacity=".35"/>
<circle cx="8" cy="8" r="1" fill="{palette['border_glow']}"/>
</svg>'''
    bezel=f'''<svg xmlns="http://www.w3.org/2000/svg" width="800" height="450" viewBox="0 0 800 450">
<rect x="2" y="2" width="796" height="446" rx="20" fill="none" stroke="{palette['border']}" stroke-width="3"/>
<rect x="9" y="9" width="782" height="432" rx="16" fill="none" stroke="{palette['border_glow']}" stroke-opacity=".4"/>
</svg>'''
    crest=f'''<svg xmlns="http://www.w3.org/2000/svg" width="256" height="96" viewBox="0 0 256 96">
<path d="M28 65L38 22L78 50L128 10L178 50L218 22L228 65Z" fill="{palette['bg_panel_hi']}" stroke="{palette['accent_warning']}" stroke-width="3"/>
<path d="M40 73H216" stroke="{palette['border_glow']}" stroke-width="3"/>
</svg>'''
    header=f'''<svg xmlns="http://www.w3.org/2000/svg" width="480" height="44" viewBox="0 0 480 44">
<rect x="1" y="1" width="478" height="42" rx="6" fill="{palette['bg_panel']}" stroke="{palette['border']}"/>
<path d="M8 40H472" stroke="{palette['border_glow']}" stroke-width="2"/>
</svg>'''
    for name,data in (("texture.svg",texture),("bezel.svg",bezel),("crest.svg",crest),("panel_header.svg",header)):
        (SKIN/"svg"/name).write_text(data,encoding="utf-8")
    # Each independent auxiliary window receives its own static frame asset.
    windows=("autochords","fx_edit","skins_gallery","import_guitar","settings",
             "auto_discover_setup_only","curator_setup_only","about_setup_only")
    for name in windows:
        badge = "SETUP-ONLY" if name.endswith("_setup_only") else "NEON STAGE"
        content=f'''<svg xmlns="http://www.w3.org/2000/svg" width="720" height="480" viewBox="0 0 720 480">
<rect x="2" y="2" width="716" height="476" rx="12" fill="{palette['bg_panel']}" stroke="{palette['border']}" stroke-width="3"/>
<rect x="8" y="8" width="704" height="40" rx="6" fill="{palette['bg_panel_hi']}" stroke="{palette['border']}"/>
<path d="M12 47H708" stroke="{palette['border_glow']}" stroke-width="2"/>
<text x="20" y="33" fill="{palette['text_primary']}" font-family="sans-serif" font-size="16">{name.replace('_',' ').upper()}</text>
<text x="560" y="33" fill="{palette['text_secondary']}" font-family="sans-serif" font-size="11">{badge}</text>
<rect x="12" y="58" width="696" height="408" rx="6" fill="{palette['bg_deep']}" stroke="{palette['border']}"/>
</svg>'''
        (WINDOWS/f"window_{name}.svg").write_text(content,encoding="utf-8")

def generate_background():
    w,h=1600,920
    im=Image.new("RGBA",(w,h),rgba("bg_deep"))
    d=ImageDraw.Draw(im)
    top=rgb("bg_deep");bottom=rgb("bg_panel")
    for y in range(h):
        t=y/(h-1)
        col=tuple(int(top[i]*(1-t)+bottom[i]*t*.34) for i in range(3))+(255,)
        d.line((0,y,w,y),fill=col)
    for x in range(0,w,64):
        d.line((x,0,x,h),fill=rgba("border")[:3]+(18,),width=1)
    for y in range(0,h,64):
        d.line((0,y,w,y),fill=rgba("border")[:3]+(18,),width=1)
    d.rectangle((12,12,w-13,h-13),outline=rgba("border"),width=2)
    d.line((28,82,w-28,82),fill=rgba("border_glow"),width=2)
    # Restrained corner-stage accents are token-derived only.
    d.arc((-220,-200,420,420),5,95,fill=rgba("accent_primary"),width=5)
    d.arc((w-420,h-420,w+220,h+220),185,275,fill=rgba("accent_secondary"),width=5)
    save_png8(im,ASSETS/"background.png")

def make_contact_sheet():
    names=("knobs_main.png","knobs_amp.png","fx_tiles.png","toggles.png","sliders.png",
           "keys_white.png","keys_black.png","meters.png","window_chrome.png","icons.png",
           "panel_header.png","lcd_panel.png","gallery_tile.png","setup_only_badge.png")
    thumbs=[]
    for name in names:
        p=ATLAS/name
        if not p.is_file(): continue
        with Image.open(p) as src:
            thumb=src.convert("RGBA")
            thumb.thumbnail((280,150),Image.Resampling.LANCZOS)
        thumbs.append((name,thumb.copy()))
    cellw,cellh=320,190;cols=3;rows=math.ceil(len(thumbs)/cols)
    sheet=Image.new("RGBA",(cols*cellw,rows*cellh),rgba("bg_deep"))
    d=ImageDraw.Draw(sheet)
    for i,(name,thumb) in enumerate(thumbs):
        x=(i%cols)*cellw;y=(i//cols)*cellh
        d.text((x+10,y+8),name,font=font(),fill=rgba("text_primary"))
        sheet.alpha_composite(thumb,(x+10,y+28))
        d.rectangle((x+4,y+4,x+cellw-5,y+cellh-5),outline=rgba("border"),width=1)
    save_png8(sheet,SHEETS/"NEON_STAGE_ATLAS_CONTACT_SHEET.png")

def hash_file(path):
    digest=hashlib.sha256()
    with open(path,"rb") as f:
        for chunk in iter(lambda:f.read(1024*1024),b""):
            digest.update(chunk)
    return digest.hexdigest()

def generate():
    ensure_dirs()
    source=json.loads((SOURCE/"theme.json").read_text(encoding="utf-8"))
    if source.get("id")!="neon-stage-default-skin-1" or source.get("index")!=52:
        raise RuntimeError("Refusing to generate against a non-canonical theme identity.")
    if source.get("palette")!=TOKENS:
        raise RuntimeError("Canonical Skin_052 palette differs from REV ULTRA 3.0 token registry.")
    checks=(("text_primary","bg_deep",7),("text_secondary","bg_deep",4.5),
            ("text_disabled","bg_deep",3),("lcd_text","lcd_bg",7),
            ("accent_primary","bg_panel",3))
    for fg,bg,minimum in checks:
        ratio=contrast(TOKENS[fg],TOKENS[bg])
        if ratio<minimum: raise RuntimeError(f"G1 FAIL {fg}/{bg}: {ratio:.2f} < {minimum}")
    # Reference-derived preview/hero: no invented UI, real people or third-party brands.
    if not REF.is_file(): raise FileNotFoundError(f"Approved reference missing: {REF.name}")
    with Image.open(REF) as opened: ref=opened.convert("RGBA")
    for size,name in (((360,240),"preview.png"),((800,450),"hero.png")):
        fitted=ImageOps.contain(ref,size,method=Image.Resampling.LANCZOS)
        canvas=Image.new("RGBA",size,rgba("bg_deep"))
        canvas.alpha_composite(fitted,((size[0]-fitted.width)//2,(size[1]-fitted.height)//2))
        save_png8(canvas,ASSETS/name)
    with Image.open(ASSETS/"hero.png") as hero:
        hero.convert("RGB").save(ASSETS/"hero.webp",format="WEBP",quality=92,method=6)
    generate_background()
    generate_knob_atlas("knobs_main",[f"knob{i:02d}" for i in range(1,10)],128)
    generate_knob_atlas("knobs_amp",["amp_gain","amp_bass","amp_treble"],96)
    generate_fx();generate_toggles();generate_sliders();generate_keys();generate_meters()
    generate_window_chrome();generate_panels();generate_icons();svg_assets()
    make_contact_sheet()
    # Complete visual manifest: extra assets are additive; runtime editor still owns live layout.
    theme=json.loads((SOURCE/"theme.json").read_text(encoding="utf-8"))
    theme["assets"].update({
        "background":"assets/background.png","heroWebp":"assets/hero.webp",
        "panelHeader":"svg/panel_header.svg","atlasDirectory":"assets/atlas",
        "windowFrames":"assets/windows","license":"LICENSE.txt","selfCheck":"SELF_CHECK.txt"
    })
    theme["designStatus"]="STAGING_FOR_USER_VISUAL_APPROVAL"
    theme["notes"]=[
        "Generated deterministically by tools/generate_neon_stage_rev_ultra.py from the approved NEON STAGE reference set and 21 canonical palette tokens.",
        "Staging only. Runtime registry/default and JUCE resources are not modified by this generator.",
        "The atlas assets are generated deliverables; runtime sprite wiring is not implied.",
        "Auto Discover, Curator, About and unimplemented AutoChords generation remain SETUP-ONLY/deferred and do not simulate activity.",
        "No APVTS IDs, parameter ranges, DSP, audio callback, sampler, MIDI routing, presets or processor state were changed."
    ]
    write_json(SKIN/"theme.json",theme)
    controls=theme.get("controls",{})
    entries={}
    for cid,spec in controls.items():
        g=spec.get("grid",{})
        entries[cid]={
            "label":cid.replace("_"," ").replace("knob","KNOB ").upper(),
            "type":"rotary" if cid.startswith("knob") else "toggle" if cid.startswith("fx_switch") else "panel",
            "col":g.get("x"),"row":g.get("y"),"colSpan":g.get("w"),"rowSpan":g.get("h"),
            "apvtsBinding":None,"bindingStatus":"UNVERIFIED_FROM_RUNTIME_SOURCE"
        }
    write_json(SKIN/"control_map.json",{
        "schemaVersion":"1.0.0","grid":{"columns":12,"rows":16},
        "controlCount":len(entries),"controls":entries,
        "note":"APVTS bindings are deliberately null until each is reconciled against PluginProcessor.cpp and the actual editor attachments."
    })
    write_json(OUT_ROOT/"theme_pack_manifest.json",{
        "schemaVersion":"1.0.0","product":"POP PUNK GUITAR ULTIMATE",
        "status":"STAGING_FOR_USER_VISUAL_APPROVAL","sourceRegistryTotal":102,
        "startIndex":52,"endIndex":52,"totalSkins":1,
        "themes":[{"id":"neon-stage-default-skin-1","index":52,"name":"NEON STAGE",
                   "family":theme["family"],"layout":theme["layout"],
                   "path":"Skin_052/theme.json","preview":"Skin_052/assets/preview.png",
                   "hero":"Skin_052/assets/hero.png"}],
        "note":"Existing canonical Skin_052 at index 52. No new registry entry or renumbering."
    })
    # Deterministic license/provenance ledger; excludes its own hash and SELF_CHECK to avoid recursion.
    files=sorted(p for p in SKIN.rglob("*") if p.is_file() and p.name not in ("LICENSE.txt","SELF_CHECK.txt"))
    license_lines=[
        "NEON STAGE — ASSET PROVENANCE AND SHA-256",
        "Product: POP PUNK GUITAR ULTIMATE 4.0",
        "Skin ID: neon-stage-default-skin-1 | Registry index: 52 | Folder: Skin_052",
        "Generator: tools/generate_neon_stage_rev_ultra.py (deterministic procedural vector/raster drawing)",
        "Palette: exactly the 21 tokens in SKIN/Skin_052/theme.json; no free-form palette colours.",
        "Preview/hero source: user-approved NEON STAGE reference mockup 01; no synthetic plugin UI or real-person imagery.",
        "All generated sprite, SVG and background assets are original project-generated graphics.",
        "No third-party fonts, photographs, artist logos, commercial sample content or external SVG resources are embedded.",
        "Hash scope: all package files except LICENSE.txt and SELF_CHECK.txt (self-referential hashes are intentionally excluded).",
        ""
    ]
    for p in files: license_lines.append(f"{hash_file(p)}  {p.relative_to(SKIN).as_posix()}")
    (SKIN/"LICENSE.txt").write_text("\n".join(license_lines)+"\n",encoding="utf-8")
    # SELF_CHECK is finalized by the staging validator after all gates pass.
    (SKIN/"SELF_CHECK.txt").write_text(
        "NEON STAGE REV ULTRA 3.0 SELF-CHECK\nSTATUS: PENDING_VALIDATOR\nG1-G4: NOT YET RUN\n",encoding="utf-8")
    print(f"Generated {sum(1 for p in SKIN.rglob('*') if p.is_file())} files in {SKIN}")
    print(f"Generated {len(entries)} control-map entries and atlas contact sheet.")
    print("Runtime registry, JUCE resources and audio/MIDI code were not modified.")

if __name__=="__main__":
    generate()
