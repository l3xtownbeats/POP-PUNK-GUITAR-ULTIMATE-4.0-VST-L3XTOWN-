# POP PUNK GUITAR ULTIMATE — Curated Feature Specification

Status: design specification; implementation and audio validation are still pending.
Target: Windows x64 VST3, FL Studio 20.

## Product rule
Use only features that materially improve playable pop-punk guitar production. Build a large, expandable library without sacrificing quality, consistency, performance, or legal provenance. This is an original design; do not copy proprietary Kontakt instruments, pattern data, samples, artwork, or factory presets.


## 0. Performance-first design — non-negotiable

Primary product goal: the instrument must remain lightweight both at plug-in startup/load time and during real-time playback. Target host environment: Windows 10 Enterprise 1809 x64 and FL Studio 20. Feature count must never take priority over low CPU/RAM use, fast startup, and reliable operation on this environment.

Design requirements:
- Load only the selected preset's required sample mappings and metadata; avoid scanning/decoding the entire library on the audio thread. Build a lightweight preset/sample index and cache it safely.
- Stream long samples from disk where appropriate, while keeping short attack/transient material and active voice data ready in memory. Provide bounded preload settings and sensible defaults.
- Use shared sample assets and references, not per-preset copies. Avoid unnecessary duplicate sample buffers.
- Provide a user-adjustable Voices/polyphony limit with a conservative default, voice stealing, and predictable CPU/memory ceilings.
- Keep optional FX bypassed/off by default where practical; allow expensive modules and oversampling to be disabled or quality-scaled. Do not run inactive processors.
- Avoid background work that competes with audio playback. Any indexing, waveform generation, preset scanning, or sample preparation must happen off the audio thread and report progress without blocking playback.
- Audio callback must be real-time safe: no heap allocation/deallocation, mutex waits, file I/O, logging, or unbounded loops. Use preallocated buffers, lock-free or bounded communication, parameter smoothing, and denormal protection.
- Keep UI rendering and skin loading separate from DSP. High-resolution skins must not inflate audio memory or delay plug-in instantiation; load only the active theme and release unused image assets.
- Make multiprocessor use selective and measurable. Do not create worker threads per voice or add parallelism that costs more than it saves; prioritize deterministic audio-thread DSP and bounded background tasks.
- Provide performance/quality options (for example Eco/Normal/High) only where they produce measurable trade-offs. Eco mode should reduce preload, oversampling, FX quality, or voice limits without changing project compatibility.
- Measure cold and warm load time, idle CPU, active CPU at representative voice counts, peak memory, preset-switch latency, and behavior under low buffer sizes.
- Ensure installer/package size stays reasonable: avoid bundling unused sample formats, duplicate assets, unneeded artwork, and development files.

Acceptance targets are to be established by measurement on the specified Windows/FL Studio setup; do not invent numeric performance claims before benchmarking. Any optimization must preserve audio correctness and avoid clicks, stuck notes, timing drift, or missing samples.

## 1. One unified preset browser
Keep one browser for the entire instrument. Each preset is tagged with:
- Sound character: Clean, Crunch, Lead, Distortion, Acoustic, Muted, Ambient.
- FX character: Chorus, Flanger, Phaser, Delay, Reverb (tags may overlap).
- Artist/genre inspiration (descriptive tags only; no implication of endorsement).
- Compatible engine: Melody, Pattern, or Both.

Selecting a preset loads its instrument/sound-bank mapping and complete parameter state. The browser must not fork into separate Melody and Pattern preset lists. Search, favorites, category filters, and clear preset names are preferred over excessive nested menus.

## 2. Two focused performance engines
### Melody Engine
Direct MIDI note performance with velocity layers, note-off/release handling, configurable polyphony, and supported articulations (for example open/muted) only where the selected sample bank actually provides them. Add legato/slide transitions only when suitable transition samples or a credible synthesis method exist.

### Pattern Engine — large, expandable original library
Use the 237 patterns in NI Electric Sunburst Deluxe as a reference point for breadth, not as a ceiling or source material. The goal is a substantially larger library that can keep growing through versioned content packs. There is no arbitrary maximum pattern count: add patterns whenever they are musically distinct, useful, properly tagged, tested, and do not merely duplicate existing variations.

Build the library in families:
- Power chords: straight eighths, driving eighths, sixteenths, syncopated, stop-start, octave movement.
- Pop-punk strumming: downstrokes, alternate strums, fast 8ths/16ths, accents, muted strums.
- Palm-muted riffs: tight chugs, gallops, double-time, rests, dynamic builds.
- Arpeggios and picked patterns: clean electric, acoustic, broken chords, string-crossing figures.
- Lead phrases: melodic hooks, octave lines, picked riffs, harmonized-style phrases where supported.
- Song sections: intro, verse, pre-chorus, chorus, bridge, breakdown, transitions, outro, fills and endings.
- Feel and meter variants: straight, half-time, double-time, swing only where stylistically appropriate, plus supported time signatures.
- Acoustic patterns: strumming and picking families with their own articulations and dynamics.

Every pattern has metadata: family, feel, meter, subdivision, articulation, compatible sound tags, energy, difficulty, phrase length, variation group, and supported tempo range. Provide meaningful variations (A/B/C/D and more when useful), not artificial count inflation. Patterns must transpose to incoming chords, follow host tempo and transport, support MIDI sync, and stop/retrigger predictably. Include controlled humanization (timing/velocity/strum) with bounded ranges; tight pop-punk timing remains the default.

Library expansion workflow:
1. Create and curate original pattern ideas.
2. Deduplicate by musical behavior, not just filename.
3. Test every pattern across chord types, root notes, tempo extremes, and both engines where compatible.
4. Check transitions, note-off behavior, timing, and audible artifacts.
5. Tag and index patterns for fast unified-browser search.
6. Ship new pattern packs without breaking older presets or user projects.

## 3. Sound-bank architecture
Keep audio assets in a shared Sounds Bank and reference them by stable IDs from presets and patterns; never duplicate the same WAV for each preset. Support sample metadata for note range, root key, velocity layers, round robins, articulation, sample rate, and license/source provenance.

Pickup blend and microphone detail are optional capabilities, not fake controls: expose them only when the library contains separately recorded bridge/neck pickup or microphone signals. Otherwise omit or disable those controls.

## 4. Focused FX chain
Implement a lightweight, modular chain with bypass and preset recall:
1. Stomp: overdrive/distortion.
2. Amp/cab simulation (only if a suitable efficient model/IR workflow is available).
3. Modulation: chorus, flanger, phaser.
4. Studio: compressor and EQ.
5. Space: tempo-synced delay and reverb.

Use sensible defaults, parameter smoothing, denormal protection, and no allocation/locking in the audio callback. Allow optional modules to be disabled to reduce CPU. Avoid implementing multiple low-quality amp/cab models just to inflate the feature list.

## 5. Delivery priorities
P0 — inspect existing project and preserve working code; document current architecture; verify licensing/provenance of every bundled sample.
P1 — unified preset data model/browser integration and shared sample-bank references.
P2 — reliable Melody Engine playback and Pattern Engine chord/tempo/MIDI behavior.
P3 — core FX chain, preset state recall, and parameter automation.
P4 — build the initial high-quality pattern families, then expand the library continuously with validation and content-pack versioning.
P5 — variations, restrained humanization, favorites/search polish, and optional Song Builder only after the engines are stable.

## 6. Acceptance tests
- VST3 loads and unloads in FL Studio 20 x64 without crash.
- MIDI notes, note-offs, chords, sustain, velocity, and polyphony behave predictably.
- Pattern playback follows host tempo and transport; chord changes update harmony without stuck notes.
- Every shipped pattern passes timing, chord-transposition, transition, and artifact checks.
- Preset changes restore sound, engine, and FX state consistently.
- No audio-thread memory allocation, blocking file I/O, or unbounded processing.
- CPU and memory are measured at representative polyphony and sample rates.
- Bundled samples have documented redistribution rights; unverified material is not packaged.

## Project journal

All future approved changes, fixes, implementation notes, and test results must be appended to [docs/CHANGELOG.md](CHANGELOG.md). The journal distinguishes planned/specification work from implemented and tested work.

## Explicitly deferred
Do not claim a pattern count until the patterns exist and pass validation. Do not claim true pickup blending, microphone mixing, legato transitions, or finished amp/cab models until real content/implementation and tests exist. Do not add a Song Builder before both performance engines are dependable.
