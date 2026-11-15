# Design asset library

This directory is the canonical home for visual reference material. These files are retained intentionally; moving them does not imply that every image is a production-ready runtime asset.

## Structure

- `reference/approved-mockups/` — approved NEON STAGE interface mockups, numbered in review order. Use these as visual references; implementation status is tracked separately in `Info.txt` and the UI specifications.
- `reference/theme-previews/` — NEON STAGE DEFAULT visual/theme sample images. These are preserved as design references, not additional active themes.
- `reference/unclassified-visuals/` — retained images whose exact role has not yet been verified. Do not delete or promote them to runtime resources until reviewed.
- `brand/` — product cover/brand artwork.
- `../../assets/` — actual repository assets referenced by code, CMake, manifests or product documentation. Keep runtime assets here unless a manifest specifies another path.

## Preservation and classification rules

1. Mockups and theme samples are source-of-truth visual references and must not be discarded during repository cleanup.
2. A visual reference is not proof that a feature is implemented.
3. Do not duplicate these large image files elsewhere; link to this canonical location from documentation.
4. Before moving any runtime image, check code references, theme manifests, packaging rules and CI.
5. Unclassified images remain preserved until their provenance and purpose are established.
