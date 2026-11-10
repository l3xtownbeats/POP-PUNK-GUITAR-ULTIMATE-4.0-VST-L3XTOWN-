# Preset File System — POP PUNK GUITAR L3XTOWN VST3

Status: approved product specification; implementation must be wired into the VST3 before these files are advertised as loadable.

## User-facing design

The plugin has a Serum/Nexus-style preset browser with two main modes:
- Melody
- Pattern

Browser features: folder/category navigation, artist/style tags, text search, favorites, previous/next preset, double-click load, save-as, rename, delete user presets, import/export banks, and rescan folders. Factory content is read-only; user content is writable.

## File and folder layout

Default Windows user-data root:
`%APPDATA%/L3XTOWN/POP PUNK GUITAR/`

```text
Presets/
  Factory/
    Melody/
      Blink-182/
      Sum 41/
      Simple Plan/
      New Found Glory/
      Good Charlotte/
      Mest/
      Angels & Airwaves/
      The All-American Rejects/
      MGK/
      Avril Lavigne/
      The Offspring/
      Green Day/
      Bowling for Soup/
      Sugarcult/
      All Time Low/
    Pattern/
      (same artist/style folders)
  User/
    Melody/
    Pattern/
  Banks/
Samples/
  (separately installed/licensed sample libraries)
```

Factory presets ship with the plugin package or a separately installable factory content pack. User presets and banks live under AppData and must never be overwritten by plugin updates.

## File extensions

- `.ppgm` — one Melody preset.
- `.ppgp` — one Pattern preset.
- `.ppgbank` — portable bank containing preset files and bank metadata; samples are referenced, not silently copied.
- `.ppgstate` — optional complete plugin state/session snapshot for DAW project recall.

All preset files are UTF-8 JSON with a format version, stable ID, display name, mode, tags, parameter values, articulation/performance settings, and optional sample-library references. The extension is the user-facing file type; JSON keeps files inspectable and portable.

## Common preset fields

Required fields:
- `format`: `l3xtown-pop-punk-preset`
- `formatVersion`: semantic version string
- `id`: stable unique identifier
- `name`: visible preset name
- `mode`: `MELODY` or `PATTERN`
- `author`, `description`, `tags`
- `parameters`: validated plugin parameter values
- `sampleProfile`: logical library/profile identifier, never an absolute machine-specific path

Melody-specific data:
- chromatic MIDI note mapping and key range
- velocity-layer and round-robin selection policy
- open/palm-muted articulation mapping
- release/legato behavior and polyphony preference

Pattern-specific data:
- pattern/groove identifier and step/event sequence
- down/up stroke direction, timing, gate, accent and mute events
- chord-following/voicing policy and humanization
- tempo-sync and MIDI chord-trigger behavior

## Portability and safety

- Resolve sample paths relative to the configured library root; never embed a user's absolute path in a preset.
- Missing samples must show a clear warning and preserve parameter settings; do not silently substitute unrelated samples.
- Validate file size, JSON schema, numeric ranges and supported format version before loading.
- Loading a preset must be atomic: reject invalid files without leaving half-applied parameter state.
- Factory files are read-only. Save As always creates a user copy.
- Bank import must preview contents and avoid overwriting existing user presets without explicit confirmation.
- Preserve third-party sample licenses and source notices separately. A preset file contains references/settings, not copyrighted audio.

## Browser behavior

- Tabs/filters: All, Melody, Pattern, Factory, User, Favorites.
- Search matches preset name, artist/style tags and description.
- Artist/category folders are navigable; sorting supports name and recently used.
- Double-click loads; single-click previews metadata; star toggles favorite.
- Save, Save As, Rename, Delete, Import Bank, Export Bank, Rescan.
- Current mode filters incompatible preset types; loading a Pattern preset switches to Pattern mode and vice versa.

## Acceptance tests before calling this feature complete

1. Load all 150 factory Melody and 150 factory Pattern definitions through the real plugin loader.
2. Confirm every preset changes the intended parameters and mode.
3. Save, close, reopen and reload a user preset with identical values.
4. Export/import a bank on a second Windows account or clean test machine.
5. Handle missing sample libraries without crashing or losing settings.
6. Verify invalid, future-version and oversized files are rejected safely.
7. Verify DAW project save/reopen restores the selected preset and plugin state.
8. Test browser search, favorites, sorting and folder rescan with 300+ entries.

Important: the existing 300-entry JSON catalog is a source catalog, not yet proof that 300 presets are loadable in the plugin. The browser and file extensions become a shipped feature only after the plugin loader, UI and acceptance tests are implemented.
