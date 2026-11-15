# NEON STAGE DEFAULT — Unified Interface Theme Handoff (REV.18)

**Canonical theme:** SKIN-1 = NEON STAGE DEFAULT = `skin-053/Skin_053`  
**Reference mockups already present in this branch:** eight PNG files:
- `NEON STAGE DEFAULT THEME (1).png` … `NEON STAGE DEFAULT THEME (7).png`
- `02 INTERFACE  NEON STAGE DEFAULT AUTOCHORDS .png`

This record treats those uploaded mockups as the visual reference set. The mockups are not permission to invent functionality: the current control inventory, UI theme spec, master journal and actual component bindings remain authoritative for behaviour.

## One visual system for every module

Every window and panel must use the same ThemeManager palette, typography, spacing scale, corner/radius language, borders, focus treatment, icon family, rotary/slider/button states, status colours, and window chrome. Module layouts may differ to fit their tasks; the visual identity must not.

Shared visual tokens:
- Base canvas: deep near-black violet `#100B18`
- Primary / raised surfaces: `#1C1224` / `#291936`
- Primary accent: neon cyan `#2DD4FF`
- Secondary light: `#7CE8FF`
- Secondary reflection: magenta used sparingly
- Text: `#F7F3FB`; secondary `#C9BDD2`
- Focus/highlight: `#B58BD8`
- Consistent semantic success/warning/danger colours from the manifest
- No module-specific alternate skin, unrelated palette, or isolated component styling.

## Module-to-theme coverage and functional truth

| Module | Visual treatment required | Functional presentation rule |
|---|---|---|
| Main PluginEditor | Full NEON STAGE shell; header, browser, playing core, FX chain, AutoChords area, amp/tone, performance zone | Only controls with actual component/action/attachment are active |
| Preset Browser | Shared search/selectors, category chips, active row, status typography | Search/category filter list; do not imply preset changes until selected |
| AutoChords | Use the uploaded AutoChords mockup as layout/art direction; shared panel header, cards/chips, selected state, compact display | Setup-only/disabled until backend and MIDI routing are implemented; no generated MIDI or fake playback |
| FX CHAIN / FX EDIT | Same rotary, toggle, tab, graph, value and page components; same window chrome | Existing seven bypass switches retain their exact DSP meanings; detail controls only when source-bound |
| AMP & TONE | Shared rotary/selector/readout components | Only amp model/Gain/Bass/Treble actually implemented are interactive |
| Import Guitar | Shared drop-zone, browse button, status panel and progress state | Real asynchronous SFZ import status only; no invented completion |
| Sound Hunter / Auto Discover | Shared module shell, source cards, license/status badges, progress and results layout | Discovery controls remain disabled until production pipeline is wired; never fabricate scan results |
| Curator | Shared result cards, GREEN/YELLOW/RED review badges, provenance/license panels | Review states must come from real curation policy/results; no simulated approvals |
| Settings | Shared section cards, toggles, selectors and labels | Expose only existing persisted settings / real controls |
| Performance | Shared compact telemetry cards and typography | Read-only; show only measured, connected sources |
| Skin Gallery | Shared cards, preview frame, active marker, search and family filter | Selecting a skin changes visual tokens only |
| About | Shared branded panel and typography | Keep entry disabled until real About panel content exists |
| Workspace / overlays | Shared title bar, close/maximize controls, focus and geometry styling | Do not claim overlay manager features before implementation |

## Required implementation sequence

1. Inspect all eight reference PNGs at native resolution and map their elements to the module inventory; record any visual mismatch rather than silently inventing UI.
2. Establish shared reusable JUCE theme components/styles (palette, typography, buttons, rotary, toggles, selectors, cards, status, window chrome).
3. Apply the shared styles to the main editor and every existing theme-aware panel.
4. Add consistent disabled/deferred states to AutoChords, Auto Discover, Curator, About, and any not-yet-backed controls.
5. Audit all hardcoded colours and per-panel styling; replace with ThemeManager tokens without changing behaviour.
6. Verify all open panels recolour on theme change and newly opened panels inherit the current palette.
7. Validate DPI 100/125/150%, resize, focus, hover, pressed, disabled, selected, success/warning/error states, and light/dark content contrast.
8. Run the static skin validators, Windows Release, CTest, pluginval strictness 5, and FL Studio 20 host check on the exact commit SHA.

## Hard constraints

- Do not redesign the NEON STAGE DEFAULT reference identity.
- Do not create separate visual themes for AutoChords, Curator, Sound Hunter, FX EDIT, or any other module.
- Do not change APVTS IDs/values, MIDI, audio/DSP, sampler, preset semantics, or routing during UI-only integration.
- Do not make a mockup element active without a real binding/action.
- Do not show fake meters, scan progress, curation verdicts, MIDI generation, or playback.
- Preserve existing skin IDs and user selection compatibility.
- Minimize CPU/RAM: static/baked assets, vector JUCE controls, cached drawing, clipped/region repaint; no unnecessary continuous animation.

## Status

The reference PNGs are present in the branch and the canonical Skin_053 identity/palette work has been prepared. This handoff is a requirements and coverage map, not proof that every module has already been restyled or that Windows/runtime acceptance has passed.
