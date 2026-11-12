# PPGU — NEON default shell + 15 selectable skins

Status: IMPLEMENTATION BASELINE / DESIGN CONTRACT / RUNTIME WORK PENDING
Working branch: `work/neon-gallery-16-skins`
Base: `work/signal-forge-panel-shell` at `840df82c1180661bff520a7a4f5671cc52d7c562`

## Product decision

The approved REV 7 mockup named **«Неоновый интерфейс PPGU для поп-панк гитары»** is the immutable default visual reference. It is not to be replaced, re-composed, or silently substituted by Blackout Voltage, Carbon Relay, or another theme. First launch must use this NEON design. Other skins are opt-in only through the SKIN gallery. Persist the selected stable skin ID; restore it on next launch. If the saved skin is missing, invalid, or incompatible, fall back to NEON and report the reason without interrupting audio.

Deliver 16 complete themes total: NEON default + 15 alternatives. Every theme must cover the same real component inventory and remain visual-only. Switching themes must not alter APVTS values/IDs, preset selection/state, MIDI, sampler, processor state, or DSP output.

## Gallery themes

1. NEON — approved REV 7 default; immutable layout/visual reference.
2. BLACKOUT VOLTAGE — graphite/black, machined metal, restrained acid-lime and teal.
3. DARK CHROME — polished dark chrome, cool silver edges.
4. CYBER PUNK — saturated cyan/magenta neon, dark futuristic surfaces.
5. VINTAGE AMP — classic tweed amplifier and warm hardware.
6. STAGE LIGHTS — concert-stage lighting, deep navy and luminous accents.
7. CLASSIC ROCK — restrained black, cream, brass and amber.
8. PUNK RIOT — distressed black, paper-white and signal red.
9. ELECTRIC BLUE — deep blue, electric cyan and steel.
10. BLOOD RED — charcoal, oxblood and controlled red.
11. PURPLE HAZE — violet, indigo and soft lavender.
12. ARCTIC ICE — cold graphite, silver and ice blue.
13. SUNSET DRIVE — warm amber, coral and magenta.
14. MILITARY GRADE — industrial olive, dark steel and safety amber.
15. CARBON FIBER — woven carbon, graphite and clean lime.
16. WHITE STUDIO — light professional surface, graphite typography and restrained blue.

Names and visual direction are initial product labels; do not change the approved NEON design to make themes fit a common template.

## Verified baseline on 2026-11-12

- Existing working branch `work/signal-forge-panel-shell` exists; its current HEAD was `840df82c1180661bff520a7a4f5671cc52d7c562`.
- A dedicated branch `work/neon-gallery-16-skins` has been created from that HEAD to isolate this work.
- The source has a SKIN button, Ctrl+K, a popup list, manifest-based loading, saved `skinId`, and palette application to several existing components. This is a selector, **not** a visual gallery.
- `SKIN/README.md` explicitly says there is no full gallery, no live layout reflow, and grid coordinates in Skin Studio manifests are metadata rather than real JUCE layout.
- `SKIN/SKIN_ENGINE_GAP_AUDIT_RU.md` records fixed-range catalog handling, incomplete 21-token runtime parity, hardcoded colours, missing dynamic discovery, no preview cards/search/favorites/fade, and missing runtime acceptance gates.
- Current registry declares 52 skins and `defaultSkin: skin-51`; current ThemeManager header defaults to `skin-51 / Blackout Voltage`. This conflicts with the approved NEON default requirement and must be corrected only after identifying the exact REV 7 source assets.
- The current legacy ThemeManager parser and newer Skin Studio schema have compatibility gaps. The newer Skin Studio parser accepts indices >=52, but the older catalog/loader has fixed-range and folder naming assumptions. Do not increase counts or merge packs until schema/registry/runtime handling are reconciled.
- Existing `Skin_052 Carbon Relay` is a reference Skin Studio candidate, not the approved NEON mockup.
- A separate branch `ui/modern-neon-stage-shell` contains three image assets, but no proof yet that they are the three REV 7 mockup files. Do not treat them as approved reference until filenames/content are compared against REV 7.
- The exact REV 7 branch was not identified in the branch listing checked so far. Continue repository-wide branch/commit/file discovery before asking the user to re-upload assets.

## Implementation sequence and acceptance gates

### A. Recover and lock the NEON reference
1. Identify the exact REV 7 branch/commit and all three mockup files.
2. Record their paths, blob SHAs, dimensions, and visual roles; preserve originals unchanged.
3. Create a reference comparison sheet from the three files and current editor screenshot/source geometry.
4. Mark all differences: overall composition, panel geometry, typography, knob/button materials, labels, decorative artwork, spacing, and auxiliary windows.
5. Treat the approved mockup as the source of truth. Do not infer missing visual details from Blackout Voltage or Skin Studio packs.

### B. Repair the shared theme engine
1. Define one canonical theme contract supporting legacy themes and Skin Studio themes without hard-coded count limits.
2. Use stable IDs and manifest-driven discovery; validate schema version, index/ID uniqueness, path containment, required assets, and control parity.
3. Make NEON the canonical first-run default and fallback. Persist manual selection only after a successful load.
4. Apply all 21 visual tokens consistently to the main editor, all real controls, keyboard, FX, and auxiliary windows. Remove remaining hard-coded visual colours where they bypass theme tokens.
5. Keep theme loading/scanning and image decode off the audio callback. No file I/O, allocation, lock, wait, or UI calls in audio processing.
6. Add safe visual-only hot swap; do not mutate processor/APVTS/MIDI/preset/DSP state.

### C. Build the SKIN gallery
- Open from the existing SKIN button and Ctrl+K.
- Show 16 selectable theme cards with preview image, name, family, selected marker, and clear Preview/Apply actions.
- Search, family filters, favorites, keyboard navigation, scroll/resize behavior, loading/error states, and explicit Apply.
- Preview must not persist; Apply persists only after successful load.
- Preserve current theme if a preview fails; fallback to NEON only when the active/saved theme is invalid.
- Do not claim a visual gallery while the implementation is still a PopupMenu.

### D. Produce the 15 additional themes
- Create genuinely distinct artwork/materials, not palette-only clones.
- All themes must map the same canonical controls and token contract.
- Validate preview/hero dimensions, SVG/PNG decoding, contrast, unique IDs, and path containment.
- The theme layout metadata must not move/rebind real controls unless a separately tested layout contract explicitly authorizes it.

### E. Required tests
- Manifest/schema validation and dynamic discovery across mixed legacy and Skin Studio IDs.
- First launch => NEON; manual theme persists after editor/host restart; invalid saved theme => NEON fallback.
- Preview does not persist; Apply does.
- 1000 repeated theme switches without crash/resource leak.
- APVTS/state/MIDI/preset invariance and rendered audio equivalence before/after theme changes.
- Main and auxiliary windows receive the same theme; no missing or clipped controls.
- DPI 100/125/150%, resize, keyboard navigation, accessibility contrast, and reduced-motion/ECO behavior.
- Windows x64 Release, full CTest, pluginval strictness 5, package contents, then manual FL Studio 20 acceptance with screenshots tied to exact commit SHA.

## Current status

This document records the approved direction and verified baseline only. It does not claim that the REV 7 assets have been recovered, the NEON default has been implemented, the gallery exists, the 15 themes are finished, or any new build has passed. Each gate must be updated only with exact source/CI/host evidence.
