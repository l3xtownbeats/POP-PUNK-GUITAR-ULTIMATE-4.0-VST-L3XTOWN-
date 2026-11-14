# Sound Hunter — global guitar-source acquisition policy

## Creative scope

Search globally, without a genre restriction, for guitar material that can serve the PPGU factory presets and engine: clean, DI, edge-of-breakup, crunch, high-gain, punk/pop-punk, pop-rock, emo, alternative, indie, metal, hard rock, post-hardcore, shoegaze, ambient, cinematic, funk, jazz, surf, country, classical, nylon, steel, 12-string, baritone, extended-range and other useful guitar tones.

Acquire and evaluate more than playable multisamples. Relevant material includes:
- multisampled instruments, SFZ/SF2/SF3/GIG banks and sample maps;
- single notes, sustains, releases, mutes, palm mutes, harmonics, slides, bends, scrapes, pick noises and fret noises;
- loops, riffs, phrases, strumming patterns, one-shots, hits, transitions and texture layers;
- DI, clean, amp/cab processed, stereo/multi-mic and acoustic recordings;
- useful metadata, mapping files, velocity/articulation maps and source documentation.

A file is not automatically useful just because its extension is recognized. Every asset must be inspected, fingerprinted, categorized and tested for relevance, quality, completeness and technical compatibility. Loops/phrases are not to be misrepresented as chromatic playable multisamples.

## User-approved unrestricted discovery scope; shipment rights remain separate

The user approves broad discovery of commercial and non-commercial guitar libraries and sound resources, with this acquisition preference:

- **Paid / purchase-required libraries:** do not purchase them and do not make them a required dependency. They may be listed as optional metadata leads, but skip downloading where payment, a paid subscription, or a purchase is required.
- **Commercial libraries freely downloadable without purchase:** download them when the files are genuinely publicly accessible through the source's normal, authorized download route and the applicable terms permit that download. A library being commercial does not by itself mean it is paid or prohibited.
- **Non-commercial libraries:** they may be downloaded/evaluated only where the license and access terms permit. If a license expressly restricts use to non-commercial projects, mark it NOT FOR COMMERCIAL PPGU SHIPMENT.
- **No circumvention:** never bypass payment, subscription gates, DRM, access controls, rate limits or other source restrictions. Do not use leaked or unauthorized copies.

Keep discovery/acquisition separate from distribution rights:

- Record source URL, access/price model, exact license text/link, attribution, version, acquisition date and SHA256.
- Include audio in commercial PPGU builds only when the license clearly permits redistribution/embedding in a commercial VST and its sound library, or explicit written permission has been obtained. Otherwise mark it NOT FOR SHIPMENT / RIGHTS_REVIEW_REQUIRED and keep it out of installers, preset banks and public sample artifacts.
- A freely accessible download, “royalty-free” label, or permission to use samples in finished music does not automatically grant the right to redistribute raw or extractable samples inside a plugin.
- Unclear or conflicting terms remain catalogued as RIGHTS_REVIEW_REQUIRED; do not silently discard the source or assume permission.

## In-plugin Samples directory layout and organization

The plugin's final sample library must be organized and browsable by **format, instrument, genre/style, sound character and articulation**. Keep this taxonomy consistent across imported assets, the registry, preset browser and runtime sample lookup.

Recommended top-level layout:

    Samples/
      _Registry/ (sample_registry.json, source_manifest.json, license_manifest.json, duplicate_index_sha256.json)
      ByFormat/ (WAV/, AIFF/, FLAC/, OGG/, SFZ/, SF2/, SF3/, GIG/, Other_Supported/)
      ByInstrument/ (Electric_Guitar/, Acoustic_Guitar/, Nylon_Classical/, Steel_Guitar/, Bass_Guitar/, Guitar_Percussion_Noises/)
      ByGenre/ (Pop_Punk/, Punk/, Pop_Rock/, Emo/, Alternative_Rock/, Indie_Rock/, Metal/, Hard_Rock/, Post_Hardcore/, Shoegaze/, Ambient_Cinematic/, Funk/, Jazz/, Surf/, Country/, Classical/, Other/)
      BySound/ (DI_Clean/, Edge_Of_Breakup/, Crunch/, High_Gain/, Distorted/, Acoustic_Natural/, Amp_Cab_Processed/, Stereo_MultiMic/)
      ByArticulation/ (Sustain/, Palm_Mute/, Mute/, Harmonics/, Slide/, Bend/, Release/, Pick_Fret_Noise/, Scrape/, Strum/, Riff_Loop/, One_Shot/)
      Sources/<source-id>/ (original_files/, license/, provenance/)

Implementation rules:
- Store each original audio payload **once** in its canonical source/format location. Do not create redundant copies just to make one sample appear under several genres or sound categories.
- Populate the ByFormat, ByInstrument, ByGenre, BySound and ByArticulation views using registry entries and portable relative-path manifests (or platform-supported links where appropriate). These views must remain usable on Windows and must not rely on symlinks being available.
- Every asset registry entry must include a stable asset ID, relative file path, actual format/codec, sample rate, bit depth where applicable, channel count, duration, instrument, genre/style tags (multiple allowed), sound/tone tags, articulation, playable note/key range and velocity layer where known, source ID, SHA-256, license status, attribution and provenance.
- Preserve original archive/source filenames and source metadata in Sources/<source-id>/; normalized plugin-ready files must be traceable back to their original asset and hash.
- Use normalized, predictable folder and filename conventions; sanitize unsafe path characters and prevent path traversal. Do not silently overwrite files with matching names—use stable IDs or collision-safe names and hash-based deduplication.
- Unsupported formats may remain in the source archive/quarantine inventory, but must not be presented as playable until a tested decoder/conversion path exists. Keep original files and converted runtime files distinguishable.
- Presets should reference stable asset IDs or registry paths, not fragile absolute machine paths. The plugin's browser should filter the same registry by format, genre, tone and articulation.
- Genre tags are descriptive and may overlap. Do not force a sound into only one genre; e.g. a clean DI guitar can be tagged Pop_Punk, Emo, Alternative_Rock and Indie_Rock when musically appropriate.
- Keep evaluation-only and rights-review assets physically separated from distributable plugin content. Only assets marked as approved for commercial shipment may be copied into the release sample package.
- This layout is a required target for implementation; adding this policy does not by itself mean the folders, registry browser or runtime integration have already been built.

## Import destinations and quality-based sound replacement

The user approves organizing all acquired sounds by format, instrument, genre/style, tone and articulation as specified above, and approves replacing an existing sound with a demonstrably higher-quality compatible alternative.

Approved import routes:
- Use the plugin's **Import Guitar** function when it is available and supports the asset type.
- Alternatively, import through the shared bank folder **Samples/Import Bank Guitar**.
- Where the importer is not available or does not support a format, use the project's common **Samples** directory through a controlled import/staging process. Preserve the approved taxonomy and registry; do not scatter unmanaged files into the folder.
- These are approved target routes. Before implementation, inspect the repository and actual plugin code to confirm which paths/functions exist; do not claim an import route is operational until verified.

Replacement rules:
- A new asset may supersede an existing one only after a documented A/B or objective quality review confirms a meaningful improvement in recording quality, noise floor, tuning, dynamics, velocity/articulation coverage, note range, consistency, sample rate/bit depth where relevant, and suitability for its assigned preset.
- Compare like with like: preserve the intended instrument, articulation, tuning, note/velocity mapping and preset role. A higher sample rate or larger file alone is not proof of better sound.
- Verify license and commercial redistribution eligibility before promoting a replacement into the shipped library. A freely downloadable but non-commercial-only asset must never replace a distributable commercial asset in the release build.
- Never overwrite or delete the previous sound. Import the candidate into a unique staging/source location, validate it, then update the registry/preset reference to the new stable asset ID. Mark the former asset as SUPERSEDED/ROLLBACK_AVAILABLE and retain its source, hash, license evidence and mapping for rollback.
- Run preset regression checks after a replacement: playback across the mapped key and velocity ranges, articulation switching, loops/one-shots behavior where applicable, and no missing-file references. If validation fails, keep the previous asset active.
- Record replacement provenance: old and new asset IDs, reason, comparison/validation results, source URL, license status, SHA-256 and date.


## Explicitly confirmed sample-library invariants

The user explicitly confirms and approves these mandatory rules:
- Store each original audio file once; avoid unnecessary duplicate payload copies. Use registry-based category views and hash deduplication.
- A sound may carry multiple genre/style tags where musically appropriate.
- Presets must reference stable asset IDs or registry identifiers, never fragile absolute paths or filenames alone.
- Unsupported or unvalidated formats must not be represented as ready for playback.
- Assets without verified permission for commercial redistribution must remain separated from the distributable release library and must not enter commercial builds.

These invariants apply to all import routes, including Import Guitar, Samples/Import Bank Guitar and controlled direct imports into Samples.

## Technical and safety requirements

- Never remove license checks, attribution obligations, source provenance or integrity checks to increase library size.
- Keep existing user files intact. Import into unique run folders; never overwrite or delete existing assets.
- Verify archive paths against traversal, file size, SHA-256 and duplicate content. Preserve the original license and source metadata beside each imported source.
- Do not execute downloaded installers, scripts, macros or binaries as part of sample acquisition. Treat archives and metadata as untrusted input.
- Prefer original recordings and documented sample libraries. Exclude ripped commercial game/DAW libraries, unauthorized artist stems, leaked content and samples extracted from copyrighted songs.
- Respect host/source rate limits, terms of service, access controls and bandwidth constraints. There is no arbitrary GB quota, but actual storage, execution time and source availability remain practical limits.
- Keep an inventory that distinguishes: VERIFIED_SHIP_ALLOWED, COMMERCIAL_PERMISSION_REQUIRED, EVALUATION_ONLY, and QUARANTINED.
- Validate musical usefulness before assigning material to a factory preset. A preset must produce its promised sound; a source name or marketing description is not validation.
