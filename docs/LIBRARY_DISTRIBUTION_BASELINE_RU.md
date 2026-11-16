# PPGU Library Distribution Baseline — BASIC 15 GB minimum / EXPANDED +15 GB initial add-on

**Status:** APPROVED PRODUCT REQUIREMENT / IMPLEMENTATION IN PROGRESS  
**Units:** Decimal GB (1 GB = 1,000,000,000 bytes).

## 1. Standard product package — BASIC LIBRARY

- **15 GB is the minimum target, not a hard maximum.**
- The complete BASIC LIBRARY is included in the standard product package. It is not an optional download, separate purchase, or dependency on EXPANDED.
- BASIC is a complete, independently playable factory library.
- All **500 primary factory presets** must resolve and play using BASIC alone.
- If more unique, license-cleared guitar audio is needed to achieve the approved sound quality, realistic playability, complete mappings, articulations, velocity layers or round-robin variation for the 500 presets, BASIC **must be allowed to grow beyond 15 GB**.
- Never reduce guitar sound quality, remove necessary articulations, or compromise preset completeness merely to stay within 15 GB.
- The actual BASIC size is determined by the real, verified content required to meet those quality and coverage requirements. The installer must report measured bytes and verify manifests and hashes.

## 2. EXPANDED LIBRARY

- Initial planning target: **+15 GB** as an additive optional add-on.
- Initial nominal combined target: **30 GB total = 15 GB BASIC + 15 GB EXPANDED add-on**.
- These are initial targets, not hard caps. If BASIC grows beyond 15 GB, EXPANDED remains additional content and must never replace or subtract from BASIC.
- EXPANDED contributes additional tone choices, performances, articulations, layers, realistic variations and additional presets.
- The library format and catalog must allow future additions beyond the initial combined target without a product-imposed maximum.

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
- The runtime uses bounded voice/memory/cache budgets and streaming/lazy loading; it must not load all library content into RAM.
- Preserve original files and source provenance. Keep per-asset license evidence, attribution obligations and SHA-256 hashes.
- No padding, duplicated WAVs, silence, synthetic filler or repeated content to reach a marketing size target.
- No third-party sample may enter the redistributable BASIC/EXPANDED package without verified rights for commercial product embedding and sample-library redistribution.

## 5. Release acceptance gates

1. The standard installer contains the complete, verified BASIC payload of at least 15 GB; a larger payload is permitted and required when needed for approved quality.
2. Every one of the 500 primary factory presets loads and plays using BASIC alone.
3. A clean install with no EXPANDED add-on has no missing factory dependencies.
4. EXPANDED adds its own verified, unique, license-cleared content; its initial planning target is +15 GB.
5. Removing/uninstalling EXPANDED leaves BASIC and all 500 primary presets fully operational.
6. Additional content can be installed beyond the initial combined target without changing existing asset IDs or breaking presets.
7. Report actual byte counts, unique sample counts, playable mappings, license status and checksum status. A target size is not evidence that the payload exists.
8. **No tests, test builds, CTest, Pluginval, preset/audio-engine or performance test runs before the real sound libraries and all 500 presets have been collected and integrated.** After that milestone, testing requires its own authorization.

## 6. Supersession

This approved baseline supersedes historical fixed library-size caps. BASIC is included, independently playable, and has a 15 GB minimum target with no hard maximum where quality requires more. EXPANDED is an optional additive library with an initial +15 GB target, also expandable. User libraries remain separately managed and bounded by available system resources rather than a product-imposed size cap.
