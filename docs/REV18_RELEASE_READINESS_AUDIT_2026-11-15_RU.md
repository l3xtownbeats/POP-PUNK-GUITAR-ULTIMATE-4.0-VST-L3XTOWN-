# REV.18 — Release Readiness Audit
**Audit date:** 2026-11-15  
**Authoritative working branch:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV.18`  
**Parent production baseline:** `main` (4.0)  
**Scope:** evidence-based status only. This document does not promote planned or partially implemented features to complete.

## 1. Verified build evidence

Latest inspected Windows workflow run:
- Run: `43700659867`
- Built source SHA: `877a0c8d96dc6ef3bdd620f8af0a75a79fa067ec`
- Branch HEAD at audit time: `fb67fb54b848540b1b877aa9b8e5ccbb9cd18b50`
- Difference after the build: README-only NEON STAGE heading update; production source was unchanged.
- Windows 2022 / Visual Studio 2022 x64 CMake Release configure: PASS.
- VST3 and test executable build: PASS.
- CTest regression suite: PASS.
- pluginval strictness level 5: PASS.
- Sample payload restore and validation: PASS, including required license files and WAV SHA-256 manifest checks.
- Portable CI package artifact: generated successfully; artifact name explicitly says `CI-NOT-RELEASE`.
- FL Studio 20 manual host acceptance: NOT RUN / NOT EVIDENCED.

Artifact:
- `POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-PORTABLE-CI-NOT-RELEASE`
- Size: 1,666,323,275 bytes
- SHA-256: `85d99a55453ae28ebe702548ebd273a1999f32bf85d6ad5260372204197028d6`
- Workflow run: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43700659867

This is a successful interim CI build, not a final product release and not proof of FL Studio compatibility.

## 2. Production functionality verified in current source

- The production `SamplePool` uses metadata indexing and the worker-backed `SampleStreamingService`; `SamplerEngine::render` reads through `StreamingVoiceReader` and bounded page-cache leases.
- Pattern mode calls `PatternScheduler::schedule` from the processor block path and dispatches scheduled NoteOn/NoteOff events to the sampler.
- The processor integration test drives MIDI chord changes through the real processor and checks rendered audio from synthetic WAV fixtures.
- Async SFZ loading uses a monotonically increasing generation token to suppress stale requests.
- DAW state serializes the selected SFZ path and attempts to restore it when the file remains available.
- Melody mode retains direct chromatic MIDI-note playback; Pattern transport stop/discontinuity handling is scoped to Pattern mode.
- The APVTS layout includes a polyphony/voices control; FX DSP stages are present in the processor.

These are source/build/test observations, not substitutes for human listening or DAW host acceptance.

## 3. Release blockers — do not mark 99% or FINAL while these remain

### A. Factory preset readiness
The current catalog contains:
- MELODY: 150 authored definitions; 0 verified READY.
- PATTERN: 150 authored definitions; 0 verified READY.

The REV.18 readiness validator requires each READY entry to have:
- implemented runtime and successful plugin-load evidence;
- verified source mapping, relative path, license evidence, SHA-256 and commercial redistribution approval;
- performance validation PASS;
- human audio listening approval PASS.

The release target encoded by the validator is 500 READY MELODY + 500 READY PATTERN. The current 300 authored definitions are not equivalent to playable, validated factory presets. Do not relabel them READY or inflate counts.

### B. Preset runtime and browser acceptance
Still needs evidence for complete real-plugin enumeration/loading of the factory bank, user preset Save/Save As/Rename/Delete, favorites/search/navigation, bank import/export, transactional failure handling and persistence across plugin upgrades. Reconcile the current implementation against `docs/FINAL_INTEGRATION_CHECKLIST.md` before claiming completion.

### C. Pattern performance content
The processor-level scheduler path has integration-test coverage, but the factory PATTERN catalog still awaits its mapped, licensed performance content and human playability/audio approval. Algorithmic scheduling alone does not prove the required recorded-performance library or 150/500 playable patterns.

### D. Sampler lifecycle and resource bounds
The current SamplePool retains explicit limits:
- 128 snapshots per plugin instance;
- 8 streaming library roots per plugin instance;
- 256 cache slots per library root, with the configured page size.

These limits must be stress-tested and either justified as intentional product limits or replaced only after safe snapshot reclamation, cache pressure handling, cancellation and repeated bank-switch tests. Do not simply delete caps or increase them without memory/lifetime evidence.

### E. Host and machine acceptance
Required but not evidenced:
- FL Studio 20 x64 on the supported Windows target: plugin scan, instantiate, MIDI Melody, Pattern host sync, note-off, transport stop/seek/loop, automation, save/reopen and audio-device change.
- Long playback and repeated preset/bank switching.
- Low-spec target CPU/RAM profile, cold start, CPU/RAM measurements, dropout/glitch checks and preset-switch latency.
- Clean-machine portable-folder test and missing-library recovery.

### F. Theme and UI acceptance
The approved NEON STAGE visual system must be checked against the actual rendered plugin, not only SVGs/mockups. The outstanding theme audit notes identify missing preview/hero artwork and registry promotion work for SKIN-2..SKIN-16. Confirm the current registry and assets before packaging; do not ship unregistered drafts or represent mockups as working UI.

### G. Sound asset / legal admission
Reconcile the Sound Hunter registry and every shipped asset against source, license, redistribution scope, relative path and checksum. The current CI package retains upstream sample licenses and hashes, but this does not automatically approve every preset-to-sample mapping or every artwork asset.

## 4. Ordered path toward release

1. Reconcile the real preset runtime/browser against the catalog and parameter registry; implement missing loading and user-preset persistence without changing the approved parameter/control contracts.
2. Complete legal source mapping for each preset; only mark a preset READY after plugin-load, performance and human listening evidence are attached.
3. Build and validate the Pattern performance library; test host tempo/transport and chord transitions in FL Studio.
4. Stress-test streaming cache, repeated bank changes, snapshot lifetime, cancellation and shutdown; close resource-bound defects safely.
5. Finish NEON STAGE visual parity and theme registry/asset admission.
6. Run the full Windows x64 CI pipeline on the final source SHA; retain CTest, pluginval, package manifest and checksums.
7. Perform manual FL Studio 20 and clean-machine/low-spec acceptance; record exact host, OS, hardware, settings and results.
8. Only then create a release-labelled portable VST3 package. No installer, Setup.exe or mandatory setup wizard.

## 5. Branch policy

All production work continues only on:
- `POP-PUNK-GUITAR-ULTIMATE-VST-REV.18` — active development.
- `main` — parent 4.0 production baseline.

Do not use 5.0, REV.17 or other historical/experimental branches as development bases. Do not mass-update or auto-merge them.
