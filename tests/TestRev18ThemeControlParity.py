#!/usr/bin/env python3
"""REV18 contract: every approved theme maps exactly the 27 canonical controls."""
from __future__ import annotations

import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SKIN = ROOT / "SKIN"


class Rev18ThemeControlParityTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.sequence = json.loads((SKIN / "approved_theme_sequence.json").read_text(encoding="utf-8"))
        cls.schema = json.loads((SKIN / "control_schema.json").read_text(encoding="utf-8"))
        cls.required = set(cls.schema["x-ppgu-contract"]["requiredControlIds"])

    def test_canonical_schema_has_exactly_27_controls(self) -> None:
        self.assertEqual(27, len(self.required))

    def test_every_approved_theme_matches_the_canonical_control_set(self) -> None:
        self.assertEqual(6, len(self.sequence["themes"]))
        for item in self.sequence["themes"]:
            with self.subTest(theme=item["label"], stable_id=item["stableId"]):
                path = SKIN / item["path"]
                theme = json.loads(path.read_text(encoding="utf-8"))
                controls = set(theme.get("controls", {}))
                self.assertEqual(self.required, controls)
                self.assertNotIn("sync_selector", controls)
                self.assertEqual(21, len(theme.get("palette", {})))

    def test_runtime_validator_uses_the_same_exact_27_control_contract(self) -> None:
        source = (ROOT / "Source" / "UI" / "ThemeManager.cpp").read_text(encoding="utf-8")
        start = source.index("bool validateSkinStudioManifest(")
        end = source.index("\n}\n\nbool PPGUThemeManager::readColour", start)
        validator = source[start:end]
        self.assertIn("!= requiredControlIds.size()", validator)
        self.assertNotIn("sync_selector", validator)

        match = re.search(
            r"const juce::StringArray requiredControlIds\\s*\\{(?P<body>.*?)\\};",
            validator,
            re.DOTALL,
        )
        self.assertIsNotNone(match, "runtime canonical control ID list is missing")
        runtime_ids = re.findall(r'"([^"]+)"', match.group("body"))
        self.assertEqual(27, len(runtime_ids))
        self.assertEqual(27, len(set(runtime_ids)))
        self.assertEqual(self.required, set(runtime_ids))

    def test_skin_one_remains_the_canonical_default(self) -> None:
        first = self.sequence["themes"][0]
        self.assertEqual("SKIN-1", first["label"])
        self.assertEqual("NEON STAGE DEFAULT", first["name"])
        self.assertEqual("skin-053", first["stableId"])
        self.assertEqual(53, first["index"])


if __name__ == "__main__":
    unittest.main(verbosity=2)
