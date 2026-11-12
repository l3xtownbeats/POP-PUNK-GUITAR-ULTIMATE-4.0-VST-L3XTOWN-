#!/usr/bin/env python3
"""Safely merge a validated REV14 Skin Studio pack into the canonical registry.

Legacy theme.schema.json remains the schema for legacy themes. New Skin Studio
manifests are governed by control_schema.json; this merge never rewrites one
schema to pretend the two manifest formats are interchangeable.
"""
from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path


def fail(message: str, code: int = 2) -> int:
    print(message, file=sys.stderr)
    return code


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path("SKIN"))
    parser.add_argument("--manifest", type=Path, default=Path("SKIN/theme_pack_manifest.json"))
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    root = args.root
    registry_path = root / "theme_manifest.json"
    legacy_schema_path = root / "theme.schema.json"
    studio_schema_path = root / "control_schema.json"
    validator = Path("tools/validate_skin_pack.py")

    if not validator.is_file():
        return fail(f"Skin pack validator not found: {validator}")
    if not registry_path.is_file() or not legacy_schema_path.is_file() or not studio_schema_path.is_file():
        return fail("Registry or one of the versioned theme schemas is missing.")
    if not args.manifest.is_file():
        return fail(f"Skin pack manifest not found: {args.manifest}")

    result = subprocess.run(
        [sys.executable, str(validator), "--root", str(root), "--manifest", str(args.manifest)],
        capture_output=True, text=True
    )
    if result.returncode:
        print(result.stdout, end="")
        print(result.stderr, end="", file=sys.stderr)
        return result.returncode

    try:
        registry = json.loads(registry_path.read_text(encoding="utf-8"))
        pack = json.loads(args.manifest.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return fail(f"Cannot read merge inputs: {exc}")

    existing = registry.get("themes")
    if not isinstance(existing, list) or not existing:
        return fail("Registry themes must be a non-empty array.")
    existing_total = registry.get("totalSkins")
    if not isinstance(existing_total, int) or isinstance(existing_total, bool) or existing_total != len(existing):
        return fail("Registry totalSkins does not match its themes array.")
    if pack.get("status") != "GENERATED_NOT_MERGED":
        return fail("Only a GENERATED_NOT_MERGED pack may be merged.")

    existing_ids = [item.get("id") for item in existing if isinstance(item, dict)]
    existing_indices = [item.get("index") for item in existing if isinstance(item, dict)]
    if len(existing_ids) != len(existing) or len(existing_indices) != len(existing):
        return fail("Registry contains malformed theme entries.")
    if len(set(existing_ids)) != existing_total or len(set(existing_indices)) != existing_total:
        return fail("Registry contains duplicate IDs or indices.")
    if sorted(existing_indices) != list(range(1, existing_total + 1)):
        return fail("Registry indices are not continuous from 1 to totalSkins.")

    start, end, count = pack.get("startIndex"), pack.get("endIndex"), pack.get("totalSkins")
    themes = pack.get("themes")
    if (not isinstance(themes, list) or not themes
            or start != existing_total + 1
            or end != existing_total + len(themes)
            or count != len(themes)):
        return fail("Registry changed or pack range/count is inconsistent; refusing merge.")

    additions = []
    for item in sorted(themes, key=lambda value: value.get("index", -1)):
        if not isinstance(item, dict):
            return fail("Pack contains a malformed theme entry.")
        index = item.get("index")
        folder = root / f"Skin_{index:03d}"
        try:
            theme = json.loads((folder / "theme.json").read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError) as exc:
            return fail(f"Cannot read {folder / 'theme.json'}: {exc}")
        additions.append({
            "id": theme["id"],
            "index": theme["index"],
            "name": theme["name"],
            "family": theme["family"],
            "layout": theme["layout"],
            "path": f"Skin_{theme['index']:03d}/theme.json",
            "preview": f"Skin_{theme['index']:03d}/assets/preview.png",
            "hero": f"Skin_{theme['index']:03d}/assets/hero.png",
        })

    all_ids = existing_ids + [item["id"] for item in additions]
    all_indices = existing_indices + [item["index"] for item in additions]
    if len(set(all_ids)) != len(all_ids) or len(set(all_indices)) != len(all_indices):
        return fail("Duplicate theme ID or index; refusing merge.")
    if sorted(all_indices) != list(range(1, existing_total + len(additions) + 1)):
        return fail("Merged registry would not have continuous indices; refusing merge.")

    new_registry = dict(registry)
    new_registry["themes"] = existing + additions
    new_registry["totalSkins"] = len(new_registry["themes"])
    new_registry["skinRange"] = {"first": 1, "last": new_registry["totalSkins"]}

    if args.dry_run:
        print(result.stdout, end="")
        print(f"DRY RUN: would merge {len(additions)} theme(s); total={new_registry['totalSkins']}.")
        print("No files or backups were changed.")
        return 0

    backup_dir = root / "backups"
    backup_dir.mkdir(parents=True, exist_ok=True)
    stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%S%fZ")
    backup_paths = [
        (registry_path, backup_dir / f"theme_manifest.{stamp}.json.bak"),
        (legacy_schema_path, backup_dir / f"theme.schema.legacy.{stamp}.json.bak"),
        (studio_schema_path, backup_dir / f"control_schema.{stamp}.json.bak"),
        (args.manifest, backup_dir / f"theme_pack_manifest.{stamp}.json.bak"),
    ]
    for source, destination in backup_paths:
        shutil.copy2(source, destination)

    pack["status"] = "MERGED"
    pack["mergedRegistryTotal"] = new_registry["totalSkins"]

    registry_temp = registry_path.with_suffix(".json.tmp")
    pack_temp = args.manifest.with_suffix(args.manifest.suffix + ".tmp")
    try:
        registry_temp.write_text(
            json.dumps(new_registry, indent=2, ensure_ascii=False) + "\\n", encoding="utf-8"
        )
        pack_temp.write_text(
            json.dumps(pack, indent=2, ensure_ascii=False) + "\\n", encoding="utf-8"
        )
        registry_temp.replace(registry_path)
        pack_temp.replace(args.manifest)
    except Exception:
        registry_temp.unlink(missing_ok=True)
        pack_temp.unlink(missing_ok=True)
        shutil.copy2(backup_paths[0][1], registry_path)
        shutil.copy2(backup_paths[3][1], args.manifest)
        raise

    print(f"MERGED {len(additions)} theme(s); registry total={new_registry['totalSkins']}.")
    print(f"Pack status set to MERGED; backups created in {backup_dir}.")
    print("Legacy and Skin Studio schemas were preserved independently.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
