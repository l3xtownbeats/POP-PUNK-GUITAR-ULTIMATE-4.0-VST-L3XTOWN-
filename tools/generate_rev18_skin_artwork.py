#!/usr/bin/env python3
"""Create deterministic original NEON STAGE artwork at native master/runtime sizes."""
from __future__ import annotations
import argparse, hashlib, io, json, zlib
from pathlib import Path
from PIL import Image, ImageDraw, ImageFilter

P = {"window_bg":(11,15,18),"panel":(20,27,32),"panel_elevated":(29,41,48),
     "control_border":(83,101,107),"text_primary":(243,247,245),
     "text_secondary":(183,197,194),"accent":(213,255,69),
     "accent_glow":(222,255,110),"lowlight":(13,20,23)}
SPECS = {
 "hero_master":("assets/master/hero/hero.png",(1600,900),"hero"),
 "hero_runtime":("assets/hero/hero.png",(800,450),"hero"),
 "preview_master":("assets/master/preview/preview.png",(720,480),"preview"),
 "preview_runtime":("assets/preview/preview.png",(360,240),"preview"),
 "background_master":("assets/master/background/main.png",(2560,1440),"background"),
 "background_runtime":("assets/background/main.png",(2560,1440),"background"),
 "texture_master":("assets/master/textures/carbon_weave.png",(1024,1024),"texture"),
 "texture_runtime":("assets/textures/carbon_weave.png",(1024,1024),"texture"),
 "glow_master":("assets/master/fx_layers/stage_glow.png",(1600,900),"glow"),
 "glow_runtime":("assets/fx_layers/stage_glow.png",(800,450),"glow"),
 "edge_master":("assets/master/fx_layers/edge_highlight.png",(1600,900),"edge"),
 "edge_runtime":("assets/fx_layers/edge_highlight.png",(800,450),"edge"),
}


def carbon_texture():
    tile=Image.new("RGB",(32,32),P["lowlight"])
    for y in range(32):
        for x in range(32):
            band=((x+y)//8)%2; v=18 if band else 12
            if x%8 in (0,1) or y%8 in (0,1): v+=3
            tile.putpixel((x,y),(v,v+5,v+7))
    out=Image.new("RGB",(1024,1024))
    for y in range(0,1024,32):
        for x in range(0,1024,32): out.paste(tile,(x,y))
    return out


def background(size,focal=True):
    scale=2 if focal else 1; w,h=size[0]*scale,size[1]*scale
    im=Image.new("RGB",(w,h)); px=im.load(); top=P["window_bg"]; bottom=P["panel"]
    for y in range(h):
        t=y/max(1,h-1); c=tuple(round(top[i]*(1-t)+bottom[i]*t*.28) for i in range(3))
        for x in range(w):
            weave=1 if ((x+y)//9)%2 else 0
            px[x,y]=tuple(min(255,v+weave) for v in c)
    glow=Image.new("RGBA",(w,h),(0,0,0,0)); gd=ImageDraw.Draw(glow)
    gd.ellipse((w*.38,-h*.65,w*1.12,h*.78),fill=(*P["accent_glow"],24))
    gd.ellipse((-w*.28,h*.05,w*.5,h*1.05),fill=(60,100,110,18))
    glow=glow.filter(ImageFilter.GaussianBlur(max(12,int(w*.045))))
    im=Image.alpha_composite(im.convert("RGBA"),glow)
    beams=Image.new("RGBA",(w,h),(0,0,0,0)); bd=ImageDraw.Draw(beams)
    bd.polygon([(w*.18,0),(w*.23,0),(w*.54,h),(w*.44,h)],fill=(183,197,194,7))
    bd.polygon([(w*.66,0),(w*.70,0),(w*.89,h),(w*.80,h)],fill=(213,255,69,8))
    for i in range(7):
        x=int(w*(.06+i*.145))
        bd.line((x,0,int(x-w*.22),h),fill=(213,255,69,25 if i==3 else 12),width=max(1,w//1600*2))
    im=Image.alpha_composite(im,beams)
    if focal:
        d=ImageDraw.Draw(im)
        pts=[(.62,.45),(.59,.36),(.56,.34),(.54,.37),(.55,.49),(.50,.47),(.465,.49),(.445,.54),(.46,.62),(.49,.68),(.48,.76),(.52,.83),(.59,.89),(.65,.90),(.72,.85),(.76,.77),(.74,.69),(.775,.62),(.78,.55),(.76,.50),(.72,.48),(.68,.49),(.69,.38),(.66,.34),(.63,.36)]
        pts=[(w*x,h*y) for x,y in pts]
        d.polygon(pts,fill=(*P["panel_elevated"],245))
        d.line(pts+[pts[0]],fill=(*P["control_border"],240),width=max(2,w//450),joint="curve")
        d.polygon([(w*.615,h*.47),(w*.655,h*.43),(w*.875,h*.105),(w*.835,h*.075),(w*.595,h*.425)],fill=(*P["panel"],255),outline=(*P["control_border"],235))
        d.polygon([(w*.835,h*.075),(w*.905,h*.012),(w*.955,h*.075),(w*.885,h*.155)],fill=(*P["panel_elevated"],255),outline=(*P["control_border"],235))
        for k in range(4):
            off=(k-1.5)*w*.008
            d.line((w*.89+off,h*.09,w*.625+off,h*.785),fill=(*P["accent_glow"],175 if k==1 else 95),width=max(1,w//1300))
        for t in (.22,.30,.38,.46,.54,.62,.70):
            x0=w*(.67+t*.18); y0=h*(.39-t*.22)
            d.line((x0-w*.012,y0-h*.006,x0+w*.012,y0+h*.006),fill=(*P["control_border"],185),width=max(1,w//1500))
        d.rounded_rectangle((w*.565,h*.595,w*.675,h*.635),radius=max(2,w//300),fill=(*P["lowlight"],255),outline=(*P["accent"],175),width=max(1,w//1000))
        d.rounded_rectangle((w*.575,h*.705,w*.675,h*.742),radius=max(2,w//300),fill=(*P["lowlight"],255),outline=(*P["control_border"],210),width=max(1,w//1000))
        d.arc((w*.515,h*.52,w*.705,h*.85),285,70,fill=(*P["accent"],105),width=max(1,w//1000))
        for r in (w*.09,w*.14,w*.19):
            d.arc((w*.12-r,h*.50-r,w*.12+r,h*.50+r),300,55,fill=(*P["accent"],45),width=max(1,w//1200))
        d.line((w*.08,h*.82,w*.38,h*.82),fill=(*P["accent"],110),width=max(1,w//900))
        d.line((w*.08,h*.845,w*.25,h*.845),fill=(*P["text_secondary"],65),width=max(1,w//1400))
    vignette=Image.new("L",(w,h),0); vd=ImageDraw.Draw(vignette)
    vd.ellipse((-w*.1,-h*.25,w*1.1,h*1.25),fill=115)
    vignette=vignette.filter(ImageFilter.GaussianBlur(max(10,int(w*.08))))
    shade=Image.new("RGBA",(w,h),(0,0,0,85)); shade.putalpha(Image.eval(vignette,lambda v:85-v//2))
    im=Image.alpha_composite(im,shade).convert("RGB")
    return im.resize(size,Image.Resampling.LANCZOS) if scale>1 else im


def fx(size,kind):
    w,h=size; im=Image.new("RGBA",size,(0,0,0,0)); d=ImageDraw.Draw(im)
    if kind=="glow":
        d.ellipse((w*.34,h*.08,w*.90,h*.92),fill=(*P["accent"],32))
        im=im.filter(ImageFilter.GaussianBlur(max(8,int(w*.055))))
    else:
        d.line((w*.02,h*.04,w*.98,h*.04),fill=(*P["text_secondary"],70),width=max(1,w//500))
        d.line((w*.02,h*.96,w*.98,h*.96),fill=(*P["accent"],55),width=max(1,w//700))
        im=im.filter(ImageFilter.GaussianBlur(max(1,int(w*.002))))
    return im


def atlas(skin):
    names=("skins","import_guitar","auto_discover","curator","settings","about")
    image=Image.new("RGBA",(768,512),(0,0,0,0)); frames={}
    for i,name in enumerate(names):
        x=(i%3)*256; y=(i//3)*256; p=skin/"assets/icons"/f"{name}.png"
        if not p.is_file(): raise FileNotFoundError(p)
        image.alpha_composite(Image.open(p).convert("RGBA"),(x,y))
        frames[name]={"x":x,"y":y,"w":256,"h":256}
    return image,{"schemaVersion":"1.0.0","skinId":"skin-001","image":"atlas.png","width":768,"height":512,"frames":frames}


def record(skin,key,rel,role,dims):
    raw=(skin/rel).read_bytes()
    return {"id":f"skin1.{key}","path":rel,"format":"PNG","bytes":len(raw),
            "dimensions":f"{dims[0]}x{dims[1]}","sha256":hashlib.sha256(raw).hexdigest(),
            "crc32":f"{zlib.crc32(raw)&0xffffffff:08X}",
            "origin":"[DECISION] original NEON STAGE artwork rendered at native resolution","role":role}


def main():
    ap=argparse.ArgumentParser(description=__doc__); ap.add_argument("--root",type=Path,default=Path("SKIN")); ap.add_argument("--check",action="store_true"); a=ap.parse_args()
    skin=a.root/"Skin_001"; errors=[]; expected={}
    for key,(rel,size,kind) in SPECS.items():
        im=background(size,True) if kind in ("hero","preview") else background(size,False) if kind=="background" else carbon_texture() if kind=="texture" else fx(size,kind)
        b=io.BytesIO(); im.save(b,format="PNG",optimize=False,compress_level=9); expected[rel]=b.getvalue()
    atlas_image,atlas_json=atlas(skin)
    b=io.BytesIO(); atlas_image.save(b,format="PNG",optimize=False,compress_level=9); expected["atlas/atlas.png"]=b.getvalue()
    expected["atlas/atlas.json"]=(json.dumps(atlas_json,indent=2,ensure_ascii=False)+"\n").encode()
    for rel,data in expected.items():
        path=skin/rel
        if a.check:
            if not path.is_file() or path.read_bytes()!=data: errors.append(str(path))
        else:
            path.parent.mkdir(parents=True,exist_ok=True); path.write_bytes(data)
    records=[]
    for key,(rel,size,kind) in SPECS.items():
        if (skin/rel).is_file(): records.append(record(skin,key,rel,kind,size))
    for key,rel,fmt,dims,role in (("atlas_png","atlas/atlas.png","PNG",(768,512),"texture-atlas"),("atlas_json","atlas/atlas.json","JSON",None,"texture-atlas-frame-map")):
        raw=(skin/rel).read_bytes()
        r={"id":f"skin1.{key}","path":rel,"format":fmt,"bytes":len(raw),"sha256":hashlib.sha256(raw).hexdigest(),"crc32":f"{zlib.crc32(raw)&0xffffffff:08X}","origin":"[DECISION] generated from approved SKIN-1 top-bar icon assets","role":role}
        if dims: r["dimensions"]=f"{dims[0]}x{dims[1]}"
        records.append(r)
    manifest={"schemaVersion":"1.0.0","skinId":"skin-001","theme":"NEON STAGE DEFAULT","assetGroup":"hero-preview-background-texture-fx-atlas","creationDecision":"[DECISION] Original carbon-stage electric-guitar artwork created because approved high-resolution masters were absent.","records":records}
    manifest_path=skin/"assets/artwork_manifest.json"; manifest_data=(json.dumps(manifest,indent=2,ensure_ascii=False)+"\n").encode()
    if a.check:
        if not manifest_path.is_file() or manifest_path.read_bytes()!=manifest_data: errors.append(str(manifest_path))
    else:
        manifest_path.parent.mkdir(parents=True,exist_ok=True); manifest_path.write_bytes(manifest_data)
    if errors:
        print("REV18 artwork mismatch: "+", ".join(errors)); return 1
    print(f"REV18 SKIN-1 artwork: {'verified' if a.check else 'generated'}; {len(SPECS)} native-resolution PNGs + atlas")
    return 0

if __name__=="__main__": raise SystemExit(main())
