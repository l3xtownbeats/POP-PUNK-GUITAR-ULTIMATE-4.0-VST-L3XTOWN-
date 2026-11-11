# PPGU Sample Library — build report

Date: 2026-11-11
Branch: 5.0-import-guitar-library-user-presets

## Scope
Tier 1 CC0 sources selected for redistribution in the automated Windows build: Meta Open Sound nylon and steel guitar; FreePats FSBS clean, distortion 1, distortion 2 and direct DI; Karoryfer Black and Green Guitars, Emilyguitar and Shinyguitar.

## Categories
- 01_Acoustic_Raw — Meta nylon guitar.
- 02_DI_Clean — FreePats clean/direct DI; Karoryfer Emilyguitar and Shinyguitar (source folders retained).
- 03_Distortion_Crunch — FreePats distortion 1/2; Karoryfer Black and Green (source folders retained).
- 04_Palm_Mute — only mappings/zones explicitly present in source SFZ assets; no invented samples.
- 05_Octaves_PowerChords — only source chord/unison mappings if present; no assumption that every single-note sample is an octave or power chord.
- 06_Leads_Articulations — mapped open/staccato/feedback/hammer-on/pickup articulations when supplied by source libraries.
- 07_Release_Noise — no standalone dedicated release-noise source has been verified for this package.
- 08_Loops — no third-party loops included because royalty-free terms reviewed do not permit isolated library redistribution.
- 09_MIDI — no MIDI files included in this build; Selekt Audio individual-file license certificates remain unverified.

## Validation gates
The CI pipeline downloads official release assets / repositories, verifies upstream license evidence, requires SFZ and audio assets for Karoryfer sources, hashes WAV files in the existing bank indexer, validates the preset catalog, builds the VST3, runs CTest and pluginval, then packages the results.

## Important status distinction
This report is a packaging specification and source audit. It does not assert that the current run has completed. Check the linked GitHub Actions run for the actual download/build/test outcome. It also does not assert every source articulation has been wired to a playable factory preset.
