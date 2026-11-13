#!/usr/bin/env python3
"""G5: enforce the natural-recordings-only factory SoundBank policy."""
from __future__ import annotations

import hashlib
import json
import re
import sys
from pathlib import Path, PurePosixPath


def fail(message: str) -> int:
    print(f"FAIL: {message}", file=sys.stderr)
    return 1


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def resolve_manifest_path(root: Path, relative: object) -> Path | None:
    if not isinstance(relative, str) or not relative:
        return None
    posix = PurePosixPath(relative)
    if posix.is_absolute() or any(part in ("", ".", "..") for part in posix.parts):
        return None
    if "\\" in relative or ":" in relative:
        return None
    try:
        candidate = (root / Path(*posix.parts)).resolve(strict=True)
        candidate.relative_to(root.resolve(strict=True))
    except (OSError, ValueError, RuntimeError):
        return None
    return candidate if candidate.is_file() else None


def main(argv: list[str]) -> int:
    if len(argv) != 2:
        print("usage: validate_soundbank.py <SOUNDBANK-directory>", file=sys.stderr)
        return 2

    root = Path(argv[1]).resolve()
    if not root.is_dir():
        return fail(f"SoundBank directory does not exist: {root}")

    policy_path = root / "policy.json"
    try:
        policy = json.loads(policy_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return fail(f"cannot read policy.json: {exc}")

    if not isinstance(policy, dict) or policy.get("sampleSourcePolicy") != "NATURAL_SAMPLES_ONLY":
        return fail("sampleSourcePolicy must be NATURAL_SAMPLES_ONLY")
    if policy.get("synthesizedSamplesAllowed") is not False:
        return fail("synthesizedSamplesAllowed must be false")

    wav_root = root / "WAV"
    wav_count = sum(1 for item in wav_root.rglob("*")
                    if item.is_file() and item.suffix.lower() == ".wav") if wav_root.is_dir() else 0

    factory_sfz = root / "factory.sfz"
    if factory_sfz.is_file():
        try:
            sfz_text = factory_sfz.read_text(encoding="utf-8", errors="replace").lower()
        except OSError as exc:
            return fail(f"cannot read factory.sfz: {exc}")
        if "<sample" in sfz_text:
            return fail("factory.sfz must not embed synthesized sample payloads")

    manifest_path = root / "manifest.json"
    if manifest_path.is_file():
        try:
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError) as exc:
            return fail(f"cannot read manifest.json: {exc}")
        files = manifest.get("files") if isinstance(manifest, dict) else None
        if not isinstance(files, list):
            return fail("manifest.json 'files' must be an array")
        seen: set[str] = set()
        for entry in files:
            if not isinstance(entry, dict):
                return fail("manifest entry must be an object")
            relative = entry.get("path")
            candidate = resolve_manifest_path(root, relative)
            if candidate is None:
                return fail(f"manifest path is missing or unsafe: {relative!r}")
            canonical = str(PurePosixPath(relative)).casefold()
            if canonical in seen:
                return fail(f"duplicate manifest path: {relative}")
            seen.add(canonical)
            expected = entry.get("sha256")
            if not isinstance(expected, str) or re.fullmatch(r"[0-9a-fA-F]{64}", expected) is None:
                return fail(f"manifest SHA-256 is malformed: {relative}")
            if sha256_file(candidate).lower() != expected.lower():
                return fail(f"SHA-256 mismatch: {relative}")

    print(f"PASS: SoundBank policy NATURAL_SAMPLES_ONLY ({wav_count} WAV file(s) under WAV/)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
