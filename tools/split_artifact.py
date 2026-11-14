#!/usr/bin/env python3
"""Split large release archives into ordered, independently verifiable parts.

The content itself is never capped. Part size is only a transport setting.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import sys

DEFAULT_PART_SIZE = 1800 * 1024 * 1024


def digest_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def split_archive(source: Path, output_dir: Path, prefix: str, part_size: int) -> Path:
    if not source.is_file():
        raise FileNotFoundError(f"Input archive not found: {source}")
    if part_size <= 0:
        raise ValueError("Part size must be positive.")
    output_dir.mkdir(parents=True, exist_ok=True)

    parts: list[dict[str, object]] = []
    archive_hash = hashlib.sha256()
    total = 0
    index = 1
    with source.open("rb") as src:
        while True:
            chunk = src.read(part_size)
            if not chunk:
                break
            name = f"{prefix}.part-{index:04d}"
            destination = output_dir / name
            with destination.open("wb") as dst:
                dst.write(chunk)
            archive_hash.update(chunk)
            total += len(chunk)
            parts.append({
                "index": index,
                "name": name,
                "size_bytes": len(chunk),
                "sha256": hashlib.sha256(chunk).hexdigest(),
            })
            index += 1

    if not parts:
        raise ValueError("Input archive is empty; refusing to create an empty parts manifest.")

    manifest = {
        "schema": "ppgu.split-artifact.v1",
        "original_filename": source.name,
        "total_size_bytes": total,
        "archive_sha256": archive_hash.hexdigest(),
        "part_size_bytes": part_size,
        "part_count": len(parts),
        "parts": parts,
    }
    manifest_path = output_dir / f"{prefix}_PARTS_MANIFEST.json"
    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

    sums_path = output_dir / f"{prefix}_PARTS_SHA256SUMS.txt"
    sums_path.write_text(
        "".join(f"{part['sha256']}  {part['name']}\n" for part in parts),
        encoding="utf-8",
    )
    return manifest_path


def verify_manifest(manifest_path: Path, parts_dir: Path) -> None:
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    if manifest.get("schema") != "ppgu.split-artifact.v1":
        raise ValueError("Unsupported split manifest schema.")
    parts = manifest.get("parts")
    if not isinstance(parts, list) or len(parts) != manifest.get("part_count"):
        raise ValueError("Part count does not match manifest.")
    archive_hash = hashlib.sha256()
    total = 0
    expected_index = 1
    for part in parts:
        if part.get("index") != expected_index:
            raise ValueError("Part order/index is not contiguous.")
        path = parts_dir / str(part["name"])
        if not path.is_file():
            raise FileNotFoundError(f"Missing part: {path.name}")
        size = path.stat().st_size
        if size != part.get("size_bytes"):
            raise ValueError(f"Part size mismatch: {path.name}")
        part_hash = hashlib.sha256()
        with path.open("rb") as stream:
            for chunk in iter(lambda: stream.read(1024 * 1024), b""):
                part_hash.update(chunk)
                archive_hash.update(chunk)
        if part_hash.hexdigest() != part.get("sha256"):
            raise ValueError(f"SHA-256 mismatch: {path.name}")
        total += size
        expected_index += 1
    if total != manifest.get("total_size_bytes"):
        raise ValueError("Reconstructed archive size does not match manifest.")
    if archive_hash.hexdigest() != manifest.get("archive_sha256"):
        raise ValueError("Reconstructed archive SHA-256 does not match manifest.")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    split_parser = sub.add_parser("split", help="split an archive and write manifests")
    split_parser.add_argument("--input", type=Path, required=True)
    split_parser.add_argument("--output-dir", type=Path, required=True)
    split_parser.add_argument("--prefix", required=True)
    split_parser.add_argument("--part-size-mb", type=int, default=1800)
    verify_parser = sub.add_parser("verify", help="verify ordered parts and whole-archive hash")
    verify_parser.add_argument("--manifest", type=Path, required=True)
    verify_parser.add_argument("--parts-dir", type=Path, default=None)
    args = parser.parse_args()

    try:
        if args.command == "split":
            manifest = split_archive(
                args.input, args.output_dir, args.prefix, args.part_size_mb * 1024 * 1024
            )
            verify_manifest(manifest, args.output_dir)
            print(f"Created and verified split manifest: {manifest}")
        else:
            verify_manifest(args.manifest, args.parts_dir or args.manifest.parent)
            print(f"Verified split archive: {args.manifest}")
    except (OSError, ValueError, KeyError, json.JSONDecodeError) as exc:
        print(f"Artifact split/verification failed: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
