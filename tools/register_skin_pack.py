#!/usr/bin/env python3
"""PPGU Skin Pack Registrar — REV ULTRA 1.3.

Imports a prebuilt, contiguous skin pack into the canonical registry.
Dry-run is the default. The existing G1-G4 validator is the acceptance gate.
No schema maxima are guessed or rewritten: control_schema.json and the runtime
skin schema are separate contracts and are not mutated by this tool.
"""
from __future__ import annotations

import argparse
import copy
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


def read_json(path: Path):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        raise ValueError(f"{path}: invalid/unreadable JSON: {exc}") from exc


def write_json_atomic(path: Path, value) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, name = tempfile.mkstemp(prefix=f".{path.name}.", suffix=".tmp", dir=path.parent)
    try:
        with os.fdopen(fd, "w", encoding="utf-8", newline="\n") as stream:
            json.dump(value, stream, indent=2, ensure_ascii=False)
            stream.write("\n")
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(name, path)
    except Exception:
        try:
            os.unlink(name)
        except FileNotFoundError:
            pass
        raise


def validate_registry(registry: dict) -> list[dict]:
    if not isinstance(registry, dict):
        raise ValueError("theme_manifest.json must be a JSON object")
    themes = registry.get("themes")
    total = registry.get("totalSkins")
    if not isinstance(themes, list) or not isinstance(total, int) or isinstance(total, bool):
        raise ValueError("canonical registry requires themes[] and integer totalSkins")
    if len(themes) != total:
        raise ValueError(f"totalSkins={total}, but themes contains {len(themes)} entries")

    indices, ids, paths = set(), set(), set()
    for entry in themes:
        if not isinstance(entry, dict):
            raise ValueError("registry contains a non-object theme entry")
        index, skin_id, path = entry.get("index"), entry.get("id"), entry.get("path")
        if not isinstance(index, int) or isinstance(index, bool) or index < 1:
            raise ValueError(f"invalid registry index: {index!r}")
        if not isinstance(skin_id, str) or not skin_id:
            raise ValueError(f"invalid registry ID at index {index}")
        if not isinstance(path, str) or not path:
            raise ValueError(f"invalid registry path at index {index}")
        if index in indices or skin_id in ids or path in paths:
            raise ValueError(f"duplicate index/ID/path in registry at index {index}")
        indices.add(index)
        ids.add(skin_id)
        paths.add(path)

    if indices != set(range(1, total + 1)):
        raise ValueError("registry indices must be continuous from 1 through totalSkins")
    default_id = registry.get("defaultSkin")
    if default_id not in ids:
        raise ValueError(f"defaultSkin {default_id!r} is not present in registry")
    return themes


def validate_pack_manifest(pack: dict, current_total: int) -> list[dict]:
    if not isinstance(pack, dict):
        raise ValueError("pack manifest must be a JSON object")
    if pack.get("status") != "GENERATED_NOT_MERGED":
        raise ValueError("only GENERATED_NOT_MERGED packs can be imported")
    entries = pack.get("themes")
    start, end, count = pack.get("startIndex"), pack.get("endIndex"), pack.get("totalSkins")
    if not isinstance(entries, list) or not entries:
        raise ValueError("pack manifest must contain a non-empty themes array")
    if any(not isinstance(x, int) or isinstance(x, bool) for x in (start, end, count)):
        raise ValueError("pack startIndex/endIndex/totalSkins must be integers")
    if count != len(entries) or end != start + count - 1:
        raise ValueError("pack range/count is inconsistent")
    if start != current_total + 1:
        raise ValueError(
            f"pack must start at {current_total + 1}; refusing to renumber or overwrite themes"
        )
    indices = [entry.get("index") if isinstance(entry, dict) else None for entry in entries]
    if indices != list(range(start, end + 1)):
        raise ValueError("pack entries must be ordered and continuous")
    return entries


def safe_source_folder(source_root: Path, entry: dict) -> tuple[str, Path]:
    rel = entry.get("path")
    if not isinstance(rel, str) or not rel.endswith("/theme.json"):
        raise ValueError(f"invalid theme path in pack manifest: {rel!r}")
    folder_name = rel.rsplit("/", 1)[0]
    if Path(folder_name).name != folder_name or not folder_name.startswith("Skin_"):
        raise ValueError(f"unsafe skin folder path: {folder_name!r}")
    source = (source_root / folder_name).resolve()
    root = source_root.resolve()
    if source == root or root not in source.parents:
        raise ValueError(f"source folder escapes source root: {folder_name}")
    return folder_name, source


def prepare_staging(root: Path, source_root: Path, registry: dict,
                    entries: list[dict], pack_path: Path) -> Path:
    stage = Path(tempfile.mkdtemp(prefix=".ppgu_skin_import_", dir=root.parent))
    try:
        (stage / "control_schema.json").write_bytes((root / "control_schema.json").read_bytes())
        (stage / "theme_manifest.json").write_text(
            json.dumps(registry, indent=2, ensure_ascii=False) + "\n", encoding="utf-8"
        )
        for entry in entries:
            folder_name, source = safe_source_folder(source_root, entry)
            if not source.is_dir():
                raise ValueError(f"missing source folder: {source}")
            destination = stage / folder_name
            if destination.exists():
                raise ValueError(f"duplicate staged folder: {folder_name}")
            shutil.copytree(source, destination)
        staged_pack = stage / "theme_pack_manifest.json"
        shutil.copy2(pack_path, staged_pack)
        return stage
    except Exception:
        shutil.rmtree(stage, ignore_errors=True)
        raise


def run_gates(stage: Path, validator: Path) -> None:
    if not validator.is_file():
        raise ValueError(f"official G1-G4 validator not found: {validator}")
    command = [
        sys.executable, str(validator), "--root", str(stage),
        "--manifest", str(stage / "theme_pack_manifest.json"),
    ]
    result = subprocess.run(command, check=False)
    if result.returncode != 0:
        raise ValueError(f"G1-G4 validator failed with exit code {result.returncode}")


def apply_transaction(root: Path, registry_path: Path, pack_path: Path,
                      registry: dict, pack: dict, entries: list[dict],
                      stage: Path) -> None:
    themes = registry["themes"]
    existing_total = registry["totalSkins"]
    new_registry = copy.deepcopy(registry)
    new_registry["themes"] = themes + copy.deepcopy(entries)
    new_registry["totalSkins"] = existing_total + len(entries)

    new_pack = copy.deepcopy(pack)
    new_pack["status"] = "MERGED"
    new_pack["mergedRegistryTotal"] = new_registry["totalSkins"]

    promoted: list[Path] = []
    registry_before = registry_path.read_bytes()
    pack_before = pack_path.read_bytes()
    try:
        for entry in entries:
            folder_name = Path(entry["path"]).parent.name
            destination = root / folder_name
            staged_folder = stage / folder_name
            if destination.exists():
                raise ValueError(f"target already exists; no overwrite allowed: {destination}")
            if not staged_folder.is_dir():
                raise ValueError(f"validated staging folder disappeared: {staged_folder}")
            # Promote the exact bytes that passed G1-G4; do not recopy mutable sources.
            os.replace(staged_folder, destination)
            promoted.append(destination)

        write_json_atomic(registry_path, new_registry)
        write_json_atomic(pack_path, new_pack)
    except Exception:
        for destination in reversed(promoted):
            if destination.is_dir():
                shutil.rmtree(destination)
            elif destination.exists():
                destination.unlink()
        # Restore exact original bytes, including formatting and key order.
        registry_path.write_bytes(registry_before)
        pack_path.write_bytes(pack_before)
        raise


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path("SKIN"))
    parser.add_argument("--source-root", type=Path, default=Path("ASSETS_PREBUILT"))
    parser.add_argument("--manifest", type=Path, default=Path("ASSETS_PREBUILT/theme_pack_manifest.json"))
    parser.add_argument("--validator", type=Path, default=Path("tools/validate_skin_pack.py"))
    parser.add_argument("--apply", action="store_true", help="commit filesystem changes; default is dry-run")
    args = parser.parse_args()

    root = args.root.resolve()
    source_root = args.source_root.resolve()
    registry_path = root / "theme_manifest.json"
    pack_path = args.manifest.resolve()
    try:
        if not registry_path.is_file():
            raise ValueError(f"registry not found: {registry_path}")
        if not (root / "control_schema.json").is_file():
            raise ValueError(f"control schema not found: {root / 'control_schema.json'}")
        if not pack_path.is_file():
            raise ValueError(f"generated pack manifest not found: {pack_path}")

        registry = read_json(registry_path)
        pack = read_json(pack_path)
        existing = validate_registry(registry)
        entries = validate_pack_manifest(pack, registry["totalSkins"])

        existing_ids = {x["id"] for x in existing}
        existing_paths = {x["path"] for x in existing}
        for entry in entries:
            if not isinstance(entry, dict):
                raise ValueError("pack contains a malformed theme entry")
            if entry.get("id") in existing_ids or entry.get("path") in existing_paths:
                raise ValueError(f"duplicate registry identity/path: {entry.get('id')!r}")
            index = entry.get("index")
            if not isinstance(index, int) or isinstance(index, bool):
                raise ValueError("pack theme index must be an integer")
            expected = f"skin-{index:03d}"
            if entry.get("id") != expected or entry.get("path") != f"Skin_{index:03d}/theme.json":
                raise ValueError(f"pack identity/path mismatch at index {index}")

        stage = prepare_staging(root, source_root, registry, entries, pack_path)
        try:
            run_gates(stage, args.validator.resolve())
        finally:
        print(f"PASS: G1-G4 accepted {len(entries)} theme(s), indices "
              f"{entries[0]['index']}..{entries[-1]['index']}.")
        print(f"Registry total after import: {registry['totalSkins'] + len(entries)}")
        print(f"Default skin preserved: {registry['defaultSkin']}")
        if args.apply:
            apply_transaction(root, registry_path, pack_path, registry, pack, entries, stage)
            print("APPLY: registration committed; pack status changed to MERGED.")
        else:
            print("DRY-RUN: no files changed. Re-run with --apply to register.")
        return 0
    except Exception as exc:
        print(f"ABORT: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
