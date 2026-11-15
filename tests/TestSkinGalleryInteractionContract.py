#!/usr/bin/env python3
"""Static interaction contract for the REV18 skin gallery workflow."""
from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GALLERY = (ROOT / "Source" / "UI" / "PPGUSkinGalleryPanel.h").read_text(encoding="utf-8")


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
