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


## 0.1 Engine architecture lessons from the Electric Sunburst analysis

The supplied Electric Sunburst analysis describes a hybrid instrument: a multisample sound library, chord recognition, pattern skeletons adapted to harmony, articulation/transition handling, and a separate direct-play melody instrument. We should adopt the useful design principles, not reproduce Kontakt's full resource footprint or proprietary content.

### Adopt for our engine
- **Pattern skeleton + harmony adapter:** store original patterns as compact event data (timing, degree/interval, duration, articulation, accent). At playback, adapt events to the detected chord and selected voicing. This is substantially lighter than storing a separate full audio recording for every chord and every pattern.
- **String/position mapper in stages:** begin with a deterministic chord-tone/string mapper for supported guitar voicings. Add advanced fretboard-position selection only when the sample set contains the required notes and the CPU/memory cost is measured.
- **Separate Pattern and Melody execution paths, shared sample engine:** Pattern generates/adapts note events; Melody responds directly to MIDI. Both use the same sample cache, voice allocator, articulation map, and output/FX path to avoid duplicate engines and duplicate sample memory.
- **Chord recognition as a small state machine:** track held MIDI notes and note-off correctly; support common power chords, major/minor triads, sus and add-tone chords first. Handle chord changes with a configurable transition policy. Do not copy the overlap limitation described for Electric Sunburst; our implementation should avoid stale/overlapping chord state and stuck notes.
- **Pattern dynamics:** support both MIDI velocity response and a user Accent control. A compatibility option may let a pattern ignore input velocity and use Accent, but this must be explicit per preset/pattern rather than universal.
- **Humanization only when requested:** deterministic, bounded timing/velocity/strum variation with a low-cost default-off or subtle setting. Avoid random processing that changes timing unpredictably.
- **MIDI export as a later utility:** pattern-to-MIDI drag/export is useful, but lower priority than reliable playback and performance.

### Do not copy blindly
- Electric Sunburst's reported 13.6 GB uncompressed / 6.3 GB compressed sample-library size is disk-library size, not a direct measurement of RAM use. Do not infer its runtime memory footprint from those figures. Our RAM use must be measured independently.
- Multiple pickup/microphone layers, many velocity layers, round robins, fret noises and transition samples can increase disk size, preload memory, voice count and disk traffic. Include only layers that audibly improve the result; use selective preload/streaming and configurable quality tiers.
- Avoid one giant always-loaded instrument. Use on-demand bank/preset loading, shared sample IDs, bounded cache, and release unused sample pages safely outside the audio callback.
- Avoid per-pattern audio recordings for every chord. Prefer compact event skeletons mapped to shared multisamples; reserve recorded phrase audio for special one-shot fills only if it is licensed and memory-safe.
- Do not add a full automatic fretboard solver, elaborate chord vocabulary, song-link system, or many simultaneous mic/pickup paths in the first engine version. Start with common pop-punk harmony and predictable guitar voicings.

### Lightweight sample/voice plan
- Prefer a small, well-recorded core set of notes/strings and a few meaningful velocity/articulation layers over exhaustive redundant multisampling.
- Use streaming for long sustain/tail material and a small configurable preloaded attack region; verify disk-streaming behavior under low audio buffers and slower storage.
- Use a bounded shared sample cache with explicit memory budget and cache metrics; never let preset browsing silently grow memory without limit.
- Share decoded sample data across presets. Voice instances hold lightweight references and playback cursors, not copies of sample buffers.
- Implement voice stealing by priority (release voices first, then quiet/oldest voices) and cap simultaneous voices. Avoid needless release tails and duplicate notes.
- Keep FX modular and bypassed when inactive. Prefer a single efficient shared chain per plugin instance over per-voice heavy FX; use per-voice processing only where musically essential.

### First-release scope recommendation
1. A lean sample playback core with shared bank, bounded cache, preload/streaming, and voice limit.
2. Melody playback with reliable note-on/note-off, velocity and a small articulation set.
3. Pattern skeleton playback with host sync, common chord recognition, transposition/voicing and Accent.
4. A compact, efficient core FX chain.
5. Advanced fretboard mapping, extensive articulation transitions, extra pickup/mic layers, MIDI drag/export and Song Builder only after profiling shows sufficient headroom.

### Comparative performance rule
The product goal is to be lighter than Session Guitarist and Nexus 5 in both load time and runtime resource use, but this is a target to verify, not a claim already established. Benchmark all products on the same Windows 10 Enterprise 1809 x64 machine, FL Studio 20 version, sample rate, buffer size, project, preset, voice count and playback duration. Record cold/warm load time, idle/active CPU, peak/private working-set memory, disk reads, preset-switch latency and audio glitches. Compare equivalent musical output and quality settings; publish results only after repeatable measurements.


## 0.2 Product direction — preserve our visual identity and standalone VST3 advantage

The primary product direction is to preserve and refine the established POP PUNK GUITAR ULTIMATE visual style and the feature layout already defined in the project journal/specification. Engine improvements must support this identity, not replace it with a generic Kontakt-style interface.

Core product principles:
- **Standalone instrument:** the plug-in runs as its own VST3 instrument and must not require Kontakt 6, Kontakt Player, or a Kontakt library wrapper. This independence is a key product advantage.
- **Visual continuity:** retain the approved interface structure, visual language, controls, sections, preset browser, and switchable skin system. New features must fit the existing layout and remain accessible; do not remove established controls or redesign the interface without approval.
- **Useful upgrades only:** add a feature only when it improves sound quality, playability, workflow, reliability, or resource efficiency. Avoid feature inflation, duplicate controls, decorative processing, and complex options with little practical value.
- **Sound quality with a lean engine:** prioritize high-quality source samples, careful mapping, velocity response, clean note-off/release behavior, useful articulations, and a small efficient FX chain. Optimize the implementation rather than reducing sound quality indiscriminately.
- **Fast startup and loading:** initialize the DSP and essential state quickly; defer preset indexing, skin decoding, waveform generation, and nonessential asset work to safe background tasks. Load only the active preset's required sound resources.
- **Low memory and CPU:** shared sample assets, bounded cache/preload, configurable polyphony, efficient voice management, optional FX bypass, and strict real-time-safe audio processing remain mandatory.
- **Skin/UI isolation:** the selected skin changes the appearance of the complete interface without changing DSP behavior or increasing audio-thread workload. Load only the active skin's assets and release replaced skin resources safely.
- **No Kontakt dependency by design:** avoid architecture or packaging choices that make Kontakt a runtime requirement. Use a native VST3 codebase and independently distributable, license-cleared sound assets.

Priority order:
1. Preserve and complete the approved visual style and full functional interface.
2. Ensure the standalone VST3 architecture and reliable FL Studio 20 loading.
3. Deliver excellent core guitar sound and dependable Melody/Pattern playback.
4. Optimize load time, RAM, CPU, disk streaming and preset switching without audible degradation.
5. Add only proven, useful enhancements after measurement and regression testing.

Acceptance rule: the plug-in must be evaluated as a complete standalone product: visual/interface regression, sound/playability tests, VST3 host compatibility, and measured CPU/RAM/load-time benchmarks. Do not claim superiority to Kontakt-based products or Nexus 5 until comparable tests have been run on the same system and settings.


## 0.3 Instant-feel preset switching and sound loading — high priority

Preset selection and sound availability must feel immediate. Avoid the slow full-instrument/sample loading workflow associated with large Kontakt-hosted libraries. Fast preset switching is a core product requirement, not a later UI polish item.

- Separate **preset state loading** from **sample asset loading**. A preset should first apply its small parameter/state data (engine mode, sound IDs, articulations, FX, pattern references and UI state) without rebuilding or reloading the entire instrument.
- Keep a lightweight in-memory index of preset metadata and stable sample IDs. The browser must not scan, decode, or preload the complete Sounds Bank whenever it opens or a preset is selected.
- Reuse already loaded/decoded samples across presets. If two presets reference the same sample asset, switching between them must not load a second copy.
- Preload only the next preset's required attack/transient regions and essential mapping data where practical. Stream long sustain/tail regions and load uncommon articulations/round robins on demand, with safe fallback behavior if an asset is not yet ready.
- Use asynchronous background loading with progress/readiness states. Never perform file I/O, sample decoding, blocking waits, or memory allocation on the real-time audio thread. Audio playback must continue safely while the next preset is prepared.
- Use a small configurable **preload-ahead** strategy for likely next presets or recently used presets, bounded by a strict memory budget. Make cache eviction deterministic (least-recently-used or equivalent) and do not evict assets currently referenced by active voices.
- On preset switch, crossfade or otherwise transition safely when required to avoid clicks; do not interrupt active notes unexpectedly. Define whether existing voices retain their old preset resources until release or are explicitly transitioned.
- Provide a clear loading state only when genuinely needed. Prefer instant switching for presets whose samples are already cached; avoid unnecessary spinners or artificial delays.
- Record separate timings for browser open, preset-state apply, first playable note, full required-asset readiness, and warm-cache preset switch. Optimize for perceived time-to-first-sound as well as total load time.
- Acceptance tests must include cold-cache and warm-cache switching, rapid repeated preset changes, switching while notes are held, large bank browsing, missing/slow disk assets, low audio buffer sizes, and memory-budget enforcement.

**Priority:** P0/P1 alongside core audio reliability and shared sample-bank infrastructure. Do not claim instant loading until measured on the target Windows 10 Enterprise 1809 x64 / FL Studio 20 setup.

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
