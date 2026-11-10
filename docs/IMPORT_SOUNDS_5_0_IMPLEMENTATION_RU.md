# IMPORT SOUNDS — implementation specification for PPGU 5.0

Status: implementation branch created from the intact `main` source tree. This document is a design/acceptance contract; it does not claim the importer is already coded.

## Product decision: preserve originals + build an optimized cache

Never destructively convert or replace user files. Import copies the source into the bank's immutable Original area, then creates a derived playback cache. The source file remains available for re-import and future decoder improvements. Cache files are disposable and reproducible.

Suggested user-data root (Windows):
`%LOCALAPPDATA%/L3XTOWN/POP PUNK GUITAR ULTIMATE/`

Layout:
- `Library/Factory/` — bundled, license-cleared factory banks; read-only from the plug-in UI.
- `Library/User/<bank-id>/Original/` — copied source files and SFZ documents.
- `Library/User/<bank-id>/Cache/` — normalized playback assets.
- `Library/User/<bank-id>/bank.json` — bank metadata, source hashes, conversion settings, diagnostics.
- `Library/Index/` — rebuildable metadata index.
- `Library/PreviewCache/` — disposable short previews/waveform peaks.

## Supported import formats

- WAV and AIFF: decode and normalize container representation only; never normalize loudness automatically.
- FLAC: lossless decode to cache PCM.
- OGG Vorbis: decode to cache PCM.
- MP3: optional decoder capability; if the build does not include a supported MP3 decoder, report unsupported format instead of pretending import succeeded.
- SFZ: preserve the original SFZ and referenced source files; parse supported opcodes into an instrument map. Unsupported opcodes and missing files are reported in an import report. Do not flatten a mapped instrument into unrelated one-shots.
- ZIP: safe extraction to a temporary staging folder, path traversal protection, archive-bomb limits, then normal file/SFZ discovery. Never execute files.

## Playback cache format

Preferred cache: WAV PCM/float at the source sample rate and channel count, with 32-bit float internal representation where practical. Do not upsample by default. Preserve sample rate and channel count; resample only when an engine constraint requires it. Keep original bit depth and source hash in metadata. A lossless source (WAV/AIFF/FLAC) must not be lossy-reencoded. OGG/MP3 are necessarily decoded from their lossy source; the cache must not encode them again to a lossy format.

## Import pipeline

1. Choose files/folders/archive; show detected formats and estimated size.
2. Copy to a temporary staging directory; reject unsafe paths, symlinks escaping the bank root, oversized archives, corrupt files and unsupported formats.
3. Parse SFZ and resolve every sample path relative to the SFZ root. Keep key/velocity ranges, root keys, groups, sequence/round-robin and supported articulation/loop data.
4. Hash original files (SHA-256), detect duplicates, and offer reuse of existing assets by content hash.
5. Decode on a worker thread (never on audio callback); write cache files atomically to a temporary name, then rename on success.
6. Write bank.json and an import report including warnings, unsupported opcodes, missing references and conversion details.
7. Add the bank to the metadata index only after the complete transaction succeeds. On cancel/failure, clean temporary files and leave existing banks unchanged.
8. Refresh browser and preview cache. Do not preload the whole library into RAM.

## Audio-thread and performance rules

- No file I/O, decoding, heap allocation, blocking mutexes or directory scans in processBlock/audio callback.
- Background indexing and cache generation with cancellation and bounded worker count.
- Bounded voice count and page/cache memory budget; pin active voice pages; prefetch upcoming pages; safe cache eviction.
- Preview playback uses the same bounded streaming path or a separate low-priority preview voice.
- Import must not interrupt audio playback or alter the currently active preset.
- Re-link missing original paths from bank metadata; cache is rebuildable.

## UI acceptance

The single main button is labelled IMPORT SOUNDS. It opens a modal/resizable import panel:
- Add Files, Add Folder, Add ZIP; drag-and-drop.
- Preview selected file; format, duration, sample rate, bit depth, channels and file size.
- Bank name and category; progress, current file, total files, cancel.
- Duplicate detection and "reuse existing sample" behavior.
- Summary with imported/skipped/failed counts and actionable diagnostics.
- User banks can be renamed, rescanned, exported as metadata package, or removed. Removing a bank must never delete Factory assets. Provide an explicit separate confirmation before deleting copied originals.
- Imported bank data and cache persist across DAW/plugin restarts.
- The UI must not claim a sound is categorized correctly unless the user confirms or classification is deterministic and explainable.

## License and provenance

User-imported material is local/private by default and is not automatically added to distributable Factory banks. Factory redistribution requires a separate per-file license review. Store source URL, author/rights holder, license identifier and exact license text/reference where available, retrieval date, archive hash, per-file SHA-256 and approval status. "Free download" is not proof of redistribution rights.

## Acceptance tests

- WAV/AIFF/FLAC/OGG supported imports produce valid cache files and can be previewed.
- MP3 capability is explicitly detected; unsupported decoder gives a clear error.
- SFZ retains mapping and reports missing samples/unsupported opcodes.
- ZIP traversal and oversized archive tests fail safely.
- Duplicate source hashes reuse existing asset without duplicate storage.
- Cancel/failure leaves no partial visible bank.
- Bank index rebuilds from bank.json; deleted cache regenerates.
- Audio thread remains free of disk access and blocking locks.
- User bank survives plugin/DAW restart; Factory bank cannot be removed from UI.
- Importing a bank does not alter version 4.0 assets or currently playing audio.
