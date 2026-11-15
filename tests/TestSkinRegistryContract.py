#!/usr/bin/env python3
"""Unit tests for the no-loss REV18 active skin registry contract."""
from __future__ import annotations

import copy
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

from validate_skin_pack import validate_registry_contract  # noqa: E402


class SkinRegistryContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.registry = json.loads((ROOT / "SKIN" / "theme_manifest.json").read_text(encoding="utf-8"))

    def test_current_registry_is_valid_and_keeps_all_historical_records(self) -> None:
        self.assertEqual(validate_registry_contract(copy.deepcopy(self.registry)), 1)
        self.assertEqual(len(self.registry["themes"]), self.registry["totalSkins"])
        self.assertEqual(self.registry["defaultSkin"], "skin-001")

    def test_declared_active_count_must_match_entries(self) -> None:
        candidate = copy.deepcopy(self.registry)
        candidate["activeThemeCount"] = 2
        with self.assertRaises(ValueError):
            validate_registry_contract(candidate)

    def test_active_ordinals_must_be_contiguous(self) -> None:
        candidate = copy.deepcopy(self.registry)
        candidate["themes"][52]["activeOrdinal"] = 2
        with self.assertRaises(ValueError):
            validate_registry_contract(candidate)

    def test_legacy_names_cannot_become_active(self) -> None:
        candidate = copy.deepcopy(self.registry)
        candidate["themes"][1]["active"] = True
        candidate["themes"][1]["activeOrdinal"] = 2
        candidate["activeThemeCount"] = 2
        with self.assertRaises(ValueError):
            validate_registry_contract(candidate)

    def test_inactive_entry_cannot_keep_an_active_ordinal(self) -> None:
        candidate = copy.deepcopy(self.registry)
        candidate["themes"][1]["activeOrdinal"] = 2
        with self.assertRaises(ValueError):
            validate_registry_contract(candidate)

    def test_default_must_be_neon_stage_skin_1(self) -> None:
        candidate = copy.deepcopy(self.registry)
        candidate["defaultSkin"] = "skin-052"
        with self.assertRaises(ValueError):
            validate_registry_contract(candidate)

    def test_default_record_is_the_canonical_neon_stage_package(self) -> None:
        default_entry = next(entry for entry in self.registry["themes"] if entry["id"] == "skin-001")
        self.assertEqual(default_entry["index"], 1)
        self.assertEqual(default_entry["name"], "NEON STAGE DEFAULT")
        self.assertEqual(default_entry["path"], "Skin_001/theme.json")
        self.assertIs(default_entry["active"], True)
        self.assertEqual(default_entry["activeOrdinal"], 1)

    def test_default_record_cannot_be_renamed_relocated_or_deactivated(self) -> None:
        mutations = (
            ("name", "Other Theme"),
            ("path", "Skin_053/theme.json"),
            ("active", False),
            ("activeOrdinal", 2),
            ("index", 2),
        )
        for key, value in mutations:
            with self.subTest(key=key):
                candidate = copy.deepcopy(self.registry)
                default_entry = next(entry for entry in candidate["themes"] if entry["id"] == "skin-001")
                default_entry[key] = value
                with self.assertRaises(ValueError):
                    validate_registry_contract(candidate)

    def test_release_target_is_exactly_sixteen(self) -> None:
        candidate = copy.deepcopy(self.registry)
        candidate["releaseTargetActiveThemeCount"] = 15
        with self.assertRaises(ValueError):
            validate_registry_contract(candidate)


if __name__ == "__main__":
    unittest.main(verbosity=2)
