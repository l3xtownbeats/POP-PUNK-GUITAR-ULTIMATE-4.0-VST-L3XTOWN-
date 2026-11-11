#!/usr/bin/env python3
"""Audit factory preset metadata and, when supplied, installed sample-set folders.

This deliberately distinguishes catalog metadata from a playable/validated preset.
"""
import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REGISTRY = {
    "meta-nylon-guitar": ("Meta-meta-nylon-guitar", "LICENSE"),
    "meta-steel-guitar": ("Meta-meta-steel-guitar", "LICENSE"),
    "freepats-electric-clean": ("FreePats-Clean", "LICENSE.txt"),
    "freepats-electric-dist1": ("FreePats-Distortion-1", "LICENSE.txt"),
    "freepats-electric-dist2": ("FreePats-Distortion-2", "LICENSE.txt"),
}

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--catalog", type=Path, default=ROOT / "presets/factory_bank_300.json")
    ap.add_argument("--samples-root", type=Path, default=None,
                    help="Optional extracted Samples directory; enables on-disk bank/license checks.")
    args = ap.parse_args()
    data = json.loads(args.catalog.read_text(encoding="utf-8"))
    melody = data.get("melodyPresets", [])
    pattern = data.get("patternPresets", [])
    all_items = melody + pattern
    errors, warnings = [], []

    if len(melody) != 165: errors.append(f"Expected 165 Melody presets, found {len(melody)}")
    if len(pattern) != 165: errors.append(f"Expected 165 Pattern presets, found {len(pattern)}")
    ids = [p.get("id") for p in all_items]
    if None in ids or len(ids) != len(set(ids)): errors.append("Preset IDs are missing or duplicated")

    artists = {a.get("name") for a in data.get("artists", [])}
    for mode, items in (("MELODY", melody), ("PATTERN", pattern)):
        for p in items:
            pid = p.get("id", "<missing-id>")
            if p.get("mode") != mode: errors.append(f"{pid}: incorrect mode")
            if p.get("referenceArtist") not in artists: errors.append(f"{pid}: unknown artist")
            if not all(p.get(k) for k in ("browserGroup", "browserArtist", "browserSound", "browserMode")):
                errors.append(f"{pid}: missing unified-browser metadata")
            if not p.get("sampleProfile") in ("electric", "acoustic"):
                errors.append(f"{pid}: missing/invalid sampleProfile")
            if p.get("sampleSetId") not in REGISTRY:
                errors.append(f"{pid}: unknown sampleSetId {p.get('sampleSetId')!r}")
            if not p.get("articulationProfile"):
                errors.append(f"{pid}: missing articulationProfile")
            params = p.get("parameters") or {}
            for key in ("drive", "tone", "reverb"):
                if not isinstance(params.get(key), (int, float)) or isinstance(params.get(key), bool):
                    errors.append(f"{pid}: invalid parameters.{key}")
            if mode == "MELODY" and not p.get("noteBehavior"):
                errors.append(f"{pid}: missing note behavior")
            if mode == "PATTERN":
                if not p.get("groove") or not isinstance(p.get("performance"), dict):
                    errors.append(f"{pid}: missing groove/performance metadata")
                if not p.get("performanceAssetRequirement"):
                    errors.append(f"{pid}: missing performance asset requirement")
            if not str(p.get("runtimeBindingStatus", "")).startswith("metadata-mapped-"):
                warnings.append(f"{pid}: no explicit runtime binding status")

    sample_report = {}
    if args.samples_root:
        root = args.samples_root.resolve()
        for sample_id, (folder, license_name) in REGISTRY.items():
            bank = root / folder
            if not bank.is_dir():
                errors.append(f"{sample_id}: missing sample folder {bank}")
                continue
            if not (bank / license_name).is_file():
                errors.append(f"{sample_id}: missing license file {license_name}")
            sfz = list(bank.rglob("*.sfz"))
            wav = list(bank.rglob("*.wav"))
            if not sfz: errors.append(f"{sample_id}: no SFZ map found")
            if not wav: errors.append(f"{sample_id}: no WAV samples found")
            sample_report[sample_id] = {"sfz": len(sfz), "wav": len(wav),
                                        "license": (bank / license_name).is_file()}

    print(f"Factory catalog: {len(melody)} Melody + {len(pattern)} Pattern; "
          f"{len(all_items)} total; {len(artists)} artists")
    print(f"Sample-set targets: {len(set(p.get('sampleSetId') for p in all_items))} distinct")
    if sample_report: print("On-disk sample audit: " + json.dumps(sample_report, sort_keys=True))
    if warnings:
        print("WARNINGS:")
        for item in warnings: print(" - " + item)
    if errors:
        print("AUDIT FAILED:")
        for item in errors: print(" - " + item)
        return 1
    print("Catalog/sample-folder structural audit: PASS")
    if not args.samples_root:
        print("NOTE: no sample files inspected; pass --samples-root for the on-disk audit.")
    print("NOTE: this audit does not prove preset-to-SFZ auto-loading, audio quality, "
          "Pattern realism, or FL Studio playback.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
