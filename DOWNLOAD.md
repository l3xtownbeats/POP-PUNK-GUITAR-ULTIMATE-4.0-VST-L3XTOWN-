<div align="center">

<img src="assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg" alt="POP PUNK GUITAR ULTIMATE — L3XTOWN" width="100%">

# POP PUNK GUITAR ULTIMATE
### WINDOWS x64 · VST3 INSTRUMENT · DOWNLOAD CENTER

**PLAY. CREATE. ROCK.**

</div>

---

## Current download status

### Previous successful package

[**⬇ DOWNLOAD VST3 + GUITAR SAMPLE LIBRARY + PRESET FILES (ZIP)**](https://api.github.com/repos/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/artifacts/13766348419/zip)

- Approximate archive size: **1.55 GiB**
- Platform: Windows x64
- Format: VST3 instrument
- Package: plugin + redistributable sample content + preset files
- Build result: [successful workflow run #216](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42821580125)
- Artifact expiration: **8 February 2027**

This is a verified earlier package from a previous development commit. It does not contain the newly requested folder layout.

### New complete package — being prepared

[**View current build and packaging run #826**](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43017263448)

The new package will be published only after Windows x64 Release build, CTest, pluginval strictness 5 and packaging assertions pass.

Planned ZIP layout:

```text
POP_PUNK_GUITAR_ULTIMATE/
├── VST3/
├── SOUNDBANK/
├── PRESETS/
├── SKINS/
├── Resources/
├── Assets/
└── INSTALL_AND_CONTENTS.txt
```

The new ZIP is **not yet available** while the current workflow is running.

## Presets

- [Factory preset catalogue — 300 definitions](presets/factory_bank_300.json)
- [Additional factory bank data](presets/factory_preset_bank_v1.json)
- [Melody example (.ppgm)](presets/examples/Example-Melody.ppgm)
- [Pattern example (.ppgp)](presets/examples/Example-Pattern.ppgp)
- [Preset integration status and acceptance criteria](docs/PRESET_BANK_4_0_READINESS_RU.md)

The 300 records are authored preset definitions (150 Melody + 150 Pattern). They are not all accepted as fully playable runtime presets yet.

## SoundBank and licensing

The previous package includes the sample collection in the archive. A separate standalone SoundBank download has not been published.

- [Sample-source and redistribution policy](docs/SAMPLE_LIBRARY_SOURCING_AND_LICENSE_POLICY.md)
- [Meta Open Sound clearance notes](licenses/META_OPEN_SOUND_CLEARANCE.md)
- [Sampler streaming architecture](docs/SAMPLE_STREAMING_PAGE_CACHE_DESIGN_RU.md)

Only audio with verified redistribution rights may be included in the plugin package.

## Skins, resources and artwork

- [Skin/theme directory](SKIN/)
- [Theme schema](SKIN/theme.schema.json)
- [Theme manifest](SKIN/theme_manifest.json)
- [Skin documentation](SKIN/README.md)
- [Product cover and interface banner](assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg)
- [Interface artwork](Cover%20Interface.jpg)

The repository contains theme manifests and skin specifications. Runtime skin switching is not yet accepted as complete.

## Installation

1. Download and extract the ZIP.
2. Copy the VST3 bundle into C:\Program Files\Common Files\VST3\.
3. Keep the extracted sample folder available to the plugin.
4. Rescan VST3 plugins in FL Studio 20 or another compatible Windows host.
5. Load the instrument and send MIDI notes.

Use the package's INSTALL_AND_CONTENTS.txt for exact folder and sample-library instructions.

## Technical target

| Item | Specification |
|---|---|
| Platform | Windows 10/11 x64 |
| Format | VST3 instrument |
| Intended DAW | FL Studio 20 |
| Framework | JUCE 7.0.12 |
| Language | C++20 |
| Audio | Stereo |
| MIDI | Note / velocity / channel |
| Sampler | SFZ-mapped WAV |
| Voice slots | Up to 64 |
| UI direction | Dark charcoal / magenta |

See the [main README](README.md) for full product description, engine status, limitations and project documentation.
