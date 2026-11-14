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

## License tiers — mandatory

1. **Redistributable in the shipped plugin (preferred):** CC0/public-domain, or a license that clearly permits commercial use, modification and redistribution of the actual audio samples as part of a commercial instrument/plugin. Preserve the exact license, attribution text, source URL, version, date and proof.
2. **Commercially licensed for plugin/sample-library redistribution:** include only after obtaining and archiving explicit written rights that cover embedding/redistributing the source audio inside a commercial VST instrument and its downloadable sound library. A normal producer/sample-pack license is not assumed to grant this right.
3. **Evaluation-only / non-commercial / research-only:** may be catalogued and, where the terms permit, used in isolated internal evaluation. Mark as NOT FOR SHIPMENT. Never package these samples in the commercial VST, installer, preset bank or public artifact. Do not circumvent paywalls, DRM, access controls or license restrictions.
4. **Unknown, conflicting or prohibited terms:** quarantine metadata only; do not download or integrate the audio until clarified. Do not infer rights from words such as “free”, “royalty-free”, “free for commercial use”, or a marketplace subscription.

A purchase or subscription does not by itself establish redistribution rights. For example, a marketplace may allow samples in finished music while prohibiting redistribution as raw or extractable samples. Ask the rights holder for explicit permission when the license is ambiguous.

## Technical and safety requirements

- Never remove license checks, attribution obligations, source provenance or integrity checks to increase library size.
- Keep existing user files intact. Import into unique run folders; never overwrite or delete existing assets.
- Verify archive paths against traversal, file size, SHA-256 and duplicate content. Preserve the original license and source metadata beside each imported source.
- Do not execute downloaded installers, scripts, macros or binaries as part of sample acquisition. Treat archives and metadata as untrusted input.
- Prefer original recordings and documented sample libraries. Exclude ripped commercial game/DAW libraries, unauthorized artist stems, leaked content and samples extracted from copyrighted songs.
- Respect host/source rate limits, terms of service, access controls and bandwidth constraints. There is no arbitrary GB quota, but actual storage, execution time and source availability remain practical limits.
- Keep an inventory that distinguishes: VERIFIED_SHIP_ALLOWED, COMMERCIAL_PERMISSION_REQUIRED, EVALUATION_ONLY, and QUARANTINED.
- Validate musical usefulness before assigning material to a factory preset. A preset must produce its promised sound; a source name or marketing description is not validation.
