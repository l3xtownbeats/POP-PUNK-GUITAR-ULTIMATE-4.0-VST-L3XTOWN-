# POP PUNK GUITAR ULTIMATE — Curated Feature Specification

Status: design specification; implementation and audio validation are still pending.
Target: Windows x64 VST3, FL Studio 20.

## Product rule
Use only features that materially improve playable pop-punk guitar production. Prefer a small, reliable, low-CPU implementation over a large collection of shallow or duplicated features. This is an original design; do not copy proprietary Kontakt instruments, pattern data, samples, artwork, or factory presets.

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

### Pattern Engine
Chord-aware phrase playback driven by MIDI input. Start with a curated, well-tested core set rather than promising a fixed large count before content exists:
- Power-chord rhythms and pop-punk eighth/sixteenth-note strums.
- Palm-muted grooves and gallops.
- Arpeggios and picked patterns.
- Intro, verse, chorus, bridge, and outro phrase roles.
- A small number of reverse patterns where musically useful.

Patterns must transpose to incoming chords, follow host tempo, support MIDI sync, and stop/retrigger predictably. Include variation A/B and controlled humanization (timing/velocity/strum) with bounded ranges. Do not randomize so much that tight pop-punk timing is lost.

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
P4 — variations, restrained humanization, favorites/search polish, and optional Song Builder only after the engines are stable.

## 6. Acceptance tests
- VST3 loads and unloads in FL Studio 20 x64 without crash.
- MIDI notes, note-offs, chords, sustain, velocity, and polyphony behave predictably.
- Pattern playback follows host tempo and transport; chord changes update harmony without stuck notes.
- Preset changes restore sound, engine, and FX state consistently.
- No audio-thread memory allocation, blocking file I/O, or unbounded processing.
- CPU and memory are measured at representative polyphony and sample rates.
- Bundled samples have documented redistribution rights; unverified material is not packaged.

## Explicitly deferred
Do not claim a 237+ pattern library, true pickup blending, microphone mixing, legato transitions, or finished amp/cab models until real content/implementation and tests exist. Do not add a Song Builder before both performance engines are dependable.
