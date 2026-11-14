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
