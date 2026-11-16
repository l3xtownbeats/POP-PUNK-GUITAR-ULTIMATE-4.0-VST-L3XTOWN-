#!/usr/bin/env python3
"""Regression tests for the approved PPGU library tiers and package rules."""
from __future__ import annotations
import copy
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from validate_library_tiers import validate

class LibraryTierContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.contract = json.loads((ROOT / "assets/library/library_tiers.json").read_text(encoding="utf-8"))

    def test_approved_baseline_passes(self):
        self.assertEqual(validate(self.contract), [])

    def test_basic_must_be_in_standard_package(self):
        data = copy.deepcopy(self.contract)
        data["libraries"]["basic"]["distribution"] = "OPTIONAL_DOWNLOAD"
        self.assertTrue(any("included in the standard product package" in e for e in validate(data)))

    def test_all_500_factory_presets_must_work_without_expanded(self):
        data = copy.deepcopy(self.contract)
        data["factoryPresets"]["requiresExpandedLibrary"] = True
        self.assertTrue(any("must not require EXPANDED" in e for e in validate(data)))

    def test_expanded_is_15gb_addon_and_30gb_combined(self):
        data = copy.deepcopy(self.contract)
        data["libraries"]["expanded"]["initialCombinedBytes"] = 45_000_000_000
        self.assertTrue(any("30 GB (15 + 15)" in e for e in validate(data)))

    def test_expanded_has_no_hard_maximum(self):
        data = copy.deepcopy(self.contract)
        data["libraries"]["expanded"]["maximumBytes"] = 30_000_000_000
        self.assertTrue(any("without a hard cap" in e for e in validate(data)))

    def test_audio_callback_remains_realtime_safe(self):
        data = copy.deepcopy(self.contract)
        data["engine"]["audioCallbackDiskIoAllowed"] = True
        self.assertTrue(any("audioCallbackDiskIoAllowed" in e for e in validate(data)))

    def test_duplicate_or_padding_audio_is_not_accepted_as_size_fill(self):
        data = copy.deepcopy(self.contract)
        data["releaseGates"]["noPaddingOrDuplicateAudioToReachTargetSize"] = False
        self.assertTrue(any("noPaddingOrDuplicateAudioToReachTargetSize" in e for e in validate(data)))

if __name__ == "__main__":
    unittest.main(verbosity=2)
