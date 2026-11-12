#!/usr/bin/env python3
"""Unit tests for the REV 10.1 asset-level license/provenance policy gate."""
from __future__ import annotations

import hashlib
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from validate_sound_asset_registry import validate_registry  # noqa: E402


class SoundAssetRegistryTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        (self.root / "samples").mkdir()
        (self.root / "licenses").mkdir()
        (self.root / "samples" / "guitar.wav").write_bytes(b"test-audio-fixture")
        (self.root / "licenses" / "CC0.txt").write_text("CC0 evidence fixture", encoding="utf-8")
        digest = hashlib.sha256((self.root / "samples" / "guitar.wav").read_bytes()).hexdigest()
        self.asset = {
            "assetId": "asset-001",
            "decision": "GREEN",
            "source": {
                "name": "Example source", "assetId": "source-item-001",
                "pageUrl": "https://example.org/item/1",
                "downloadUrl": "https://example.org/files/guitar.wav",
                "retrievedAt": "2026-11-11"
            },
            "file": {"relativePath": "samples/guitar.wav", "sha256": digest, "directDownload": True},
            "license": {
                "spdxId": "CC0-1.0", "clearanceBasis": "CC0_PD_VERIFIED", "licenseUrl": "https://creativecommons.org/publicdomain/zero/1.0/",
                "evidencePath": "licenses/CC0.txt", "verifiedAt": "2026-11-11",
                "commercialUse": True, "redistribution": True, "modificationAllowed": True,
                "attributionRequired": False, "shareAlike": False,
                "additionalRestrictions": [], "legalReview": "APPROVED"
            },
            "screening": {
                "status": "PASS", "snrDb": 45.0, "clipping": False,
                "dcOffsetAbs": 0.00001, "integrity": "PASS"
            },
            "classification": {
                "guitarStatus": "GUITAR_CONFIRMED", "profiles": ["PP-ROOT"],
                "reviewRequired": False
            },
            "curator": {
                "status": "UNREVIEWED", "copyrightSuspect": False,
                "manualReviewRequired": False, "humanListeningApproved": False
            }
        }

    def tearDown(self):
        self.temp.cleanup()

    def registry(self, asset=None):
        return {"schemaVersion": "1.0.0", "policy": "PPGU-REV-10.1",
                "assets": [self.asset if asset is None else asset]}

    def errors(self, asset=None):
        return validate_registry(self.registry(asset), self.root)[0]

    def test_green_with_complete_provenance_and_clearance_passes(self):
        errors, count = validate_registry(self.registry(), self.root)
        self.assertEqual(errors, [])
        self.assertEqual(count, 1)

    def test_yellow_never_enters_auto_add(self):
        yellow = {**self.asset, "decision": "YELLOW"}
        errors, count = validate_registry(self.registry(yellow), self.root)
        self.assertEqual(errors, [])
        self.assertEqual(count, 0)

    def test_red_cannot_be_overridden_by_curator(self):
        red = {**self.asset, "decision": "RED",
               "curator": {**self.asset["curator"], "status": "GOLD", "humanListeningApproved": True}}
        self.assertTrue(any("non-overridable" in e for e in self.errors(red)))

    def test_html_or_non_direct_download_blocks_green(self):
        asset = {**self.asset, "file": {**self.asset["file"], "directDownload": False}}
        self.assertTrue(any("direct file" in e for e in self.errors(asset)))

    def test_hash_mismatch_blocks_green(self):
        asset = {**self.asset, "file": {**self.asset["file"], "sha256": "0" * 64}}
        self.assertTrue(any("does not match" in e for e in self.errors(asset)))

    def test_non_cc0_spdx_cannot_claim_cc0_clearance(self):
        asset = {**self.asset, "license": {**self.asset["license"], "spdxId": "CC-BY-4.0"}}
        self.assertTrue(any("CC0_PD_VERIFIED requires CC0/Public-Domain SPDX" in e
                            for e in self.errors(asset)))

    def test_unapproved_explicit_permission_blocks_green(self):
        (self.root / "licenses" / "permission.txt").write_text(
            "Permission evidence fixture", encoding="utf-8")
        asset = {**self.asset, "license": {
            **self.asset["license"],
            "spdxId": "LicenseRef-Explicit-Permission",
            "clearanceBasis": "EXPLICIT_PERMISSION",
            "legalReview": "PENDING",
            "permissionScope": ["commercial_use", "embedding", "redistribution",
                                "sample_library_redistribution"],
            "permissionEvidencePath": "licenses/permission.txt",
            "attributionRequired": False
        }}
        self.assertTrue(any("explicit permission requires APPROVED evidence review" in e
                            for e in self.errors(asset)))

    def test_copyright_suspect_blocks_green(self):
        asset = {**self.asset, "curator": {**self.asset["curator"], "copyrightSuspect": True}}
        self.assertTrue(any("copyright-suspect" in e for e in self.errors(asset)))

    def test_guitar_review_and_failed_quality_block_green(self):
        asset = {**self.asset,
                 "classification": {**self.asset["classification"], "guitarStatus": "GUITAR_REVIEW"},
                 "screening": {**self.asset["screening"], "snrDb": 32.0}}
        errors = self.errors(asset)
        self.assertTrue(any("guitar content is not confirmed" in e for e in errors))
        self.assertTrue(any("below 40 dB" in e for e in errors))

    def test_gold_requires_human_listening(self):
        asset = {**self.asset,
                 "curator": {**self.asset["curator"], "status": "GOLD", "humanListeningApproved": False}}
        self.assertTrue(any("humanListeningApproved" in e for e in self.errors(asset)))

    def test_duplicate_asset_ids_are_rejected(self):
        registry = self.registry()
        registry["assets"].append({**self.asset, "decision": "YELLOW"})
        errors, _ = validate_registry(registry, self.root)
        self.assertTrue(any("duplicate assetId" in e for e in errors))

    def test_cc_by_with_exact_attribution_is_factory_green(self):
        asset = {**self.asset, "license": {
            **self.asset["license"], "spdxId": "CC-BY-4.0",
            "clearanceBasis": "CC_BY_ATTRIBUTION",
            "licenseUrl": "https://creativecommons.org/licenses/by/4.0/",
            "attributionRequired": True,
            "attributionText": "Author — Guitar Sample, CC BY 4.0"
        }}
        errors, count = validate_registry(self.registry(asset), self.root)
        self.assertEqual(errors, [])
        self.assertEqual(count, 1)

    def test_looperman_explicit_permission_can_be_green(self):
        (self.root / "licenses" / "permission.txt").write_text(
            "Permission to embed and redistribute in commercial sample library.", encoding="utf-8")
        asset = {**self.asset, "license": {
            **self.asset["license"], "spdxId": "LicenseRef-Explicit-Permission",
            "clearanceBasis": "EXPLICIT_PERMISSION", "legalReview": "APPROVED",
            "permissionScope": ["commercial_use", "embedding", "redistribution",
                                "sample_library_redistribution"],
            "permissionEvidencePath": "licenses/permission.txt",
            "attributionRequired": False
        }}
        errors, count = validate_registry(self.registry(asset), self.root)
        self.assertEqual(errors, [])
        self.assertEqual(count, 1)

    def test_own_dsp_requires_generator_provenance(self):
        asset = {**self.asset, "license": {
            **self.asset["license"], "spdxId": "LicenseRef-PPGU-Own-DSP",
            "clearanceBasis": "OWN_DSP", "ownershipConfirmed": True,
            "generator": "PPGU IR Synth", "generatorVersion": "1.0",
            "parameterSummary": "seed=42; room=small",
            "permissionScope": ["commercial_use", "embedding", "redistribution",
                                "sample_library_redistribution"],
            "attributionRequired": False
        }}
        errors, count = validate_registry(self.registry(asset), self.root)
        self.assertEqual(errors, [])
        self.assertEqual(count, 1)

    def test_copyright_suspect_can_be_committed_as_red_candidate(self):
        red = {**self.asset, "decision": "RED",
               "curator": {**self.asset["curator"], "copyrightSuspect": True}}
        errors, count = validate_registry(self.registry(red), self.root)
        self.assertEqual(errors, [])
        self.assertEqual(count, 0)

    def test_cc_by_sa_is_not_auto_add(self):
        yellow = {**self.asset, "decision": "YELLOW",
                  "license": {**self.asset["license"], "spdxId": "CC-BY-SA-4.0",
                              "clearanceBasis": "CC_BY_SA_REVIEW"}}
        errors, count = validate_registry(self.registry(yellow), self.root)
        self.assertEqual(errors, [])
        self.assertEqual(count, 0)

    def test_empty_registry_is_valid_and_approves_nothing(self):
        errors, count = validate_registry(
            {"schemaVersion": "1.0.0", "policy": "PPGU-REV-10.1", "assets": []}, self.root)
        self.assertEqual(errors, [])
        self.assertEqual(count, 0)


if __name__ == "__main__":
    unittest.main(verbosity=2)
