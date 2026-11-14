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


## Consolidated user-confirmed decisions on intake, library organization and commercial release

This section consolidates the user's latest confirmations and supersedes any conflicting wording elsewhere in this policy on the same topics:

1. **No license-review gate on lawful download or direct intake.** Do not block an otherwise lawful download or local intake solely because a license is unknown, non-commercial, or awaiting review.
2. **One unified local library.** Do not physically segregate samples into separate local libraries or folders solely by license status. Keep license/provenance information attached as metadata in the common registry.
3. **Local discovery and creative use.** The engine may index, audition, analyze, classify and use imported assets for local evaluation and preset generation to the extent permitted by the source terms. Unclear rights must not cause silent deletion or rejection at intake.
4. **Commercial release preference and legal boundary.** The user requested removal of the rule excluding assets without confirmed commercial redistribution rights from the commercial distribution. Record this as the user's requested preference, but it cannot override third-party rights or grant redistribution permission. Therefore the release builder must not package third-party samples or derived preset content in a way that reproduces/redistributes those samples unless the applicable license or written permission allows it. This is a distribution-rights safeguard, not a download/intake restriction or a requirement for physically separate local storage.
5. **No bypass.** Nothing in these decisions authorizes bypassing paywalls, DRM, access controls, or source terms.

Implementation must preserve these distinctions in code, UI and documentation: intake is not release packaging; local library organization is not a license grant; and release eligibility is determined by the actual permissions applicable to the material. This consolidated policy records the user's requested changes and the boundary that cannot be waived by the project.


## User reconfirmation of approved Sound Hunter requirements

The user reconfirms and approves the following requirements already documented in this policy:
- Lawfully accessible downloads and direct local intake are not blocked solely by pending or unknown license review.
- A unified local sample library is used; assets are not physically segregated solely by license status.
- The engine automatically discovers, validates, analyzes, classifies, tags and registers imported samples.
- The engine uses suitable registered sounds in existing presets and can generate new candidate presets from coherent playable sample sets, subject to technical validation and source-term limits.
- License/provenance metadata is retained. This reconfirmation does not grant rights to redistribute third-party samples beyond permissions granted by their owners.

This entry confirms the user's approval of the above requirements; it does not claim that the features have already been implemented.


## Final user-confirmed direct-import folder name and automatic organization

This section supersedes earlier references in this policy to `Samples/Import Guitar Sample/` (singular) or `Samples/Import Bank Guitar/` as the direct-import folder.

The exact required direct-import folder is:

`Samples/Import Guitar Samples/`

It must be located directly inside the main `Samples/` directory. The user will place or copy incoming guitar sample files into this one intake folder and is not expected to manually sort them into genre, instrument, format, tone or articulation subfolders.

The PPGU system/plugin/engine is responsible for scanning this folder, validating and analyzing its contents, identifying formats and metadata, classifying and tagging assets, deduplicating them, registering them in the unified sample registry, and organizing their logical placement in the library taxonomy. The engine should present scan results and flag uncertain classifications rather than silently guessing. Once processed, assets should be managed through the registry and library organization; the intake folder is not the user's manual filing task.

Scanning, hashing, decoding and analysis must run outside the real-time audio thread. Preserve original files and provenance; do not delete or overwrite intake files during processing. License metadata remains attached to assets, but pending license review must not block lawful local intake.

This is a product requirement only. Do not claim the folder watcher, scanner, classifier or automatic organizer exists until the code is implemented and tested.

## Technical and safety requirements

- Never remove license checks, attribution obligations, source provenance or integrity checks to increase library size.
- Keep existing user files intact. Import into unique run folders; never overwrite or delete existing assets.
- Verify archive paths against traversal, file size, SHA-256 and duplicate content. Preserve the original license and source metadata beside each imported source.
- Do not execute downloaded installers, scripts, macros or binaries as part of sample acquisition. Treat archives and metadata as untrusted input.
- Prefer original recordings and documented sample libraries. Exclude ripped commercial game/DAW libraries, unauthorized artist stems, leaked content and samples extracted from copyrighted songs.
- Respect host/source rate limits, terms of service, access controls and bandwidth constraints. There is no arbitrary GB quota, but actual storage, execution time and source availability remain practical limits.
- Keep an inventory that distinguishes: VERIFIED_SHIP_ALLOWED, COMMERCIAL_PERMISSION_REQUIRED, EVALUATION_ONLY, and QUARANTINED.
- Validate musical usefulness before assigning material to a factory preset. A preset must produce its promised sound; a source name or marketing description is not validation.

## User-confirmed manual activation policy for scanning and global search

The user confirms that **global internet search must run only when explicitly started by the user in manual mode**. It must never start automatically, periodically, on plugin launch, when a preset is loaded, or merely because the local sample folder contains files.

The local folder workflow and global internet discovery are separate operations:

- **Local import scan:** the engine may scan `Samples/Import Guitar Samples/` only when the user explicitly invokes the local scan / AUTO DISCOVER action. Do not silently enable continuous background auto-scanning or start a scan automatically at application/plugin launch. The user must be able to see and control when this scan runs.
- **Global search:** a separate, explicitly user-triggered manual action. It must remain OFF by default and must never be implicitly started by local scanning, AUTO DISCOVER, preset generation, startup, timers, or background activity.
- The UI must clearly distinguish the actions, for example **SCAN LOCAL IMPORT FOLDER** and **START GLOBAL SEARCH**, and show their independent status/progress.
- Running a local scan must never initiate network requests. Running global search requires a separate deliberate user action and should expose the search scope and progress.
- The user can cancel either operation. No hidden scheduled scans, recurring searches, startup scans, or automatic network discovery.
- Keep file scanning, hashing, decoding and analysis off the real-time audio thread even when the user manually starts a local scan.

This manual-activation policy supersedes any earlier wording that could be interpreted as automatically scanning on startup or automatically launching global discovery. AUTO DISCOVER remains a user-invoked feature, not an always-on background process.

This is a confirmed behavior requirement, not a claim that the controls or scanning/search implementation are already complete or tested.

## User-directed URL source inspection and download mode

Add a third, separate user-controlled Sound Hunter workflow: **SCAN / DOWNLOAD FROM URL**.

The user may paste one or more direct links or source-page URLs pointing to guitar sounds, sample libraries, sound banks, archives, or other potentially relevant audio resources. This mode is distinct from both local-folder scanning and global internet search:

- **Local folder scan:** manually scans the user's `Samples/Import Guitar Samples/` folder; no network access.
- **Global search:** broad internet discovery, started only by the user with the dedicated manual action.
- **User-provided URL mode:** inspects only the URL(s) explicitly supplied by the user. It must not expand into broad web searches or crawl unrelated sites.

For a supplied URL, the engine/tooling should, where technically and legally supported:
1. Inspect the page or direct resource and identify whether it points to downloadable guitar audio, sample banks, archives, metadata or license information.
2. Show the detected source name, candidate files/formats, approximate sizes when available, source/license information and any access requirements.
3. Distinguish a direct file URL from a web page that merely describes or links to a library.
4. Let the user choose which candidate files to download; do not silently download every linked file.
5. Download only through the source's normal authorized route. Never bypass logins, subscriptions, payment, DRM, access controls, CAPTCHAs or rate limits; do not use leaked or unauthorized copies.
6. Allow a user-directed scan/inspection without downloading where possible. Clearly report when a page cannot be inspected or a file is not directly accessible.
7. Preserve source URL, original filename, acquisition date, license/attribution evidence and SHA-256. Do not overwrite or delete existing files; use safe staging and deduplication.
8. After an approved download, place the files into the same local intake pipeline as other direct imports, then let the user explicitly start the local scan/classification workflow. URL inspection or download must not silently trigger local scanning, global search or preset generation.

The UI should provide a clearly separate URL input/action (for example, **INSPECT URL** and then **DOWNLOAD SELECTED**) and show progress and cancellation. URL inspection/download, local folder scanning and global search are three independent manually initiated operations. All remain OFF until the user deliberately starts the corresponding action. This feature is a product requirement, not a claim that URL inspection, download, or scanning is already implemented or tested.

## User-approved unified Sound Hunter feature and single naming

The user wants all sound acquisition and discovery capabilities presented as one unified feature named **SOUND HUNTER**, rather than as several separately branded engines or top-level features.

Implement one Sound Hunter interface, one shared discovery/analysis pipeline, one unified asset registry and one consistent results view. Within Sound Hunter, the user manually chooses the source/mode:
- **LOCAL** — inspect the local `Samples/Import Guitar Samples/` intake folder.
- **GLOBAL** — search the internet for guitar sounds and libraries.
- **URL** — inspect user-provided URLs and optionally download user-selected accessible files.

These are source modes of the same Sound Hunter feature, not separate engines. All modes feed the same validation, metadata extraction, audio analysis, classification, deduplication, registry and preset-suitability pipeline wherever applicable. A URL inspection/download can stage files for the local pipeline; it must not silently trigger another mode.

Keep **AUTO DISCOVER** as the name of the user-invoked discovery/analyze action or command within Sound Hunter, not as a separate competing product/engine. The interface may present one main **SOUND HUNTER** panel with a source selector and a clearly visible manual **START / AUTO DISCOVER** action. The selected source determines what is inspected. For URL mode, provide URL entry and explicit inspect/download-selected steps.

All source modes remain manually initiated and independently cancellable. Default state is idle/off. No startup scan, scheduled task, background global search, implicit network access from LOCAL mode, or automatic mode switching. Display selected source, progress, results and cancellation in the same unified interface.

This naming and UX consolidation is approved product direction. It does not claim that the unified UI or engine pipeline is implemented or tested.

## User-approved compact unified Sound Hunter UI layout

The user approves the single-page UI optimization: do not create separate windows or duplicate panels for LOCAL, GLOBAL and URL. Present all source modes inside one dedicated **SOUND HUNTER** page/panel in the main plugin interface.

### Layout and interaction
- One persistent Sound Hunter workspace; no separate LOCAL/GLOBAL/URL pop-up windows.
- A compact source selector switches between **LOCAL**, **GLOBAL**, and **URL**.
- One shared central work area changes only the source-specific controls:
  - LOCAL: display the configured `Samples/Import Guitar Samples/` path and local scan options.
  - GLOBAL: display search query/scope controls.
  - URL: display URL entry, **INSPECT URL**, and explicit **DOWNLOAD SELECTED** controls.
- One shared results area displays discovered assets, technical metadata, tags/classification, source/provenance/license status, preview availability, duplicate status, and import/preset suitability.
- One shared progress/status area shows the selected mode, current operation, progress, result counts, and cancellation.
- Shared actions are context-sensitive and not duplicated across source-specific windows. Keep preset generation/review actions in this shared workflow where appropriate.
- Preserve clear separation between inspection, downloading, local scanning, adding to library, and preset generation; one action must not silently trigger another.
- LOCAL mode must not make network requests. GLOBAL and URL modes require explicit user initiation. All modes are idle/off by default and cancellable.
- Reuse existing plugin navigation, theme, layout and controls where possible. Avoid redundant navigation items, repeated result browsers, duplicate status panels, and multiple competing Sound Hunter entry points.
- Keep all filesystem scanning, network operations, hashing, decoding, analysis and registry writes off the real-time audio thread.
- UI must remain responsive, compact, accessible and consistent with the NEON STAGE DEFAULT skin; other skins may change graphics only, not layout geometry, controls, bindings or behavior.

This is the user's approved design direction. It is a requirement for implementation and code review, not a claim that the unified page is already implemented or tested.

