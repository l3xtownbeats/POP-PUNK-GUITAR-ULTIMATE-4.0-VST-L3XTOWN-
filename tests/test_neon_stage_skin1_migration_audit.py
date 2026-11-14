import json
import tempfile
import unittest
from pathlib import Path

from audit_neon_stage_skin1_migration import audit


def write_manifest(root: Path, rel: str, data: dict) -> None:
    path = root / rel
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data), encoding="utf-8")


class NeonStageMigrationAuditTests(unittest.TestCase):
    def make_registry(self, root: Path) -> None:
        entries = []
        for index in range(1, 54):
            skin_id = f"skin-{index:02d}" if index < 10 else f"skin-{index:02d}"
            name = "NEON STAGE" if index == 53 else f"Legacy {index}"
            rel = f"Skin_{index:02d}/theme.json" if index < 10 else f"Skin_{index:03d}/theme.json"
            if index == 53:
                skin_id = "skin-053"
                rel = "Skin_053/theme.json"
            entries.append({"id": skin_id, "index": index, "name": name, "path": rel})
            identity = {"id": skin_id, "index": index, "name": name}
            write_manifest(root, rel, identity)
        for index in range(54, 103):
            skin_id = f"skin-{index:03d}"
            rel = f"Skin_{index:03d}/theme.json"
            entries.append({"id": skin_id, "index": index, "name": f"Theme {index}", "path": rel})
            write_manifest(root, rel, {"id": skin_id, "index": index, "name": f"Theme {index}"})
        write_manifest(root, "Skin_052/theme.json",
                       {"id": "legacy-skin-052", "index": 52, "name": "Carbon Relay"})
        (root / "theme_manifest.json").write_text(json.dumps({
            "totalSkins": len(entries), "defaultSkin": "skin-053", "themes": entries
        }), encoding="utf-8")

    def test_valid_migrated_registry_passes_without_mutation(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.make_registry(root)
            result = audit(root)
            self.assertEqual("PASS", result["status"])
            self.assertFalse(result["mutation_performed"])
            self.assertEqual(49, len(result["preservation"]["stable_ids_54_102"]))

    def test_current_wrong_default_and_skin053_archive_are_blocked(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.make_registry(root)
            registry = json.loads((root / "theme_manifest.json").read_text())
            registry["defaultSkin"] = "neon-stage-default-skin-1"
            registry["themes"][52]["id"] = "skin-053"
            registry["themes"][52]["name"] = "NEON STAGE — LEGACY ARCHIVE"
            (root / "theme_manifest.json").write_text(json.dumps(registry))
            write_manifest(root, "Skin_053/theme.json", {
                "id": "skin-053", "index": 53, "name": "NEON STAGE — LEGACY ARCHIVE"
            })
            result = audit(root)
            codes = {item["code"] for item in result["issues"]}
            self.assertEqual("BLOCKED", result["status"])
            self.assertIn("WRONG_DEFAULT_ID", codes)
            self.assertIn("CANONICAL_ENTRY_MISMATCH", codes)
            self.assertIn("CANONICAL_FILE_IDENTITY_MISMATCH", codes)

    def test_duplicate_ids_and_indices_are_blocked(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.make_registry(root)
            registry = json.loads((root / "theme_manifest.json").read_text())
            registry["themes"][53]["id"] = registry["themes"][52]["id"]
            registry["themes"][53]["index"] = 53
            (root / "theme_manifest.json").write_text(json.dumps(registry))
            result = audit(root)
            codes = {item["code"] for item in result["issues"]}
            self.assertIn("DUPLICATE_ID", codes)
            self.assertIn("DUPLICATE_INDEX", codes)

    def test_path_traversal_is_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.make_registry(root)
            registry = json.loads((root / "theme_manifest.json").read_text())
            registry["themes"][52]["path"] = "../outside/theme.json"
            (root / "theme_manifest.json").write_text(json.dumps(registry))
            result = audit(root)
            self.assertIn("UNSAFE_MANIFEST_PATH", {item["code"] for item in result["issues"]})


if __name__ == "__main__":
    unittest.main()
