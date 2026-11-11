#!/usr/bin/env python3
"""Build a deterministic, license-aware index for the PPGU guitar sound bank.

This tool does not copy or modify source audio. It scans WAV/FLAC/AIFF/OGG
assets and SFZ maps, records hashes and source provenance, and marks an asset
factory-eligible only when a nearby license file explicitly identifies CC0.
Unknown/custom licenses remain visible as review-only user-bank content.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path

AUDIO_EXTS = {".wav", ".flac", ".aif", ".aiff", ".ogg"}
CATEGORIES = (
    "00_DI_Raw", "01_Acoustic", "02_Clean_Pickup",
    "03_Distortion_PowerChords", "04_PalmMute",
    "05_HighGain_Direct", "06_Distortion_Sustains",
    "_SFZ", "_Unclassified",
)
CATEGORY_RULES = (
    ("00_DI_Raw", ("direct di", "di raw", "raw di", "direct-input", "unprocessed")),
    ("01_Acoustic", ("nylon", "steel", "acoustic", "ovation", "classical")),
    ("04_PalmMute", ("palm mute", "palmmute", "palm_muted", "muted")),
    ("05_HighGain_Direct", ("high gain", "high-gain", "metal", "heavy")),
    ("03_Distortion_PowerChords", ("dist", "crunch", "power chord", "powerchord", "overdrive")),
    ("02_Clean_Pickup", ("clean", "neck pickup", "bridge pickup", "single coil")),
    ("06_Distortion_Sustains", ("sustain", "sustained", "lead")),
)

def digest(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()

def license_for(path: Path, root: Path) -> dict:
    candidates = []
    for parent in (path.parent, *path.parents):
        if parent == root.parent:
            break
        for name in ("LICENSE", "LICENSE.txt", "COPYING", "COPYING.txt", "README", "README.txt", "README.md"):
            p = parent / name
            if p.is_file() and p not in candidates:
                candidates.append(p)
        if parent == root:
            break
    for p in candidates:
        try:
            text = p.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        low = text.lower()
        if "cc0 1.0" in low or "creative commons zero" in low or "public domain dedication" in low:
            return {"status": "APPROVED_CC0_CANDIDATE", "license": "CC0-1.0",
                    "licenseFile": p.relative_to(root).as_posix()}
    return {"status": "REVIEW_REQUIRED", "license": "UNKNOWN",
            "licenseFile": None}

def classify(path: Path, root: Path) -> str:
    rel = path.relative_to(root).as_posix().lower().replace("-", " ").replace("_", " ")
    for category, terms in CATEGORY_RULES:
        if any(term in rel for term in terms):
            return category
    return "_Unclassified"

def sfz_samples(sfz: Path) -> list[str]:
    try:
        text = sfz.read_text(encoding="utf-8", errors="ignore")
    except OSError:
        return []
    found = []
    for line in text.splitlines():
        line = line.split("//", 1)[0]
        for match in re.finditer(r"""(?i)\bsample\s*=\s*["']?([^"'\s]+)""", line):
            found.append(match.group(1).replace("\\", "/"))
    return found

def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", default="Samples", type=Path)
    ap.add_argument("--output", default="Samples/SoundBank/catalog.json", type=Path)
    ap.add_argument("--report", default="Samples/SoundBank/BUILD_REPORT.txt", type=Path)
    args = ap.parse_args()
    root = args.root.resolve()
    if not root.is_dir():
        ap.error(f"Sample root does not exist: {root}")

    assets, seen, duplicates = [], {}, []
    sfz_maps, missing_refs = [], []
    for sfz in sorted(root.rglob("*.sfz")):
        refs = sfz_samples(sfz)
        resolved = []
        for ref in refs:
            p = (sfz.parent / ref).resolve()
            try:
                p.relative_to(root)
            except ValueError:
                missing_refs.append({"sfz": sfz.relative_to(root).as_posix(),
                                     "sample": ref, "reason": "path escapes sample root"})
                continue
            if p.is_file():
                resolved.append(p)
            else:
                # Some official SFZ maps refer to OGG previews while WAV releases
                # carry production masters. Do not silently substitute extensions.
                missing_refs.append({"sfz": sfz.relative_to(root).as_posix(),
                                     "sample": ref, "reason": "referenced file missing"})
        sfz_maps.append({"path": sfz.relative_to(root).as_posix(),
                         "sampleReferenceCount": len(refs),
                         "resolvedReferenceCount": len(resolved),
                         "license": license_for(sfz, root)})

    for path in sorted(p for p in root.rglob("*") if p.is_file() and p.suffix.lower() in AUDIO_EXTS):
        rel = path.relative_to(root).as_posix()
        sha = digest(path)
        lic = license_for(path, root)
        category = classify(path, root)
        item = {
            "id": sha[:16], "path": rel, "format": path.suffix.lower().lstrip("."),
            "sizeBytes": path.stat().st_size, "sha256": sha,
            "category": category, **lic,
            "factoryEligible": lic["status"] == "APPROVED_CC0_CANDIDATE",
            "sourceLibrary": path.relative_to(root).parts[0] if len(path.relative_to(root).parts) > 1 else ".",
        }
        if sha in seen:
            duplicates.append({"path": rel, "duplicateOf": seen[sha], "sha256": sha})
        else:
            seen[sha] = rel
        assets.append(item)

    counts = defaultdict(int)
    for item in assets:
        counts[item["category"]] += 1
    approved = sum(1 for x in assets if x["factoryEligible"])
    payload = {
        "schemaVersion": 1, "product": "POP PUNK GUITAR ULTIMATE",
        "generatedAtUtc": datetime.now(timezone.utc).isoformat(),
        "policy": "Unknown licenses are review-only; factoryEligible is a candidate flag, not final audio/legal QA.",
        "root": str(args.root).replace("\\", "/"),
        "summary": {"audioFiles": len(assets), "cc0Candidates": approved,
                    "reviewRequired": len(assets) - approved,
                    "uniqueAudioFiles": len(seen), "duplicateFiles": len(duplicates),
                    "sfzMaps": len(sfz_maps), "missingSfzReferences": len(missing_refs),
                    "categoryCounts": dict(sorted(counts.items()))},
        "categories": list(CATEGORIES), "assets": assets,
        "sfzMaps": sfz_maps, "duplicates": duplicates,
        "missingSfzReferences": missing_refs,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    report = [
        "PPGU SOUND BANK INDEX REPORT",
        f"Audio files: {len(assets)}",
        f"CC0 candidates (not yet audio-QA approved): {approved}",
        f"Review required: {len(assets) - approved}",
        f"Unique audio files: {len(seen)}",
        f"Duplicates: {len(duplicates)}",
        f"SFZ maps: {len(sfz_maps)}",
        f"Missing SFZ references: {len(missing_refs)}",
        "",
        "Category counts:",
        *[f"  {k}: {v}" for k, v in sorted(counts.items())],
        "",
        "No audio files were copied, converted, or deleted.",
    ]
    args.report.write_text("\n".join(report) + "\n", encoding="utf-8")
    print("\n".join(report))
    return 0 if assets else 2

if __name__ == "__main__":
    raise SystemExit(main())
