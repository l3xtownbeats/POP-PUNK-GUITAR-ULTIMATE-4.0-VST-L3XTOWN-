# Library Manager Ultimate — approved requirements and acceptance contract

**Status:** APPROVED / FROZEN REQUIREMENTS  
**Product:** POP PUNK GUITAR ULTIMATE (PPGU)  
**Scope:** Portable Library Manager, shared library database, VST3 integration, and end-to-end sound availability verification.  
**Approval:** User explicitly approved and confirmed this specification on 2026-11-18.  
**Important:** This document records requirements. It does not claim that the manager, database integration, or end-to-end tests are already implemented.

## 1. Non-negotiable product outcome

A library is not considered ready merely because files have been scanned or sorted. It is READY only after the VST3 can resolve the registered assets, load the relevant mappings, and pass the required playback/availability checks. The system must not silently omit a library, WAV, SFZ region, note, velocity layer, articulation, or preset dependency.

The acceptance chain is:

`DISCOVER → SCAN → ANALYZE → CLASSIFY → REGISTER → VERIFY FILES → VERIFY MAPPINGS → VERIFY PLUGIN ACCESS → PLAYBACK TEST → READY`

Any failed or uncertain stage must produce a visible diagnostic and a non-READY state.

## 2. Approved portable folder layout

The project may live on any drive, directory, or external disk. No hard-coded drive letters or absolute installation paths are allowed. The root must be resolved relative to the manager executable/script.

```text
POP PUNK GUITAR ULTIMATE/
└── Samples/
    ├── FACTORY/
    ├── Import Sample Guitar/
    │   ├── START SCANNER.bat
    │   └── Library Manager.exe
    └── Other/
```

- `Import Sample Guitar` is the sole incoming location for new libraries, archives, banks, WAV, SFZ, and supported materials.
- `FACTORY` contains validated, correctly mapped, technically usable material whose rights permit the intended use/distribution.
- `Other` contains unknown, damaged, incomplete, incompatible, unclear-license, or review-required material.
- Do not create additional top-level directories under `Samples` without explicit user approval.
- Preserve original source material. Never delete or overwrite originals automatically.
- Do not automatically delete exact or near-duplicates.
- Only clear/move an incoming item after a successful, journaled, verified operation. Failed/uncertain items remain recoverable.

## 3. Two entry points, one shared engine

The same analysis/classification engine, database schema, IDs, validation rules, and diagnostics must serve:
1. The standalone portable manager launched by `START SCANNER.bat` or `Library Manager.exe`.
2. The manager/import workflow exposed from the VST3 interface.

The plugin must not scan large libraries, hash files, or perform blocking filesystem operations in the audio callback. Use worker threads/processes and a safe IPC/service boundary. The audio thread may consume only already-published, immutable/atomic-ready runtime data.

## 4. Discovery and format validation

Recursively inspect the incoming folder and supported archives. Identify libraries, instruments, SFZ definitions, WAV assets, mappings, metadata, documentation, license files, and relationships between them.

### WAV/audio validation
- Parse RIFF/WAVE chunks safely; validate encoding, sample rate, bit depth, channels, frame count, duration, and file integrity.
- Detect truncated, malformed, unsupported, silent, or unreadable files.
- Preserve metadata and source identity.
- Record exact SHA-256 checksums. Audio similarity is a separate, non-destructive signal.

### SFZ validation
- Parse regions and supported opcodes, including sample references, key/velocity ranges, root key, groups, sequence/Round Robin, crossfades, tuning, gain, pan, offsets, loops, and release behavior.
- Resolve relative paths, `default_path`, includes, and supported macros/wildcards according to documented parser capabilities.
- Verify every referenced sample exists and is readable.
- Detect missing references, invalid regions, overlapping/conflicting mappings, unsupported opcodes, and incomplete key/velocity coverage.
- Attempt path recovery only when there is a unique/reliable match; otherwise report ambiguity and require review.
- Never silently discard unsupported mapping behavior.

## 5. Musical and instrument analysis

Classify only what can be supported by source metadata or reliable analysis. Never invent uncertain information from filenames.

Supported dimensions include:
- Instrument family: Electric Guitar, Acoustic Guitar, Nylon Guitar, Steel Guitar, Bass.
- Tone: Clean, Crunch, Overdrive, Distortion, High Gain, Ambient.
- Role: Rhythm, Melody, Lead, Solo, Riff, Chords, Power Chords.
- Articulations when actually mapped: Palm Mute, Sustain, Muted, Slide, Harmonics, Staccato.
- Techniques when actually represented: Downstroke, Upstroke, Alternate Picking, Strumming.
- Genres/styles: Pop Punk, Skate Punk, Punk Rock, Melodic Punk, Emo, Alternative Rock, Post-Hardcore, and other supported tags.
- Musical key/mode only when reliably determined: C–B, Major/Minor.
- MIDI root key and low/high key ranges.
- Velocity layers, dynamic layers, Round Robin groups, strings, positions, regions, and sample groups.

Use `UNKNOWN` / `REVIEW_REQUIRED` for uncertain values. One asset may have multiple metadata tags; do not physically duplicate the same WAV into many genre/key folders just to represent multi-tag classification.

## 6. Database and stable asset identity

Maintain a persistent, versioned library database with stable asset IDs. At minimum store:
- Asset ID, source/library identity, relative path, file type, checksum, file metadata.
- License evidence, attribution/source, rights decision, and review status.
- SFZ/map identity and region references.
- MIDI key/root/ranges, velocity ranges/layers, Round Robin data.
- Instrument family, tone, role, genre/style, articulation, technique, string/position where known.
- Validation state, diagnostics, timestamps, and manager/schema version.
- Availability flags/capabilities for Melody and Pattern.
- Preset dependencies using stable IDs, not fragile absolute paths.

Database updates and file moves must be transactional or recoverable. Maintain operation journal, resume support, cache, and rollback/recovery. Verify checksums before/after moves and check available disk space.

## 7. Melody and Pattern compatibility maps

The manager must explicitly determine which assets/mappings are suitable for each playback mode.

- **Melody:** single-note ranges, lead/riff/hook lines, octave parts, picked notes, sustain and melodic articulations where available.
- **Pattern:** chord/power-chord structures, rhythm/riff layers, downstroke/upstroke/strumming and palm-muted patterns where available.

Do not mark a capability available merely because a filename or library title mentions it. Confirm the mapping and required samples. A sound may be available to one mode and unavailable to the other. Unsupported capabilities must be reported, not fabricated.

## 8. Mandatory plugin compatibility and playback gate

This is a blocking release/READY requirement.

For every registered asset and mapping, verify:
1. Database record and stable ID exist.
2. Resolved relative path points to the final FACTORY location.
3. File exists, checksum matches, and audio reader can open it.
4. SFZ region references resolve to the intended WAV assets.
5. Key/root/MIDI ranges are valid and coverage gaps are identified.
6. Every declared velocity layer and Round Robin sequence is accounted for.
7. Articulations and techniques are exposed only when real mappings support them.
8. Melody/Pattern capability flags match actual mapping support.
9. VST3 registry/loader can resolve and load the asset without blocking the audio thread.
10. Required note/velocity/articulation playback probes produce valid audio, not silence or missing-sample fallbacks.

A library is **READY** only if all mandatory checks pass. Partial success must be reported as **PARTIAL** with exact missing capabilities. Any missing file, broken path, failed load, missing note/layer, invalid map, or playback failure prevents full READY status.

### Plugin startup diagnostics
On plugin open, validate the library database/version and the dependencies required by the selected preset. Display:
- `LIBRARY STATUS: READY` when verified.
- `LIBRARY STATUS: ATTENTION REQUIRED` when there are errors, unresolved items, or incompatible data.

Never silently skip a missing asset. Show the affected library, asset ID/name, file/map, MIDI note or velocity range (when applicable), and actionable reason. Plugin must remain usable with verified assets while clearly reporting unavailable assets.

## 9. Reports and audit trail

After each scan, produce a report containing:
- Libraries discovered and processed.
- WAV files discovered, validated, registered, and rejected.
- SFZ files parsed and regions checked.
- Missing/broken sample references.
- MIDI notes and range coverage, including missing notes.
- Velocity layers and Round Robin coverage.
- Articulations/techniques actually available.
- Assets available to Melody and Pattern.
- Exact duplicates and audio-similarity candidates (non-destructive).
- License-cleared, unclear-license, and review-required items.
- File move/copy operations, checksums, warnings, errors, and recovery state.
- Plugin compatibility and playback probe results.

## 10. Licensing and provenance

Inspect LICENSE, README, EULA, credits, source page, and package documentation. The existence of a license file alone is not proof of redistribution permission. Download/evaluation permission is not redistribution permission. If redistribution/inclusion rights are unclear or disallowed, do not bundle the source samples into a distributable plugin/library; route the material to `Other` for review and record the evidence.

## 11. Safety, reliability, and performance

- Portable Windows 10 x64 operation; end user must not need to install Python.
- Deliver a real EXE/BAT workflow; GUI polish is secondary to correct execution.
- Multithreaded scanning, hashing, and analysis; bounded resource use.
- Cache and resume interrupted scans.
- Journal all mutations; support rollback/recovery.
- Dry-run/preview before file moves.
- Preserve source files and never overwrite silently.
- Do not scan or mutate files from the plugin audio callback.
- Do not mark features as complete without implementation and test evidence.

## 12. Acceptance test strategy

Use synthetic fixtures and copies of real libraries; never test destructive operations on the only originals. Include cases for:
- Valid and malformed WAV, unsupported format, truncated file, silent audio.
- Valid SFZ, missing sample, broken relative path, default_path, include, wildcard/macro cases supported by parser, invalid key/velocity ranges, incomplete note coverage.
- Multiple velocity layers and Round Robin sequences.
- Missing individual notes and articulations.
- Duplicate checksums and similar audio.
- Unclear/forbidden redistribution license.
- Relocated project root and external drive letter change.
- Interrupted scan, restart/resume, rollback, disk-space failure.
- Database version mismatch/corruption and plugin startup diagnostics.
- End-to-end VST3 loading and audible playback probes for registered notes/layers in Melody and Pattern.

A test result must distinguish **implemented**, **unit-tested**, **integration-tested**, and **verified in a real DAW host**. Passing parser tests alone does not prove plugin playback. Do not claim DAW verification until actually performed.

## 13. Release gate

No library release, preset dependency, or project milestone may be declared complete solely because scanning/classification finished. The final gate is successful end-to-end VST3 asset resolution and playback verification, with no unexplained missing assets or notes. Uncertain items remain clearly marked and excluded from READY.

---

## Approval record

User approval received: **“Утверждаю подтверждаю фиксирую”** (2026-11-18).

This specification is the approved baseline. Any change to the folder layout, preservation rules, classification contract, database identity model, compatibility gates, or licensing restrictions requires explicit user approval.

## 14. In-plugin scanner: identical workflow and triggers

The scanner exposed inside the VST3 must be the same Library Manager engine, not a reduced second implementation and not a separate set of classification rules.

The in-plugin scanner must be callable through:
- An explicit **SCAN / IMPORT GUITAR LIBRARY** button.
- Adding or selecting guitar sound libraries/files through the plugin's import workflow. The user action must initiate the same discovery, validation, classification, registration, and compatibility checks.
- Re-scan / refresh when the user requests it, including after files have been added or relocated.

The in-plugin workflow is:
`USER ADDS LIBRARY OR PRESSES SCAN → SHARED SCANNER ENGINE → ANALYZE → CLASSIFY → REGISTER IN SHARED DATABASE → VERIFY FILES AND MAPPINGS → UPDATE PLUGIN LIBRARY VIEW → REPORT RESULT`

Requirements:
- Standalone manager and in-plugin scanner use the same engine, schema/version, stable IDs, metadata, classification, license policy, and READY gate.
- A library imported from the plugin must become visible to the standalone manager, and vice versa, because both use the same project root and shared database.
- The plugin must show scan progress, current item/library, warnings, errors, and a final summary. It must not imply success when only the file selection dialog completed.
- Scanning, hashing, audio analysis, archive extraction, database writes, and filesystem changes run outside the audio callback on worker threads/processes.
- Existing assets and presets must not be invalidated by rescanning. Use stable IDs, transactional database updates, journaling, and safe publication of completed scan results.
- If the user adds one file or one library, perform an incremental scan where possible, while checking dependencies and affected mappings. Provide a full rescan command for recovery.
- After import, run the same VST3 compatibility checks: resolve the registered asset, validate SFZ/WAV mappings and note/velocity coverage, and perform supported playback probes before marking the imported content READY.
- If a scan fails or is interrupted, preserve the previous valid library state and allow retry/resume. Never silently replace a working mapping with an incomplete one.
- The plugin's library browser must refresh from the shared database after a successful scan and clearly distinguish READY, PARTIAL, and ERROR/REVIEW_REQUIRED assets.

This section makes the plugin's import button and add-library workflow first-class entry points to the exact same scanner mechanism used by the standalone manager.

## 15. Mandatory scan completion certificate and reconciliation report

A scan is not considered complete merely because the progress bar reaches 100% or the scanner finishes traversing folders. After every scan/import/distribution run, the Library Manager MUST produce a persistent, human-readable completion report that proves what was discovered, analyzed, validated, classified, moved, and left unresolved.

### 15.1 Required summary
The report must include:
- run ID, start/end timestamps, elapsed time, scanner/database schema versions, selected Samples root;
- libraries/archives/directories discovered;
- total files discovered, supported files, unsupported files, files actually analyzed, files skipped (with reason);
- WAV/audio validation counts, SFZ/map counts, resolved and missing sample references;
- detected note/key ranges, velocity layers, round-robin groups, articulations and techniques only where verified;
- exact-duplicate SHA-256 groups and audio-similarity candidates, clearly distinguished;
- license states: verified for permitted use, prohibited, unknown, and review required;
- counts and byte totals successfully placed in FACTORY, placed in Other, already correctly registered, and not moved;
- every warning, error, skipped item, and unresolved item with library name, relative path, reason, and current location;
- final verification results, including file existence, checksum, SFZ dependency resolution, database registration/stable IDs, and plugin-readiness checks where implemented.

### 15.2 Full accounting / reconciliation
Every discovered input item must end in exactly one explicit outcome: successfully registered and placed in FACTORY; quarantined/placed in Other; recognized as an already-processed item; or unresolved and retained in Import Sample Guitar. No item may silently disappear from the report or be counted as successful merely because it was encountered.

The report must reconcile totals and show why discovered, analyzed, classified, moved, skipped, and unresolved counts differ. Folder emptiness alone is not proof of successful processing.

### 15.3 Completion states
- COMPLETE: all discovered items have an accounted-for outcome, all required analysis and post-move verification passed, and no unresolved items remain in Import Sample Guitar. Items intentionally classified to Other may still exist; their reasons must be listed.
- COMPLETE WITH WARNINGS: all items are accounted for and safely placed, but warnings/review items remain (for example unknown licenses, unsupported formats, or incomplete sample coverage). Such items must not be presented as FACTORY-ready.
- INCOMPLETE: scan interrupted, errors prevented full analysis or safe placement, reconciliation failed, or unresolved items remain in Import Sample Guitar. The report must identify the exact remaining items and preserve them in place.
- FAILED: a fatal error prevented a trustworthy report or safe completion; preserve prior valid state and all unprocessed source materials.

### 15.4 User-facing confirmation
At the end of each run, show a clear summary in the manager UI/console and save a detailed report (human-readable plus machine-readable, e.g. JSON/CSV). Provide OPEN REPORT and, where safe, RETRY FAILED/REVIEW ITEMS actions. The confirmation must explicitly state:
- what was scanned;
- what was analyzed and validated;
- how many items were placed in FACTORY and why;
- how many were placed in Other and why;
- what was not processed and why;
- whether post-move checks passed;
- whether the run is COMPLETE, COMPLETE WITH WARNINGS, INCOMPLETE, or FAILED.

Never display "everything checked" or "COMPLETE" unless the report's accounting and required verification checks substantiate that statement. Distinguish implemented, unit-tested, integration-tested, and verified in a real Windows/DAW environment.

### 15.5 Safe cleanup of Import Sample Guitar
Successfully processed source items may be removed from Import Sample Guitar only after their destination files, checksums, dependencies, and database records have been verified and the report records the outcome. Unknown-license, unsupported, corrupt, incomplete, or otherwise rejected materials must be routed to Other with their original files and supporting context preserved. Any item that cannot be safely moved must remain in Import Sample Guitar and force INCOMPLETE status. Never delete source material as a substitute for successful classification or verification.

### 15.6 Normal exclusions are not critical errors

The user-facing completion report must distinguish ordinary library curation from genuine processing failures.

Files that do not belong in the factory guitar library (irrelevant formats, unrelated content, non-guitar material, junk, unsupported or unusable samples, incomplete/unmapped content, and materials whose license does not permit factory redistribution) are normal exclusions or review items. They must NOT inflate the critical-error count merely because they were correctly rejected.

For a fully completed run, the preferred summary is:
- eligible items successfully validated and placed in FACTORY: count;
- non-eligible / irrelevant / junk items excluded or routed to Other: count;
- items awaiting manual license or technical review: count;
- genuinely failed processing operations: count;
- critical errors: 0, provided no actual integrity, database, filesystem, data-loss, or reconciliation failure occurred.

The report must include a concise plain-language confirmation, for example:
"Сканирование и распределение завершено. В FACTORY добавлено: N файлов / M библиотек. Не добавлено в заводскую библиотеку как неподходящие материалы или мусор: K файлов (перечень доступен в отчёте). Критических ошибок: 0."

Do not call excluded items "errors" when the scanner successfully identified and safely excluded them. Do not hide real failures: unreadable files, checksum mismatches, failed moves, unresolved dependencies, database write failures, or unaccounted inputs must remain visible as processing failures and prevent a COMPLETE status where they compromise correctness.

The top-level status may be COMPLETE WITH EXCLUSIONS when all discovered inputs have been accounted for, all eligible materials passed required checks and were safely placed, and all excluded items are safely retained in Other or otherwise accounted for. Use INCOMPLETE only for genuinely unfinished/unaccounted processing, not merely because irrelevant files were excluded.
