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

SCHEMA_VERSION = "1.0.0"
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
    parsed = urlparse(value)
    return parsed.scheme in {"http", "https"} and bool(parsed.netloc)


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
    """Return policy eligibility and the exact unmet gates for an asset."""
    reasons: list[str] = []
    source = asset.get("source", {})
    file_info = asset.get("file", {})
    license_info = asset.get("license", {})
    screening = asset.get("screening", {})
    classification = asset.get("classification", {})
    curator = asset.get("curator", {})

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
        candidate = (root / relative).resolve()
        try:
            candidate.relative_to(root.resolve())
        except ValueError:
            reasons.append("sample path escapes registry root")
        else:
            if not candidate.is_file():
                reasons.append("sample file is missing")
            else:
                expected = file_info.get("sha256", "")
                if not re.fullmatch(r"[0-9a-fA-F]{64}", expected or ""):
                    reasons.append("SHA-256 is missing or malformed")
                elif _sha256(candidate).lower() != expected.lower():
                    reasons.append("SHA-256 does not match the local sample")
    if not _nonempty(source.get("name")) or not _nonempty(source.get("assetId")):
        reasons.append("source name and source asset ID are required")
    if not _valid_date(source.get("retrievedAt")):
        reasons.append("retrieval date is missing or invalid")
    if not _valid_date(license_info.get("verifiedAt")):
        reasons.append("license verification date is missing or invalid")
    if not _valid_url(license_info.get("licenseUrl")):
        reasons.append("license URL/evidence link is missing or invalid")
    if not _nonempty(license_info.get("evidencePath")):
        reasons.append("archived license evidence path is missing")
    else:
        evidence = (root / license_info["evidencePath"]).resolve()
        try:
            evidence.relative_to(root.resolve())
        except ValueError:
            reasons.append("license evidence path escapes registry root")
        else:
            if not evidence.is_file():
                reasons.append("archived license evidence file is missing")

    if license_info.get("spdxId") not in ALLOWED_GREEN_LICENSES:
        reasons.append("license is not an explicitly approved CC0/Public-Domain identifier")
    for key in ("commercialUse", "redistribution", "modificationAllowed"):
        if license_info.get(key) is not True:
            reasons.append(f"license does not explicitly permit {key}")
    if license_info.get("legalReview") != "APPROVED":
        reasons.append("asset-level legal review is not APPROVED")
    if license_info.get("attributionRequired") is not False:
        reasons.append("attribution requirement is unresolved or present")
    if license_info.get("shareAlike") is not False:
        reasons.append("share-alike condition is unresolved or present")
    if license_info.get("additionalRestrictions") not in ([], None):
        reasons.append("additional license restrictions require manual review")

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
    if not isinstance(profiles, list) or not profiles or any(p not in PROFILES for p in profiles):
        reasons.append("one or more valid style profiles are required")
    if classification.get("reviewRequired") is not False:
        reasons.append("classification requires manual review")

    if curator.get("copyrightSuspect") is not False:
        reasons.append("copyright-suspect flag is set or unresolved")
    if curator.get("manualReviewRequired") is not False:
        reasons.append("manual curator review is required")
    if curator.get("status") == "REJECT":
        reasons.append("asset was rejected by curator")
    return not reasons, reasons


def validate_registry(data: object, root: Path) -> tuple[list[str], int]:
    errors: list[str] = []
    if not isinstance(data, dict):
        return ["registry root must be a JSON object"], 0
    _check(data.get("schemaVersion") == SCHEMA_VERSION, errors, "registry", f"schemaVersion must be {SCHEMA_VERSION}")
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
        _check(decision in DECISIONS, errors, where, "decision must be GREEN, YELLOW or RED")
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
        _check(re.fullmatch(r"[0-9a-fA-F]{64}", file_info.get("sha256", "") or "") is not None,
               errors, where, "file.sha256 must be 64 hexadecimal characters")

        _check(_nonempty(license_info.get("spdxId")), errors, where, "license.spdxId is required")
        _check(_valid_url(license_info.get("licenseUrl")), errors, where, "license.licenseUrl must be HTTP(S)")
        _check(_nonempty(license_info.get("evidencePath")), errors, where, "license.evidencePath is required")
        _check(_valid_date(license_info.get("verifiedAt")), errors, where, "license.verifiedAt must be ISO date")
        for key in ("commercialUse", "redistribution", "modificationAllowed", "attributionRequired", "shareAlike"):
            _check(isinstance(license_info.get(key), bool), errors, where, f"license.{key} must be boolean")
        _check(license_info.get("legalReview") in {"PENDING", "APPROVED", "REJECTED"},
               errors, where, "license.legalReview must be PENDING, APPROVED or REJECTED")
        _check(isinstance(license_info.get("additionalRestrictions", []), list),
               errors, where, "license.additionalRestrictions must be an array")

        _check(screening.get("status") in {"PENDING", "PASS", "FAIL"}, errors, where, "screening.status is invalid")
        _check(classification.get("guitarStatus") in GUITAR_STATES, errors, where, "classification.guitarStatus is invalid")
        profiles = classification.get("profiles", [])
        _check(isinstance(profiles, list) and all(p in PROFILES for p in profiles),
               errors, where, "classification.profiles contains an invalid profile")
        _check(isinstance(classification.get("reviewRequired"), bool), errors, where, "classification.reviewRequired must be boolean")
        _check(curator.get("status") in CURATOR_STATES, errors, where, "curator.status is invalid")
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
