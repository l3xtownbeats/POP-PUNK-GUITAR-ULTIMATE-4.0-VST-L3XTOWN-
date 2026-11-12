#!/usr/bin/env python3
"""Lightweight offline SFZ/WAV instrument packager for PPGU.

No third-party dependencies. It never modifies source files and performs no work
inside the VST3/audio callback. Hashing is chunked to keep memory bounded.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import shutil
import sys
import wave
from pathlib import Path

CHUNK = 1024 * 1024
ARTICULATIONS = {
    "palm_mute": ("palm", "mute", "pm"),
    "harmonic": ("harmonic", "flageolet"),
    "slide": ("slide", "gliss"),
    "bend": ("bend",),
    "tremolo": ("tremolo",),
    "finger": ("finger", "fingerstyle"),
    "pick": ("pick", "picked"),
    "strum": ("strum",),
    "release": ("release",),
}

def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        while True:
            block = stream.read(CHUNK)
            if not block:
                break
            digest.update(block)
    return digest.hexdigest()

def parse_sfz(path: Path):
    """Read common SFZ section inheritance without decoding sample audio."""
    regions = []
    inherited = {"global": {}, "master": {}, "group": {}}
    scope = "control"
    current = None
    token_pattern = re.compile(r'<[^>]+>|([A-Za-z0-9_]+)\s*=\s*(?:"([^"]*)"|([^\s]+))')
    for line_no, raw in enumerate(path.read_text(encoding="utf-8-sig", errors="replace").splitlines(), 1):
        line = raw.split("//", 1)[0].strip()
        if not line:
            continue
        if line.startswith("#include") or "default_path=" in line:
            raise ValueError(f"line {line_no}: #include/default_path needs manual resolution before packaging")
        for match in token_pattern.finditer(line):
            token = match.group(0)
            if token.startswith("<"):
                tag = token[1:-1].strip().lower()
                if tag == "region":
                    if current is not None and current.get("sample"): regions.append(current)
                    current = {}
                    current.update(inherited["global"]); current.update(inherited["master"]); current.update(inherited["group"])
                    current["line"] = line_no; scope = "region"
                elif tag in ("global", "master", "group"):
                    if current is not None and current.get("sample"): regions.append(current)
                    current = None; scope = tag
                    if tag == "master": inherited["master"] = {}; inherited["group"] = {}
                    elif tag == "group": inherited["group"] = {}
                else:
                    if current is not None and current.get("sample"): regions.append(current)
                    current = None; scope = "control"
                continue
            key = match.group(1).lower()
            value = match.group(2) if match.group(2) is not None else match.group(3)
            if scope == "region" and current is not None: current[key] = value
            elif scope in inherited: inherited[scope][key] = value
    if current is not None and current.get("sample"): regions.append(current)
    return regions
def safe_sample_path(root: Path, sample: str) -> Path:
    candidate = (root / sample.replace("\\", os.sep)).resolve()
    base = root.resolve()
    if candidate != base and base not in candidate.parents:
        raise ValueError(f"sample path escapes instrument folder: {sample}")
    return candidate

def inspect_wav(path: Path):
    with wave.open(str(path), "rb") as wav:
        channels = wav.getnchannels()
        rate = wav.getframerate()
        frames = wav.getnframes()
        width = wav.getsampwidth()
        if channels not in (1, 2):
            raise ValueError(f"unsupported channel count {channels}; sampler supports mono/stereo")
        if not 8000 <= rate <= 384000:
            raise ValueError(f"unsupported sample rate {rate}")
        if frames <= 0:
            raise ValueError("empty WAV")
        return {
            "channels": channels, "sample_rate": rate, "frames": frames,
            "duration_seconds": round(frames / rate, 6),
            "bit_depth": width * 8, "compression": wav.getcomptype(),
        }

def capability_summary(regions):
    keys, velocity_ranges, rr_lengths = [], set(), set()
    articulations = set()
    for region in regions:
        lo = int(region.get("lokey", region.get("key", 0)))
        hi = int(region.get("hikey", region.get("key", 127)))
        keys.extend((max(0, min(127, lo)), max(0, min(127, hi))))
        vlo = int(region.get("lovel", 0)); vhi = int(region.get("hivel", 127))
        velocity_ranges.add((vlo, vhi))
        rr_lengths.add(int(region.get("seq_length", 1)))
        label = (region.get("sample", "") + " " + region.get("label", "")).lower()
        for name, tokens in ARTICULATIONS.items():
            if any(token in label for token in tokens):
                articulations.add(name)
    return {
        "note_range": [min(keys), max(keys)] if keys else None,
        "velocity_layers": len(velocity_ranges),
        "velocity_ranges": [list(pair) for pair in sorted(velocity_ranges)],
        "round_robin_max_length": max(rr_lengths, default=1),
        "round_robin_present": any(length > 1 for length in rr_lengths),
        "articulation_tags_detected": sorted(articulations),
        "region_count": len(regions),
    }

def build_manifest(sfz: Path):
    sfz = sfz.resolve()
    if not sfz.is_file() or sfz.suffix.lower() != ".sfz":
        raise ValueError("input must be an existing .sfz file")
    root = sfz.parent
    regions = parse_sfz(sfz)
    if not regions:
        raise ValueError("no explicit <region> sample=... entries found; resolve includes/inheritance in the source SFZ first")
    assets, missing, seen = [], [], set()
    for region in regions:
        sample = region["sample"]
        path = safe_sample_path(root, sample)
        if not path.is_file():
            missing.append({"sample": sample, "line": region["line"]})
            continue
        if path.suffix.lower() != ".wav":
            missing.append({"sample": sample, "line": region["line"], "reason": "first release supports WAV assets only"})
            continue
        metadata = inspect_wav(path)
        digest = sha256_file(path)
        duplicate_of = next((item["path"] for item in assets if item["sha256"] == digest), None)
        if path.as_posix() not in seen:
            assets.append({"path": path.relative_to(root).as_posix(), "bytes": path.stat().st_size,
                           "sha256": digest, **metadata, "duplicate_of": duplicate_of})
            seen.add(path.as_posix())
    return {
        "schema": "ppgu.instrument-package.v1",
        "instrument": sfz.stem,
        "source_sfz": sfz.name,
        "source_root": str(root),
        "assets": assets,
        "missing_or_unsupported": missing,
        "capabilities": capability_summary(regions),
        "available_modes": {
            "rhythm": True, "melody": True,
            "shared_sample_pool": True,
            "note": "Both modes reference the same SFZ package; this manifest does not create duplicated audio assets."
        },
        "runtime_contract": {
            "first_release_formats": ["SFZ", "WAV"],
            "audio_assets_loaded_once": True,
            "offline_hashing_only": True,
            "source_files_modified": False,
        },
        "ready": bool(assets) and not missing,
    }

def package(sfz: Path, output: Path):
    manifest = build_manifest(sfz)
    if not manifest["ready"]:
        raise ValueError(f"package is incomplete: {len(manifest['missing_or_unsupported'])} missing/unsupported sample reference(s)")
    source_root = sfz.resolve().parent
    staging = output.with_name(output.name + ".staging")
    if staging.exists():
        shutil.rmtree(staging)
    staging.mkdir(parents=True)
    try:
        # Copy the SFZ and only referenced, verified WAVs. Preserve relative paths.
        shutil.copy2(sfz.resolve(), staging / sfz.name)
        for asset in manifest["assets"]:
            source = source_root / asset["path"]
            target = staging / asset["path"]
            target.parent.mkdir(parents=True, exist_ok=True)
            if not target.exists():
                shutil.copy2(source, target)
        manifest["package_file"] = sfz.name
        manifest["source_root"] = "."
        (staging / "instrument-package.json").write_text(
            json.dumps(manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        if output.exists():
            shutil.rmtree(output)
        staging.replace(output)
    except Exception:
        if staging.exists():
            shutil.rmtree(staging)
        raise
    return manifest

def main(argv=None):
    parser = argparse.ArgumentParser(description="PPGU Sound Forge — inspect and package SFZ/WAV guitar instruments")
    sub = parser.add_subparsers(dest="command", required=True)
    scan = sub.add_parser("scan", help="scan SFZ/WAV and write a quality manifest")
    scan.add_argument("sfz", type=Path)
    scan.add_argument("--json", type=Path, help="write manifest to this path")
    build = sub.add_parser("package", help="create a portable package from a verified SFZ/WAV instrument")
    build.add_argument("sfz", type=Path)
    build.add_argument("output", type=Path)
    args = parser.parse_args(argv)
    try:
        manifest = build_manifest(args.sfz) if args.command == "scan" else package(args.sfz, args.output)
        rendered = json.dumps(manifest, indent=2, ensure_ascii=False)
        if args.command == "scan" and args.json:
            args.json.write_text(rendered + "\n", encoding="utf-8")
        print(rendered)
        return 0 if manifest["ready"] else 2
    except Exception as exc:
        print(f"Sound Forge error: {exc}", file=sys.stderr)
        return 1

if __name__ == "__main__":
    raise SystemExit(main())
