# PPGU Guitar Import & Library Workbench — Stage 2

**Project:** POP PUNK GUITAR ULTIMATE  
**Branch:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`  
**Status:** approved engineering requirement / implementation specification; not yet implemented or runtime-verified.

## User requirement

Stage 2 adds a dedicated guitar-import workflow and a library-processing engine. It accepts individual guitar audio files and complete sample libraries, unpacks archives, organizes their contents into a predictable folder structure, provides non-destructive editing of sample metadata and mappings, and converts audio into supported formats when requested.

## Non-negotiable storage rule: APPEND ONLY

1. Existing source libraries and WAV files are never routinely downloaded again.
2. Existing source files are never overwritten, renamed, moved, deleted, or converted in place by the importer.
3. Import first inventories existing libraries and files. Exact duplicates are detected by content hash; same-name/different-content files are treated as collisions, not duplicates.
4. New material is added only when absent. An existing library is skipped; a partial library is completed by adding only missing files where the source permits selective retrieval.
5. Every operation is staged and can be cancelled. Failed imports do not alter the active library.
6. Editing and conversion create derived copies or metadata revisions; original source audio remains intact and recoverable.
7. No automatic bulk re-download or destructive “replace library” operation.

## Import sources and entry points

- Import one or more WAV files.
- Import a folder containing samples and optional SFZ maps.
- Import ZIP archives containing one or more libraries.
- Import an SFZ file and resolve its referenced samples safely.
- Later extension points: FLAC and other supported lossless sources; user-selected external library folders.

The UI must show an explicit drop target and a Browse/Import button. Dragging a file onto an unrelated panel must not silently import it.

## Processing pipeline

1. **Preflight inventory:** scan the target library root, registry, manifests and existing files. Report present, new, duplicate, collision, missing-reference and unsupported items before writing.
2. **Safe extraction:** unpack outside the audio thread; reject path traversal, absolute paths, unsafe symlinks, excessive file counts and decompression bombs.
3. **Discovery:** identify WAV, SFZ, manifests, readme/metadata and archive structure. Do not assume every audio file is a guitar sample.
4. **Classification proposal:** suggest instrument, tuning/range, articulation, velocity layer, round-robin group, source library and sample role from SFZ data and filenames. Ambiguous files remain in an Unsorted/Needs Mapping area; do not guess silently.
5. **Duplicate/collision resolution:** compare SHA-256 and file metadata. Identical content is referenced rather than copied again. Same filename with different content receives a collision report and safe unique storage path.
6. **Mapping:** parse SFZ regions and sample paths where supported; otherwise offer Auto Map plus a manual Mapping Editor. Validate key/velocity coverage and missing references.
7. **Preview:** audition the selected sample and mapped zones outside the real-time audio callback.
8. **Organize:** copy only new items into the managed library layout and write a manifest/index.
9. **Publish:** atomically register the completed library only after all required checks pass.
10. **Report:** show added/skipped/duplicate/collision/unsupported counts, storage delta and any mapping warnings.

## Managed folder layout

Use a stable library identity and preserve each source library as a distinct namespace. Suggested layout:

```text
Samples/
  Factory/
    <LibraryId>/
      Source/                 # preserved source files and original SFZ
      Maps/                   # normalized/internal maps; original maps retained
      Samples/
        Electric/
          Clean/
          Crunch/
          HighGain/
        Acoustic/
          Steel/
          Nylon/
        Articulations/
          Open/
          PalmMute/
          MutedChug/
          Harmonic/
          Slide/
          Release/
        Unsorted/
      Derived/
        WAV_PCM16/
        WAV_PCM24/
        WAV_FLOAT32/
        FLAC/
        Preview/
      Metadata/
        manifest.json
        regions.json
        conversion_history.json
        checksums.sha256
  User/
    <LibraryId>/
      ...same logical layout...
  Index/
    library_registry.json
```

The importer must not force every source library into this exact taxonomy if that would break relative SFZ paths. Keep an untouched Source tree and use normalized managed references in the internal index. Factory and User roots remain separate.

## Mapping and metadata editor

The editor must support, per sample/region:
- display name and source filename;
- root key, low/high key;
- velocity low/high and layer;
- articulation (open, palm mute, muted/chug, harmonic, slide, release, etc.);
- round-robin group and sequence;
- gain, pan, tuning and optional trim points;
- sample rate, bit depth, channel count and duration;
- library/source provenance and file hash.

Changes to mappings and metadata are versioned in sidecar files; they do not rewrite source WAV data. Provide undo/redo and reset-to-imported-map.

## Audio conversion engine

Conversion is explicit and non-destructive. Supported targets for the initial design:
- WAV PCM 16-bit;
- WAV PCM 24-bit;
- WAV IEEE Float 32-bit;
- FLAC lossless, only when a compatible decoder/encoder is included.

The engine must preserve channel count by default, expose sample-rate conversion as an explicit option, avoid clipping, report peak/true-peak and loudness changes, and retain conversion settings in history. No lossy format should be the default for the source archive. OGG/MP3 may be considered for preview-only derivatives, not as a replacement for master samples.

Batch conversion must support selected files, selected folders or a complete library, with a before/after size estimate and a dry-run mode. Never convert or overwrite original source files in place.

## Runtime and realtime constraints

- File I/O, archive extraction, hashing, waveform generation, decoding and conversion run on worker/background threads.
- No disk I/O, heap allocation, blocking mutex, archive operation, file decoding or conversion inside `processBlock`.
- The sampler receives a completed immutable index/library snapshot through a safe handoff.
- Use bounded queues and memory budgets; do not preload every library sample into RAM.
- Preview must not steal or corrupt the live instrument voice state.
- Cancellation and shutdown must join workers safely and leave the last valid library active.

## Acceptance tests

- Re-importing the same library adds zero duplicate audio files and does not replace any existing file.
- Importing a partially present library adds only missing content.
- Same-name/different-hash files are preserved and reported as collisions.
- ZIP path traversal and oversized archives are rejected.
- SFZ relative paths resolve correctly after import; missing samples are reported.
- Ambiguous samples are routed to Needs Mapping rather than silently assigned.
- Mapping edits persist and can be undone without changing original WAV bytes.
- WAV PCM16/24/Float32 and FLAC conversions round-trip within format-appropriate tolerances.
- Conversion never overwrites the source; cancellation leaves the original and active library intact.
- No import, preview, conversion or file operation blocks the audio callback.
- Test large libraries with bounded memory, cancellation and repeated scans.
- Verify in Windows x64 and FL Studio 20 after implementation.

## Implementation order

**Stage 2A — Inventory and append-only import core:** registry scan, hash index, duplicate/collision detection, safe folder/ZIP import, staging and atomic publish.

**Stage 2B — Guitar classification and organization:** SFZ parser integration, folder taxonomy, Unsorted queue, metadata manifest.

**Stage 2C — Mapping Editor and Preview:** key/velocity/articulation editing, audition, undo/redo.

**Stage 2D — Conversion Workbench:** WAV PCM16/24/Float32 and FLAC, batch jobs, dry-run, conversion history.

**Stage 2E — Verification:** unit tests, stress tests, realtime-safety audit, Windows build, pluginval and FL Studio 20 host validation.

## Current repository findings (audit snapshot)

- `assets/sound_hunter/registry.json` currently contains an empty assets array.
- The REV.17 repository tree audit found no standalone WAV/SFZ/AIFF/FLAC/OGG audio files. Four ZIP archives are present, but their binary contents have not been inspected; they cannot be assumed to contain guitar WAVs.
- Existing sampler infrastructure includes SamplePool, SampleMetadataIndex, SampleStreamingService, PageCacheSlots, StreamingVoiceReader, WavPageReader, SfzParser and related tests. Reuse and extend these components; do not build a parallel sampler.
- `docs/BANK_LIBRARY_SYSTEM_RU.md` already defines folder/ZIP import, SFZ mapping, metadata, preview and realtime constraints. This Stage 2 document extends that architecture with a complete user-facing import/workbench workflow, append-only behavior, collision handling and non-destructive conversion.
- The existing Meta download script currently backs up and replaces an existing destination. It conflicts with APPEND ONLY and must be corrected before it is used again.
- The GitHub repository cannot inspect the user's local `%APPDATA%\PopPunkGuitarUltimate\Samples\SourceLibraries` directory. The installer must perform the inventory on the user's Windows machine before any download.

**Status:** REQUIREMENT RECORDED. No WAV assets were downloaded or modified in this documentation pass. No production importer, editor or converter is claimed as implemented.
