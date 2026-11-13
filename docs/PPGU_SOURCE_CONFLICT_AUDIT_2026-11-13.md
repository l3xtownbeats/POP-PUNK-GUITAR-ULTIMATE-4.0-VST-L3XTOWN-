# PPGU — Source Conflict & Release Blocker Audit
Date: 2026-11-13
Audit target: `main` HEAD `97652bc24eefbc86a08579218b32910ef55f5671`
Audit branch: `audit/skin-conflict-resolution-2026-11-13`
Scope: source-backed inspection of theme registry/runtime/editor/validator and current INFO release risks.
Status: AUDIT FINDINGS RECORDED; NO RUNTIME FIXES CLAIMED.

## Executive result

The repository is accessible and the current source was inspected. The theme system is not yet consistent with the latest approved SIGNAL FORGE / NEON gallery contract. Several source-level contradictions are release blockers. This document records them before implementation so fixes can be atomic and testable. No C++ source, runtime registry, sample engine, or binary was changed by this audit.

## P0 — Must resolve before calling the theme gallery complete

### P0-1. Wrong default and fallback identity
Evidence:
- `Source/UI/ThemeManager.h`: Palette initializes to `skin-51 / Blackout Voltage`.
- `Source/UI/ThemeManager.cpp`: default-constructed Palette and `resetToDefault()` restore that same Blackout palette.
- `Source/PluginEditor.cpp`: `loadSavedSkin()` defaults missing preference to `skin-51`.
- `SKIN/theme_manifest.json`: `defaultSkin` is `skin-51`.
- `tests/ValidateSkinManifests.py`: `DEFAULT_ID = "skin-51"` and validator hard-requires it.
- `tests/ThemeManagerTests.cpp`: test expects `skin-51` as default.
Approved product decision (2026-11-13): SIGNAL FORGE / NEON is the only DEFAULT, first in SKIN, startup default and fallback; Blackout Voltage `skin-51` is second and remains user-selectable.
Resolution: identify/create the canonical SIGNAL FORGE / NEON stable theme ID without renumbering or deleting any existing theme; update manager defaults, registry ordering/default, editor preference fallback, tests and packaging in one migration. Do not simply relabel Skin_01 (currently Neon Riot) or reuse skin-51.

### P0-2. Registry order and UI selection are index-coupled
Evidence:
- `getSkinCount()` validates a numeric continuous range 1..total.
- `getSkinManifest(index)` searches by numeric index.
- `showSkinMenu()` loops integer indices 1..count and immediately loads/persists the selected item.
- Current registry order is numeric skin-01 ... skin-51, then skin-052; defaultSkin points to skin-51.
Approved contract requires display order independent of IDs: SIGNAL FORGE / NEON first, Blackout Voltage second, all other existing themes afterwards, preserving IDs and original manifests.
Resolution: registry-driven ordered entries, not index-derived iteration; retain a stable ID for every theme and map ID to manifest. Validate unique IDs and paths independently of display position. Do not renumber old themes.

### P0-3. Gallery transaction model is absent
Evidence: `showSkinMenu()` creates a PopupMenu; selecting an item calls `loadSkinByIndex(index, true)`, which immediately persists. There is no observed PREVIEW/APPLY/BACK transaction, entry-theme snapshot, favorites, category/search/large preview flow or rollback path in this editor code.
Resolution: implement gallery as a separate UI layer using the existing editor and ThemeManager; PREVIEW is runtime-only, APPLY commits/persists atomically, BACK/close rolls back, failed preview restores prior theme. Add tests for each transaction and persistence behavior.

### P0-4. Runtime theme asset contract is not enforced by the runtime loader
Evidence:
- ThemeManager validates manifest strings and declared dimensions/formats but does not load/decode the referenced preview/hero/texture assets, verify file existence, dimensions, checksums, or licenses.
- Runtime Skin Studio branch validates paths as strings only.
- Current registry points to assets such as `Skin_01/assets/preview.png`; direct fetch of that asset path returned 404 (this confirms the path is not available through the queried GitHub contents endpoint; it does not by itself prove the local checkout is missing it).
Resolution: a single package validator must verify every registry path, actual asset existence, PNG/WebP decoding, dimensions, 1x/2x policy, SHA256SUMS, safe SVG and license evidence. Runtime loader must fail closed to NEON without crashing. Do not decode large assets in audio callback.

### P0-5. Layout metadata conflicts with the approved fixed-geometry contract
Evidence:
- Skin Studio manifests require a `layout` value among STUDIO_CONSOLE / WORKSTATION_TOPBAR / RACK_SIDE / COMPACT_MINI.
- `Skin_052/theme.json` declares `layout: STUDIO_CONSOLE` and includes per-control grid coordinates.
- Approved gallery contract and current user decision require shared, invariant geometry and control placement across all themes; only visual layers/tokens/materials/decor may vary.
- The same manifest says `compatibility.layoutChanges: false`, while also carrying a layout variant and grid map. This is semantically contradictory even if current runtime ignores the coordinates.
Resolution: retain design metadata for historical traceability if needed, but mark layout/grid fields non-authoritative and reject them as runtime layout instructions. Canonical control geometry and parameter bindings must come from one shared control schema. Validate that skins cannot change bounds, IDs, parameter bindings or behavior.

### P0-6. The manifest and UI claim more than runtime implements
Evidence:
- Skin_052 notes explicitly say several controls are placeholders and grid coordinates are design metadata only.
- ThemeManager currently converts manifest data into a Palette; it does not implement the full four-layer renderer, sprite state system, fonts, 1x/2x assets, materials, decorative SVG composition or control-specific skin assets.
- PluginEditor::applyPalette applies a subset of palette colours to existing JUCE controls.
Resolution: use accurate status labels. Do not advertise the full gallery/four-layer theme engine as implemented until all layers are consumed and tested. Keep current palette switching labelled as partial runtime support until then.

## P1 — Architecture and quality gaps

### P1-1. Two incompatible manifest generations coexist
Legacy themes 01–51 use `cluster`, `identity`, nested `tokens`, `assetSpec`, `compatibility`, `validation`; Skin Studio 052+ uses `family`, `layout`, flat `palette`, `typography`, `effects`, `controls`, `assets`, `reduceInEco`, `hotSwapFadeMs`. ThemeManager has separate parsing branches. This creates schema drift and doubles validation obligations.
Resolution: define one canonical versioned schema and a deliberate migration/adapter layer. Do not bulk-rewrite theme files until the adapter and migration tests cover all currently registered themes.

### P1-2. Runtime menu does not use the approved gallery presentation contract
No evidence in inspected editor code of card previews, hero preview, categories, search, favorites or DEFAULT label. The popup menu is not the approved SKIN gallery.
Resolution: implement the gallery as a child/overlay window over the main UI, respecting `auxiliaryWindowsDoNotReflowMainWindow=true`.

### P1-3. Runtime root and packaging contract need end-to-end verification
Editor searches beside the VST3 bundle for `Skins/`, then beside executable, then development `SKIN/`. The release contract requires sibling `Skins/` next to the installed VST3 bundle. Verify Windows module path resolution, nested bundle traversal, install ZIP layout and actual FL Studio scan path. Do not silently treat a development checkout as a production install.

### P1-4. Token coverage is incomplete in rendering
The legacy Palette mapping exposes only a subset of the 21-token Skin Studio palette and the editor uses a subset of those fields. There is no evidence that every draw path is token-driven. Audit all LookAndFeel and component drawing code for literal colours and ensure all required control states use the shared tokens.

### P1-5. UI preference state and multi-instance isolation need explicit tests
The editor uses `juce::ApplicationProperties` user settings for `skinId`. Verify whether the preference is intentionally global per user or per plugin instance. User-approved behavior requires stable, predictable persistence; do not introduce static mutable theme state. Test two plugin instances selecting different skins if per-instance state is intended.

## P1 — Existing non-theme release blockers retained from INFO

The repository INFO records these open sampler risks and they remain release gates:
- safe retirement of old sample snapshots without use-after-free;
- aggregate memory/thread pressure from multiple streaming roots;
- worker exception/recovery policy;
- 64-voice stress under cache pressure, queue overflow and loop prefetch;
- cancellation/lifetime races during load and processor destruction;
- Windows Release + complete CTest + pluginval after changes.
The worker is documented as not automatically recovering after an exception. Do not alter snapshot ownership until quiescence/read-side lifetime is proven.

Other product-level gaps in INFO remain: Pattern runtime acceptance, full S6/BankScanner pipeline, full SFZ compatibility, all 300 preset definitions being playable, FL Studio 20 host acceptance, and CPU/RAM profiling are not confirmed.

## P2 — Build-system / version-policy conflict

The current source CMake pin was observed as JUCE 7.0.12 / CMake 3.22 in project records, while the newer REV 13.0 specification requires JUCE 8 / CMake >=3.24. This is a controlled dependency migration, not a search-and-replace. Keep the current pins until a separate upgrade branch builds and passes the complete regression suite.

## Required implementation order

1. Freeze a single source-of-truth decision record and resolve canonical NEON ID.
2. Add migration-safe ordered registry model; preserve all existing IDs/manifests.
3. Consolidate schema validation and explicitly deprecate layout-changing metadata.
4. Implement asset verification and package integrity checks.
5. Correct default/fallback and saved preference behavior.
6. Implement PREVIEW/APPLY/BACK gallery transactions, search, filters, favorites and preview surfaces.
7. Connect all four visual layers and every control state to tokens/assets; no layout or parameter binding changes.
8. Add tests: registry preservation, ID uniqueness, order/default, malformed manifest, missing/corrupt assets, preview rollback, apply persistence, favorites, two-instance state, auxiliary windows, visual-only/audio-state invariance.
9. Audit sampler lifecycle and add missing stress/failure/recovery coverage without real-time regressions.
10. Run Windows x64 Release, full CTest, pluginval strictness 5, package integrity and FL Studio 20 host acceptance. Record exact commit SHA and logs.

## Non-negotiable invariants

- SIGNAL FORGE / NEON is the only DEFAULT and fallback; Blackout Voltage skin-51 is second.
- Existing themes and IDs are preserved; no deletion, renumbering or silent remapping.
- Shared geometry and control functionality across all themes.
- Auxiliary windows overlay the main UI and never reflow it.
- Theme switching is visual-only; no APVTS, processor, MIDI, sampler, scheduler or DSP changes.
- No file I/O, asset decode, allocation-heavy work or blocking on audio callback.
- No duplicate ThemeManager/registry/pipeline.
- Never claim build, pluginval or FL Studio acceptance without current evidence.

## Audit boundary

This is a source-backed static audit of the listed files at the target commit. It is not a runtime test. No source fixes, build, CTest, pluginval or FL Studio host test are claimed by this document.
