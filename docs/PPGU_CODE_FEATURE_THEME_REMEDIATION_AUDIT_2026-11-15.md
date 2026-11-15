# PPGU — Code, Feature and NEON STAGE Remediation Audit
Date: 2026-11-15
Audited revision: 38235b91be38d4dac9eb6fe104c8a218256acb6d
Branch: work/neon-stage-rich-console-ui
Scope: source-level verification. Findings are not marked fixed unless a later commit and test proves the correction.

## Executive result

The current revision has successful Windows x64 Release build, CTest and pluginval strictness 5 steps according to workflow run 43656174444. This verifies compilation and the automated tests that exist; it does not prove every advertised control is connected, validate the visual result inside FL Studio 20, or establish final package/release acceptance.

## Confirmed findings

### P1 — FX parameters exposed but not implemented in DSP

Source/PluginProcessor.cpp declares the following APVTS parameters, but the audited DSP body reads each only once (its declaration). They are therefore not connected to audible processing in this revision:
- eqLowFreq, eqMidFreq, eqMidQ, eqHighFreq
- distortionTone
- cabinetLowCut, cabinetHighCut

Evidence: parameter declarations around lines 133–145 and 168; DSP implementation around lines 597–846. Current EQ split uses fixed 250 Hz and 2500 Hz one-pole filters; distortion is a fixed tanh shaper without tone control; cabinet processing uses the fixed original 64-tap IR without the advertised cuts.

Required remediation:
1. Implement frequency/Q-aware EQ with stable coefficient updates and state continuity, or remove/hide unsupported controls without breaking preset/state compatibility.
2. Implement distortion tone shaping with bounded, click-free parameter behavior.
3. Implement cabinet low/high-cut filtering with correct sample-rate bounds and stable state.
4. Add DSP tests proving parameter changes affect output, bypass is transparent, and no NaN/Inf or runaway gain is produced.
5. Preserve existing APVTS IDs, ranges, defaults and serialized state.

### P1 — AutoChords is a setup-only placeholder

Source/PluginEditor.cpp, PPGUAutoChordsPanel (around lines 1061–1139): button is permanently disabled; tooltip says MIDI generation engine is not implemented; panel only attaches scaleRoot, scaleMode and patternSlot; UI text says MIDI chord generation/custom mode/preview are not connected.

This is not a completed feature. Required remediation is a real chord-generation/preview/MIDI-output design, with explicit ownership of generated notes and note-off/panic handling; do not simply enable the button before the engine and tests exist.

### P2 — FX response graph is decorative, not a measured response

PPGUFXEditPanel::paint() draws the same generic cubic curve regardless of selected effect or parameter values. The caption calls it a VISUAL GUIDE, so it is not mislabelled as a measurement, but it does not provide a meaningful live response display. Either implement effect-specific response visualization or keep the visual-guide label and document the limitation.

### P1 — Theme asset metadata validation is not equivalent to runtime asset validation

Source/UI/ThemeManager.cpp validates manifest paths/specifications and palette structure. The audit did not find proof that every referenced asset is resolved, checked for existence, decoded, dimensions-checked and safely confined to the active skin root before use. PluginEditor has a fallback path for hero artwork, but that is not a complete asset integrity audit.

Required remediation: validate normalized relative paths and reject traversal/absolute paths; check required files exist and are readable; decode preview/hero images and verify dimensions/format; define optional/required policy for texture/bezel/crest; test missing, corrupt, oversized and traversal paths; keep fallback rendering deterministic and report missing assets.

### P2 — Visual host acceptance remains open

Automated build, CTest and pluginval do not prove that the complete NEON STAGE DEFAULT interface renders correctly at minimum/maximum editor sizes or in FL Studio 20. Main editor, popups, auxiliary panels, keyboard, focus states, scaling and skin switching need a real host capture/review. No FL Studio acceptance is claimed.

### P2 — INFO.txt contains historical statements that contradict the current implementation

Info.txt is a long append-only history and still contains older statements such as runtime skin engine not implemented, 50/51 skin counts, old default skin IDs and older UI state. Some are valid historical records, but the document needs a prominent dated current-state index and clear labels so old entries are not mistaken for current truth. Do not erase historical records; append a canonical status section and cross-link superseded entries.

## Verified boundaries

- No source changes were made during this audit.
- No APVTS IDs/ranges/defaults, DSP, sampler, MIDI or preset serialization were modified by this audit.
- No claim is made that the findings above are already fixed.
- Each implementation commit must be followed by exact-HEAD Windows build, CTest, targeted regression tests, pluginval, package checks and visual host review where applicable.

## Required remediation order

1. Fix and test the seven declared-but-unused FX controls.
2. Implement AutoChords as a real, safe MIDI feature, or explicitly scope it out of the product UI until implemented.
3. Complete strict runtime theme asset validation and tests.
4. Complete theme-wide UI parity and host visual acceptance for NEON STAGE DEFAULT / SKIN-1 (skin-001).
5. Update the current-state index in INFO.txt while preserving history.
6. Run final exact-HEAD validation and inspect the packaged VST3 + Skins layout.

## Change log

- 2026-11-15: Initial source-level audit at 38235b91be38d4dac9eb6fe104c8a218256acb6d. Confirmed findings listed above; remediation pending.

## Revalidation — 2026-11-15, branch HEAD after DSP control implementation

Rechecked the current branch source after the original audit SHA. The original seven-control finding is historical and must not be read as the current source state.

### P1 — Seven FX controls: SOURCE CONNECTION CONFIRMED

Current source review:
- EQ frequency/Q controls are read in `processEq()` and passed to `makeLowShelf()`, `makePeaking()`, and `makeHighShelf()`: `eqLowFreq`, `eqMidFreq`, `eqMidQ`, `eqHighFreq`.
- Distortion tone is read in `processDistortion()` and determines the bounded post-shaper one-pole low-pass cutoff: `distortionTone`.
- Cabinet low/high cuts are read in `processCabinet()` and determine the low-pass/high-pass shaping of the built-in cabinet-colour response: `cabinetLowCut`, `cabinetHighCut`.
- Existing APVTS IDs and parameter ranges remain present.

**Updated status:** the seven controls are connected at source level in this branch. The original “declared but unused” finding is superseded for this HEAD. This is not yet a complete DSP acceptance: processor-level parameter-sweep tests, automation stress, click/transient review and exact-HEAD CI are still required. The current DSP contract test only directly exercises the biquad coefficient builders.

### Regression coverage added

- `tests/DspFilterTests.cpp`: added response-difference assertions for low-shelf frequency, peaking frequency, peaking Q and high-shelf frequency. This guards against accidentally making these EQ controls inert at the coefficient layer.
- Commit: `99e28835798ef4586256e80dcf650b0a64aeea4e`.

### Still open

1. AutoChords remains a setup-only placeholder; keep its action disabled until a real MIDI generation/preview engine and note ownership tests exist.
2. Runtime theme asset validation still needs path traversal, missing/corrupt/oversized image and decode/dimension coverage.
3. FX processor-level parameter response tests and click-free automation checks remain open.
4. Exact-HEAD Windows x64 Release, full CTest, pluginval strictness 5 and FL Studio 20 host review have not been run for commit `99e28835798ef4586256e80dcf650b0a64aeea4e`.

**No PASS claim:** this revalidation is a source inspection plus a test-source change. It is not a build, runtime, pluginval or DAW acceptance result.


### Processor-level EQ response test — 2026-11-15

- Added a real `processBlock()` integration check in `tests/ProcessorPatternIntegrationTests.cpp`: compare the same synthetic SFZ playback with neutral EQ versus +9 dB Low Shelf at 300 Hz.
- Existing Chorus and Reverb oversized-block tests remain in place.
- Test fixture uses 24 parameter entries and explicit EQ state per render.
- Commit: `818520fadcdcfbd1ecd73651f0ecfc210033eeee`.
- Status: SOURCE TEST ADDED; exact-SHA build and execution pending. No PASS claim until CI.
