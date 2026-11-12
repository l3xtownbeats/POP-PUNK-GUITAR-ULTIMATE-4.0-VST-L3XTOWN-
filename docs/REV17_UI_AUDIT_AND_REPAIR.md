# REV.17 — UI audit and mandatory repair register

Date: 2026-11-12  
Branch: `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`  
Reference: the three user-approved SIGNAL FORGE mockups. The mockups are the visual source of truth; this document is not a replacement layout and must not be used to invent a different design.

## Working protocol

**CHECK → ANALYZE → FIX → RE-CHECK → RESULT**

Do not mark a control complete because it exists in a schema or APVTS. A control is complete only when the intended component is visible in the approved location, has the correct label, is attached to the correct parameter or real action, displays truthful state, and passes build/host checks.

## Source audit — confirmed

### Existing editor components
- Preset selector: `presetBox`
- Category filter: `categoryBox`
- Preset search: `presetSearch`
- Skin popup: `skinButton`
- Import entry point: `loadButton`
- AutoChords entry point: `autoChordsButton`
- Mode selector: `modeBox` with `playMode`
- Sync selector: `startModeBox` with `syncMode`
- Nine rotary sliders and APVTS attachments
- Seven FX toggle buttons and APVTS attachments
- MIDI keyboard, MIDI range 36–84
- Performance display component
- Auxiliary multi-window workspace component

### Exact nine-knob bindings — preserve without renaming
1. GAIN → `ampGain`
2. TREBLE → `eqHigh`
3. MID → `eqMid`
4. BASS → `eqLow`
5. VELOCITY → `velocity`
6. DYNAMICS → `dynamics`
7. SWING → `swing`
8. HUMANIZE → `humanize`
9. ACCENT → `accent`

### Exact seven FX bindings — preserve without renaming
1. COMPRESSOR → `compressorOn`
2. EQ → `eqOn`
3. DISTORTION → `distortionOn`
4. CHORUS → `chorusOn`
5. DELAY → `delayOn`
6. REVERB → `reverbOn`
7. CABINET IR → `cabinetOn`

The seven buttons currently open their corresponding FX editor selection while retaining their APVTS ButtonAttachment. Do not repurpose them as effect-detail controls.

## Defects and required repairs

| Priority | Finding in REV.17 source | Required correction | Acceptance evidence |
|---|---|---|---|
| P0 | Current `resized()` places the nine knobs in two unrelated groups: four in AMP and five in PLAYBACK; FX is a separate narrow column. This does not match the approved left/center/right mockup zones. | Rebuild the main editor bounds against the approved mockup zones: left preset browser; center guitar/performance area with all nine knobs and mode controls; right AMP & TONE/LED and FX controls; lower keyboard, Pitch/Mod, Performance Display and status. | Screenshot comparison against all three mockups at supported sizes; no clipped or overlapping controls. |
| P0 | Preset selector, category and search are currently in the top header rather than grouped as the left preset-browser region. | Move and group them in the left browser region, keeping filtering behaviour and selected preset stable. | Browser placement and functional search/category regression test. |
| P0 | The painted hero/banner consumes a large fixed vertical region and the source editor still uses the legacy three-panel AMP/FX/PLAYBACK arrangement. | Replace the legacy arrangement with the approved SIGNAL FORGE shell; do not merely recolour it. Keep guitar artwork only in the mockup-designated center area. | No legacy panel geometry remains; screenshot parity review. |
| P0 | The skin JSON explicitly says its 12×16 coordinates are metadata only; live JUCE bounds remain authoritative. | Implement and validate runtime layout mapping from the selected skin/control layout, with approved mockup geometry as the baseline. Skin changes must not change control meaning or APVTS IDs. | Runtime parity assertion: each required control ID resolves to one visible component and valid bounds. |
| P1 | The current main editor does not expose the complete top-bar actions listed in the product inventory (Prev/Next/Favorite, Auto Discover, Curator, Settings, About). | Add real actions when implemented; until then show explicit disabled/unavailable states, never clickable dummy controls. | Every visible action has a real handler or is visibly disabled with an explanatory tooltip. |
| P1 | The main editor exposes only MELODY and PATTERN. | Add PUNK GROUP only with its actual processor behaviour; otherwise show it as unavailable, not as a functioning mode. | Mode-switch tests and truthful disabled state. |
| P1 | The current keyboard is present, but the full approved lower strip including PITCH, MOD, performance information and status is not yet represented as one coherent zone. | Complete the lower strip and wire each live display to its actual MIDI/host source. | MIDI/host state tests; no fabricated meter or transport values. |
| P1 | FX EDIT, AutoChords and Import panels exist in partial form; several named sections are not implemented. | Apply the same visual system to every panel and finish each control only with a real handler/parameter. | Open/close, resize, focus, state and visual consistency tests for every panel. |
| P1 | Workspace has close/maximize/drag/resize/tile infrastructure, but full product-level behaviour still needs validation. | Verify independent windows, X, maximize/restore, resizing, adaptive tiling and compact multi-panel view. | 1–6 panel test matrix, including small/large editor sizes. |
| P2 | Current minimum editor size is 980×800 and the layout uses fixed vertical allocations. | Make layout responsive to editor size and DPI; do not silently clip controls. | 100%, 125%, 150% DPI and resize screenshots. |

## Parameter inventory rule

The processor contains additional parameters beyond the 9 knobs and 7 FX switches, including `master`, `voices`, `palmMuteThreshold`, `tuning`, `pan`, `output`, articulation/playing parameters, melody/pattern parameters, Pattern DNA, FX slots/detail parameters and double-track controls.

These are engine parameters, not automatically GUI controls. Each must be assigned to its appropriate approved panel (Performance, Articulation, Pattern DNA, FX EDIT, Settings, etc.) without changing IDs, ranges, defaults or processor behaviour. Do not add duplicate visible controls where the approved mockups do not call for them.

## Non-negotiable visual and functional constraints

- The three approved mockups are the only visual layout authority.
- The former editor layout is not to be retained as an alternate shell.
- Copy the mockups' hierarchy, placement, grouping, proportions and visual language; do not embed the mockup image as the plugin UI.
- Every existing control and engine function must remain accounted for.
- All auxiliary panels use the same design system as the main shell.
- Never present placeholder functionality as operational.
- No APVTS ID changes without an explicit approved migration.
- No audio-thread blocking, allocation or UI access.
- A source change is not "complete" until build, tests and FL Studio 20 validation have been performed.

## Current audit result

- [x] Source editor and processor declarations inspected.
- [x] Nine knob attachments and exact IDs verified in `PluginEditor.h`.
- [x] Seven FX attachments and exact IDs verified in `PluginEditor.h`.
- [x] Current layout mismatch confirmed in `PluginEditor.cpp::resized()`.
- [x] Skin layout metadata confirmed not to drive runtime bounds yet.
- [ ] Main shell corrected to the three approved mockups.
- [ ] Runtime layout mapping implemented.
- [ ] All required panels visually unified.
- [ ] Build and automated tests passed.
- [ ] FL Studio 20 host acceptance passed.

**Status: AUDIT COMPLETE; UI REPAIR NOT YET COMPLETE.** This register records verified findings and acceptance criteria. It does not claim that the UI fixes or build tests have already been completed.


## Repair pass 1 — main-shell geometry (2026-11-12)

- [x] Replaced the legacy AMP / FX / PLAYBACK three-column control placement with the approved high-level zones: left preset browser, center instrument/performance controls, right AMP & TONE / FX rack, lower performance and MIDI strip.
- [x] Grouped preset selector, category filter and search in the left browser region.
- [x] Repositioned all nine existing knobs together in a 3×3 center grid; all nine original APVTS SliderAttachments and parameter IDs were preserved.
- [x] Kept the seven existing FX buttons together in the right rack; all seven original APVTS ButtonAttachments and IDs were preserved.
- [x] Removed the legacy oversized hero/banner painting so it no longer occupies the main instrument area.
- [x] Kept the existing MIDI keyboard and live Performance Display in the lower full-width strip.
- [x] Retained the existing top-bar actions that are genuinely implemented: Skin, Import Guitar and AutoChords.
- [ ] Exact pixel-level comparison against the three original mockup image files — not performed in this pass.
- [ ] Runtime skin-schema-to-bounds mapping — not implemented in this pass.
- [ ] New top-bar actions (Auto Discover, Curator, Settings, About, preset navigation/favorite) — not implemented.
- [ ] PUNK GROUP mode — not implemented; the existing MELODY/PATTERN behaviour remains unchanged.
- [ ] Multi-window matrix and all DPI/resize states — not tested in this pass.
- [ ] Build, automated tests and FL Studio 20 validation — not run; no build runner/host was invoked.

**Repair pass 1 status: SOURCE LAYOUT REORGANIZED; BUILD/HOST VALIDATION PENDING.** This is a source-level UI repair, not a claim of a successful compiled plugin.
