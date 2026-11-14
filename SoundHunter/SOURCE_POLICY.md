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

## Technical and safety requirements

- Never remove license checks, attribution obligations, source provenance or integrity checks to increase library size.
- Keep existing user files intact. Import into unique run folders; never overwrite or delete existing assets.
- Verify archive paths against traversal, file size, SHA-256 and duplicate content. Preserve the original license and source metadata beside each imported source.
- Do not execute downloaded installers, scripts, macros or binaries as part of sample acquisition. Treat archives and metadata as untrusted input.
- Prefer original recordings and documented sample libraries. Exclude ripped commercial game/DAW libraries, unauthorized artist stems, leaked content and samples extracted from copyrighted songs.
- Respect host/source rate limits, terms of service, access controls and bandwidth constraints. There is no arbitrary GB quota, but actual storage, execution time and source availability remain practical limits.
- Keep an inventory that distinguishes: VERIFIED_SHIP_ALLOWED, COMMERCIAL_PERMISSION_REQUIRED, EVALUATION_ONLY, and QUARANTINED.
- Validate musical usefulness before assigning material to a factory preset. A preset must produce its promised sound; a source name or marketing description is not validation.
