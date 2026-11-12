#!/usr/bin/env python3
"""Generate a truthful per-file Sound Hunter inventory from the staged SoundBank.

Only audio files physically present under Samples are registered. Every record
contains a content hash and points to the source folder's license evidence.
This inventory does not claim that the ANGELA playback engine has integrated
or auditioned a file; runtime playback status remains explicitly unverified.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
from datetime import datetime, timezone
from pathlib import Path

AUDIO_EXTENSIONS = {".wav", ".flac", ".ogg", ".aiff", ".aif", ".mp3"}
SOURCES = {
    "Meta-meta-steel-guitar": ("Meta Open Sound Steel Guitar", "https://github.com/facebookincubator/Meta-open-sound", "LICENSE"),
    "Meta-meta-nylon-guitar": ("Meta Open Sound Nylon Guitar", "https://github.com/facebookincubator/Meta-open-sound", "LICENSE"),
    "FreePats-Clean": ("FreePats FSBS Clean Electric Guitar", "https://github.com/freepats/electric-guitar-FSBS-clean", ("LICENSE.txt", "LICENSE")),
    "FreePats-Distortion-1": ("FreePats FSBS Distortion 1", "https://github.com/freepats/electric-guitar-FSBS-dist1", ("LICENSE.txt", "LICENSE")),
    "FreePats-Distortion-2": ("FreePats FSBS Distortion 2", "https://github.com/freepats/electric-guitar-FSBS-dist2", ("LICENSE.txt", "LICENSE")),
    "FreePats-Jazz-Electric": ("FreePats Jazz Electric Guitar", "https://github.com/freepats/electric-guitar-FSBS-jazz", ("LICENSE.txt", "LICENSE")),
    "FreePats-Direct": ("FreePats FSBS Direct Electric Guitar DI", "https://github.com/freepats/electric-guitar-FSBS-direct", ("LICENSE.txt", "LICENSE")),
    "FreePats-Spanish-Classical": ("FreePats Spanish Classical Guitar", "https://github.com/freepats/spanish-classical-guitar", ("LICENSE.txt", "LICENSE")),
    "Karoryfer-Black-And-Green": ("Karoryfer Black And Green Guitars", "https://github.com/sfzinstruments/karoryfer.black-and-green-guitars", "LICENSE"),
    "Karoryfer-Emilyguitar": ("Karoryfer Emilyguitar", "https://github.com/sfzinstruments/karoryfer.emilyguitar", "LICENSE"),
    "Karoryfer-Shinyguitar": ("Karoryfer Shinyguitar", "https://github.com/sfzinstruments/karoryfer.shinyguitar", "LICENSE"),
}


def digest_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def find_license(source_root: Path, candidates: str | tuple[str, ...]) -> Path | None:
    names = (candidates,) if isinstance(candidates, str) else candidates
    for name in names:
        candidate = source_root / name
        if candidate.is_file():
            return candidate
    return None


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path("Samples"))
    parser.add_argument("--output", type=Path, default=None)
    args = parser.parse_args()
    root = args.root.resolve()
    output = (args.output or (root / "SOUND_HUNTER_REGISTRY.json")).resolve()
    if not root.is_dir():
        raise SystemExit(f"SoundBank root missing: {root}")

    assets = []
    sources = []
    errors = []
    for folder, (label, url, license_names) in SOURCES.items():
        source_root = root / folder
        if not source_root.is_dir():
            errors.append(f"source folder missing: {folder}")
            continue
        license_path = find_license(source_root, license_names)
        if not license_path:
            errors.append(f"license evidence missing: {folder}")
            continue
        license_text = license_path.read_text(encoding="utf-8-sig", errors="replace")
        cc0_match = re.search(r"CC0\\s*(?:1\\.0|v1\\.0)|Creative Commons Zero", license_text, re.IGNORECASE)
        license_status = "CC0-1.0 text detected" if cc0_match else "REVIEW_REQUIRED"
        source_assets = []
        for path in sorted(source_root.rglob("*")):
            if not path.is_file() or path.suffix.lower() not in AUDIO_EXTENSIONS:
                continue
            relative = path.relative_to(root).as_posix()
            file_hash = digest_file(path)
            record = {
                "id": f"{folder.lower()}-{file_hash[:16]}",
                "path": relative,
                "sourceId": folder,
                "sourceName": label,
                "sourceUrl": url,
                "licenseEvidence": license_path.relative_to(root).as_posix(),
                "licenseStatus": license_status,
                "format": path.suffix.lower().lstrip("."),
                "sizeBytes": path.stat().st_size,
                "sha256": file_hash,
                "ingestionStatus": "verified_on_disk",
                "playbackStatus": "not_yet_verified",
            }
            assets.append(record)
            source_assets.append(record["id"])
        sources.append({
            "id": folder,
            "name": label,
            "url": url,
            "licenseEvidence": license_path.relative_to(root).as_posix(),
            "licenseStatus": license_status,
            "assetCount": len(source_assets),
            "assetIds": source_assets,
        })

    if errors:
        raise SystemExit("Registry generation failed:\n" + "\n".join(f"- {e}" for e in errors))
    if not assets:
        raise SystemExit("No supported audio files found; refusing to generate an empty inventory.")

    payload = {
        "schemaVersion": "1.1.0",
        "policy": "PPGU-REV-10.1",
        "generatedAtUtc": datetime.now(timezone.utc).isoformat(),
        "inventoryMode": "generated_from_files_physically_present_under_Samples",
        "assetCount": len(assets),
        "sourceCount": len(sources),
        "sources": sources,
        "assets": assets,
        "notes": [
            "Only files physically present in the staged SoundBank are listed.",
            "verified_on_disk means the file exists and its SHA-256 was computed.",
            "not_yet_verified playbackStatus does not claim ANGELA engine integration or audition.",
            "REVIEW_REQUIRED licenseStatus must be cleared before redistribution.",
        ],
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"Sound Hunter registry: {len(assets)} audio assets across {len(sources)} sources")
    print(f"Output: {output}")
    for source in sources:
        print(f"- {source['name']}: {source['assetCount']} files [{source['licenseStatus']}]")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
