<div align="center">

<img src="assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg" alt="POP PUNK GUITAR ULTIMATE — L3XTOWN AUDIO LAB" width="100%">

# DOWNLOAD CENTER
### WINDOWS x64 · VST3 INSTRUMENT

**PLAY. CREATE. ROCK.**

</div>

---

## Current release status

**No current COMPLETE ZIP is verified or published.** The repository contains source code, sample/preset resources and skin data, but the current main revision has not been proven by a complete Windows Release → all CTest targets → VST3 locate → pluginval strictness 5 → packaging pipeline.

The latest checked commit-status query returned no status checks. The available commit workflow lookup is limited to PR-triggered runs and cannot establish the full repository Actions history. Do not interpret either result as a successful build.

- [Open GitHub Actions](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions)
- [Open current main branch](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/tree/main)
- [Single engineering journal](Info.txt)

## Previous artifact — historical only

[Download previous VST3 + sample content artifact](https://api.github.com/repos/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/artifacts/13766348419/zip)

[Open historical successful workflow run](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42821580125)

This artifact belongs to an earlier source revision and package layout. It is not the current release candidate and must not be presented as a verified build of current main.

## Required COMPLETE package layout

The release archive name is:

**POP_PUNK_GUITAR_ULTIMATE_Windows_x64_VST3_COMPLETE.zip**

The package must provide the VST3 bundle and its runtime skin directory as siblings, so extracting the archive creates the expected runtime relationship without manual file moves:

```text
POP_PUNK_GUITAR_ULTIMATE/
├── VST3/
│   └── POP PUNK GUITAR ULTIMATE.vst3/
├── Skins/                 # runtime themes; count derived from registry/schema
├── SOUNDBANK/
├── PRESETS/
├── Resources/
├── Assets/
├── MANIFEST.json
└── INSTALL_AND_CONTENTS.txt
```

Source-only skin authoring data may be included additionally, but it cannot replace the runtime `Skins/` directory. Package assertions must derive counts from the authoritative manifests, not hardcoded values.

## Product content and verification

- Windows x64 VST3 instrument.
- Redistributable guitar SoundBank with source licenses, provenance and checksums.
- Factory preset data and supported user-bank examples.
- Runtime resources and skin data required by the actual editor.
- Installation instructions, manifest and package contents list.

The 300 factory catalogue records are definitions, not 300 verified playable presets. A release report must state the tested playable count as N/300.

## Installation — only after a verified package is published

1. Extract the archive.
2. Copy the `.vst3` bundle into `C:\Program Files\Common Files\VST3\`.
3. Keep the sibling runtime `Skins/` directory in the exact package-prescribed location relative to the installed bundle.
4. Rescan VST3 plugins in FL Studio 20.
5. Load the instrument and audition only verified presets or licensed SFZ libraries with MIDI.

Follow the `INSTALL_AND_CONTENTS.txt` shipped with the exact package. Do not apply instructions from another artifact revision.

## References

- [Main product README](README.md)
- [Single engineering journal](Info.txt)
- [Preset catalogue](presets/)
- [Sample sourcing and license policy](docs/SAMPLE_LIBRARY_SOURCING_AND_LICENSE_POLICY.md)
- [SoundBank audit](docs/SB0_SOUNDBANK_CODE_AUDIT_RU.md)
- [Skin/UI implementation audit](SKIN/SKIN_ENGINE_GAP_AUDIT_RU.md)
