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
python SoundHunter/sound_hunter_real_guitars.py --max-gb 25
```

The importer writes only to a new unique directory under `Samples/SoundHunterImports/<run-id>/`. It does not replace or delete existing files. It checks repository license text, handles Git LFS pointers with object size/SHA-256 validation, deduplicates by SHA-256, verifies imported bytes, and writes a per-run `SOUND_HUNTER_REGISTRY.json`.

**Important size rule:** the 25 GB limit applies only to newly imported bytes in the current Sound Hunter run. Existing/manual libraries in `Samples` do not consume this run's allowance. Manual import has no artificial 25 GB cap. This run limit is separate from the approved 25–30 GB target for the final factory Guitar Sound Library; that final target must be met with verified, licensed audio assets and must never be padded with duplicates or placeholders.

## GitHub Actions

Run **Actions → Sound Hunter — Real Guitar Samples → Run workflow**. The workflow runs focused regression tests before downloading and uploads the additive import as artifact `PPGU-REAL-GUITAR-SAMPLES-25GB-MAX`. It does not commit sample binaries into Git history or change the VST3 build workflow.

## Current limitations and validation status

- The source catalog is curated, not an exhaustive Internet crawler.
- Each source's current repository layout, release asset names, license file, manifest, SFZ mapping and checksums must be revalidated before treating a download run as current. Meta Open Sound has evolved its repository layout/catalog since the first catalog entry; do not assume an old release-asset filename still matches.
- A source is accepted only when its catalogue classification is `REAL_RECORDED` and the downloaded repository license text matches the declared license. Per-collection license and manifest evidence must be retained.
- A failed license, download, LFS, size, path-safety or hash check must stop or reject the affected content; uncertain files are not accepted.
- This workflow creates a downloadable artifact; it does not automatically merge the library into a release package. Runtime integration, preset mapping, playback validation and final package integration are separate acceptance steps.
- Do not claim that samples are downloaded, imported, playable, integrated or factory-ready until the corresponding artifact and evidence have been inspected.
