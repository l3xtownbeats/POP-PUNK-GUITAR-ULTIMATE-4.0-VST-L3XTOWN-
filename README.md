<div align="center">

<img src="assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg" alt="POP PUNK GUITAR ULTIMATE — L3XTOWN product cover and interface concept" width="100%">

# POP PUNK GUITAR ULTIMATE
### BY L3XTOWN · WINDOWS x64 · VST3 INSTRUMENT

**THE POP-PUNK GUITAR WORKSTATION FOR RIFFS THAT HIT HARD.**

[⬇ DOWNLOAD PREVIOUS VERIFIED BUILD](https://api.github.com/repos/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/artifacts/13766348419/zip) · [LATEST BUILD STATUS](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43017263448) · [PRESETS](presets/) · [DOCUMENTATION](docs/)

![Windows](https://img.shields.io/badge/Windows-x64-24292f?style=for-the-badge&logo=windows)
![VST3](https://img.shields.io/badge/Plugin-VST3-8e2462?style=for-the-badge)
![JUCE](https://img.shields.io/badge/Framework-JUCE-6b315a?style=for-the-badge)
![C++20](https://img.shields.io/badge/Language-C%2B%2B20-24292f?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Development%20Preview-orange?style=for-the-badge)

*PLAY. CREATE. ROCK.*

</div>

---

## About the instrument

**POP PUNK GUITAR ULTIMATE** is an actively developed sample-based electric-guitar VST3 instrument focused on pop-punk, skate-punk and melodic alternative rock. It is being built for fast songwriting workflows: punchy power chords, tight palm-muted verses, energetic eighth-note rhythms, octave hooks, melodic leads and open choruses.

The target workflow is immediate MIDI playability in **FL Studio 20** and other Windows VST3 hosts. The project combines a JUCE/C++20 instrument, an SFZ/WAV sampler, a growing factory-preset catalogue and a dark charcoal interface with magenta accents.

> **Development preview — not a finished commercial release.** The repository contains working engine components and buildable project source, but some catalogue entries and performance systems are not yet fully integrated. This page separates implemented components from planned or not-yet-accepted features.

## Downloads

### Previous verified Windows x64 VST3 package

<div align="center">

### [⬇ DOWNLOAD VST3 + GUITAR SAMPLES + PRESET FILES](https://api.github.com/repos/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/artifacts/13766348419/zip)

**ZIP · approximately 1.55 GiB · GitHub Actions artifact**

[Open the successful build and test run](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42821580125)

</div>

This is the previous successful package from an earlier development commit. It contains the VST3 instrument, the redistributable sample collection and preset files. It uses the earlier package layout; it is **not** the new VST3 / SOUNDBANK / PRESETS / SKINS / Resources / Assets layout currently being prepared.

**Current packaging run:** [Run #826 — inspect live CI status](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43017263448). The new complete ZIP is not published until this run passes build, CTest, pluginval and packaging checks. Do not treat the previous artifact as the new package.

### Presets and examples

- [Factory catalogue — 300 preset definitions (JSON)](presets/factory_bank_300.json)
- [Additional factory bank data (JSON)](presets/factory_preset_bank_v1.json)
- [Melody portable preset example (.ppgm)](presets/examples/Example-Melody.ppgm)
- [Pattern portable preset example (.ppgp)](presets/examples/Example-Pattern.ppgp)
- [Preset readiness and integration status](docs/PRESET_BANK_4_0_READINESS_RU.md)

**Important:** the 300 entries are authored sound/performance definitions (150 Melody + 150 Pattern). They are not a claim that 300 fully playable presets are already integrated into the current plugin UI. Pattern playback still requires full sampler/runtime and performance-asset acceptance.

### SoundBank and sample sources

The previous complete ZIP contains the sample collection. A separate standalone SoundBank archive is not currently published.

- [Sample sourcing and redistribution policy](docs/SAMPLE_LIBRARY_SOURCING_AND_LICENSE_POLICY.md)
- [Meta Open Sound CC0 clearance notes](licenses/META_OPEN_SOUND_CLEARANCE.md)
- [SFZ/WAV streaming design](docs/SAMPLE_STREAMING_PAGE_CACHE_DESIGN_RU.md)
- [SoundBank engine audit](docs/SB0_SOUNDBANK_CODE_AUDIT_RU.md)

Only sample material whose exact license permits redistribution inside a third-party instrument may be bundled. Commercial Session Guitarist/Kontakt samples, stems and proprietary recordings are not included.

### Skins, resources and visual assets

- [Skin specifications and asset requirements](SKIN/README.md)
- [Skin asset specifications](SKIN/ASSET_SPECIFICATIONS_RU.md)
- [Skin control inventory](SKIN/CONTROL_INVENTORY_MATRIX_RU.md)
- [Theme schema](SKIN/theme.schema.json)
- [Theme manifest](SKIN/theme_manifest.json)
- [Product cover artwork (SVG)](assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg)
- [Interface artwork / source image](Cover%20Interface.jpg)

The repository contains skin/theme specifications and 50 theme folders/manifests. Their presence is not proof that runtime skin switching is finished. The new complete build workflow is intended to package these into SKINS/, with supporting files in Resources/ and Assets/.

## Instrument architecture

### MELODY — chromatic guitar instrument

- MIDI keyboard / piano-roll note workflow.
- Each MIDI note is intended to trigger its mapped guitar pitch.
- Designed for riffs, octave lines, hooks, picked parts and lead melodies.
- SFZ region selection supports key, velocity and MIDI-channel mapping.

### PATTERN — chord-driven rhythm workflow

- Designed for synchronized strumming and pop-punk rhythm parts.
- Target vocabulary includes downstrokes, alternate picking, palm-muted eighths, open chorus strums, ska offbeats, stop-start riffs, arpeggios and acoustic strumming.
- Scheduler components and tests exist; full real-sampler pattern playback and host acceptance remain in progress.

## Sound engine and controls

| Component | Current implementation / status |
|---|---|
| Plugin format | VST3 instrument |
| Audio engine | JUCE / C++20 |
| Sampler | SFZ parser and WAV playback path |
| Mapping | Key, velocity, channel, root key, tuning, volume, pan and loop metadata |
| Voice management | Bounded voice engine, up to 64 voice slots |
| Streaming | Background WAV page streaming and bounded page-cache components |
| Loop modes | Continuous, sustain, ping-pong and one-shot metadata/path |
| DSP stages | Basic distortion, chorus, stereo delay and JUCE reverb components |
| UI direction | Resizable dark charcoal interface with magenta accents |
| Pattern engine | Scheduler components present; complete runtime integration not accepted |

The parameter model includes master output, voices/polyphony, velocity response, palm-mute threshold, tuning, pan, pick/finger selection, articulation-related controls, scale root/mode and pattern timing controls. A parameter or catalogue entry is not automatically proof that every related performance behavior is complete.

### Tone and performance targets

**Tone palette:** Clean · Crunch · Drive · Palm Mute · Open Chords · Acoustic · Lead Drive · Muted Chugs.

**Effects in the current DSP codebase:** Distortion · Chorus · Stereo Delay · Reverb. Flanger and expanded amp/cab processing remain targets unless separately confirmed in the current build.

**Style references:** Blink-182, Sum 41, Simple Plan, New Found Glory, Good Charlotte, Mest, Angels & Airwaves, The All-American Rejects, MGK, Avril Lavigne, Green Day, Fall Out Boy, Yellowcard, My Chemical Romance, The Offspring, Sugarcult and All Time Low.

Artist and product names are used as stylistic references only. This is an independent project and is not affiliated with or endorsed by any artist, label or third-party instrument maker.

## Technical specifications

| Specification | Detail |
|---|---|
| Platform | Windows 10/11 x64 |
| Plugin format | VST3 instrument |
| Intended DAW | FL Studio 20 |
| Framework | JUCE 7.0.12 |
| Language | C++20 |
| Toolchain | Visual Studio 2022 / MSVC v143 |
| Audio | Stereo output |
| Input | MIDI notes, velocity and channel |
| Sampler format | WAV audio mapped by SFZ |
| Maximum sampler voices | 64 |
| UI | Resizable dark charcoal / magenta |
| Build type | Windows x64 Release |

CPU/RAM performance targets, full 7 GB library stress acceptance and final low-latency claims are withheld until repeatable profiling is complete. The configured streaming page-cache figures are engineering limits, not a guarantee of total plugin memory usage.

## Installation

1. Download and extract the complete ZIP.
2. Copy the .vst3 bundle into C:\Program Files\Common Files\VST3\.
3. Keep the extracted sample folder available to the plugin; do not move or delete it after installation.
4. Open FL Studio 20 or another VST3 host and run a plugin rescan.
5. Load the instrument and use MIDI input / Piano Roll to audition notes.

The current previous-build ZIP includes INSTALL_AND_CONTENTS.txt with package-specific notes. The new folder layout will be documented in the new artifact when the current packaging workflow passes.

## Development and verification

| Milestone | Status |
|---|---|
| VST3 foundation / Windows build | Earlier Windows x64 CI build passed; current HEAD is rebuilding |
| SFZ/WAV sampler | Core playback and streaming components implemented; stress and full-library acceptance remain |
| Pattern transport/runtime | Scheduler components exist; full runtime playback is not accepted |
| Factory presets | 300 definitions catalogued; runtime playability not accepted for all entries |
| Skin/theme system | Specifications and theme manifests exist; runtime switching not accepted |
| FL Studio 20 current-HEAD acceptance | Not confirmed |
| CPU/RAM profiling | Not completed for release claims |

**Latest packaging workflow:** [Run #826](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43017263448)  
**Previous successful Windows build:** [Run #216](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42821580125)

## Project links

- [Source repository](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-)
- [Latest build workflow](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/workflows/vst3-build.yml)
- [Previous verified package download](https://api.github.com/repos/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/artifacts/13766348419/zip)
- [Preset library](presets/)
- [Skin/theme library](SKIN/)
- [Product and engineering documentation](docs/)
- [Single development log — Info.txt](Info.txt)
- [Full project prompt](Promt.txt)

## Русское описание

**POP PUNK GUITAR ULTIMATE** — разрабатываемый VST3-инструмент для гитарных партий в стиле Pop-Punk / Skate-Punk / Alternative Rock. Цель — быстрое создание риффов, power chords, palm-muted куплетов, открытых припевов, мелодических хуков и лид-партий в FL Studio 20.

В проекте есть SFZ/WAV sampler, до 64 voice slots, компоненты фонового стриминга сэмплов, DSP-компоненты Distortion/Chorus/Delay/Reverb, каталог из 300 описаний пресетов и спецификации тем оформления. При этом 300 пресетов пока не следует считать полностью playable, а переключение скинов и Pattern runtime ещё не прошли финальную приёмку.

**Скачать предыдущую успешную сборку:** [VST3 + SoundBank + preset files (ZIP)](https://api.github.com/repos/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/artifacts/13766348419/zip).  
**Текущая новая сборка и упаковка:** [GitHub Actions run #826](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43017263448) — новый ZIP появится после успешного завершения CI.

---

<div align="center">

**L3XTOWN AUDIO LAB**

*RIFFS · HOOKS · POWER CHORDS*

**PLAY. CREATE. ROCK.**

</div>
