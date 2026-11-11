# POP PUNK GUITAR ULTIMATE — Change Log

This journal records confirmed project decisions, specification changes, implementation work, fixes, and validation results.

## Entry format

- Date:
- Version / area:
- Change:
- Reason:
- Files:
- Status: Planned / Specified / Implemented / Tested / Fixed
- Validation:
- Follow-up:

Only mark a feature **Implemented** when code or content has actually been added. Only mark it **Tested** when a reproducible test has been run and its result recorded. A design decision is not proof of implementation.

---

## 2026-11-11 — Project journal started

- Date: 2026-11-11
- Version / area: Project-wide documentation and repository audit
- Change:
  1. Started this permanent change journal.
  2. Confirmed the active feature branch is `feature/pattern-melody-fx-spec`.
  3. Confirmed the feature specification exists at `docs/CURATED_FEATURE_SPEC.md`.
  4. Recorded that the repository search available in this session did not find build/source entry points such as `CMakeLists.txt`, JUCE processor source files, or C++ headers.
  5. Kept implementation status explicitly separate from the written specification.
- Reason: Maintain a traceable history and avoid losing decisions or claiming unverified progress.
- Files: `docs/CHANGELOG.md`; `docs/CURATED_FEATURE_SPEC.md`
- Status: Journal created; repository audit partial.
- Validation: Read back the current feature specification and inspected the latest specification commit. No VST3 build or audio test was run.
- Follow-up: Locate the actual source tree/project archive and inspect its build system before modifying implementation code.

## Previously recorded specification changes

### 2026-11-11 — Expandable Pattern Engine specification

- Change: Replaced the limited starter-library framing with a large, expandable original pattern-library plan. Added pattern families, metadata requirements, deduplication, chord/tempo/transition QA, and versioned content-pack workflow.
- Reason: The library should grow beyond the 237-pattern reference point without artificial limits or duplicate filler.
- Files: `docs/CURATED_FEATURE_SPEC.md`
- Status: Specified (not implemented).
- Validation: Documentation commit inspected; no pattern content was generated or audio-tested.
- Commit: `a1ff2dc6b253d37d0f263bac8176989779498466`

## 2026-11-11 — Lightweight loading and runtime optimization made a core requirement

- Date: 2026-11-11
- Version / area: Performance architecture; Windows/host compatibility
- Change: Added a non-negotiable performance-first section to the feature specification. It covers fast plug-in startup, selective sample loading/streaming, shared assets, bounded polyphony and voice stealing, optional FX quality/bypass, real-time-safe audio processing, background indexing, UI/skin memory isolation, measured rather than speculative multiprocessing, Eco/Normal/High trade-offs, package size, and a benchmark/acceptance checklist.
- Reason: The instrument must stay simple and lightweight both while loading and during playback on Windows 10 Enterprise 1809 x64 with FL Studio 20. New features must not compromise responsiveness or system load.
- Files: `docs/CURATED_FEATURE_SPEC.md`; `docs/CHANGELOG.md`
- Status: Specified (optimization implementation and benchmarking are not yet verified).
- Validation: Documentation updated and read back through GitHub. No Windows 10 / FL Studio build, profiling, or audio stress test was run in this step.
- Follow-up: Audit the actual source/build tree, implement the audio-thread and sample-loading safeguards, then benchmark cold/warm load, CPU, RAM, preset switching, and low-buffer stability on the target setup.

## 2026-11-11 — Analysis of Electric Sunburst Deluxe applied to our engine

- Date: 2026-11-11
- Version / area: Pattern Engine, chord engine, sample architecture, performance strategy
- Change:
  1. Reviewed the user-provided `Анализ_движка_Electric_Sunburst_Deluxe.md` and extracted design principles relevant to POP PUNK GUITAR ULTIMATE.
  2. Specified compact pattern skeletons (timing, interval/scale degree, duration, articulation and accent) adapted to incoming harmony, instead of storing separate audio for every chord/pattern combination.
  3. Specified a staged guitar-string/voicing mapper, shared sample/voice infrastructure for Melody and Pattern engines, and explicit chord-state/note-off handling.
  4. Added optional Accent-based pattern dynamics, bounded humanization, and deferred MIDI pattern export until core playback is stable.
  5. Added resource cautions: the document's 13.6 GB uncompressed / 6.3 GB compressed figures describe the sample library size, not measured RAM use; pickup/mic layers, velocity layers, round robins and articulations can increase disk, memory and processing costs.
  6. Prioritized shared sample references, bounded cache, selective preload/streaming, capped polyphony, voice stealing, and a shared FX path. Advanced fretboard mapping, extensive transitions, extra pickup/mic layers and Song Builder remain later-stage features subject to profiling.
  7. Set a fair comparison target against Session Guitarist and Nexus 5 using the same machine, Windows 10 Enterprise 1809 x64, FL Studio 20, sample rate, buffer, preset, voice count and playback duration.
- Reason: Reuse useful hybrid pattern/harmony concepts while making low memory use, low CPU load and fast loading the overriding design constraints.
- Files: `docs/CURATED_FEATURE_SPEC.md`; `docs/CHANGELOG.md`; source reference: `Анализ_движка_Electric_Sunburst_Deluxe.md` (provided in conversation)
- Status: Specified (design/documentation only; no engine code was implemented and no benchmark was run).
- Validation: Source document reviewed. RAM usage was not inferred from the source's disk-library size figures. Comparative performance remains unverified until measured on the target setup.
- Follow-up: Audit the actual source/build tree; implement the shared sample cache, voice allocator and pattern-to-harmony mapping; then profile load time, CPU, RAM, disk streaming and audio stability.

## 2026-11-11 — Product direction confirmed: visual identity, standalone VST3, quality and efficiency

- Date: 2026-11-11
- Version / area: Product direction; UI/UX; standalone architecture; performance and sound quality
- Change: Recorded the user's priority that the approved POP PUNK GUITAR ULTIMATE visual style and full functional interface remain central. Confirmed that the instrument is a native standalone VST3 and must not depend on Kontakt 6 / Kontakt Player. Added a useful-upgrades-only rule: changes must improve sound, playability, workflow, reliability or efficiency. Reaffirmed that fast loading, low RAM/CPU, selective sample loading, shared assets, bounded cache/polyphony, optional efficient FX, and UI/skin isolation must be achieved without unnecessarily sacrificing sound quality.
- Reason: The product's differentiator is its own visual identity, its own functionality, and independence from the Kontakt shell; it should be easy to launch and load while still delivering high-quality guitar sound.
- Files: `docs/CURATED_FEATURE_SPEC.md`; `docs/CHANGELOG.md`
- Status: Specified (product direction and acceptance priorities documented; no new engine/UI code or benchmark was performed in this step).
- Validation: Specification updated on the feature branch. Standalone VST3 behavior, FL Studio 20 compatibility, visual regression, audio quality, CPU/RAM and load-time performance still require implementation and testing.
- Follow-up: Audit the actual source/build tree; preserve the approved UI and skin assets; implement/test native VST3 loading, sample/voice engine and performance safeguards; benchmark on Windows 10 Enterprise 1809 x64 with FL Studio 20.

## 2026-11-11 — Fast preset switching and sound preloading promoted to high priority

- Date: 2026-11-11
- Version / area: Preset browser; Sounds Bank; sample cache; loading performance
- Change: Added a dedicated high-priority requirement for fast preset switching and sound availability. Preset state application is separated from sample asset loading; the browser uses a lightweight preset/sample index; shared samples are reused across presets; only essential attack regions and mappings are prepared ahead where practical; long sustains and uncommon articulations are streamed/loaded on demand. Added asynchronous background preparation, bounded preload-ahead/cache eviction, safe handling of active voices during switching, and cold/warm cache acceptance tests. Added separate timing metrics for browser open, state apply, first playable note, required-asset readiness and warm-cache switching.
- Reason: Large Kontakt-hosted instruments can take noticeable time to load presets and their sounds. Our standalone VST3 should minimize time-to-first-sound and make cached preset changes feel immediate without loading the entire library into RAM.
- Files: `docs/CURATED_FEATURE_SPEC.md`; `docs/CHANGELOG.md`
- Status: Specified (architecture requirements documented; no loading code or timing benchmark was implemented in this step).
- Validation: Documentation update committed. Fast loading remains an acceptance target to verify on Windows 10 Enterprise 1809 x64 / FL Studio 20 with cold-cache and warm-cache tests.
- Follow-up: Implement lightweight preset indexing, asynchronous asset preparation, shared bounded sample cache, preload-ahead and safe voice/resource handoff; benchmark first playable note and preset-switch latency.

## 2026-11-11 — Source-tree audit and release integration blockers

- Date: 2026-11-11
- Version / area: Repository audit; VST3 build; presets; Pattern Engine; skins; sample-bank runtime
- Change: Audited the recursive source tree, CMake target, Windows CI workflow, processor/editor, sampler and current release checklists. Confirmed native JUCE 7.0.12 / C++20 VST3 source, SFZ/WAV streaming components, bounded page cache, PatternScheduler dispatch, a 300-definition catalog, and a Windows workflow that downloads license-cleared sample libraries, builds/tests the VST3 and packages Samples + Presets. Recorded the actual gaps: factory preset selection currently applies only four tone values and mode (not sampleProfile/full parameter state); Pattern mode currently uses eight hard-coded rhythm masks rather than the full authored pattern bank; no runtime skin/theme manager or complete switchable-skin folder is present in the audited tree; SamplePool retains snapshots and has explicit 128-snapshot/eight-library-root limits; target-machine CPU/RAM and FL Studio 20 acceptance remain unmeasured.
- Reason: Avoid treating catalog entries, source files or an old CI artifact as a complete playable release. Establish an evidence-based implementation order before making risky changes to real-time audio and sample lifetimes.
- Files: `docs/IMPLEMENTATION_AUDIT_2026-11-11_RU.md`; `docs/CHANGELOG.md`
- Status: Audited / Specified (source inspection only; no new engine/UI code, Windows build, or audio benchmark in this step).
- Validation: Recursive Git tree and relevant source/build/configuration files inspected. Current PatternScheduler call path confirmed in `PluginProcessor.cpp`. Existing workflow is configured for Windows Release, CTest, pluginval strictness 5 and packaging; no workflow run was triggered by this audit-only commit.
- Follow-up: Implement full preset-to-bank integration, replace placeholder pattern masks with versioned original pattern data, implement the skin manager, then run Windows CI and manual FL Studio 20 acceptance. Preserve current streaming safety limits until stress tests justify changing them.

---

## Rules for future entries

1. Add a dated entry for every meaningful user-approved requirement, code/content change, bug, fix, or test.
2. Record the exact affected paths and commit SHA when available.
3. For bugs, record reproduction steps, expected behavior, actual behavior, fix, and regression test.
4. For tests, record environment (Windows version, FL Studio version, plugin build, sample rate, buffer size), result, and any logs.
5. Never mark a planned or specified feature as implemented.
6. Do not delete earlier entries; append corrections with a new dated entry.
