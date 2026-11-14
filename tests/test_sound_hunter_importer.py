#!/usr/bin/env python3
"""Focused regression tests for Sound Hunter import path and per-run size cap."""
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
        self.assertEqual(
            target,
            source / "release_assets" / "release" / "steel" / "sample.wav",
        )
        self.assertNotEqual(target, source / "release" / "steel" / "sample.wav")

    def test_release_asset_path_rejects_traversal(self):
        with self.assertRaises(ValueError):
            sound_hunter.release_asset_target(
                Path("/tmp/run/source"), "../outside/sample.wav"
            )

    def test_aggregate_library_ceiling_is_40_gib(self):
        limit = 40 * 1024**3
        self.assertTrue(sound_hunter.within_library_limit(0, 39 * 1024**3, limit))
        self.assertTrue(sound_hunter.within_library_limit(39 * 1024**3, 1 * 1024**3, limit))
        self.assertFalse(sound_hunter.within_library_limit(39 * 1024**3, 2 * 1024**3, limit))
        self.assertFalse(sound_hunter.within_library_limit(40 * 1024**3, 1, limit))

    def test_existing_audio_bytes_counts_audio_only(self):
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "guitar.wav").write_bytes(b"x" * 123)
            (root / "notes.txt").write_text("not audio")
            nested = root / "nested"
            nested.mkdir()
            (nested / "bank.sfz").write_bytes(b"x" * 17)
            self.assertEqual(sound_hunter.existing_audio_bytes(root), 140)


if __name__ == "__main__":
    unittest.main()
