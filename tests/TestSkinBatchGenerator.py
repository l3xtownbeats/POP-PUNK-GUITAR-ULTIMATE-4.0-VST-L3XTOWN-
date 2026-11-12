#!/usr/bin/env python3
"""Release-active unit tests for the deterministic PPGU 50-skin generator."""
from __future__ import annotations

import importlib.util
import tempfile
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Cannot import {path}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


generator = load_module("ppgu_skin_batch_generator", ROOT / "tools/generate_skin_batch.py")
validator = load_module("ppgu_skin_pack_validator", ROOT / "tools/validate_skin_pack.py")
failures = 0


def check(condition: bool, message: str) -> None:
    global failures
    if not condition:
        failures += 1
        print("FAIL:", message)


def main() -> int:
    entries = generator.build_pack_entries(53)
    check(len(entries) == 50, "generator creates exactly 50 entries")
    check(entries[0]["index"] == 53 and entries[-1]["index"] == 102,
          "generated range is continuous from the registry's next index")
    check(len({entry["id"] for entry in entries}) == 50, "all generated IDs are unique")
    check(len({entry["index"] for entry in entries}) == 50, "all generated indices are unique")
    check(len({entry["layout"] for entry in entries}) == 4, "all four layout variants are used")
    family_counts = Counter(entry["family"] for entry in entries)
    check(family_counts == {
        "carbon_studio": 8, "neon_basement": 8, "chrome_parade": 8,
        "wood_summer": 8, "vintage_tape": 8, "sunset_suburb": 8,
        "special": 2,
    }, "batch contains six families x eight plus two special themes")
    check(entries[-2]["name"] == "Hazard Line" and entries[-1]["name"] == "Whiteout Bleach",
          "special theme identities are stable")

    for family in generator.FAMILIES + ("special",):
        for variant in range(4):
            palette = generator.make_palette(family, variant)
            check(len(palette) == 21, f"{family}/{variant} contains exactly 21 tokens")
            check(validator.contrast(palette["text_primary"], palette["window_bg"]) >= 4.0,
                  f"{family}/{variant} primary text contrast")
            check(validator.contrast(palette["text_secondary"], palette["window_bg"]) >= 4.0,
                  f"{family}/{variant} secondary text contrast")
            check(validator.contrast(palette["text_disabled"], palette["window_bg"]) >= 4.0,
                  f"{family}/{variant} disabled text contrast")
            check(validator.contrast(palette["lcd_text"], palette["lcd_bg"]) >= 4.0,
                  f"{family}/{variant} LCD contrast")
            check(validator.contrast(palette["accent"], palette["panel"]) >= 2.2,
                  f"{family}/{variant} accent contrast")

    with tempfile.TemporaryDirectory(prefix="ppgu-skin-generator-test-") as temp:
        root = Path(temp)
        palette = generator.make_palette("neon_basement", 1)
        preview = root / "preview.png"
        hero = root / "hero.png"
        generator.write_png(preview, 360, 240, palette, 53)
        generator.write_png(hero, 800, 450, palette, 70)
        check(validator.png_dimensions(preview) == (360, 240), "preview PNG structure and dimensions")
        check(validator.png_dimensions(hero) == (800, 450), "hero PNG structure and dimensions")

        svg = root / "texture.svg"
        generator.write_svg(svg, palette["accent"], palette["accent_glow"], 53, "texture")
        validator.safe_svg(svg)
        check(True, "generated SVG passes safety/XML validation")

        corrupt = root / "corrupt.png"
        data = bytearray(preview.read_bytes())
        data[20] ^= 0x01
        corrupt.write_bytes(data)
        try:
            validator.png_dimensions(corrupt)
            check(False, "corrupt PNG CRC must be rejected")
        except ValueError:
            check(True, "corrupt PNG CRC is rejected")

        unsafe = root / "unsafe.svg"
        unsafe.write_text('<svg xmlns="http://www.w3.org/2000/svg"><script/></svg>', encoding="utf-8")
        try:
            validator.safe_svg(unsafe)
            check(False, "active SVG script must be rejected")
        except ValueError:
            check(True, "active SVG script is rejected")

    if failures:
        print(f"{failures} generator contract check(s) failed.")
        return 1
    print("Skin batch generator contract tests passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
