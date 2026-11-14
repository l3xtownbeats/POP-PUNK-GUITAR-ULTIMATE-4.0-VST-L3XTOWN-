#!/usr/bin/env python3
"""Regression tests for Sound Hunter path safety and uncapped additive imports."""
from pathlib import Path
import importlib.util
import unittest

ROOT = Path(__file__).resolve().parents[1]
MODULE_PATH = ROOT / "SoundHunter" / "sound_hunter_real_guitars.py"
SPEC = importlib.util.spec_from_file_location("ppgu_sound_hunter", MODULE_PATH)
sound_hunter = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(sound_hunter)


class SoundHunterRegressionTests(unittest.TestCase):
    def test_release_assets_use_separate_directory(self):
        source = Path("/tmp/run/meta-open-sound-steel")
        target = sound_hunter.release_asset_target(
            source, "meta-open-sound-steel/release/steel/sample.wav"
        )
        self.assertEqual(target, source / "release_assets" / "release" / "steel" / "sample.wav")
        self.assertNotEqual(target, source / "release" / "steel" / "sample.wav")

    def test_release_asset_path_rejects_traversal(self):
        with self.assertRaises(ValueError):
            sound_hunter.release_asset_target(Path("/tmp/run/source"), "../outside/sample.wav")

    def test_no_artificial_library_size_cap(self):
        self.assertFalse(hasattr(sound_hunter, "within_run_limit"))
        self.assertNotIn("--max-gb", sound_hunter.main.__code__.co_consts)


if __name__ == "__main__":
    unittest.main()
