#!/usr/bin/env python3
"""Read-only audit for the REV 18 NEON STAGE SKIN-1 migration.

This tool never renames, deletes, copies, or edits skin assets or the registry.
It reports whether the current registry can safely be migrated to the binding
identity SKIN-1 = NEON STAGE = skin-053/Skin_053.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from pathlib import Path, PurePosixPath

CANONICAL_ID = "skin-053"
CANONICAL_INDEX = 53
CANONICAL_NAME = "NEON STAGE"
CANONICAL_PATH = "Skin_053/theme.json"
OLD_DEFAULT_PATH = "Skin_052/theme.json"


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def safe_relative_path(value: str) -> bool:
    candidate = PurePosixPath(value)
    return (bool(value) and not candidate.is_absolute()
            and ".." not in candidate.parts and "\\" not in value)


def read_json(path: Path, label: str, issues: list[dict]) -> dict | None:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        issues.append({"severity": "BLOCKER", "code": "JSON_READ_FAILED",
                       "object": label, "detail": str(error)})
        return None
    if not isinstance(value, dict):
        issues.append({"severity": "BLOCKER", "code": "JSON_ROOT_NOT_OBJECT",
                       "object": label, "detail": "Expected a JSON object"})
        return None
    return value


def audit(skin_root: Path) -> dict:
    issues: list[dict] = []
    registry_path = skin_root / "theme_manifest.json"
    registry = read_json(registry_path, "theme_manifest.json", issues)
    if registry is None:
        return {"status": "BLOCKED", "issues": issues}

    entries = registry.get("themes")
    total = registry.get("totalSkins")
    if not isinstance(entries, list):
        issues.append({"severity": "BLOCKER", "code": "THEMES_NOT_ARRAY",
                       "object": "theme_manifest.json", "detail": "themes must be an array"})
        entries = []
    if not isinstance(total, int) or isinstance(total, bool):
        issues.append({"severity": "BLOCKER", "code": "TOTAL_NOT_INTEGER",
                       "object": "theme_manifest.json", "detail": "totalSkins must be an integer"})
        total = -1
    if total != len(entries):
        issues.append({"severity": "BLOCKER", "code": "TOTAL_MISMATCH",
                       "object": "theme_manifest.json",
                       "detail": f"totalSkins={total}, entries={len(entries)}"})

    ids: dict[str, int] = {}
    indices: dict[int, int] = {}
    entry_by_index: dict[int, dict] = {}
    resolved: list[dict] = []
    for position, entry in enumerate(entries):
        if not isinstance(entry, dict):
            issues.append({"severity": "BLOCKER", "code": "ENTRY_NOT_OBJECT",
                           "object": f"themes[{position}]", "detail": "Expected object"})
            continue
        skin_id = entry.get("id")
        index = entry.get("index")
        path_value = entry.get("path")
        if not isinstance(skin_id, str) or not skin_id:
            issues.append({"severity": "BLOCKER", "code": "INVALID_ID",
                           "object": f"themes[{position}]", "detail": repr(skin_id)})
        elif skin_id in ids:
            issues.append({"severity": "BLOCKER", "code": "DUPLICATE_ID",
                           "object": skin_id, "detail": f"positions {ids[skin_id]} and {position}"})
        else:
            ids[skin_id] = position
        if not isinstance(index, int) or isinstance(index, bool) or index < 1:
            issues.append({"severity": "BLOCKER", "code": "INVALID_INDEX",
                           "object": f"themes[{position}]", "detail": repr(index)})
            continue
        if index in indices:
            issues.append({"severity": "BLOCKER", "code": "DUPLICATE_INDEX",
                           "object": str(index), "detail": f"positions {indices[index]} and {position}"})
        else:
            indices[index] = position
            entry_by_index[index] = entry
        if not isinstance(path_value, str) or not safe_relative_path(path_value):
            issues.append({"severity": "BLOCKER", "code": "UNSAFE_MANIFEST_PATH",
                           "object": str(skin_id), "detail": repr(path_value)})
            continue
        manifest_path = skin_root / Path(*PurePosixPath(path_value).parts)
        exists = manifest_path.is_file()
        resolved.append({"id": skin_id, "index": index, "path": path_value,
                         "exists": exists,
                         "sha256": sha256(manifest_path) if exists else None})
        if not exists:
            issues.append({"severity": "BLOCKER", "code": "MISSING_THEME_MANIFEST",
                           "object": str(skin_id), "detail": path_value})

    expected_indices = set(range(1, total + 1)) if total >= 0 else set()
    actual_indices = set(indices)
    if expected_indices != actual_indices:
        issues.append({"severity": "BLOCKER", "code": "NON_CONTIGUOUS_INDICES",
                       "object": "theme_manifest.json",
                       "detail": f"missing={sorted(expected_indices - actual_indices)}, "
                                 f"unexpected={sorted(actual_indices - expected_indices)}"})

    default_id = registry.get("defaultSkin")
    if default_id != CANONICAL_ID:
        issues.append({"severity": "BLOCKER", "code": "WRONG_DEFAULT_ID",
                       "object": "theme_manifest.json",
                       "detail": f"defaultSkin={default_id!r}; required {CANONICAL_ID!r}"})

    canonical_entry = entry_by_index.get(CANONICAL_INDEX)
    if not isinstance(canonical_entry, dict):
        issues.append({"severity": "BLOCKER", "code": "CANONICAL_INDEX_MISSING",
                       "object": "theme_manifest.json", "detail": "index 53 is absent"})
    else:
        actual = (canonical_entry.get("id"), canonical_entry.get("name"),
                  canonical_entry.get("path"))
        expected = (CANONICAL_ID, CANONICAL_NAME, CANONICAL_PATH)
        if actual != expected:
            issues.append({"severity": "BLOCKER", "code": "CANONICAL_ENTRY_MISMATCH",
                           "object": "theme_manifest.json",
                           "detail": f"actual={actual!r}; required={expected!r}"})

    for relative, expected in (
        (CANONICAL_PATH, (CANONICAL_ID, CANONICAL_INDEX, CANONICAL_NAME)),
        (OLD_DEFAULT_PATH, None),
    ):
        path = skin_root / relative
        if not path.is_file():
            issues.append({"severity": "BLOCKER", "code": "MIGRATION_SOURCE_MISSING",
                           "object": relative, "detail": "Required source manifest is absent"})
            continue
        manifest = read_json(path, relative, issues)
        if manifest is None:
            continue
        actual = (manifest.get("id"), manifest.get("index"), manifest.get("name"))
        if expected is not None and actual != expected:
            issues.append({"severity": "BLOCKER", "code": "CANONICAL_FILE_IDENTITY_MISMATCH",
                           "object": relative, "detail": f"actual={actual!r}; required={expected!r}"})
        if relative == OLD_DEFAULT_PATH:
            issues.append({"severity": "REVIEW", "code": "PRESERVE_PREVIOUS_DEFAULT",
                           "object": relative,
                           "detail": f"Existing manifest identity={actual!r}; preserve its bytes and assets"})
        if relative == CANONICAL_PATH and actual != expected:
            issues.append({"severity": "REVIEW", "code": "SKIN_053_COLLISION",
                           "object": relative,
                           "detail": f"Current Skin_053 identity={actual!r}; never overwrite without archival backup"})

    # Stable legacy numeric IDs must remain untouched; display numbering is a
    # separate mapping and must not be implemented by renaming these IDs.
    stable_ids = []
    for index in range(54, 103):
        entry = entry_by_index.get(index)
        if not isinstance(entry, dict):
            continue
        stable_id = f"skin-{index:03d}"
        if entry.get("id") != stable_id:
            issues.append({"severity": "BLOCKER", "code": "STABLE_ID_CHANGED",
                           "object": f"index {index}",
                           "detail": f"actual={entry.get('id')!r}; expected stable ID {stable_id!r}"})
        stable_ids.append({"index": index, "id": stable_id,
                           "display_number": index - 52})

    legacy = [entry for entry in entries if isinstance(entry, dict)
              and isinstance(entry.get("index"), int) and 1 <= entry["index"] <= 51]
    if legacy:
        issues.append({"severity": "REVIEW", "code": "LEGACY_SELECTOR_POLICY",
                       "object": "indices 1..51",
                       "detail": f"{len(legacy)} historical entries remain in the live registry; "
                                 "verify they are archival/non-selectable before promotion"})

    blockers = sum(item["severity"] == "BLOCKER" for item in issues)
    return {
        "status": "PASS" if blockers == 0 else "BLOCKED",
        "registry": {"defaultSkin": default_id, "totalSkins": total,
                     "entryCount": len(entries)},
        "canonical_target": {"id": CANONICAL_ID, "index": CANONICAL_INDEX,
                             "name": CANONICAL_NAME, "path": CANONICAL_PATH},
        "preservation": {
            "skin_052_sha256": sha256(skin_root / OLD_DEFAULT_PATH)
                if (skin_root / OLD_DEFAULT_PATH).is_file() else None,
            "skin_053_sha256": sha256(skin_root / CANONICAL_PATH)
                if (skin_root / CANONICAL_PATH).is_file() else None,
            "stable_ids_54_102": stable_ids,
        },
        "resolved_manifests": resolved,
        "issues": issues,
        "mutation_performed": False,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skin-root", type=Path, default=Path("SKIN"))
    args = parser.parse_args()
    report = audit(args.skin_root)
    print(json.dumps(report, ensure_ascii=False, indent=2))
    return 0 if report["status"] == "PASS" else 2


if __name__ == "__main__":
    sys.exit(main())
