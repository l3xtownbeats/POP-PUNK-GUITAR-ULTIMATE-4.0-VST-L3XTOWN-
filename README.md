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
| **M1 — VST3 foundation** | Windows x64 Release VST3 build, APVTS registry/state static audit, CTest and pluginval strictness 5 pass in CI run [42864523781](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42864523781). Full state round-trip, library-reference restore, automation, panic and manual FL Studio 20 acceptance remain open. |
| **M2 — SFZ/WAV sampler** | Production SamplerEngine voices use bounded background WAV page streaming and Hermite reads; multi-page audible playback and SFZ round-robin regressions pass CTest. Velocity crossfade is parsed/published as metadata but DSP blending is not yet verified. Windows mmap/OVERLAPPED, safe snapshot retirement, separate palm-mute zones, 100-hit no-click and sustained stress acceptance remain open. |
| **M3 — Pattern transport/runtime** | Scheduler events are dispatched from the processor path; sample-accurate MIDI timeline and explicit output-overflow all-notes-off fail-safe have regression coverage passing CTest. Full musical/DAW acceptance across transport, tempo, loops, block sizes and long notes remains open. **NOT ACCEPTED.** |
| **M4 — AutoChords / FingeringGenerator** | Foundation exists in isolated work; scale-aware mapping, full runtime integration and acceptance remain open. |
| **M5 — Style Engine** | StyleEngine is invoked in the processor pattern path for velocity/accent transformation; complete Style DNA articulation/FX behavior and preset binding remain open. |

### Latest verified CI build

Latest Windows x64 Release VST3 verification is on branch HEAD `2e9356b729e3daba6a5c62c31fed7257072c6f12`:
- [Main Windows VST3 workflow 42871881934](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42871881934) — SUCCESS: Release build, 12/12 CTest tests, VST3 bundle, pluginval strictness 5, package and artifact upload.
- [M1/M2 Windows VST3 workflow 42871881954](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42871881954) — SUCCESS: APVTS audit (91 unique IDs), Release build, sampler tests, VST3 bundle and artifact upload.

The importer regression verifies 3 imported / 2 mapped / 1 needs mapping, Float32 WAV output, SHA-256 known vector and manifest checks. This green CI result does **not** mean M1–M5 are accepted: FL Studio 20 host checks and remaining runtime/stress gates are still open. See the current dated status at the end of this README and the M1–M5 work log.

## DEVELOPMENT EXECUTION — THREE STAGES (10 NOV 2026)

Development is organized into three controlled stages. The primary audio line always has priority; secondary work must not overload its build/test cycle.

| Stage | Milestones | Execution rule |
|---|---|---|
| **1 — Core audio line** | **M1, M2, M3, M4, M5** | Highest priority. Stabilize state/loading, streaming sampler, MIDI/transport, AutoChords/fingering and Style Engine; close tests and integration gates before declaring completion. |
| **2 — Secondary product work** | **M6, M7, M8, M9, M10** | While Stage 1 is active, only lightweight isolated preparation: code audits, licensed sample-source research, preset catalogue validation, Style DNA planning and GUI/DSP audits. No heavy audio-thread integration or competing stress builds. Full implementation follows Stage 1 acceptance. |
| **3 — QA and release infrastructure** | **M11, M12, M13** | Starts only after M1–M5 are confirmed complete. Bank tools, full stress/performance QA, clean Windows install, Pluginval and FL Studio 20 release acceptance. M13 closes only after all earlier gates pass. |

**Current execution state (10 November 2026):** Stage 1 remains active. The latest full Windows VST3 workflow `42864523781` completed successfully, including CTest and pluginval strictness 5. The earlier importer regression/path issue is not present in this successful run; the published-SFZ-path fix is covered by the prior successful M1/M2 workflow. M1–M3 remain **NOT ACCEPTED** because host acceptance and the remaining runtime/stress gates are still open. No milestone is closed by this CI result alone.

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


## Обязательное ТЗ: реализовать весь согласованный объём

**Решение пользователя от 10.11.2026:** все функции и дополнения, согласованные в этой ветке и перечисленные в полном описании, должны быть реализованы в конечном продукте. Это обязательные требования, не просто пожелания. Единое ТЗ с критериями приёмки: [ТЗ — полная реализация согласованных функций](docs/ТЗ_ПОЛНАЯ_РЕАЛИЗАЦИЯ_СОГЛАСОВАННЫХ_ФУНКЦИЙ_RU.md).

Каждое новое утверждённое предложение заносится в backlog с ID, milestone, статусом и тестом; каждое изменение отражается в README, `jurnal.md`, `Info.txt`, `docs/ЖУРНАЛ_БАГОВ_RU.md` (или реестре требований) и `docs/WORK_LOG_M1_M5_INTEGRATION_RU.md` по применимости. Наличие кода/параметра/кнопки не означает готовность: требуются regression tests, актуальный Windows CI/CTest, pluginval и ручная FL Studio 20 проверка, где применимо. Ничего не терять и не объявлять DONE без доказательств.

**Открытая сверка:** в согласованном описании импорт должен нормализоваться в WAV IEEE Float 32-bit, однако INFO содержит запись о временном 24-bit PCM workaround. Реальный writer и тесты необходимо сверить и привести к утверждённому требованию либо запросить отдельное решение; до этого форматный пункт открыт.


## Последняя работа: IMP-002 — SHA-256 и metadata manifest

В importer добавлены source/converted SHA-256, source format, sample rate, channels, frames и duration в manifest. Regression test проверяет контрольные суммы и NCW warning. Реализация ожидает Windows CI; это не означает, что весь импортёр или все форматы приняты. Следующий приоритет основной линии — M1–M3, затем M4/M5 integration.


## Последняя подтверждённая Windows CI-проверка — 11.11.2026

Текущая ветка: `fix/m1-m2-m3-release-gates`  
Проверенный HEAD: `2e9356b729e3daba6a5c62c31fed7257072c6f12` — `test(import): verify SHA-256 against known vector`.

- Основной Windows VST3 workflow: [run 42871881934 — SUCCESS](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42871881934).
- Дополнительный Windows VST3 M1/M2 workflow: [run 42871881954 — SUCCESS](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42871881954).
- Основной workflow: Windows 2022 / MSVC x64 Release configure и VST3 build — SUCCESS; все 12 CTest regression tests — PASS (0 failed); VST3 bundle найден; pluginval strictness 5 — SUCCESS; package и artifact upload — SUCCESS.
- Дополнительный M1/M2 workflow: APVTS audit — PASS (91 уникальный ID, обязательные M1 controls, 7 FX slots); Release VST3 build, sampler unit/integration tests, bundle verification и artifact upload — SUCCESS.
- CTest включает SFZ parser, Pattern scheduler, Pattern/Sampler integration, SamplePool, page-cache slots, bounded SPSC queue, streaming service, sample metadata index, AutoChords, StyleEngine, SampleLibraryImporter и Mapping Editor model.
- Importer regression на этом HEAD проверяет ровно 3 imported / 2 mapped / 1 needs mapping; SFZ содержит только две автоматически сопоставленные regions (C3 и D4), а файл без root note остаётся без автоматически придуманного ключа. Также проверяются Float32 WAV conversion, SHA-256 известный вектор, source/converted hashes в manifest, аудиометаданные и NCW warning.
- Несколько более ранних runs со статусом `cancelled` были остановлены concurrency при новых push; это не считать failure. Исторические compile failures на HEAD `2a0f15b` исправлены и перекрыты успешными runs выше.

**Границы доказанного результата:** это подтверждает зелёную автоматизированную Windows build/test/package цепочку на HEAD `2e9356b`, но не закрывает M1–M5 как milestones. Остаются отдельные runtime/stress и host acceptance gates: полное сохранение/восстановление состояния и библиотеки, FL Studio 20 automation/save-reopen и transport acceptance, M2 velocity crossfade DSP/palm-mute/audio stress, а также полная M4/M5 runtime/preset binding. M3 остаётся NOT ACCEPTED. Stage 3 (M11–M13) не запускать до формальной приёмки M1–M5.

### Баги импортера после успешной проверки
- **IMP-001 — FIXED / REGRESSION PASS:** fixture подтверждает 3/2/1, два валидных SFZ regions и отсутствие выдуманного root key для unmapped take; CTest importer test прошёл в run 42871881934.
- **IMP-003 — FIXED / WINDOWS CI PASS:** недоступный `juce::SHA256` заменён переносимой реализацией SHA-256; известный вектор `abc` проверяется регрессионным тестом; Windows build прошёл.
- **IMP-002 — IMPLEMENTED / WINDOWS CI PASS:** manifest source/converted SHA-256 и аудиометаданные проверяются importer regression test. Это не означает, что все возможные внешние decoder files и license provenance уже прошли полноценную проверку.
