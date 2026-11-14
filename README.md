<div align="center">

<img src="assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg" alt="POP PUNK GUITAR ULTIMATE — NEON STAGE visual cover" width="100%">

# POP PUNK GUITAR ULTIMATE 4.0
### L3XTOWN AUDIO LAB · WINDOWS x64 · VST3 · JUCE · C++20

**A dedicated pop-punk guitar instrument for tight rhythm parts, power chords, palm-muted patterns, riffs and melodic hooks.**

[DEVELOPMENT BRANCH](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/tree/POP-PUNK-GUITAR-ULTIMATE-VST-REV.18) · [ACTIONS & BUILDS](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions) · [ENGINEERING JOURNAL](Info.txt) · [CODE HISTORY](CODE.txt) · [DOCUMENTATION](docs/) · [ISSUES](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/issues)

![Platform](https://img.shields.io/badge/PLATFORM-Windows%2010%20x64-111827?style=for-the-badge)
![Format](https://img.shields.io/badge/PLUGIN-VST3-075985?style=for-the-badge)
![Core](https://img.shields.io/badge/CORE-C%2B%2B20-312e81?style=for-the-badge)
![Status](https://img.shields.io/badge/STATUS-IN%20ACTIVE%20DEVELOPMENT-d97706?style=for-the-badge)

**PLAY. CREATE. ROCK.**

</div>

---

## A guitar instrument built around the song

**POP PUNK GUITAR ULTIMATE (PPGU)** is a sample-based virtual guitar instrument in development for producers and songwriters working across pop-punk, skate-punk, melodic punk, emo and alternative rock.

The product goal is simple: load a sound, play immediately, write convincing guitar parts and keep the workflow fast. The instrument is being engineered around a single existing Sample Engine, a focused interface and a carefully curated guitar sound library.

> **Development transparency:** this repository separates approved product requirements from source implementation, automated test evidence, DAW host verification and release status. A design, mockup, checklist or approved requirement is never presented as proof that a feature is finished.

## Two focused playing modes

| Mode | Designed for | Workflow |
|---|---|---|
| **MELODY** | Riffs, octave lines, lead hooks, picked notes and melodic guitar parts | Preset → Playing Style → Articulation → MIDI performance |
| **PATTERN** | Chord-driven rhythm guitars, downstrokes, upstrokes, palm-muted verses, open choruses and rhythmic variations | Preset → Playing Style → Optional Variation |

These are the product's two playing modes. The interface architecture remains deliberately focused: **two main windows only** — Preset Browser and the Editor. No third application window and no parallel sound engine.

## Instrument and sound design

The approved sound direction covers:

- Tight, controlled palm mutes and punchy power chords.
- Bright open chords, octave lines and melodic lead parts.
- Clean and driven electric-guitar tones.
- Acoustic/nylon colour where lawful source material and correct mappings are available.
- Expressive velocity, dynamics, note transitions and guitar articulations.
- Individually recognizable presets rather than superficial name variations.
- Clean **DRY** sounds and processed **WET** variants sharing the same source samples wherever possible.

Artist and record references in the project are tonal and arrangement references only. They do not imply endorsement, affiliation or the inclusion of copied recordings, proprietary samples, artwork or presets.

## AUTOCHORDS ULTIMATE

AUTOCHORDS ULTIMATE is an approved development specification for chord-aware guitar performance. Its design covers chromatic chord selection, major/minor choices, recognition of incoming MIDI notes, CURRENT/NEXT chord display, progression editing and guitar-style rhythm performance.

It must read the notes actually received over MIDI, represent ambiguous chords honestly and use articulations that exist in the mapped library. The specification is approved; runtime completeness and DAW behaviour remain subject to source audit and testing.

## Sound library and sample workflow

The library is being organized around one controlled path:

`SOURCE → IMPORT → VALIDATE → REGISTER → LIBRARY → SAMPLE ENGINE`

The intended workflow supports deliberate import, validation, mapping, duplicate detection, provenance and integrity checks. Sample registration uses stable IDs so presets do not depend on fragile absolute paths. Heavy scanning, hashing and file operations belong on worker threads — never in the real-time audio callback.

**Factory bank target:** 250 MELODY READY + 250 PATTERN READY presets. This is a readiness target, not a claim that 500 playable and human-auditioned presets currently exist. A preset is marked TESTED only after actual playback and style-fit acceptance.

Only material whose license permits the intended use may be bundled or redistributed. Download permission, evaluation permission and redistribution permission are different rights. Unclear sample rights mean the material is quarantined, not shipped.

## Engineering priorities

The product is being developed against a single-engine, real-time-safe architecture. The approved engineering scope includes:

- **Audio path:** gain staging, DRY/WET integrity, FX routing, output peak protection and consistent preset levels.
- **Voice and MIDI:** voice allocation/priority/reclamation, safe voice stealing, articulation handling, note lifecycle and no stuck notes.
- **Playback quality:** sample fidelity, interpolation, smooth transitions, click/dropout prevention and correct velocity layers.
- **DSP:** AMP & TONE, dynamics, EQ, effects and measured CPU cost.
- **Resources:** shared sample memory, bounded cache, safe streaming and controlled background work.
- **Presets and recovery:** dependency integrity, compatibility, state save/restore and safe error handling.
- **Host and release:** VST3 behaviour, FL Studio 20 validation, buffer/sample-rate coverage, Windows x64 builds and exact-SHA release checks.
- **Diagnostics and automation:** reproducible tests, stress runs, long-duration playback and evidence-based performance profiling.

The detailed requirements are maintained in the engineering journal and master register. Approval means the requirement is part of the plan; it does not mean the implementation or tests are complete.

## Interface and visual identity

The repository cover now uses the **NEON STAGE** visual direction: a premium dark stage-console aesthetic with controlled electric-blue, cyan, violet and magenta lighting, high-contrast typography and a refined instrument-panel presentation.

The cover is a **brand/design showcase**, not a screenshot of a running VST3. It does not certify that NEON STAGE is the current runtime default skin. Runtime skin identity and default selection are governed by the canonical skin registry and must be reconciled through the project's skin audit before any runtime claim is made.

The product UI remains governed by the approved NEON STAGE design direction and the existing two-window architecture. Skins are visual-only: they must not change DSP, MIDI, APVTS semantics, sampler behaviour or preset state.

## Platform and technical baseline

| Item | Target |
|---|---|
| Product | Windows x64 virtual guitar instrument |
| Plugin format | VST3 |
| Primary DAW acceptance target | FL Studio 20 |
| Build system | CMake |
| Language | C++20 |
| Audio architecture | One existing Sample Engine |
| Playing modes | MELODY and PATTERN |
| Main windows | Preset Browser and Editor |
| Realtime rule | No file/network I/O, blocking waits or unbounded work in the audio callback |

Performance goals are measured, not assumed. CPU, RAM, latency, sample-load time and preset-switch time must be reported against a reproducible test profile before numerical performance claims are published.

## Current development status

- **Active engineering branch:** [`POP-PUNK-GUITAR-ULTIMATE-VST-REV.18`](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/tree/POP-PUNK-GUITAR-ULTIMATE-VST-REV.18).
- **Current product state:** active development; not declared release-ready.
- **Latest known run checked for the previous UI audit:** [GitHub Actions #43592701287](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43592701287) — **CANCELLED**, on source SHA `7a7fb55641f36353fe32873c677e5db64142624c`. It is not a successful build or release gate.
- **Current exact-SHA build, full CTest, pluginval and package verification:** not confirmed by this page.
- **FL Studio 20 host acceptance on the current exact SHA:** not confirmed.
- **Download:** no current release package is declared approved here.

A successful run on an older commit does not certify a newer commit. Release claims require a matching source SHA, completed Windows build, full test suite, pluginval, package integrity checks and separate DAW host acceptance.

## Development records

- [Master implementation and requirements register](docs/PPGU_MASTER_IMPLEMENTATION_REGISTER_RU.md)
- [Development Control Center](docs/PPGU_DEVELOPMENT_CONTROL_CENTER.md)
- [Engineering journal](Info.txt)
- [Code change record](CODE.txt)
- [Task registry](PPGU_TASK_REGISTRY.md)
- [Skin system and current limitations](SKIN/README.md)
- [Sample-library sourcing and license policy](docs/SAMPLE_LIBRARY_SOURCING_AND_LICENSE_POLICY.md)
- [Preset bank readiness](docs/PRESET_BANK_4_0_READINESS_RU.md)
- [Download and installation notes](DOWNLOAD.md)

## Project principles

**One engine. Two playing modes. Two main windows. Real functionality over decorative controls.**

Preserve the approved architecture, MIDI/APVTS and preset-state compatibility. Improve existing components before proposing new ones. Keep audio processing real-time safe, control CPU/RAM, protect sample rights and validate every material change on the exact commit that contains it.

---

<div align="center">

**L3XTOWN AUDIO LAB**  
*ENGINEERED FOR THE NEXT RIFF.*  
**PLAY. CREATE. ROCK.**

</div>
