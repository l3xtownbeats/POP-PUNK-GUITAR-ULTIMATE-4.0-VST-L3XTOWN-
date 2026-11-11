# POP PUNK GUITAR ULTIMATE 5.0 — Product UI and Preset Logic Contract

Status: authoritative implementation specification for branch 5.0.
Target: Windows x64 VST3, JUCE, FL Studio 20.
This document defines expected behavior; it is not a claim that every item is already implemented.

## 1. Unified PRESET BROWSER

The browser is one connected system, not unrelated preset dropdowns. It must combine:
- Preset identity / punk-group profile
- MELODY sound character
- STYLE playing articulation
- Sound Bank instrument/sample mapping
- FX chain and parameter snapshot

The selected combination must be visible as a concise summary and must be recallable with the DAW project/state.

### MELODY — sound character
MELODY describes the timbre / articulation source selected for note-by-note playing or chord construction. Initial taxonomy:
- Clean / Open Clean
- Acoustic
- Muted
- Palm Mute (where sample articulation exists)
- Flageolet / Harmonics
- Tremolo Clean
- Crunch
- Distortion
- Overdrive
- Lead Sustain
- Chorus Clean
- Octave / layered lead
- additional sound characters only when backed by an actual playable sample mapping or defined DSP chain.

MELODY mode: each incoming MIDI key plays its corresponding note. The chosen MELODY sound character is the base tone. The STYLE layer modifies the playing articulation/voicing without replacing the chosen sound character.

Examples:
- MELODY=Acoustic + STYLE=Lead -> acoustic note source with lead-like phrasing/longer sustain, if mapped.
- MELODY=Clean + STYLE=Lead -> clean electric lead.
- MELODY=Muted + STYLE=Lead -> muted lead, not a generic full-open guitar.
- MELODY=Clean + STYLE=Palm Mute -> palm-muted clean sound, only where the sample bank has a mute region or an explicitly implemented DSP substitute.

### STYLE — playing method
STYLE describes how notes/chords are performed, not the base tone:
- Lead Guitar
- Solo Guitar
- Palm Mute
- Open Chords
- Power Chords
- Rhythm Downstrokes
- Rhythm Upstrokes / Ska
- Alternate Picking
- Arpeggio
- Stop/Start
- Sustained / Legato
- Octave Lead
- other styles from the approved band/group catalogue.

MELODY and STYLE are orthogonal selectors. Choosing one must not silently reset the other. If a requested articulation is unavailable in the loaded bank, show an explicit unavailable/missing-articulation state; never silently pretend a different sample was used.

## 2. PATTERN mode and band/group presets

PATTERN is performance behavior: MIDI keys trigger chord progressions, voicings and rhythmic patterns. A group preset (e.g. Blink-182 — Lead) supplies its authored default sound character, pattern/groove, articulation, and FX snapshot.

- Selecting a group preset loads its own authored default configuration.
- If STYLE is left at AUTO / GROUP DEFAULT, the group preset's authored playing style is used unchanged.
- If the user explicitly chooses a STYLE override (e.g. Palm Mute), that articulation overrides the group default while preserving the group preset identity, groove/pattern, sound character, and FX unless the preset explicitly marks a dependent parameter.
- Selecting a group preset must not force MELODY/PATTERN mode. The user chooses the performance mode independently.
- The same group preset identity must remain selectable in both modes; behavior adapts to MELODY vs PATTERN.
- The preset browser must show the difference between group defaults and user overrides.

Example: Blink-182 — Lead + AUTO uses the authored Blink-182 Lead pattern/tone. Blink-182 — Lead + STYLE=Palm Mute plays the same group rhythm with palm-muted articulation.

## 3. Sound Bank and FX integration

A preset combination resolves to:
1. Sound Bank instrument / sample map and articulation availability.
2. MELODY sound character.
3. STYLE articulation / pattern behavior.
4. FX chain state and parameter values.

Imported source samples remain in their original format and are stored in user library folders with metadata, IDs, checksums, note/velocity mappings and license provenance. SFZ/WAV is the first supported import path. Do not advertise unsupported formats as working. Missing/unsupported mappings must be reported.

FX preset snapshots must use existing APVTS parameters and must restore host automation-compatible parameter values. No UI-only toggles.

## 4. Main interface controls

- PRESET BROWSER: unified selector with search and filters for group, MELODY and STYLE.
- IMPORT SOUNDS: open sound import/library workflow; current supported formats must be stated truthfully.
- MELODY: single-note / chord note performance.
- PATTERN: chord progressions and rhythmic performance patterns; AUTOCHORDS is available only here.
- FX STUDIO: separate full FX editor with real APVTS-linked controls.
- SKINS: one-button selector for complete interface themes; theme affects main UI, controls, FX Studio and keyboard. Theme selection persists per plugin instance.
- MIDI: separate piano-roll window for MIDI editing, import and export. Keep disabled until functional; do not present a dead button as working.
- DOUBLE: doubles/layers the guitar for width and density; must be phase-safe and controllable, not merely a cosmetic toggle.
- PROCESSOR: GOOD / NORMAL / NIGHT / ECO resource profiles. Each profile must have a documented, measurable behavior; do not label voice-count-only changes as full DSP quality modes.
- MULTICORE: enable only when real worker-thread/background processing is implemented safely. Never parallelize the real-time audio callback unsafely.

## 5. Interface visual direction

Use the approved original generated interface artwork from the 4.0 repository as the source of visual direction. Premium dark graphite / black surfaces, metallic details, controlled pink accent, realistic guitar hero art, layered depth, legible labels, polished rotary controls and consistent FX Studio. No cheap generic gradient/placeholder backgrounds and no vector guitar silhouette as the primary hero artwork.

Ten switchable skins must be coherent high-quality variants of the approved design language. Changing skin changes the complete shell and all relevant controls, not only accent colours. Keep controls, labels, spacing, and functional layout consistent between skins.

## 6. Required verification before calling the product complete

- Build Windows x64 Release VST3.
- CTest/regression suite passes.
- VST3 bundle discovery and pluginval strictness level 5 pass.
- Verify all parameter attachments, state save/restore and host automation.
- Verify preset combinations in MELODY and PATTERN, including AUTO vs explicit STYLE override.
- Verify sample/articulation availability reporting.
- Verify FX Studio and all theme changes, including persisted theme state.
- Verify MIDI window, import/export, DOUBLE, processor profiles and MULTICORE only when implemented.
- Manually load in FL Studio 20, play notes/chords, test MIDI input, presets, FX, state recall, CPU and audio output.
- Report automated tests separately from manual FL Studio host testing. Never claim the latter passed without a recorded test.

## 7. Acceptance principle

A visible control is not complete unless it performs its named function. Unsupported features must be disabled and clearly identified, not presented as finished. A successful compile is not equivalent to a commercially complete instrument.
