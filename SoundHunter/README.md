# PPGU Sound Hunter — Real Guitar Audio

This first implementation imports only catalogued sources classified as `REAL_RECORDED`. Synthetic and modeled guitar sources are excluded from this acquisition path.

## Current verified source candidates

- [Meta Open Sound](https://github.com/facebookincubator/Meta-open-sound): Steel Guitar and Nylon Guitar, CC0-1.0; OGG repository material and 24-bit/48 kHz WAV release archives.
- [FreePats Clean Electric Guitar](https://github.com/freepats/electric-guitar-FSBS-clean): direct guitar sampling, CC0-1.0.
- [FreePats Direct DI Electric Guitar](https://github.com/freepats/electric-guitar-FSBS-direct): raw direct guitar recordings, CC0-1.0.
- [FreePats Distortion 1](https://github.com/freepats/electric-guitar-FSBS-dist1) and [Distortion 2](https://github.com/freepats/electric-guitar-FSBS-dist2): recorded guitar, amp/effects processed, CC0-1.0.
- [FreePats Jazz Electric Guitar](https://github.com/freepats/electric-guitar-FSBS-jazz): recorded guitar, CC0-1.0.
- [FreePats Spanish Classical Guitar](https://github.com/freepats/spanish-classical-guitar): recorded acoustic guitar, CC0-1.0.
- [Karoryfer Black and Green Guitars](https://github.com/sfzinstruments/karoryfer.black-and-green-guitars), [Emilyguitar](https://github.com/sfzinstruments/karoryfer.emilyguitar), and [Shinyguitar](https://github.com/sfzinstruments/karoryfer.shinyguitar): recorded guitar instruments, CC0-1.0.

Source catalog: `real_guitar_sources.json`.

## Run locally

Python 3.10+; standard library only.

```bash
python SoundHunter/sound_hunter_real_guitars.py --discover-only
python SoundHunter/sound_hunter_real_guitars.py --max-gb 25
```

The importer writes only to a new unique directory under `Samples/SoundHunterImports/<run-id>/`. It does not replace or delete existing files. It checks the downloaded repository license text, handles Git LFS pointers with object size/SHA-256 validation, deduplicates by SHA-256, verifies imported bytes, and writes a per-run `SOUND_HUNTER_REGISTRY.json`. The total Samples tree is capped at 25 GB.

## GitHub Actions

Run **Actions → Sound Hunter — Real Guitar Samples → Run workflow**. The workflow uploads the additive import as artifact `PPGU-REAL-GUITAR-SAMPLES-25GB-MAX`. This does not commit sample binaries into Git history or change the VST3 build workflow.

## Safety and limitations

- A source is accepted only when its catalogue classification is `REAL_RECORDED` and the repository's downloaded license text matches the declared CC0-1.0 terms.
- A failed license, download, LFS, size, or hash check stops the run; uncertain files are not accepted.
- Source metadata and licenses are preserved. Only supported audio/sample/map file extensions are imported; unrelated repository files are not.
- This is an initial curated source set, not a claim of exhaustive Internet coverage or proof that every downloaded instrument has already been integrated into ANGELA Engine or validated as a playable PPGU preset.
- The current workflow produces a downloadable artifact; it does not automatically merge the library into a release package. Integration and runtime playback validation remain separate acceptance steps.
