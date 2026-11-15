# REV18 SKIN-2…SKIN-6 — No-Loss Compatibility Audit

**Date:** 2026-11-15  
**Scope:** visual theme assets, registry compatibility and runtime packaging only  
**Branch:** `ui/neon-stage-skin1-completion`  
**Reference commit supplied for this pass:** `081f0c0c64e18614d9b39242c9ded00a36bbbd00`

## Decision

**Do not run the legacy 50-theme generator or merge tool against the approved five-theme sequence.** Do not change the registry or runtime loader until the asset-pack contract has been adapted and tested against the current source.

This is a compatibility hold, not a rejection of SKIN-2…SKIN-6. All five concepts remain approved. No theme is promoted to runtime-ready by this audit.

## Verified repository facts

1. `SKIN/approved_theme_sequence.json` (blob `22d99f20a7db3115f9e81dd42887c7035c351378`) defines:
   - SKIN-1 = NEON STAGE DEFAULT = `skin-053`, index 53, `Skin_053/theme.json`.
   - SKIN-2…SKIN-6 = OBSIDIAN GOLD, CARBON FORGE, MIDNIGHT CRIMSON, ARCTIC PLATINUM, VIOLET NOIR; stable IDs/indices `skin-103`…`skin-107`, indices 103…107.
   - The sequence status explicitly says PNG and runtime validation are pending.
2. `SKIN/theme_manifest.json` (blob `c77674fc1bddadf31fe901b1d4954c5ed360784b`) still declares `totalSkins: 102`. Its final registered indices are 99…102. It does not register indices 103…107.
3. All five new theme directories contain a `theme.json` and SVG source assets. Their `assets/preview.png` file is absent (checked for SKIN-2); the approved sequence also lists preview and hero PNGs as pending. No PNG completeness/CRC pass is claimed.
4. `SKIN/control_schema.json` (blob `48085d8d300f7d9d3787b06976c3dc02ab6362c4`) defines exactly 21 palette tokens and 27 required Control-IDs. These are a frozen compatibility contract and must remain unchanged:
   - Palette tokens: `window_bg`, `panel`, `panel_elevated`, `control_bg`, `control_face`, `control_border`, `text_primary`, `text_secondary`, `text_disabled`, `accent`, `accent_glow`, `danger`, `warning`, `success`, `lcd_bg`, `lcd_text`, `led_on`, `led_off`, `shadow`, `highlight`, `lowlight`.
   - Required Control-IDs remain exactly as declared in the schema; no rename, removal, fabricated implementation or binding change is permitted.
5. `.github/workflows/vst3-build.yml` (blob `92db54600985e070342690eeb2d2afee765140f2`) copies only directories referenced by `SKIN/theme_manifest.json`. Consequently, SKIN-2…SKIN-6 are currently excluded from the packaged runtime payload because they are not yet in that registry.
6. `tools/generate_skin_batch.py` is hard-coded to a 50-theme REV14 batch, old theme families/layout variants, and starts from the existing registry total. It is not a suitable generator for the five individually approved REV18 designs.
7. `tools/validate_skin_pack.py` validates the legacy pack schema, including exactly 21 palette tokens, registry count consistency, PNG structure/CRC/dimensions, SVG safety and G1–G4 pack/registry parity. It is useful as a source of checks, but its pack contract must be explicitly adapted or wrapped for this five-theme sequence.
8. `tools/merge_skin_pack.py` requires a continuous numeric registry and exact pack-to-registry field parity. It must not be used until a dry-run migration proves that all 102 existing records are byte-for-byte preserved and the five new records append at 103…107 without rewriting historical IDs, paths, metadata or artwork.

## Required no-loss migration protocol

1. Freeze and hash the current 102-entry registry, all existing theme manifests/assets, both schemas, and the approved sequence. Record per-file path, byte length and SHA-256.
2. Validate SKIN-2…SKIN-6 independently against their approved JSON specifications. Keep logical labels SKIN-2…SKIN-6 distinct from immutable stable IDs/indices `skin-103`…`skin-107`.
3. Generate the approved asset kit per theme (preview 360×240 PNG; hero 800×450 PNG; SVG source layers; required sprites/atlas and baked FX layers only where the runtime contract consumes them). No placeholder art or fake runtime behavior.
4. Verify every PNG signature, dimensions, complete chunk structure and CRC; verify SVG XML safety; verify all referenced paths exist; verify G1–G4 and manifest-to-folder parity.
5. Build a dedicated five-theme pack manifest. Run compatibility validation in read-only/dry-run mode first. The expected result is exactly five additions, zero deletions or modifications among the historical 102 entries, no duplicate IDs/indices/paths, and final indices 1…107.
6. Only after the audit passes, append the five exact approved entries to the canonical registry, set the total to 107, and update the package manifest transactionally with rollback. Do not renumber or rewrite the 102 historical entries.
7. Update runtime packaging so the registry-approved 107 themes and their complete assets are copied. Add a packaged-resource check that fails if any registered theme asset is missing.
8. Confirm the runtime loader actually resolves each theme's manifest and assets from the packaged VST3 resource location. A successful source-tree lookup is not runtime proof.
9. Run exact-HEAD Windows x64 Release build, full CTest, pluginval strictness 5, packaged runtime skin checks, then manual FL Studio 20 host acceptance. Keep PR #43 Draft until all gates pass.

## Hard exclusions

- No changes to Audio Engine, DSP, MIDI, APVTS, parameter IDs/ranges/defaults, preset data, audio callbacks, sample playback or existing control attachments.
- No layout redesign, no fabricated controls, no false “12 modules complete” claim. Existing runtime availability must remain truthful.
- No automatic renumbering, no bulk registry rewrite, no legacy 50-theme batch generation, and no merge into the base branch.

## Current result

**Compatibility audit recorded; no registry migration, theme merge, PNG generation, or runtime promotion performed by this document.** The next implementation unit is a REV18-specific pack generator/validator that consumes the approved sequence directly and emits a dry-run migration report before any canonical registry write.
