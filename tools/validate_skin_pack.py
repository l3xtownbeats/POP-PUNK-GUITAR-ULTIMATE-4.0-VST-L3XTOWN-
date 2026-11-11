#!/usr/bin/env python3
"""Validate PPGU REV 14 skin packs (G1-G4). Uses only Python standard library."""
from __future__ import annotations
import argparse, json, re, struct, sys, xml.etree.ElementTree as ET
from pathlib import Path

def fail(message: str) -> None:
    raise ValueError(message)

def load_json(path: Path):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        fail(f"{path}: invalid JSON: {exc}")

def channel(hex_value: str) -> float:
    v = int(hex_value, 16) / 255.0
    return v / 12.92 if v <= 0.04045 else ((v + 0.055) / 1.055) ** 2.4

def luminance(colour: str) -> float:
    value = colour.lstrip("#")
    if not re.fullmatch(r"[0-9A-Fa-f]{6}", value):
        fail(f"invalid colour {colour!r}")
    r, g, b = (channel(value[i:i+2]) for i in (0, 2, 4))
    return 0.2126*r + 0.7152*g + 0.0722*b

def contrast(a: str, b: str) -> float:
    x, y = sorted((luminance(a), luminance(b)), reverse=True)
    return (x + 0.05) / (y + 0.05)

def png_dimensions(path: Path):
    data = path.read_bytes()
    if len(data) < 24 or data[:8] != b"\x89PNG\r\n\x1a\n" or data[12:16] != b"IHDR":
        fail(f"{path}: not a valid PNG")
    return struct.unpack(">II", data[16:24])

def safe_svg(path: Path):
    raw = path.read_text(encoding="utf-8")
    if re.search(r"<\s*script|\bon[a-z]+\s*=|(?:href|src)\s*=\s*['\"]https?://", raw, re.I):
        fail(f"{path}: active script/event/external URL is forbidden")
    try:
        root = ET.fromstring(raw)
    except ET.ParseError as exc:
        fail(f"{path}: invalid SVG: {exc}")
    if not root.tag.endswith("svg"):
        fail(f"{path}: root element is not svg")

def validate(root: Path, pack_manifest_path: Path) -> list[str]:
    schema=load_json(root/"control_schema.json")
    registry=load_json(root/"theme_manifest.json")
    pack=load_json(pack_manifest_path)
    required=schema.get("x-ppgu-contract",{}).get("requiredControlIds",[])
    tokens=schema.get("x-ppgu-contract",{}).get("paletteTokens",[])
    if not required or len(required)!=len(set(required)):
        fail("control_schema.json has no unique requiredControlIds")
    if len(tokens)!=21 or len(tokens)!=len(set(tokens)):
        fail("control_schema.json must define exactly 21 unique palette tokens")
    existing_total=registry.get("totalSkins")
    if not isinstance(existing_total,int):
        fail("theme_manifest.json totalSkins is not an integer")
    entries=pack.get("themes")
    if not isinstance(entries,list) or not entries:
        fail("theme_pack_manifest.json must contain a non-empty themes array")
    start,end=pack.get("startIndex"),pack.get("endIndex")
    if pack.get("totalSkins")!=len(entries) or start!=existing_total+1 or end!=existing_total+len(entries):
        fail("pack range/count is not continuous with current registry")
    indices=[]
    for entry in entries:
        index=entry.get("index")
        if not isinstance(index,int): fail("pack theme index must be integer")
        indices.append(index)
        folder=root/f"Skin_{index:03d}"
        theme_path=folder/"theme.json"
        if not theme_path.is_file(): fail(f"missing {theme_path}")
        theme=load_json(theme_path)
        if theme.get("index")!=index or theme.get("id")!=f"skin-{index:03d}":
            fail(f"{theme_path}: ID/index mismatch")
        if entry.get("id")!=theme.get("id") or entry.get("name")!=theme.get("name"):
            fail(f"{theme_path}: pack manifest identity mismatch")
        palette=theme.get("palette",{})
        if set(palette)!=set(tokens): fail(f"{theme_path}: palette must contain exactly the 21 contract tokens")
        for token in tokens: luminance(palette[token])
        for fg,bg,minimum in [
            ("text_primary","window_bg",4.0),("text_secondary","window_bg",4.0),
            ("text_disabled","window_bg",4.0),("lcd_text","lcd_bg",4.0),("accent","panel",2.2)
        ]:
            ratio=contrast(palette[fg],palette[bg])
            if ratio < minimum: fail(f"{theme_path}: {fg}/{bg} contrast {ratio:.2f} < {minimum:.1f}")
        controls=theme.get("controls",{})
        missing=sorted(set(required)-set(controls))
        if missing: fail(f"{theme_path}: missing control IDs: {', '.join(missing)}")
        for control_id in required:
            grid=controls[control_id].get("grid",{})
            x,y,w,h=(grid.get(k) for k in ("x","y","w","h"))
            if not all(isinstance(v,int) for v in (x,y,w,h)) or min(x,y)<0 or min(w,h)<1 or x+w>12 or y+h>16:
                fail(f"{theme_path}: invalid grid bounds for {control_id}")
        if theme.get("reduceInEco") is not True or theme.get("hotSwapFadeMs")!=150:
            fail(f"{theme_path}: ECO/hotSwap contract mismatch")
        if theme.get("family") not in ("carbon_studio","neon_basement","chrome_parade","wood_summer","vintage_tape","sunset_suburb","special"):
            fail(f"{theme_path}: invalid family")
        if theme.get("layout") not in ("STUDIO_CONSOLE","WORKSTATION_TOPBAR","RACK_SIDE","COMPACT_MINI"):
            fail(f"{theme_path}: invalid layout")
        assets=theme.get("assets",{})
        preview=folder/assets.get("preview","")
        hero=folder/assets.get("hero","")
        if not preview.is_file() or png_dimensions(preview)!=(360,240):
            fail(f"{theme_path}: preview must exist and be 360x240 PNG")
        if not hero.is_file() or png_dimensions(hero)!=(800,450):
            fail(f"{theme_path}: hero must exist and be 800x450 PNG")
        for key in ("texture","bezel","crest"):
            asset=folder/"svg"/f"{key}.svg"
            if not asset.is_file(): fail(f"{theme_path}: missing {asset}")
            safe_svg(asset)
    if sorted(indices)!=list(range(start,end+1)) or len(indices)!=len(set(indices)):
        fail("pack theme IDs are not unique and continuous")
    return [f"PASS {len(entries)} theme(s)","PASS G1 contrast","PASS G2 control parity","PASS G3 PNG/SVG assets","PASS G4 identity and continuous numbering"]

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument("--root",type=Path,default=Path("SKIN"))
    parser.add_argument("--manifest",type=Path,default=Path("SKIN/theme_pack_manifest.json"))
    args=parser.parse_args()
    try:
        for line in validate(args.root,args.manifest): print(line)
    except Exception as exc:
        print(f"FAIL: {exc}",file=sys.stderr); return 1
    return 0

if __name__=="__main__":
    raise SystemExit(main())
