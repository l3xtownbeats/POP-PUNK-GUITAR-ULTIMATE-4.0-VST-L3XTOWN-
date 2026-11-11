# Sound Bank expansion and optimization

## Current source policy
Factory assets are restricted to recordings whose exact license and provenance permit commercial embedding and redistribution. Unknown, non-commercial, attribution-incomplete, or custom terms remain user-import-only/review-required. Artist and product names in the preset catalog are tonal references, not permission to copy commercial recordings, stems, riffs, or proprietary sample libraries.

## Source priority
1. Meta Open Sound Nylon and Steel: CC0 1.0, 348 and 375 samples respectively; retain upstream SFZ, design spec, LICENSE, manifest and SHA256SUMS.
2. FreePats FSBS electric guitar clean and distortion variants: CC0 per upstream LICENSE; retain SFZ and license files.
3. Additional SFZ catalogue entries only after checking each source's actual license, creator and redistribution terms. A repository being free to download is not enough.
4. Freesound: individual sound license must be exact CC0 before factory candidacy. API search requires an API key; original download requires OAuth2. No key/token is generated automatically by this project.
5. Commission original pop-punk performance recordings with a written grant covering commercial embedding, modification and redistribution.

## Library layout
- 00_DI_Raw
- 01_Acoustic
- 02_Clean_Pickup
- 03_Distortion_PowerChords
- 04_PalmMute
- 05_HighGain_Direct
- 06_Distortion_Sustains
- _SFZ
- _Unclassified

The indexer scans the existing source-library folders without moving or rewriting files. It records SHA-256, file size, category, license evidence, duplicate content and SFZ references in `Samples/SoundBank/catalog.json`. Unknown licenses remain visible but are not factory eligible. CC0 is only a legal candidate flag: listening, tuning, noise, clipping, root-note, articulation and mapping QA are still required.

## Run on Windows
```powershell
python tools/build_sound_bank_index.py --root Samples
```

Outputs:
- `Samples/SoundBank/catalog.json`
- `Samples/SoundBank/BUILD_REPORT.txt`

## Runtime optimization requirements
- Keep the source WAV/SFZ files in their original libraries; do not duplicate samples into each preset.
- Presets reference stable asset IDs or SFZ maps, not copied audio files.
- Scan/hash/index on a background worker; never scan folders, hash files, decode audio or access disk in the audio callback.
- Load only the selected preset's required regions; stream large samples through bounded page caches.
- Deduplicate by SHA-256 in the index; do not delete originals automatically.
- Separate factory assets from user-imported banks. USER presets and favorites reference local library IDs and survive library rescans.
- Expose library states: Installed, Indexed, Review Required, Missing Assets, Playable. Only mark Playable after a real MIDI note-on/audio-output check.
- Melody and Pattern share the same sound-bank IDs and FX chain. Melody selects note/articulation mappings; Pattern selects chord/performance mappings.
- Pattern performance recordings must carry BPM, meter, key, chord/voicing, stroke direction, articulation, transient offset and tail metadata. A single-note multisample must never be labelled as a complete strum/pattern library.

## Important limitation
The indexer creates a deterministic inventory; it does not itself download sample archives, generate missing guitar recordings, prove that every preset is playable, or change the C++ runtime. Build and host validation are still required after runtime integration.
