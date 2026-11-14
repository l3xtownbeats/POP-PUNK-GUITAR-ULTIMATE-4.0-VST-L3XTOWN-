"""Procedural asset builder for PPGU NEON STAGE Skin_053.
Writes staging outputs only; never mutates the runtime registry or JUCE resources.
"""
from __future__ import annotations
import json, math, os, zipfile
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont, ImageFilter, ImageOps

ROOT = Path(__file__).resolve().parent
SOURCE_THEME = ROOT / "SKIN" / "Skin_053" / "theme.json"
OUT = ROOT / "dist" / "neon_stage_staging"
SKIN = OUT / "Skin_053"
ASSETS = SKIN / "assets"
PALETTE = {
    "window_bg":"#050811", "panel":"#091321", "panel_elevated":"#101F32",
    "control_bg":"#07101C", "control_face":"#14263A", "control_border":"#52718B",
    "text_primary":"#FFFFFF", "text_secondary":"#D9E8F7", "text_disabled":"#AABBD0",
    "accent":"#66FFFF", "accent_glow":"#B8FFFF", "danger":"#FF5577",
    "warning":"#FFD166", "success":"#67F5B5", "lcd_bg":"#020A12",
    "lcd_text":"#C9FFFF", "led_on":"#66FFFF", "led_off":"#263C50",
    "shadow":"#02040A", "highlight":"#B8D9F0", "lowlight":"#0A1421"
}
TOKENS = tuple(PALETTE)
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
              ASSETS/"led",ASSETS/"texture",ASSETS/"svg"):
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
    strip.save(path,optimize=True)

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
    atlas.save(ASSETS/"buttons"/"btn_states.png",optimize=True)
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
    fx.save(ASSETS/"fx"/"fx_tiles.png",optimize=True)
    track=Image.new("RGBA",(360,28),(0,0,0,0)); d=ImageDraw.Draw(track)
    d.rounded_rectangle((4,10,356,18),radius=4,fill=PALETTE["control_bg"],outline=PALETTE["control_border"])
    d.rounded_rectangle((4,10,210,18),radius=4,fill=PALETTE["accent"])
    track.save(ASSETS/"slider"/"slider_track.png",optimize=True)
    cap=Image.new("RGBA",(32,40),(0,0,0,0)); d=ImageDraw.Draw(cap)
    d.ellipse((4,4,28,28),fill=PALETTE["control_face"],outline=PALETTE["accent"],width=2)
    d.line((16,9,16,23),fill=PALETTE["text_primary"],width=2)
    cap.save(ASSETS/"slider"/"slider_cap.png",optimize=True)
    led=Image.new("RGBA",(64,32),(0,0,0,0)); d=ImageDraw.Draw(led)
    d.ellipse((3,3,28,28),fill=PALETTE["led_off"],outline=PALETTE["control_border"],width=2)
    d.ellipse((35,3,60,28),fill=PALETTE["led_on"],outline=PALETTE["accent_glow"],width=2)
    led.save(ASSETS/"led"/"led_states.png",optimize=True)
    print("Generated six button states, seven FX x two states, slider and LED atlas.")

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
    (ASSETS/"texture.svg").write_text(texture,encoding="utf-8")
    (ASSETS/"bezel.svg").write_text(bezel,encoding="utf-8")
    (ASSETS/"crest.svg").write_text(crest,encoding="utf-8")
    print("Generated safe, static SVG assets.")

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
    indexed = canvas.convert("RGB").quantize(colors=256, method=Image.Quantize.FASTOCTREE)
    indexed.save(path, "PNG", optimize=True)


def step_promo():
    ensure_dirs()
    # Dimensions follow the current SKIN asset production specification.
    render_reference_art((360, 240), ASSETS / "preview.png")
    render_reference_art((800, 450), ASSETS / "hero.png")
    print("Rendered preview and hero from the approved NEON STAGE reference mockup.")
    print("No synthetic UI screenshot or invented control layout was generated.")


def step_manifest():
    ensure_dirs()
    if not SOURCE_THEME.is_file(): raise FileNotFoundError(f"Source theme missing: {SOURCE_THEME}")
    theme=json.loads(SOURCE_THEME.read_text(encoding="utf-8"))
    if theme.get("index")!=53 or theme.get("id")!="skin-053":
        raise RuntimeError("Skin_053 identity mismatch; refusing to overwrite another skin.")
    if set(theme.get("palette",{}))!=set(TOKENS):
        raise RuntimeError("Source theme palette does not match the 21-token contract.")
    theme["name"]="NEON STAGE"
    theme["family"]="neon_basement"
    theme["description"]="NEON STAGE: electric cyan stage lighting, deep midnight panels and restrained neon accents."
    theme["palette"]=PALETTE
    theme["effects"]={
      "material":"midnight stage console with cyan edge lighting",
      "texture":"assets/texture.svg","bezel":"assets/bezel.svg","crest":"assets/crest.svg",
      "shadowDepth":0.38,"edgeHighlight":0.34,"glow":"restrained cyan LED halo; reduced in ECO mode"
    }
    theme["assets"]={"preview":"assets/preview.png","hero":"assets/hero.png",
                     "texture":"assets/texture.svg","bezel":"assets/bezel.svg","crest":"assets/crest.svg"}
    # Canonical 28-control grid map for the REV ULTRA G2 contract.
    control_map={}
    for control_id, spec in theme.get("controls",{}).items():
        grid=spec.get("grid",{})
        control_map[control_id]={
            "col":grid.get("x"),"row":grid.get("y"),
            "colSpan":grid.get("w"),"rowSpan":grid.get("h")
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
      "Control positions copied from the existing Skin_053 metadata; coordinates are not runtime layout changes."
    ]
    (SKIN/"theme.json").write_text(json.dumps(theme,indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
    entry={"id":"skin-053","index":53,"name":"NEON STAGE","family":"neon_basement",
           "layout":theme["layout"],"path":"Skin_053/theme.json",
           "preview":"Skin_053/assets/preview.png","hero":"Skin_053/assets/hero.png"}
    pack={"schemaVersion":"1.0.0","product":"POP PUNK GUITAR ULTIMATE",
          "status":"STAGING_NOT_MERGED","sourceRegistryTotal":102,
          "startIndex":53,"endIndex":53,"totalSkins":1,"themes":[entry],
          "note":"Draft for review only. Not accepted by merge_skin_pack.py; no registry/default change."}
    (OUT/"theme_pack_manifest.json").write_text(json.dumps(pack,indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
    print("Wrote Skin_053 draft and staging manifest. Registry untouched.")

def step_package():
    """Create a staging ZIP without modifying any runtime/JUCE resource tree."""
    ensure_dirs()
    dist=ROOT/"dist"
    dist.mkdir(exist_ok=True)
    zip_path=dist/"NEON_STAGE_SKIN_053_STAGING.zip"
    temp_path=dist/(zip_path.name + ".tmp")
    try:
        with zipfile.ZipFile(temp_path,"w",zipfile.ZIP_DEFLATED,compresslevel=9) as z:
            for p in OUT.rglob("*"):
                if p.is_file():
                    z.write(p,p.relative_to(OUT).as_posix())
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
    if not theme_path.is_file(): errors.append("theme.json missing")
    else:
        theme=json.loads(theme_path.read_text(encoding="utf-8"))
        if theme.get("id")!="skin-053" or theme.get("name")!="NEON STAGE": errors.append("theme identity mismatch")
        if set(theme.get("palette",{}))!=set(TOKENS): errors.append("palette token parity failure")
        missing=sorted(set(REQUIRED)-set(theme.get("controls",{})))
        if missing: errors.append("missing controls: "+", ".join(missing))
        for fg,bg,minimum in (("text_primary","window_bg",7),("text_secondary","window_bg",4.5),
                              ("text_disabled","window_bg",3),("lcd_text","lcd_bg",7),("accent","panel",3)):
            ratio=contrast(theme["palette"][fg],theme["palette"][bg])
            if ratio<minimum: errors.append(f"contrast {fg}/{bg}: {ratio:.2f} < {minimum}")
    pngs={"assets/preview.png":(360,240),"assets/hero.png":(800,450),
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
                expected_mode="P" if rel in ("assets/preview.png","assets/hero.png") else "RGBA"
                if im.format!="PNG" or im.mode!=expected_mode or im.size!=expected:
                    errors.append(f"{rel}: expected {expected_mode} PNG {expected}, got {im.format}/{im.mode}/{im.size}")
                im.verify()
        except Exception as exc: errors.append(f"{rel}: decode failed: {exc}")
    for rel in ("assets/texture.svg","assets/bezel.svg","assets/crest.svg","assets/knob/knob_body.svg","assets/knob/knob_pointer.svg"):
        p=SKIN/rel
        if not p.is_file(): errors.append(f"missing {rel}"); continue
        data=p.read_text(encoding="utf-8")
        if "<script" in data.lower() or "javascript:" in data.lower() or "<animate" in data.lower() or "xlink:href" in data.lower():
            errors.append(f"unsafe SVG content in {rel}")
    # G4 identity and authoritative registry contract.
    if SKIN.name != "Skin_053":
        errors.append(f"G4 folder identity mismatch: expected Skin_053, got {SKIN.name}")
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
        matches=[entry for entry in entries if entry.get("id")=="skin-053"]
        if len(matches)!=1:
            errors.append(f"G4 expected one registry entry for skin-053, found {len(matches)}")
        elif matches[0].get("index")!=53 or matches[0].get("path")!="Skin_053/theme.json":
            errors.append("G4 registry identity/path mismatch for Skin_053")
    if theme_path.is_file():
        generated_theme=json.loads(theme_path.read_text(encoding="utf-8"))
        if generated_theme.get("id")!="skin-053" or generated_theme.get("index")!=53:
            errors.append("G4 generated theme id/index mismatch")
    if not control_map_path.is_file():
        errors.append("missing control_map.json")
    else:
        control_data=json.loads(control_map_path.read_text(encoding="utf-8"))
        mapped=control_data.get("controls",{})
        if control_data.get("grid")!={"columns":12,"rows":16} or len(mapped)!=28 or len(set(mapped))!=28:
            errors.append("G2 control_map.json must contain exactly 28 unique controls in a 12x16 grid")
        for cid,box in mapped.items():
            col,row,cs,rs=(box.get(k) for k in ("col","row","colSpan","rowSpan"))
            if not all(isinstance(v,int) for v in (col,row,cs,rs)) or col<0 or row<0 or cs<1 or rs<1 or col+cs>12 or row+rs>16:
                errors.append(f"G2 out-of-bounds or invalid control map entry: {cid}")
    result={"project":"PPGU NEON STAGE","skin":"Skin_053","status":"PASS" if not errors else "FAIL",
            "checks":{"palette_tokens":len(TOKENS),"required_controls":len(REQUIRED),"png_assets":len(pngs),
                      "control_map_controls":28,"svg_assets":5,"skin_registry_modified":False,"runtime_tests_run":False},
            "errors":errors,
            "limitations":["This is a procedural staging render, not a verified pixel-match to the approved reference mockup.",
                           "No JUCE build, CTest, pluginval or FL Studio runtime test is performed by this Python asset pipeline."]}
    (ROOT/"dist"/"NEON_STAGE_BUILD_REPORT.json").write_text(json.dumps(result,indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
    (OUT/"NEON_STAGE_BUILD_REPORT.json").write_text(json.dumps(result,indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
    print(json.dumps(result,indent=2,ensure_ascii=False))
    if errors: raise SystemExit(1)

STEPS={"palette":step_palette,"knobs":step_knobs,"ui":step_ui,"svg":step_svg,
       "promo":step_promo,"manifest":step_manifest,"package":step_package,"report":step_report}
if __name__=="__main__":
    import argparse
    p=argparse.ArgumentParser(); p.add_argument("step",choices=STEPS); a=p.parse_args(); STEPS[a.step]()
