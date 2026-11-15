# NEON STAGE — Unified Visual Design System (REV.18)

**Status:** APPROVED IMPLEMENTATION CONTRACT / SOURCE INTEGRATION NOT YET CLAIMED  
**Work branch:** `work/rev18-neon-stage-unified-ui`  
**Base:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV.18`  
**Scope:** visual layer only.

## 1. Authority and reference rule

The user-designated NEON STAGE DEFAULT THEME mockups already stored in this repository are the primary visual reference. Before changing any component, identify the exact approved mockup assets and their module mapping from the repository; do not substitute the README cover, generic artwork, or an assistant-generated concept for an approved UI reference. If several candidate images exist and canonical mapping is ambiguous, record the ambiguity and stop visual implementation for that component until the source is resolved.

This contract applies the same visual language to every module listed below. It does not force identical layouts: each module retains its own task-specific information architecture and functional composition.

## 2. Modules in scope

1. Main Interface — NEON STAGE shell.
2. AutoChords — harmony, articulation, rhythm and chord chips.
3. Curator — analysis cards and genuine GREEN / YELLOW / RED states.
4. Sound Hunter — guitar sound search and catalog.
5. Auto Discover — library discovery and analysis.
6. FX EDIT — effect pages and parameter controls.
7. AMP & TONE — amplifier and tone controls.
8. Import Guitar — SFZ import, validation and status.
9. Performance — actual MIDI/performance monitoring.
10. Settings — plugin/workspace settings.
11. Skin Gallery — theme browsing and selection.
12. About — product information.

## 3. Shared design tokens and components

Create or reconcile one canonical theme-token source, consumed by all module views:
- semantic palette: application background, raised panel, inset surface, border, primary text, secondary text, muted text, accent, accent-hover, focus, success, warning, error, disabled, meter and selection;
- typography: one font family/scale/weight/line-height contract;
- spacing, corner radii, border widths, panel elevation and layout grid;
- rotary controls: track, value arc, pointer, centre, label, value readout, hover/focus/disabled states;
- buttons: primary, secondary, toggle, segmented, icon and destructive variants;
- switches, dropdowns, chips, tabs, cards, meters, status badges, tooltips and empty/loading/error states;
- icon family, stroke weight, optical size and alignment;
- animation durations/easing and reduced-motion/ECO behavior.

Token values must be extracted from the approved NEON STAGE references and existing canonical theme manifests/source. Do not invent a replacement palette or silently change SKIN-1 identity. If source values conflict, document and resolve the conflict before editing the canonical registry.

## 4. Functional truth rule

Every visible interactive element must map to an existing real action, callback, state or parameter. A visual control without a working implementation must be visibly disabled or explicitly marked unavailable/coming later; it must not imitate live activity. Indicators and meters must be driven by actual subsystem data. GREEN/YELLOW/RED labels must reflect the Curator's real policy result, not decorative colour.

## 5. Strict non-regression boundary

This is a visual-only change. Do not modify:
- audio engine, DSP graph, sampler, sample mappings or audio callback;
- MIDI input/output, timing, note lifecycle or performance behavior;
- APVTS parameter IDs, parameter ranges/semantics or host automation;
- preset serialization/state compatibility;
- existing stable control identifiers and public module contracts.

Do not create a second engine, second library scanner, second discovery pipeline or parallel parameter model. Reuse existing production services and callbacks. If a required module has no production implementation, style only the existing truthful shell and record missing runtime behavior separately.

## 6. Integration sequence

**Phase 0 — Preflight and reference audit**
- Confirm base branch/HEAD and clean working state.
- Inventory all NEON STAGE mockups/assets and map each to its module/screen.
- Read the current skin registry, schemas, theme manager, UI component inventory and module source.
- Record current control IDs, callbacks and known gaps.
- Resolve SKIN-1/default/fallback identity before any manifest edit.

**Phase 1 — Shared foundation**
- Implement/reconcile shared semantic tokens and reusable JUCE look-and-feel components.
- Preserve legacy manifest compatibility and stable theme IDs.
- Add unit/contract tests for token completeness, control states, contrast, DPI scaling and deterministic rendering where practical.

**Phase 2 — Main shell**
- Integrate the shared foundation into the main NEON STAGE shell.
- Verify visual parity against the approved main-window reference.
- Verify every control against the source inventory.

**Phase 3 — Module-by-module rollout**
- AutoChords → Curator → Sound Hunter → Auto Discover → FX EDIT → AMP & TONE → Import Guitar → Performance → Settings → Skin Gallery → About.
- For each module: capture baseline; map existing controls; apply shared components; preserve its layout; connect only existing real actions; run module tests; capture after-state; record gaps. Do not move to the next module while a regression remains open.

**Phase 4 — Cross-module consistency**
- Compare typography, spacing, palette, knob geometry, button states, borders, icons, meters, focus, disabled, hover and error states across all 12 modules.
- Check light-independent contrast, Windows scaling/DPI, resize behavior, keyboard focus and reduced-motion/ECO policy.

**Phase 5 — Validation**
- Build Windows x64 Release on the exact final SHA.
- Run full CTest and pluginval strictness 5.
- Run visual/UI contract tests and inspect screenshots from the built VST3, not mockups.
- Verify state/preset compatibility and unchanged APVTS IDs.
- Verify no audio/MIDI/APVTS source changes in the UI-only diff.
- Separately confirm FL Studio 20 host behavior on that exact SHA.
- Publish no release-ready claim unless all required gates pass.

## 7. Acceptance criteria

A module is accepted only when:
1. Its approved reference has been identified.
2. It uses the shared NEON STAGE token/component system.
3. Its functional layout remains module-specific and intact.
4. Every visible control is mapped to real behavior or honestly disabled.
5. All visible states are consistent with real subsystem state.
6. Existing stable IDs, APVTS, MIDI, audio and preset behavior are unchanged.
7. Module tests and exact-SHA visual/build evidence are recorded.

The whole UI is accepted only after all 12 modules pass, cross-module review passes, and exact-SHA build/CTest/pluginval/host gates are recorded.

## 8. Reporting format

For every phase record: date, branch, base/head SHA, changed paths, controls/modules touched, tests actually run and result, screenshots/evidence, known gaps, and explicit audio/MIDI/APVTS non-regression check. Use the project's single append-only engineering journal; do not create a competing journal.

**Current state:** This document establishes the user's unified visual contract. It is not evidence that the modules have already been restyled, built, or host-verified.
