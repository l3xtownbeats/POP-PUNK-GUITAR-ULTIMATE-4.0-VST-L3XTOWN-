<!-- Snapshot of README.md from main immediately before integrating the full development branch on 2026-11-10. Preserved to avoid losing main-only documentation edits. -->

<div align="center">

<img src="assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg" alt="POP PUNK GUITAR ULTIMATE VST BY L3XTOWN" width="100%">

# POP PUNK GUITAR ULTIMATE VST BY L3XTOWN

### Premium pop-punk guitar instrument for Windows VST3

**PLAY. CREATE. ROCK.**


</div>

---

## The sound

**POP PUNK GUITAR ULTIMATE** is a Windows x64 VST3 guitar-instrument project built around the energy of pop-punk, skate-punk and melodic alternative rock: tight verse rhythms, punchy power-chord tones, bright open choruses and expressive lead parts.

The product target is a fast, expressive, commercial-grade pop-punk guitar instrument for FL Studio 20 and other VST3 hosts. The sound-quality ambition is to compete with premium instruments such as Native Instruments Session Guitarist — Electric Sunburst Deluxe, while building an original pop-punk-focused identity. This is a target, not a claim of current parity: achieving it requires premium multi-velocity / round-robin recordings, detailed articulations, authentic chord-strum performances, a polished amp/cab chain and extensive listening tests.

> This is an actively developed instrument. The current build is an engineering preview, not a finished commercial release. The feature and preset status below separates working code from planned/catalogued functionality.



<div align="center">


[Open the build page if the direct artifact link does not start](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42747513529)

**Product:** `POP PUNK GUITAR ULTIMATE VST BY L3XTOWN`  
**Package:** `POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES-AND-PRESETS`  
**Contents:** VST3 plugin + multisample sound bank + SFZ maps/licences + 300-preset factory catalogue + portable preset examples  
**Target:** Windows x64 / FL Studio 20 / VST3  
**Build:** [GitHub Actions run #133](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42747513529)

The direct link opens the ZIP artifact download; GitHub may ask you to sign in. Archive size: approximately 2.24 GB. SHA-256: `f0c6867d685ecf356769aac1f14089893cd894d30272ede5798565a83348644f`. This is a temporary GitHub Actions artifact (available until 8 February 2027), not yet a permanent versioned GitHub Release.

The factory catalogue contains 300 definitions (150 Melody + 150 Pattern). The package also includes the portable preset examples, source SFZ maps, sample licence notices and WAV SHA-256 manifest.

### Installation preview

1. Download and extract the artifact ZIP.
2. Locate the `.vst3` bundle inside it.
3. Copy the complete bundle to `C:\Program Files\Common Files\VST3\`.
4. Restart or rescan plugins in your DAW.
5. Load the instrument and use **LOAD SFZ** to select a compatible, licensed guitar SFZ library.

**Important:** keep the extracted `Samples/` folder with the plugin package. If the plugin asks for a sample-library folder, select `Samples/`. The ZIP also contains the `presets/` folder with the 300-entry factory catalogue and portable preset examples.

## What is implemented

| Area | Current implementation |
|---|---|
| Plugin format | Windows x64 VST3, JUCE 7.0.12, C++20 |
| Audio I/O | Stereo instrument output; MIDI note input |
| Sampler | SFZ parser, WAV sample loading, key/velocity/channel zones |
| SFZ mapping | Global / master / group / region inheritance; sample ranges, root key, tuning, volume, pan and release metadata |
| Sample playback | Voice allocation, bounded polyphony (up to 64 voices), pitch/sample-rate conversion, release envelope |
| Loop handling | Continuous, sustain, ping-pong and one-shot loop modes |
| Loading | SFZ load on a worker thread; sample load is kept off the audio callback |
| Controls | APVTS-hosted parameters including Master, Voices, Velocity, Dynamics, Tuning, Pan and Output |
| UI | Resizable dark charcoal interface with magenta accents, Melody / Pattern mode selection, SFZ load/status, preset catalogue search and keyboard |
| DSP currently present | Basic distortion, chorus, stereo delay and JUCE reverb stages |
| Build checks | Windows MSVC Release build; SFZ parser, SamplePool and PatternScheduler CTest targets |

## Instrument controls and sound design

The project is designed around two strictly separated playing workflows:

- **MELODY** — direct chromatic guitar-note playing from MIDI keys, for riffs, hooks, octave parts and lead lines.
- **PATTERN** — rhythm-guitar performance mode, intended for synchronized strumming and chord-driven parts. The transport scheduler is under integration; full pattern playback is not yet accepted.

The parameter model includes:
- Master level and polyphony / voice count
- Velocity response and dynamics
- Palm-mute threshold and articulation selection
- Guitar tuning, pan and output level
- Pick / finger style selection
- Vibrato, slide, legato and hammer-on / pull-off controls
- Scale root and scale mode
- Pattern slot, pattern length, host / host-start / internal sync and swing base

The presence of a parameter in the project does not by itself mean every associated performance feature is complete. See the milestone status below.

## Factory preset catalogue — 300 sound targets

The repository contains a validated JSON catalogue with **300 authored preset definitions** across 21 artist-inspired style profiles:

- **150 Melody definitions**
- **150 Pattern definitions**

These are sound-design targets and structured preset data, not 300 fully integrated, loadable factory presets yet. The plugin preset browser and runtime integration still need to be completed and acceptance-tested before the bank can be advertised as ready to play.

### Melody preset examples

- Blink-182 — Bright Rhythm
- Blink-182 — Tight Verse
- Blink-182 — Wide Chorus
- Blink-182 — Palm Mute
- Blink-182 — Open Chords
- Blink-182 — Power Chords
- Blink-182 — Lead Hook
- Sum 41 — Crunch Drive
- Simple Plan — Bright Rhythm
- New Found Glory — Bright Rhythm / Tight Verse
- Good Charlotte — Bright Rhythm / Tight Verse
- Mest — Bright Rhythm / Tight Verse
- Angels & Airwaves — Bright Rhythm / Tight Verse
- The All-American Rejects — Bright Rhythm / Tight Verse
- MGK — Bloody Valentine Inspired Lead / Bloody Valentine Inspired Chorus
- Fall Out Boy — syncopated emo-pop-punk / octave hooks
- Yellowcard — melodic acoustic/electric layering / picked arpeggios
- My Chemical Romance — dramatic emo-punk / octave lines
- Less Than Jake — ska-punk offbeat upstrokes
- Fenix TX — fast melodic skate-punk
- Midtown — urgent melodic emo-pop-punk
- Avril Lavigne — Bright Rhythm / Tight Verse

### Pattern preset examples

- Blink-182 — Straight Eighths
- Blink-182 — Fast Downstrokes
- Blink-182 — Palm-Muted Verse
- Blink-182 — Open Chorus
- Blink-182 — Power-Punk Drive
- Blink-182 — Syncopated Pop Punk
- Blink-182 — Arpeggio Lift
- Blink-182 — Half-Time Bridge
- Sum 41 — Alternate Picking
- Simple Plan — Acoustic Strum

### Styles represented in the catalogue

Blink-182 · Sum 41 · Green Day · Fall Out Boy · Good Charlotte · Simple Plan · New Found Glory · Avril Lavigne · Yellowcard · The All-American Rejects · My Chemical Romance · Less Than Jake · Fenix TX · Midtown · Bowling for Soup · MGK · Mest · Angels & Airwaves · The Offspring · Sugarcult · All Time Low.

### Pattern sound characters and playing styles

**Sound character (tone):** Clean, Crunch, Drive, Palm Mute, Open Chords, Acoustic, Lead Drive and Muted Chugs. These are sound/voicing targets, not interchangeable names for a groove.

**Playing style (performance):** straight eighth-note downstrokes, alternate down-up picking, fast skate-punk drive, palm-muted verse into open chorus, open down/up strums, ska offbeat upstrokes, syncopated stop-start riffs, octave-hook rhythms, arpeggio picking, acoustic strumming and dynamic builds. A finished Pattern preset must select both a tone and a genuinely distinct playable performance. At present these catalogue records are authored definitions awaiting the completed Pattern runtime and licensed performance recordings.

Artist and track names are used only as musical reference points for tonal direction. This project is independent and is not affiliated with or endorsed by any artist, label or third-party instrument maker.

## Sample library and licensing

The sampler is designed to load compatible SFZ/WAV libraries. The build workflow also prepares redistributable sample sources from:
- Meta Open Sound Steel Guitar and Nylon Guitar — CC0 source release.
- FreePats FSBS electric guitar clean and distorted sets — CC0 source repositories.

The standalone VST3 preview artifact linked above is not represented as a complete sound-library installer. Check the package contents and the sample source/license notices before redistributing any sample data. Commercial Session Guitarist libraries and other restricted content are not included.

## Premium sound target and quality requirements

The commercial sound target is a dense, polished, mix-ready guitar that remains articulate in a busy pop-punk arrangement: controlled low end, strong pick attack, defined upper mids, stable stereo image, convincing palm-mute/open transitions and natural dynamics. Loudness alone is not quality; avoid harsh clipping, brittle top end, muddy low mids and over-compression.

To approach or exceed premium commercial instruments, the production roadmap requires:

1. **Source recordings:** professionally performed DI and miked guitar takes; consistent instrument setup, intonation and tuning; separate neck/bridge pickup or equivalent source options; low-noise capture and phase-coherent multi-mic blends.
2. **Multisampling:** multiple velocity layers and round robins per note/articulation; full useful fretboard range; realistic note decay, release, string/fret noise and controlled transitions. Never fake round robins by randomizing one sample.
3. **Articulations:** open, palm-muted, dead/muted, harmonics, tremolo, slides, hammer-ons/pull-offs and legato only where corresponding performances are recorded and mapped.
4. **Pattern performances:** real down/up strums, string order/timing, chord voicings, alternate picking, arpeggios, stop-start accents, verse/chorus transitions and endings. Melody must remain one MIDI key = one chromatic note; Pattern keys select chords and trigger performance.
5. **Tone chain:** quality amp/cab IR or modelled amp stages, pickup blend/EQ, gate, compression, saturation, chorus/flanger/phaser, delay and room/plate reverbs with gain staging and preset-level output matching.
6. **Mix readiness:** DI/processed blend where licensed and available, mono compatibility, phase checks, transient preservation, sensible headroom, low-noise tails and loudness-matched A/B listening.
7. **Validation:** blind/loudness-matched comparison against commercial references, test riffs across clean/crunch/drive/palm-mute/open-chord/acoustic sounds, low-end and transient analysis, CPU/RAM profiling on the target dual-core/2 GB machine.

These are acceptance requirements and targets. They are not all implemented or verified in the current build. No claim of being sonically superior to Electric Sunburst Deluxe is made until the complete sound library and engine pass comparative listening tests.

## Technical characteristics

| Specification | Value |
|---|---|
| Platform | Windows x64 |
| Plugin | VST3 instrument |
| Framework | JUCE 7.0.12 |
| Language | C++20 |
| Build toolchain | Visual Studio 2022 / MSVC v143 |
| Intended DAW | FL Studio 20 |
| Audio output | Stereo |
| MIDI | Note / velocity / channel input |
| Sampler voice cap | 64 voices |
| UI | Resizable; dark charcoal / magenta theme |
| Sample format | WAV playback via SFZ mapping |
| CPU / RAM | Not yet benchmarked on the minimum target PC; no performance guarantee claimed |

## Development status

| Milestone | Status |
|---|---|
| M1 — VST3 foundation and parameter model | Windows Release build and parameter audit pass; user confirms VST3 opens in FL Studio 20 on Windows 10 1809, dual-core / 2 GB RAM. Full state/automation/panic acceptance remains |
| M2 — SFZ/WAV sampler | Parser and SamplePool tests pass; long-playback, 100-hit no-click, streaming/cache and full library acceptance remain |
| M3 — Pattern transport and runtime | Scheduler unit tests pass; connection to real sampler playback and full pattern runtime are still in progress |
| M4 — AutoChords / FingeringGenerator | Not started for release; gated on M1–M3 acceptance |

### Latest verified CI evidence

- M1/M2 Windows build: [run #162 — successful](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42729490984)
- Pattern scheduler regression and sampler tests: 3/3 passed in [run #78](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42729491121); that workflow subsequently failed in a separate packaging/validation step referencing a missing script. The missing-script reference was removed in a later commit; the replacement workflow is being checked.
- CI success confirms compilation and automated tests, not manual FL Studio 20 validation or measured CPU/RAM targets.

## Roadmap

1. Complete M1 host-side state recall, MIDI and automation checks in FL Studio 20.
2. Complete M2 sampler stress tests, RR / velocity mapping, click-free voice stealing and bounded sample-cache strategy.
3. Connect M3 scheduler events to real sampler playback; verify block-boundary timing, transport stop and eight-slot switching.
4. Complete the professional guitar recording and sample-map plan; verify redistribution licenses.
5. Build the tone engine and authentic performance articulation pipeline; add AutoChords / FingeringGenerator and the separate Melody/Pattern runtimes in milestone order.
6. Complete the 300-preset factory bank, sample browser/preview, UI controls and release packaging.
7. Run loudness-matched sound comparisons and CPU/RAM profiling on the target low-spec PC; optimize from measurements.

## Build from source

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

## Project links

- [Source repository](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE)
- [Active development branch](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/tree/feat/m3-transport-scheduler-v2)
- [Windows build actions](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions)
- [Russian development journal](docs/ЖУРНАЛ_РАЗРАБОТКИ_RU.md)
- [300-entry factory catalogue](presets/factory_bank_300.json)

---

<div align="center">

**POP PUNK GUITAR ULTIMATE BY L3XTOWN**

*Built for riffs, hooks, power chords and loud choruses.*

</div>
