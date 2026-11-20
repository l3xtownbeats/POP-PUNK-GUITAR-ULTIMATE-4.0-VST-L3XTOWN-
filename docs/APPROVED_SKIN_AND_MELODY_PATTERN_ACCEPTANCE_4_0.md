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
## AutoChords — expanded PPGU scale note list

AutoChords is intended to be more flexible than a small fixed chord-choice list.

### AutoChords ON
- The user selects a key/root and scale (at minimum Major and Minor; Chromatic remains available as a mode).
- Display the complete ordered set of notes in the selected scale, with all scale degrees available for selection. Do not show only a few preset chords.
- For a seven-note diatonic scale, expose all seven degrees and their resulting chord options/qualities (including the diminished degree where applicable).
- The selected degree/chord must provide the corresponding MIDI note set to the Pattern performance engine; preserve the selected Pattern rhythm/loop and its timing.
- Use context-appropriate enharmonic spelling: e.g. Bb in flat-oriented keys and A# in sharp-oriented keys. The spelling changes the display name, not the MIDI pitch.

### AutoChords OFF
- PATTERN accepts the full 12-semitone chromatic input range without scale filtering or automatic remapping.
- Every MIDI pitch class remains playable: C, C#/Db, D, D#/Eb, E, F, F#/Gb, G, G#/Ab, A, A#/Bb, B.
- Display a consistent sharp/flat spelling appropriate to the selected key context; both names must resolve to the same MIDI note where enharmonic.
- Disabling AutoChords must not disable Pattern playback, keyswitches, or the full MIDI note range.

### Acceptance
- Switching key/scale refreshes the complete note/degree list and its chord choices.
- Major and Minor scales are checked across all 12 roots; the UI displays the expected notes and diatonic chord qualities.
- AutoChords OFF accepts all 12 pitch classes in Pattern and does not quantize/filter notes to the selected key.
- Verify MIDI output and Pattern playback in FL Studio 20; unit tests for scale-note and diatonic-chord generation do not replace host testing.
- AutoChords is not complete until the visible selector/list, MIDI generation, Pattern integration, state recall and tests are all implemented.

## Pattern loop acceptance — full source or extracted fragment

- A complete original WAV may be used as a Pattern performance when it contains a stable repeating musical drawing and its loop transition is verified clean/seamless.
- A full-source loop may use sample range 0 .. frame_count; a shorter extracted loop is optional when the source also contains changing sections, fills, intros, or outros that do not belong in the repeated pattern.
- Retain exact sample boundaries, beat grid, attacks/strokes, harmonic/chord events, articulation data and tempo/meter evidence or explicit UNKNOWN values.
- Never extract fragments solely because the source is a complete WAV. Preserve the whole source when it is already a suitable repeating performance.
- A full loop does not remove the need for reviewed note/chord zones when the player is expected to change harmony interactively; do not pitch-shift one full-band recording as a substitute for those zones.
