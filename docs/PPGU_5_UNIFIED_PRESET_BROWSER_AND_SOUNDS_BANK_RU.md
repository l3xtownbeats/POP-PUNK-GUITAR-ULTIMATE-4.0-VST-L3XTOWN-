# PPGU 5.0 — Unified Preset Browser & Sound Architecture

Status: architecture decision / implementation contract  
Target branch: `feature/ppgu-5-import-sounds`  
Scope: MELODY, PATTERN, STYLE overlays, factory/user presets, Sounds Bank, FX recall.

## 1. Core decision

Use ONE preset browser backed by ONE preset model and one Sounds Bank index. Do not create separate unrelated preset systems for Melody and Style.

The browser has two primary performance modes:

- **MELODY** — direct MIDI-note playing. Every incoming key plays its corresponding pitch (or a configured chord voicing where the selected feature explicitly requests it).
- **PATTERN** — performance/rhythm playback. Incoming MIDI notes select/transposes notes or chord roots according to the selected pattern definition. A pattern preset owns its rhythm, articulation defaults, strum behavior and FX snapshot.

A preset is not merely a label or a collection of UI values. It is a resolvable recipe containing sound sources, articulations, performance behavior, FX state and compatible mode.

## 2. Browser information architecture

Top-level browser tabs:

1. **MELODY**
2. **PATTERN**
3. **FAVORITES**
4. **USER** (user-created/imported presets)

Inside MELODY, show two coordinated selectors:

- **SOUND CHARACTER** — what the instrument sounds like.
- **PLAYING STYLE** — how the note is articulated/performed.

MELODY sound-character groups:

- Clean
- Acoustic
- Nylon
- Steel Acoustic
- Open / Natural Electric
- Muted
- Flageolet / Harmonics
- Tremolo Clean
- Chorus Clean
- Crunch
- Overdrive
- Distortion
- High Gain
- Ambient / Sustained
- Experimental

MELODY playing-style profiles:

- Lead
- Solo
- Legato
- Riff
- Power Chords
- Open Chords
- Palm Mute
- Tight Chugs
- Tremolo Picking
- Arpeggio
- Octave
- Single-Note Rhythm
- Fingerstyle
- Strummed Acoustic

The two selectors are orthogonal. Example: **Clean + Lead** is a clean sustained lead; **Muted + Lead** is a muted lead/palm-muted single-note tone; **Acoustic + Arpeggio** is an acoustic arpeggio. A style profile may request articulations and performance shaping, but must not silently replace the chosen sound character.

For PATTERN, the browser presents:

- **ARTIST / SONG-INSPIRED BASE PRESET** (e.g. Blink-182 — Lead, Sum 41 — Rhythm, Simple Plan — Lead, Good Charlotte — Rhythm, New Found Glory — Rhythm, Mest — Photographs).
- **OPTIONAL PLAYING STYLE OVERLAY** (None / Lead / Solo / Palm Mute / Open Chords / Power Chords / Tight Chugs / Acoustic Strum / Arpeggio / Tremolo Picking).
- **FX SNAPSHOT** is recalled from the base preset unless the overlay explicitly contains an FX override.

If no PATTERN overlay is selected, play the artist preset exactly as authored. If an overlay is selected, it modifies only the properties it owns (articulation, note length/gate, strum spread, picking/velocity accents, and optionally a declared FX override). It must not replace the base preset's identity, sound source, pattern notes, tempo behavior or FX values that the overlay does not explicitly override.

Example: `Blink-182 — Lead` + `None` = original authored Blink-inspired lead pattern.  
`Blink-182 — Lead` + `Palm Mute` = same base pattern, rendered with short damped palm-muted articulation.  
`Blink-182 — Lead` + `Open Chords` = same base pattern with ringing/open articulation where the mapping supports it.

## 3. Resolution and precedence rules

Resolve a playable selection in this order:

1. Load the selected base preset (mode, pattern/MIDI behavior, sound profile, sample references, FX snapshot, macro defaults).
2. Resolve its sample references against the Sounds Bank index.
3. Apply the selected optional Style overlay, if any. Only explicitly declared overlay fields override base values.
4. Apply user-edited controls as the final runtime layer.
5. Persist user edits only when the user chooses Save / Save As; browsing a factory preset must not mutate it.

For MELODY, the sound character is the base tone and the playing style is an articulation/performance layer. For PATTERN, the artist preset is the base performance and the selected style is an optional overlay. This removes the ambiguity between a sound preset and a playing-style preset.

## 4. Preset data contract

Use stable IDs, not display names, for all references. Suggested logical shape:

```json
{
  "schemaVersion": 1,
  "id": "melody.clean.lead.001",
  "name": "Clean Lead",
  "mode": "MELODY",
  "category": "Clean",
  "soundCharacter": "clean",
  "playingStyle": "lead",
  "sampleSetId": "bank.electric.clean.main",
  "articulationMapId": "articulation.electric.standard",
  "fxSnapshotId": "fx.clean.space",
  "parameters": {},
  "tags": ["clean", "lead", "sustain"]
}
```

Pattern preset example:

```json
{
  "schemaVersion": 1,
  "id": "pattern.blink182.lead.001",
  "name": "Blink-182 — Lead",
  "mode": "PATTERN",
  "artistReference": "Blink-182",
  "soundCharacter": "electric",
  "patternId": "pattern.blink182.lead.core",
  "sampleSetId": "bank.electric.pop-punk",
  "fxSnapshotId": "fx.blink182.lead",
  "defaultStyleOverlay": null,
  "parameters": {},
  "tags": ["pop-punk", "lead", "artist-inspired"]
}
```

Overlay example:

```json
{
  "schemaVersion": 1,
  "id": "style.palm-mute.tight",
  "name": "Palm Mute — Tight",
  "overrides": {
    "articulation": "palm_mute",
    "noteGate": 0.42,
    "strumSpreadMs": 0,
    "velocityAccentProfile": "tight_chug"
  }
}
```

These are contract examples, not a claim that the current runtime already supports every field. Validators must reject unknown enum values, duplicate IDs, missing references, invalid numeric ranges and incompatible mode combinations.

## 5. Sounds Bank layout and import rules

Keep imported originals in a managed library, grouped by format, without destructive conversion:

```text
Sounds Bank/
  WAV/
    Electric/
    Acoustic/
    Palm Mute/
    User Samples/
  AIFF/
    Electric/
    Acoustic/
    User Samples/
  FLAC/
    Electric/
    Acoustic/
    User Samples/
  OGG/
    Electric/
    Acoustic/
    User Samples/
  SFZ/
    Electric Guitar/
    Nylon Guitar/
    Steel Guitar/
    User Instruments/
  LibraryIndex/
    sounds-index.json
    sources.json
    checksums.sha256
  Cache/
```

Import behavior:

- Copy or register files into the chosen Sounds Bank location; never silently alter originals.
- Detect file type from content/header where practical, not extension alone.
- For SFZ, preserve the complete relative directory tree and all referenced sample paths. Reject or report missing samples; do not flatten the instrument folder.
- Generate stable sample IDs, metadata, duration/channel/sample-rate/bit-depth fields where available, source path and SHA-256 checksum.
- Build/update the index during import or explicit rescan, never recursively scan the full bank on every preset selection.
- Decode supported formats through tested readers. WAV is the initial guaranteed runtime format; AIFF/FLAC/OGG remain capability-gated until decoder support and tests are in place. SFZ is a mapping format whose referenced audio files must also use supported decoders.
- Stream large files from disk using bounded page cache; do not preload the entire bank into RAM.
- Missing files must produce a clear browser warning and a repair/relink action. Never substitute a different sample silently.
- Cache is disposable and rebuildable. Deleting presets/cache must never delete imported source audio.
- Do not bundle or redistribute third-party samples unless their license explicitly permits it; preserve license/source metadata.

## 6. FX and sound ownership

A preset may reference an FX snapshot by stable ID. The snapshot stores real DSP parameter values, not just UI positions. FX chain order is explicit and validated. Style overlays may optionally define a small, declared FX delta (for example, slightly shorter gate or brighter tone), but the default overlay must not overwrite the base preset FX chain.

The same FX engine is shared by the browser, preset recall and manual FX Studio controls. Selecting a preset must update the actual DSP state and UI consistently.

## 7. UX behavior

- One search field searches names, artist references, sound character, playing style, tags and source labels.
- Filters: Mode, Sound Character, Playing Style, Artist, Source Format, Favorites, Factory/User.
- Preset rows/cards show name, mode, sound character, playing style, source availability and a small FX summary.
- A preview action auditions the selected sound safely without changing the active MIDI performance state.
- Selecting a preset loads it; selecting a style overlay updates the active selection non-destructively.
- Provide clear **Base Preset** and **Style Overlay** labels in PATTERN mode so users understand what is being combined.
- Provide a one-click **Reset Overlay** and **Reset Preset**.
- Favorites and last-used selections persist.
- The UI must remain responsive; library indexing, file decoding and preview preparation happen off the audio thread.

## 8. Real-time and reliability requirements

- No file I/O, blocking locks, heap allocation or library scanning on the audio thread.
- Preset/sample resolution and decoding happen on worker threads; publish immutable prepared snapshots to the audio engine.
- Preset changes must be safe while notes are active; use a bounded transition/crossfade or defer swap to a safe audio boundary.
- MIDI note-on/note-off pairing must remain correct if preset, key mapping or overlay changes while notes are held.
- Test missing files, corrupt audio, unsupported codec, large bank, duplicate import, moved library, SFZ relative paths, rapid preset switching, active-note switching and cache pressure.
- Performance profiles and polyphony caps remain separate from sound/style identity; do not hide or remove the existing voices control.

## 9. Delivery phases and acceptance gates

A. Data model + validators: stable IDs, mode/category/overlay schema, reference validation.  
B. Sounds Bank index/import: safe copy/register, format detection, SFZ path preservation, checksums and relink.  
C. Runtime resolver: base preset + overlay + user override precedence, worker-thread preparation and safe audio handoff.  
D. Browser UI: MELODY/PATTERN tabs, two-level selectors, filters, search, favorites, preview and reset.  
E. DSP binding: articulation routing, sample-set selection, FX snapshot recall and overlay deltas.  
F. QA: unit/integration tests, Windows x64 Release, CTest, VST3 bundle check, pluginval strictness 5, then manual Windows 10 + FL Studio 20 acceptance.

A feature is not complete merely because its data or controls exist. Mark each phase complete only after its code, tests and acceptance evidence are present.

## 10. Product naming and scope

The product remains **POP PUNK GUITAR ULTIMATE**. The Sounds Bank and unified browser are the 5.0 architecture direction; do not label the existing 4.0 release as 5.0 or replace its stable branch. Keep the current 4.0 baseline intact and develop this work on the dedicated 5.0/import branch.
