#!/usr/bin/env python3
"""Focused regression tests for canonical sample inventory SFZ path matching."""
import importlib.util
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "audit_sample_library", ROOT / "tools" / "audit_sample_library.py"
)
AUDIT = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(AUDIT)


class SfzSuffixKeyTests(unittest.TestCase):
    def test_parent_relative_samples_reference_matches_library_suffix(self):
        self.assertEqual(
            AUDIT.suffix_key("../Samples/black/btb/btb_gb4_rr1.wav"),
            "samples/black/btb/btb_gb4_rr1.wav",
        )

    def test_nested_parent_components_are_normalized(self):
        self.assertEqual(
            AUDIT.suffix_key("Samples/black/../black/btb/note.wav"),
            "samples/black/btb/note.wav",
        )

    def test_plain_relative_reference_is_preserved(self):
        self.assertEqual(
            AUDIT.suffix_key("guitar/note.wav"),
            "guitar/note.wav",
        )


class SfzBuiltinSampleTokenTests(unittest.TestCase):
    def test_known_builtin_oscillators_are_explicitly_recognized(self):
        for token in ("*sine", "*triangle", "*saw", "*square", "*noise", "*silence"):
            with self.subTest(token=token):
                self.assertIn(token, AUDIT.BUILTIN_SAMPLE_TOKENS)

    def test_builtin_token_matching_is_case_insensitive(self):
        self.assertIn("*SINE".casefold(), AUDIT.BUILTIN_SAMPLE_TOKENS)

    def test_unknown_builtin_like_token_is_not_silently_ignored(self):
        self.assertNotIn("*procedural_guitar", AUDIT.BUILTIN_SAMPLE_TOKENS)


if __name__ == "__main__":
    unittest.main()
