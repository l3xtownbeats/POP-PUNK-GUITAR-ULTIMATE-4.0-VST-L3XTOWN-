# Preset File System — POP PUNK GUITAR L3XTOWN VST3

Status: approved product specification; implementation must be wired into the VST3 before these files are advertised as loadable.

## User-facing design — reconciled 2026-11-14

The PRESET BROWSER is the separate Tone / Factory / User preset browser. MELODY and PATTERN remain preset/data classifications and the approved 150 + 150 factory-definition target; they do not merge the separate STYLE and PATTERN discovery browsers into one preset-browser screen.

The browser supports the approved logical actions: category navigation, search, favorites, previous/next within the current result set, isolated preview where implemented, explicit SELECT/LOAD, Save As, rename, delete user presets, import/export banks and rescan. A click selects only; double-click is not an implicit load. Factory content is read-only; user content is writable. These actions must map to the first approved mockup's exact control inventory and positions; no additional visible controls or screens are authorized by this file.

## File and folder layout

Default Windows user-data root:
`%APPDATA%/L3XTOWN/POP PUNK GUITAR/`

```text
Presets/
  Factory/
    Melody/
    Pattern/
  User/
    Melody/
    Pattern/
  Banks/
Samples/
  (separately installed/licensed sample libraries)
```

Factory preset files may carry descriptive genre/style and compatibility metadata. The tree above is a storage layout only, not a visible named artist/band catalog or authorization to expose protected artist/song identities. GROUP/ARTIST/SONG levels remain subject to the current MASTER PROMPT restrictions; do not publish or populate a named public catalog while that gate is BLOCKED. STYLE and PATTERN browsers remain separate from PRESET BROWSER.

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
- Search matches preset name, approved non-protected descriptive tags and description; do not expose a blocked named artist/band/song catalog.
- Artist/category folders are navigable; sorting supports name and recently used.
- Single-click selects/highlights only; it must not change the active sound.
- Preview is a separate, isolated audition and must not alter the active preset or DAW project state.
- Explicit SELECT / LOAD commits a validated preset atomically; failed loads preserve the previous active preset.
- Double-click is not an implicit load command under the newer approved workflow.
- Save, Save As, Rename, Delete, Import Bank, Export Bank, Rescan.
- Current mode may filter incompatible preset types; a selected/previewed item never switches active mode. Only successful explicit LOAD may switch MELODY/PATTERN mode atomically with the preset.

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


## Approved browser workflow reconciliation — 2026-11-14 (supersedes older interaction wording)

This file's earlier double-click-load interaction is superseded by the newer approved contract recorded in Info.txt. Keep this document as the preset file/storage specification; Info.txt is the sole development journal.

### Navigation models
- Pattern discovery: GENRE → ARTIST/BAND → SONG → SECTION → STYLE → PATTERN.
- Section values: Intro, Verse, Pre-Chorus, Chorus, Bridge, Outro.
- Melody discovery remains PRESET → STYLE → PATTERN. Do not force the Pattern hierarchy onto Melody.
- The hierarchy is logical metadata navigation, not permission to add new screens or visible controls outside the first approved mockup.

### Browser controls and actions
BROWSER; FAVORITES; CATEGORIES; FACTORY/USER; SEARCH; FILTER; SORT; PREV/NEXT; FAVORITE; PRESET NAME; CATEGORY; PREVIEW; SELECT/LOAD; SAVE; SAVE AS; RENAME; DELETE; IMPORT; EXPORT; RESCAN.

Map these functions to the first approved mockup's control inventory and positions. Do not create duplicate browser/catalog/schema implementations or decorative controls. If an action has no real backend yet, show it as unavailable/disabled rather than simulating success.

### Result-set behavior
- Search, filters and sort operate on one current deterministic result set.
- PREV/NEXT traverse only that result set and respect the active hierarchy and filters.
- Changing a parent hierarchy value refreshes dependent child values and clears only invalid descendants.
- Favorites are persistent user preferences; retain whether an item belongs to Factory or User.
- Empty/loading/unavailable/invalid/error states and counts must reflect actual catalog/index data only.

### Selection, preview and load are separate
- Selected: browser focus/highlight only; no audio, parameter, mode, or project-state change.
- Previewed: isolated audition only; must not replace active preset, change persistent parameters, or dirty/alter DAW project state.
- Loaded/Active: only after explicit SELECT/LOAD and successful validation plus atomic commit.
- If safe independent audition is not implemented, PREVIEW is disabled with an honest reason. Never implement preview by loading the preset.
- On any preview/load failure, retain the previous active state and report the actual error.

### Storage, validation and safety
- Stable unique ID is authoritative; display name is not. Rename preserves ID.
- Factory presets are immutable/read-only. SAVE AS always creates a User copy. User destructive operations require confirmation.
- Factory and User namespaces remain separate; updates must never overwrite user presets/favorites/banks.
- Validate schema, format version, ranges, mode, APVTS mapping, state restore, sample profile/library, articulation and FX dependencies before commit.
- Missing/invalid assets, unsupported version, malformed data or incompatible parameters must fail safely without partial state or silent substitute samples.
- Import/export/rescan must enforce path containment, size/schema validation, compatibility and licensing/provenance checks. Index publication is atomic; failed operations retain the prior valid index.
- Keep the 300 factory definitions and 150 MELODY + 150 PATTERN target unchanged unless separately approved. Definitions are not proof of playable/loadable presets.
- File I/O, parsing, indexing, hashing, scanning and preview preparation stay off the audio thread. No audio callback file I/O, allocations, blocking locks/waits, decoding, logging or GUI calls.

### Visual and product invariants
- First approved mockup is the sole layout/control-inventory authority.
- Second reference “02 INTERFACE NEON STAGE AUTOCHORDS” provides palette only, not an alternate layout.
- Third FX EDIT + AutoChords reference remains a distinct coordinated UI state.
- Skins may change visual appearance/palette only; they do not change geometry/function, APVTS IDs/semantics, processor state, presets, MIDI, sampler, DSP or scheduler behavior.
- Artist/Band/Song names are metadata/reference taxonomy only; they do not authorize copying protected songs, riffs, stems, recordings, commercial libraries or proprietary presets.
- Apply the approved SoundBank GREEN/provenance and license-clearance gate to every included sample/asset.
