# POP PUNK GUITAR ULTIMATE — implementation audit (2026-11-11)

Branch audited: `feature/pattern-melody-fx-spec`  
Target: Windows x64, Windows 10 Enterprise 1809, FL Studio 20  
Purpose: distinguish actual checked-in implementation from catalog/specification and release aspirations.

## 1. Confirmed project foundation

- Native JUCE 7.0.12 / C++20 VST3 instrument target exists in the repository-root `CMakeLists.txt`.
- Windows x64 GitHub Actions workflow configures a VS 2022 Release build, runs CTest, runs pluginval strictness level 5, and packages a VST3 bundle with Samples and Presets.
- Source includes `PluginProcessor`, `PluginEditor`, SFZ parser, metadata index, WAV page reader, bounded page cache, background streaming service and streaming voice reader.
- A factory catalog file contains 300 authored definitions: 150 Melody and 150 Pattern. The catalog explicitly states that entries are tonal/performance targets awaiting runtime integration and sample validation.
- A fixed-size PatternScheduler exists and is called by the current processor in Pattern mode. The current Pattern mode builds events from eight hard-coded rhythm masks and held MIDI notes; this is a functional scheduler path, but it is not yet the large curated, chord-adaptive pattern library described in the product specification.
- The sampler supports asynchronous SFZ loading and streams WAV pages through a bounded cache. The current cache defaults to 256 pages × 4096 frames × 2 channels = about 8 MiB per library root; up to eight roots are configured. This is cache capacity, not a measurement of total plugin RAM.

## 2. Confirmed gaps before a complete release

### P0 — Preset-to-sound integration
- The editor parses the embedded 300-entry catalog.
- Selecting a factory entry currently applies only four numeric tone values (drive, tone, reverb, output) and changes Melody/Pattern mode.
- It does not yet resolve `sampleProfile` to a selected/installed library, load the appropriate SFZ bank, apply a full preset parameter state, or guarantee that the selected preset is immediately playable.
- Therefore, the 300 entries are not yet 300 fully playable factory presets.

### P0 — Pattern content
- Pattern mode currently synthesizes event sequences from eight hard-coded rhythm masks and the held MIDI notes.
- It does not yet load a broad original pattern content bank with tagged variations, chord/voicing adaptation and section-specific phrases.
- Do not claim a large factory pattern library until authored content is integrated and tested.

### P0 — Visual skin system
- The current editor has a dark charcoal / magenta visual base and a functional set of controls.
- The audited source tree does not contain a runtime theme/skin manager or the requested set of switchable full-interface skins.
- A folder of static theme pictures alone would not implement skin switching. Theme definitions, asset loading, palette application, UI state preservation and safe asset release must be implemented together.

### P1 — Sample-bank lifecycle and memory
- Sample data uses background page streaming and a bounded cache, which is the correct direction for large banks.
- SamplePool retains immutable snapshots until destruction and currently has explicit limits of 128 snapshots and eight library roots. This avoids unsafe pointer reclamation but can prevent long-running preset/library workflows.
- Repeated bank replacement, rapid successive requests, cache pressure, active-voice handoff and memory ceilings need stress tests before changing these safety limits.
- Preset switching must be separated from library replacement; ordinary tone/pattern preset changes should not trigger a full sample-bank scan.

### P1 — Host acceptance
- CMake/CI can verify compilation and automated tests, and pluginval can verify plugin protocol behavior.
- Manual validation remains required in FL Studio 20 on Windows 10 Enterprise 1809: MIDI note-on/off, held notes during preset changes, transport start/stop/seek, project save/reopen, automation, missing-bank recovery, low buffer sizes and long playback.
- CPU, peak RAM, first-playable-note time and warm/cold preset-switch latency have not yet been measured on the target machine.

## 3. Current build/package pipeline

The checked-in Windows workflow downloads the stated redistributable Meta Open Sound and FreePats guitar libraries, retains source licenses/SFZ maps and SHA-256 manifests, builds the VST3, runs CTest and pluginval, and packages:
- the VST3 bundle;
- `Samples/` with the downloaded sound bank;
- `presets/` including the factory catalog and portable examples;
- installation/content notes.

The current repository tree does not contain the downloaded WAV bank as committed source assets; the workflow assembles it during CI. The current tree also has no completed switchable-skins asset/runtime folder. Consequently, a complete package artifact depends on a successful workflow run and does not yet imply that every catalog preset or every requested skin is implemented.

## 4. Next implementation order

1. Keep the approved visual language; implement the skin manager without replacing the approved main layout.
2. Build a typed preset model and map every supported catalog field to real APVTS parameters; resolve sample profiles against an installed bank manifest. Keep state application fast and do not reload shared samples unnecessarily.
3. Separate preset-state switching from SFZ/library loading; add request generations, readiness reporting and safe voice/resource handoff.
4. Replace the eight-mask Pattern placeholder with versioned, original pattern data and chord/voicing mapping; preserve the fixed-size real-time scheduler.
5. Stress-test page streaming, cache pressure, active voices and repeated bank/preset changes before relaxing snapshot/root limits.
6. Build/package through Windows CI; then perform manual FL Studio 20 acceptance and target-machine profiling.

## 5. Status rule

This audit is source inspection, not a build or audio test. Features are not marked complete based only on a source file, preset catalog, screenshot, or successful CI. The release is complete only after the relevant implementation, tests, package artifact and host acceptance are all recorded.
