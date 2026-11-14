# Sound Hunter import verification — 2026-11-14

## Result

- GitHub Actions run: `43580131518`
- Workflow: `Sound Hunter — Real Guitar Samples`
- Source branch: `soundhunter-unbounded-library`
- Source commit: `91c344ebd2d5c38cd409ad7d1c09335d01cab863`
- Run conclusion: **success**
- Import job: **success**
- Registry artifact: `PPGU-SOUND-HUNTER-REGISTRY` (artifact ID `14107892331`)
- Registry run ID: `20261114T092106Z-7a39e5`

The registry artifact was downloaded and inspected locally. Its ZIP contains
`SOUND_HUNTER_REGISTRY.json` (2,319,221 bytes). Parsed registry totals:

- **5,182** recorded assets across **11** source libraries.
- **5,182** assets carry `CC0-1.0`.
- **5,182** assets have SHA-256 values; **5,182 unique hashes**.
- **4,457** entries have `license-and-sha256-verified`.
- **725** entries have `release-license-and-sha256-verified`.
- Formats: 3,789 WAV; 563 FLAC; 725 OGG; 105 SFZ.

## Imported source inventory

| Source ID | Registry entries |
| --- | ---: |
| meta-open-sound-steel | 752 |
| meta-open-sound-nylon | 700 |
| freepats-electric-clean | 137 |
| freepats-electric-direct | 123 |
| freepats-electric-dist1 | 123 |
| freepats-electric-dist2 | 123 |
| freepats-electric-jazz | 137 |
| freepats-spanish-classical | 50 |
| karoryfer-black-green | 1,846 |
| karoryfer-emilyguitar | 329 |
| karoryfer-shinyguitar | 862 |
| **Total** | **5,182** |

## Per-source GitHub Actions artifacts

All artifacts below belong to run `43580131518`, are unexpired at the time of
this report, and were emitted after the import, verification, and upload steps
completed successfully.

| Artifact | Artifact ID | Size (bytes) | SHA-256 |
| --- | ---: | ---: | --- |
| PPGU-SOUND-HUNTER-REGISTRY | 14107892331 | 243,276 | b791f79073d9dba1d4c5e4f451aa3453a79059b29cd416bb9ddbac057d61f69e |
| PPGU-SAMPLES-meta-open-sound-steel | 14107753503 | 871,541,037 | d812d6ffcefb8a38b7106d40d89257b4a0594edfc99aadb0a3e3f7a523ed4b14 |
| PPGU-SAMPLES-meta-open-sound-nylon | 14108456192 | 567,131,351 | 04557937c0e6bf547a3e8cc8e075d288efbbd4fd3d742c0f5607ef7ceff4611d |
| PPGU-SAMPLES-freepats-electric-clean | 14108545912 | 132,521,832 | d8c77ddf8d5ebf6fced533b84de6acf550a8f8d4c74068b1365d424da8761193 |
| PPGU-SAMPLES-freepats-electric-direct | 14107239393 | 329,686,854 | 19cbb4af041e2d0e8457f61c3596b8c22d6abd92873927b71cd407eb00031c06 |
| PPGU-SAMPLES-freepats-electric-dist1 | 14107529231 | 315,354,741 | d8da64108116e64416eab6d9121465d9631967279f41d00b2ebf0e590b4afcf3 |
| PPGU-SAMPLES-freepats-electric-dist2 | 14108380518 | 136,952,020 | 29b65823b41000cea65fe4e024c377ad4d2ef5881ec79e4aa27c90b2916295e6 |
| PPGU-SAMPLES-freepats-electric-jazz | 14108530365 | 70,134,530 | 88257f8651490e86f4ae9d442b8f164b7f1bd418ad316a3e87d2a82c17edc87b |
| PPGU-SAMPLES-freepats-spanish-classical | 14108705172 | 5,384,709 | b11161f9c9243badfec2595a6d3ae89a4c53b829a73cdfd546cd18df93044836 |
| PPGU-SAMPLES-karoryfer-black-green | 14108670217 | 582,266,111 | b8ce1e5fe160c54f32e4dfa43ac0f78b54660f16cd0d149c477f55be270741de |
| PPGU-SAMPLES-karoryfer-emilyguitar | 14107862687 | 125,612,631 | d34ae88d625c03d21ea5d546e93691918fc4cf990d700c267f4644b48c79fdf2 |
| PPGU-SAMPLES-karoryfer-shinyguitar | 14108040961 | 460,256,978 | d58dfb13663b9d8c4290b2b93b2fc6edebc115dcc22af96bd84c54f6efef2623 |

## Scope and limitations

This report verifies the Sound Hunter acquisition artifacts and the registry
metadata. It does **not** claim that the samples have already been copied into
the VST3 runtime, mapped into playable plugin instruments, or used by factory
presets. Those are separate implementation and validation steps.

The separate Windows VST3 workflow run `43580011941` was cancelled before
sample restoration, CMake configuration, compilation, CTest, pluginval, and
package creation. No successful VST3 build is claimed by this report.

Next integration work must consume the verified registry and source maps,
preserve attribution/license files, validate playable SFZ mappings and sample
coverage, then build and test the Windows VST3 package.
