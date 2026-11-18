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
