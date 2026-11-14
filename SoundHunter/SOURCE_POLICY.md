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

Approved import routes (keep these mechanisms distinct):
- **Import Guitar** is the plugin's separate user-facing import function. It is one import mechanism, but it is NOT the route meant by the direct-folder request below.
- **Samples/Import Guitar Sample/** is the dedicated subfolder inside the main **Samples/** directory for placing/importing individual sound files or sound assets directly, without using the **Import Guitar** function. This is the user's explicitly confirmed direct-import folder.
- Do not use or refer to **Samples/Import Bank Guitar** as the direct-import folder; that was an incorrect earlier name and is superseded by **Samples/Import Guitar Sample/**.
- Files brought in through **Samples/Import Guitar Sample/** must be scanned, validated, hash-deduplicated, assigned metadata/tags, checked for license status, and then registered into the approved Samples taxonomy. The folder is an intake/staging location, not an unmanaged permanent dumping ground.
- Preserve the approved taxonomy and registry; do not scatter unmanaged files into the folder.
- This is the required target path and behavior. Before claiming it is operational, inspect and implement the actual plugin/importer code and verify that direct-folder scanning works.

Replacement rules:
- A new asset may supersede an existing one only after a documented A/B or objective quality review confirms a meaningful improvement in recording quality, noise floor, tuning, dynamics, velocity/articulation coverage, note range, consistency, sample rate/bit depth where relevant, and suitability for its assigned preset.
- Compare like with like: preserve the intended instrument, articulation, tuning, note/velocity mapping and preset role. A higher sample rate or larger file alone is not proof of better sound.
- Verify license and commercial redistribution eligibility before promoting a replacement into the shipped library. A freely downloadable but non-commercial-only asset must never replace a distributable commercial asset in the release build.
- Never overwrite or delete the previous sound. Import the candidate into a unique staging/source location, validate it, then update the registry/preset reference to the new stable asset ID. Mark the former asset as SUPERSEDED/ROLLBACK_AVAILABLE and retain its source, hash, license evidence and mapping for rollback.
- Run preset regression checks after a replacement: playback across the mapped key and velocity ranges, articulation switching, loops/one-shots behavior where applicable, and no missing-file references. If validation fails, keep the previous asset active.
- Record replacement provenance: old and new asset IDs, reason, comparison/validation results, source URL, license status, SHA-256 and date.



## User confirmation — direct import folder

The user explicitly confirms the corrected direct-import folder path and distinction:
- `Samples/Import Guitar Sample/` is the dedicated folder inside `Samples/` for directly placing/importing individual sound files without using the separate `Import Guitar` plugin function.
- `Import Guitar` remains a distinct, separate import mechanism.
- `Samples/Import Bank Guitar` is not the approved name/path for this direct-import workflow and must not be used as a substitute.
- Directly placed files must pass the defined scan, validation, hash deduplication, metadata/tagging, licensing and registry workflow before becoming available to presets.
- This folder path is an approved implementation requirement; do not claim its automatic scanning/import is operational until the code exists and has been tested.

## Explicitly confirmed sample-library invariants

The user explicitly confirms and approves these mandatory rules:
- Store each original audio file once; avoid unnecessary duplicate payload copies. Use registry-based category views and hash deduplication.
- A sound may carry multiple genre/style tags where musically appropriate.
- Presets must reference stable asset IDs or registry identifiers, never fragile absolute paths or filenames alone.
- Unsupported or unvalidated formats must not be represented as ready for playback.
- Assets without verified permission for commercial redistribution must remain separated from the distributable release library and must not enter commercial builds.

These invariants apply to all import routes, including Import Guitar, Samples/Import Bank Guitar and controlled direct imports into Samples.


## Explicitly confirmed replacement quality gate

Before any existing sound is superseded, the candidate must be checked and the results recorded for every applicable item:
- recording cleanliness and overall audio quality;
- audible noise, clicks, clipping, artifacts or other defects;
- dynamics and velocity-layer coverage/consistency;
- playable note range and mapping accuracy;
- articulation coverage, including palm mute where relevant;
- musical fit for the target genre, style and factory preset;
- license terms and verified permission for commercial redistribution in the PPGU plugin/library.

A replacement is approved only when it is demonstrably better or fills a documented missing capability, remains compatible with the preset role, and passes license and playback regression checks. If any critical check fails or rights are unclear, do not promote it; retain the current sound and keep the candidate staged for review. Preserve the old asset and rollback path.


## Engine-owned automatic discovery, classification and preset assignment

The intended final behavior is that the **PPGU plugin/engine itself** manages the imported sample intake after files are placed in `Samples/Import Guitar Sample/`. The user should not have to manually sort every file or manually decide which preset uses each sound.

Required engine workflow:
1. Detect new files in the direct-import folder at a safe scan point (startup, explicit Rescan/Auto Discover action, or a background scan that does not interrupt audio processing).
2. Validate file safety and readability; identify actual format/codec and technical properties; compute SHA-256; deduplicate without deleting the user's original files.
3. Read embedded metadata and accompanying SFZ/SF2/SF3/GIG maps or manifests where supported. Analyze audio characteristics where technically feasible (instrument family, clean/DI/processed tone, tempo/rhythm for loops, note range, velocity layers, articulations and recording quality). Do not invent note ranges, velocity layers, genre or articulation when evidence is insufficient; mark unknown and request review or keep the asset unassigned.
4. Classify assets with multiple tags across format, instrument, genre/style, tone and articulation. Preserve source and license metadata and keep rights-restricted assets isolated from commercial-shipment content.
5. Decide which compatible assets are suitable for each factory preset based on the preset's actual sound role, required articulation, tuning, note/velocity coverage and validated quality. A file's name or genre label alone is not enough to assign it.
6. Create/update registry entries and stable asset IDs, then make approved assets available to the preset browser and engine. Presets reference registry IDs, not hard-coded import-folder filenames.
7. If a better compatible sample is found, stage and compare it against the active asset, run the replacement quality gate and preset regression checks, then switch the registry reference only after passing. Keep the former asset available for rollback.
8. Provide user-visible scan/import results, including accepted, duplicate, unsupported, unassigned, rights-review and failed assets, plus the preset assignments/replacements made. Allow manual reassignment or exclusion where automatic classification is uncertain.

Audio-thread safety and performance are mandatory: file scanning, decoding, hashing, analysis and registry writes must happen off the real-time audio thread. Cache analysis results and avoid repeated full scans. The engine must never load every file indiscriminately; it should load only the assets needed by the selected preset/voice and required streaming or preloading strategy.

This is an approved product requirement, not a claim that the current engine already implements it. Verify the existing AUTO DISCOVER/import pipeline, then implement and test each stage before describing it as operational.


## Automatic preset creation from imported samples

The PPGU engine must also be able to **create new usable presets from suitable imported sounds**, not merely classify and assign samples to existing factory presets.

Required behavior:
- Detect coherent playable instruments/sample sets and determine whether they contain enough note range, velocity layers and articulations to form a playable instrument. Do not create a full-range preset from an isolated one-shot or loop; those may instead become phrase, riff, texture or one-shot presets where appropriate.
- Generate candidate presets using compatible sample groups and mappings. Configure key/velocity zones, root notes, tuning, round-robin/random variation where supported, sustain/release behavior, palm mute and other articulations only when the source actually contains the required data.
- Build a sensible instrument layer and sound-design chain for the detected source (for example clean/DI, crunch, high-gain, acoustic, ambient or processed), using the plugin's supported amp, cabinet and effects modules. Do not invent unavailable processing modules or claim unsupported articulations.
- Give each generated preset a stable unique preset ID, descriptive name, genre/style and tone tags, source asset references, mapping summary, license/shipment status, and provenance. Keep generated presets separate from curated factory presets until they pass validation.
- Automatically audition/render test notes and relevant velocity/articulation ranges where the test harness supports it; check for silence, clipping, abrupt key-zone gaps, tuning errors, clicks, missing files and excessive CPU/memory use. Require listening/QA review for musical quality before promoting a candidate to the factory library.
- Preserve the source assets and previous preset versions. Generated presets must be reproducible from registry IDs and versioned settings, and must support rollback.
- Only assets cleared for commercial redistribution may contribute audio to presets shipped in the commercial plugin. Presets derived from evaluation-only or rights-review assets must remain isolated and marked NOT FOR SHIPMENT.
- Report generated, rejected, incomplete and review-needed presets with reasons. Allow the user to audition, edit, approve or exclude generated presets.

Preset generation must run outside the real-time audio thread. It is an approved engine requirement; do not claim it exists until implemented and tested.


## User-confirmed automatic preset generation
The user explicitly approves and confirms the requirement that PPGU automatically generate new presets from suitable imported sample assets, in addition to automatically discovering, classifying and assigning sounds to existing presets. Generated presets must follow the documented mapping, metadata, quality-validation, licensing, separation from curated factory presets, user-review and rollback requirements above.


## Direct-import license gate clarification

For direct user imports, license verification must **never block loading, scanning, classifying, auditioning, or using a file locally inside the user's own PPGU installation**. Do not reject or quarantine a directly imported sound solely because its license is missing, unknown, non-commercial, or not yet reviewed. Preserve the user's file and make it available for local/private use where technically supported.

The earlier requirement to perform license checks during intake is therefore clarified: license metadata may be recorded and surfaced as informational status, but it is not an import gate and must not prevent direct local use. The user may choose to mark a source as personal-use-only or provide license details later.

This does **not** grant redistribution rights or authorize bundling raw samples in a commercial PPGU installer, factory library, downloadable content pack, or other distributed build. Before any third-party audio is shipped to other users, the project must still verify that the applicable license or written permission allows that exact redistribution/embedding. Assets with unknown or incompatible redistribution rights may remain usable locally but must not be silently included in a commercial distribution. This distinction is about distribution rights, not restricting the user's direct import.

This clarification supersedes any conflicting wording elsewhere in this policy that could be read as requiring license clearance before direct import or local use.


## Direct download/import must not be blocked by license review

The user confirms that license review must **never be used as a prerequisite or blocker for direct downloading or intake/import** of an otherwise lawfully accessible sample into the user's local research/intake library. A file may be downloaded from a source that permits that download and placed into `Samples/Import Guitar Sample/` even when its redistribution license is unknown, non-commercial, or still awaiting review. The intake pipeline must not reject or delete such a file solely because commercial redistribution rights have not yet been established.

This does **not** grant rights that the source license does not grant. The engine must continue to preserve provenance and license status, and must keep assets whose commercial redistribution rights are absent or unclear out of any commercial distribution package. Such assets may remain available for local evaluation, testing, and user-authorized private use where the source terms permit it, clearly marked with their rights status. No DRM/access-control circumvention, paywall bypass, or unauthorized access is permitted.


## Unified local sample library without physical rights-based segregation

The user confirms that assets with different license statuses must not be forced into separate physical folders or separate local libraries merely because of their rights status. All lawfully obtained and locally usable assets may be indexed in the same unified PPGU sample registry and made available for local audition, evaluation, sound design and preset creation where the source terms permit those uses. Rights status must remain attached to each asset and generated preset as metadata; this metadata is not a reason to block ordinary local intake.

This does not authorize redistribution contrary to a source license. Commercial release packaging must still exclude assets and presets whose required redistribution/embedding rights are absent or unclear, unless the user obtains the necessary permission. The release builder must enforce that rights check at packaging time and report exclusions. Do not silently strip provenance or license metadata to bypass this requirement.

## Technical and safety requirements

- Never remove license checks, attribution obligations, source provenance or integrity checks to increase library size.
- Keep existing user files intact. Import into unique run folders; never overwrite or delete existing assets.
- Verify archive paths against traversal, file size, SHA-256 and duplicate content. Preserve the original license and source metadata beside each imported source.
- Do not execute downloaded installers, scripts, macros or binaries as part of sample acquisition. Treat archives and metadata as untrusted input.
- Prefer original recordings and documented sample libraries. Exclude ripped commercial game/DAW libraries, unauthorized artist stems, leaked content and samples extracted from copyrighted songs.
- Respect host/source rate limits, terms of service, access controls and bandwidth constraints. There is no arbitrary GB quota, but actual storage, execution time and source availability remain practical limits.
- Keep an inventory that distinguishes: VERIFIED_SHIP_ALLOWED, COMMERCIAL_PERMISSION_REQUIRED, EVALUATION_ONLY, and QUARANTINED.
- Validate musical usefulness before assigning material to a factory preset. A preset must produce its promised sound; a source name or marketing description is not validation.
