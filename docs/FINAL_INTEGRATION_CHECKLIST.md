# Integration Preparation — VST3

This document is the implementation hand-off checklist for the final integration phase. It deliberately separates checked-in assets/specifications from runtime features that are not yet implemented.

## Confirmed repository assets

- `presets/factory_bank_300.json`: authored catalog with 150 Melody and 150 Pattern definitions.
- `presets/examples/Example-Melody.ppgm` and `Example-Pattern.ppgp`: portable-format examples.
- `docs/PRESET_FILE_SYSTEM.md`: approved browser, folder and file-format behavior.
- `tools/validate_factory_bank.py`: validates the factory catalog.
- `tools/validate_preset_files.py`: validates portable preset examples.

## Integration sequence

### 1. Runtime preset model
Create one typed in-memory preset model shared by factory JSON and user files. Required fields: stable ID, format version, mode, name, tags, sample profile and validated parameter map. Add mode-specific Melody and Pattern payloads. Never parse files on the audio thread.

### 2. Parameter binding
Audit the existing JUCE AudioProcessorValueTreeState parameter IDs and map catalog controls to real parameter IDs. Do not assume display labels are parameter IDs. Reject or migrate unknown fields; clamp only documented numeric ranges. Loading must be transactional and preserve the previous state on failure.

### 3. File-system service
Implement scanning on a worker/message thread, not processBlock. Roots:
- Factory: installed with product content, read-only.
- User: `%APPDATA%/L3XTOWN/POP PUNK GUITAR/Presets/User`.
- Banks: `%APPDATA%/L3XTOWN/POP PUNK GUITAR/Banks`.
- Samples: separately selected library roots.

Use `.ppgm`, `.ppgp`, and `.ppgbank`. Resolve sampleProfile through a library manifest; never store absolute sample paths in portable presets.

### 4. Browser UI
Add mode filter, folder/category and artist navigation, search, favorites, double-click load, previous/next, Save, Save As, Rename, Delete, Import Bank, Export Bank and Rescan. Factory content is immutable. User files must survive plugin upgrades.

### 5. Melody engine
Preserve one MIDI note = one chromatic guitar note. Implement correct note-on/off and release, velocity-layer selection, round robin, open/palm-muted articulations, polyphony limits and electric/acoustic profiles. Validate mapped zones and sample root keys; missing libraries must be reported visibly.

### 6. Pattern engine
Create a dedicated performance scheduler driven by host playhead tempo/transport. MIDI chord input selects a guitar voicing; the engine schedules down/up strokes, palm mutes, dead strokes, accents, gate lengths and humanization. Use recorded chord/strum performances mapped by voicing, stroke, velocity and articulation. A single-note multisample bank is not a substitute for chord-strum recordings. Keep scheduling bounded and deterministic; no file I/O or allocation in processBlock.

### 7. Bank format
A `.ppgbank` is a versioned archive with a manifest and preset files. Import must preview contents, reject traversal paths and unsupported versions, and never overwrite user content without confirmation. Samples are referenced by logical library ID, not silently copied.

### 8. Release gates
- Catalog and example validators pass.
- All 300 factory presets are enumerated by the real browser and load without errors.
- Save/reopen, import/export, favorites and search pass.
- Melody and Pattern are tested with MIDI keyboard and FL Studio Piano Roll.
- Host tempo/transport sync and chord changes are tested.
- Sample license files, SFZ maps and WAV SHA-256 manifest are packaged.
- pluginval strictness 5 passes.
- FL Studio 20+ host test is recorded separately; pluginval does not replace DAW testing.
- Clean-machine install and missing-library recovery pass.

## Current limitation
The checked-in catalog and examples are data/specification, not a claim that the runtime browser, file loader or Pattern performance engine already exists. Do not label the release complete until all release gates above have evidence.

## Current implementation evidence — 10 Nov 2026

- [x] Windows x64 Release build, full CTest, VST3 locate, pluginval strictness 5 and package/upload: run `42843030370` SUCCESS.
- [x] Additional MSVC v143 VST3/sampler validation: run `42843030228` SUCCESS.
- [x] Scheduler-to-SamplerEngine module integration test included in CTest and passed.
- [x] M4 AutoChords/FingeringGenerator scale-mapping foundation passed run `42843127831`; subsequent overflow-hardening commits require fresh CI.
- [x] M5 StyleEngine foundation passed run `42840410645`.
- [ ] M2 velocity crossfade latest branch run `42843931173` must pass before merge.
- [ ] M1 FL Studio 20 save/reopen, automation, missing-library recovery, editor-close/load, device-change and long-playback checks.
- [ ] M2 palm-mute mapping, real-library click/pop stress, cache/concurrency pressure and audio-thread allocation/lock/I/O proof.
- [ ] M3 FL Studio 20 play/stop/loop/seek/tempo/chord transitions/live MIDI checks.
- [ ] M4/M5 runtime binding into Pattern Scheduler, Processor, preset/state and UI.
- [ ] Unified integration branch and final combined release run.

CI success is not DAW acceptance. Keep the release marked incomplete while any host/runtime item above remains unchecked.
