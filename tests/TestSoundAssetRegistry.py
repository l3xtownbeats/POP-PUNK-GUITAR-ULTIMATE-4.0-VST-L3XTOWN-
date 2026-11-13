#!/usr/bin/env python3
"""CLI regression tests for the PPGU asset-level license/provenance policy gate."""
from __future__ import annotations

import copy
import hashlib
import json
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VALIDATOR = ROOT / "tools" / "validate_sound_asset_registry.py"


def CHECK(condition: bool, message: str) -> None:
    """Fail immediately with a non-zero process exit; independent of Python assert."""
    if not condition:
        print(f"FAIL: {message}", file=sys.stderr)
        sys.exit(1)


def make_base_asset(root: Path) -> dict:
    (root / "samples").mkdir(exist_ok=True)
    (root / "licenses").mkdir(exist_ok=True)
    sample = root / "samples" / "guitar.wav"
    sample.write_bytes(b"test-audio-fixture")
    (root / "licenses" / "CC0.txt").write_text(
        "CC0 evidence fixture", encoding="utf-8"
    )
    digest = hashlib.sha256(sample.read_bytes()).hexdigest()
    return {
        "assetId": "asset-001",
        "decision": "GREEN",
        "source": {
            "name": "Example source",
            "assetId": "source-item-001",
            "pageUrl": "https://example.org/item/1",
            "downloadUrl": "https://example.org/files/guitar.wav",
            "retrievedAt": "2026-11-11",
        },
        "file": {
            "relativePath": "samples/guitar.wav",
            "sha256": digest,
            "directDownload": True,
        },
        "license": {
            "spdxId": "CC0-1.0",
            "clearanceBasis": "CC0_PD_VERIFIED",
            "licenseUrl": "https://creativecommons.org/publicdomain/zero/1.0/",
            "evidencePath": "licenses/CC0.txt",
            "verifiedAt": "2026-11-11",
            "commercialUse": True,
            "redistribution": True,
            "modificationAllowed": True,
            "attributionRequired": False,
            "shareAlike": False,
            "additionalRestrictions": [],
            "legalReview": "APPROVED",
        },
        "screening": {
            "status": "PASS",
            "snrDb": 45.0,
            "clipping": False,
            "dcOffsetAbs": 0.00001,
            "integrity": "PASS",
        },
        "classification": {
            "guitarStatus": "GUITAR_CONFIRMED",
            "profiles": ["PP-ROOT"],
            "reviewRequired": False,
        },
        "curator": {
            "status": "UNREVIEWED",
            "copyrightSuspect": False,
            "manualReviewRequired": False,
            "humanListeningApproved": False,
        },
    }


def run_validator(root: Path, assets: list[dict]) -> subprocess.CompletedProcess[str]:
    registry_path = root / "registry.json"
    registry_path.write_text(
        json.dumps(
            {"schemaVersion": "1.0.0", "policy": "PPGU-REV-10.1", "assets": assets},
            ensure_ascii=False,
            indent=2,
        ),
        encoding="utf-8",
    )
    return subprocess.run(
        [
            sys.executable,
            str(VALIDATOR),
            "--registry",
            str(registry_path),
            "--root",
            str(root),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )


def expect_pass(root: Path, assets: list[dict], label: str, output: str) -> None:
    result = run_validator(root, assets)
    CHECK(result.returncode == 0, f"{label}: expected CLI exit 0; stdout={result.stdout!r}; stderr={result.stderr!r}")
    CHECK(output in result.stdout, f"{label}: expected output {output!r}; got {result.stdout!r}")
    print(f"PASS: {label}")


def expect_fail(root: Path, assets: list[dict], label: str, diagnostic: str) -> None:
    result = run_validator(root, assets)
    combined = result.stdout + result.stderr
    CHECK(result.returncode == 1, f"{label}: expected CLI exit 1; code={result.returncode}; output={combined!r}")
    CHECK(diagnostic in combined, f"{label}: expected diagnostic {diagnostic!r}; got {combined!r}")
    print(f"PASS: {label}")


def run_tests() -> None:
    with tempfile.TemporaryDirectory(prefix="ppgu-sound-registry-") as temp:
        root = Path(temp)
        base = make_base_asset(root)

        expect_pass(root, [copy.deepcopy(base)], "complete GREEN provenance", "1 GREEN AUTO ADD eligible")

        yellow = copy.deepcopy(base)
        yellow["decision"] = "YELLOW"
        expect_pass(root, [yellow], "YELLOW never enters AUTO ADD", "0 GREEN AUTO ADD eligible")

        red_override = copy.deepcopy(base)
        red_override["decision"] = "RED"
        red_override["curator"].update(status="GOLD", humanListeningApproved=True)
        expect_fail(root, [red_override], "RED cannot be overridden", "non-overridable")

        non_direct = copy.deepcopy(base)
        non_direct["file"]["directDownload"] = False
        expect_fail(root, [non_direct], "non-direct download blocks GREEN", "direct file")

        bad_hash = copy.deepcopy(base)
        bad_hash["file"]["sha256"] = "0" * 64
        expect_fail(root, [bad_hash], "SHA-256 mismatch blocks GREEN", "does not match")

        false_cc0 = copy.deepcopy(base)
        false_cc0["license"]["spdxId"] = "CC-BY-4.0"
        expect_fail(root, [false_cc0], "CC-BY cannot claim CC0 clearance", "CC0_PD_VERIFIED requires CC0/Public-Domain SPDX")

        (root / "licenses" / "permission.txt").write_text(
            "Permission evidence fixture", encoding="utf-8"
        )
        explicit_pending = copy.deepcopy(base)
        explicit_pending["license"].update(
            spdxId="LicenseRef-Explicit-Permission",
            clearanceBasis="EXPLICIT_PERMISSION",
            legalReview="PENDING",
            permissionScope=["commercial_use", "embedding", "redistribution", "sample_library_redistribution"],
            permissionEvidencePath="licenses/permission.txt",
            attributionRequired=False,
        )
        expect_fail(root, [explicit_pending], "EXPLICIT_PERMISSION PENDING blocks GREEN", "explicit permission requires APPROVED evidence review")

        copyright_suspect = copy.deepcopy(base)
        copyright_suspect["curator"]["copyrightSuspect"] = True
        expect_fail(root, [copyright_suspect], "copyright-suspect asset blocks GREEN", "copyright-suspect")

        weak_guitar = copy.deepcopy(base)
        weak_guitar["classification"]["guitarStatus"] = "GUITAR_REVIEW"
        weak_guitar["screening"]["snrDb"] = 32.0
        weak_result = run_validator(root, [weak_guitar])
        weak_output = weak_result.stdout + weak_result.stderr
        CHECK(weak_result.returncode == 1, "unconfirmed guitar / weak SNR must fail CLI")
        CHECK("guitar content is not confirmed" in weak_output, "guitar classification diagnostic missing")
        CHECK("below 40 dB" in weak_output, "SNR diagnostic missing")
        print("PASS: guitar classification and quality gates")

        unapproved_gold = copy.deepcopy(base)
        unapproved_gold["curator"].update(status="GOLD", humanListeningApproved=False)
        expect_fail(root, [unapproved_gold], "GOLD requires human listening approval", "humanListeningApproved")

        duplicate = copy.deepcopy(base)
        duplicate_yellow = copy.deepcopy(base)
        duplicate_yellow["decision"] = "YELLOW"
        expect_fail(root, [duplicate, duplicate_yellow], "duplicate asset IDs are rejected", "duplicate assetId")

        cc_by = copy.deepcopy(base)
        cc_by["license"].update(
            spdxId="CC-BY-4.0",
            clearanceBasis="CC_BY_ATTRIBUTION",
            licenseUrl="https://creativecommons.org/licenses/by/4.0/",
            attributionRequired=True,
            attributionText="Author — Guitar Sample, CC BY 4.0",
            legalReview="PENDING",
        )
        expect_pass(root, [cc_by], "CC-BY with exact asset-level attribution and PENDING review", "1 GREEN AUTO ADD eligible")

        cc0_pending = copy.deepcopy(base)
        cc0_pending["license"]["legalReview"] = "PENDING"
        expect_pass(root, [cc0_pending], "CC0 PENDING does not require explicit-permission review", "1 GREEN AUTO ADD eligible")

        explicit_approved = copy.deepcopy(base)
        explicit_approved["license"].update(
            spdxId="LicenseRef-Explicit-Permission",
            clearanceBasis="EXPLICIT_PERMISSION",
            legalReview="APPROVED",
            permissionScope=["commercial_use", "embedding", "redistribution", "sample_library_redistribution"],
            permissionEvidencePath="licenses/permission.txt",
            attributionRequired=False,
        )
        expect_pass(root, [explicit_approved], "approved explicit permission", "1 GREEN AUTO ADD eligible")

        own_dsp = copy.deepcopy(base)
        own_dsp["license"].update(
            spdxId="LicenseRef-PPGU-Own-DSP",
            clearanceBasis="OWN_DSP",
            ownershipConfirmed=True,
            generator="PPGU IR Synth",
            generatorVersion="1.0",
            parameterSummary="seed=42; room=small",
            permissionScope=["commercial_use", "embedding", "redistribution", "sample_library_redistribution"],
            attributionRequired=False,
        )
        expect_pass(root, [own_dsp], "own DSP generator provenance", "1 GREEN AUTO ADD eligible")

        red_candidate = copy.deepcopy(base)
        red_candidate["decision"] = "RED"
        red_candidate["curator"]["copyrightSuspect"] = True
        expect_pass(root, [red_candidate], "RED candidate can be recorded without AUTO ADD", "0 GREEN AUTO ADD eligible")

        cc_by_sa_yellow = copy.deepcopy(base)
        cc_by_sa_yellow["decision"] = "YELLOW"
        cc_by_sa_yellow["license"].update(
            spdxId="CC-BY-SA-4.0",
            clearanceBasis="CC_BY_SA_REVIEW",
        )
        expect_pass(root, [cc_by_sa_yellow], "CC-BY-SA remains manual review", "0 GREEN AUTO ADD eligible")

        expect_pass(root, [], "empty registry approves no assets", "0 asset(s), 0 GREEN AUTO ADD eligible")

    print("Sound asset registry CLI tests: 18/18 PASS")


if __name__ == "__main__":
    run_tests()
