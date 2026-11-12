#!/usr/bin/env python3
"""Audit the redistributable guitar SoundBank after download or cache restore.

Checks SHA256 coverage and contents, required source folders/licenses/SFZ maps,
guitar-audio file count, and the preferred/hard payload size limits.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import sys

AUDIO_EXTENSIONS = {".wav", ".flac"}
SOURCES = {
    "Meta-meta-steel-guitar": ("Meta Open Sound Steel Guitar", "LICENSE"),
    "Meta-meta-nylon-guitar": ("Meta Open Sound Nylon Guitar", "LICENSE"),
    "FreePats-Clean": ("FreePats FSBS Clean Electric Guitar", ("LICENSE.txt", "LICENSE")),
    "FreePats-Distortion-1": ("FreePats FSBS Distortion 1", ("LICENSE.txt", "LICENSE")),
    "FreePats-Distortion-2": ("FreePats FSBS Distortion 2", ("LICENSE.txt", "LICENSE")),
    "FreePats-Jazz-Electric": ("FreePats Jazz Electric Guitar", ("LICENSE.txt", "LICENSE")),
    "FreePats-Direct": ("FreePats FSBS Direct Electric Guitar DI", ("LICENSE.txt", "LICENSE")),
    "FreePats-Spanish-Classical": ("FreePats Spanish Classical Guitar", ("LICENSE.txt", "LICENSE")),
    "Karoryfer-Black-And-Green": ("Karoryfer Black And Green Guitars", "LICENSE"),
    "Karoryfer-Emilyguitar": ("Karoryfer Emilyguitar", "LICENSE"),
    "Karoryfer-Shinyguitar": ("Karoryfer Shinyguitar", "LICENSE"),
}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path("Samples"))
    parser.add_argument("--preferred-gb", type=float, default=25.0)
    parser.add_argument("--hard-limit-gb", type=float, default=30.0)
    args = parser.parse_args()

    root = args.root.resolve()
    if not root.is_dir():
        print(f"SoundBank root missing: {root}", file=sys.stderr)
        return 2

    errors: list[str] = []
    audio_files = sorted(
        p for p in root.rglob("*")
        if p.is_file() and p.suffix.lower() in AUDIO_EXTENSIONS
    )
    if len(audio_files) < 100:
        errors.append(f"expected at least 100 WAV/FLAC files, found {len(audio_files)}")

    for folder, (label, license_names) in SOURCES.items():
        source_root = root / folder
        if not source_root.is_dir():
            errors.append(f"required source folder missing: {folder} ({label})")
            continue
        license_candidates = (license_names,) if isinstance(license_names, str) else license_names
        if not any((source_root / candidate).is_file() for candidate in license_candidates):
            errors.append(f"license evidence missing in source root: {folder}")
        sfz = list(source_root.rglob("*.sfz"))
        if not sfz:
            errors.append(f"SFZ mapping missing: {folder}")
        if not any(p.is_file() and p.suffix.lower() in AUDIO_EXTENSIONS
                   for p in source_root.rglob("*")):
            errors.append(f"recorded audio missing: {folder}")

    manifest = root / "SHA256SUMS-POP-PUNK.txt"
    if not manifest.is_file():
        errors.append("SHA256SUMS-POP-PUNK.txt is missing")
    else:
        expected: dict[str, str] = {}
        for number, line in enumerate(manifest.read_text(encoding="utf-8-sig").splitlines(), 1):
            if not line.strip():
                continue
            parts = line.strip().split(None, 1)
            if len(parts) != 2 or len(parts[0]) != 64:
                errors.append(f"malformed checksum line {number}")
                continue
            digest, relative = parts[0].lower(), parts[1].lstrip("*").replace("\\", "/")
            if relative in expected:
                errors.append(f"duplicate checksum path: {relative}")
            expected[relative] = digest

        actual_paths = {p.relative_to(root).as_posix(): p for p in audio_files}
        if set(expected) != set(actual_paths):
            missing = sorted(set(actual_paths) - set(expected))
            stale = sorted(set(expected) - set(actual_paths))
            if missing:
                errors.append(f"{len(missing)} audio file(s) missing from checksum manifest; first: {missing[0]}")
            if stale:
                errors.append(f"{len(stale)} stale checksum entr(y/ies); first: {stale[0]}")
        for relative in sorted(set(expected) & set(actual_paths)):
            if sha256(actual_paths[relative]) != expected[relative]:
                errors.append(f"SHA-256 mismatch: {relative}")

    total_bytes = sum(p.stat().st_size for p in audio_files)
    gib = 1024 ** 3
    size_gb = total_bytes / gib
    if size_gb > args.hard_limit_gb:
        errors.append(f"audio payload {size_gb:.2f} GiB exceeds hard limit {args.hard_limit_gb:.2f} GiB")

    lines = [
        "POP PUNK GUITAR ULTIMATE — SOUNDBANK AUDIT",
        f"Audio files: {len(audio_files)}",
        f"Audio payload: {size_gb:.3f} GiB",
        f"Preferred target: <= {args.preferred_gb:.2f} GiB",
        f"Hard ceiling: {args.hard_limit_gb:.2f} GiB",
        f"Checksum manifest entries: {len(expected) if manifest.is_file() else 0}",
        "Source libraries:",
    ]
    for folder, (label, _) in SOURCES.items():
        source_root = root / folder
        count = sum(1 for p in source_root.rglob("*")
                    if p.is_file() and p.suffix.lower() in AUDIO_EXTENSIONS) if source_root.is_dir() else 0
        lines.append(f"- {label}: {count} audio files")
    lines.append("Result: FAIL" if errors else "Result: PASS")
    (root / "SOUNDBANK_AUDIT.txt").write_text("\n".join(lines) + "\n", encoding="utf-8")

    if errors:
        print("SoundBank audit FAILED:")
        for error in errors:
            print(f" - {error}")
        return 1
    print("\n".join(lines))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
