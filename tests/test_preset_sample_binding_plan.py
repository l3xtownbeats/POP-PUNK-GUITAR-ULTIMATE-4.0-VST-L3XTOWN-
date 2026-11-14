from __future__ import annotations

import json
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

class PresetSampleBindingPlanTests(unittest.TestCase):
    def test_every_legacy_factory_preset_has_traceable_candidate_sources(self):
        catalog = json.loads((ROOT / "presets/factory_bank_300.json").read_text(encoding="utf-8"))
        plan = json.loads((ROOT / "presets/rev18_preset_sample_binding_plan.json").read_text(encoding="utf-8"))
        known_sources = {source["id"] for source in plan["sourceDefinitions"]}
        expected = {
            preset["id"]: mode
            for mode, presets in (("MELODY", catalog["melodyPresets"]), ("PATTERN", catalog["patternPresets"]))
            for preset in presets
        }
        actual = {entry["presetId"]: entry["mode"] for entry in plan["entries"]}
        self.assertEqual(actual, expected)
        self.assertEqual(len(actual), len(plan["entries"]))
        for entry in plan["entries"]:
            self.assertTrue(entry["sourceCandidates"], entry["presetId"])
            self.assertTrue(set(entry["sourceCandidates"]).issubset(known_sources))
            self.assertFalse(entry["ready"], "Candidate links must not be represented as READY before runtime/audio QA.")
            self.assertIn("pending", entry["bindingStatus"])

    def test_pattern_entries_keep_performance_articulation_gate(self):
        plan = json.loads((ROOT / "presets/rev18_preset_sample_binding_plan.json").read_text(encoding="utf-8"))
        pattern = [entry for entry in plan["entries"] if entry["mode"] == "PATTERN"]
        self.assertEqual(len(pattern), 150)
        for entry in pattern:
            self.assertTrue(any("strum" in blocker.lower() or "articulation" in blocker.lower()
                                for blocker in entry["readinessBlockers"]))

if __name__ == "__main__":
    unittest.main()
