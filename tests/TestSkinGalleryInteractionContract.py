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
OPEN_START = SOURCE.index("void PopPunkGuitarUltimateAudioProcessorEditor::openSkinGalleryPanel()")
OPEN_END = SOURCE.index("void PopPunkGuitarUltimateAudioProcessorEditor::openSettingsPanel()", OPEN_START)
OPEN_GALLERY = SOURCE[OPEN_START:OPEN_END]


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

    def test_card_thumbnail_decode_is_deferred_outside_paint(self) -> None:
        card_start = GALLERY.index("class Card final")
        card_end = GALLERY.index("class Grid final", card_start)
        card = GALLERY[card_start:card_end]
        paint_start = card.index("void paint(juce::Graphics& g) override")
        paint_end = card.index("void setPalette", paint_start)
        paint_body = card[paint_start:paint_end]
        self.assertNotIn("ImageCache::getFromFile", paint_body)
        self.assertNotIn("existsAsFile", paint_body)
        self.assertIn("MessageManager::callAsync", card)
        self.assertIn("SafePointer<Card>", card)
        self.assertIn("ImageCache::getFromFile", card[card.index("void requestThumbnailLoad"):])

    def test_filter_keeps_selection_inside_visible_registry(self) -> None:
        rebuild_start = GALLERY.index("void rebuildCards()")
        rebuild_end = GALLERY.index("std::vector<Skin> skins;", rebuild_start)
        rebuild = GALLERY[rebuild_start:rebuild_end]
        self.assertIn("visibleSkins", rebuild)
        self.assertIn("selectedIsVisible", rebuild)
        self.assertIn("if (!selectedIsVisible && !visibleSkins.empty())", rebuild)
        self.assertIn("previewImage = {}", rebuild)
        self.assertIn("updateSelectionControls()", rebuild)
        self.assertIn("const auto active = std::find_if", rebuild)
        self.assertIn("visibleSkins.front()->index", rebuild)

    def test_empty_results_state_is_explicit_and_preview_safe(self) -> None:
        self.assertIn('emptyResultsLabel.setText("NO THEMES MATCH YOUR SEARCH"', GALLERY)
        self.assertIn("emptyResultsLabel.setVisible(visibleSkins.empty() && !showingPreview)", GALLERY)
        preview_start = GALLERY.index("if (showingPreview)")
        preview_end = GALLERY.index("search.setVisible(true)", preview_start)
        preview_layout = GALLERY[preview_start:preview_end]
        self.assertIn("emptyResultsLabel.setVisible(false)", preview_layout)
        self.assertIn("emptyResultsLabel.setBounds(viewport.getBounds())", GALLERY)

    def test_favourites_are_restored_only_for_registered_skin_ids(self) -> None:
        self.assertIn('getValue("ppguGalleryFavourites")', OPEN_GALLERY)
        self.assertIn("juce::StringArray::fromTokens(storedFavourites, \"|\", \"\")", OPEN_GALLERY)
        self.assertIn("std::find_if(skins.begin(), skins.end()", OPEN_GALLERY)
        self.assertIn("knownSkin != skins.end()", OPEN_GALLERY)
        self.assertIn("!favourites.contains(id)", OPEN_GALLERY)

    def test_favourites_are_persisted_to_user_properties(self) -> None:
        self.assertIn("std::move(favourites)", OPEN_GALLERY)
        self.assertIn('setValue("ppguGalleryFavourites", ids.joinIntoString("|"))', OPEN_GALLERY)
        self.assertIn("settings->saveIfNeeded()", OPEN_GALLERY)
        self.assertIn("saveFavourites(favourites)", GALLERY)

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
