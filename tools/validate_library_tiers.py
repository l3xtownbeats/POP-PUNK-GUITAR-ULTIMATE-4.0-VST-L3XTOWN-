#!/usr/bin/env python3
"""Validate the approved PPGU BASIC/EXPANDED library distribution contract."""
from __future__ import annotations
import json
import sys
from pathlib import Path

GB = 1_000_000_000

def validate(data: dict) -> list[str]:
    errors: list[str] = []
    if data.get("schemaVersion") != "1.0.0":
        errors.append("schemaVersion must be 1.0.0")
    units = data.get("storageUnits", {})
    if units.get("definition") != "DECIMAL" or units.get("bytesPerGB") != GB:
        errors.append("GB must use decimal SI units (1 GB = 1,000,000,000 bytes)")
    presets = data.get("factoryPresets", {})
    if presets.get("requiredPlayableCount") != 500:
        errors.append("BASIC must support exactly the approved 500-preset factory target")
    if presets.get("requiresBasicLibrary") is not True:
        errors.append("factory presets must require BASIC")
    if presets.get("requiresExpandedLibrary") is not False:
        errors.append("factory presets must not require EXPANDED")
    libraries = data.get("libraries", {})
    basic = libraries.get("basic", {})
    expanded = libraries.get("expanded", {})
    if basic.get("targetBytes") != 15 * GB or basic.get("targetGB") != 15:
        errors.append("BASIC LIBRARY target must be 15 GB")
    if basic.get("distribution") != "INCLUDED_IN_STANDARD_PRODUCT_PACKAGE":
        errors.append("BASIC 15 GB must be included in the standard product package")
    if basic.get("mandatory") is not True or basic.get("independentlyPlayable") is not True:
        errors.append("BASIC must be mandatory and independently playable")
    if basic.get("supportsFactoryPresetCount") != 500:
        errors.append("BASIC must support all 500 factory presets")
    if expanded.get("addonBytes") != 15 * GB or expanded.get("addonGB") != 15:
        errors.append("EXPANDED ADD-ON must add 15 GB")
    if expanded.get("initialCombinedBytes") != 30 * GB or expanded.get("initialCombinedGB") != 30:
        errors.append("initial combined EXPANDED library must total 30 GB (15 + 15)")
    if expanded.get("distribution") != "OPTIONAL_ADDITIVE_ADDON":
        errors.append("EXPANDED must be an optional additive add-on")
    if expanded.get("requiresBasic") is not True or expanded.get("replacesBasic") is not False:
        errors.append("EXPANDED must add to, never replace, BASIC")
    if expanded.get("expandable") is not True or expanded.get("maximumBytes") is not None:
        errors.append("library growth beyond 30 GB must remain possible without a hard cap")
    engine = data.get("engine", {})
    for key in ("sampleDiscovery", "sampleAnalysis", "sampleDistribution"):
        if engine.get(key) != "AUTOMATIC" and not (
            key == "sampleClassification"
            and engine.get(key) == "AUTOMATIC_WITH_CONFIDENCE_AND_REVIEW_GATES"
        ):
            errors.append(f"{key} must be automatic")
    if engine.get("manualPerSampleAssignmentRequired") is not False:
        errors.append("manual per-sample assignment must not be required")
    for key in ("audioCallbackDiskIoAllowed", "audioCallbackBlockingAllowed",
                "audioCallbackUnboundedAllocationAllowed"):
        if engine.get(key) is not False:
            errors.append(f"{key} must remain false")
    if engine.get("preserveSourceLicenseAndProvenance") is not True:
        errors.append("license/provenance preservation is mandatory")
    if engine.get("preserveSha256Integrity") is not True:
        errors.append("SHA-256 integrity verification is mandatory")
    gates = data.get("releaseGates", {})
    required_gates = (
        "basicPackageMustContainAllRequiredAssets",
        "all500FactoryPresetsMustResolveAgainstBasic",
        "expandedMustNotBeRequiredForBasicPresetPlayback",
        "noPaddingOrDuplicateAudioToReachTargetSize",
        "noAssetMayBeRedistributedWithoutVerifiedRights",
        "measuredPackageSizeMustBeReported",
    )
    for key in required_gates:
        if gates.get(key) is not True:
            errors.append(f"release gate {key} must be true")
    return errors

def main() -> int:
    path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(__file__).resolve().parents[1] / "assets/library/library_tiers.json"
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        print(f"FAIL: cannot read library tier contract: {exc}")
        return 1
    errors = validate(data)
    if errors:
        for error in errors:
            print(f"FAIL: {error}")
        return 1
    print("PASS: BASIC 15 GB included; EXPANDED +15 GB (30 GB initial total), expandable; 500 factory presets covered.")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
