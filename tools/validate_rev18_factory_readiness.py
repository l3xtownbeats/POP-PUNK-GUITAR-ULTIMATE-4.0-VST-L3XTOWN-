#!/usr/bin/env python3
"""Audit REV.18 READY preset evidence without promoting legacy definitions.

A preset counts as READY only when runtime, redistributable source mapping,
performance validation and human-reviewed audio validation are all recorded.
Default mode is an audit report (exit 0 for an incomplete roadmap); --strict
is intended only for release gates and fails unless both 500-entry targets pass.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

TARGET_PER_MODE = 500
SHA256_RE = re.compile(r"^[0-9a-fA-F]{64}$")


def ready_evidence_errors(preset: object) -> list[str]:
    if not isinstance(preset, dict):
        return ["preset record is not an object"]
    errors: list[str] = []
    readiness = preset.get("readiness")
    if not isinstance(readiness, dict):
        return ["missing readiness evidence object"]

    runtime = readiness.get("runtime")
    if not isinstance(runtime, dict) or runtime.get("implemented") is not True or runtime.get("loadedByPlugin") is not True:
        errors.append("runtime implementation/load evidence is incomplete")

    mappings = readiness.get("sourceMappings")
    if not isinstance(mappings, list) or not mappings:
        errors.append("no source mappings")
    else:
        for index, mapping in enumerate(mappings):
            if not isinstance(mapping, dict):
                errors.append(f"sourceMappings[{index}] is not an object")
                continue
            for key in ("assetId", "relativePath", "licenseEvidencePath"):
                if not isinstance(mapping.get(key), str) or not mapping[key].strip():
                    errors.append(f"sourceMappings[{index}].{key} is missing")
            if not isinstance(mapping.get("sha256"), str) or not SHA256_RE.fullmatch(mapping.get("sha256", "")):
                errors.append(f"sourceMappings[{index}].sha256 is invalid")
            if mapping.get("commercialRedistributionApproved") is not True:
                errors.append(f"sourceMappings[{index}] lacks commercial redistribution approval")
            if mapping.get("mappingVerified") is not True:
                errors.append(f"sourceMappings[{index}] mapping is not verified")

    performance = readiness.get("performanceValidation")
    if not isinstance(performance, dict) or performance.get("status") != "PASS":
        errors.append("performance validation is not PASS")

    audio = readiness.get("audioValidation")
    if not isinstance(audio, dict) or audio.get("status") != "PASS" or audio.get("humanListeningApproved") is not True:
        errors.append("audio validation/human listening approval is incomplete")
    return errors


def audit(data: object) -> tuple[dict[str, int], list[str]]:
    if not isinstance(data, dict):
        return {"MELODY": 0, "PATTERN": 0}, ["catalog root must be an object"]
    counts = {"MELODY": 0, "PATTERN": 0}
    errors: list[str] = []
    for mode, key in (("MELODY", "melodyPresets"), ("PATTERN", "patternPresets")):
        entries = data.get(key)
        if not isinstance(entries, list):
            errors.append(f"{key} must be an array")
            continue
        seen: set[str] = set()
        for index, preset in enumerate(entries):
            if not isinstance(preset, dict):
                errors.append(f"{key}[{index}] is not an object")
                continue
            preset_id = preset.get("id")
            if not isinstance(preset_id, str) or not preset_id.strip():
                errors.append(f"{key}[{index}] has no stable id")
                continue
            if preset_id in seen:
                errors.append(f"{mode} has duplicate id {preset_id}")
                continue
            seen.add(preset_id)
            if preset.get("mode") != mode:
                errors.append(f"{preset_id}: mode does not match {mode}")
            if preset.get("status") == "READY":
                evidence_errors = ready_evidence_errors(preset)
                if evidence_errors:
                    errors.extend(f"{preset_id}: {message}" for message in evidence_errors)
                else:
                    counts[mode] += 1
    return counts, errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("catalog", nargs="?", type=Path, default=Path("presets/factory_bank_300.json"))
    parser.add_argument("--strict", action="store_true", help="fail unless 500 verified READY presets exist in each mode")
    args = parser.parse_args()
    try:
        data = json.loads(args.catalog.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        print(f"REV.18 readiness audit: cannot read catalog: {exc}", file=sys.stderr)
        return 2
    counts, errors = audit(data)
    print(f"REV.18 READY audit: MELODY {counts['MELODY']}/{TARGET_PER_MODE}; PATTERN {counts['PATTERN']}/{TARGET_PER_MODE}")
    if errors:
        print("Invalid READY claims / catalog errors:")
        for error in errors:
            print(f" - {error}")
    if args.strict and (errors or any(counts[mode] < TARGET_PER_MODE for mode in counts)):
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
