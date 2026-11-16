# PPGU Library Distribution Baseline — BASIC 15 GB / EXPANDED 30 GB

**Status:** APPROVED PRODUCT REQUIREMENT / IMPLEMENTATION IN PROGRESS  
**Units:** Decimal GB (1 GB = 1,000,000,000 bytes).

## 1. Standard product package — BASIC LIBRARY

- Target content size: **15 GB**.
- The complete BASIC LIBRARY is included in the standard product package.
- It is not an optional download, separate purchase, or dependency on EXPANDED.
- BASIC is a complete, independently playable factory library.
- All **500 primary factory presets** must resolve and play using BASIC alone.
- The installer must report the measured installed/download size and verify the package manifest and hashes.

## 2. EXPANDED LIBRARY

- EXPANDED ADD-ON adds **15 GB** to BASIC.
- Initial combined library size: **30 GB total = 15 GB BASIC + 15 GB EXPANDED ADD-ON**.
- EXPANDED is additive and optional. It never replaces BASIC.
- It contributes additional tone choices, performances, articulations, layers, realistic variations and additional presets.
- The library format and catalog must allow future additions beyond 30 GB. Thirty GB is the initial target, not a maximum.

## 3. Automatic engine integration

Every installed factory or user library is discovered, indexed and analyzed by background services. The engine builds metadata and maps, then selects suitable material for presets and playback according to note range, articulation, velocity, round-robin group, performance style and other available mapping data.

Automatic classification must be confidence-aware:
- High-confidence, fully mapped material can be registered automatically after technical and license gates pass.
- Ambiguous or incomplete mapping is quarantined for review rather than silently assigned to wrong notes or articulations.
- User-imported content remains local to the user unless its rights explicitly allow redistribution.
- Presets reference stable library/asset IDs, never fragile absolute paths.

Manual assignment of every individual WAV is not required. A review step is still required when metadata is ambiguous or rights are unclear.

## 4. Real-time and integrity requirements

- Directory scanning, license checks, hashing, decoding, cache generation and mapping analysis run off the audio callback.
- No disk I/O, blocking waits, unbounded allocations or filesystem operations in the real-time audio callback.
- The runtime uses bounded voice/memory/cache budgets and streaming/lazy loading; it must not load all 30+ GB into RAM.
- Preserve original files and source provenance. Keep per-asset license evidence, attribution obligations and SHA-256 hashes.
- No padding, duplicated WAVs, silence, synthetic filler or repeated content to reach a marketing size target.
- No third-party sample may enter the redistributable BASIC/EXPANDED package without verified rights for commercial product embedding and sample-library redistribution.

## 5. Release acceptance gates

1. The standard installer actually contains the complete 15 GB BASIC payload.
2. Every one of the 500 primary factory presets loads and plays using BASIC alone.
3. A clean install with no EXPANDED add-on has no missing factory dependencies.
4. Installing EXPANDED adds 15 GB of verified, unique, license-cleared content and yields 30 GB combined initial total.
5. Removing/uninstalling EXPANDED leaves BASIC and all 500 primary presets fully operational.
6. Additional content can be installed beyond 30 GB without changing existing asset IDs or breaking presets.
7. Report actual byte counts, unique sample counts, playable mappings, license status, checksum status, and test evidence. A target size is not evidence that the payload exists.

## 6. Supersession

This approved baseline supersedes the historical 20 GB factory-package cap previously recorded in `docs/BANK_LIBRARY_SYSTEM_RU.md`. The new factory baseline is BASIC 15 GB included in the standard package plus an optional EXPANDED add-on of 15 GB (30 GB combined initial total), expandable beyond 30 GB. User libraries remain separately managed and are bounded by available system resources rather than a product-imposed size cap.
