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

---

## Rules for future entries

1. Add a dated entry for every meaningful user-approved requirement, code/content change, bug, fix, or test.
2. Record the exact affected paths and commit SHA when available.
3. For bugs, record reproduction steps, expected behavior, actual behavior, fix, and regression test.
4. For tests, record environment (Windows version, FL Studio version, plugin build, sample rate, buffer size), result, and any logs.
5. Never mark a planned or specified feature as implemented.
6. Do not delete earlier entries; append corrections with a new dated entry.
