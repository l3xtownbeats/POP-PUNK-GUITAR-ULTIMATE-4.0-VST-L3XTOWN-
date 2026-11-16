# PPGU — Library Candidate Register
Status: working research register; candidates are NOT approved for product inclusion until audio, exact license, redistribution rights, technical metadata, and duplicate checks are completed.

## Fixed project filters
- Product: POP PUNK GUITAR ULTIMATE (guitar only; no bass).
- Electric guitar is the priority (about 70–80% of research effort); acoustic steel/nylon is secondary.
- Search for playable multisamples and articulations first; loops/phrases/MIDI are supplementary and must have explicit redistribution rights.
- No duplicate samples or presets. Exact duplicates: SHA-256. Similarity fingerprints are review flags only, never automatic deletion.
- Only assets whose license explicitly permits embedding and redistribution of the source samples inside a commercial VST product can be approved.
- Do not treat "royalty-free for music production" as permission to redistribute raw samples.
- Do not use commercial reference-library samples (Ample, Shreddage, NI Session Guitarist) without explicit written redistribution rights.
- No testing/building until all approved libraries are integrated and all 500 BASIC factory presets are created; then wait for the user's separate test authorization.

## Candidate status definitions
- FOUND: source identified; no rights or quality conclusion.
- LICENSE REVIEW: license located, but exact scope/asset terms still need confirmation.
- AUDIO REVIEW: actual sample content needs listening/technical inspection.
- APPROVED: only after both license and audio review, plus uniqueness/provenance metadata.
- REJECTED: incompatible rights, poor quality, duplicate content, or unsuitable material.

## Electric guitar candidates

| Candidate | Source / official page | Formats / published details | License indication | Status / notes |
|---|---|---|---|---|
| FreePats FSBS Electric Guitar Clean #1 | https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html | SFZ+WAV 456 MiB; also FLAC/SF2; processed clean bridge-pickup samples | CC0-1.0 stated by source | FOUND; audio review required |
| FreePats FSBS Electric Guitar Clean #2 (Jazz) | https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html | SFZ+WAV 193 MiB; also FLAC/SF2 | CC0-1.0 stated by source | FOUND; secondary clean colour; audio review |
| FreePats FSBS Electric Guitar Direct | https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html | SFZ+WAV 153 MiB; raw DI/direct guitar | CC0-1.0 stated by source | FOUND; DI requires amp path; not a ready-mix source alone |
| FreePats FSBS Electric Guitar Distorted #1 | https://freepats.zenvoid.org/ElectricGuitar/distorted-electric-guitar.html | SFZ+WAV 600 MiB; processed distorted/overdriven guitar | CC0-1.0 stated by source | FOUND; high-priority audio review |
| FreePats FSBS Electric Guitar Distorted #2 | https://freepats.zenvoid.org/ElectricGuitar/distorted-electric-guitar.html | SFZ+WAV 245 MiB; second processed distortion variant | CC0-1.0 stated by source | FOUND; compare against #1 for genuine unique content |
| Karoryfer Black and Green Guitars | https://shop.karoryfer.com/pages/free-black-and-green-guitars | About 500 MB, >1,500 samples; two hollowbody electrics; staccato, feedback, hammer-ons, behind-bridge plucks | CC0-1.0 listed by SFZ Instruments / Karoryfer free-samples page | FOUND; likely useful for articulation variety; indie/twangy character, audio review |
| Karoryfer Shinyguitar | https://shop.karoryfer.com/pages/free-shinyguitar | About 352 MB; archtop electric guitar | CC0-1.0 listed by SFZ Instruments / Karoryfer free-samples page | FOUND; audio review; not assumed Pop Punk-ready |
| Karoryfer Emilyguitar | https://shop.karoryfer.com/pages/free-emilyguitar | 323 WAV samples, 24-bit/44.1 kHz, about 98 MB; six SFZ mappings | CC0-1.0 per Karoryfer free-samples page | FOUND; general-purpose electric guitar; limited articulation design |
| Blue Jeans And Moonbeams | https://sfzinstruments.github.io/guitars/ | Electric and acoustic guitar; SFZ Instruments catalog lists MIT | MIT listing needs repository/asset-level license and redistribution review | FOUND / LICENSE REVIEW; do not approve from index alone |
| Standard Guitar (Unreal Instruments) | https://unreal-instruments.wixsite.com/unreal-instruments/standard-guitar | Multi-articulation electric guitar; catalog lists FLAC, 44.1 kHz, about 716 MB | Custom license | FOUND / LICENSE REVIEW; custom terms may not permit redistribution |
| Metal GTX (Unreal Instruments) | https://sfzinstruments.github.io/guitars/ | Multi-articulation electric guitar; catalog lists about 1.3 GB | Custom license | FOUND / LICENSE REVIEW; custom terms required |
| fakeguitar | https://github.com/AprilDolly/Free-Audio-Software-List/blob/main/free_sample_libraries.md | SFZ electric guitar; 3 articulations, described as unprocessed | Exact upstream source/license not yet verified | LEAD ONLY; locate original maintainer/source before consideration |

## Acoustic secondary candidates

| Candidate | Source | Formats / published details | License indication | Status / notes |
|---|---|---|---|---|
| Meta Open Sound Steel Guitar | https://github.com/facebookincubator/Meta-open-sound | WAV 24-bit/48 kHz, SFZ maps, note/velocity specification, SHA256SUMS | CC0-1.0 stated by Meta | FOUND; secondary acoustic; user's downloader was launched, completion not yet confirmed |
| Meta Open Sound Nylon Guitar | https://github.com/facebookincubator/Meta-open-sound | WAV 24-bit/48 kHz, SFZ maps, note/velocity specification, SHA256SUMS | CC0-1.0 stated by Meta | FOUND; secondary acoustic; user's downloader was launched, completion not yet confirmed |

## Leads not approved
- Pianobook guitar instruments: use only as a discovery pool. Pianobook permits use in compositions but explicitly forbids selling or redistributing sample libraries that the user does not own; therefore individual packs are NOT eligible for bundling unless the uploader's separate license grants raw-sample redistribution.
  - Policy: https://www.pianobook.co.uk/faq/are-pianobook-sample-packs-royalty-free-or-free-for-commercial-use/
- Commercial Karoryfer products (Glockenskull, Secret Agent, Snowkiss, Surfkiss) are not candidates for redistribution without a separate license.
- Any Kontakt/NKI/NKX/NCW library: inspect its own EULA and container format; technical extraction does not grant redistribution rights.

## Required metadata before approval
For every individual library and archive, add:
1. Exact official source/download URL, author, product/version and retrieval date.
2. License file/text, SPDX identifier if applicable, and a recorded decision on commercial embedding + raw-sample redistribution.
3. Archive bytes, extracted bytes, file count, WAV count, sample rate, bit depth, channel count, duration.
4. SFZ/NKI/other mapping files and note/key/velocity ranges.
5. Articulations, round-robin groups, velocity layers, string/position/pick metadata where available.
6. SHA-256 for every file; provenance path and archive checksum.
7. Audio review notes: noise, clipping, tuning, phase, sustain/decay, pick attack, amp/cab tone, Pop Punk suitability.
8. Duplicate check result against existing BASIC/EXPANDED and user-supplied libraries.
9. Final status: FOUND / LICENSE REVIEW / AUDIO REVIEW / APPROVED / REJECTED.

## Important
This register records research leads, not completed downloads. No candidate is counted toward the BASIC 15 GB or EXPANDED +15 GB target until it is approved and integrated. Never inflate library size with duplicate or low-quality content.
