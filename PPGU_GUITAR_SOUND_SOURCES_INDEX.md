# PPGU — Guitar sound acquisition index (NATURAL SAMPLES ONLY)

**Status:** acquisition sources identified; payload acquisition and per-file verification remain in progress.  
**Rule:** only natural recorded guitar/bass material may ship. No synthesized guitar audio, silence, duplicated files or filler may be used to inflate library size.

## Factory library target

- BASIC is included in the standard product package and has a **15 GB minimum target, not a hard maximum**.
- BASIC may and must grow beyond 15 GB when required to deliver the approved quality, complete mappings and all 500 factory presets.
- EXPANDED is additive; initial planning target +15 GB. It never replaces BASIC.
- The 500 main factory presets must play from BASIC alone.

## Priority source candidates

| Source | Guitar content | Public license evidence | Current PPGU status |
|---|---|---|---|
| Meta Open Sound — Meta Steel Guitar | 375 multisamples, steel-string acoustic, E2–C6, 3 velocity layers, SFZ + design spec | Collection marked CC0-1.0; collection-specific LICENSE and SHA256SUMS are provided | Approved source candidate; download, checksum and package audit pending |
| Meta Open Sound — Meta Nylon Guitar | 348 multisamples, nylon-string acoustic, E2–B5, 3 dynamic layers, SFZ + design spec | Collection marked CC0-1.0; collection-specific LICENSE and SHA256SUMS are provided | Approved source candidate; download, checksum and package audit pending |
| FreePats — FSBS Clean Electric Guitar | Natural electric guitar multisamples; SFZ WAV/FLAC variants | Source page marks CC0-1.0 | Candidate; choose one canonical format/variant and verify exact archive/license |
| FreePats — FSBS Distorted Electric Guitar | Natural recorded electric guitar processed through amp/effects; SFZ WAV/FLAC variants | Source page marks CC0-1.0 | Candidate; verify exact archive/license and avoid duplicate variants |
| Karoryfer — Emilyguitar / Shinyguitar / Black And Blue Basses | Recorded guitar/bass libraries; SFZ availability varies by collection | Publisher says free sample libraries are CC0, with an explicit exception for Marie Ork and a warning about older downloads | Candidate only; verify each collection's current included LICENSE and files before use |

## Mandatory per-asset intake gates

1. Download the original archive from the official publisher/repository release.
2. Preserve the archive, collection README/design spec, SFZ map, LICENSE and attribution files.
3. Verify archive checksum against publisher-provided SHA256SUMS where available; independently calculate SHA-256 for each shipped asset.
4. Confirm commercial embedding and redistribution rights for the exact collection/version. Do not infer a license from a search snippet or a general publisher statement.
5. Inspect SFZ mappings: key range, root key, velocity range/layers, round-robin, string zones and articulation labels.
6. Detect duplicate audio by hash and flag near-duplicates for human review.
7. Record exact file count, unique bytes, sample rate, bit depth, channels, duration and license evidence in the asset registry.
8. Quarantine ambiguous rights, incomplete mappings, corrupt files and non-guitar material.
9. Register only verified content under stable asset IDs. Never assign ambiguous files to notes or articulations automatically.

## Links to authoritative source records

- Meta Open Sound repository: https://github.com/facebookincubator/Meta-open-sound
- Meta Open Sound releases: https://github.com/facebookincubator/Meta-open-sound/releases
- Meta Open Sound catalog: https://github.com/facebookincubator/Meta-open-sound/blob/main/catalog.json
- FreePats clean electric guitar: https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html
- FreePats distorted electric guitar: https://freepats.zenvoid.org/ElectricGuitar/distorted-electric-guitar.html
- Karoryfer free sample libraries and licensing: https://shop.karoryfer.com/pages/free-samples
- SFZ Instruments guitar index (discovery only; publisher license remains authoritative): https://sfzinstruments.github.io/guitars/

**Important:** these sources are a starting set, not a claim that the 15 GB BASIC payload has been downloaded or that any source alone satisfies the target. No audio payload is counted until the original files are downloaded, rights-cleared, checksummed and catalogued.
