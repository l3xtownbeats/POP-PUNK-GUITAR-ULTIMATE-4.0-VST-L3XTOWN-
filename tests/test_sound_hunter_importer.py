#!/usr/bin/env python3
"""Focused regression tests for Sound Hunter import path and aggregate 40 GB library ceiling."""
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

    def test_aggregate_library_ceiling_is_40_gb(self):
        limit = 40_000_000_000
        self.assertTrue(sound_hunter.within_library_limit(0, 39_000_000_000, limit))
        self.assertTrue(sound_hunter.within_library_limit(39_000_000_000, 1_000_000_000, limit))
        self.assertFalse(sound_hunter.within_library_limit(39_000_000_000, 2_000_000_000, limit))
        self.assertFalse(sound_hunter.within_library_limit(40_000_000_000, 1, limit))

    def test_release_checksum_manifest_verifies_all_members(self):
        import hashlib, io, zipfile
        payload=b"real guitar sample"
        digest=hashlib.sha256(payload).hexdigest()
        buffer=io.BytesIO()
        with zipfile.ZipFile(buffer,"w") as z:
            z.writestr("bank/sample.wav",payload)
            z.writestr("bank/SHA256SUMS",f"{digest}  sample.wav\\n")
        with zipfile.ZipFile(io.BytesIO(buffer.getvalue())) as z:
            count,members=sound_hunter.verify_zip_sha256_manifest(z)
        self.assertEqual(count,1)
        self.assertEqual(members,["bank/sample.wav"])

    def test_release_checksum_manifest_rejects_modified_member(self):
        import hashlib, io, zipfile
        buffer=io.BytesIO()
        with zipfile.ZipFile(buffer,"w") as z:
            z.writestr("sample.wav",b"changed")
            z.writestr("SHA256SUMS",f"{hashlib.sha256(b'original').hexdigest()}  sample.wav\\n")
        with zipfile.ZipFile(io.BytesIO(buffer.getvalue())) as z:
            with self.assertRaises(RuntimeError):
                sound_hunter.verify_zip_sha256_manifest(z)

    def test_existing_audio_bytes_counts_audio_only(self):
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "guitar.wav").write_bytes(b"x" * 123)
            (root / "notes.txt").write_text("not audio")
            nested = root / "nested"
            nested.mkdir()
            (nested / "bank.sfz").write_bytes(b"x" * 17)
            self.assertEqual(sound_hunter.existing_audio_bytes(root), 123)


if __name__ == "__main__":
    unittest.main()
