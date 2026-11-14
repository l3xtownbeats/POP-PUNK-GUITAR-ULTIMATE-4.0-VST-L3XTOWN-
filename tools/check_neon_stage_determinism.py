#!/usr/bin/env python3
"""Byte-for-byte determinism gate for NEON STAGE staging assets and ZIP."""
from __future__ import annotations
import hashlib
import sys
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT))
import neon_stage_engine as engine

OUT=ROOT/"dist"/"neon_stage_staging"
DIST=ROOT/"dist"
CONTACTS=(DIST/"NEON_STAGE_ATLAS_CONTACT_SHEET.png",
          DIST/"NEON_STAGE_REFERENCE_CONTACT_SHEET.png")
REPORTS=(DIST/"NEON_STAGE_BUILD_REPORT.json",)
ZIP=DIST/"NEON_STAGE_DEFAULT_SKIN_1_STAGING.zip"
SELF_CHECK=OUT/"Skin_052"/"SELF_CHECK.txt"

def digest(path:Path)->str:
    h=hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda:f.read(1024*1024),b""):
            h.update(block)
    return h.hexdigest()

def snapshot():
    files={}
    for path in sorted(OUT.rglob("*")):
        if path.is_file():
            files[path.relative_to(ROOT).as_posix()]=digest(path)
    for path in (*CONTACTS,*REPORTS):
        if path.is_file():
            files[path.relative_to(ROOT).as_posix()]=digest(path)
    return files

def main():
    before=snapshot()
    required=("palette_report.json","Skin_052/theme.json","Skin_052/control_map.json",
              "Skin_052/LICENSE.txt","Skin_052/SELF_CHECK.txt")
    missing=[name for name in required if not (OUT/name).is_file()]
    if missing:
        raise SystemExit("Determinism gate requires completed staging outputs: "+", ".join(missing))
    # Repeat the complete pipeline in the same deterministic order. step_ui also
    # invokes the indexed atlas generator; no runtime or registry files are touched.
    for name in ("palette","knobs","ui","svg","promo","contact","manifest","report"):
        engine.STEPS[name]()
    after=snapshot()
    if before!=after:
        keys=sorted(set(before)|set(after))
        changed=[k for k in keys if before.get(k)!=after.get(k)]
        print("NEON STAGE DETERMINISM: FAIL")
        for key in changed[:100]:
            print(f" - {key}: {before.get(key)} -> {after.get(key)}")
        raise SystemExit(1)
    # Record the byte-for-byte result in SELF_CHECK (excluded from LICENSE hash scope).
    current=SELF_CHECK.read_text(encoding="utf-8")
    current += "DETERMINISM: PASS — repeated assets, manifests, reports and contact sheets are byte-identical.\n"
    SELF_CHECK.write_text(current,encoding="utf-8")
    # Verify deterministic ZIP creation twice with fixed timestamps and sorted members.
    engine.step_package()
    first=digest(ZIP)
    engine.step_package()
    second=digest(ZIP)
    if first!=second:
        raise SystemExit(f"NEON STAGE ZIP determinism failed: {first} != {second}")
    print("NEON STAGE DETERMINISM: PASS")
    print(f"STAGING ZIP SHA-256: {second}")

if __name__=="__main__":
    main()
