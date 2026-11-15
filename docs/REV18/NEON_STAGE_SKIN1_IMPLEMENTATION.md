# NEON STAGE — SKIN-1 implementation record (REV.18)

**Canonical identity:** `SKIN-1 = NEON STAGE = skin-053 / Skin_053`  
**Working branch:** `rev18/neon-stage-skin1-implementation`  
**Scope:** visual theme and fallback palette only.

## Applied in this branch

- Updated `SKIN/Skin_053/theme.json` identity to `NEON STAGE`, family `neon_basement`.
- Preserved `skin-053`, index 53, all 28 control-map entries, their existing 12×16 grid coordinates, and runtime binding names.
- Preserved all 21 required palette-token keys and supplied the NEON STAGE palette.
- Replaced the three decorative SVG assets (texture, bezel, crest) with a coherent dark stage / cyan-neon / magenta-reflection visual system.
- Changed the default `PPGUThemeManager::Palette` fallback identity and colours in `Source/UI/ThemeManager.h` to SKIN-1 / NEON STAGE. The existing `resetToDefault()` assigns `Palette{}`, so this makes the fallback identity consistent without touching processor/APVTS/DSP code.

## Compatibility guardrails

- No APVTS parameter IDs or values changed.
- No audio, MIDI, sampler, scheduler, or DSP code changed.
- No control IDs added, removed, or renamed.
- No layout coordinates changed.
- No existing skin IDs were renumbered.
- ECO and 150 ms hot-swap manifest settings retained.

## Validation performed here

- Parsed the edited manifest as JSON.
- Verified exact identity `skin-053 / 53 / NEON STAGE`.
- Verified exactly 21 palette tokens.
- Verified exactly 28 canonical control bindings; no missing or extra IDs.
- Verified every declared grid rectangle remains within the 12×16 grid.
- Verified visual-only compatibility flags remain false for APVTS/layout/audio changes.

## Not yet verified — release blockers

- The existing `assets/preview.png` and `assets/hero.png` have not been regenerated or visually inspected against the new theme. The old preview may still depict the previous Carbon Studio artwork.
- The repository's runtime path/discovery and actual application of Skin_053 in the main editor and FX EDIT window have not yet been traced end-to-end.
- SVG loading/rendering, PNG CRC/dimensions, HiDPI, resize, ECO behaviour, and hot-swap fade have not been run in a Windows build.
- CMake/Release, CTest, and pluginval have not been executed in this environment.

**Status:** implementation branch contains the identity/fallback and decorative-asset preparation. This is not a release-ready or runtime-verified theme yet. Do not merge to the production branch until the blockers above are closed.
