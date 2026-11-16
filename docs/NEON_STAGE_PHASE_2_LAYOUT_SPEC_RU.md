# NEON STAGE — Phase 2 interface layout and visual-shell contract

**Branch:** `ui/neon-stage-playing-core`  
**Scope:** visual shell and geometry only. This phase does not change DSP, audio callback, MIDI routing, sampler, presets, APVTS state, parameter IDs, theme IDs, or control meanings.

## Main-window composition

The main editor remains a single responsive three-zone instrument workspace with a full-width performance footer.

| Zone | Responsibility | Existing live components | Visual treatment |
|---|---|---|---|
| Header rail | Product identity and top-level navigation | PPGU title, L3XTOWN AUDIO LAB subtitle, IMPORT GUITAR, AUTOCHORDS, AUTO SOUND HUNTER (disabled), CURATOR (disabled), SETTINGS, ABOUT (disabled), SKINS | Theme-derived raised rail, fine border, accent spine and lower divider |
| Left | Preset discovery and selection | Factory preset selector, category filter, preset search | PRESET LIBRARY panel |
| Centre | Playing mode, guitar artwork and performance controls | MELODY/PATTERN, mode selector, active-skin hero image, nine existing rotary controls, sync selector | PLAYING CORE panel; artwork is a visual surface, never a substitute for controls |
| Right | Amplifier and effect bypass access | AMP & TONE heading and seven existing FX buttons | FX CHAIN panel; existing FX order and bindings are immutable |
| Footer | Performance visibility and MIDI input | Performance display, status text, 49-key MIDI keyboard (notes 36–84) | Full-width lower strip |

## Geometry rules

- Existing editor bounds remain resizable: 900×640 minimum, 1600×1100 maximum; first launch remains 1024×700.
- Main content uses 14 px outer inset, 10 px inter-column gap, 48 px header, and 132 px footer.
- Left and right columns retain the current clamped responsive widths (190–250 px and 230–300 px respectively).
- Centre zone retains the three-column × three-row arrangement for the nine existing controls.
- Artwork occupies the dedicated 116 px strip below the MELODY/PATTERN and mode row.
- All JUCE component bounds and hit targets remain authoritative. Skin grid metadata is not a live layout engine.

## Binding and compatibility invariants

- Nine rotary controls keep their current order and APVTS IDs: `ampGain`, `eqHigh`, `eqMid`, `eqLow`, `velocity`, `dynamics`, `swing`, `humanize`, `accent`.
- Seven FX bypass controls keep their approved order and IDs: `compressorOn`, `eqOn`, `distortionOn`, `chorusOn`, `delayOn`, `reverbOn`, `cabinetOn`.
- No additional APVTS ID or control is introduced by the visual shell.
- Disabled / deferred modules remain visibly disabled; visual styling does not imply their backends exist.
- Theme changes affect semantic palette, typography and skin artwork only.

## Implemented in this slice

- Added a theme-aware graphical header rail behind the existing navigation controls.
- Renamed the right-hand panel chrome from SIGNAL CHAIN to FX CHAIN; the existing AMP & TONE and seven FX controls remain unchanged.
- Added a truthful artwork fallback label when the active skin has no valid hero image.
- Clipped the central technical grid to the Playing Core zone so it cannot draw across neighboring panels.

## Not claimed as complete

This source-only interface slice is not a screenshot-matched reproduction or host-accepted release. Exact reference-image comparison, Windows x64 VST3 build, CTest, pluginval strictness 5, portable packaging, and FL Studio 20 acceptance remain separate validation gates on the exact resulting commit SHA.
