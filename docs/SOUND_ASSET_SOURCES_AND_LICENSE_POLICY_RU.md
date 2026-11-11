# Sound Asset Sources and License Policy — PPGU 4.0

Date: 2026-11-10
Status: research shortlist and integration policy. No third-party audio files are bundled by this document.

## Product policy

The user library must not have an artificial product-level cap on number of BANKs, presets, samples, or total library size. Actual use remains bounded by available disk space, filesystem limits, address space, RAM, CPU, and host stability. The engine must scale through lazy/background loading, indexing, cache, and bounded voice/memory budgets rather than loading every installed sample into RAM.

## Candidate public sources

### Meta Open Sound
- Repository: https://github.com/facebookincubator/Meta-open-sound
- Relevant instrument collections: Meta Nylon Guitar (348 samples) and Meta Steel Guitar (375 samples).
- Published collection information: WAV 24-bit / 48 kHz releases, SFZ maps, sample design specs, manifests, checksums.
- The two instrument collections are identified as CC0 1.0; confirm each downloaded release's own LICENSE/manifest before packaging.
- Oculus Audio Pack is CC BY 4.0 and is primarily sound effects, not a guitar instrument. If used, preserve exact attribution; do not mix its license status with CC0 instruments.

### FreePats
- Website: https://freepats.zenvoid.org/
- Candidate: FSBS Electric Guitar Clean, including SFZ WAV and FLAC variants.
- The cited page identifies the bank as CC0 1.0.
- Before bundling, archive the exact bank page, LICENSE/README, version/date, download URL and checksum. Verify that the particular downloadable package matches the CC0 statement.

### SFZ Instruments catalogue
- Catalogue: https://sfzinstruments.github.io/guitars/
- Discovery index only; entries have different licenses (CC0, MIT, custom, commercial, etc.).
- Do not treat catalogue inclusion or a free download as redistribution permission.
- Per-library allowlist: accept CC0 / public domain and permissive licenses after reviewing full terms; CC BY only when attribution and other obligations can be met; exclude NC, ND when incompatible with required modifications, no-redistribution, unclear/custom restrictive, and commercial assets unless explicit written permission covers bundling and redistribution.

### Stargate Sample Pack
- Repository: https://github.com/stargatedaw/stargate-sample-pack
- Project describes its sample pack as public domain / unrestricted redistribution.
- Treat as candidate pending review of repository LICENSE, individual included files, contributor provenance and actual audio contents. Do not infer rights beyond the actual license and provenance.

### Freesound and similar per-file archives
- Each file must be reviewed individually.
- CC0 may be eligible; CC BY may be eligible with exact attribution; CC BY-SA requires compatibility review; CC BY-NC is not eligible for the commercial/distributable plugin bundle.
- User-imported files can be used locally by that user, but the plugin must not repackage or redistribute them without appropriate rights.

## License acceptance workflow

For every candidate asset/library:
1. Record canonical source URL, creator/rights holder, title, version, retrieval date and file list.
2. Store the complete license text or a stable copy/reference, SPDX identifier where valid, and exact attribution text.
3. Confirm rights cover commercial use, modification/conversion, embedding in a VST3, redistribution to end users, and redistribution as part of a sample library. These are separate questions.
4. Check third-party dependencies, embedded metadata, contributor terms and any exceptions.
5. Calculate SHA-256 per file and archive checksum.
6. Mark one of: Approved Bundled, Approved User-Import Only, Attribution Required, Review Required, Rejected.
7. Never ship Review Required / Rejected assets in factory bundles.
8. Recheck license and source before each release; keep provenance manifest with the shipped content.

## Formats and conversion

- Preserve original WAV whenever practical.
- Read SFZ as the primary mapping format where supplied.
- Support WAV PCM and IEEE float, mono/stereo, differing sample rates and bit depths through decoding/resampling.
- FLAC may be supported as an optional storage format if the decoder is included with compatible license; do not force lossy conversion.
- Optional derived cache is reproducible and keyed by source SHA-256 + cache format version.
- Conversion does not remove source license obligations.

## Unlimited-library architecture (not unlimited resource consumption)

- No hard-coded cap on total registered BANK count or aggregate library bytes.
- Index metadata and sample headers first; do not preload all audio.
- Background scan, hash, decode/cache and waveform generation.
- Bounded audio voice count and per-instance RAM budget, configurable by user/host constraints.
- Stream large samples or page them into preallocated buffers; audio callback must not perform disk I/O, decoding, heap allocation or blocking locks.
- Evict unused cache pages safely; pin active voices and prefetch likely next samples.
- Detect missing files, changed checksums, invalid paths and duplicate content.
- Keep factory, user-imported and licensed downloadable libraries in separate roots.
- Provide storage usage, cache usage and missing-file diagnostics; user can add as many libraries as their storage permits.

## Initial sound-library strategy

1. Electric guitar core: prioritize legally distributable electric-guitar multisamples (FreePats and individually approved SFZ catalogue entries).
2. Acoustic layer: Meta Steel Guitar and Meta Nylon Guitar as CC0 acoustic foundations.
3. Expand with original recordings commissioned/recorded for PPGU, with written assignment/license covering commercial redistribution.
4. User BANK import: permit locally used libraries regardless of whether they can be redistributed by PPGU; show license state and never silently include them in factory installer.
5. Create original pop-punk performance recordings (strums, palm mutes, power chords, chugs, slides, harmonics, articulations) under explicit PPGU distribution rights. Single-note samples alone do not provide authentic performance patterns.

## Status and acceptance

This file is a research and engineering policy, not proof that any source has been downloaded, legally cleared, integrated, or tested. Every bundled library requires a separate completed asset manifest and legal review. No audio files are added by this commit.


## 2026-11-11 — Native Instruments Session Guitarist compatibility boundary

The product may use Session Guitarist – Electric Sunburst Deluxe and Electric Vintage/Vintage Deluxe as sound/performance references, but PPGU must not extract, decrypt, copy, repackage or redistribute their protected Kontakt samples, NKI/NKX/NKC containers, patterns or presets. Native Instruments' EULA allows the supplied content to be used in musical/audio productions but restricts using it to create or function as another sound library or sample-based instrument and restricts separate sample redistribution. Official references: https://www.native-instruments.com/de/pages/end-user-license-agreement and https://www.native-instruments.com/products/session-guitarist-electric-sunburst-deluxe .

Supported user workflow: run the NI instrument in a separate Kontakt instance/channel inside FL Studio and mix it alongside PPGU. PPGU's own user-bank importer is for original recordings and WAV/SFZ sources whose license explicitly permits local use in another sample instrument. Similar sound categories should be created from original or permissively licensed multisamples and PPGU's own DSP; do not promise direct NI library import or extraction.
