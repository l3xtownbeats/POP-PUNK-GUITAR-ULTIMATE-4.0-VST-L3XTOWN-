#!/usr/bin/env python3
"""G1-G4 staging validator for the complete NEON STAGE REV ULTRA 3.0 asset pack."""
from __future__ import annotations
import argparse
import hashlib
import importlib.util
import json
import re
import struct
import sys
import xml.etree.ElementTree as ET
import zlib
from pathlib import Path
from PIL import Image

ROOT=Path(__file__).resolve().parents[1]
SKIN=ROOT/"dist"/"neon_stage_staging"/"Skin_052"
REGISTRY=ROOT/"SKIN"/"theme_manifest.json"
SOURCE=ROOT/"SKIN"/"Skin_052"/"theme.json"
EXPECTED_TOKENS={
"bg_deep":"#0A0E18","bg_panel":"#141B2A","bg_panel_hi":"#1E2A40",
"border":"#2A3A55","border_glow":"#22CFFF","text_primary":"#EAF6FF",
"text_secondary":"#9FB6D4","text_disabled":"#5A6B85","accent_primary":"#22CFFF",
"accent_secondary":"#FF3EA5","accent_warning":"#FFB454","accent_danger":"#FF5A5A",
"accent_success":"#46E88C","lcd_bg":"#061021","lcd_text":"#7FE8FF",
"knob_body":"#0C1220","knob_ring":"#22CFFF","led_on":"#46E8FF","led_off":"#1A2436",
"overlay_scrim":"#04080FCC","focus_ring":"#7FE8FF"}
EXPECTED_PNG={
"assets/preview.png":(360,240),"assets/hero.png":(800,450),
"assets/background.png":(1600,920),
"assets/atlas/knobs_main.png":(1664,3456),
"assets/atlas/knobs_main@2x.png":(3328,6912),
"assets/atlas/knobs_amp.png":(1248,1152),
"assets/atlas/knobs_amp@2x.png":(2496,2304),
"assets/atlas/fx_tiles.png":(980,336),"assets/atlas/toggles.png":(216,96),
"assets/atlas/keys_white.png":(138,160),"assets/atlas/keys_black.png":(84,96),
"assets/atlas/meters.png":(100,20),"assets/atlas/window_chrome.png":(48,48),
"assets/atlas/icons.png":(160,128),"assets/atlas/panel_header.png":(480,44),
"assets/atlas/lcd_panel.png":(320,54),"assets/atlas/setup_only_badge.png":(112,24),
"assets/atlas/gallery_tile.png":(260,160),"assets/atlas/tabs.png":(336,36),
"assets/atlas/transport.png":(120,40),"assets/atlas/gr_column.png":(28,180),
"assets/atlas/output_column.png":(28,180)}
EXPECTED_JSON={
"assets/atlas/knobs_main.json":351,"assets/atlas/knobs_main@2x.json":351,
"assets/atlas/knobs_amp.json":117,"assets/atlas/knobs_amp@2x.json":117,
"assets/atlas/fx_tiles.json":28,"assets/atlas/toggles.json":9,
"assets/atlas/keys_white.json":3,"assets/atlas/keys_black.json":3,
"assets/atlas/meters.json":10,"assets/atlas/window_chrome.json":4,
"assets/atlas/icons.json":20,"assets/atlas/sliders.json":8,
"assets/atlas/tabs.json":3,"assets/atlas/transport.json":3,
"assets/atlas/gr_column.json":1,"assets/atlas/output_column.json":1}
WINDOWS=("autochords","fx_edit","skins_gallery","import_guitar","settings",
         "auto_discover_setup_only","curator_setup_only","about_setup_only")
def sha(path):
    h=hashlib.sha256()
    with open(path,"rb") as f:
        for chunk in iter(lambda:f.read(1024*1024),b""):h.update(chunk)
    return h.hexdigest()
def png_check(path,expected):
    raw=path.read_bytes()
    if raw[:8]!=b"\x89PNG\r\n\x1a\n":raise ValueError(f"{path}: invalid PNG signature")
    off=8;w=h=depth=ctype=None;idat=bytearray();palette=0;saw_end=False
    while off+12<=len(raw):
        n=struct.unpack(">I",raw[off:off+4])[0];kind=raw[off+4:off+8];end=off+12+n
        if end>len(raw):raise ValueError(f"{path}: truncated chunk")
        payload=raw[off+8:off+8+n];crc=struct.unpack(">I",raw[off+8+n:end])[0]
        if zlib.crc32(kind+payload)&0xffffffff!=crc:raise ValueError(f"{path}: CRC mismatch in {kind!r}")
        if kind==b"IHDR":w,h,depth,ctype,_,_,_=struct.unpack(">IIBBBBB",payload)
        elif kind==b"PLTE":palette=n//3
        elif kind==b"IDAT":idat.extend(payload)
        elif kind==b"IEND":saw_end=True;off=end;break
        off=end
    if not saw_end or (w,h)!=expected:raise ValueError(f"{path}: expected dimensions {expected}, got {(w,h)}")
    if depth!=8 or ctype!=3 or palette<1 or palette>256:raise ValueError(f"{path}: expected indexed 8-bit PNG-8, got depth={depth}, type={ctype}, palette={palette}")
    with Image.open(path) as im:im.verify()
def safe_svg(path):
    raw=path.read_text(encoding="utf-8")
    if re.search(r"<\s*(script|foreignObject)\b|<!DOCTYPE|<!ENTITY|\bon[a-z]+\s*=|\b(?:href|src)\s*=",raw,re.I):
        raise ValueError(f"{path}: script, event, entity or external reference detected")
    root=ET.fromstring(raw)
    if not root.tag.endswith("svg"):raise ValueError(f"{path}: root is not SVG")
def contrast(a,b):
    def lum(v):
        s=v[1:7];c=[int(s[i:i+2],16)/255 for i in (0,2,4)]
        q=[x/12.92 if x<=.04045 else ((x+.055)/1.055)**2.4 for x in c]
        return .2126*q[0]+.7152*q[1]+.0722*q[2]
    hi,lo=sorted((lum(a),lum(b)),reverse=True)
    return (hi+.05)/(lo+.05)
def validate(determinism=False):
    errors=[];checks={}
    source=json.loads(SOURCE.read_text(encoding="utf-8"))
    theme=json.loads((SKIN/"theme.json").read_text(encoding="utf-8"))
    if theme.get("id")!="neon-stage-default-skin-1" or theme.get("index")!=52 or theme.get("name")!="NEON STAGE":
        errors.append("G4: staging theme identity/index/name mismatch")
    if theme.get("palette")!=EXPECTED_TOKENS:errors.append("G1: palette differs from the exact REV ULTRA 3.0 token registry")
    ratios={}
    for fg,bg,minimum in (("text_primary","bg_deep",7),("text_secondary","bg_deep",4.5),
                          ("text_disabled","bg_deep",3),("lcd_text","lcd_bg",7),
                          ("accent_primary","bg_panel",3)):
        ratio=contrast(EXPECTED_TOKENS[fg],EXPECTED_TOKENS[bg]);ratios[f"{fg}/{bg}"]=round(ratio,2)
        if ratio<minimum:errors.append(f"G1: {fg}/{bg} {ratio:.2f}:1 < {minimum}:1")
    checks["G1_contrast"]=ratios
    # G2: exact control inventory, unique IDs, 12x16 bounds and no overlap.
    control_map=json.loads((SKIN/"control_map.json").read_text(encoding="utf-8"))
    controls=control_map.get("controls",{})
    source_controls=source.get("controls",{})
    if len(controls)!=28 or set(controls)!=set(source_controls):
        errors.append(f"G2: expected exact 28-control parity; got {len(controls)}")
    if control_map.get("grid")!={"columns":12,"rows":16}:
        errors.append("G2: grid must be exactly 12x16")
    rects=[]
    for cid,item in controls.items():
        x,y,w,h=(item.get(k) for k in ("col","row","colSpan","rowSpan"))
        if not all(isinstance(v,int) and not isinstance(v,bool) for v in (x,y,w,h)) or x<0 or y<0 or w<1 or h<1 or x+w>12 or y+h>16:
            errors.append(f"G2: invalid/out-of-bounds rectangle for {cid}");continue
        for other,box in rects:
            if x<box[0]+box[2] and box[0]<x+w and y<box[1]+box[3] and box[1]<y+h:
                errors.append(f"G2: overlap {other}/{cid}")
        rects.append((cid,(x,y,w,h)))
        if item.get("apvtsBinding") is not None and not isinstance(item.get("apvtsBinding"),str):
            errors.append(f"G2: invalid APVTS binding value for {cid}")
    checks["G2_control_count"]=len(controls)
    # G3: all required PNGs, CRCs, dimensions and palette-indexed art.
    for rel,size in EXPECTED_PNG.items():
        p=SKIN/rel
        try:
            if not p.is_file():raise FileNotFoundError(p)
            png_check(p,size)
        except Exception as exc:errors.append(f"G3: {exc}")
    checks["G3_png_count"]=len(EXPECTED_PNG)
    # WebP hero must decode and be exactly 800x450.
    try:
        with Image.open(SKIN/"assets/hero.webp") as im:
            if im.format!="WEBP" or im.size!=(800,450):raise ValueError("hero.webp must be 800x450 WebP")
            im.verify()
    except Exception as exc:errors.append(f"G3: {exc}")
    svg_paths=[SKIN/"svg"/n for n in ("texture.svg","bezel.svg","crest.svg","panel_header.svg")]
    svg_paths += [SKIN/"assets"/"windows"/f"window_{n}.svg" for n in WINDOWS]
    for p in svg_paths:
        try:
            if not p.is_file():raise FileNotFoundError(p)
            safe_svg(p)
        except Exception as exc:errors.append(f"G3: {exc}")
    checks["G3_svg_count"]=len(svg_paths)
    for rel,count in EXPECTED_JSON.items():
        p=SKIN/rel
        try:
            data=json.loads(p.read_text(encoding="utf-8"))
            frames=data.get("frames",[])
            if len(frames)!=count:raise ValueError(f"expected {count} indexed frames, got {len(frames)}")
            names=[f.get("name") for f in frames]
            if len(names)!=len(set(names)):raise ValueError("duplicate frame names")
            atlas_name=data.get("atlas")
            png_path=p.with_suffix(".png")
            if atlas_name in ("gr_column","output_column"):png_path=SKIN/"assets"/"atlas"/f"{atlas_name}.png"
            with Image.open(png_path) as im:aw,ah=im.size
            for f in frames:
                x,y,w,h=(f.get(k) for k in ("x","y","w","h"))
                if not all(isinstance(v,int) and not isinstance(v,bool) for v in (x,y,w,h)) or x<0 or y<0 or w<1 or h<1 or x+w>aw or y+h>ah:
                    raise ValueError(f"frame outside atlas: {f.get('name')}")
        except Exception as exc:errors.append(f"G3 index {rel}: {exc}")
    checks["G3_index_files"]=len(EXPECTED_JSON)
    for name in ("LICENSE.txt","SELF_CHECK.txt"):
        if not (SKIN/name).is_file():errors.append(f"G3 missing {name}")
    # Provenance ledger must match every generated package file except itself and SELF_CHECK.
    try:
        license_text=(SKIN/"LICENSE.txt").read_text(encoding="utf-8")
        declared={}
        for line in license_text.splitlines():
            m=re.fullmatch(r"([0-9a-f]{64})  (.+)",line)
            if m:declared[m.group(2)]=m.group(1)
        actual={p.relative_to(SKIN).as_posix():sha(p) for p in sorted(SKIN.rglob("*"))
                if p.is_file() and p.name not in ("LICENSE.txt","SELF_CHECK.txt")}
        if declared!=actual:
            missing=sorted(set(actual)-set(declared));extra=sorted(set(declared)-set(actual))
            changed=sorted(k for k in set(actual)&set(declared) if actual[k]!=declared[k])
            errors.append(f"G3 LICENSE hash mismatch missing={missing} extra={extra} changed={changed}")
    except Exception as exc:errors.append(f"G3 LICENSE invalid: {exc}")
    # G4: validate against the actual authoritative registry; never invent a free index.
    try:
        registry=json.loads(REGISTRY.read_text(encoding="utf-8"))
        entries=registry.get("themes",[])
        active=[x for x in entries if x.get("id")=="neon-stage-default-skin-1"]
        indices=[x.get("index") for x in entries]
        ids=[x.get("id") for x in entries]
        if len(active)!=1 or active[0].get("index")!=52 or active[0].get("path")!="Skin_052/theme.json":
            errors.append("G4: authoritative registry does not map NEON STAGE DEFAULT SKIN-1 to Skin_052/index 52")
        if len(indices)!=len(set(indices)) or sorted(indices)!=list(range(1,len(entries)+1)):
            errors.append("G4: authoritative registry indices are not unique/continuous")
        if len(ids)!=len(set(ids)):errors.append("G4: duplicate registry IDs")
        if registry.get("defaultSkin")!="neon-stage-default-skin-1":errors.append("G4: defaultSkin is not canonical NEON STAGE")
        if theme.get("path") is not None:errors.append("G4: staging manifest must not invent a second runtime path field")
    except Exception as exc:errors.append(f"G4 registry invalid: {exc}")
    checks["G4_registry_entries"]=len(entries) if "entries" in locals() else 0
    # Optional byte-for-byte deterministic rerun. Generator rewrites SELF_CHECK to PENDING;
    # that file is intentionally excluded from the hash comparison and finalized below.
    if determinism:
        before={p.relative_to(SKIN).as_posix():sha(p) for p in SKIN.rglob("*")
                if p.is_file() and p.name!="SELF_CHECK.txt"}
        spec=importlib.util.spec_from_file_location("neon_stage_generator",ROOT/"tools"/"generate_neon_stage_rev_ultra.py")
        module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module);module.generate()
        after={p.relative_to(SKIN).as_posix():sha(p) for p in SKIN.rglob("*")
               if p.is_file() and p.name!="SELF_CHECK.txt"}
        if before!=after:errors.append("G3 deterministic regeneration mismatch")
        checks["deterministic_byte_match"]=before==after
    if errors:
        print("NEON STAGE REV ULTRA 3.0: FAIL")
        for e in errors:print(" - "+e)
        raise SystemExit(1)
    license_hash=sha(SKIN/"LICENSE.txt")
    self_check=[
        "NEON STAGE REV ULTRA 3.0 SELF-CHECK",
        "STATUS: PASS",
        "G1: PASS — WCAG palette contrast thresholds verified",
        "G2: PASS — exact 28 control IDs, 12x16 bounds, no overlaps",
        "G3: PASS — PNG CRC/dimensions/PNG-8, SVG safety, atlas indices and SHA-256 provenance verified",
        "G4: PASS — canonical registry identity/index/path and continuous registry verified",
        f"LICENSE_SHA256: {license_hash}",
        f"DETERMINISM: {'PASS' if checks.get('deterministic_byte_match',False) else 'NOT_RUN'}",
        "LIMITATION: staging asset contract only; no JUCE build, CTest, pluginval or FL Studio 20 host acceptance is implied."
    ]
    (SKIN/"SELF_CHECK.txt").write_text("\n".join(self_check)+"\n",encoding="utf-8")
    print(json.dumps({"status":"PASS","checks":checks},indent=2))
    print("NEON STAGE REV ULTRA 3.0: PASS")
if __name__=="__main__":
    ap=argparse.ArgumentParser();ap.add_argument("--determinism",action="store_true")
    args=ap.parse_args();validate(args.determinism)
