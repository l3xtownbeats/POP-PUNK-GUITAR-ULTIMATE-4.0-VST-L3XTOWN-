# MAJOR USER-APPROVED PRODUCT REQUIREMENTS AMENDMENT — 2026-11-14
## PPGU — mandatory requirements; append-only; documentation baseline

**Authority:** explicit user approval in the conversation dated 2026-11-14. This amendment supersedes earlier conflicting product decisions only in the domains listed below. All unrelated master invariants, safety rules, compatibility rules, realtime rules, stable identifiers, and release gates remain in force.

**Status vocabulary:** APPROVED / REQUIRED / DESIGNED / SOURCE PRESENT / INTEGRATED / TESTED / HOST VERIFIED / RELEASED. This document records approved requirements; it does not claim that the corresponding production implementation exists.

## 1. FX EDIT and DSP — expanded mandatory scope

- FX EDIT is not limited to the seven effects shown in the earlier compact inventory.
- The complete FX EDIT must support **up to 15 fully implemented effect/processors**, plus a separate cabinet section (cabinet models and/or cabinet/IR handling as specified by the approved FX design).
- The DSP product requirement is **at least 10 processors/effects, with a target scope of 15**. The 15 FX EDIT slots are the upper design scope; cabinets are additional and are not silently counted as a substitute for the minimum DSP processors.
- Each module must have real DSP/backend wiring, parameter mapping, truthful enable/bypass state, preset/state persistence, automation where supported, safe parameter updates, and tests. No decorative or fake controls.
- Preserve the approved FX EDIT mockup: do not redesign its composition or module placement as part of backend implementation.
- Existing APVTS IDs and semantics remain immutable. New parameters/IDs or routing changes require scoped compatibility review and migration/regression tests; do not rename or repurpose existing IDs.
- The exact 15-module inventory must be reconciled against the user's earlier FX specification and repository design records before implementation. Do not invent module names to fill slots.

## 2. AUTO DISCOVER — full implementation required

- AUTO DISCOVER is a mandatory complete production feature, not a placeholder.
- Preserve the approved UI contract: explicit ON/OFF control inside the window; OFF means no background or persistent discovery; ON permits discovery but does not start an implicit scan; a separate SCAN/SEARCH action starts a bounded, cancellable task; the user can stop it.
- No scheduler, automatic repeat, or silent background scanning without separate explicit approval.
- Run discovery only outside the audio callback and through the existing approved discovery/import pipeline (no second competing pipeline).
- Provide real progress, results, errors, cancellation, deduplication, provenance/checksum and clear actions. Never fabricate activity or results.
- Discovery must not prevent the user from manually importing or using a library.

## 3. SKIN SYSTEM — NEON STAGE DEFAULT is the primary default

- The primary/default theme display name is **NEON STAGE DEFAULT**. This is the user's explicit MAJOR change to the former I7 default-theme naming rule.
- The first user-supplied NEON STAGE mockup is the authoritative reference for exact layout, proportions, component placement and visual appearance. The second supplied mockup is the authoritative colour-palette reference.
- Do not substitute a fresh design, simplify the approved layout, move controls, change functions, change bindings, or alter sound.
- Additional themes are derived from the NEON STAGE DEFAULT master layout. Skins may change graphic presentation, textures, colours, decorative assets and approved visual states only. They must not change control inventory, coordinates/layout geometry, functions, APVTS bindings, MIDI behaviour, DSP, sampler behaviour or sound.
- Convert the approved mockups into real, correctly sliced/exported PNG asset sets (and other explicitly approved asset formats where required). Each asset must map to a real component/state/scale use; include manifests, dimensions, checksums, density/scale variants as needed, and validation. Do not treat one flattened mockup image as a functioning interface.
- Default theme must load automatically on first launch and when no valid saved theme exists. A user's valid saved theme may persist according to the existing state/persistence contract.
- Retain stable IDs and migration safety. The canonical default display name changes to NEON STAGE DEFAULT; retain the existing canonical default ID `default-ppgu` unless a separately reviewed migration proves a safe additive ID transition. Do not silently break stored projects or skin references.
- Update the previous I7/R18 default-name contract through this explicit MAJOR amendment; all unrelated I1–I8/R01–R34 invariants remain unchanged.
- Full runtime integration, gallery application, persistence, visual parity, zero-residue and all-panel tests remain required; design assets alone are not implementation.

## 4. Factory preset catalog — open-ended, extension-only

- The factory catalog is **300 or more**; 300 is a minimum baseline, not a maximum or fixed target.
- Add as many factory presets as can be supported by verified source material, valid performance maps and quality control. There is no arbitrary 300-preset ceiling.
- Every factory preset must correspond to its intended PUNK GROUP / sound-group definition and actually produce the intended tone and playing behaviour. The existing user-supplied PUNK GROUP list is the canonical source for group/style mapping; do not replace it with an invented list.
- Each preset must identify its actual guitar source, tone/DSP configuration, articulation/performance map, playable range/velocity coverage and required assets. Missing source mappings must be marked unavailable/design-only, not called playable.
- Existing public internet research and ready-made development work may be reused only where the exact rights permit the intended use. Do not copy proprietary samples, closed commercial presets, protected riffs/transcriptions, or other restricted assets. Keep source/provenance, license, checksum and attribution records.
- Factory catalog remains append-only: stable IDs, factory read-only, no silent overwrite/deletion/substitution. User presets/banks remain separate and user data survives updates.

## 5. Preset Browser — first category and navigation

- The first top-level category is **Pop Punk Essentials**.
- Preserve the approved contextual sequence: **GENRE → ARTIST/BAND → SONG/SECTION → STYLE → PATTERN**, with section choices INTRO, VERSE, PRE-CHORUS, CHORUS, BRIDGE and OUTRO.
- STYLE and PATTERN remain separate browser entities/windows as already approved; the hierarchy must not flatten their different meanings. STYLE selects how the guitar is performed; PATTERN selects the rhythmic figure.
- Browser controls and behaviour remain mandatory: BROWSER, FAVORITES, CATEGORIES, SEARCH, FILTER, SORT, PREV/NEXT, FAVORITE, PRESET NAME, CATEGORY, PREVIEW and SELECT/APPLY/CANCEL as applicable.
- PREVIEW must not modify the active preset, project, persistent DAW state or saved Pattern Slot. PREV/NEXT must remain inside the current search/filter/sort result context.
- The ARTIST/BAND visible-name restriction in the pre-existing master rules is not silently waived by this amendment. Preserve the hierarchy slot, but do not display restricted artist/band/product names until that separate restriction is explicitly amended. PUNK GROUP mapping may still be used internally for sound matching and QA.

## 6. SOUND HUNTER / commercial libraries — user-use non-blocking rule

- SOUND HUNTER must **not prohibit or restrict the user's ability to download, import, load or locally use commercial sample libraries** that the user has lawfully obtained or is otherwise entitled to use.
- Licensing labels/checks are for provenance, factory-content clearance and redistribution/package decisions; they are not a local-use lockout.
- GREEN/YELLOW/RED/GOLD may describe whether PPGU may redistribute/include a library in its own installer. They must not silently disable, delete, quarantine, hide or block the user's locally selected library solely because it is commercial or not redistributable by PPGU.
- The plugin must distinguish: (a) user-local use, (b) inclusion in a PPGU factory bank, and (c) redistribution in a release package. Only (b)/(c) require rights covering that distribution.
- Preserve safe path validation, checksum/provenance metadata, corruption handling, explicit warnings and user control. Technical capacity limits are engineering constraints, never disguised licensing restrictions.

## 7. Implementation order and acceptance

1. Reconcile this amendment with the exact FX/DSP list, PUNK GROUP source list, approved mockup files and current source/control schemas.
2. Update canonical requirements and journals without rewriting history.
3. Implement in scoped changes: NEON STAGE asset slicing/theme integration; AUTO DISCOVER; FX/DSP modules; preset catalog and first browser category; commercial-library non-blocking behaviour.
4. For each change, record exact files/symbols, before/after, unchanged invariants, tests and exact commit SHA.
5. Require Windows x64 Release, full CTest, pluginval strictness 5, package audit and separate FL Studio 20 host acceptance on the same release-candidate SHA.
6. No implementation, test, CI, host-verification or release status may be inferred from this requirements amendment.

## 8. Unchanged strict rules

Realtime callback remains free of file/network I/O, blocking locks/waits, GUI calls, decoding and unbounded allocation. Existing APVTS IDs/semantics, MIDI contracts, sampler safety, stable preset IDs, factory read-only/append-only behaviour, explicit import conflict handling, secure path/asset validation, exact-SHA release evidence and honest implementation statuses remain mandatory.


## 9. MAJOR UI clarification — MELODY and PATTERN browser behaviour (2026-11-14)

This section records the user's explicit clarification of the previously agreed browser design. It supplements section 5 and supersedes only conflicting browser-display restrictions to the extent stated below.

### 9.1 MELODY browser: exactly three coordinated areas
The MELODY-mode browser is split into three distinct areas, in this order:

1. **PRESET / GUITAR IDENTITY** — pop-punk guitar/genre, PUNK GROUP or band-style identity, and the individual song part/section. This selects **which guitar/source and which section character** is used.
2. **STYLE / PLAYING ARTICULATION** — how the guitar performs the notes. Examples: **Open Chord, Palm Mute, Muted**, plus other styles/articulations only where supported by actual mapped recordings. STYLE does not select the rhythmic sequence.
3. **PATTERN / RHYTHM VARIATION** — the rhythmic figure/variation associated with the selected guitar/section/style. It is an optional layer available from within MELODY mode and does not turn ordinary melody notes into chord triggers.

Keep the three areas visually and functionally separate. Preserve independent selection state and compatibility filtering; do not silently conflate guitar identity, articulation and rhythm.

### 9.2 MELODY note semantics and optional pattern activation
- Ordinary MELODY keys play their corresponding pitched guitar notes. A single ordinary melody note is played once and decays; it does not automatically launch a strumming loop.
- The optional PATTERN layer is activated by additional pattern-trigger MIDI notes/events drawn or played in the Piano Roll, or by an explicit dedicated PATTERN on-screen button/control.
- The approved Piano Roll key-zone convention reserves **C0–C1** for pattern-trigger keys; **C1 and above** remain the ordinary melody-note range. Pattern-trigger keys must not also play as ordinary chromatic guitar notes. The exact boundary and mapping must be covered by tests and exposed clearly to users.
- Holding a valid pattern-trigger note sustains/runs the selected rhythmic pattern according to its gate/host-transport rules; releasing it ends the pattern cleanly.
- The PATTERN area/button lets the user switch among different rhythm variations. While playing, changes must transition safely at a musical boundary, avoiding stuck notes, duplicate triggers, timing jumps and abrupt invalid cuts.
- The on-screen activation route must not alter the ordinary melody-note MIDI semantics.

### 9.3 PATTERN mode remains separate
- MELODY and PATTERN are separate performance modes with distinct MIDI semantics and preset/performance state, while sharing the approved sample library/cache and compatible source assets.
- In PATTERN mode, mapped keys select chord/voicing and trigger the corresponding recorded or validly assembled rhythmic guitar performance.
- Guitar/source identity, PLAYING STYLE / ARTICULATION, and RHYTHMIC PATTERN / variation remain separate selection dimensions. Preserve the already approved PATTERN categories and eight Pattern Slots per section.
- Native Instruments Session Guitarist — Electric Sunburst Deluxe is a behavioural/workflow reference only; no protected code, samples, presets or UI assets may be copied.

### 9.4 Browser invariants and visible PUNK GROUP selection
- PREVIEW remains non-destructive; APPLY/SELECT commits; CANCEL rolls back. PREV/NEXT, SEARCH, FILTER, SORT and FAVORITES preserve the current result context.
- Unsupported articulations or patterns must be visibly unavailable/disabled with a truthful reason; no decorative controls or silent substitution.
- The PRESET/GUITAR IDENTITY area explicitly includes the user's PUNK GROUP/band-style identity and song-section selection. This direct clarification supersedes the earlier visible-name restriction only for this approved selection dimension. Use the canonical user-provided PUNK GROUP list once reconciled; do not invent or substitute group names.
- All realtime, APVTS, sampler, MIDI, stable-ID, factory/user separation and exact-SHA validation rules remain unchanged.

**Status:** APPROVED / DOCUMENTATION ONLY. No production code or runtime behaviour is claimed implemented or tested by this amendment.


## 10. PATTERN mode — two-window preset and performance browser (2026-11-14)

This records the user's explicit clarification of the previously agreed PATTERN-mode browser layout. PATTERN mode has **two separate windows**:

### 10.1 Window 1 — PRESET / GUITAR IDENTITY
A hierarchical browser for:
- pop-punk genre / guitar type;
- PUNK GROUP / band identity;
- song-based selection;
- individual song section and compatible nested sub-items.

The group/band and song/section levels are required dimensions and must not be omitted or collapsed into generic categories. Use the canonical user-provided group list and real source mappings; do not fabricate names or claim unsupported recordings exist.

### 10.2 Window 2 — PLAYING STYLE / RHYTHMIC PERFORMANCE
A separate browser/window for the performance choices associated with the selected guitar identity:
- **PLAYING STYLE / ARTICULATION** — how the guitar is played;
- **RHYTHMIC CATEGORY / PATTERN / VARIATION** — which rhythmic figure is performed and which variation is selected.

STYLE and PATTERN remain distinct dimensions inside this second window. The approved PATTERN categories remain DOWNSTROKES, DOWN/UP, PALM MUTE, POWER CHORDS, ALTERNATE PICKING and CUSTOM PATTERNS. Preserve P1–P10 only where these IDs exist in the canonical pattern catalog, and preserve eight Pattern Slots per section. Do not manufacture missing IDs or present unsupported mappings as playable.

### 10.3 Mode semantics and state
- PATTERN is a separate performance mode from MELODY. PATTERN plays rhythmic guitar performances and retains the approved AUTO CHORDS ON/OFF behaviour; MELODY remains one ordinary key = one pitched note.
- Window 1 selects guitar/source identity and song context; Window 2 selects playing manner and rhythmic figure. They must not be merged into a single flattened list.
- Guitar/source identity, chord/voicing, tone preset/FX, playing style/articulation and rhythm pattern remain separate semantic axes. Context filtering may narrow available choices without merging their state.
- Keep PREVIEW non-destructive; SELECT/APPLY commits; CANCEL restores prior state. PREV/NEXT, SEARCH, FILTER, SORT and FAVORITES preserve the active browser scope.
- Unsupported combinations must be unavailable with a truthful reason. No fake controls, random/incompatible sample substitution or silent fallback.
- Keep the MIDI keyboard visible and interactive; preserve all realtime, APVTS, sampler, MIDI, stable-ID, factory/user and exact-SHA rules.

**Status:** APPROVED REQUIREMENT / DOCUMENTATION ONLY. No production UI, browser backend, pattern engine, MIDI implementation or validation is implied.
