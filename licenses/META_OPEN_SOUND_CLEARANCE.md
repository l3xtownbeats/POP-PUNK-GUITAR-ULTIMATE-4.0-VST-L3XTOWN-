# Third-party sample clearance record — Meta Open Sound

Audit date: 2026-11-10  
Product: POP PUNK GUITAR ULTIMATE  
Purpose: document candidate acoustic reference libraries before any sample is bundled.

## Meta Steel Guitar

- Upstream: https://github.com/facebookincubator/Meta-open-sound
- Release: https://github.com/facebookincubator/Meta-open-sound/releases/tag/meta-steel-guitar-v1.0.0
- Collection directory: `systems/instruments/meta-steel-guitar/`
- Collection license: CC0 1.0 Universal; the collection contains its own `LICENSE`.
- Format: WAV, 24-bit / 48 kHz.
- Published sample count: 375.
- Published mapped range: E2–C6 (MIDI 40–84), three velocity layers.
- SFZ map: `meta-steel-guitar.sfz`
- SHA256SUMS file: `systems/instruments/meta-steel-guitar/SHA256SUMS`
- SFZ SHA-256 (from upstream SHA256SUMS): `5110a8218ac6723c2cc37dd6b792f365ed76148f623d10edddfa23019eb2f885`
- Manifest SHA-256: `9f057185ac410ce2b12642318076e7391f0c025730d59d659e96f71b95be1a80`
- License SHA-256: `a2010f343487d3f7618affe54f789f5487602331c0a8d03f49e9a7c547cf0499`
- WAV release asset: `meta-open-sound-meta-steel-guitar-wav.zip`
- Published archive SHA-256: `caaa8a9c07217f4a82d1dfd9f5657c286155464b66bf4dd27797811c2558a909`
- Published archive size: 663,385,210 bytes.

## Meta Nylon Guitar

- Upstream: https://github.com/facebookincubator/Meta-open-sound
- Release: https://github.com/facebookincubator/Meta-open-sound/releases/tag/meta-nylon-guitar-v1.0.0
- Collection directory: `systems/instruments/meta-nylon-guitar/`
- Collection license: CC0 1.0 Universal; the collection contains its own `LICENSE`.
- Format: WAV, 24-bit / 48 kHz.
- Published sample count: 348.
- Published mapped range: E2–B5, three dynamic layers including picked/fingerstyle material.
- SFZ map: `meta-nylon-guitar.sfz`
- SHA256SUMS file: `systems/instruments/meta-nylon-guitar/SHA256SUMS`
- SFZ SHA-256 (from upstream SHA256SUMS): `80b1262f6f7915cffe00669b144078d75356b321ac8a809955fff97d68bc8f8e`
- Manifest SHA-256: `0b73ada4c3319ca67cba0cb94eafb7d4fe794cbdfd688969702c3d72ad6d7024`
- License SHA-256: `a2010f343487d3f7618affe54f789f5487602331c0a8d03f49e9a7c547cf0499`
- The upstream nylon SFZ uses keyswitch groups and velocity crossfade opcodes. The current PPGU SFZ runtime does not yet implement those semantics; do not advertise the nylon map as fully supported until parser/runtime coverage and integration tests pass.

## Clearance decision

- CC0 source licensing and commercial redistribution permission are confirmed by the upstream collection README, per-collection LICENSE, manifest and release metadata.
- This is a source/license audit, not a legal opinion.
- Neither collection is copied into the plugin repository or distributed in the VST3 installer by this record.
- The current WAV loader is a prototype with a full-preload memory model and a 120-second per-file safety limit. A memory-budget/streaming design and a real library-package verification step are required before bundling the full collections.
- Meta Steel Guitar is the first integration candidate because its SFZ mapping uses ordinary key/velocity zones; Meta Nylon Guitar remains blocked on keyswitch/crossfade support.
- No commercial Session Guitarist, Kontakt, or artist recordings are included.

## Source references

- Meta Open Sound README: https://github.com/facebookincubator/Meta-open-sound
- Meta Steel Guitar design specification: https://github.com/facebookincubator/Meta-open-sound/blob/main/systems/instruments/meta-steel-guitar/README.md
- Meta Nylon Guitar design specification: https://github.com/facebookincubator/Meta-open-sound/blob/main/systems/instruments/meta-nylon-guitar/README.md
- CC0 1.0 legal text: https://creativecommons.org/publicdomain/zero/1.0/
