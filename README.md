<div align="center">

<img src="assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg" alt="POP PUNK GUITAR ULTIMATE — L3XTOWN AUDIO LAB" width="100%">

# POP PUNK GUITAR ULTIMATE
### L3XTOWN AUDIO LAB · WINDOWS x64 · VST3 INSTRUMENT

**A dedicated pop-punk guitar instrument for riffs, hooks, power chords and high-energy rhythm parts.**

[CURRENT BUILD](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43113647167) · [DOWNLOAD CENTER](DOWNLOAD.md) · [ENGINEERING LOG](Info.txt) · [DOCUMENTATION](docs/)

![Windows x64](https://img.shields.io/badge/PLATFORM-Windows%20x64-20242b?style=for-the-badge)
![VST3](https://img.shields.io/badge/FORMAT-VST3-8c245e?style=for-the-badge)
![C++20](https://img.shields.io/badge/CORE-C%2B%2B20-20242b?style=for-the-badge)
![Status](https://img.shields.io/badge/STATUS-ACTIVE%20DEVELOPMENT-d69b28?style=for-the-badge)

**PLAY. CREATE. ROCK.**

</div>

---

## Product

**POP PUNK GUITAR ULTIMATE (PPGU)** is an independently developed Windows VST3 guitar instrument for pop-punk, skate-punk, melodic punk and alternative-rock production. Its two complementary workflows are:

- **MELODY** — chromatic MIDI keyboard playing for riffs, octave lines, hooks and lead parts.
- **PATTERN** — chord-oriented rhythm workflow intended for tight downstrokes, palm-muted verses, open choruses, offbeats and arpeggiated parts.

The product direction is immediate playability, a focused control surface and an efficient sample-based engine suitable for songwriting on modest Windows systems.

> **Development status:** this is an active engineering project, not a declared final commercial release. The status table distinguishes implemented code from features that have passed release acceptance.

## Current build — run #827

**Last checked status: Windows Release build in progress.** The source fix for the previous generated JUCE BinaryData.h compile failure is committed. Sample-cache restore, license validation, x64 CMake configuration and pre-build catalogue checks passed. The Windows Release build step is running; CTest, VST3 discovery, pluginval strictness 5 and packaging have not yet reported results.

- [Open run #827](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43113647167)
- Source commit: 0ef0ebe934350d57779328a5e0979f1134ae85f2
- [Single engineering journal — Info.txt](Info.txt)

**Scope:** run #827 validates the `main` source commit shown above only. It does not validate the 40 other feature, fix, release and integration branches in this repository.

No new COMPLETE ZIP is declared available until every release gate passes. The earlier artifact is a historical previous build, not the current release candidate.

### Previous package

[Download previous Windows x64 VST3 artifact](https://api.github.com/repos/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/artifacts/13766348419/zip) · [Previous successful run](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42821580125)

This package comes from an earlier source revision and layout. Do not confuse it with current HEAD.

## Real plugin interface

The current JUCE editor is a **single fixed product interface**, not a selectable theme marketplace. The source editor defines the dark charcoal / magenta visual language and these actual controls:

- Factory preset selector, category filter and preset search.
- MELODY / PATTERN mode controls.
- SFZ instrument loading and library status.
- Nine rotary parameter controls with APVTS attachments.
- Seven effect switches: Compressor, EQ, Distortion, Chorus, Delay, Reverb and Cabinet IR.
- MIDI keyboard component.
- Resizable editor with minimum dimensions intended to keep the control layout visible.

There is **no active in-plugin skin switcher, theme gallery or runtime skin hot-swap** in the accepted editor implementation. The separate Blackout Voltage SVG is a visual concept asset, not a screenshot of the running plugin, and is not presented as the live UI. Cover artwork is branding artwork, not proof of a tested runtime screen. A host-captured screenshot will be labelled as real only after capture from the built VST3.

## Implementation status

| Area | Repository evidence | Release status |
|---|---|---|
| VST3 / JUCE project | CMake project and JUCE processor/editor source | Current HEAD build pending CI |
| SFZ/WAV sampler | Parser/playback path, metadata and streaming components | Full-library and stress acceptance pending |
| Voice management | 64 preallocated sampler voice slots | 64-voice stress acceptance pending |
| Streaming | Background page streaming and fixed page-cache components | Lifecycle / recovery stress pending |
| MIDI / editor | Melody and Pattern controls, MIDI keyboard, APVTS attachments | Current-host acceptance pending |
| Preset catalogue | 300 authored definitions (150 Melody + 150 Pattern) | Not 300 confirmed playable presets |
| Pattern runtime | Scheduler/transport components | End-to-end sampler playback acceptance pending |
| DSP | Compressor/EQ/Distortion/Chorus/Delay/Reverb/Cabinet controls and DSP components | Signal-quality and host acceptance pending |
| SoundBank | Redistributable sample sources and license metadata in build pipeline | Current package must be verified from CI artifact |
| UI | One fixed dark charcoal / magenta editor | No runtime theme switching |
| Sound Hunter | Policy and registry contract tooling | Production discovery/curation pending |
| FL Studio 20 | Target host | Manual acceptance on current HEAD not confirmed |
| CPU / RAM | Defined engineering targets | No release-grade measurements claimed |

### Catalogue count is not playable count

The catalogue contains **300 preset definitions**. This is not a claim that all 300 are fully mapped, audible and accepted in the running plugin. The release report must state the verified playable count as **N / 300** after runtime audition and validation.

## Sound and instrument design

Target sound palette:

- Tight palm-muted eighth notes and low-string chugs.
- Bright open power chords and wide chorus rhythm guitars.
- Crunch and driven rhythm tones.
- Clean picked patterns, octave hooks and melodic leads.
- Acoustic / nylon textures where licensed source material supports them.

The architecture combines sample playback and DSP processing. The audio thread is designed to avoid file/network I/O, blocking locks, dynamic allocation after note start and GUI work. Streaming, ownership and snapshot reclamation remain explicit QA gates.

### Factory catalogue and preset formats

- [300-definition factory catalogue](presets/factory_bank_300.json)
- [Factory bank data](presets/factory_preset_bank_v1.json)
- [Melody example](presets/examples/Example-Melody.ppgm)
- [Pattern example](presets/examples/Example-Pattern.ppgp)
- [Preset readiness audit](docs/PRESET_BANK_4_0_READINESS_RU.md)

The canonical user-bank conversion format is .pgp; .ppg is not accepted. User imports and discovered content belong in the user library outside the plugin binary.

## Technical profile

| Specification | Current project / target |
|---|---|
| Product | Windows VST3 virtual guitar instrument |
| Target OS | Windows 10 version 1809+ x64 |
| Target DAW | FL Studio 20 |
| Language | C++20 |
| Current JUCE pin | 7.0.12 |
| Build system | CMake; current minimum differs from REV 13.0 target |
| Compiler target | MSVC / Visual Studio 2022, x64 |
| Audio validation profile | 48 kHz, 64-sample buffer |
| MIDI | Note, velocity and channel mapping |
| Sample format | WAV mapped through SFZ |
| Sampler capacity | 64 physical voice slots; default musical polyphony target 32 |
| UI | One fixed dark charcoal / magenta editor |
| Package target | VST3 + SOUNDBANK + PRESETS + runtime resources and assets |

CPU targets, RAM ceilings, cold-start time and latency stability are engineering acceptance targets—not measured product claims—until the current release build is profiled on the reference machine.

## SoundBank, licensing and provenance

Only sample content whose exact license permits redistribution inside a third-party instrument may ship. Every included bank must retain provenance, license evidence and checksum information.

- [Sample-library sourcing and license policy](docs/SAMPLE_LIBRARY_SOURCING_AND_LICENSE_POLICY.md)
- [Meta Open Sound clearance notes](licenses/META_OPEN_SOUND_CLEARANCE.md)
- [SoundBank engine audit](docs/SB0_SOUNDBANK_CODE_AUDIT_RU.md)
- [Streaming/page-cache design](docs/SAMPLE_STREAMING_PAGE_CACHE_DESIGN_RU.md)

Commercial Kontakt libraries, proprietary stems and recordings are not included or implied by the style references.

## Engineering and release gates

1. Resolve and verify the generated JUCE binary-data build regression.
2. Complete Windows x64 Release build for the current source SHA.
3. Run the full CTest suite.
4. Locate and validate the VST3 bundle.
5. Pass pluginval at strictness level 5.
6. Verify the exact package tree, sample count, licenses, manifests and checksums.
7. Perform manual FL Studio 20 host acceptance.
8. Measure CPU/RAM and complete outstanding streaming, voice and lifecycle stress tests.
9. Only then publish the canonical archive:

**POP_PUNK_GUITAR_ULTIMATE_Windows_x64_VST3_COMPLETE.zip**

The exact package layout is documented in [DOWNLOAD.md](DOWNLOAD.md). An artifact from an older commit or earlier successful run does not satisfy current-HEAD release gates.

## Documentation

- [Download center and installation](DOWNLOAD.md)
- [Single engineering journal / authoritative status](Info.txt)
- [Product and engine specification](docs/MASTER_PRODUCT_SPEC_4_0_RU.md)
- [Production priorities](docs/PRODUCTION_PRIORITIES_4_0.md)
- [Preset bank readiness](docs/PRESET_BANK_4_0_READINESS_RU.md)
- [SoundBank audit](docs/SB0_SOUNDBANK_CODE_AUDIT_RU.md)
- [Skin/UI gap audit](SKIN/SKIN_ENGINE_GAP_AUDIT_RU.md)
- [Sample sourcing and license policy](docs/SAMPLE_LIBRARY_SOURCING_AND_LICENSE_POLICY.md)

**Specification authority:** the latest accepted project requirements are MASTER PROMPT REV 13.0 FINAL. Older REV 10.x and earlier prompt files are historical references and must not override REV 13.0 or factual status in Info.txt.

---

<div align="center">

**L3XTOWN AUDIO LAB**  
*ENGINEERED FOR THE NEXT RIFF.*  
**PLAY. CREATE. ROCK.**

</div>
