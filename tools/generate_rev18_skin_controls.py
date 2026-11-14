#!/usr/bin/env python3
"""Generate deterministic REV18 SKIN-1 control/state sprite masters and runtime copies."""
from __future__ import annotations
import argparse, hashlib, io, json, zlib
from pathlib import Path
from PIL import Image, ImageDraw

SIZE=512
STATES=("default","hover","pressed","disabled","dragged")
P={"bg":(11,15,18,255),"panel":(20,27,32,255),"face":(25,35,41,255),
   "elevated":(29,41,48,255),"border":(83,101,107,255),"text":(243,247,245,255),
   "muted":(135,151,145,255),"accent":(213,255,69,255),"glow":(222,255,110,255),
   "disabled":(51,68,59,255)}
RESERVED={"LED_indicator","Auto_Discover","Curator","Settings","About"}


def colors(state, disabled=False):
    if disabled: state="disabled"
    return {"default":(P["face"],P["border"],P["accent"]),
            "hover":(P["elevated"],P["muted"],P["glow"]),
            "pressed":(P["bg"],P["accent"],P["accent"]),
            "disabled":(P["bg"],P["disabled"],P["disabled"]),
            "dragged":(P["elevated"],P["glow"],P["glow"])}[state]


def draw_knob(d,face,border,accent,state):
    import math
    d.ellipse((70,70,442,442),fill=P["panel"],outline=border,width=14)
    d.ellipse((100,100,412,412),fill=face,outline=P["bg"],width=10)
    for i in range(21):
        a=math.radians(-135+i*13.5); x1,y1=256+math.cos(a)*185,256+math.sin(a)*185
        x2,y2=256+math.cos(a)*204,256+math.sin(a)*204
        d.line((x1,y1,x2,y2),fill=accent if i<=13 else P["border"],width=5)
    d.ellipse((145,145,367,367),fill=P["panel"],outline=border,width=8)
    d.ellipse((174,174,338,338),fill=face,outline=P["border"],width=4)
    d.rounded_rectangle((248,184,264,264),radius=7,fill=accent)
    d.ellipse((234,234,278,278),fill=P["elevated"],outline=border,width=4)
    if state=="pressed": d.arc((116,116,396,396),205,330,fill=accent,width=10)


def draw_switch(d,face,border,accent,state):
    d.rounded_rectangle((56,154,456,358),radius=102,fill=P["panel"],outline=border,width=14)
    d.rounded_rectangle((82,180,430,332),radius=76,fill=face,outline=P["border"],width=6)
    d.line((120,256,392,256),fill=border,width=7)
    d.ellipse((174,174,338,338),fill=P["elevated"],outline=accent,width=12)
    d.ellipse((210,210,302,302),fill=face,outline=border,width=6)
    if state=="hover": d.arc((70,168,442,344),185,350,fill=accent,width=8)


def draw_browser(d,face,border,accent,state):
    d.rounded_rectangle((40,166,472,346),radius=30,fill=P["panel"],outline=border,width=12)
    d.rounded_rectangle((62,188,450,324),radius=18,fill=face,outline=P["border"],width=5)
    d.rounded_rectangle((88,216,360,296),radius=12,fill=P["bg"],outline=border,width=4)
    d.line((116,242,300,242),fill=P["text"],width=8)
    d.line((116,270,260,270),fill=P["muted"],width=6)
    d.line((390,238,410,258,430,238),fill=accent,width=9,joint="curve")


def draw_mode(d,face,border,accent,state):
    d.rounded_rectangle((48,176,464,336),radius=78,fill=P["panel"],outline=border,width=12)
    d.rounded_rectangle((72,200,256,312),radius=52,fill=face,outline=accent,width=7)
    d.rounded_rectangle((256,200,440,312),radius=52,fill=P["bg"],outline=P["border"],width=5)
    d.line((256,214,256,298),fill=border,width=5)


def draw_keyboard(d,face,border,accent,state):
    x0,y0,x1,y1=32,196,480,348; white_w=(x1-x0)/14
    d.rounded_rectangle((x0-10,y0-18,x1+10,y1+18),radius=18,fill=P["panel"],outline=border,width=10)
    for i in range(14):
        x=x0+i*white_w
        d.rectangle((x,y0,x+white_w,y1),fill=P["text"],outline=P["border"],width=3)
    for i in (0,1,3,4,5,7,8,10,11,12):
        x=x0+(i+1)*white_w-13
        d.rounded_rectangle((x,y0,x+26,y0+88),radius=5,fill=P["bg"],outline=border,width=2)
    d.line((x0,y1+30,x1,y1+30),fill=accent,width=5)


def draw_lcd(d,face,border,accent,state):
    d.rounded_rectangle((38,150,474,362),radius=26,fill=P["panel"],outline=border,width=12)
    d.rounded_rectangle((64,176,448,336),radius=14,fill=P["bg"],outline=P["border"],width=5)
    d.line((92,292,420,292),fill=P["disabled"],width=4)


def draw_button(d,face,border,accent,state,icon=None):
    d.rounded_rectangle((62,170,450,342),radius=38,fill=P["panel"],outline=border,width=12)
    d.rounded_rectangle((82,190,430,322),radius=28,fill=face,outline=P["border"],width=5)
    if icon is not None:
        icon=icon.resize((112,112),Image.Resampling.LANCZOS).convert("RGBA")
        if state=="disabled":
            alpha=icon.getchannel("A").point(lambda value:int(value*.55))
            tinted=Image.new("RGBA",icon.size,P["muted"]); tinted.putalpha(alpha); icon=tinted
        d._image.paste(icon,(200,200),icon)
    else:
        d.rounded_rectangle((190,230,322,282),radius=18,outline=accent,width=7)


def render(control_id,state,inventory,icon_dir):
    status=inventory.get(control_id,{}).get("runtimeStatus","implemented")
    hidden=control_id=="LED_indicator"
    disabled=control_id in RESERVED or status=="not_implemented"
    if hidden: return Image.new("RGBA",(SIZE,SIZE),(0,0,0,0))
    effective="disabled" if disabled else state
    face,border,accent=colors(effective,disabled)
    im=Image.new("RGBA",(SIZE,SIZE),(0,0,0,0)); d=ImageDraw.Draw(im)
    if control_id.startswith("knob"): draw_knob(d,face,border,accent,effective)
    elif control_id.startswith("fx_switch"): draw_switch(d,face,border,accent,effective)
    elif control_id=="preset_browser": draw_browser(d,face,border,accent,effective)
    elif control_id=="mode_selector": draw_mode(d,face,border,accent,effective)
    elif control_id=="midi_keyboard": draw_keyboard(d,face,border,accent,effective)
    elif control_id=="LCD_display": draw_lcd(d,face,border,accent,effective)
    else:
        name={"SKIN_button":"skins","Import_Guitar":"import_guitar","Auto_Discover":"auto_discover",
              "Curator":"curator","Settings":"settings","About":"about"}.get(control_id)
        p=icon_dir/f"{name}.png" if name else None
        icon=Image.open(p).convert("RGBA") if p and p.is_file() else None
        draw_button(d,face,border,accent,effective,icon)
    return im


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root",type=Path,default=Path("SKIN")); ap.add_argument("--check",action="store_true")
    a=ap.parse_args()
    schema=json.loads((a.root/"control_schema.json").read_text(encoding="utf-8"))["x-ppgu-contract"]
    ids=tuple(schema["requiredControlIds"]); inventory=schema.get("runtimeControlInventory",{})
    skin=a.root/"Skin_053"; icon_dir=skin/"assets/icons"; errors=[]; records=[]
    for cid in ids:
        for state in STATES:
            im=render(cid,state,inventory,icon_dir); b=io.BytesIO()
            im.save(b,format="PNG",optimize=False,compress_level=9); data=b.getvalue()
            for tier,rel in (("master",f"assets/master/controls/{cid}_{state}.png"),
                             ("runtime",f"assets/controls/{cid}_{state}.png")):
                path=skin/rel
                if a.check:
                    if not path.is_file() or path.read_bytes()!=data: errors.append(str(path))
                else:
                    path.parent.mkdir(parents=True,exist_ok=True); path.write_bytes(data)
                if path.is_file():
                    raw=path.read_bytes()
                    records.append({"id":f"{cid}.{state}.{tier}","controlId":cid,"state":state,
                        "tier":tier,"path":rel,"bytes":len(raw),"dimensions":"512x512",
                        "sha256":hashlib.sha256(raw).hexdigest(),"crc32":f"{zlib.crc32(raw)&0xffffffff:08X}",
                        "runtimeStatus":inventory.get(cid,{}).get("runtimeStatus","implemented"),
                        "renderMode":"transparent_hidden" if cid=="LED_indicator" else "disabled_static" if cid in RESERVED else "stateful",
                        "origin":"[DECISION] original NEON STAGE control sprite"})
    manifest={"schemaVersion":"1.0.0","skinId":"skin-053","assetGroup":"control-state-sprites",
              "states":list(STATES),"controlIds":list(ids),"records":records}
    path=skin/"assets/control_manifest.json"; data=(json.dumps(manifest,indent=2,ensure_ascii=False)+"\n").encode()
    if a.check:
        if not path.is_file() or path.read_bytes()!=data: errors.append(str(path))
    else:
        path.parent.mkdir(parents=True,exist_ok=True); path.write_bytes(data)
    if errors:
        print("REV18 control sprite mismatch: "+", ".join(errors[:20])); return 1
    print(f"REV18 SKIN-1 controls: {'verified' if a.check else 'generated'} ({len(ids)} IDs x {len(STATES)} states x master/runtime)")
    return 0


if __name__=="__main__": raise SystemExit(main())
