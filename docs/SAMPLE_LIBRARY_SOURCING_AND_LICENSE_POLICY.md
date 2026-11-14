# Sample library sourcing policy

Product: POP PUNK GUITAR L3XTOWN VST3

## User's sound-quality direction

Use the highest-quality guitar recordings available and prioritize pop-punk suitability over convenience. Reference Session Guitarist – Electric Sunburst Deluxe for playability and production quality, not as a source of ripped or extracted audio assets.

## Hard distribution rule

A sample library may be bundled with the plugin only when its actual license explicitly permits redistribution/embedding of the source audio in a third-party virtual instrument distributed to end users, or when the rights holder grants written permission covering that exact use. A license to use samples in music productions is not the same as a license to redistribute the samples inside another instrument.

Commercial purchases and end-user licenses do not automatically grant sublicensing or redistribution rights. Do not extract, rip, record internally, resample, or package audio from Native Instruments Session Guitarist / Electric Sunburst Deluxe, Kontakt instruments, or other commercial libraries without a separate written license from the rights holder.

## Approved sourcing routes

1. CC0/public-domain guitar multisamples whose individual collection license and sample manifest confirm redistribution.
2. CC BY material only with required attribution and compliance review.
3. Commissioned original pop-punk guitar recordings under a signed work-for-hire or assignment/license agreement explicitly granting plugin embedding, modification, redistribution, sublicensing to plugin customers, worldwide commercial distribution, and continued use in updates.
4. Commercial libraries only after a written custom agreement explicitly permits embedding and redistribution of the source samples in this VST3 and its sample content package.
5. Optional external-library integration only where the library's EULA permits that exact use. A plugin must not bypass another product's access controls or expose its samples.

## Candidate reference

Meta Open Sound:
- Meta Nylon Guitar and Meta Steel Guitar collections are stated as CC0, with WAV, SFZ maps, mapping specs, and sample manifests.
- Use as legal acoustic baseline and test of the SFZ/WAV engine; these are not a substitute for purpose-recorded high-gain pop-punk electric guitar.
- Verify the exact collection folder LICENSE, manifest license ID, release archive, and SHA256SUMS before inclusion.

Other sources:
- Evaluate FreePats and other candidate libraries individually. Do not assume a repository's general label or a product's royalty-free marketing permits redistribution inside a competing virtual instrument.
- Record exact source URL, author, license version, download date, permission evidence, attribution text, checksum, and included file list in a third-party notices manifest.

## Required sound library design

Melody:
- Chromatic, correctly mapped single-note electric guitar coverage.
- Multiple velocity layers, round robins, note releases/sustain, and recorded articulations where available.
- Separate clean/crunch/high-gain and acoustic sets where legal source material exists.
- Avoid extreme repitching; use adjacent multisampled notes and sensible key zones.

Pattern:
- Purpose-recorded performance samples with chord/voicing, tempo, meter, stroke direction, articulation, velocity, and timing metadata.
- Record real down/up strums, palm-muted eighths/sixteenths, power-chord riffs, arpeggios, dead-note/percussive strokes, stops, fills, and transitions.
- A single-note multisample bank alone cannot provide Session Guitarist-like pattern realism.

## Per-library clearance checklist

- [ ] Exact license text archived with version/date.
- [ ] Redistribution in a commercial third-party VST instrument explicitly permitted, or signed written permission attached.
- [ ] Modification, format conversion, mapping, and bundling rights confirmed.
- [ ] Sublicensing/installation by plugin customers permitted.
- [ ] Attribution and notice obligations met.
- [ ] Sample manifest and SHA-256 checksums generated.
- [ ] No extracted content from libraries whose terms prohibit redistribution or derivative instruments.
- [ ] Legal review recorded before shipping.

## Important distinction

A library may permit royalty-free use in finished songs while prohibiting redistribution of its raw samples, multisamples, loops, or instrument content in another VST. For example, Native Instruments' Electric Sunburst Deluxe is a product and workflow reference; its recordings are not cleared for extraction or bundling by default. Obtain a separate written license if its actual sample content is desired.


## PPGU MASTER SOUND LIBRARY EXPANSION — USER APPROVED 2026-11-14

The approved target is an open-ended, maximum-scale guitar-only sound library: One-Shots, Sample Loops, multisample/performance sets, SoundBank metadata and corresponding unlimited MELODY/PATTERN presets and browser categories. These are distinct content types and must not be conflated. Every asset requires source provenance, SHA-256, technical/audio validation, accurate mapping and license review. Factory redistribution requires explicit rights to bundle raw samples; royalty-free use in finished music is not sufficient. Local user import remains governed by the user's own license and must not be blanket-blocked by factory redistribution rules.

Automated validators should check physical files, hashes, duplicates, format/integrity, clipping/silence/DC/noise, loop seams/tempo/bars, note/velocity/RR/articulation mappings, metadata/reference integrity and redistribution rights. Quality thresholds must be versioned and calibrated; don't reject expressive content using blind universal thresholds. See `docs/PPGU_GUITAR_SOUND_LIBRARY_MASTER_PLAN_RU.md` for the complete approved scope and autonomous defect-fix workflow.
