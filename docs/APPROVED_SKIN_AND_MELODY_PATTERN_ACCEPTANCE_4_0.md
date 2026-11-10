# POP PUNK GUITAR ULTIMATE 4.0 — approved main skin

Status: user-approved visual reference. This document is an implementation contract, not a claim that the UI is already implemented.

## Non-negotiable visual direction
Use the user's supplied 1402x1122 screenshot as the visual source of truth. Preserve the dark charcoal/black hardware-panel look, hot-magenta accents, white labels, thin magenta outlines, brushed/soft metallic controls, and the full-width concert/guitar hero artwork. Do not redesign this into a generic JUCE panel or replace it with a different theme.

## Main window hierarchy
1. Top transport/header strip: PPGU mark; previous/next preset; editable current preset name; save; preset folder/browser selector; CPU and RAM meters; settings; minimize/close.
2. Hero banner: product wordmark and slogan on the left, electric-guitar/concert visual on the right. This is branding artwork, separate from functional controls.
3. Performance mode strip: two mutually exclusive primary modes, MELODY and PATTERN. AutoChords toggle, musical key and scale selectors remain visible.
4. Preset and instrument strip: previous/next preset, preset title and short description; Guitar Type selector with Electric and Acoustic. Hybrid can be added in the instrument selector without changing the skin language.
5. Tone/effects/playback panels: AMP & TONE knobs; FX CHAIN pedals (Reverb, Chorus, Flanger, Delay, Distortion); PLAYBACK knobs (Velocity, Dynamics). Controls must be real host-automatable parameters, not decorative mock controls.
6. On-screen keyboard at the bottom of the main work area, with pitch/key highlights and functional pitch/mod wheels.
7. Bottom navigation: Browser, Presets, Settings, About; version label at lower right.

## Two performance modes
- MELODY: each MIDI note plays a pitched guitar note; support note-off/release, velocity response, pitch bend, sustain and voice stealing. No chord-pattern substitution in this mode.
- PATTERN: trigger a selected guitar rhythm/pattern; synchronize to host tempo and transport; support MIDI keyswitch/CC selection and a clear stop/hold behavior. Do not label a pattern engine complete until real MIDI playback and host sync are tested.
- Switching modes must not reset the selected preset or tone controls unexpectedly.

## Functional requirements
- Preset browser must load exactly the shipped factory catalog and display category/name; 300 entries are required in the release bank.
- Bank controls must expose Electric, Acoustic and Hybrid availability honestly, with independent load/unload status.
- CPU/RAM indicators must report measured values or be removed until implemented; never show hard-coded demo numbers.
- All knobs, toggles, selectors, keyboard keys, preset navigation, save and browser actions must be wired to real state and parameter handling.
- Settings is a dedicated panel/window; main skin remains visually unchanged.

## Acceptance checks
- Compare the rendered plugin at 1402x1122 against the approved screenshot.
- Verify both modes by sending MIDI in FL Studio 20 (Windows 10 x64), including note-off, velocity, host tempo sync and transport stop.
- Verify all 300 presets can be enumerated, selected, saved/restored and produce valid parameter states.
- Verify no decorative control is presented as functional unless wired.
- Build the VST3 with Visual Studio 2022 x64, run pluginval strictness 5, then test in FL Studio 20. Record each result separately; CI success alone is not proof of DAW validation.
