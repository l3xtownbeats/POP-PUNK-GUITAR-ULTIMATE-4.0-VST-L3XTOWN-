# POP PUNK GUITAR ULTIMATE — Downloads

## Windows x64 VST3 + Sound Bank + Factory Presets

The combined release package is built by GitHub Actions:

**[Open the Windows VST3 + Samples + Presets build and download its artifact](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42747513529)**

Artifact name:
`POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES-AND-PRESETS`

The archive is intended to contain:
- `POP PUNK GUITAR ULTIMATE.vst3` — VST3 instrument bundle
- `Samples/` — guitar sound bank, SFZ maps, license files and WAV SHA-256 manifest
- `presets/factory_bank_300.json` — 300 factory presets (150 Melody + 150 Pattern)
- `presets/examples/` — portable preset examples
- `INSTALL_AND_CONTENTS.txt` — installation and package notes

## Build and validation status

This link points to the build run for commit `f4647855f465d1b684b522cdcd8bb3b4cd310bed`. Wait for the run to finish successfully before downloading the combined archive. Automated Windows compilation and CTest do not replace a manual FL Studio 20 host test; DAW compatibility is not yet certified.

GitHub Actions artifacts are temporary and expire according to the repository's artifact-retention settings. This is a build-artifact download page, not a permanent GitHub Release.

## Installation

1. Extract the ZIP archive.
2. Copy `POP PUNK GUITAR ULTIMATE.vst3` to `C:\Program Files\Common Files\VST3\`.
3. Keep the `Samples` folder with the extracted package; if the plugin asks for a sample-library location, point it to that folder.
4. In FL Studio 20.7 or later, run Plugin Manager and rescan VST3 plugins.
5. Load the instrument on a channel and select the factory preset bank if the current plugin build exposes its preset browser.

