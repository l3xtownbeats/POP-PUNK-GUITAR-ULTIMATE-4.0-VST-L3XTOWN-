"""Procedural asset builder for PPGU NEON STAGE Skin_052.
Writes staging outputs only; never mutates the runtime registry or JUCE resources.
"""
from __future__ import annotations
import json, math, os, zipfile, hashlib, re, xml.etree.ElementTree as ET
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont, ImageFilter, ImageOps

ROOT = Path(__file__).resolve().parent
SOURCE_THEME = ROOT / "SKIN" / "Skin_052" / "theme.json"
OUT = ROOT / "dist" / "neon_stage_staging"
SKIN = OUT / "Skin_052"
ASSETS = SKIN / "assets"
def load_source_palette():
    """Use Skin_052/theme.json as the sole palette source; never fork hex values."""
    if not SOURCE_THEME.is_file():
        raise FileNotFoundError(f"Canonical source theme missing: {SOURCE_THEME}")
    source = json.loads(SOURCE_THEME.read_text(encoding="utf-8"))
    palette = source.get("palette")
    if not isinstance(palette, dict) or len(palette) != 21:
        raise RuntimeError("Canonical source theme must provide exactly 21 palette tokens.")
    if any(not isinstance(value, str) or not value.startswith("#")
           or len(value) != (9 if key == "overlay_scrim" else 7)
           for key, value in palette.items()):
        raise RuntimeError("Canonical source theme contains an invalid palette colour.")
    return palette


SOURCE_PALETTE = load_source_palette()
TOKENS = tuple(SOURCE_PALETTE)
# Drawing aliases map exclusively to the 21 canonical REV ULTRA 3.0 tokens.
PALETTE = {
    "window_bg": SOURCE_PALETTE["bg_deep"],
    "panel": SOURCE_PALETTE["bg_panel"],
    "panel_elevated": SOURCE_PALETTE["bg_panel_hi"],
    "control_bg": SOURCE_PALETTE["knob_body"],
    "control_face": SOURCE_PALETTE["knob_body"],
    "control_border": SOURCE_PALETTE["border"],
    "text_primary": SOURCE_PALETTE["text_primary"],
    "text_secondary": SOURCE_PALETTE["text_secondary"],
    "text_disabled": SOURCE_PALETTE["text_disabled"],
    "accent": SOURCE_PALETTE["accent_primary"],
    "accent_glow": SOURCE_PALETTE["border_glow"],
    "danger": SOURCE_PALETTE["accent_danger"],
    "warning": SOURCE_PALETTE["accent_warning"],
    "success": SOURCE_PALETTE["accent_success"],
    "lcd_bg": SOURCE_PALETTE["lcd_bg"],
    "lcd_text": SOURCE_PALETTE["lcd_text"],
    "led_on": SOURCE_PALETTE["led_on"],
    "led_off": SOURCE_PALETTE["led_off"],
    "shadow": SOURCE_PALETTE["bg_deep"],
    "highlight": SOURCE_PALETTE["focus_ring"],
    "lowlight": SOURCE_PALETTE["led_off"],
}
REQUIRED = (
    "preset_browser","mode_selector",*(f"knob{i:02d}" for i in range(1,10)),
    *(f"fx_switch{i:02d}" for i in range(1,8)),"midi_keyboard","LCD_display",
    "LED_indicator","SKIN_button","Import_Guitar","Auto_Discover","Curator",
    "Settings","About","sync_selector"
)
FX_NAMES = ("compressor","eq","distortion","chorus","delay","reverb","cabinet")

def rgb(value):
    return tuple(int(value[i:i+2],16) for i in (1,3,5))

def luminance(hex_color):
    vals=[]
    for c in rgb(hex_color):
        x=c/255
        vals.append(x/12.92 if x<=0.04045 else ((x+0.055)/1.055)**2.4)
    return .2126*vals[0]+.7152*vals[1]+.0722*vals[2]

def contrast(a,b):
    x,y=sorted((luminance(a),luminance(b)),reverse=True)
    return (x+.05)/(y+.05)

def ensure_dirs():
    for p in (ASSETS/"knob",ASSETS/"buttons",ASSETS/"fx",ASSETS/"slider",
              ASSETS/"led",SKIN/"svg"):
        p.mkdir(parents=True,exist_ok=True)

def font(size=14,bold=False):
    candidates = ["C:/Windows/Fonts/segoeuib.ttf" if bold else "C:/Windows/Fonts/segoeui.ttf",
                  "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf" if bold else
                  "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"]
    for p in candidates:
        try: return ImageFont.truetype(p,size)
        except OSError: pass
    return ImageFont.load_default()

def glow_layer(size, center, radius, color, blur=12):
    layer=Image.new("RGBA",size,(0,0,0,0)); d=ImageDraw.Draw(layer)
    x,y=center
    d.ellipse((x-radius,y-radius,x+radius,y+radius),fill=(*rgb(color),115))
    return layer.filter(ImageFilter.GaussianBlur(blur))

def render_knob(size, frame):
    im=Image.new("RGBA",(size,size),(0,0,0,0))
    c=size//2; r=int(size*.37)
    im=Image.alpha_composite(im,glow_layer(im.size,(c,c),int(size*.39),PALETTE["accent"],max(3,size//18)))
    d=ImageDraw.Draw(im)
    d.ellipse((c-r,c-r,c+r,c+r),fill=rgb(PALETTE["shadow"])+(255,),
              outline=rgb(PALETTE["control_border"])+(255,),width=max(1,size//64))
    d.ellipse((c-r+size*.045,c-r+size*.045,c+r-size*.045,c+r-size*.045),
              fill=rgb(PALETTE["control_face"])+(255,),
              outline=rgb(PALETTE["highlight"])+(100,),width=max(1,size//96))
    # 270-degree sweep, -135..+135, with pointer rendered per frame.
    angle=math.radians(-135.0 + 270.0*frame/120.0)
    inner=int(size*.08); outer=int(size*.28)
    x1=c+math.cos(angle)*inner; y1=c+math.sin(angle)*inner
    x2=c+math.cos(angle)*outer; y2=c+math.sin(angle)*outer
    d.line((x1,y1,x2,y2),fill=rgb(PALETTE["accent_glow"])+(255,),width=max(2,size//28))
    d.ellipse((c-size*.045,c-size*.045,c+size*.045,c+size*.045),
              fill=rgb(PALETTE["highlight"])+(255,))
    return im

def vertical_strip(size,path):
    strip=Image.new("RGBA",(size,size*121),(0,0,0,0))
    for i in range(121): strip.alpha_composite(render_knob(size,i),(0,i*size))
    _indexed_png(strip,path)

def step_palette():
    ensure_dirs()
    checks={
      "text_primary/window_bg":(PALETTE["text_primary"],PALETTE["window_bg"],7.0),
      "text_secondary/window_bg":(PALETTE["text_secondary"],PALETTE["window_bg"],4.5),
      "text_disabled/window_bg":(PALETTE["text_disabled"],PALETTE["window_bg"],3.0),
      "lcd_text/lcd_bg":(PALETTE["lcd_text"],PALETTE["lcd_bg"],7.0),
      "accent/panel":(PALETTE["accent"],PALETTE["panel"],3.0)
    }
    report={}
    for name,(fg,bg,minimum) in checks.items():
        ratio=contrast(fg,bg); report[name]={"ratio":round(ratio,2),"minimum":minimum,"pass":ratio>=minimum}
    if not all(v["pass"] for v in report.values()):
        raise RuntimeError("Palette contrast gate failed: "+json.dumps(report))
    (OUT/"palette_report.json").write_text(json.dumps(report,indent=2),encoding="utf-8")
    print("PASS palette contrast:",json.dumps(report))

def step_knobs():
    ensure_dirs()
    vertical_strip(96,ASSETS/"knob"/"knob_filmstrip.png")
    vertical_strip(192,ASSETS/"knob"/"knob_filmstrip_2x.png")
    (ASSETS/"knob"/"knob_body.svg").write_text(svg_knob_body(),encoding="utf-8")
    (ASSETS/"knob"/"knob_pointer.svg").write_text(svg_knob_pointer(),encoding="utf-8")
    print("Generated 121 frames at 96x96 and 192x192.")

def step_ui():
    ensure_dirs()
    states=("normal","hover","down","on","off","disabled")
    w,h=180,58
    atlas=Image.new("RGBA",(w*len(states),h),(0,0,0,0))
    for i,state in enumerate(states):
        x=i*w; d=ImageDraw.Draw(atlas)
        fill={"normal":PALETTE["control_face"],"hover":PALETTE["panel_elevated"],
              "down":PALETTE["lowlight"],"on":PALETTE["accent"],"off":PALETTE["control_bg"],
              "disabled":PALETTE["panel"]}[state]
        outline=PALETTE["accent"] if state in ("hover","on") else PALETTE["control_border"]
        d.rounded_rectangle((x+5,5,x+w-6,h-6),radius=12,fill=fill,
                            outline=outline,width=2)
        if state=="on":
            d.line((x+24,h//2,x+36,h//2),fill=PALETTE["window_bg"],width=3)
    _indexed_png(atlas,ASSETS/"buttons"/"btn_states.png")
    tile_w,tile_h=220,72
    fx=Image.new("RGBA",(tile_w*len(FX_NAMES),tile_h*2),(0,0,0,0))
    for row in range(2):
        for col,name in enumerate(FX_NAMES):
            x,y=col*tile_w,row*tile_h
            d=ImageDraw.Draw(fx)
            on=row==1
            fill=PALETTE["panel_elevated"] if on else PALETTE["control_bg"]
            edge=PALETTE["accent"] if on else PALETTE["control_border"]
            d.rounded_rectangle((x+5,y+5,x+tile_w-6,y+tile_h-6),radius=10,fill=fill,outline=edge,width=2)
            d.text((x+18,y+25),name.upper(),font=font(15,True),
                   fill=PALETTE["text_primary"] if on else PALETTE["text_secondary"])
            d.ellipse((x+tile_w-34,y+tile_h//2-6,x+tile_w-22,y+tile_h//2+6),
                      fill=PALETTE["led_on"] if on else PALETTE["led_off"])
    _indexed_png(fx,ASSETS/"fx"/"fx_tiles.png")
    track=Image.new("RGBA",(360,28),(0,0,0,0)); d=ImageDraw.Draw(track)
    d.rounded_rectangle((4,10,356,18),radius=4,fill=PALETTE["control_bg"],outline=PALETTE["control_border"])
    d.rounded_rectangle((4,10,210,18),radius=4,fill=PALETTE["accent"])
    _indexed_png(track,ASSETS/"slider"/"slider_track.png")
    cap=Image.new("RGBA",(32,40),(0,0,0,0)); d=ImageDraw.Draw(cap)
    d.ellipse((4,4,28,28),fill=PALETTE["control_face"],outline=PALETTE["accent"],width=2)
    d.line((16,9,16,23),fill=PALETTE["text_primary"],width=2)
    _indexed_png(cap,ASSETS/"slider"/"slider_cap.png")
    led=Image.new("RGBA",(64,32),(0,0,0,0)); d=ImageDraw.Draw(led)
    d.ellipse((3,3,28,28),fill=PALETTE["led_off"],outline=PALETTE["control_border"],width=2)
    d.ellipse((35,3,60,28),fill=PALETTE["led_on"],outline=PALETTE["accent_glow"],width=2)
    _indexed_png(led,ASSETS/"led"/"led_states.png")
    print("Generated legacy UI elements; creating the complete indexed sprite contract.")
    step_atlases()



def _indexed_png(image, path):
    """Save deterministic indexed PNG-8; preserve alpha when a sprite needs transparency."""
    if image.mode != "P":
        image = image.quantize(colors=256, method=Image.Quantize.FASTOCTREE,
                               dither=Image.Dither.NONE)
    image.save(path, "PNG", optimize=True)


def _atlas(name, frames, columns):
    """Write a deterministic sprite atlas and its frame-coordinate JSON index."""
    if not frames:
        raise ValueError(f"Atlas {name} has no frames")
    cell_w, cell_h = frames[0][2].size
    if any(frame[2].size != (cell_w, cell_h) for frame in frames):
        raise ValueError(f"Atlas {name} contains mixed frame dimensions")
    rows = (len(frames) + columns - 1) // columns
    sheet = Image.new("RGB", (columns * cell_w, rows * cell_h), rgb(PALETTE["panel"]))
    index = {"schemaVersion":"1.0.0", "atlas":name, "cellWidth":cell_w,
             "cellHeight":cell_h, "columns":columns, "rows":rows, "frameCount":len(frames),
             "frames":[]}
    for i, (frame_name, state, frame) in enumerate(frames):
        x, y = (i % columns) * cell_w, (i // columns) * cell_h
        sheet.paste(frame.convert("RGB"), (x, y))
        index["frames"].append({"name":frame_name,"state":state,"x":x,"y":y,
                                "w":cell_w,"h":cell_h})
    atlas_dir = ASSETS / "atlas"
    atlas_dir.mkdir(parents=True, exist_ok=True)
    _indexed_png(sheet, atlas_dir / f"{name}.png")
    (atlas_dir / f"{name}.json").write_text(
        json.dumps(index, indent=2, ensure_ascii=False) + "\n", encoding="utf-8"
    )
    return atlas_dir / f"{name}.png"


def _state_knob(size, angle_index, state):
    frame = render_knob(size, angle_index * 10).convert("RGBA")
    overlay = Image.new("RGBA", frame.size, (0,0,0,0))
    d = ImageDraw.Draw(overlay)
    if state == "highlight":
        d.ellipse((2,2,size-3,size-3), outline=(*rgb(SOURCE_PALETTE["focus_ring"]),210),
                  width=max(2,size//32))
    elif state == "disabled":
        d.rectangle((0,0,size,size), fill=(*rgb(SOURCE_PALETTE["bg_deep"]),135))
    return Image.alpha_composite(frame, overlay).convert("RGB")


def _fx_glyph(draw, box, module, color):
    x0,y0,x1,y1=box
    cx,cy=(x0+x1)//2,(y0+y1)//2
    w=max(2,(x1-x0)//14)
    if module=="compressor":
        pts=[(x0+4,cy),(x0+14,cy),(x0+21,y0+5),(x0+29,y1-5),(x1-4,cy)]
        draw.line(pts,fill=color,width=w)
    elif module=="eq":
        for i,h in enumerate((8,18,12,25,15)):
            x=x0+5+i*7
            draw.line((x,cy-h,x,cy+h),fill=color,width=w)
    elif module=="distortion":
        draw.line([(x0+3,y1-4),(x0+12,y0+5),(x0+20,y1-4),(x0+28,y0+5),(x1-3,y1-4)],
                  fill=color,width=w)
    elif module=="chorus":
        draw.arc((x0+2,y0+5,x1-2,y1-5),190,350,fill=color,width=w)
        draw.arc((x0+2,y0+5,x1-2,y1-5),10,170,fill=color,width=w)
    elif module=="delay":
        draw.ellipse((cx-11,cy-11,cx+11,cy+11),outline=color,width=w)
        draw.line((cx,cy,cx,cy-7),fill=color,width=w)
        draw.line((cx,cy,cx+7,cy+4),fill=color,width=w)
    elif module=="reverb":
        for r in (5,11,17):
            draw.ellipse((cx-r,cy-r,cx+r,cy+r),outline=color,width=max(1,w-1))
    else:
        draw.ellipse((cx-13,cy-13,cx+13,cy+13),outline=color,width=w)
        draw.line((cx-17,cy,cx+17,cy),fill=color,width=w)
        draw.line((cx,cy-17,cx,cy+17),fill=color,width=w)


def _make_fx_frame(module, state, size=(140,84)):
    accents={
        "compressor":SOURCE_PALETTE["accent_primary"], "eq":SOURCE_PALETTE["border_glow"],
        "distortion":SOURCE_PALETTE["accent_warning"], "chorus":SOURCE_PALETTE["focus_ring"],
        "delay":SOURCE_PALETTE["accent_secondary"], "reverb":SOURCE_PALETTE["accent_success"],
        "cabinet_ir":SOURCE_PALETTE["accent_warning"]
    }
    accent=accents[module]
    im=Image.new("RGB",size,rgb(PALETTE["panel"]))
    d=ImageDraw.Draw(im)
    enabled=state=="on"
    edge=accent if enabled else rgb(PALETTE["border"])
    if state=="bypass": edge=rgb(SOURCE_PALETTE["accent_warning"])
    if state=="disabled": edge=rgb(SOURCE_PALETTE["text_disabled"])
    d.rounded_rectangle((2,2,size[0]-3,size[1]-3),radius=9,
                        fill=rgb(PALETTE["panel_elevated"] if enabled else PALETTE["control_bg"]),
                        outline=edge,width=2)
    d.rounded_rectangle((8,6,size[0]-9,11),radius=2,
                        fill=rgb(accent if enabled else PALETTE["led_off"]))
    _fx_glyph(d,(12,19,48,59),module,rgb(accent if state!="disabled" else PALETTE["text_disabled"]))
    d.text((55,32),module.replace("_"," ").upper(),font=font(11,True),
           fill=rgb(PALETTE["text_primary"] if state!="disabled" else PALETTE["text_disabled"]))
    if state=="bypass":
        d.rounded_rectangle((size[0]-49,17,size[0]-7,31),radius=4,
                            fill=rgb(SOURCE_PALETTE["accent_warning"]))
        d.text((size[0]-45,18),"BYP",font=font(8,True),fill=rgb(PALETTE["window_bg"]))
    return im


def _make_toggle(style, state):
    im=Image.new("RGB",(72,32),rgb(PALETTE["panel"]))
    d=ImageDraw.Draw(im)
    on=state=="on"
    bypass=state=="bypass"
    accent=SOURCE_PALETTE["accent_warning"] if bypass else (
        SOURCE_PALETTE["accent_primary"] if on else SOURCE_PALETTE["text_disabled"])
    if style=="neon_pill":
        d.rounded_rectangle((2,2,69,29),radius=13,fill=rgb(PALETTE["control_bg"]),
                            outline=rgb(accent),width=2)
        x=51 if on else 20
        d.ellipse((x-9,7,x+9,25),fill=rgb(accent))
    elif style=="flat":
        d.rounded_rectangle((2,2,69,29),radius=4,fill=rgb(PALETTE["panel_elevated"]),
                            outline=rgb(PALETTE["border"]),width=1)
        d.rectangle((5,6,35 if on else 17,25),fill=rgb(accent))
    else:
        d.rounded_rectangle((2,2,69,29),radius=7,fill=rgb(PALETTE["control_face"]),
                            outline=rgb(accent),width=2)
        d.line((10,16,62,16),fill=rgb(accent),width=2)
    label={"on":"ON","off":"OFF","bypass":"BYP"}[state]
    d.text((25,10),label,font=font(8,True),fill=rgb(PALETTE["text_primary"]))
    return im


def _make_icon(name):
    im=Image.new("RGB",(32,32),rgb(PALETTE["panel"]))
    d=ImageDraw.Draw(im)
    c=rgb(SOURCE_PALETTE["focus_ring"])
    w=2
    if name=="folder":
        d.rounded_rectangle((4,10,28,25),radius=3,outline=c,width=w)
        d.line((4,11,12,11,15,7,23,7),fill=c,width=w)
    elif name=="star":
        pts=[(16,3),(19,12),(29,12),(21,18),(24,28),(16,22),(8,28),(11,18),(3,12),(13,12)]
        d.polygon(pts,outline=c)
    elif name=="search":
        d.ellipse((4,4,21,21),outline=c,width=w); d.line((19,19,28,28),fill=c,width=3)
    elif name=="close":
        d.line((7,7,25,25),fill=c,width=3); d.line((25,7,7,25),fill=c,width=3)
    elif name=="palette":
        d.ellipse((3,3,29,29),outline=c,width=w)
        for x,y in ((10,10),(20,9),(12,20),(22,20)): d.ellipse((x-2,y-2,x+2,y+2),fill=c)
    elif name=="guitar":
        d.ellipse((6,15,22,29),outline=c,width=w); d.ellipse((12,10,23,21),outline=c,width=w)
        d.line((18,13,27,4),fill=c,width=3); d.line((17,17,26,8),fill=c,width=1)
    elif name=="radar":
        d.ellipse((3,3,29,29),outline=c,width=w); d.arc((8,8,24,24),200,330,fill=c,width=w)
        d.line((16,16,26,7),fill=c,width=2)
    elif name=="curator":
        d.ellipse((11,4,21,14),outline=c,width=w); d.arc((5,12,27,30),180,360,fill=c,width=w)
        d.polygon([(23,4),(25,9),(30,9),(26,12),(28,17),(23,14),(19,17),(20,12),(16,9),(21,9)],outline=c)
    elif name=="settings":
        d.ellipse((8,8,24,24),outline=c,width=3); d.ellipse((13,13,19,19),outline=c,width=2)
        for coords in ((16,2,16,8),(16,24,16,30),(2,16,8,16),(24,16,30,16)):
            d.line(coords,fill=c,width=2)
    elif name in ("i","info"):
        d.ellipse((4,4,28,28),outline=c,width=w)
        d.text((12,5),"i" if name=="i" else "?",font=font(19,True),fill=c)
    elif name=="note":
        d.line((20,5,20,23),fill=c,width=3); d.line((20,5,28,3),fill=c,width=3)
        d.ellipse((8,20,19,28),outline=c,width=2)
    elif name=="grid":
        for y in (5,14,23):
            for x in (5,14,23): d.rectangle((x,y,x+5,y+5),outline=c,width=1)
    elif name=="group":
        for x in (7,16,25): d.ellipse((x-3,5,x+3,11),outline=c,width=1)
        d.arc((2,10,13,27),180,360,fill=c,width=2); d.arc((10,10,22,27),180,360,fill=c,width=2)
        d.arc((19,10,30,27),180,360,fill=c,width=2)
    elif name=="play":
        d.polygon([(9,5),(26,16),(9,27)],fill=c)
    elif name=="pause":
        d.rectangle((8,6,13,26),fill=c); d.rectangle((19,6,24,26),fill=c)
    elif name=="stop":
        d.rounded_rectangle((7,7,25,25),radius=2,fill=c)
    elif name=="loop":
        d.arc((5,5,27,27),35,300,fill=c,width=3); d.polygon([(23,3),(29,8),(21,10)],fill=c)
    elif name=="dice":
        d.rounded_rectangle((5,5,27,27),radius=4,outline=c,width=2)
        for x,y in ((10,10),(22,10),(16,16),(10,22),(22,22)): d.ellipse((x-1,y-1,x+1,y+1),fill=c)
    elif name=="crown":
        d.polygon([(4,23),(3,9),(10,15),(16,5),(22,15),(29,9),(28,23)],outline=c)
        d.line((4,27,28,27),fill=c,width=2)
    return im


def _make_aux_window(name):
    """Render a static auxiliary-window reference shell without simulating live activity."""
    w,h=720,480
    im=Image.new("RGB",(w,h),rgb(PALETTE["panel"]))
    d=ImageDraw.Draw(im)
    d.rounded_rectangle((2,2,w-3,h-3),radius=12,fill=rgb(PALETTE["panel"]),
                        outline=rgb(PALETTE["border"]),width=3)
    d.rounded_rectangle((9,9,w-10,52),radius=6,fill=rgb(PALETTE["panel_elevated"]),
                        outline=rgb(PALETTE["border"]),width=1)
    d.line((12,50,w-12,50),fill=rgb(SOURCE_PALETTE["border_glow"]),width=2)
    title=name.replace("_"," ").upper()
    d.text((22,24),title,font=font(15,True),fill=rgb(PALETTE["text_primary"]))
    d.rectangle((w-68,20,w-52,36),outline=rgb(PALETTE["text_secondary"]),width=2)
    d.line((w-36,20,w-20,36),fill=rgb(PALETTE["text_secondary"]),width=2)
    d.line((w-20,20,w-36,36),fill=rgb(PALETTE["text_secondary"]),width=2)
    d.rounded_rectangle((12,62,w-13,h-13),radius=6,fill=rgb(PALETTE["window_bg"]),
                        outline=rgb(PALETTE["border"]),width=1)
    if name.endswith("_setup_only"):
        d.rounded_rectangle((24,76,156,104),radius=6,fill=rgb(PALETTE["panel_elevated"]),
                            outline=rgb(PALETTE["text_secondary"]),width=1)
        d.text((36,85),"SETUP-ONLY",font=font(11,True),fill=rgb(PALETTE["text_secondary"]))
        d.text((24,126),"Setup only — not connected",font=font(12),
               fill=rgb(PALETTE["text_secondary"]))
        return im
    if name=="autochords":
        d.text((26,76),"CHORD STYLES",font=font(11,True),fill=rgb(PALETTE["text_secondary"]))
        d.text((204,76),"CHORD PROGRESSION",font=font(11,True),fill=rgb(PALETTE["text_secondary"]))
        d.text((510,76),"PATTERN INFO",font=font(11,True),fill=rgb(PALETTE["text_secondary"]))
        for y in (104,142,180,218,256,294):
            d.rounded_rectangle((24,y,164,y+28),radius=4,fill=rgb(PALETTE["panel"]),
                                outline=rgb(PALETTE["border"]),width=1)
        for i,label in enumerate(("Am","F","C","G","Am","F","C","G")):
            x=194+(i%4)*76;y=108+(i//4)*68
            d.rounded_rectangle((x,y,x+66,y+48),radius=5,fill=rgb(PALETTE["panel_elevated"]),
                                outline=rgb(PALETTE["border"]),width=1)
            d.text((x+23,y+18),label,font=font(12,True),fill=rgb(PALETTE["text_primary"]))
        d.rounded_rectangle((510,104,684,332),radius=5,fill=rgb(PALETTE["panel"]),
                            outline=rgb(PALETTE["border"]),width=1)
        d.text((526,124),"PATTERN BROWSER",font=font(10,True),fill=rgb(PALETTE["text_secondary"]))
    elif name=="fx_edit":
        d.text((26,76),"FX MODULES",font=font(11,True),fill=rgb(PALETTE["text_secondary"]))
        for i in range(7):
            y=102+i*43
            d.rounded_rectangle((24,y,170,y+32),radius=4,fill=rgb(PALETTE["panel"]),
                                outline=rgb(PALETTE["border"]),width=1)
        d.rounded_rectangle((188,82,684,272),radius=5,fill=rgb(PALETTE["panel"]),
                            outline=rgb(PALETTE["border"]),width=1)
        d.line((206,230,260,210,316,222,374,160,430,176,488,128,548,142,662,104),
               fill=rgb(SOURCE_PALETTE["accent_primary"]),width=2)
        for i in range(6):
            x=202+i*78
            d.ellipse((x,304,x+48,352),fill=rgb(PALETTE["knob_body"]),
                      outline=rgb(PALETTE["border_glow"]),width=2)
    elif name=="skins_gallery":
        d.text((26,76),"SEARCH SKINS...",font=font(11,True),fill=rgb(PALETTE["text_secondary"]))
        for i in range(6):
            x=24+(i%3)*218;y=112+(i//3)*150
            d.rounded_rectangle((x,y,x+198,y+126),radius=6,fill=rgb(PALETTE["panel"]),
                                outline=rgb(PALETTE["border"]),width=1)
            if i==0:
                d.rectangle((x+8,y+8,x+190,y+86),fill=rgb(PALETTE["window_bg"]),
                            outline=rgb(PALETTE["border_glow"]),width=1)
                d.text((x+16,y+98),"NEON STAGE",font=font(11,True),fill=rgb(PALETTE["text_primary"]))
    elif name=="import_guitar":
        d.rounded_rectangle((24,88,432,332),radius=8,fill=rgb(PALETTE["panel"]),
                            outline=rgb(PALETTE["border_glow"]),width=2)
        d.text((80,178),"SFZ / WAV Files or Click to Browse",font=font(13,True),
               fill=rgb(PALETTE["text_primary"]))
        d.rounded_rectangle((450,88,684,332),radius=6,fill=rgb(PALETTE["panel"]),
                            outline=rgb(PALETTE["border"]),width=1)
        d.text((468,108),"LIBRARY INFO",font=font(11,True),fill=rgb(PALETTE["text_secondary"]))
    elif name=="settings":
        for i,label in enumerate(("GENERAL","AUDIO","MIDI","GUI")):
            x=24+i*164
            d.rounded_rectangle((x,84,x+150,118),radius=5,fill=rgb(PALETTE["panel_elevated"]),
                                outline=rgb(PALETTE["border"]),width=1)
            d.text((x+24,96),label,font=font(10,True),fill=rgb(PALETTE["text_primary"]))
        for i in range(4):
            y=144+i*54
            d.rectangle((32,y,48,y+16),outline=rgb(PALETTE["text_secondary"]),width=1)
            d.line((66,y+8,300,y+8),fill=rgb(PALETTE["border"]),width=2)
    return im


def step_atlases():
    """Build the full deterministic NEON STAGE sprite contract in staging only."""
    ensure_dirs()
    main=[]
    for control in [f"knob{i:02d}" for i in range(1,10)]:
        for state in ("base","highlight","disabled"):
            for angle in range(13):
                main.append((f"{control}_angle{angle:02d}_{state}",state,
                             _state_knob(128,angle,state)))
    _atlas("knobs_main",main,13)
    main2=[]
    for control in [f"knob{i:02d}" for i in range(1,10)]:
        for state in ("base","highlight","disabled"):
            for angle in range(13):
                main2.append((f"{control}_angle{angle:02d}_{state}",state,
                              _state_knob(256,angle,state)))
    _atlas("knobs_main_2x",main2,13)
    amp=[]
    for control in ("amp_gain","amp_bass","amp_treble"):
        for state in ("base","highlight","disabled"):
            for angle in range(13):
                amp.append((f"{control}_angle{angle:02d}_{state}",state,
                            _state_knob(96,angle,state)))
    _atlas("knobs_amp",amp,13)
    amp2=[]
    for control in ("amp_gain","amp_bass","amp_treble"):
        for state in ("base","highlight","disabled"):
            for angle in range(13):
                amp2.append((f"{control}_angle{angle:02d}_{state}",state,
                             _state_knob(192,angle,state)))
    _atlas("knobs_amp_2x",amp2,13)

    modules=("compressor","eq","distortion","chorus","delay","reverb","cabinet_ir")
    fx=[(f"{module}_{state}",state,_make_fx_frame(module,state))
        for module in modules for state in ("off","on","bypass","disabled")]
    _atlas("fx_tiles",fx,4)
    toggles=[(f"{style}_{state}",state,_make_toggle(style,state))
             for style in ("neon_pill","flat","metallic") for state in ("on","off","bypass")]
    _atlas("toggles",toggles,3)

    tracks=[]
    for name,accent in (("horizontal",SOURCE_PALETTE["accent_primary"]),
                        ("sync",SOURCE_PALETTE["accent_secondary"])):
        im=Image.new("RGB",(360,28),rgb(PALETTE["panel"])); d=ImageDraw.Draw(im)
        d.rounded_rectangle((4,9,356,19),radius=5,fill=rgb(PALETTE["control_bg"]),
                            outline=rgb(PALETTE["border"]),width=1)
        d.rounded_rectangle((4,9,210,19),radius=5,fill=rgb(accent))
        tracks.append((name,"normal",im))
    _atlas("slider_tracks",tracks,2)
    vtrack=Image.new("RGB",(28,180),rgb(PALETTE["panel"])); d=ImageDraw.Draw(vtrack)
    d.rounded_rectangle((9,4,19,176),radius=5,fill=rgb(PALETTE["control_bg"]),
                        outline=rgb(PALETTE["border"]),width=1)
    d.rounded_rectangle((9,65,19,176),radius=5,fill=rgb(SOURCE_PALETTE["accent_primary"]))
    _atlas("slider_vertical", [("vertical_track","normal",vtrack)],1)
    thumbs=[]
    for state in ("normal","hover","dragged"):
        im=Image.new("RGB",(32,40),rgb(PALETTE["panel"])); d=ImageDraw.Draw(im)
        accent=SOURCE_PALETTE["accent_primary"] if state!="normal" else SOURCE_PALETTE["border"]
        d.ellipse((4,4,28,28),fill=rgb(PALETTE["control_face"]),outline=rgb(accent),width=3)
        d.line((16,9,16,23),fill=rgb(PALETTE["text_primary"]),width=2)
        if state=="dragged": d.ellipse((12,12,20,20),outline=rgb(SOURCE_PALETTE["accent_secondary"]),width=2)
        thumbs.append((f"thumb_{state}",state,im))
    _atlas("slider_thumbs",thumbs,3)
    wheels=[]
    for name in ("pitch","mod"):
        im=Image.new("RGB",(40,180),rgb(PALETTE["panel"])); d=ImageDraw.Draw(im)
        accent=SOURCE_PALETTE["accent_primary"] if name=="pitch" else SOURCE_PALETTE["accent_secondary"]
        d.rounded_rectangle((12,3,28,177),radius=7,fill=rgb(PALETTE["control_bg"]),outline=rgb(PALETTE["border"]))
        d.rounded_rectangle((5,72,35,104),radius=7,fill=rgb(PALETTE["control_face"]),outline=rgb(accent),width=2)
        d.text((14,82),name[0].upper(),font=font(12,True),fill=rgb(PALETTE["text_primary"]))
        wheels.append((f"{name}_wheel","normal",im))
    _atlas("pitch_mod_wheels",wheels,2)

    white=[]; black=[]
    for state in ("normal","pressed","glow"):
        wi=Image.new("RGB",(46,160),rgb(PALETTE["panel"])); d=ImageDraw.Draw(wi)
        fill={"normal":SOURCE_PALETTE["text_primary"],"pressed":SOURCE_PALETTE["text_secondary"],
              "glow":SOURCE_PALETTE["lcd_text"]}[state]
        d.rounded_rectangle((1,1,44,158),radius=3,fill=rgb(fill),outline=rgb(PALETTE["border"]),width=1)
        if state=="glow": d.rectangle((2,145,43,157),fill=rgb(SOURCE_PALETTE["accent_primary"]))
        white.append((f"white_{state}",state,wi))
        bi=Image.new("RGB",(28,96),rgb(PALETTE["panel"])); d=ImageDraw.Draw(bi)
        bfill=PALETTE["control_bg"] if state!="pressed" else PALETTE["panel_elevated"]
        d.rounded_rectangle((1,1,26,94),radius=3,fill=rgb(bfill),outline=rgb(SOURCE_PALETTE["border_glow"] if state=="glow" else PALETTE["border"]),width=1)
        if state=="glow": d.line((3,90,24,90),fill=rgb(SOURCE_PALETTE["accent_primary"]),width=3)
        black.append((f"black_{state}",state,bi))
    _atlas("keys_white",white,3); _atlas("keys_black",black,3)

    meter_frames=[]
    meter_colors=(("cyan",SOURCE_PALETTE["accent_primary"]),("magenta",SOURCE_PALETTE["accent_secondary"]),
                  ("orange",SOURCE_PALETTE["accent_warning"]),("green",SOURCE_PALETTE["accent_success"]),
                  ("red",SOURCE_PALETTE["accent_danger"]))
    for name,color in meter_colors:
        for state in ("on","off"):
            im=Image.new("RGB",(24,12),rgb(PALETTE["panel"])); d=ImageDraw.Draw(im)
            d.rounded_rectangle((1,1,22,10),radius=4,
                                fill=rgb(color if state=="on" else PALETTE["led_off"]))
            meter_frames.append((f"{name}_{state}",state,im))
    _atlas("meters_led",meter_frames,2)
    columns=[]
    for name,color in (("gain_reduction",SOURCE_PALETTE["accent_primary"]),
                       ("output_level",SOURCE_PALETTE["accent_success"])):
        im=Image.new("RGB",(28,180),rgb(PALETTE["panel"])); d=ImageDraw.Draw(im)
        for i in range(12):
            y=174-i*14
            d.rounded_rectangle((3,y-10,24,y),radius=2,
                                fill=rgb(color if i<8 else PALETTE["led_off"]))
        columns.append((name,"normal",im))
    _atlas("meters_columns",columns,2)

    chrome=[]
    for name in ("close","maximize","restore"):
        for state in ("normal","hover"):
            im=Image.new("RGB",(40,40),rgb(PALETTE["panel"])); d=ImageDraw.Draw(im)
            color=SOURCE_PALETTE["accent_primary"] if state=="hover" else SOURCE_PALETTE["text_secondary"]
            if name=="close":
                d.line((12,12,28,28),fill=rgb(color),width=3); d.line((28,12,12,28),fill=rgb(color),width=3)
            elif name=="maximize":
                d.rectangle((10,10,30,30),outline=rgb(color),width=2)
            else:
                d.rectangle((13,10,29,26),outline=rgb(color),width=2)
                d.rectangle((9,14,25,30),outline=rgb(color),width=2)
            chrome.append((f"{name}_{state}",state,im))
    _atlas("window_chrome",chrome,3)

    header=Image.new("RGB",(480,44),rgb(PALETTE["panel"])); d=ImageDraw.Draw(header)
    d.rectangle((0,0,479,43),outline=rgb(PALETTE["border"]),width=2)
    d.rectangle((0,40,479,43),fill=rgb(SOURCE_PALETTE["accent_primary"]))
    d.text((16,12),"NEON STAGE",font=font(15,True),fill=rgb(PALETTE["text_primary"]))
    _atlas("panel_header",[("panel_header","normal",header)],1)
    lcd=Image.new("RGB",(440,64),rgb(SOURCE_PALETTE["lcd_bg"])); d=ImageDraw.Draw(lcd)
    d.rounded_rectangle((1,1,438,62),radius=5,outline=rgb(PALETTE["border"]),width=2)
    d.text((14,10),"Dreaming in Neon",font=font(16,True),fill=rgb(SOURCE_PALETTE["lcd_text"]))
    d.text((14,37),"Pop Punk  |  Bright & Energetic",font=font(10),fill=rgb(PALETTE["text_secondary"]))
    _atlas("preset_lcd",[("preset_lcd","normal",lcd)],1)
    badge=Image.new("RGB",(160,32),rgb(PALETTE["panel"])); d=ImageDraw.Draw(badge)
    d.rounded_rectangle((1,1,158,30),radius=5,outline=rgb(PALETTE["text_secondary"]),width=1)
    d.text((13,9),"SETUP-ONLY",font=font(11,True),fill=rgb(PALETTE["text_secondary"]))
    _atlas("setup_badge",[("setup_only","normal",badge)],1)
    gallery=[]
    for state in ("normal","selected"):
        im=Image.new("RGB",(220,120),rgb(PALETTE["panel"])); d=ImageDraw.Draw(im)
        edge=SOURCE_PALETTE["accent_primary"] if state=="selected" else PALETTE["border"]
        d.rounded_rectangle((2,2,217,117),radius=8,fill=rgb(PALETTE["panel_elevated"]),outline=rgb(edge),width=3)
        d.text((14,82),"NEON STAGE",font=font(13,True),fill=rgb(PALETTE["text_primary"]))
        gallery.append((f"gallery_{state}",state,im))
    _atlas("gallery_tiles",gallery,2)
    tabs=[]
    for state in ("active","inactive","hover"):
        im=Image.new("RGB",(140,36),rgb(PALETTE["panel"])); d=ImageDraw.Draw(im)
        edge=SOURCE_PALETTE["accent_primary"] if state!="inactive" else PALETTE["border"]
        d.rounded_rectangle((1,1,138,34),radius=5,fill=rgb(PALETTE["panel_elevated"]),outline=rgb(edge),width=2)
        d.text((16,11),state.upper(),font=font(10,True),fill=rgb(PALETTE["text_primary"] if state!="inactive" else PALETTE["text_secondary"]))
        tabs.append((f"tab_{state}",state,im))
    _atlas("tabs",tabs,3)
    transport=[]
    for name in ("play","pause","stop"):
        im=_make_icon(name).resize((48,48),Image.Resampling.LANCZOS)
        transport.append((name,"normal",im))
    _atlas("transport",transport,3)
    icons=[(name,"normal",_make_icon(name)) for name in
           ("folder","star","search","close","palette","guitar","radar","curator","settings","i",
            "note","grid","group","play","pause","stop","loop","dice","info","crown")]
    # Keep the two information glyph slots uniquely addressable in the index.
    icons=[(f"{name}_{i:02d}" if i==19 else name,state,im) for i,(name,state,im) in enumerate(icons)]
    _atlas("icons",icons,5)
    window_names=("autochords","fx_edit","skins_gallery","import_guitar","settings",
                  "auto_discover_setup_only","curator_setup_only","about_setup_only")
    aux_windows=[(name,"setup_only" if name.endswith("_setup_only") else "window",
                  _make_aux_window(name)) for name in window_names]
    _atlas("aux_windows",aux_windows,2)
    print("Generated deterministic NEON STAGE sprite atlases and JSON frame indices.")


def step_contact_sheets():
    """Create review-only contact sheets; no runtime assets are promoted."""
    atlas_dir=ASSETS/"atlas"
    files=sorted(atlas_dir.glob("*.png"))
    if not files:
        raise FileNotFoundError("Sprite atlases missing; run step_ui / step_atlases first.")
    thumb_w,thumb_h=320,150
    cols=3; rows=(len(files)+cols-1)//cols
    sheet=Image.new("RGB",(cols*thumb_w,rows*(thumb_h+30)),rgb(PALETTE["window_bg"]))
    d=ImageDraw.Draw(sheet)
    for i,path in enumerate(files):
        with Image.open(path) as opened:
            thumb=opened.convert("RGB")
            thumb.thumbnail((thumb_w-12,thumb_h-12),Image.Resampling.LANCZOS)
        x=(i%cols)*thumb_w+6; y=(i//cols)*(thumb_h+30)+6
        sheet.paste(thumb,(x,y))
        d.text((x,y+thumb_h),path.stem,font=font(11,True),fill=rgb(PALETTE["text_primary"]))
    _indexed_png(sheet,ROOT/"dist"/"NEON_STAGE_ATLAS_CONTACT_SHEET.png")
    refs=[ROOT/f"{i:02d} INTERFACE  NEON STAGE DEFAULT .png" for i in (1,2,3,4,6)]
    thumbs=[]
    for ref in refs:
        if not ref.is_file():
            raise FileNotFoundError(f"Approved reference missing: {ref.name}")
        with Image.open(ref) as opened:
            im=opened.convert("RGB")
            im.thumbnail((480,276),Image.Resampling.LANCZOS)
        thumbs.append((ref.stem,im))
    ref_sheet=Image.new("RGB",(3*500,2*320),rgb(PALETTE["window_bg"]))
    d=ImageDraw.Draw(ref_sheet)
    for i,(label,im) in enumerate(thumbs):
        x=(i%3)*500+10; y=(i//3)*320+8
        ref_sheet.paste(im,(x,y))
        d.text((x,y+280),label,font=font(10,True),fill=rgb(PALETTE["text_primary"]))
    _indexed_png(ref_sheet,ROOT/"dist"/"NEON_STAGE_REFERENCE_CONTACT_SHEET.png")
    print("Created atlas and approved-reference contact sheets for visual sign-off.")


def step_provenance():
    """Hash every file that enters the staging ZIP, excluding only self-referential reports."""
    files=sorted(p for p in OUT.rglob("*") if p.is_file()
                 and p.name not in ("LICENSE.txt","SELF_CHECK.txt"))
    lines=[
        "NEON STAGE DEFAULT SKIN-1 — STAGING ASSET PROVENANCE",
        "Source: repository-approved NEON STAGE mockups and canonical Skin_052/theme.json.",
        "Generation: deterministic Python/Pillow pipeline; no external image URLs or third-party brand artwork.",
        "Scope: staging assets only; not a runtime installation or release license for third-party samples.",
        "SHA-256 entries below cover every file included in the staging ZIP except LICENSE.txt and SELF_CHECK.txt.",
        ""
    ]
    for path in files:
        digest=hashlib.sha256(path.read_bytes()).hexdigest()
        lines.append(f"{digest}  {path.relative_to(OUT).as_posix()}")
    license_path=SKIN/"LICENSE.txt"
    license_path.write_text("\n".join(lines)+"\n",encoding="utf-8")
    declared={}
    for line in lines:
        parts=line.split("  ",1)
        if len(parts)==2 and len(parts[0])==64: declared[parts[1]]=parts[0]
    actual={p.relative_to(OUT).as_posix():hashlib.sha256(p.read_bytes()).hexdigest()
            for p in sorted(OUT.rglob("*")) if p.is_file()
            and p.name not in ("LICENSE.txt","SELF_CHECK.txt")}
    if declared!=actual:
        raise RuntimeError("SHA-256 provenance ledger does not match all staging ZIP files.")


def svg_knob_body():
    return f'''<svg xmlns="http://www.w3.org/2000/svg" width="96" height="96" viewBox="0 0 96 96">
<defs><radialGradient id="g"><stop stop-color="{PALETTE['control_face']}"/><stop offset="1" stop-color="{PALETTE['shadow']}"/></radialGradient></defs>
<circle cx="48" cy="48" r="38" fill="url(#g)" stroke="{PALETTE['control_border']}" stroke-width="2"/>
<circle cx="48" cy="48" r="31" fill="none" stroke="{PALETTE['highlight']}" stroke-opacity=".45"/>
</svg>'''

def svg_knob_pointer():
    return f'''<svg xmlns="http://www.w3.org/2000/svg" width="96" height="96" viewBox="0 0 96 96">
<path d="M48 12 L51 39 L45 39 Z" fill="{PALETTE['accent_glow']}"/>
</svg>'''

def step_svg():
    ensure_dirs()
    texture=f'''<svg xmlns="http://www.w3.org/2000/svg" width="128" height="128" viewBox="0 0 128 128">
<rect width="128" height="128" fill="{PALETTE['window_bg']}"/>
<path d="M0 16H128 M0 64H128 M0 112H128" stroke="{PALETTE['highlight']}" stroke-opacity=".08"/>
<path d="M16 0V128 M64 0V128 M112 0V128" stroke="{PALETTE['highlight']}" stroke-opacity=".06"/>
<circle cx="8" cy="8" r="1" fill="{PALETTE['accent']}" opacity=".18"/>
</svg>'''
    bezel=f'''<svg xmlns="http://www.w3.org/2000/svg" width="800" height="450" viewBox="0 0 800 450">
<rect x="2" y="2" width="796" height="446" rx="22" fill="none" stroke="{PALETTE['control_border']}" stroke-width="3"/>
<rect x="9" y="9" width="782" height="432" rx="17" fill="none" stroke="{PALETTE['accent']}" stroke-opacity=".28"/>
</svg>'''
    crest=f'''<svg xmlns="http://www.w3.org/2000/svg" width="256" height="96" viewBox="0 0 256 96">
<path d="M18 48 L48 12 H208 L238 48 L208 84 H48 Z" fill="{PALETTE['panel']}" stroke="{PALETTE['accent']}" stroke-width="2"/>
<path d="M58 48 H198" stroke="{PALETTE['accent_glow']}" stroke-width="2" opacity=".7"/>
</svg>'''
    (SKIN/"svg"/"texture.svg").write_text(texture,encoding="utf-8")
    (SKIN/"svg"/"bezel.svg").write_text(bezel,encoding="utf-8")
    (SKIN/"svg"/"crest.svg").write_text(crest,encoding="utf-8")
    panel_header = f"""<svg xmlns="http://www.w3.org/2000/svg" width="480" height="44" viewBox="0 0 480 44">
<rect x="1" y="1" width="478" height="42" rx="6" fill="{PALETTE['panel']}" stroke="{PALETTE['control_border']}" stroke-width="2"/>
<path d="M8 40H472" stroke="{SOURCE_PALETTE['accent_primary']}" stroke-width="3"/>
</svg>"""
    (SKIN/"svg"/"panel_header.svg").write_text(panel_header,encoding="utf-8")
    window_dir=SKIN/"svg"/"windows"
    window_dir.mkdir(parents=True,exist_ok=True)
    for name in ("autochords","fx_edit","skins_gallery","import_guitar","settings",
                 "auto_discover_setup_only","curator_setup_only","about_setup_only"):
        badge="SETUP-ONLY" if name.endswith("_setup_only") else "NEON STAGE"
        title=name.replace("_"," ").upper()
        window_svg=f'''<svg xmlns="http://www.w3.org/2000/svg" width="720" height="480" viewBox="0 0 720 480">
<rect x="2" y="2" width="716" height="476" rx="12" fill="{PALETTE['panel']}" stroke="{PALETTE['control_border']}" stroke-width="3"/>
<rect x="9" y="9" width="702" height="42" rx="6" fill="{PALETTE['panel_elevated']}" stroke="{PALETTE['control_border']}"/>
<path d="M12 50H708" stroke="{SOURCE_PALETTE['border_glow']}" stroke-width="2"/>
<text x="22" y="35" fill="{PALETTE['text_primary']}" font-family="sans-serif" font-size="16">{title}</text>
<rect x="650" y="20" width="16" height="16" fill="none" stroke="{PALETTE['text_secondary']}" stroke-width="2"/>
<path d="M680 20L696 36M696 20L680 36" stroke="{PALETTE['text_secondary']}" stroke-width="2"/>
<rect x="12" y="62" width="696" height="405" rx="6" fill="{PALETTE['window_bg']}" stroke="{PALETTE['control_border']}"/>
<text x="560" y="35" fill="{PALETTE['text_secondary']}" font-family="sans-serif" font-size="10">{badge}</text>
</svg>'''
        (window_dir/f"window_{name}.svg").write_text(window_svg,encoding="utf-8")
    print("Generated safe static SVG assets and independent auxiliary-window shells.")

def render_reference_art(size, path):
    """Render a faithful contain-fit of the approved primary UI mockup.

    Do not synthesize a fake plugin screenshot: the uploaded NEON STAGE
    reference is the source of truth for preview and hero artwork.
    """
    reference = ROOT / "01 INTERFACE  NEON STAGE DEFAULT .png"
    if not reference.is_file():
        raise FileNotFoundError(
            f"Approved NEON STAGE reference mockup is missing: {reference.name}"
        )
    with Image.open(reference) as opened:
        source = opened.convert("RGBA")
    fitted = ImageOps.contain(source, size, method=Image.Resampling.LANCZOS)
    canvas = Image.new("RGBA", size, (*rgb(PALETTE["window_bg"]), 255))
    x = (size[0] - fitted.width) // 2
    y = (size[1] - fitted.height) // 2
    canvas.alpha_composite(fitted, (x, y))
    draw = ImageDraw.Draw(canvas)
    inset = max(2, min(size) // 100)
    draw.rounded_rectangle(
        (inset, inset, size[0] - inset - 1, size[1] - inset - 1),
        radius=max(4, min(size) // 45),
        outline=rgb(PALETTE["control_border"]),
        width=max(1, min(size) // 240),
    )
    path.parent.mkdir(parents=True, exist_ok=True)
    # REV ULTRA requires indexed 8-bit PNG assets (not RGBA PNGs).
    indexed = canvas.convert("RGB").quantize(colors=256, method=Image.Quantize.FASTOCTREE, dither=Image.Dither.NONE)
    indexed.save(path, "PNG", optimize=True)


def render_background(size, path):
    """Create a restrained token-only NEON STAGE backdrop, not a fake UI screenshot."""
    width,height=size
    im=Image.new("RGB",size,rgb(SOURCE_PALETTE["bg_deep"]))
    d=ImageDraw.Draw(im)
    d.polygon([(0,int(height*.76)),(int(width*.34),int(height*.66)),
               (int(width*.72),int(height*.72)),(width,height*.61),
               (width,height),(0,height)],fill=rgb(SOURCE_PALETTE["bg_panel"]))
    d.polygon([(0,0),(int(width*.22),0),(0,int(height*.25))],
              fill=rgb(SOURCE_PALETTE["bg_panel_hi"]))
    d.polygon([(width,0),(width,int(height*.28)),(int(width*.78),0)],
              fill=rgb(SOURCE_PALETTE["bg_panel"]))
    for i in range(12):
        y=int(height*.72+i*height*.022)
        d.line((0,y,width,y-int(height*.08)),fill=rgb(SOURCE_PALETTE["led_off"]),width=1)
    d.arc((int(width*.70),-int(height*.25),int(width*1.12),int(height*.38)),
          35,155,fill=rgb(SOURCE_PALETTE["accent_primary"]),width=4)
    d.arc((-int(width*.12),int(height*.58),int(width*.30),int(height*1.12)),
          210,330,fill=rgb(SOURCE_PALETTE["accent_secondary"]),width=4)
    d.line((0,2,width,2),fill=rgb(SOURCE_PALETTE["border"]),width=2)
    d.line((0,height-3,width,height-3),fill=rgb(SOURCE_PALETTE["border"]),width=2)
    path.parent.mkdir(parents=True,exist_ok=True)
    _indexed_png(im,path)


def step_promo():
    ensure_dirs()
    # Dimensions follow the current SKIN asset production specification.
    render_reference_art((360, 240), ASSETS / "preview.png")
    render_reference_art((800, 450), ASSETS / "hero.png")
    render_background((1600, 920), ASSETS / "background.png")
    with Image.open(ASSETS / "hero.png") as hero:
        hero.convert("RGB").save(ASSETS / "hero.webp", "WEBP", quality=92, method=6)
    print("Rendered preview, hero, background and WebP hero from the approved NEON STAGE reference mockup.")
    print("No synthetic UI screenshot or invented control layout was generated.")


def step_manifest():
    ensure_dirs()
    if not SOURCE_THEME.is_file(): raise FileNotFoundError(f"Source theme missing: {SOURCE_THEME}")
    theme=json.loads(SOURCE_THEME.read_text(encoding="utf-8"))
    if theme.get("index")!=52 or theme.get("id")!="neon-stage-default-skin-1":
        raise RuntimeError("Skin_052 identity mismatch; refusing to overwrite another skin.")
    if set(theme.get("palette",{}))!=set(TOKENS):
        raise RuntimeError("Source theme palette does not match the 21-token contract.")
    theme["name"]="NEON STAGE"
    theme["family"]="neon_basement"
    theme["description"]="NEON STAGE: electric cyan stage lighting, deep midnight panels and restrained neon accents."
    # Preserve the source theme palette; generation must not introduce a second palette authority.
    theme["effects"]={
      "material":"midnight stage console with cyan edge lighting",
      "texture":"svg/texture.svg","bezel":"svg/bezel.svg","crest":"svg/crest.svg",
      "shadowDepth":0.38,"edgeHighlight":0.34,"glow":"restrained cyan LED halo; reduced in ECO mode"
    }
    theme["schema"]="ppgu-theme/1.2"
    theme["assets"]={"preview":"assets/preview.png","hero":"assets/hero.png",
                     "heroWebp":"assets/hero.webp","background":"assets/background.png",
                     "texture":"svg/texture.svg","bezel":"svg/bezel.svg","crest":"svg/crest.svg",
                     "panelHeader":"svg/panel_header.svg","atlas":"assets/atlas/",
                     "atlasIndexes":"assets/atlas/*.json","windowChrome":"assets/atlas/window_chrome.png",
                     "auxWindows":"assets/atlas/aux_windows.png","windowSVGs":"svg/windows/",
                     "license":"LICENSE.txt","selfCheck":"SELF_CHECK.txt"}
    # G2 inventory comes from the audited runtimeControlInventory, not guessed labels or IDs.
    schema=json.loads((ROOT/"SKIN"/"control_schema.json").read_text(encoding="utf-8"))
    contract=schema.get("x-ppgu-contract",{})
    inventory=contract.get("runtimeControlInventory",{})
    additional=contract.get("additionalCurrentControls",{})
    control_map={}
    for control_id, spec in theme.get("controls",{}).items():
        grid=spec.get("grid",{})
        detail=inventory.get(control_id,additional.get(control_id,{}))
        parameter_ids=detail.get("parameterIds")
        if parameter_ids is None and isinstance(detail.get("parameterBinding"),str):
            parameter_ids=[detail["parameterBinding"]]
        control_map[control_id]={
            "id":control_id,
            "label":detail.get("label",control_id.replace("_"," ").upper()),
            "type":detail.get("controlType",detail.get("kind","unclassified")),
            "runtimeStatus":detail.get("runtimeStatus","unverified"),
            "component":detail.get("component",detail.get("components")),
            "function":detail.get("function"),
            "col":grid.get("x"),"row":grid.get("y"),
            "colSpan":grid.get("w"),"rowSpan":grid.get("h"),
            "apvtsBinding":parameter_ids,
            "parameterDefinitions":detail.get("parameterDefinitions",[]),
            "actionBinding":detail.get("actionBinding"),
            "midiBinding":detail.get("midiBinding"),
            "noteRange":detail.get("noteRange"),
            "availability":detail.get("availability")
        }
    if len(control_map)!=28 or len(set(control_map))!=28:
        raise RuntimeError(f"G2 requires exactly 28 unique controls; found {len(control_map)}")
    (SKIN/"control_map.json").write_text(json.dumps({
        "schemaVersion":"1.0.0","grid":{"columns":12,"rows":16},
        "controlCount":28,"controls":control_map
    },indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
    theme["designStatus"]="STAGING_NOT_MERGED"
    theme["notes"]=[
      "Generated by NEON_STAGE_BUILD_ENGINE.",
      "Staging draft only: theme_manifest.json and the runtime default were not modified.",
      "Visual-only theme; APVTS, live layout, audio and MIDI contracts remain unchanged.",
      "Control positions copied from the existing Skin_052 metadata; coordinates are not runtime layout changes."
    ]
    (SKIN/"theme.json").write_text(json.dumps(theme,indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
    entry={"id":"neon-stage-default-skin-1","index":52,"name":"NEON STAGE","family":"neon_basement",
           "layout":theme["layout"],"path":"Skin_052/theme.json",
           "preview":"Skin_052/assets/preview.png","hero":"Skin_052/assets/hero.png"}
    pack={"schemaVersion":"1.0.0","product":"POP PUNK GUITAR ULTIMATE",
          "status":"STAGING_NOT_MERGED","sourceRegistryTotal":102,
          "startIndex":52,"endIndex":52,"totalSkins":1,"themes":[entry],
          "note":"Draft for review only. Not accepted by merge_skin_pack.py; no registry/default change."}
    (OUT/"theme_pack_manifest.json").write_text(json.dumps(pack,indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
    print("Wrote Skin_052 draft and staging manifest. Registry untouched.")

def step_package():
    """Create a staging ZIP without modifying any runtime/JUCE resource tree."""
    ensure_dirs()
    dist=ROOT/"dist"
    dist.mkdir(exist_ok=True)
    zip_path=dist/"NEON_STAGE_DEFAULT_SKIN_1_STAGING.zip"
    temp_path=dist/(zip_path.name + ".tmp")
    try:
        with zipfile.ZipFile(temp_path,"w",zipfile.ZIP_DEFLATED,compresslevel=9) as z:
            for p in sorted(OUT.rglob("*")):
                if p.is_file():
                    info=zipfile.ZipInfo(p.relative_to(OUT).as_posix(),date_time=(2020,1,1,0,0,0))
                    info.compress_type=zipfile.ZIP_DEFLATED
                    info.external_attr=0o100644 << 16
                    info.create_system=3
                    z.writestr(info,p.read_bytes(),compress_type=zipfile.ZIP_DEFLATED,compresslevel=9)
        with zipfile.ZipFile(temp_path,"r") as check:
            bad_member=check.testzip()
            if bad_member is not None:
                raise RuntimeError(f"Staging ZIP integrity check failed at: {bad_member}")
        os.replace(temp_path,zip_path)
    finally:
        if temp_path.exists():
            temp_path.unlink()
    print(f"Packaged staging-only archive: {zip_path}")
    print("Runtime/JUCE resources were not modified.")

def step_report():
    errors=[]
    theme_path=SKIN/"theme.json"
    control_map_path=SKIN/"control_map.json"
    if not theme_path.is_file(): errors.append("theme.json missing")
    else:
        theme=json.loads(theme_path.read_text(encoding="utf-8"))
        if theme.get("id")!="neon-stage-default-skin-1" or theme.get("name")!="NEON STAGE": errors.append("theme identity mismatch")
        if set(theme.get("palette",{}))!=set(TOKENS): errors.append("palette token parity failure")
        missing=sorted(set(REQUIRED)-set(theme.get("controls",{})))
        if missing: errors.append("missing controls: "+", ".join(missing))
        for fg,bg,minimum in (("text_primary","bg_deep",7),("text_secondary","bg_deep",4.5),
                              ("text_disabled","bg_deep",3),("lcd_text","lcd_bg",7),("accent_primary","bg_panel",3)):
            ratio=contrast(theme["palette"][fg],theme["palette"][bg])
            if ratio<minimum: errors.append(f"contrast {fg}/{bg}: {ratio:.2f} < {minimum}")
    pngs={"assets/preview.png":(360,240),"assets/hero.png":(800,450),
          "assets/background.png":(1600,920),
          "assets/knob/knob_filmstrip.png":(96,96*121),
          "assets/knob/knob_filmstrip_2x.png":(192,192*121),
          "assets/buttons/btn_states.png":(180*6,58),
          "assets/fx/fx_tiles.png":(220*7,72*2),
          "assets/slider/slider_track.png":(360,28),"assets/slider/slider_cap.png":(32,40),
          "assets/led/led_states.png":(64,32)}
    for rel,expected in pngs.items():
        p=SKIN/rel
        if not p.is_file(): errors.append(f"missing {rel}"); continue
        try:
            with Image.open(p) as im:
                expected_mode="P"
                if im.format!="PNG" or im.mode!=expected_mode or im.size!=expected:
                    errors.append(f"{rel}: expected {expected_mode} PNG {expected}, got {im.format}/{im.mode}/{im.size}")
                im.verify()
        except Exception as exc: errors.append(f"{rel}: decode failed: {exc}")
    webp_path=ASSETS/"hero.webp"
    if not webp_path.is_file():
        errors.append("missing assets/hero.webp")
    else:
        try:
            with Image.open(webp_path) as webp:
                if webp.format!="WEBP" or webp.size!=(800,450):
                    errors.append(f"hero.webp: expected WEBP 800x450, got {webp.format}/{webp.size}")
                webp.verify()
        except Exception as exc:
            errors.append(f"hero.webp: decode failed: {exc}")
    svg_files=["svg/texture.svg","svg/bezel.svg","svg/crest.svg","svg/panel_header.svg",
               "assets/knob/knob_body.svg","assets/knob/knob_pointer.svg"]
    svg_files += [f"svg/windows/window_{name}.svg" for name in
                  ("autochords","fx_edit","skins_gallery","import_guitar","settings",
                   "auto_discover_setup_only","curator_setup_only","about_setup_only")]
    for rel in svg_files:
        p=SKIN/rel
        if not p.is_file(): errors.append(f"missing {rel}"); continue
        data=p.read_text(encoding="utf-8")
        if re.search(r"<\s*(script|foreignObject)\b|<!DOCTYPE|<!ENTITY|\bon[a-z]+\s*=|\b(?:href|src)\s*=",data,re.I):
            errors.append(f"unsafe SVG content in {rel}")
        try:
            root=ET.fromstring(data)
            if not root.tag.endswith("svg"): errors.append(f"invalid SVG root in {rel}")
        except ET.ParseError as exc:
            errors.append(f"invalid SVG XML in {rel}: {exc}")

    atlas_dir=ASSETS/"atlas"
    atlas_jsons=sorted(atlas_dir.glob("*.json")) if atlas_dir.is_dir() else []
    expected_atlases={
        "knobs_main":351,"knobs_main_2x":351,"knobs_amp":117,"knobs_amp_2x":117,
        "fx_tiles":28,"toggles":9,"slider_tracks":2,"slider_vertical":1,
        "slider_thumbs":3,"pitch_mod_wheels":2,"keys_white":3,"keys_black":3,
        "meters_led":10,"meters_columns":2,"window_chrome":6,"panel_header":1,
        "preset_lcd":1,"setup_badge":1,"gallery_tiles":2,"tabs":3,"transport":3,
        "icons":20,"aux_windows":8
    }
    actual_atlases={p.stem for p in atlas_jsons}
    if actual_atlases!=set(expected_atlases):
        errors.append(f"sprite atlas index set mismatch: missing={sorted(set(expected_atlases)-actual_atlases)}, extra={sorted(actual_atlases-set(expected_atlases))}")
    atlas_frames=0
    for index_path in atlas_jsons:
        try:
            idx=json.loads(index_path.read_text(encoding="utf-8"))
            image_path=atlas_dir/(index_path.stem+".png")
            if not image_path.is_file():
                errors.append(f"atlas PNG missing for {index_path.name}")
                continue
            with Image.open(image_path) as atlas_image:
                atlas_image.verify()
            with Image.open(image_path) as atlas_image:
                if atlas_image.format!="PNG" or atlas_image.mode!="P":
                    errors.append(f"{image_path.name}: expected indexed PNG-8, got {atlas_image.format}/{atlas_image.mode}")
                expected_count=expected_atlases.get(index_path.stem)
                if idx.get("frameCount")!=len(idx.get("frames",[])):
                    errors.append(f"{index_path.name}: frameCount mismatch")
                if expected_count is not None and idx.get("frameCount")!=expected_count:
                    errors.append(f"{index_path.name}: expected {expected_count} frames, got {idx.get('frameCount')}")
                names=[frame.get("name") for frame in idx.get("frames",[])]
                if len(names)!=len(set(names)):
                    errors.append(f"{index_path.name}: duplicate frame names")
                if idx.get("cellWidth") is None or idx.get("cellHeight") is None:
                    errors.append(f"{index_path.name}: missing frame size metadata")
                for frame in idx.get("frames",[]):
                    x,y,w,h=(frame.get(k) for k in ("x","y","w","h"))
                    if not all(isinstance(v,int) and not isinstance(v,bool) for v in (x,y,w,h)):
                        errors.append(f"{index_path.name}: invalid frame coordinates")
                        break
                    if x<0 or y<0 or w<1 or h<1 or x+w>atlas_image.width or y+h>atlas_image.height:
                        errors.append(f"{index_path.name}: frame out of atlas bounds")
                        break
                    if w!=idx.get("cellWidth") or h!=idx.get("cellHeight"):
                        errors.append(f"{index_path.name}: frame dimensions differ from indexed cell size")
                        break
                atlas_frames+=idx.get("frameCount",0)
        except Exception as exc:
            errors.append(f"{index_path.name}: atlas validation failed: {exc}")
    if atlas_frames!=1044:
        errors.append(f"sprite frame contract mismatch: expected 1044 frames, found {atlas_frames}")
    for contact_name, expected_size in (("NEON_STAGE_ATLAS_CONTACT_SHEET.png",None),
                                         ("NEON_STAGE_REFERENCE_CONTACT_SHEET.png",(1500,640))):
        contact_path=ROOT/"dist"/contact_name
        if not contact_path.is_file():
            errors.append(f"missing review contact sheet: {contact_name}")
            continue
        try:
            with Image.open(contact_path) as contact:
                if contact.format!="PNG" or contact.mode!="P":
                    errors.append(f"{contact_name}: expected indexed PNG-8")
                if expected_size is not None and contact.size!=expected_size:
                    errors.append(f"{contact_name}: expected {expected_size}, got {contact.size}")
                if contact.width<1 or contact.height<1:
                    errors.append(f"{contact_name}: invalid dimensions")
                contact.verify()
        except Exception as exc:
            errors.append(f"{contact_name}: decode failed: {exc}")

    # G4 identity and authoritative registry contract.
    if SKIN.name != "Skin_052":
        errors.append(f"G4 folder identity mismatch: expected Skin_052, got {SKIN.name}")
    registry_path=ROOT/"SKIN"/"theme_manifest.json"
    if not registry_path.is_file():
        errors.append("G4 authoritative SKIN/theme_manifest.json is missing")
    else:
        registry=json.loads(registry_path.read_text(encoding="utf-8"))
        entries=registry.get("themes",[])
        indices=[entry.get("index") for entry in entries]
        ids=[entry.get("id") for entry in entries]
        if indices != list(range(1,len(entries)+1)):
            errors.append("G4 registry indices are not continuous from 1 through the final registered skin")
        if len(ids)!=len(set(ids)):
            errors.append("G4 duplicate skin IDs found in authoritative registry")
        matches=[entry for entry in entries if entry.get("id")=="neon-stage-default-skin-1"]
        if len(matches)!=1:
            errors.append(f"G4 expected one registry entry for neon-stage-default-skin-1, found {len(matches)}")
        elif matches[0].get("index")!=52 or matches[0].get("path")!="Skin_052/theme.json":
            errors.append("G4 registry identity/path mismatch for Skin_052")
    if theme_path.is_file():
        generated_theme=json.loads(theme_path.read_text(encoding="utf-8"))
        if generated_theme.get("id")!="neon-stage-default-skin-1" or generated_theme.get("index")!=52:
            errors.append("G4 generated theme id/index mismatch")
    control_data={}
    if not control_map_path.is_file():
        errors.append("missing control_map.json")
    else:
        control_data=json.loads(control_map_path.read_text(encoding="utf-8"))
        mapped=control_data.get("controls",{})
        schema=json.loads((ROOT/"SKIN"/"control_schema.json").read_text(encoding="utf-8"))
        schema_ids=set(schema.get("x-ppgu-contract",{}).get("requiredControlIds",[]))
        # sync_selector is a live current control recorded in additionalCurrentControls.
        expected_ids=schema_ids | {"sync_selector"}
        if control_data.get("grid")!={"columns":12,"rows":16} or len(mapped)!=28 or set(mapped)!=expected_ids:
            errors.append(
                "G2 control_map.json must contain the exact 28-control inventory "
                f"(missing={sorted(expected_ids-set(mapped))}, extra={sorted(set(mapped)-expected_ids)})"
            )
        inventory=schema.get("x-ppgu-contract",{}).get("runtimeControlInventory",{})
        additional=schema.get("x-ppgu-contract",{}).get("additionalCurrentControls",{})
        for cid,item in mapped.items():
            detail=inventory.get(cid,additional.get(cid,{}))
            expected_binding=detail.get("parameterIds")
            if expected_binding is None and isinstance(detail.get("parameterBinding"),str):
                expected_binding=[detail["parameterBinding"]]
            if item.get("apvtsBinding")!=expected_binding:
                errors.append(f"G2 APVTS binding mismatch for {cid}: {item.get('apvtsBinding')} != {expected_binding}")
            if item.get("runtimeStatus")!=detail.get("runtimeStatus","unverified"):
                errors.append(f"G2 runtimeStatus mismatch for {cid}")
            if item.get("parameterDefinitions",[])!=detail.get("parameterDefinitions",[]):
                errors.append(f"G2 parameter definition mismatch for {cid}")
        rectangles=[]
        for cid,box in mapped.items():
            col,row,cs,rs=(box.get(k) for k in ("col","row","colSpan","rowSpan"))
            if not all(isinstance(v,int) and not isinstance(v,bool) for v in (col,row,cs,rs)) or col<0 or row<0 or cs<1 or rs<1 or col+cs>12 or row+rs>16:
                errors.append(f"G2 out-of-bounds or invalid control map entry: {cid}")
                continue
            for other_id,other in rectangles:
                overlap=(col < other["col"]+other["colSpan"] and
                         col+cs > other["col"] and
                         row < other["row"]+other["rowSpan"] and
                         row+rs > other["row"])
                if overlap:
                    errors.append(f"G2 overlapping grid controls: {other_id} and {cid}")
            rectangles.append((cid,{"col":col,"row":row,"colSpan":cs,"rowSpan":rs}))
    result={"project":"PPGU NEON STAGE","skin":"NEON STAGE DEFAULT SKIN-1","status":"PASS" if not errors else "FAIL",
            "checks":{"palette_tokens":len(TOKENS),"required_controls":28,"control_map_controls":len(control_data.get("controls",{})) if control_map_path.is_file() else 0,"png_assets":len(pngs),
                      "atlas_indexes":len(atlas_jsons),"atlas_frames":atlas_frames,
                      "svg_assets":len(svg_files),"skin_registry_modified":False,"runtime_tests_run":False},
            "errors":errors,
            "limitations":["This is a procedural staging render, not a verified pixel-match to the approved reference mockup.",
                           "No JUCE build, CTest, pluginval or FL Studio runtime test is performed by this Python asset pipeline."]}
    (ROOT/"dist"/"NEON_STAGE_BUILD_REPORT.json").write_text(json.dumps(result,indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
    (OUT/"NEON_STAGE_BUILD_REPORT.json").write_text(json.dumps(result,indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
    if errors: raise SystemExit(1)
    step_provenance()
    (SKIN/"SELF_CHECK.txt").write_text(
        "NEON STAGE G1-G4 STAGING SELF-CHECK\n"
        "Result: PASS (staging asset contract only)\n"
        f"Palette tokens: {len(TOKENS)}\n"
        f"Control map: {len(control_data.get('controls',{}))} controls\n"
        f"Atlas indexes: {len(atlas_jsons)}\n"
        f"Atlas frames: {atlas_frames}\n"
        f"LICENSE SHA-256: {hashlib.sha256((SKIN/'LICENSE.txt').read_bytes()).hexdigest()}\n"
        "Registry/runtime unchanged by staging pipeline.\n"
        "No JUCE build, CTest, pluginval or FL Studio 20 host test is performed here.\n",
        encoding="utf-8"
    )
    print(json.dumps(result,indent=2,ensure_ascii=False))

STEPS={"palette":step_palette,"knobs":step_knobs,"ui":step_ui,"atlases":step_atlases,
       "svg":step_svg,"promo":step_promo,"contact":step_contact_sheets,
       "provenance":step_provenance,"manifest":step_manifest,"package":step_package,"report":step_report}
if __name__=="__main__":
    import argparse
    p=argparse.ArgumentParser(); p.add_argument("step",choices=STEPS); a=p.parse_args(); STEPS[a.step]()
