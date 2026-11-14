<div align="center">

<img src="assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg" alt="POP PUNK GUITAR ULTIMATE — L3XTOWN AUDIO LAB" width="100%">

# POP PUNK GUITAR ULTIMATE REV.18
### SIGNAL FORGE · REV.18 · WINDOWS 10 x64 · VST3 · JUCE 7.0.12 · C++20

**A focused pop-punk guitar instrument for riffs, power chords, tight rhythm parts and melodic hooks.**

[REV.17 BRANCH](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/tree/POP-PUNK-GUITAR-ULTIMATE-VST-REV-17) · [ACTIONS / BUILDS](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions) · [DOWNLOAD & INSTALL](DOWNLOAD.md) · [ENGINEERING JOURNAL](Info.txt) · [CODE CHANGELOG](CODE.txt) · [DOCUMENTATION INDEX](docs/)

![Windows x64](https://img.shields.io/badge/PLATFORM-Windows%2010%20x64-20242b?style=for-the-badge)
![VST3](https://img.shields.io/badge/FORMAT-VST3-8c245e?style=for-the-badge)
![JUCE](https://img.shields.io/badge/JUCE-7.0.12-20242b?style=for-the-badge)
![C++](https://img.shields.io/badge/CORE-C%2B%2B20-20242b?style=for-the-badge)
![Status](https://img.shields.io/badge/STATUS-ACTIVE%20DEVELOPMENT-d69b28?style=for-the-badge)

**PLAY. CREATE. ROCK.**

</div>

---

## Project status — read before downloading

PPGU is an actively developed Windows VST3 instrument. This repository page distinguishes **product intent**, **source presence**, **automated test evidence**, and **manual host acceptance**. A specification or UI mockup is not proof that a feature is implemented.

- **Active development branch:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`
- **Documentation baseline before this README update:** `700f56dbbeeab0e7e02559a29dfa9a5d92772b23`
- **Current engineering specification:** SIGNAL FORGE v10 + v10.1 engineering clarification.
- **Latest CI evidence before this README update:** run [#43169004397](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43169004397), SUCCESS on parent SHA `881732c44214ea9160150fc69cd77ad7d85d5bc0`.
- **CI for the current pre-README SHA:** run [#43171568092](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43171568092) was IN PROGRESS when this page was refreshed. Do not infer a result until the run completes and its `head_sha` matches the checked-out commit.
- **FL Studio 20 HOST VERIFIED:** not yet claimed.
- **Performance targets:** targets only until a reproducible measurement report is attached.
- **Release status:** no current-HEAD release package is declared approved by this page.

A green run on an earlier SHA does not certify a later commit. Release claims require build, full CTest, pluginval, package audit and host evidence on the same release-candidate SHA.

## Product concept and playing workflow

**POP PUNK GUITAR ULTIMATE (PPGU)** is designed as a sample-based virtual guitar instrument for pop-punk, skate-punk, melodic punk, emo and alternative rock.

- **MELODY:** chromatic MIDI playing for riffs, octave lines, hooks and lead parts.
- **PATTERN:** chord/rhythm-oriented playing for downstrokes, palm-muted verses, open choruses, offbeats and arpeggiated parts.
- **PUNK GROUP:** planned/required playing-mode workflow; its production completeness must be established from source and tests, not from this description.
- **Sound design direction:** tight palm mutes, bright open power chords, driven rhythm guitars, clean picked parts, octave hooks and appropriately licensed acoustic/nylon textures.

Functional reference direction includes the playability and articulation concepts of Session Guitarist Sunburst Deluxe. It is a behaviour reference only: no proprietary code, samples, graphics or other protected assets are included or implied.

## Visual identity and skin system

### Intended primary skin — NEON PUNK

The accepted SIGNAL FORGE design direction is:
- black / dark-metal chassis and restrained machined-metal surfaces;
- electric neon-blue outlines, key illumination and signal accents;
- punk paint splashes, textured graphic details and the PPGU crown/monogram;
- high-contrast typography, readable labels, professional rotary controls, switches and LED indicators;
- clear hierarchy between preset browsing, playing controls, effects, amplifier section, keyboard and performance status.

### Required initial skin collection

| Skin | Visual direction | Default |
|---|---|---:|
| **NEON PUNK** | Black metal, neon blue, punk textures and PPGU crown | **Yes** |
| **BLACK METAL** | Restrained black / graphite minimalism | No |
| **CYBER BLUE** | Cool cybernetic blue lighting | No |
| **DARK INDUSTRIAL** | Industrial dark metal and utility styling | No |
| **CLASSIC STUDIO** | Professional neutral studio surface | No |

The skin is a visual data layer, not a separate plugin build. All skins must share the same controls, APVTS mappings, DSP, sampler, MIDI behaviour and additional-window functionality. User-added skins are intended to be discovered as data packages without rebuilding the plugin.

**Implementation truth:** the source audit previously found a palette-oriented skin selector and a registry of 52 entries, but did not prove the complete v10 contract. In particular, the full SKINS Gallery, exact five-skin naming/default mapping, asset lifecycle, hot-swap timing, persistence, zero-residue and all-skin control parity remain acceptance items until source-level implementation and tests prove them. Do not treat a manifest entry or preview image as a working skin.

## Interface inventory — zones A–H

The required interface inventory is preserved across all skins:

| Zone | Required content |
|---|---|
| **A — TOP BAR** | PPGU crown/logo, preset previous/next, favourite, preset name/category, statistics, search, SKINS, Import Guitar, Auto Discover, Curator, Settings, About |
| **B — PRESET BROWSER** | BROWSER / FAVORITES / CATEGORIES; eight named sound categories and counts; preset artwork, title, description and information |
| **C — PLAYING CORE** | MELODY / PATTERN / PUNK GROUP; hero artwork; nine rotary controls with their existing APVTS mappings |
| **D — FX CHAIN** | Compressor, EQ, Distortion, Chorus, Delay, Reverb and Cabinet IR with truthful on/off state |
| **E — AUTOCHORDS** | On/off, chord mode, key, pattern editor, speed, current chord, fretboard, progression pads, preview and options |
| **F — FX EDIT** | Effect selection, power, effect presets, mapped parameters, gain-reduction meter, processing graph and mix |
| **G — AMP & TONE** | Amp model, Gain/Bass/Treble and level indication |
| **H — PERFORMANCE / KEYBOARD** | Pitch/Mod, MIDI keyboard, real key-reactive glow, chord/note/transport/tempo/sync/volume and status strip |

The inventory is a completeness contract, not a statement that every panel is already implemented. A control is accepted only when its visible element, action, mapping (where applicable) and test/status are documented together. Existing APVTS IDs and parameter semantics are not to be renamed for visual convenience.

## Core architecture and technical characteristics

| Item | Target / contract |
|---|---|
| Plugin format | VST3 instrument |
| OS / host | Windows 10 x64; FL Studio 20 target |
| Framework / language | JUCE 7.0.12; C++20 |
| Build | CMake; MSVC x64 Release |
| Sample source format | WAV through SFZ mapping |
| Voice capacity | 64 preallocated sampler voice slots; default musical polyphony target 32 |
| Audio callback | No file/network I/O, GUI work, blocking filesystem access or unbounded allocation |
| Library | Background indexing/streaming, bounded resident page cache and validated resource paths |
| Presets | Factory definitions and user-bank formats; playable count must be runtime-verified |
| Skin assets | Manifest-driven, validated, bounded cache; background decode; safe fallback |
| Content root | Portable `PPGU/` home with VST3, SOUNDBANK, PRESETS, SKINS, Resources and Assets |
| Compatibility | Existing DSP, sampler, MIDI routing and APVTS contract preserved during skin work |

### Performance targets — not yet measured claims

- First audible response target: ≤50 ms for an already-loaded preset and resident preview data.
- Indexed preset switch target: ≤300 ms p95 without blocking the audio thread.
- Skin swap target: approximately 150 ms p95 with assets already decoded.
- CPU targets: ≤2% at 4 voices and ≤15% at 64 voices under a fixed, documented benchmark profile.
- RAM targets: ≤60 MB idle and ≤512 MB with the complete resident bank under a documented measurement profile.
- Audio callback peak target: ≤60% of the available buffer time at 128 samples.
- Skin work must not introduce audio-thread allocations or alter rendered audio. UI CPU, paint time and skin-owned memory are measured separately; “zero footprint” is verified by methodology, not assumed.

## Coherence, portable content and expansion

The intended portable content home is:

```text
PPGU/
├── VST3/        plugin bundle (or standard Windows VST3 install)
├── SOUNDBANK/   WAV, SFZ, provenance, licenses and checksums
├── PRESETS/     factory and user preset data
├── SKINS/       built-in and user-added skin packages
├── Resources/   manifests, documentation and legal notices
└── Assets/      shared graphics and runtime assets
```

On Windows, standard VST3 installation and portable content-root mode must both be supported. The content resolver must use validated paths and safe fallback behaviour. “Unlimited” means no compile-time count ceiling for user libraries/skins; decoded memory and streaming caches remain bounded by configurable quotas.

A release package must be relocatable as a complete folder, reject duplicate/unsafe paths, verify manifests and checksums, and contain only redistributable content. New sample libraries and skins are data additions, not reasons to fork the plugin.

## Licensing and open-source policy

- Reuse of ideas and UX patterns is encouraged.
- Open-source code may be embedded only after checking the exact version, SPDX license, commercial/closed-source compatibility, notices, attribution and linking obligations.
- Proprietary product code, samples, artwork and protected assets are not copied.
- Unclear or incompatible license means the component is not shipped.
- Every embedded third-party block must be listed in the Open Source Ledger and in the package notices.

See [sample-library sourcing and license policy](docs/SAMPLE_LIBRARY_SOURCING_AND_LICENSE_POLICY.md) and the `licenses/` directory.

## Engineering protocol and acceptance

The binding development standard is **SIGNAL FORGE v10 MASTER**, clarified by **v10.1 Engineering Clarification**. The universal work cycle is:

**ПРОВЕРКА → АНАЛИЗ → ИСПРАВЛЕНИЕ → ПОВТОРНЫЙ АНАЛИЗ → РЕЗУЛЬТАТ**

One commit addresses one cause. Every change updates `Info.txt` and `CODE.txt` synchronously. The branch is updated with a non-forced, expected-SHA guard. A task is not complete merely because it is designed or documented.

Release acceptance is grouped into:
1. **Integrity & safety:** schemas, manifests, path validation, licenses, checksums.
2. **Build:** Windows x64 configure, compile, link and VST3 validation.
3. **Correctness:** full CTest, lifecycle, fallback, control parity and regression tests.
4. **Realtime:** callback allocation/timing, dropout and audio-identity tests.
5. **UI:** key reactivity, DPI, panel/window behaviour and approved mockup comparison.
6. **Performance:** reproducible CPU/RAM/paint/hot-swap/streaming measurements.
7. **Host & package:** pluginval strictness 5, FL Studio 20 manual test, relocation smoke test and ZIP audit.

A release gate must use one exact source SHA and a package manifest tied to that SHA. CI success alone does not establish FL Studio 20 host verification.

## Documentation and project records

- [SIGNAL FORGE v10.1 engineering clarification](docs/PPGU_SIGNAL_FORGE_V10_1_ENGINEERING_CLARIFICATION_RU.md)
- [SIGNAL FORGE v7 skin-system master record](docs/PPGU_SIGNAL_FORGE_V7_MASTER_RU.md)
- [Single authoritative engineering journal](docs/PPGU_JOURNAL/PPGU_JOURNAL.md)
- [Append-only code change index](docs/PPGU_JOURNAL/CODE.md)
- [Download and installation status](DOWNLOAD.md)
- [Preset bank readiness](docs/PRESET_BANK_4_0_READINESS_RU.md)
- [SoundBank code audit](docs/SB0_SOUNDBANK_CODE_AUDIT_RU.md)
- [Sample streaming/page-cache design](docs/SAMPLE_STREAMING_PAGE_CACHE_DESIGN_RU.md)
- [Sample-library sourcing and license policy](docs/SAMPLE_LIBRARY_SOURCING_AND_LICENSE_POLICY.md)
- [Skin engine gap audit](SKIN/SKIN_ENGINE_GAP_AUDIT_RU.md)
- [Master product specification](docs/MASTER_PRODUCT_SPEC_4_0_RU.md)
- [PPGU 4.0 master development plan and release gates](docs/PPGU_MASTER_DEVELOPMENT_PLAN_RU.md)
- [Production priorities](docs/PRODUCTION_PRIORITIES_4_0.md)

**Authority order:** actual source tree and build configuration establish what is implemented; tests/CI establish what is verified; `Info.txt` records project status and decisions; `CODE.txt` indexes source changes; specifications define intended behaviour. Any disagreement is recorded and investigated, never silently reconciled by editing history.

---

<div align="center">

**L3XTOWN AUDIO LAB**  
*ENGINEERED FOR THE NEXT RIFF.*  
**PLAY. CREATE. ROCK.**

</div>

### Additional approved preset research
- [Angels & Airwaves / Tom DeLonge preset research and implementation requirements](docs/PPGU_ANGELS_AIRWAVES_TOM_DELONGE_PRESET_RESEARCH_RU.md)
