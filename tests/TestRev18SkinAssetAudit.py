#!/usr/bin/env python3
"""Focused unit tests for the REV.18 skin PNG and contrast audit primitives."""
from __future__ import annotations

import importlib.util
import struct
import tempfile
import unittest
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MODULE_PATH = ROOT / "tools" / "audit_skin_rev18_assets.py"
SPEC = importlib.util.spec_from_file_location("audit_skin_rev18_assets", MODULE_PATH)
AUDIT = importlib.util.module_from_spec(SPEC)
assert SPEC and SPEC.loader
SPEC.loader.exec_module(AUDIT)


def chunk(kind: bytes, payload: bytes) -> bytes:
    return (struct.pack(">I", len(payload)) + kind + payload
            + struct.pack(">I", zlib.crc32(kind + payload) & 0xFFFFFFFF))


def rgba_png(width: int, height: int, pixel: tuple[int, int, int, int] = (0, 0, 0, 0)) -> bytes:
    raw = b"".join(b"\x00" + bytes(pixel) * width for _ in range(height))
    def chunk(kind: bytes, payload: bytes) -> bytes:
        body = kind + payload
        return struct.pack(">I", len(payload)) + body + struct.pack(">I", zlib.crc32(body) & 0xffffffff)
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    return b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", ihdr) + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b"")


def tiny_png() -> bytes:
    # 2x1, 8-bit RGBA, filter byte + two opaque white pixels.
    header = struct.pack(">IIBBBBB", 2, 1, 8, 6, 0, 0, 0)
    pixels = b"\x00" + b"\xff\xff\xff\xff" * 2
    return (b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", header)
            + chunk(b"IDAT", zlib.compress(pixels)) + chunk(b"IEND", b""))


class Rev18PngAuditTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.path = Path(self.temp.name) / "sample.png"

    def tearDown(self) -> None:
        self.temp.cleanup()

    def test_valid_png_returns_dimensions_after_crc_and_decode(self) -> None:
        self.path.write_bytes(tiny_png())
        self.assertEqual(AUDIT.png_info(self.path), (2, 1))

    def test_invalid_signature_is_rejected(self) -> None:
        self.path.write_bytes(b"not a png" + b"\x00" * 40)
        with self.assertRaisesRegex(ValueError, "invalid PNG signature"):
            AUDIT.png_info(self.path)

    def test_chunk_crc_corruption_is_rejected(self) -> None:
        data = bytearray(tiny_png())
        data[29] ^= 0x01
        self.path.write_bytes(data)
        with self.assertRaisesRegex(ValueError, "CRC mismatch"):
            AUDIT.png_info(self.path)

    def test_truncated_chunk_is_rejected(self) -> None:
        self.path.write_bytes(tiny_png()[:-5])
        with self.assertRaisesRegex(ValueError, "truncated chunk|incomplete PNG"):
            AUDIT.png_info(self.path)


class Rev18SvgSafetyTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.path = Path(self.temp.name) / "icon.svg"

    def tearDown(self) -> None:
        self.temp.cleanup()

    def test_valid_static_svg_is_accepted(self) -> None:
        self.path.write_text('<svg xmlns="http://www.w3.org/2000/svg"><path d="M0 0h10"/></svg>', encoding="utf-8")
        AUDIT.validate_svg(self.path)

    def test_script_element_is_rejected(self) -> None:
        self.path.write_text('<svg xmlns="http://www.w3.org/2000/svg"><script>alert(1)</script></svg>', encoding="utf-8")
        with self.assertRaisesRegex(ValueError, "forbidden SVG element"):
            AUDIT.validate_svg(self.path)

    def test_external_href_is_rejected(self) -> None:
        self.path.write_text('<svg xmlns="http://www.w3.org/2000/svg"><image href="https://example.invalid/a.png"/></svg>', encoding="utf-8")
        with self.assertRaisesRegex(ValueError, "external href/src"):
            AUDIT.validate_svg(self.path)

    def test_event_handler_is_rejected(self) -> None:
        self.path.write_text('<svg xmlns="http://www.w3.org/2000/svg"><path onclick="alert(1)"/></svg>', encoding="utf-8")
        with self.assertRaisesRegex(ValueError, "event handler"):
            AUDIT.validate_svg(self.path)

    def test_doctype_is_rejected(self) -> None:
        self.path.write_text('<!DOCTYPE svg><svg xmlns="http://www.w3.org/2000/svg"/>', encoding="utf-8")
        with self.assertRaisesRegex(ValueError, "DOCTYPE/ENTITY"):
            AUDIT.validate_svg(self.path)


class Rev18IconManifestTests(unittest.TestCase):
    def test_manifest_checksums_and_paths_are_verified(self) -> None:
        import hashlib, json
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / "assets/master/icons").mkdir(parents=True)
            (root / "assets/icons").mkdir(parents=True)
            svg = b'<svg xmlns="http://www.w3.org/2000/svg"><path d="M0 0h10"/></svg>'
            (root / "assets/master/icons/skins.svg").write_bytes(svg)
            image_path = root / "assets/icons/skins.png"
            image_path.write_bytes(rgba_png(256, 256))
            records = []
            for suffix, rel in (("master_svg", "assets/master/icons/skins.svg"),
                                ("runtime_png", "assets/icons/skins.png")):
                data = (root / rel).read_bytes()
                records.append({"id": f"topbar.skins.{suffix}", "path": rel,
                    "bytes": len(data), "sha256": hashlib.sha256(data).hexdigest(),
                    "crc32": f"{zlib.crc32(data) & 0xffffffff:08X}", "origin": "[DECISION] test"})
            (root / "assets/asset_manifest.json").write_text(
                json.dumps({"skinId": "skin-053", "assetGroup": "topbar-icons", "records": records}),
                encoding="utf-8")
            AUDIT.validate_icon_manifest(root, ("skins",))
            image_path.write_bytes(b"tampered")
            with self.assertRaisesRegex(ValueError, "byte count mismatch|SHA-256 mismatch"):
                AUDIT.validate_icon_manifest(root, ("skins",))


class Rev18ArtworkManifestTests(unittest.TestCase):
    def test_artwork_manifest_validates_all_registered_checksums(self) -> None:
        import hashlib, json
        expected = {
            "skin1.hero_master": "assets/master/hero/hero.png",
            "skin1.hero_runtime": "assets/hero/hero.png",
            "skin1.preview_master": "assets/master/preview/preview.png",
            "skin1.preview_runtime": "assets/preview/preview.png",
            "skin1.background_master": "assets/master/background/main.png",
            "skin1.background_runtime": "assets/background/main.png",
            "skin1.texture_master": "assets/master/textures/carbon_weave.png",
            "skin1.texture_runtime": "assets/textures/carbon_weave.png",
            "skin1.glow_master": "assets/master/fx_layers/stage_glow.png",
            "skin1.glow_runtime": "assets/fx_layers/stage_glow.png",
            "skin1.edge_master": "assets/master/fx_layers/edge_highlight.png",
            "skin1.edge_runtime": "assets/fx_layers/edge_highlight.png",
            "skin1.atlas_png": "atlas/atlas.png",
            "skin1.atlas_json": "atlas/atlas.json",
        }
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            records = []
            for asset_id, rel in expected.items():
                path = root / rel
                path.parent.mkdir(parents=True, exist_ok=True)
                raw = (json.dumps({"frames": {str(i): {} for i in range(6)}}).encode()
                       if rel.endswith(".json") else tiny_png())
                path.write_bytes(raw)
                record = {"id": asset_id, "path": rel, "bytes": len(raw),
                    "sha256": hashlib.sha256(raw).hexdigest(),
                    "crc32": f"{zlib.crc32(raw) & 0xffffffff:08X}",
                    "origin": "[DECISION] test"}
                if rel.endswith(".png"):
                    record["dimensions"] = "2x1"
                records.append(record)
            manifest = {"skinId": "skin-053",
                "assetGroup": "hero-preview-background-texture-fx-atlas",
                "records": records}
            (root / "assets/artwork_manifest.json").write_text(
                json.dumps(manifest), encoding="utf-8")
            AUDIT.validate_artwork_manifest(root)
            (root / expected["skin1.hero_master"]).write_bytes(b"tampered")
            with self.assertRaisesRegex(ValueError, "byte count mismatch|SHA-256 mismatch"):
                AUDIT.validate_artwork_manifest(root)


class Rev18ControlManifestTests(unittest.TestCase):
    def test_control_manifest_checks_master_runtime_and_reserved_visibility(self) -> None:
        import hashlib, json
        with tempfile.TemporaryDirectory() as temp:
            theme_dir = Path(temp)
            records = []
            for tier, relbase in (("master", "assets/master/controls"),
                                  ("runtime", "assets/controls")):
                for state in AUDIT.STATES:
                    rel = f"{relbase}/knob01_{state}.png"
                    path = theme_dir / rel
                    path.parent.mkdir(parents=True, exist_ok=True)
                    path.write_bytes(rgba_png(512, 512))
                    raw = path.read_bytes()
                    records.append({"id": f"knob01.{state}.{tier}", "path": rel,
                        "bytes": len(raw), "sha256": hashlib.sha256(raw).hexdigest(),
                        "crc32": f"{zlib.crc32(raw) & 0xffffffff:08X}",
                        "dimensions": "512x512", "origin": "[DECISION] test",
                        "renderMode": "stateful"})
            manifest = {"skinId": "skin-053", "assetGroup": "control-state-sprites",
                "states": list(AUDIT.STATES), "controlIds": ["knob01"], "records": records}
            (theme_dir / "assets/control_manifest.json").write_text(
                json.dumps(manifest), encoding="utf-8")
            AUDIT.validate_control_manifest(theme_dir, ("knob01",))
            (theme_dir / "assets/controls/knob01_default.png").write_bytes(b"tampered")
            with self.assertRaisesRegex(ValueError, "byte count mismatch|SHA-256 mismatch"):
                AUDIT.validate_control_manifest(theme_dir, ("knob01",))


class Rev18FontManifestTests(unittest.TestCase):
    def test_font_manifest_requires_valid_crc_and_ofl_license(self) -> None:
        import json
        with tempfile.TemporaryDirectory() as temp:
            theme_dir = Path(temp)
            root = theme_dir / "fonts"
            root.mkdir()
            font = b"font-bytes"
            (root / "Inter[opsz,wght].ttf").write_bytes(font)
            (root / "OFL.txt").write_text(
                "SIL OPEN FONT LICENSE Version 1.1\nThis Font Software is licensed under OFL.",
                encoding="utf-8")
            manifest = {
                "skinId": "skin-053", "fontFamily": "Inter",
                "fontFile": "Inter[opsz,wght].ttf", "licenseFile": "OFL.txt",
                "bytes": len(font), "crc32": f"{zlib.crc32(font)&0xffffffff:08X}",
                "sourceBlobSha1": "a" * 40, "licenseBlobSha1": "b" * 40,
            }
            (root / "font_manifest.json").write_text(json.dumps(manifest), encoding="utf-8")
            AUDIT.validate_font_manifest(theme_dir)
            (root / "Inter[opsz,wght].ttf").write_bytes(b"changed")
            with self.assertRaisesRegex(ValueError, "byte count mismatch|CRC32 mismatch"):
                AUDIT.validate_font_manifest(theme_dir)


class Rev18ContrastAuditTests(unittest.TestCase):
    def test_primary_text_contrast_exceeds_rev18_minimum(self) -> None:
        self.assertGreaterEqual(AUDIT.contrast("#FFFFFF", "#202020"), 7.0)

    def test_invalid_palette_hex_is_rejected(self) -> None:
        with self.assertRaisesRegex(ValueError, "invalid HEX token"):
            AUDIT.luminance("white")


if __name__ == "__main__":
    unittest.main(verbosity=2)
