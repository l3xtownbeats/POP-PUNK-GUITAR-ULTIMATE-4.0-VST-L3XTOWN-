# PPGU SoundBank — Guitar source intake (2026-11-13)

This is an intake/research record, NOT a record of downloaded or imported audio.
No third-party audio files are included by this commit. Each source must be acquired lawfully and its exact asset-level terms checked before use.

## Candidate inventory

| Source | Candidate content | Source status | PPGU embedding status | Evidence / next action |
|---|---|---|---|---|
| Godlike Loops — Godlike Guitars | 10 electric/acoustic loops, WAV 24-bit, MIDI, stems | Free product page | RESTRICTED for inclusion as reusable source assets under current general terms | https://godlikeloops.com/product/godlike-guitars/ ; https://godlikeloops.com/licensing-terms/ — terms permit sounds in finished End Products but prohibit source redistribution and placing substantially unchanged sounds in another sample pack, sound library or competing product. PPGU is a sample-based virtual instrument; obtain separate written permission before any bundle use. |
| Godlike Loops — Suffering Guitars | 22 electric/acoustic loops, WAV 24-bit | Free product page | RESTRICTED for inclusion as reusable source assets under current general terms | https://godlikeloops.com/product/suffering-guitars-free-download/ ; https://godlikeloops.com/licensing-terms/ — same restriction. |
| SlammingSamples — Acoustic Guitar Loops FREE | 10 acoustic loops, WAV 96 kHz/24-bit, 130 BPM | Free download page | REVIEW / UNKNOWN | https://slammingsamples.com/acoustic-guitar-loops-free/ — product page says royalty-free for production use; no explicit permission located for redistribution inside a sample-based VST. Ask provider for written VST-instrument redistribution permission. |
| SampleSwap — Guitar Samples | Individual guitar sounds, notes, FX and textures | Community collection; account required for downloads | REVIEW / UNKNOWN at asset level | https://sampleswap.org/filebrowser-new.php?d=INSTRUMENTS%20%28SINGLE%20SAMPLES%29%2FGuitars%2F ; https://sampleswap.org/SAMPLESWAP/%20%20LEGAL%20LICENSE%20INFO%20READ%20ME.html — site says contributors are asked to release uploads to public domain and permits redistribution with notice, but explicitly warns it cannot guarantee all uploads are copyright-clear. Do not import any asset until its provenance and rights are individually verified. |
| Soundza — Rock & Indie Packs | Rock/indie loops and one-shots, mostly 24-bit WAV | Free catalog | REVIEW / likely incompatible with PPGU source-library distribution | https://soundza.com/free-sample-packs/rock-indie ; https://soundza.com/license — license permits sounds in finished works and says shipping in a game/app is fine, but prohibits standalone/reusable source assets and sample-library redistribution. PPGU's core purpose is reusable guitar playback, so get explicit written clearance before inclusion. |

## Acquisition and integration gate

1. Download only from the official provider, under the user's own lawful account where required. Do not bypass login, checkout, DRM, or access controls.
2. Save the exact license/EULA text that applied on the acquisition date, product receipt/download record, and any provider email granting VST redistribution rights.
3. For every archive, record source URL, acquisition date, archive SHA-256, archive size, extracted file count, audio format/sample rate/bit depth, and per-file SHA-256.
4. Check every asset for its own license/provenance; a catalog-wide claim is not enough where the provider warns of unverified uploads.
5. Only set VERIFIED-ALLOW after the license expressly allows inclusion and distribution of reusable audio assets inside a commercial sample-based VST3 instrument, including modifications/conversions as needed.
6. RESTRICTED and REVIEW assets must not be copied into distributable SoundBank packages or CI build artifacts.
7. Do not mark any candidate as downloaded, imported, playable, or packaged until the actual files are present and independently verified.

## Current result

- Audio archives downloaded by this intake commit: 0
- Audio files imported by this intake commit: 0
- Assets approved for distributable PPGU SoundBank from this batch: 0
- Written permission requests required: Godlike Loops, SlammingSamples, Soundza
- Asset-level provenance review required: SampleSwap

This conservative result prevents a royalty-free music-production license from being mistaken for permission to redistribute the underlying sounds as a reusable virtual-instrument library.
