# PPGU 5.0 — Commercial Instrument Product Requirements

Date: 2026-11-11
Status: Product specification / implementation gates. This document is not a claim that every item is already implemented.

## Product position

POP PUNK GUITAR ULTIMATE is intended to be a premium, fast-loading Windows x64 VST3 electric-guitar instrument for FL Studio 20 and compatible hosts. Its competitive goal is to exceed the day-to-day playability, browsing speed, sound quality and workflow convenience of Native Instruments Session Guitarist Electric Sunburst Deluxe / Electric Vintage Deluxe and Ample Sound Eclipse. Those products are quality and UX benchmarks only; no proprietary recordings, code, patterns, UI assets or presets may be copied.

The instrument's identity is a California pop-punk guitar collection: tight, punchy, bright but not harsh, mix-ready rhythm guitars, realistic palm-muted/open transitions, articulate leads and convincing acoustic contrast. A large SoundBank may reach 10 GB when its content justifies the size; do not fill space with duplicates or low-value files.

## Five product pillars

### 1. Fast startup and fast preset switching

- Show the plugin UI immediately; do not synchronously scan or decode the whole SoundBank during editor construction.
- Keep a compact library manifest/index available at startup. Build or refresh indexes on background workers.
- Preset selection should resolve a stable libraryId + mapId + articulation/profile reference, not copy or duplicate audio.
- Preload only the selected preset's attack/transient data and a bounded working set; stream long samples/tails with a worker and page cache.
- Keep a small LRU cache of recently used maps and preview assets. Switching between presets sharing the same map should reuse decoded data.
- Decode/validate newly imported audio off the audio thread. Publish a fully validated bank atomically; retain the previous active bank if import fails.
- No disk I/O, sample decoding, unbounded allocation, blocking locks or library scanning in processBlock.
- Provide visible loading/progress/status and never report a preset as loaded until its map resolves.
- Measure cold startup, warm startup, first-note latency, preset-switch latency, peak memory and CPU in release tests. Set targets after baseline profiling rather than inventing unmeasured figures.

### 2. Large high-quality SoundBank

- Factory SoundBank target ceiling: 10 GB. Size is a maximum, not a quota.
- Prioritize multi-velocity, multi-round-robin, chromatically mapped electric guitar recordings with real open, palm-muted and muted articulations, multiple pick attacks, consistent tuning and edited tails.
- Include clean DI and amp-coloured sources, crunchy rhythm, high-gain rhythm, lead/sustain, harmonics where recorded, and steel/nylon acoustic contrast.
- Use recorded chord/strum performances for PATTERN where licensed and available. A single-note sample transposed into a chord is not a substitute for a recorded strum.
- Every sample zone needs root key, key range, velocity range, articulation, round-robin group, gain/tuning, sample rate, channel count, source/license and checksum metadata.
- Keep commercial stems and proprietary sample packs out unless redistribution rights explicitly cover embedding them in a commercial VST.
- Imported user libraries are external to the factory bank and are not copied into the factory installer unless the user explicitly exports a redistributable pack.

### 3. Two playable modes

**MELODY**
- One MIDI key = one chromatic guitar note at the corresponding pitch.
- Real multisample key/velocity mapping, note-off/release, round robin where alternate takes exist, configurable voices/polyphony and voice stealing.
- Articulation selection: Open, Palm Mute, Muted/Dead, Harmonic/Flageolet, Tremolo, Clean, Crunch, Distortion, Acoustic and other mapped content.
- Velocity can select recorded layers and optionally cross a configurable palm-mute threshold. Do not claim a recorded articulation when only DSP shaping is used.

**PATTERN**
- One MIDI key or incoming MIDI chord controls a tempo-synchronised guitar performance pattern in the selected key/harmony.
- Pattern events follow host tempo/transport and incoming chord changes; note-off, transport stop, seek and loop jumps must cancel queued events correctly.
- Support down/up strokes, palm-muted/open transitions, rests, accents, arpeggios, octave hooks, stop-start hits and half-time/double-time feels.
- AUTOCHORDS is optional and PATTERN-only. With AUTOCHORDS OFF, play the actual incoming note/chord; the Pattern engine remains active.
- Where real recorded chord/strum samples exist, use them. Otherwise use a clearly documented note-event performance engine and do not market it as recorded strumming.

### 4. Import Guitar — automatic bank onboarding

One clearly visible **IMPORT GUITAR** action should support a folder or ZIP bank:
1. Select folder/ZIP; show file count and estimated unpacked size.
2. Safely inspect archive paths, reject traversal/symlinks and enforce configurable extraction limits.
3. Detect SFZ/SF2/WAV and common manifest formats; parse supported maps and verify every referenced file.
4. If no map exists, offer conservative auto-mapping suggestions from filenames/metadata; ambiguous notes and articulations require user confirmation.
5. Detect duplicate files by SHA-256 and avoid duplicate storage/decoding.
6. Validate license/provenance metadata and mark unknown terms as License Review; never imply the user has redistribution rights.
7. Generate a manifest, searchable index, waveform/preview data and sample map in the background.
8. Run preview audition and MIDI key test; publish the new library atomically only after validation.
9. Make the new bank and its presets immediately available in the unified browser, without restarting the plugin where host-safe.
10. Provide rescan, remove, missing-file repair and clear-cache actions. Never delete unrelated libraries or user presets during an import/update.

Supported user-facing states: Scanning, Mapping Needed, License Review, Ready, Missing Samples, Invalid, Duplicate/Already Installed.

### 5. Live Note / Chord display

A persistent high-contrast **LIVE INPUT** display on the main panel must:
- Show each currently held MIDI note using note name and octave.
- When notes form a confidently recognised chord, show chord root, quality and inversion/bass note where meaningful (e.g. C, Cm, C5, Csus4, Cdim, Caug, C7, Cmaj7, Cm7, C/E).
- Support common pop-punk power-chord voicings and octave duplicates.
- If notes are incomplete or ambiguous, show the actual note names and do not guess a chord.
- Update immediately on MIDI note-on/note-off and clear correctly on all-notes-off, panic, transport changes and editor reopen.
- MELODY displays the notes actually held; never auto-harmonise them.
- PATTERN + AUTOCHORDS ON displays the effective generated harmony; PATTERN + AUTOCHORDS OFF displays the actual incoming notes/chord.
- Audio-to-UI state transfer must be preallocated and lock-free/atomic. Chord naming, string formatting and UI repaint remain off the audio thread.
- Test single notes, inversions, duplicate octaves, power chords, major/minor/sus/diminished/augmented/seventh chords, ambiguous voicings and fast note changes.

## Browser and workflow expectations

- One unified browser with separate MELODY sound character and STYLE/performance filters, plus independent PUNK GROUP and USER banks.
- Search by preset name, group, track reference, articulation, playing technique, sample profile and tags.
- A preset is a lightweight recipe pointing to a sound map and FX snapshot. It must not contain duplicate WAV payloads.
- Preview before loading: audition the selected mapped sample or performance preview, then load the complete preset.
- User presets and imported banks must survive plugin updates and factory preset regeneration.
- Pattern and Melody can share a named style identity while retaining mode-specific playable content.

## Release acceptance gates

- Windows 10/11 x64 VST3 build, pluginval and CTest pass on the exact release commit.
- FL Studio 20 x64 manual tests: MIDI input, Piano Roll, automation, preset recall, import, editor resize, transport and note-off.
- Cold/warm startup and preset-switch timings measured on a documented test PC.
- No audio-thread disk access or blocking operations under stress.
- Verify all factory presets resolve to valid sound maps; separate playable presets from incomplete style-study metadata.
- Verify imported-bank safety, duplicate handling, corruption recovery and license status.
- A commercial quality claim requires blind listening and documented audio QA; a large preset count alone is not proof of quality.

## Current implementation audit (2026-11-11)

Confirmed in checked-in source:
- An asynchronous SFZ load API and asynchronous Import Guitar folder API exist in the processor.
- A sample-load worker pool is present; this alone does not prove non-blocking sample streaming in the rendering path.
- A 10-skin palette selector, Melody/Pattern controls, unified factory catalog browser and FX Studio exist.
- The preset browser embeds the catalog as JUCE BinaryData; changes to the source JSON must be reflected by the build's generated BinaryData.
- The MIDI keyboard visual exists.

Known incomplete or unverified from the checked-in editor/source:
- The MIDI and MULTICORE buttons are explicitly disabled with tooltips saying the features are not connected/enabled.
- The main editor currently has no dedicated live note/chord display component.
- The product specification for live note/chord recognition exists in the implementation log, but the display and processor-to-UI snapshot are not yet confirmed implemented.
- Fast startup, first-note latency, cache hit rate and preset-switch time have not been benchmarked.
- Full import format coverage, automatic mapping quality, user-facing license workflow and seamless post-import browser refresh require end-to-end verification.
- Factory catalogue definitions and generated preset files are not automatically proof of fully playable presets; each must resolve to valid mapped audio and pass host audition.
