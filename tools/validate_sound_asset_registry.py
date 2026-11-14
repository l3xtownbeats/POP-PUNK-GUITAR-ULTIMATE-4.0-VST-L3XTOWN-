#!/usr/bin/env python3
"""Validate asset-level Sound Hunter licensing and provenance records.

This is a metadata/policy gate only. It does not perform legal review,
download files, identify guitar content, or measure audio quality.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import math
import re
import sys
from datetime import date
from pathlib import Path
from urllib.parse import urlparse

SUPPORTED_SCHEMA_VERSIONS = {"1.0.0", "1.1.0"}
DECISIONS = {"GREEN", "YELLOW", "RED"}
PROFILES = {"PP-ROOT", "PP-NEO2000", "PP-MODERN", "PP-EMO", "UNCATEGORIZED"}
CURATOR_STATES = {"UNREVIEWED", "GOLD", "KEEP", "REJECT"}
GUITAR_STATES = {"GUITAR_CONFIRMED", "GUITAR_REVIEW", "NOT_GUITAR"}
ALLOWED_GREEN_LICENSES = {"CC0-1.0", "PDM-1.0", "PUBLIC-DOMAIN"}


def _nonempty(value) -> bool:
    return isinstance(value, str) and bool(value.strip())


def _valid_date(value) -> bool:
    if not _nonempty(value):
        return False
    try:
        date.fromisoformat(value[:10])
        return True
    except ValueError:
        return False


def _valid_url(value) -> bool:
    if not _nonempty(value):
        return False
    try:
        parsed = urlparse(value)
        return parsed.scheme in {"http", "https"} and bool(parsed.netloc)
    except ValueError:
        return False


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _check(condition: bool, errors: list[str], where: str, message: str) -> None:
    if not condition:
        errors.append(f"{where}: {message}")


def green_auto_add_gates(asset: dict, root: Path) -> tuple[bool, list[str]]:
    """Check technical and asset-level distribution clearance for factory GREEN."""
    reasons: list[str] = []
    source, file_info = asset.get("source", {}), asset.get("file", {})
    license_info, screening = asset.get("license", {}), asset.get("screening", {})
    classification, curator = asset.get("classification", {}), asset.get("curator", {})
    if asset.get("decision") != "GREEN":
        reasons.append("asset decision is not GREEN")
    if not _valid_url(source.get("pageUrl")) or not _valid_url(source.get("downloadUrl")):
        reasons.append("source page/download URL is missing or invalid")
    if file_info.get("directDownload") is not True:
        reasons.append("download is not confirmed as a direct file")
    relative = file_info.get("relativePath")
    if not _nonempty(relative):
        reasons.append("local relativePath is missing")
    else:
        try:
            candidate = (root / relative).resolve()
            candidate.relative_to(root.resolve())
        except (OSError, ValueError, RuntimeError):
            reasons.append("sample path is invalid or escapes registry root")
        else:
            if not candidate.is_file():
                reasons.append("sample file is missing")
            else:
                expected = file_info.get("sha256", "")
                if not isinstance(expected, str) or not re.fullmatch(r"[0-9a-fA-F]{64}", expected):
                    reasons.append("SHA-256 is missing or malformed")
                else:
                    try:
                        actual_hash = _sha256(candidate).lower()
                    except OSError:
                        reasons.append("sample file cannot be read for SHA-256 verification")
                    else:
                        if actual_hash != expected.lower():
                            reasons.append("SHA-256 does not match the local sample")
    if not _nonempty(source.get("name")) or not _nonempty(source.get("assetId")):
        reasons.append("source name and source asset ID are required")
    if not _valid_date(source.get("retrievedAt")):
        reasons.append("retrieval date is missing or invalid")
    if not _valid_date(license_info.get("verifiedAt")):
        reasons.append("license verification date is missing or invalid")
    if not _valid_url(license_info.get("licenseUrl")):
        reasons.append("license URL/evidence link is missing or invalid")
    evidence_path = license_info.get("evidencePath")
    if not _nonempty(evidence_path):
        reasons.append("archived license evidence path is missing")
    else:
        try:
            evidence = (root / evidence_path).resolve()
            evidence.relative_to(root.resolve())
        except (OSError, ValueError, RuntimeError):
            reasons.append("license evidence path is invalid or escapes registry root")
        else:
            if not evidence.is_file():
                reasons.append("archived license evidence file is missing")

    basis, spdx = license_info.get("clearanceBasis"), license_info.get("spdxId")
    for key in ("commercialUse", "redistribution", "modificationAllowed"):
        if license_info.get(key) is not True:
            reasons.append(f"license does not explicitly permit {key}")
    if license_info.get("additionalRestrictions") not in ([], None):
        reasons.append("additional license restrictions require review")
    if license_info.get("shareAlike") is not False:
        reasons.append("share-alike condition is unresolved or present")

    required_scope = {"commercial_use", "embedding", "redistribution", "sample_library_redistribution"}
    scope = license_info.get("permissionScope", [])
    if not isinstance(scope, list):
        scope = []
    if basis == "CC0_PD_VERIFIED":
        if spdx not in ALLOWED_GREEN_LICENSES:
            reasons.append("CC0_PD_VERIFIED requires CC0/Public-Domain SPDX")
        if license_info.get("attributionRequired") is not False:
            reasons.append("CC0/Public Domain must not carry unresolved attribution obligations")
    elif basis == "CC_BY_ATTRIBUTION":
        if not isinstance(spdx, str) or re.fullmatch(r"CC-BY-(?:2.0|2.5|3.0|4.0)", spdx) is None:
            reasons.append("CC_BY_ATTRIBUTION requires a standard CC-BY SPDX identifier")
        if license_info.get("attributionRequired") is not True or not _nonempty(license_info.get("attributionText")):
            reasons.append("CC-BY requires exact per-asset attributionText")
        if license_info.get("legalReview") == "REJECTED":
            reasons.append("CC-BY license was rejected")
    elif basis == "EXPLICIT_PERMISSION":
        if spdx != "LicenseRef-Explicit-Permission":
            reasons.append("EXPLICIT_PERMISSION requires LicenseRef-Explicit-Permission")
        if not required_scope.issubset(set(scope)):
            reasons.append("explicit permission scope must cover commercial use, embedding, redistribution and sample-library redistribution")
        if license_info.get("legalReview") != "APPROVED":
            reasons.append("explicit permission requires APPROVED evidence review")
        ppath = license_info.get("permissionEvidencePath")
        if not _nonempty(ppath):
            reasons.append("explicit permission evidence path is missing")
        else:
            try:
                pfile = (root / ppath).resolve()
                pfile.relative_to(root.resolve())
                if not pfile.is_file():
                    reasons.append("permission evidence file is missing")
            except (OSError, ValueError, RuntimeError):
                reasons.append("permission evidence path escapes registry root")
    elif basis == "OWN_RECORDING":
        if spdx != "LicenseRef-PPGU-Own-Recording":
            reasons.append("OWN_RECORDING requires LicenseRef-PPGU-Own-Recording")
        if license_info.get("ownershipConfirmed") is not True or not _nonempty(license_info.get("rightsHolder")):
            reasons.append("own recording requires ownershipConfirmed and rightsHolder")
        if not required_scope.issubset(set(scope)):
            reasons.append("own recording rights scope is incomplete")
    elif basis == "OWN_DSP":
        if spdx != "LicenseRef-PPGU-Own-DSP":
            reasons.append("OWN_DSP requires LicenseRef-PPGU-Own-DSP")
        for key in ("generator", "generatorVersion", "parameterSummary"):
            if not _nonempty(license_info.get(key)):
                reasons.append(f"own DSP provenance requires {key}")
        if license_info.get("ownershipConfirmed") is not True:
            reasons.append("own DSP provenance requires ownershipConfirmed")
        if not required_scope.issubset(set(scope)):
            reasons.append("own DSP rights scope is incomplete")
    elif basis == "COMMERCIAL_REDISTRIBUTION":
        if spdx != "LicenseRef-Commercial-Royalty-Free":
            reasons.append("COMMERCIAL_REDISTRIBUTION requires LicenseRef-Commercial-Royalty-Free")
        if not required_scope.issubset(set(scope)):
            reasons.append("commercial license scope must cover commercial use, embedding, redistribution and sample-library redistribution")
        if license_info.get("legalReview") != "APPROVED":
            reasons.append("commercial redistribution license requires APPROVED evidence review")
    else:
        reasons.append("clearanceBasis is missing or not approved for factory GREEN")

    if screening.get("status") != "PASS":
        reasons.append("technical screening has not passed")
    snr = screening.get("snrDb")
    if isinstance(snr, bool) or not isinstance(snr, (int, float)) or not math.isfinite(snr) or snr < 40.0:
        reasons.append("SNR is missing or below 40 dB")
    if screening.get("clipping") is not False:
        reasons.append("clipping is present or not checked")
    dc = screening.get("dcOffsetAbs")
    if isinstance(dc, bool) or not isinstance(dc, (int, float)) or not math.isfinite(dc) or dc > 0.0001:
        reasons.append("DC offset is missing or exceeds 0.0001")
    if screening.get("integrity") != "PASS":
        reasons.append("file integrity check has not passed")
    if classification.get("guitarStatus") != "GUITAR_CONFIRMED":
        reasons.append("guitar content is not confirmed")
    profiles = classification.get("profiles")
    if not isinstance(profiles, list) or not profiles or any(
            not isinstance(profile, str) or profile not in PROFILES for profile in profiles):
        reasons.append("one or more valid style profiles are required")
    if classification.get("reviewRequired") is not False:
        reasons.append("classification requires manual review")
    if curator.get("copyrightSuspect") is not False:
        reasons.append("copyright-suspect flag is unresolved; GREEN is blocked")
    if curator.get("manualReviewRequired") is not False:
        reasons.append("manual curator review is required")
    if curator.get("status") == "REJECT":
        reasons.append("asset was rejected by curator")
    return not reasons, reasons

def validate_registry(data: object, root: Path) -> tuple[list[str], int]:
    errors: list[str] = []
    if not isinstance(data, dict):
        return ["registry root must be a JSON object"], 0
    _check(data.get("schemaVersion") in SUPPORTED_SCHEMA_VERSIONS, errors, "registry", f"schemaVersion must be one of {sorted(SUPPORTED_SCHEMA_VERSIONS)}")
    _check(data.get("policy") == "PPGU-REV-10.1", errors, "registry", "policy must be PPGU-REV-10.1")
    assets = data.get("assets")
    _check(isinstance(assets, list), errors, "registry", "assets must be an array")
    if not isinstance(assets, list):
        return errors, 0

    seen_ids: set[str] = set()
    auto_add_count = 0
    for index, asset in enumerate(assets):
        where = f"assets[{index}]"
        if not isinstance(asset, dict):
            errors.append(f"{where}: asset must be an object")
            continue
        asset_id = asset.get("assetId")
        _check(_nonempty(asset_id), errors, where, "assetId is required")
        if _nonempty(asset_id):
            _check(asset_id not in seen_ids, errors, where, f"duplicate assetId {asset_id!r}")
            seen_ids.add(asset_id)

        decision = asset.get("decision")
        _check(isinstance(decision, str) and decision in DECISIONS, errors, where, "decision must be GREEN, YELLOW or RED")
        for section in ("source", "file", "license", "screening", "classification", "curator"):
            _check(isinstance(asset.get(section), dict), errors, where, f"{section} must be an object")
        if any(not isinstance(asset.get(section), dict) for section in ("source", "file", "license", "screening", "classification", "curator")):
            continue

        source, file_info = asset["source"], asset["file"]
        license_info, screening = asset["license"], asset["screening"]
        classification, curator = asset["classification"], asset["curator"]

        _check(_nonempty(source.get("name")), errors, where, "source.name is required")
        _check(_nonempty(source.get("assetId")), errors, where, "source.assetId is required")
        _check(_valid_url(source.get("pageUrl")), errors, where, "source.pageUrl must be HTTP(S)")
        _check(_valid_url(source.get("downloadUrl")), errors, where, "source.downloadUrl must be HTTP(S)")
        _check(_valid_date(source.get("retrievedAt")), errors, where, "source.retrievedAt must be ISO date")
        _check(isinstance(file_info.get("directDownload"), bool), errors, where, "file.directDownload must be boolean")
        _check(_nonempty(file_info.get("relativePath")), errors, where, "file.relativePath is required")
        _check(isinstance(file_info.get("sha256"), str) and re.fullmatch(r"[0-9a-fA-F]{64}", file_info.get("sha256", "")) is not None,
               errors, where, "file.sha256 must be 64 hexadecimal characters")

        _check(_nonempty(license_info.get("spdxId")), errors, where, "license.spdxId is required")
        _check(_valid_url(license_info.get("licenseUrl")), errors, where, "license.licenseUrl must be HTTP(S)")
        _check(_nonempty(license_info.get("evidencePath")), errors, where, "license.evidencePath is required")
        _check(_valid_date(license_info.get("verifiedAt")), errors, where, "license.verifiedAt must be ISO date")
        for key in ("commercialUse", "redistribution", "modificationAllowed", "attributionRequired", "shareAlike"):
            _check(isinstance(license_info.get(key), bool), errors, where, f"license.{key} must be boolean")
        _check(isinstance(license_info.get("legalReview"), str) and license_info.get("legalReview") in {"PENDING", "APPROVED", "REJECTED"},
               errors, where, "license.legalReview must be PENDING, APPROVED or REJECTED")
        _check(isinstance(license_info.get("additionalRestrictions", []), list),
               errors, where, "license.additionalRestrictions must be an array")

        _check(isinstance(screening.get("status"), str) and screening.get("status") in {"PENDING", "PASS", "FAIL"}, errors, where, "screening.status is invalid")
        _check(isinstance(classification.get("guitarStatus"), str) and classification.get("guitarStatus") in GUITAR_STATES, errors, where, "classification.guitarStatus is invalid")
        profiles = classification.get("profiles", [])
        _check(isinstance(profiles, list) and all(isinstance(p, str) and p in PROFILES for p in profiles),
               errors, where, "classification.profiles contains an invalid profile")
        _check(isinstance(classification.get("reviewRequired"), bool), errors, where, "classification.reviewRequired must be boolean")
        _check(isinstance(curator.get("status"), str) and curator.get("status") in CURATOR_STATES, errors, where, "curator.status is invalid")
        _check(isinstance(curator.get("copyrightSuspect"), bool), errors, where, "curator.copyrightSuspect must be boolean")
        _check(isinstance(curator.get("manualReviewRequired"), bool), errors, where, "curator.manualReviewRequired must be boolean")
        if curator.get("status") == "GOLD":
            _check(curator.get("humanListeningApproved") is True, errors, where,
                   "GOLD requires humanListeningApproved=true")
        if decision == "RED":
            _check(curator.get("status") not in {"GOLD", "KEEP"}, errors, where,
                   "RED is non-overridable and cannot be approved by curator")
        if decision == "YELLOW":
            _check(curator.get("status") != "GOLD", errors, where,
                   "YELLOW requires manual import and cannot become factory GOLD")
        if decision == "GREEN":
            eligible, reasons = green_auto_add_gates(asset, root)
            _check(eligible, errors, where, "GREEN is not AUTO ADD eligible: " + "; ".join(reasons))
            if eligible:
                auto_add_count += 1
    return errors, auto_add_count


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--registry", type=Path, default=Path("assets/sound_hunter/registry.json"))
    parser.add_argument("--root", type=Path, default=Path("."),
                        help="root used to resolve sample and archived license evidence paths")
    args = parser.parse_args()
    try:
        data = json.loads(args.registry.read_text(encoding="utf-8"))
    except Exception as exc:
        print(f"Sound asset registry: cannot read JSON: {exc}", file=sys.stderr)
        return 2
    errors, auto_add_count = validate_registry(data, args.root.resolve())
    if errors:
        print("Sound asset registry validation FAILED:")
        for error in errors:
            print(f" - {error}")
        return 1
    print(f"Sound asset registry OK: {len(data['assets'])} asset(s), {auto_add_count} GREEN AUTO ADD eligible")
    if not data["assets"]:
        print("No assets registered yet; no sample content is approved by this empty registry.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
