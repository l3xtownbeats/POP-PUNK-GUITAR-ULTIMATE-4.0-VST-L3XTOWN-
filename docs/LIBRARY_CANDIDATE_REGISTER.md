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

## Canonical Factory sample tree (mandatory)\n\n```text\nSamples/\n└── Factory/\n    ├── Electric/\n    │   ├── Clean/\n    │   ├── Crunch/\n    │   ├── Overdrive/\n    │   ├── Distortion/\n    │   ├── High_Gain/\n    │   ├── DI/\n    │   ├── Rhythm/\n    │   ├── Lead/\n    │   └── Palm_Mute/\n    ├── Acoustic/\n    │   ├── Steel/\n    │   └── Nylon/\n    ├── Articulations/\n    │   ├── Sustain/\n    │   ├── Staccato/\n    │   ├── Harmonics/\n    │   └── Slides/\n    ├── Mapping/\n    ├── Velocity_Layers/\n    ├── Round_Robin/\n    └── Manifests/\n```\n\n`Electric/Rhythm` and `Electric/Lead` also contain the corresponding musical phrases; no parallel `Phrases/` tree is permitted. Mapping, velocity, round-robin, and manifests remain separate metadata/organization areas as specified.\n\n## Electric guitar candidates

| Candidate | Source / official page | Formats / published details | License indication | Status / notes |
|---|---|---|---|---|
| FreePats FSBS Electric Guitar Clean #1 | https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html | SFZ+WAV 456 MiB; also FLAC/SF2; processed clean bridge-pickup samples | CC0-1.0 stated by source | FOUND; audio review required |
| FreePats FSBS Electric Guitar Clean #2 (Jazz) | https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html | SFZ+WAV 193 MiB; also FLAC/SF2 | CC0-1.0 stated by source | FOUND; secondary clean colour; audio review |
| FreePats FSBS Electric Guitar Direct | https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html | SFZ+WAV 153 MiB; raw DI/direct guitar | CC0-1.0 stated by source | FOUND; DI requires amp path; not a ready-mix source alone |
| FreePats FSBS Electric Guitar Distorted #1 | https://freepats.zenvoid.org/ElectricGuitar/distorted-electric-guitar.html | SFZ+WAV 600 MiB; processed distorted/overdriven guitar | CC0-1.0 stated by source | FOUND; high-priority audio review |
| FreePats FSBS Electric Guitar Distorted #2 | https://freepats.zenvoid.org/ElectricGuitar/distorted-electric-guitar.html | SFZ+WAV 245 MiB; second processed distortion variant | CC0-1.0 stated by source | FOUND; compare against #1 for genuine unique content |
| Karoryfer Black and Green Guitars | https://shop.karoryfer.com/pages/free-black-and-green-guitars | About 500 MB, >1,500 samples; two hollowbody electrics; staccato, feedback, hammer-ons, behind-bridge plucks | Karoryfer's official free-samples page states free sample libraries are CC0; verify the exact archive's included license on receipt | FOUND; likely useful for articulation variety; indie/twangy character, audio review |
| Karoryfer Shinyguitar | https://shop.karoryfer.com/pages/free-shinyguitar | About 352 MB; archtop electric guitar | Karoryfer's official free-samples page states free sample libraries are CC0; verify exact archive license | FOUND; audio review; not assumed Pop Punk-ready |
| Karoryfer Emilyguitar | https://shop.karoryfer.com/pages/free-emilyguitar | 323 WAV samples, 24-bit/44.1 kHz, about 98 MB; six SFZ mappings | Karoryfer's official free-samples page states free sample libraries are CC0; verify exact archive license | FOUND; general-purpose electric guitar; limited articulation design |
| Blue Jeans And Moonbeams | https://sfzinstruments.github.io/guitars/ | Electric and acoustic guitar; SFZ Instruments catalog lists MIT | MIT listing needs repository/asset-level license and redistribution review | FOUND / LICENSE REVIEW; do not approve from index alone |
| Standard Guitar (Unreal Instruments) | https://unreal-instruments.wixsite.com/unreal-instruments/standard-guitar | Multi-articulation electric guitar; catalog lists FLAC, 44.1 kHz, about 716 MB | Custom license | FOUND / LICENSE REVIEW; custom terms may not permit redistribution |
| Metal GTX (Unreal Instruments) | https://sfzinstruments.github.io/guitars/ | Multi-articulation electric guitar; catalog lists about 1.3 GB | Custom license | FOUND / LICENSE REVIEW; custom terms required |
| fakeguitar | https://github.com/AprilDolly/Free-Audio-Software-List/blob/main/free_sample_libraries.md | SFZ electric guitar; 3 articulations, described as unprocessed | Exact upstream source/license not yet verified | LEAD ONLY; locate original maintainer/source before consideration |

## Acoustic secondary candidates

| Candidate | Source | Formats / published details | License indication | Status / notes |
|---|---|---|---|---|
| Meta Open Sound Steel Guitar | https://github.com/facebookincubator/Meta-open-sound | WAV 24-bit/48 kHz, SFZ maps, note/velocity specification, SHA256SUMS | CC0-1.0 stated by Meta | FOUND; user reports download completed; local archive checksum and extracted inventory still need confirmation |
| Meta Open Sound Nylon Guitar | https://github.com/facebookincubator/Meta-open-sound | WAV 24-bit/48 kHz, SFZ maps, note/velocity specification, SHA256SUMS | CC0-1.0 stated by Meta | FOUND; user reports download completed; local archive checksum and extracted inventory still need confirmation |

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


## Research update — 2026-11-16

New leads found during electric-guitar-first research. These are discovery results only; no samples have been downloaded, auditioned, or approved in this update.

| Candidate | Source / official or primary page | Published details | License / redistribution assessment | Status / notes |
|---|---|---|---|---|
| Guitar DI Lite (Boyss-Sound-e-Scapes / Sebastian McQueen) | https://www.audiartist.com/guitar-di-lite-free-kontakt-sfz-decent-sampler-wav/ | 96 WAV; 3 velocity layers; 2 round robins; Kontakt, SFZ, Decent Sampler; dry DI electric guitar | The article describes a free pack but the exact source-pack license and explicit permission to redistribute raw WAVs inside a commercial VST must be checked at the pack's own download/license page | FOUND / LICENSE REVIEW; technically promising for velocity/RR and DI coverage, but dry-only and not a ready-mix source |
| Mood Guitars (BeastSamples) | https://beastsamples.com/mood-guitars/ | 15 electric guitar riffs; WAV and AIFF; key/BPM-labelled loops | “Royalty-free” usage for music productions does not establish permission to redistribute the raw loops inside PPGU | FOUND / LICENSE REVIEW; possible phrase-content lead only; exclude from bundled library unless explicit raw-loop redistribution rights are obtained |
| SampleScience Clean Electric Guitar | https://wavloft.app/plugins/samplescience-clean-electric-guitar/ | Two clean electric guitars sampled note-by-note; intended as source for amp sims/effects | Public-domain description appears in secondary listing; verify SampleScience's own license and exact downloadable sample terms before inclusion | FOUND / LICENSE REVIEW; potential multisample source, clean/DI-oriented rather than finished Pop Punk tone |
| FreePats FSBS Jazz/Clean GitHub mirror | https://github.com/freepats/electric-guitar-FSBS-jazz | SFZ instrument, bridge-pickup samples, LICENSE.txt and README; source describes CC0 | Repository contains a CC0 license file; verify checksum/content against official FreePats download before integrating | FOUND; corroborating source for FreePats clean/jazz family, not a separate sound bank if identical files |
| FreePats FSBS Distortion #1 GitHub mirror | https://github.com/freepats/electric-guitar-FSBS-dist1 | SFZ instrument, processed distortion/overdrive; LICENSE.txt and README | Repository describes CC0; verify exact archive/file hashes against official source | FOUND; corroborating source, not an additional bank unless audio/file comparison proves distinct |
| FreePats FSBS Distortion #2 GitHub mirror | https://github.com/freepats/electric-guitar-FSBS-dist2 | SFZ instrument, second processed distortion/overdrive variant; LICENSE.txt and README | Repository describes CC0; verify exact archive/file hashes against official source | FOUND; compare against Distortion #1 and official downloads; do not count as unique capacity until deduplication |

### Structural classification rules reinforced by this research
- Multisampled playable instruments belong under `Samples/Factory/Electric/<ToneFamily>/` with their original SFZ/mapping files preserved alongside or in a matching `Mapping/` record.
- DI sources belong under `Samples/Factory/Electric/DI/`; do not label them as ready-mix presets.
- Finished processed clean/crunch/overdrive/distortion sources are preferred for immediate factory-preset usability; DI is a supplemental signal path.
- Riffs and musical phrases are classified inside the approved tree: rhythmic riffs/parts under `Samples/Factory/Electric/Rhythm/`; solo/lead phrases under `Samples/Factory/Electric/Lead/`. Do not create a separate `Phrases/` directory.
- A royalty-free composition-use license is not sufficient for raw sample redistribution. Such candidates remain unapproved until the exact pack terms explicitly permit commercial redistribution/embedding in a VST.
- Mirror repositories and alternate formats are not separate content by default. Compare SHA-256 at file level and count only unique audio.
