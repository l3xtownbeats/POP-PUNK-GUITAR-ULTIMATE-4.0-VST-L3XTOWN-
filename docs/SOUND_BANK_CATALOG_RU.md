# POP PUNK GUITAR ULTIMATE — SOUNDS BANK CATALOG
**Updated:** 2026-11-11 · **Target:** Windows x64 VST3 · **L3XTOWN**

## Status key
- **CI factory source** — named in the current Windows build/package workflow.
- **Candidate** — source and license are listed for evaluation; not claimed as downloaded, bundled or playable.
- **User bank** — local user-owned/appropriately licensed content; never silently included in the factory installer.

## 1. Factory source libraries named by the CI pipeline

| Library | Sound family | Count / size known from source review | License status | Package status |
|---|---|---:|---|---|
| Meta Open Sound Steel Guitar | Acoustic steel-string | 375 WAV | CC0 1.0 | CI source |
| Meta Open Sound Nylon Guitar | Acoustic nylon/classical | 348 WAV | CC0 1.0 | CI source |
| FreePats FSBS Clean #1 | Electric clean | Exact count must come from artifact manifest | CC0 1.0 | CI source |
| FreePats FSBS Distorted #1 | Electric distortion | Exact count must come from artifact manifest | CC0 1.0 | CI source |
| FreePats FSBS Distorted #2 | Electric distortion variant | Exact count must come from artifact manifest | CC0 1.0 | CI source |

Meta subtotal: **723 WAV**. Do not invent a total for FreePats; read the exact file count, size and SHA-256 entries from the built package's manifest.

Official sources:
- Meta Open Sound: https://github.com/facebookincubator/Meta-open-sound
- FreePats electric guitar: https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html
- FreePats distorted guitar: https://freepats.zenvoid.org/ElectricGuitar/distorted-electric-guitar.html

## 2. Expanded candidate library catalog

| Library | Content / source-reported size | Intended PPGU use | Status |
|---|---|---|---|
| Karoryfer Emilyguitar | 323 WAV, 24-bit/44.1 kHz, six SFZ maps | Electric clean/character and chord voicings | Candidate; not in current CI package |
| Karoryfer Shinyguitar | 846 WAV, 24-bit/44.1 kHz; DI and mic sources | Bright/warm clean, blended DI/mic tone | Candidate; not in current CI package |
| Karoryfer Black And Green Guitars | 1,500+ samples, multiple electric guitars and articulations | Alternative electric attack, staccato and special articulations | Candidate; not in current CI package |
| FreePats FSBS Direct DI | SFZ FLAC/WAV variants | Dry DI source for PPGU FX path | Candidate; not in current CI package |
| FreePats FSBS Clean #2 / Jazz | SFZ FLAC/WAV variants | Soft clean/jazz tone | Candidate; not in current CI package |
| FreePats Spanish Classical Guitar | SFZ FLAC/WAV variants | Additional nylon/acoustic layer | Candidate; not in current CI package |

Official candidate pages:
- Karoryfer free samples and license: https://shop.karoryfer.com/pages/free-samples
- Emilyguitar: https://shop.karoryfer.com/pages/free-emilyguitar
- Shinyguitar: https://shop.karoryfer.com/pages/free-shinyguitar
- Black And Green Guitars: https://shop.karoryfer.com/pages/free-black-and-green-guitars
- FreePats acoustic guitar: https://freepats.zenvoid.org/Guitar/acoustic-guitar.html

Before any candidate is packaged, archive its own license text, source URL/version, original SFZ maps and SHA-256 manifest. Verify license scope and actual sample archive; publisher-level statements are not a substitute for checking the downloaded asset.

## 3. Native Instruments Session Guitarist compatibility

Reference products:
- Session Guitarist – Electric Sunburst Deluxe: https://www.native-instruments.com/products/session-guitarist-electric-sunburst-deluxe
- Electric Sunburst: https://www.native-instruments.com/products/session-guitarist-electric-sunburst

They are Kontakt instruments, not general-purpose WAV/SFZ sample packs. Their product pages describe Kontakt Player/Kontakt as the required playback environment. NI EULA permits use of samples, instruments and presets in musical/audio productions but restricts using them to create or function as another sound library/sample-based instrument, and restricts separate redistribution/repackaging of samples and sound sets.

**Therefore PPGU will not:**
- extract or decrypt samples from NKI/NKX/NKC or other protected Kontakt containers;
- bypass Kontakt/Native Access authorization;
- copy NI sample pools, patterns, presets or performances into PPGU;
- redistribute rendered NI notes as a PPGU factory sample library.

**Supported workflow:**
1. Keep Electric Sunburst/Vintage Deluxe loaded as a separate Kontakt instrument in FL Studio and mix it on its own mixer channel alongside PPGU.
2. Use PPGU's user-bank import for original recordings and third-party WAV/SFZ banks whose license explicitly permits local use in another sample instrument.
3. Build similar functional sound categories using original or permissively licensed multisamples: clean, bright/warm, open, muted, harmonics, tremolo, crunch, driven lead, plus velocity and round-robin variations.
4. Use NI as a tonal/performance reference only; do not advertise PPGU as a replacement or claim it contains NI sounds.

Official NI references:
- NI EULA: https://www.native-instruments.com/de/pages/end-user-license-agreement
- NI support — how to use Kontakt libraries: https://support.native-instruments.com/support/solutions/articles/69000879762
- NI support — licensed vs unlicensed Kontakt libraries: https://support.native-instruments.com/support/solutions/articles/69000879739-third-party-kontakt-libraries-licensed-vs-unlicensed

## 4. Sounds Bank taxonomy

**Instrument family:** Electric Clean, Electric Crunch, Electric Distortion, Electric DI, Hollowbody/Archtop, Acoustic Steel, Acoustic Nylon, Hybrid DI+Mic, User Imported.

**Sound character:** Bright Clean, Warm Clean, Chorus Clean, Tremolo Clean, Open Electric, Muted, Palm Mute, Tight Chugs, Harmonics/Flageolet, Crunch, Overdrive, Distortion/High Gain, Ambient/Wide, Acoustic Steel, Acoustic Nylon, Special.

**Articulation / playing style:** Picked Open, Finger Open, Palm Mute, Dead Note, Harmonics, Tremolo, Alternate Picking, Arpeggio, Power Chords, Open Chords, Octave Hook, Fingerstyle, Strummed Acoustic. Slides, hammer-ons, pull-offs, bends and vibrato are enabled only where the actual source contains suitable recordings or a validated synthesis method.

## 5. File formats

| Format | Role | Status |
|---|---|---|
| WAV | Primary production samples | Current engine/build target |
| SFZ | Sample maps, note/velocity regions, groups and articulation metadata | Current map format |
| AIFF | Optional user import | Requires decoder/streaming acceptance before advertised support |
| FLAC | Lossless compressed library source | Candidate source format; production decode/stream path requires acceptance |
| OGG/MP3 | Optional preview assets | Not production multisample format |

Do not claim a format as supported merely because it appears in a design proposal. Production acceptance requires decoder, mapping, playback, streaming and regression tests.

## 6. Bank structure and loading principles

Factory and user libraries remain separate. Preserve source folder layout and original attribution. Presets refer to stable bank/profile IDs rather than absolute machine-specific paths. Do not duplicate sample files per preset.

Recommended logical structure (final paths must match actual installer/CMake configuration):
- `BANK/Factory/<Library-ID>/`
- `BANK/User/<Library-ID>/`
- `BANK/Indexes/`
- `BANK/Previews/`
- adjacent license, SFZ map and SHA-256 manifest files.

Performance rules:
- background metadata indexing;
- load only the selected bank/profile, not the entire library;
- selective preload of attacks and short transient regions;
- stream long sustains/tails through bounded cache;
- no disk I/O, blocking locks or unbounded allocation in audio callback;
- retain current cache/root/snapshot safety limits until stress tests support a change.

The product target of 20 GB is a factory package-on-disk ceiling, not a plugin RAM budget. User banks are not counted against the factory installer size.

## 7. Acceptance gate for each new bank

1. Source URL, author, version/date and license text archived.
2. SHA-256 manifest generated and verified.
3. WAV/FLAC count, size, sample rate, bit depth and channel layout recorded.
4. SFZ opcodes checked against the actual parser; unsupported opcodes must be reported, not silently ignored.
5. Root key, key range, velocity layers, round robin, articulations, loops and release zones validated.
6. Every selected preset profile resolves to the correct bank and produces a playable first note.
7. Pattern performance sources are genuine chord/strum/phrase recordings or explicitly authored performance events; single notes are not falsely marketed as realistic strums.

**Current summary:** five source-bank families are named in the CI package pipeline, including 723 Meta WAV samples plus FreePats assets. Six additional library families are catalogued as candidates. NI Session Guitarist remains a separate Kontakt workflow; PPGU uses original or legally compatible sound sources for its own bank.
