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

---

## Rules for future entries

1. Add a dated entry for every meaningful user-approved requirement, code/content change, bug, fix, or test.
2. Record the exact affected paths and commit SHA when available.
3. For bugs, record reproduction steps, expected behavior, actual behavior, fix, and regression test.
4. For tests, record environment (Windows version, FL Studio version, plugin build, sample rate, buffer size), result, and any logs.
5. Never mark a planned or specified feature as implemented.
6. Do not delete earlier entries; append corrections with a new dated entry.
