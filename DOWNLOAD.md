# POP PUNK GUITAR ULTIMATE — Downloads

## Windows x64 VST3 + Sound Bank + Preset Catalogue

**[OPEN VST3 + SAMPLES + PRESETS BUILDS](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42763253767)**

[All workflow runs for the active branch](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions?query=branch%3Amigration%2Ffull-copy-m3-transport-scheduler-v2)

The workflow artifact to download is named:

`POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES-AND-PRESETS`

It is created only after the Windows VST3 build, CTest and packaging steps succeed. The verified successful build is [run #5]( https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42763253767 ). In its Artifacts section, download **POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES-AND-PRESETS** (artifact ID: 13738701864, approximately 2.24 GB). This is a temporary GitHub Actions artifact, not a permanent release.

## Expected archive contents

- `.vst3` — Windows x64 VST3 instrument bundle
- `Samples/` — CC0 guitar sound-bank WAV files, SFZ maps, original licence files and WAV SHA-256 manifest
- `presets/factory_bank_300.json` — 300 preset definitions (150 Melody + 150 Pattern)
- `presets/examples/` — portable preset examples
- `INSTALL_AND_CONTENTS.txt` — package contents and installation notes

**Important status note:** the 300-entry JSON file is a validated preset catalogue, not a claim that all 300 definitions are already integrated as selectable factory presets in the plugin. The combined artifact is currently published in the successful run linked above. GitHub Actions artifacts are temporary and may require you to be signed in to GitHub. FL Studio 20 manual host validation remains a separate acceptance step.

## Installation

1. Download the named artifact ZIP from a successful run and extract it.
2. Copy the complete `.vst3` bundle to `C:\\Program Files\\Common Files\\VST3\\`.
3. Keep the `Samples/` directory with the extracted package; if prompted, select it as the sample-library folder.
4. In FL Studio 20, run Plugin Manager and rescan VST3 plugins.
5. Load the instrument and verify sample-bank loading. Preset-browser support must be confirmed against the actual build.
