# PPGU Sound Hunter — Real Guitar Audio

This importer acquires only catalogued sources classified as `REAL_RECORDED`. Synthetic and modeled guitar sources are excluded from this acquisition path.

## Current curated source set

- [Meta Open Sound](https://github.com/facebookincubator/Meta-open-sound): Steel Guitar and Nylon Guitar, CC0-1.0; 24-bit/48 kHz WAV release archives and OGG repository previews. The current upstream repository uses `systems/instruments/meta-steel-guitar` and `systems/instruments/meta-nylon-guitar`; its catalog/manifests/SFZ maps are the authoritative current mapping references.
- [FreePats Clean Electric Guitar](https://github.com/freepats/electric-guitar-FSBS-clean): direct guitar sampling, CC0-1.0.
- [FreePats Direct DI Electric Guitar](https://github.com/freepats/electric-guitar-FSBS-direct): raw direct guitar recordings, CC0-1.0.
- [FreePats Distortion 1](https://github.com/freepats/electric-guitar-FSBS-dist1) and [Distortion 2](https://github.com/freepats/electric-guitar-FSBS-dist2): recorded guitar, amp/effects processed, CC0-1.0.
- [FreePats Jazz Electric Guitar](https://github.com/freepats/electric-guitar-FSBS-jazz): recorded guitar, CC0-1.0.
- [FreePats Spanish Classical Guitar](https://github.com/freepats/spanish-classical-guitar): recorded acoustic guitar, CC0-1.0.
- [Karoryfer Black and Green Guitars](https://github.com/sfzinstruments/karoryfer.black-and-green-guitars), [Emilyguitar](https://github.com/sfzinstruments/karoryfer.emilyguitar), and [Shinyguitar](https://github.com/sfzinstruments/karoryfer.shinyguitar): recorded guitar instruments, catalogued as CC0-1.0.

Source catalog: `real_guitar_sources.json`.

## Run locally

Python 3.10+; standard library only.

```bash
python SoundHunter/sound_hunter_real_guitars.py --discover-only
python SoundHunter/sound_hunter_real_guitars.py
```

The importer writes only to a new unique directory under `Samples/SoundHunterImports/<run-id>/`. It does not replace or delete existing files. It checks repository license text, handles Git LFS pointers with object size/SHA-256 validation, deduplicates by SHA-256, verifies imported bytes, and writes a per-run `SOUND_HUNTER_REGISTRY.json`.

**Approved library-size rule (updated 2026-11-14):** the complete managed `Samples` library has no artificial total-size target or ceiling, and Sound Hunter has no per-run cap. Keep adding unique, technically valid, quality-checked and properly licensed audio assets; never pad the library with duplicates or placeholders. GitHub Actions/artifact/transfer limits are transport constraints only: package large libraries into multiple checksummed parts, retain the per-file registry and SHA-256 records, and reconstruct/verify the complete package after download. Transport chunking must never be treated as a content-size cap. Existing Samples files remain permanent and untouched; SHA-256 duplicates are skipped, never removed. External libraries merely referenced in place, outside `Samples`, are not copied or modified.

## GitHub Actions

Run **Actions → Sound Hunter — Real Guitar Samples → Run workflow**. The workflow runs focused regression tests before downloading and uploads the additive import as artifact `PPGU-REAL-GUITAR-SAMPLES-UNLIMITED`. It does not commit sample binaries into Git history or change the VST3 build workflow.

## Current limitations and validation status

- The source catalog is curated, not an exhaustive Internet crawler.
- Each source's current repository layout, release asset names, license file, manifest, SFZ mapping and checksums must be revalidated before treating a download run as current. Meta Open Sound has evolved its repository layout/catalog since the first catalog entry; do not assume an old release-asset filename still matches.
- A source is accepted only when its catalogue classification is `REAL_RECORDED` and the downloaded repository license text matches the declared license. Per-collection license and manifest evidence must be retained.
- A failed license, download, LFS, path-safety or hash check must stop or reject the affected content; uncertain files are not accepted.
- This workflow creates a downloadable artifact; it does not automatically merge the library into a release package. Runtime integration, preset mapping, playback validation and final package integration are separate acceptance steps.
- Do not claim that samples are downloaded, imported, playable, integrated or factory-ready until the corresponding artifact and evidence have been inspected.


## Permanent Samples retention rule

Every successfully acquired audio asset placed in the managed `Samples` library is permanent project content and must not be deleted by downloads, updates, cleanup, optimization, deduplication, or conversion. Before every acquisition, inventory existing files and SHA-256 hashes and reuse what is already present. Skip identical hashes; download only genuinely missing unique assets. Never overwrite existing paths. New versions and converted formats must be additive in new unique paths while retaining prior files and all provenance/mapping/license metadata. Never reclaim space by deleting old samples. If a GitHub transport/artifact limit is encountered, split the package into checksummed parts; do not cap or discard library content. Full enforcement in every manual-import/conversion path remains subject to audit and regression tests.
