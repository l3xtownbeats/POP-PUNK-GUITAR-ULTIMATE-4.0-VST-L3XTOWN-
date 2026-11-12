#!/usr/bin/env python3
"""Merge a validated PPGU skin pack into theme_manifest.json with timestamped backups."""
from __future__ import annotations
import argparse, json, shutil, subprocess, sys
from datetime import datetime, timezone
from pathlib import Path

def verify_runtime_rev14_support(root: Path, pack: dict) -> None:
    """Fail closed until schema AND runtime can consume REV14 palette packages."""
    schema_path = root / "theme.schema.json"
    runtime_header = Path("Source/UI/ThemeManager.h")
    runtime_source = Path("Source/UI/ThemeManager.cpp")
    try:
        schema_text = schema_path.read_text(encoding="utf-8")
        header_text = runtime_header.read_text(encoding="utf-8")
        source_text = runtime_source.read_text(encoding="utf-8")
    except OSError as exc:
        raise RuntimeError(f"REV14 runtime preflight cannot read required contract files: {exc}") from exc

    # These are deliberate capability gates, not version guesses: a merge is
    # forbidden until the JSON schema declares the palette format and the
    # ThemeManager exposes both the REV14 parser and token lookup API.
    missing = []
    if '"palette"' not in schema_text:
        missing.append("theme.schema.json REV14 palette branch")
    if "validateRev14Manifest" not in source_text:
        missing.append("ThemeManager REV14 manifest parser")
    if "token(" not in header_text:
        missing.append("ThemeManager::token() lookup API")
    if not missing:
        return

    details = ", ".join(missing)
    raise RuntimeError(
        "Refusing skin-pack merge: runtime integration is incomplete (" + details + "). "
        "A staged pack must not be written into the legacy runtime registry."
    )


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument("--root",type=Path,default=Path("SKIN"))
    parser.add_argument("--manifest",type=Path,default=Path("SKIN/theme_pack_manifest.json"))
    parser.add_argument("--dry-run",action="store_true")
    args=parser.parse_args()
    root=args.root
    try:
        pack_preflight = json.loads(args.manifest.read_text(encoding="utf-8"))
        if pack_preflight.get("status") != "DESIGNED_NOT_RUNTIME_INTEGRATED":
            raise RuntimeError("Pack status must be DESIGNED_NOT_RUNTIME_INTEGRATED before runtime merge.")
        verify_runtime_rev14_support(root, pack_preflight)
    except (OSError, json.JSONDecodeError, RuntimeError) as exc:
        print(f"MERGE PREFLIGHT FAILED: {exc}", file=sys.stderr)
        return 4

    validator=Path("tools/validate_skin_pack.py")
    result=subprocess.run([sys.executable,str(validator),"--root",str(root),"--manifest",str(args.manifest)],capture_output=True,text=True)
    if result.returncode:
        print(result.stdout+result.stderr,file=sys.stderr); return result.returncode
    registry_path=root/"theme_manifest.json"
    schema_path=root/"theme.schema.json"
    registry=json.loads(registry_path.read_text(encoding="utf-8"))
    pack=json.loads(args.manifest.read_text(encoding="utf-8"))
    existing=registry.get("themes",[])
    if registry.get("totalSkins")!=pack["startIndex"]-1:
        print("Registry changed since pack validation; refusing merge.",file=sys.stderr); return 2
    additions=[]
    for item in sorted(pack["themes"],key=lambda x:x["index"]):
        folder=root/f"Skin_{item['index']:03d}"
        theme=json.loads((folder/"theme.json").read_text(encoding="utf-8"))
        additions.append({"id":theme["id"],"index":theme["index"],"name":theme["name"],
                          "family":theme["family"],"layout":theme["layout"],
                          "path":f"Skin_{theme['index']:03d}/theme.json",
                          "preview":f"Skin_{theme['index']:03d}/assets/preview.png",
                          "hero":f"Skin_{theme['index']:03d}/assets/hero.png"})
    ids={x["id"] for x in existing}
    if any(x["id"] in ids for x in additions):
        print("Duplicate skin ID; refusing merge.",file=sys.stderr); return 3
    new_registry=dict(registry)
    new_registry["themes"]=existing+additions
    new_registry["totalSkins"]=len(new_registry["themes"])
    new_registry["skinRange"]={"first":min(x["index"] for x in new_registry["themes"]),
                               "last":max(x["index"] for x in new_registry["themes"])}
    if args.dry_run:
        print(f"DRY RUN: would merge {len(additions)} skin(s); total={new_registry['totalSkins']}")
        return 0
    backup_dir=root/"backups"
    backup_dir.mkdir(parents=True,exist_ok=True)
    stamp=datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    shutil.copy2(registry_path,backup_dir/f"theme_manifest.{stamp}.json.bak")
    shutil.copy2(schema_path,backup_dir/f"theme.schema.{stamp}.json.bak")
    registry_path.write_text(json.dumps(new_registry,indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
    print(f"MERGED {len(additions)} skin(s); registry total={new_registry['totalSkins']}; backup={backup_dir}")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
