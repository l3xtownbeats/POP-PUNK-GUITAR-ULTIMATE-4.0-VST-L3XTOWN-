#!/usr/bin/env python3
"""Static interaction contract for the REV18 skin gallery workflow."""
from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "Source" / "PluginEditor.cpp").read_text(encoding="utf-8")
START = SOURCE.index("class PPGUSkinGalleryPanel")
END = SOURCE.index("// UI-only settings panel", START)
GALLERY = SOURCE[START:END]


class SkinGalleryInteractionContractTests(unittest.TestCase):
    def test_card_click_selects_without_applying(self) -> None:
        start = GALLERY.index("void mouseUp(const juce::MouseEvent& event) override")
        end = GALLERY.index("private:", start)
        handler = GALLERY[start:end]
        self.assertIn("select(item.index)", handler)
        self.assertNotIn("apply(item.index)", handler)

    def test_preview_is_a_separate_view(self) -> None:
        self.assertIn('previewButton.onClick', GALLERY)
        self.assertIn("showingPreview = true", GALLERY)
        self.assertIn("previewBounds", GALLERY)
        self.assertIn("juce::ImageCache::getFromFile", GALLERY)

    def test_preview_image_decode_is_outside_paint(self) -> None:
        paint_start = GALLERY.index("void paint(juce::Graphics& g) override")
        paint_end = GALLERY.index("void setThemePalette", paint_start)
        paint_body = GALLERY[paint_start:paint_end]
        self.assertNotIn("ImageCache::getFromFile", paint_body)
        preview_start = GALLERY.index("previewButton.onClick")
        preview_end = GALLERY.index("applyButton.onClick", preview_start)
        preview_handler = GALLERY[preview_start:preview_end]
        self.assertIn("ImageCache::getFromFile", preview_handler)
        self.assertIn("previewImage", paint_body)

    def test_family_filter_is_derived_from_registry(self) -> None:
        constructor_start = GALLERY.index("PPGUSkinGalleryPanel(std::vector<Skin> skinsIn")
        constructor_end = GALLERY.index("~PPGUSkinGalleryPanel", constructor_start)
        constructor = GALLERY[constructor_start:constructor_end]
        self.assertIn("familyFilterKeys.clear()", constructor)
        self.assertIn("for (const auto& skin : skins)", constructor)
        self.assertIn("familyFilterKeys.contains(skin.familyKey)", constructor)
        self.assertNotIn('"CARBON STUDIO", 2', constructor)
        self.assertIn("familyFilterKeys[familyIndex]", GALLERY)

    def test_apply_uses_selected_index_only(self) -> None:
        self.assertIn('applyButton.onClick', GALLERY)
        self.assertIn("apply(selectedIndex)", GALLERY)
        self.assertIn('backButton.setButtonText("BACK")', GALLERY)

    def test_back_and_close_do_not_apply(self) -> None:
        self.assertIn("showingPreview = false", GALLERY)
        self.assertIn("else if (back)", GALLERY)
        self.assertIn('backButton.setButtonText("CLOSE")', GALLERY)


if __name__ == "__main__":
    unittest.main(verbosity=2)
