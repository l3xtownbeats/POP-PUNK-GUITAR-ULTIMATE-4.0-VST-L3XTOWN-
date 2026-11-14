#!/usr/bin/env python3
"""Unit tests for the REV.18 READY evidence audit."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

# The validator lives in tools/, while this test is executed by path from the
# repository root. Add that directory explicitly so Windows and local runs
# resolve the same module without relying on PYTHONPATH side effects.
TOOLS_DIR = Path(__file__).resolve().parents[1] / "tools"
sys.path.insert(0, str(TOOLS_DIR))

from validate_rev18_factory_readiness import audit, ready_evidence_errors


def valid_ready(preset_id: str, mode: str) -> dict:
    return {
        "id": preset_id,
        "mode": mode,
        "status": "READY",
        "readiness": {
            "runtime": {"implemented": True, "loadedByPlugin": True},
            "sourceMappings": [{
                "assetId": "asset-001",
                "relativePath": "Samples/guitar.wav",
                "sha256": "a" * 64,
                "licenseEvidencePath": "Samples/LICENSE.txt",
                "commercialRedistributionApproved": True,
                "mappingVerified": True,
            }],
            "performanceValidation": {"status": "PASS"},
            "audioValidation": {"status": "PASS", "humanListeningApproved": True},
        },
    }


class Rev18ReadinessTests(unittest.TestCase):
    def test_legacy_catalog_definitions_are_not_ready(self):
        catalog = {
            "melodyPresets": [{"id": "M001", "mode": "MELODY", "status": "catalog-definition-awaiting-plugin-integration"}],
            "patternPresets": [{"id": "P001", "mode": "PATTERN", "status": "catalog-definition-awaiting-performance-library"}],
        }
        counts, errors = audit(catalog)
        self.assertEqual(counts, {"MELODY": 0, "PATTERN": 0})
        self.assertEqual(errors, [])

    def test_complete_ready_evidence_counts(self):
        counts, errors = audit({"melodyPresets": [valid_ready("M001", "MELODY")],
                                "patternPresets": [valid_ready("P001", "PATTERN")]})
        self.assertEqual(counts, {"MELODY": 1, "PATTERN": 1})
        self.assertEqual(errors, [])

    def test_ready_without_mapping_is_not_counted(self):
        preset = valid_ready("M002", "MELODY")
        preset["readiness"]["sourceMappings"] = []
        counts, errors = audit({"melodyPresets": [preset], "patternPresets": []})
        self.assertEqual(counts["MELODY"], 0)
        self.assertTrue(any("no source mappings" in error for error in errors))

    def test_ready_with_bad_hash_is_not_counted(self):
        preset = valid_ready("M003", "MELODY")
        preset["readiness"]["sourceMappings"][0]["sha256"] = "not-a-hash"
        counts, errors = audit({"melodyPresets": [preset], "patternPresets": []})
        self.assertEqual(counts["MELODY"], 0)
        self.assertTrue(any("sha256 is invalid" in error for error in errors))

    def test_ready_without_human_audio_approval_is_not_counted(self):
        preset = valid_ready("P002", "PATTERN")
        preset["readiness"]["audioValidation"]["humanListeningApproved"] = False
        counts, errors = audit({"melodyPresets": [], "patternPresets": [preset]})
        self.assertEqual(counts["PATTERN"], 0)
        self.assertTrue(any("human listening approval" in error for error in errors))

    def test_duplicate_ids_are_rejected(self):
        counts, errors = audit({"melodyPresets": [valid_ready("M004", "MELODY"), valid_ready("M004", "MELODY")],
                                "patternPresets": []})
        self.assertEqual(counts["MELODY"], 1)
        self.assertTrue(any("duplicate id" in error for error in errors))


if __name__ == "__main__":
    unittest.main(verbosity=2)
