# POP PUNK GUITAR ULTIMATE 4.0
## Product Profile & Technical Specifications
**L3XTOWN · 2026-11-11**

## Product overview
POP PUNK GUITAR ULTIMATE (PPGU) is an independent native Windows x64 VST3 guitar sampler/performance instrument for pop-punk, skate-punk, melodic punk and alternative rock. Its product design combines direct chromatic guitar performance (MELODY), chord-driven rhythm performance (PATTERN), a shared preset browser, a Sounds Bank architecture and a lightweight sample streaming engine.

It is designed for FL Studio 20 x64 and Windows 10 x64 (build 1809+). It is built with JUCE 7.0.12, C++20, CMake and Visual Studio 2022/MSVC v143.

## Product identity
**Positioning:** a focused pop-punk guitar instrument, independent of Kontakt.  
**Core promise:** fast access to guitar sounds, playable riffs and hooks, low idle overhead and a clean workflow.  
**Visual reference:** approved `Cover Interface.jpg`, black/charcoal control surface, magenta accents, guitar/concert artwork, Melody/Pattern, Amp & Tone, FX Chain, Playback and keyboard sections.

## Playing modes
### MELODY
Direct MIDI note-to-note performance. One MIDI key should trigger its mapped chromatic guitar note. Target articulations include picked/open, palm-muted, dead note, harmonics/flageolet, tremolo, acoustic, crunch and distortion. Velocity layers, round robin, release behavior and legato/slide support depend on the mapped recordings.

### PATTERN
Incoming MIDI chords define harmony and voicing; the selected pattern provides the rhythmic performance, synchronized to host tempo/transport. Target families include eighth-note strumming, downstrokes, alternate picking, palm-muted verse grooves, open chorus, arpeggios, octave hooks, stop-start riffs and acoustic strums. The scheduler exists in source, but the full authored pattern library and recorded performance bank are not yet release-complete.

### Unified preset workflow
The intended browser uses one artist/style identity with mode-aware entries and sound-character filters. A preset definition is not a finished playable preset until its sample profile is resolved, full state is applied, first note plays correctly and regression/host checks pass.

## Factory presets
The primary source is `presets/factory_bank_300.json`: 300 definitions, split into 150 Melody and 150 Pattern, with 21 style families:
Blink-182; Sum 41; Simple Plan; New Found Glory; Good Charlotte; Mest; Angels & Airwaves; The All-American Rejects; MGK; Avril Lavigne; The Offspring; Green Day; Bowling for Soup; Sugarcult; All Time Low; Fall Out Boy; Yellowcard; My Chemical Romance; Less Than Jake; Fenix TX; Midtown.

`presets/factory_preset_bank_v1.json` is a previous 300-entry catalog schema, not an additional 300 unique presets. Do not advertise 600 by adding both files. Definitions still require complete sample/profile resolution and runtime validation.

## Sounds Bank
The CI source pipeline currently packages five source families:
- Meta Open Sound Steel Guitar — 375 WAV, CC0 1.0.
- Meta Open Sound Nylon Guitar — 348 WAV, CC0 1.0.
- FreePats FSBS Clean #1 — CC0 1.0.
- FreePats FSBS Distorted #1 — CC0 1.0.
- FreePats FSBS Distorted #2 — CC0 1.0.

Meta subtotal: 723 WAV. The exact FreePats file count, size and checksums must be read from the manifest in the produced artifact. Additional candidate banks are documented separately and are not counted as already packaged: Karoryfer Emilyguitar, Shinyguitar, Black And Green Guitars, FreePats Direct DI, Clean #2/Jazz and Spanish Classical Guitar.

Factory and user libraries are separate. Each bank should retain source attribution, license, SHA-256, SFZ maps and metadata. Presets reference stable library/profile IDs; samples are not duplicated per preset.

## Native Instruments Session Guitarist compatibility boundary
Target references include Native Instruments Session Guitarist – Electric Sunburst Deluxe and Electric Vintage/Vintage Deluxe as sound and performance benchmarks. The plugin should support original user-created recordings and third-party WAV/SFZ libraries whose terms allow local sample-based use. It must not extract, decrypt, repackage or redistribute protected NI Kontakt samples or pattern content from NKI/NKX/NKC libraries. Native Instruments' EULA permits use of its content in musical productions but restricts turning the samples/instruments into another sound library or sample-based product without prior permission.

Supported practical workflow:
1. Use Electric Sunburst/Vintage Deluxe as a separate Kontakt instrument in FL Studio, on its own channel, and mix it alongside PPGU.
2. For PPGU's own Sounds Bank, import original guitar recordings or appropriately licensed WAV/SFZ sources.
3. Create similar functional tone families through original multisamples, articulations, velocity layers and PPGU's own FX chain, without copying NI sample content.
4. If a user owns independent recordings with clear rights, import them through the user-bank workflow.

References: NI product page https://www.native-instruments.com/products/session-guitarist-electric-sunburst-deluxe ; NI EULA https://www.native-instruments.com/de/pages/end-user-license-agreement .

## Technical specification

| Parameter | Current target / source-confirmed status |
|---|---|
| Product | Native guitar sampler / performance instrument |
| Plugin format | VST3 instrument, Windows x64 |
| OS | Windows 10 x64, build 1809+ target |
| DAW | FL Studio 20 x64 target |
| Framework | JUCE 7.0.12 |
| Language | C++20 |
| Build | CMake, Visual Studio 2022 / MSVC v143 |
| Sample map | SFZ |
| Audio assets | WAV |
| Output | Stereo |
| MIDI | Notes, velocity, channels; chord input in Pattern mode |
| Voice ceiling | Up to 64 voices in current design/source |
| Factory preset definitions | 300 (150 + 150) |
| Style families | 21 |
| Factory sound sources | 5 source families in CI packaging |
| Product package | VST3 + samples + presets + license/checksum manifests |
| Package size ceiling | 20 GB product target; not a RAM limit |
| UI | Approved dark hardware-panel visual direction |

## Engine and performance
The engine uses SFZ/WAV-oriented sample loading, zone selection, bounded voice allocation, background streaming and a bounded page cache. A semaphore-driven worker is intended to sleep while idle instead of polling every millisecond. The compressor curve was simplified to reduce repeated per-sample dB conversions.

Performance priorities:
- fast plugin initialization and first playable note;
- fast preset switching without loading the full bank;
- background indexing and sample preparation;
- bounded memory and cache;
- no disk I/O, blocking locks or unbounded allocations in audio callback;
- no changes to current cache/root/snapshot limits without stress tests.

No hard CPU/RAM/latency claims are made without target-machine measurements.

## FX and controls
Current source path includes drive/distortion, chorus, stereo delay and reverb stages. APVTS exposes the plugin parameter/state model, including output, polyphony, dynamics/velocity, palm-mute threshold and tone/performance controls. Additional requested effects and controls (flanger/phaser, amp/cab quality path, pickup blend, full articulation switching, advanced audition and complete preset-state mapping) require feature-specific integration and validation before they can be marked complete.

## Skins and interface
The approved interface artwork is `Cover Interface.jpg`. A complete runtime theme manager with multiple installable skins, asset validation, full palette/layout application and safe state preservation is still a development requirement, not a confirmed shipped capability.

## Validation status
Windows CI run #618: compile, CTest and pluginval strictness 5 completed successfully; packaging and artifact upload were still in progress at the time this profile was written. See https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42958088120 .
Manual FL Studio 20 acceptance, target Windows 10 Enterprise 1809 tests, preset-by-preset playback, bank mapping verification and CPU/RAM/startup benchmarks remain release gates.

## Official project links
- Repository: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-
- Active branch: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/tree/feature/pattern-melody-fx-spec
- CI runs: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions
- Main preset bank: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/blob/feature/pattern-melody-fx-spec/presets/factory_bank_300.json
- Sounds Bank: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/blob/feature/pattern-melody-fx-spec/docs/SOUND_BANK_CATALOG_RU.md
- Implementation audit: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/blob/feature/pattern-melody-fx-spec/docs/IMPLEMENTATION_AUDIT_2026-11-11_RU.md
