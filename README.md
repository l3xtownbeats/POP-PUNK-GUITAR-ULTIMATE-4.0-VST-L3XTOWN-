<div align="center">

<img src="assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg" alt="POP PUNK GUITAR ULTIMATE — L3XTOWN" width="100%">

# POP PUNK GUITAR ULTIMATE
### BY L3XTOWN
**THE POP-PUNK GUITAR INSTRUMENT FOR RIFFS THAT HIT HARD.**

*PLAY. CREATE. ROCK.*



</div>

---

## THE SOUND OF POP-PUNK. BUILT FOR YOUR NEXT TRACK.

**POP PUNK GUITAR ULTIMATE** is an evolving Windows VST3 guitar instrument shaped around the sound and energy of pop-punk, skate-punk and melodic alternative rock.

From tight palm-muted verses to explosive open choruses, from punchy power chords to bright melodic hooks — the project brings the essential guitar vocabulary of the genre into one focused instrument.

Designed with **FL Studio 20** and other VST3 hosts in mind, the instrument combines a sample-based engine, MIDI performance controls and a growing preset system in a streamlined dark interface.

<div align="center">

| MELODY | PATTERN | TONE |
|:---:|:---:|:---:|
| Chromatic guitar-note workflow | Chord-driven rhythm workflow | Clean, crunch and driven directions |
| Riffs, hooks and lead lines | Strums, grooves and rhythmic parts | Electric and acoustic sound targets |

</div>

> **Development preview:** this is an actively developed engineering build, not a finished commercial release. The download is real, but some performance systems and preset definitions are still being integrated and acceptance-tested. This page distinguishes implemented code from planned or catalogued features.



<div align="center">


</div>

### Included in the package

**Important:** the VST3 binary and full Sample Bank are currently distributed together in the complete ZIP. There are no separate standalone VST3 or Sample Bank archive assets published at this time. Preset JSON and example files can be downloaded individually from the links above. The 300-entry catalogue is not a claim that all 300 presets are already playable in the plugin.

### Installation

1. Download and extract the complete ZIP.
2. Copy the `.vst3` bundle into `C:\\Program Files\\Common Files\\VST3\\`.
3. Keep the extracted `Samples` folder available to the plugin.
4. Open or rescan the plugin in your VST3 host.

See `INSTALL_AND_CONTENTS.txt` inside the package for the package-specific instructions.

<sub>The download is a GitHub Actions artifact, not a permanent GitHub Release asset. GitHub may require repository access/authentication, and artifacts can expire. The currently verified artifact is scheduled to expire on 8 February 2027.</sub>

## INSTRUMENT ARCHITECTURE

### Two ways to play

**01 — MELODY**

A direct chromatic instrument workflow: each MIDI key triggers its corresponding guitar note. Built for riffs, octave lines, hooks, picked parts and expressive leads.

**02 — PATTERN**

A chord-oriented rhythm workflow intended for synchronized strumming and pop-punk performance patterns. The scheduler is connected to the processing path, but full real-sampler pattern playback and complete runtime acceptance are still in progress.

### Sound design and controls

The current parameter model includes:

- Master output and polyphony / voice count
- Velocity response, dynamics and palm-mute threshold
- Guitar tuning, pan and output level
- Pick / finger style selection
- Articulation-related controls: vibrato, slide, legato, hammer-on / pull-off
- Scale root and scale mode
- Pattern slot, pattern length, sync mode and swing base

The presence of a parameter does not mean every associated articulation or performance feature is complete.

## SOUND ENGINE

| System | Current state |
|---|---|
| SFZ engine | Parser with global / master / group / region inheritance |
| WAV playback | Key, velocity and MIDI-channel zone selection |
| Sample mapping | Root key, tuning, volume, pan, release and loop metadata |
| Voice engine | Bounded polyphony, up to 64 voices |
| Loop modes | Continuous, sustain, ping-pong and one-shot |
| Loading | SFZ loading on a worker thread; sample loading kept off the audio callback |
| DSP | Basic distortion, chorus, stereo delay and JUCE reverb stages |
| UI | Resizable dark charcoal interface with magenta accents |

The project is designed for efficient startup and controlled resource use. Final CPU/RAM performance claims are withheld until repeatable profiling on the target hardware has been completed.

**Large-library memory target:** the sound library may occupy up to 7 GB on disk; this is not a target for resident RAM usage. SFZ/WAV loading now indexes file metadata instead of decoding the whole bank, and production SamplerEngine voices read audio through the background WAV page streamer. The default cache is 256 pages × 4096 frames × stereo float samples (about 8 MiB per library root), with up to eight roots per sampler instance (about 64 MiB of configured page-cache storage). The underlying page-cache allocator independently rejects any single cache above its hard 256 MiB limit. These figures describe page-cache allocation only, not total plugin memory. Streaming integration and multi-page playback regression tests are in CI; 7 GB library acceptance and final RAM/CPU claims remain gated on successful stress profiling.

## FACTORY SOUND DIRECTION

### 300 authored preset definitions

The repository includes a structured catalogue of **300 preset definitions**:

- **150 Melody**
- **150 Pattern**
- **21 artist-inspired style profiles**

The catalogue explores the sound vocabulary associated with Blink-182, Sum 41, Simple Plan, New Found Glory, Good Charlotte, Mest, Angels & Airwaves, The All-American Rejects, MGK, Avril Lavigne, Green Day, Fall Out Boy, Yellowcard, My Chemical Romance, Less Than Jake, Fenix TX, Midtown, Bowling for Soup, The Offspring, Sugarcult and All Time Low.

### Tone palette

Clean · Crunch · Drive · Palm Mute · Open Chords · Acoustic · Lead Drive · Muted Chugs.

### Performance vocabulary

Straight eighth-note downstrokes · alternate picking · fast skate-punk drive · palm-muted verses · open chorus strums · ska upstrokes · syncopated stop-start riffs · octave hooks · arpeggios · acoustic strumming · dynamic builds.

These are authored sound and performance targets. The Pattern catalogue still requires completed runtime integration and licensed performance recordings before it can be described as a fully playable factory bank.

Artist and track names are used only as stylistic reference points. This independent project is not affiliated with or endorsed by any artist, label or third-party instrument maker.

## SAMPLE BANK & LICENSING

The sampler is designed for compatible SFZ/WAV libraries. The build pipeline references redistributable sources including:

- Meta Open Sound Steel Guitar and Nylon Guitar — CC0 source release.
- FreePats FSBS electric guitar clean and distorted sets — CC0 source repositories.

Sample redistribution is subject to source and license verification. Commercial Session Guitarist libraries, protected recordings, stems and proprietary content are not included. See the [sample licensing policy](docs/SAMPLE_LIBRARY_SOURCING_AND_LICENSE_POLICY.md) and [Meta clearance notes](licenses/META_OPEN_SOUND_CLEARANCE.md).

## DEVELOPMENT STATUS

| Milestone | Current status |
|---|---|
| **M1 — VST3 foundation** | Windows Release build and parameter audit pass. User previously confirmed the plugin opens in FL Studio 20; full state, automation and panic acceptance remains. |
| **M2 — SFZ/WAV sampler** | SFZ/WAV regions now route through bounded background page streaming in SamplerEngine. Multi-page playback regression coverage is added; CI, long playback, 100-hit no-click, cache stress and full-library acceptance remain. |
| **M3 — Pattern transport/runtime** | Scheduler unit tests pass. Full connection to real sampler playback and complete pattern runtime acceptance remain in progress. **NOT ACCEPTED.** |
| **M4 — AutoChords / FingeringGenerator** | Foundation exists; runtime integration and acceptance remain open. |

### Latest verified CI build

The package was produced by a successful Windows x64 VST3 build and CTest workflow: [VIEW VERIFIED BUILD](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42763253767).

This confirms compilation and automated workflow tests for that build. It does not replace manual host validation or milestone acceptance.

## DEVELOPMENT EXECUTION — THREE STAGES (10 NOV 2026)

Development is organized into three controlled stages. The primary audio line always has priority; secondary work must not overload its build/test cycle.

| Stage | Milestones | Execution rule |
|---|---|---|
| **1 — Core audio line** | **M1, M2, M3, M4, M5** | Highest priority. Stabilize state/loading, streaming sampler, MIDI/transport, AutoChords/fingering and Style Engine; close tests and integration gates before declaring completion. |
| **2 — Secondary product work** | **M6, M7, M8, M9, M10** | While Stage 1 is active, only lightweight isolated preparation: code audits, licensed sample-source research, preset catalogue validation, Style DNA planning and GUI/DSP audits. No heavy audio-thread integration or competing stress builds. Full implementation follows Stage 1 acceptance. |
| **3 — QA and release infrastructure** | **M11, M12, M13** | Starts only after M1–M5 are confirmed complete. Bank tools, full stress/performance QA, clean Windows install, Pluginval and FL Studio 20 release acceptance. M13 closes only after all earlier gates pass. |

**Current execution state:** Stage 1 remains active. The latest M1/M2 Windows run exposed one failing importer regression test; the full VST3 workflow for the documentation update is still running. This is not an accepted build. See the development journal and bug register below.

**Project journals:** [Chronological development journal](jurnal.md) · [Unified project INFO](Info.txt) · [Bug and fix register](docs/ЖУРНАЛ_БАГОВ_RU.md) · [M1–M5 integration work log](docs/WORK_LOG_M1_M5_INTEGRATION_RU.md).

## TECHNICAL SPECIFICATION

| Specification | Detail |
|---|---|
| Platform | Windows x64 |
| Plugin format | VST3 instrument |
| Framework | JUCE 7.0.12 |
| Language | C++20 |
| Toolchain | Visual Studio 2022 / MSVC v143 |
| Intended DAW | FL Studio 20 |
| Audio | Stereo output |
| MIDI | Note, velocity and channel input |
| Sampler voice cap | 64 |
| Sample format | WAV via SFZ mapping |
| Interface | Resizable dark charcoal / magenta |

## ROADMAP

1. Finish M1 host-side state recall, MIDI, automation and panic acceptance.
2. Complete M2 sampler stress, velocity / round-robin mapping, click-free voice stealing and bounded streaming cache.
3. Complete M3 scheduler-to-sampler runtime, transport boundaries, stop/reset and pattern-slot validation.
4. Complete the licensed guitar recording and sample-map plan.
5. Develop the tone and articulation pipeline, then proceed to AutoChords / FingeringGenerator in milestone order.
6. Integrate and validate the factory preset bank, sample browser and preview workflow.
7. Run loudness-matched listening comparisons and CPU/RAM profiling on target hardware.

## BUILD FROM SOURCE

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

- [Полное описание согласованных функций, форматов импорта и изменений ветки (RU)](docs/ПОЛНОЕ_ОПИСАНИЕ_ФУНКЦИЙ_И_ИЗМЕНЕНИЙ_ВЕТКИ_RU.md)

## PROJECT & DOCUMENTATION

- [Source repository](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-)
- [Active development branch](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/tree/migration/full-copy-m3-transport-scheduler-v2)
- [Windows build verification](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42763253767)
- [Full project prompt](Promt.txt)
- [Master product specification](docs/MASTER_PRODUCT_SPEC_4_0_RU.md)
- [Bank and library system](docs/BANK_LIBRARY_SYSTEM_RU.md)
- [Preset bank readiness](docs/PRESET_BANK_4_0_READINESS_RU.md)
- [Sampler / streaming design](docs/SAMPLE_STREAMING_PAGE_CACHE_DESIGN_RU.md)
- [Sample source and license policy](docs/SAMPLE_LIBRARY_SOURCING_AND_LICENSE_POLICY.md)
- [Complete development journal — Info.txt](Info.txt)
- [Development journal (legacy archive)](docs/ЖУРНАЛ_РАЗРАБОТКИ_RU.md)
- [Historical M3 work log](docs/WORK_LOG_M3.txt)
- [Bug log (RU)](docs/ЖУРНАЛ_БАГОВ_RU.md)

## CONTACT & OFFICIAL LINKS

<div align="center">

**L3XTOWN — MUSIC · SOUND · DEVELOPMENT**

[![Email](https://img.shields.io/badge/EMAIL-l3xtown%40yandex.ru-cb2b83?style=for-the-badge&logo=yandex)](mailto:l3xtown@yandex.ru)

[![Website](https://img.shields.io/badge/WEBSITE-BEATMAKER.TV-171717?style=for-the-badge&logo=googlechrome)](https://beatmaker.tv/user/182573)

[![Telegram](https://img.shields.io/badge/TELEGRAM-%40l3xtown-229ED9?style=for-the-badge&logo=telegram)](https://t.me/l3xtown)

**Business & project enquiries:** [l3xtown@yandex.ru](mailto:l3xtown@yandex.ru)

</div>

---

<div align="center">

### POP PUNK GUITAR ULTIMATE BY L3XTOWN

*Built for riffs, hooks, power chords and loud choruses.*

**PLAY. CREATE. ROCK.**

</div>
