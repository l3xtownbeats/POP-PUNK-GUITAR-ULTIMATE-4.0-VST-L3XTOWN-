from __future__ import annotations

import importlib.util
import json
from pathlib import Path
import tempfile
import unittest

MODULE_PATH = Path(__file__).resolve().parents[1] / "tools" / "split_artifact.py"
SPEC = importlib.util.spec_from_file_location("ppgu_split_artifact", MODULE_PATH)
assert SPEC and SPEC.loader
splitter = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(splitter)


class SplitArtifactTests(unittest.TestCase):
    def test_split_writes_ordered_parts_and_verifiable_manifest(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            source = root / "complete.zip"
            source.write_bytes(b"ABCDEFGHIJ")
            output = root / "parts"

            manifest_path = splitter.split_archive(source, output, "PPGU_TEST", 4)
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

            self.assertEqual(manifest["part_count"], 3)
            self.assertEqual(
                [part["name"] for part in manifest["parts"]],
                ["PPGU_TEST.part-0001", "PPGU_TEST.part-0002", "PPGU_TEST.part-0003"],
            )
            self.assertEqual([part["size_bytes"] for part in manifest["parts"]], [4, 4, 2])
            splitter.verify_manifest(manifest_path, output)

    def test_verification_rejects_corrupted_part(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            source = root / "complete.zip"
            source.write_bytes(b"sample archive payload")
            output = root / "parts"
            manifest_path = splitter.split_archive(source, output, "PPGU_TEST", 5)
            first_part = output / "PPGU_TEST.part-0001"
            first_part.write_bytes(b"XXXXX")
            with self.assertRaises(ValueError):
                splitter.verify_manifest(manifest_path, output)

    def test_empty_archive_is_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            source = root / "empty.zip"
            source.write_bytes(b"")
            with self.assertRaises(ValueError):
                splitter.split_archive(source, root / "parts", "PPGU_EMPTY", 4)


if __name__ == "__main__":
    unittest.main()
