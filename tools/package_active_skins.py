#!/usr/bin/env python3
"""Stage only the 16 registry-referenced factory skins beside the VST3 bundle."""
from __future__ import annotations
import argparse
import json
import shutil
from pathlib import Path

REQUIRED_ASSETS = ("preview", "hero", "texture", "bezel", "crest")

def safe_source(root: Path, relative: str) -> Path:
    rel = Path(relative)
    if rel.is_absolute() or not relative or any(part in ("", ".", "..") for part in rel.parts):
        raise ValueError(f"unsafe relative asset path: {relative!r}")
    candidate = root / rel
    if candidate.is_symlink():
        raise ValueError(f"symbolic-link asset path is not allowed: {relative}")
    source = candidate.resolve()
    root_real = root.resolve()
    if not source.is_relative_to(root_real) or not source.is_file():
        raise ValueError(f"missing, linked, or out-of-root source file: {relative}")
    return source

def copy_one(root: Path, output: Path, relative: str) -> None:
    source = safe_source(root, relative)
    destination = output / Path(relative)
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, destination)

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path("SKIN"))
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    root = args.root.resolve()
    output_candidate = args.output.absolute()
    if output_candidate.is_symlink():
        raise SystemExit("output path must not be a symbolic link")
    output = output_candidate.resolve()
    if output == root or root.is_relative_to(output):
        raise SystemExit("output must be a separate build/staging directory, not the SKIN source root or its parent")

    registry_path = safe_source(root, "theme_manifest.json")
    registry = json.loads(registry_path.read_text(encoding="utf-8"))
    sequence_path = safe_source(root, "approved_theme_sequence.json")
    sequence = json.loads(sequence_path.read_text(encoding="utf-8"))
    approved = sequence.get("themes", [])
    entries = registry.get("themes", [])
    if registry.get("totalSkins") != 16 or len(entries) != 16:
        raise SystemExit("active registry must contain exactly 16 factory skins")
    if registry.get("defaultSkin") != "skin-053":
        raise SystemExit("active registry default must be skin-053 (NEON STAGE)")
    if len(approved) != 16:
        raise SystemExit("approved_theme_sequence.json must contain exactly 16 themes")

    ordinals = [entry.get("ordinal") for entry in entries]
    if ordinals != list(range(1, 17)):
        raise SystemExit("active registry ordinals must be exactly 1..16 in order")

    # Stage into a clean directory so no stale/legacy skin folders can survive.
    if output.exists():
        if output.is_symlink() or output == Path(output.anchor):
            raise SystemExit("refusing to clean an unsafe output path")
        shutil.rmtree(output)
    output.mkdir(parents=True, exist_ok=True)

    runtime_registry = dict(registry)
    runtime_registry.pop("legacyRegistryArchive", None)
    (output / "theme_manifest.json").write_text(
        json.dumps(runtime_registry, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8")
    copy_one(root, output, "control_schema.json")
    copy_one(root, output, "approved_theme_sequence.json")

    approved_by_ordinal = {item.get("ordinal"): item for item in approved if isinstance(item, dict)}
    seen_ids = set()
    seen_indices = set()
    expected_folders = set()
    for entry in entries:
        ordinal = entry.get("ordinal")
        stable_id = entry.get("id")
        index = entry.get("index")
        name = entry.get("name")
        if not isinstance(ordinal, int) or not isinstance(index, int):
            raise SystemExit(f"invalid ordinal/index in registry entry: {entry}")
        approved_entry = approved_by_ordinal.get(ordinal)
        if not isinstance(approved_entry, dict) or (
            approved_entry.get("stableId"), approved_entry.get("index"),
            approved_entry.get("name"), approved_entry.get("path")
        ) != (stable_id, index, name, entry.get("path")):
            raise SystemExit(f"registry/approved-sequence mismatch for ordinal {ordinal}")
        if stable_id != f"skin-{index:03d}" or index < 52:
            raise SystemExit(f"stable ID/index mismatch for {name}: {stable_id} / {index}")
        if stable_id in seen_ids or index in seen_indices:
            raise SystemExit(f"duplicate stable ID/index in active registry: {stable_id}")
        seen_ids.add(stable_id); seen_indices.add(index)

        manifest_rel = entry.get("path", "")
        manifest_source = safe_source(root, manifest_rel)
        theme = json.loads(manifest_source.read_text(encoding="utf-8"))
        if (theme.get("id"), theme.get("index"), theme.get("name")) != (stable_id, index, name):
            raise SystemExit(f"theme manifest identity mismatch for ordinal {ordinal}: {manifest_rel}")
        if theme.get("schemaVersion") != "1.0.0":
            raise SystemExit(f"unsupported theme schema for {name}")

        folder = Path(manifest_rel).parent.as_posix()
        if folder in expected_folders:
            raise SystemExit(f"duplicate source folder in active registry: {folder}")
        expected_folders.add(folder)
        copy_one(root, output, manifest_rel)

        assets = theme.get("assets", {})
        for key in REQUIRED_ASSETS:
            relative = assets.get(key)
            if not isinstance(relative, str) or not relative:
                raise SystemExit(f"{name}: missing {key} asset path")
            source_relative = (Path(folder) / relative).as_posix()
            copy_one(root, output, source_relative)

        if entry.get("preview") != (Path(folder) / assets["preview"]).as_posix():
            raise SystemExit(f"{name}: registry preview path mismatch")
        if entry.get("hero") != (Path(folder) / assets["hero"]).as_posix():
            raise SystemExit(f"{name}: registry hero path mismatch")

    actual_folders = {
        p.name for p in output.iterdir()
        if p.is_dir() and p.name.startswith("Skin_")
    }
    if actual_folders != expected_folders:
        raise SystemExit(f"staged folders differ from active registry: {sorted(actual_folders ^ expected_folders)}")
    if len(actual_folders) != 16:
        raise SystemExit(f"expected 16 staged skin folders, found {len(actual_folders)}")

    print("ACTIVE SKIN PACKAGE: PASS")
    print("Staged exactly 16 registry-referenced themes and their 5 required assets each.")
    print("Legacy folders, archive manifests and unregistered themes are excluded.")
    print(f"Output: {output}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
