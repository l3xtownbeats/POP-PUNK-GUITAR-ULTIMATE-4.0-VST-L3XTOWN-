#!/usr/bin/env python3
"""Unit tests for the REV ULTRA 1.3 skin pack registrar."""
from __future__ import annotations

import importlib.util
from pathlib import Path
import unittest

MODULE_PATH = Path(__file__).resolve().parents[1] / "tools" / "register_skin_pack.py"
SPEC = importlib.util.spec_from_file_location("register_skin_pack", MODULE_PATH)
registrar = importlib.util.module_from_spec(SPEC)
assert SPEC and SPEC.loader
SPEC.loader.exec_module(registrar)


def registry(total=2):
    return {
        "totalSkins": total,
        "defaultSkin": "skin-001",
        "themes": [
            {"id": f"skin-{i:03d}", "index": i, "path": f"Skin_{i:03d}/theme.json"}
            for i in range(1, total + 1)
        ],
    }


def pack(start=3, count=2):
    return {
        "status": "GENERATED_NOT_MERGED",
        "startIndex": start,
        "endIndex": start + count - 1,
        "totalSkins": count,
        "themes": [
            {"id": f"skin-{i:03d}", "index": i, "path": f"Skin_{i:03d}/theme.json"}
            for i in range(start, start + count)
        ],
    }


class RegistryTests(unittest.TestCase):
    def test_accepts_canonical_registry_and_keeps_default(self):
        data = registry()
        entries = registrar.validate_registry(data)
        self.assertEqual(len(entries), 2)
        self.assertEqual(data["defaultSkin"], "skin-001")

    def test_rejects_count_mismatch(self):
        data = registry()
        data["totalSkins"] = 3
        with self.assertRaisesRegex(ValueError, "does not match"):
            registrar.validate_registry(data)

    def test_rejects_duplicate_index(self):
        data = registry()
        data["themes"][1]["index"] = 1
        with self.assertRaisesRegex(ValueError, "duplicate"):
            registrar.validate_registry(data)

    def test_rejects_gap_in_registry(self):
        data = registry()
        data["themes"][1]["index"] = 3
        with self.assertRaisesRegex(ValueError, "continuous"):
            registrar.validate_registry(data)

    def test_rejects_missing_default(self):
        data = registry()
        data["defaultSkin"] = "skin-999"
        with self.assertRaisesRegex(ValueError, "defaultSkin"):
            registrar.validate_registry(data)


class PackTests(unittest.TestCase):
    def test_accepts_next_contiguous_pack(self):
        entries = registrar.validate_pack_manifest(pack(), current_total=2)
        self.assertEqual([x["index"] for x in entries], [3, 4])

    def test_rejects_already_merged_pack(self):
        data = pack()
        data["status"] = "MERGED"
        with self.assertRaisesRegex(ValueError, "GENERATED_NOT_MERGED"):
            registrar.validate_pack_manifest(data, current_total=2)

    def test_rejects_wrong_start_without_renumbering(self):
        with self.assertRaisesRegex(ValueError, "must start at 3"):
            registrar.validate_pack_manifest(pack(start=4), current_total=2)

    def test_rejects_non_contiguous_pack_entries(self):
        data = pack()
        data["themes"][1]["index"] = 9
        with self.assertRaisesRegex(ValueError, "ordered and continuous"):
            registrar.validate_pack_manifest(data, current_total=2)

    def test_rejects_inconsistent_count(self):
        data = pack()
        data["totalSkins"] = 99
        with self.assertRaisesRegex(ValueError, "range/count"):
            registrar.validate_pack_manifest(data, current_total=2)


if __name__ == "__main__":
    unittest.main()
