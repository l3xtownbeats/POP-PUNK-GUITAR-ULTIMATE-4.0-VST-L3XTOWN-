# PPGU Sound Forge (first working slice)

A small offline-only helper. It does not run in the VST3 and does not touch the realtime audio thread.

## Commands

    python tools/soundforge/ppgu_soundforge.py scan path/to/instrument.sfz --json instrument-package.json
    python tools/soundforge/ppgu_soundforge.py package path/to/instrument.sfz path/to/output-instrument

The first slice supports explicit SFZ `<region>` maps with common `<global>`, `<master>` and `<group>` inheritance, plus referenced mono/stereo WAV assets. It checks sample paths, WAV headers, sample-rate/channel/frame sanity, computes SHA-256 in 1 MiB chunks, detects exact duplicate content, and reports note/velocity/round-robin/articulation hints. Package mode copies only referenced WAVs and the SFZ into a staging directory, writes `instrument-package.json`, then publishes the package.

Both `rhythm` and `melody` are declared as consumers of the same package. In the plugin, the existing processor already has one `SamplerEngine` instance; Melody dispatches direct MIDI notes and Pattern/Rhythm dispatches scheduled events into that same sampler. Switching modes therefore changes event generation, not the loaded sample bank.

## Deliberate limits

- WAV + SFZ only in this first slice. Unsupported sample types and SFZ `#include` / `default_path` constructs are rejected for manual resolution rather than silently mispackaged.
- No automatic denoising, mastering or speculative audio enhancement.
- Hashing/copying is explicit offline work; no background service or GPU.
- Source files remain unchanged. Package output is separate.
- This is a first functional utility, not yet a full GUI Sound Forge editor or full multisample authoring suite.