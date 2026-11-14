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


    def test_split_uses_bounded_memory_chunks(self):
        source = MODULE_PATH.read_text(encoding="utf-8")
        self.assertIn("COPY_CHUNK_BYTES = 8 * 1024 * 1024", source)
        self.assertIn("src.read(min(COPY_CHUNK_BYTES, part_size - part_bytes))", source)
        self.assertNotIn("chunk = src.read(part_size)", source)

    def test_verification_rejects_manifest_path_traversal(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            manifest = root / "manifest.json"
            manifest.write_text(
                json.dumps({
                    "schema": "ppgu.split-artifact.v1",
                    "part_count": 1,
                    "parts": [{"index": 1, "name": "../outside.part-0001", "size_bytes": 1, "sha256": "0" * 64}],
                    "total_size_bytes": 1,
                    "archive_sha256": "0" * 64,
                }),
                encoding="utf-8",
            )
            with self.assertRaises(ValueError):
                splitter.verify_manifest(manifest, root)


if __name__ == "__main__":
    unittest.main()
