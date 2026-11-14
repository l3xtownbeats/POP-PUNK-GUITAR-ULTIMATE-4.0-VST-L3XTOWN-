# PPGU REV.17 — Full-tree reconciliation preflight (2026-11-14)

## Scope and evidence

Base branch: POP-PUNK-GUITAR-ULTIMATE-VST-REV.17  
Base / parent HEAD: f135b0f5c9609286086772450f69d287d8ff71c5  
Working branch: work/rev17-full-reconciliation-2026-11-14

The recursive Git tree contains **769 tracked entries**. The inventory includes source, tests, build/workflow files, docs, 102 theme directories, and binary assets. This is not a claim that every binary image, ZIP archive, or WAV payload was decoded or auditioned. Targeted text review covered the processor/editor, sampler/performance code, CMake/workflow, tests, theme/gallery, sound licensing/policy, task registries, README and journals.

## Current implementation map

| Area | Live REV.17 evidence | Honest status |
|---|---|---|
| Windows x64 VST3 | CMake JUCE 7.0.12 / C++20; CI run 43463167899 on SHA 8c75820… | Prior exact-SHA build/test/pluginval/package PASS |
| FL Studio 20 | User confirmed plugin launches and works | User-reported host evidence; exact installed binary SHA not matched |
| Sampler | Source/Sampler production modules and async SFZ loading | Source present; full format compatibility not claimed |
| PATTERN | Host/internal PPQ scheduler, chord transitions, 8 rhythm masks | Runtime path present; section-first UX absent |
| Presets | 300 definitions/catalog | Not proof of 300 fully playable presets |
| Skin Gallery | Image cards, search, family filter, active marker | Partial; click commits immediately |
| Skin Engine | 21 tokens / 28 control IDs in contract; 102 themes | Reconcile runtime schema and current tree before further merge |
| Overlay / secure import | No live production OverlayManager/SecureSkinImport files | Blocked / absent |
| Sound Hunter | Validator and tests; empty registry | Policy tooling present; discovery/curation/approved assets absent |

## Highest-value safe integration order

### P1 — PATTERN Section → Rhythm/Pattern
Add a distinct musical section model (Intro, Verse, Chorus, Bridge, Outro) and a separate rhythm-pattern catalog/selection stage. Persist section-to-pattern associations and preserve old sessions using the legacy global patternSlot as migration fallback. Preview must not silently commit. Keep scheduler sample accuracy, host/internal clock, chord handling, and audio-thread boundedness unchanged. Add tests for association, persistence/migration, MIDI, and transport.

### P1 — Skin Engine security foundation
Do not transplant the uploaded ZIP tree wholesale. Its OverlayManager close behavior has a failing assertion, and its SVG policy has confirmed mixed-case event-handler and protocol-relative href bypasses. Reimplement/adapt against current REV.17 APIs, test security edge cases, then integrate workspace overlays and ZIP quarantine/import.

### P2 — Gallery transaction UX
Add testable draft selection state first, then enlarged Preview, Apply, Back/cancel and Favorites. Preserve the existing theme identity and ensure theme operations never touch APVTS/DSP/MIDI.

### P2 — Sound Hunter
Keep asset-level legal review, provenance, SHA-256, quality screening and guitar-only classification mandatory. Registry is currently empty; no new sample should enter the distributable bank until exact per-asset rights and technical evidence are present. DeepSearch/Curator is not production-integrated.

## Conflicts requiring explicit correction

1. PPGU_TASK_REGISTRY.md says SH-01 sound-registry CTest assertions are unfixed; later Info.txt and CODE records document test corrections and CTest success on SHA 8c75820…. Preserve history and add a dated correction rather than silently editing the old record.
2. SKIN/SKIN_ENGINE_GAP_AUDIT_RU.md describes older REV.14 fixed 1..51 behavior; live REV.17 has a 102-theme registry and a more developed gallery. Treat the old audit as historical, not current runtime truth.
3. Earlier README/INFO entries say FL Studio 20 host validation pending on prior SHAs. The user's 2026-11-14 confirmation establishes successful use, but not the exact binary SHA installed. Keep host evidence distinct from CI evidence.
4. Windows Release/CTest/pluginval strictness 5/package PASS belongs to SHA 8c75820cab1b909ff43c55736f25150338e384d0 / run 43463167899. It does not automatically certify later HEAD f135b0f5c9609286086772450f69d287d8ff71c5.

## Non-negotiable invariants

- SIGNAL FORGE / NEON visual identity remains unchanged.
- No silent APVTS ID/semantic changes.
- No unbounded work, file I/O, locks, allocations, waits, networking or decoding in audio callback.
- Preserve sampler snapshot lifetime, cache/voice limits, MIDI scheduler and host timing.
- No unsupported sample licensing claims; no ZIP-wide rollback.
- Update INFO and CODE in the same transaction as every accepted change; record exact SHA and exact checks.

## This checkpoint

This branch records reconciliation only. No production source, CMake target, binary asset or runtime behavior was modified. No build, CTest, pluginval, package or host test was run on this checkpoint. Next source change should be a separately scoped PATTERN section model with regression coverage, after the complete APVTS/state/preset migration map is finalized.


## 2026-11-14 — Conversation-wide product and NEON STAGE decision register

This addendum consolidates accessible prior discussion, approved user decisions and project specification. It does not replace earlier history and does not imply implementation.

### Primary identity and visual direction
SIGNAL FORGE / NEON is the immutable brand/system identity. NEON STAGE is the latest requested primary/default theme label; recorded order: NEON STAGE (skin-1), SOUND FORGE (skin-2), BLACKOUT VINTAGE (skin-3). Reconcile label/default with the current registry and Skin_052 mapping through a dedicated tested change; never silently rename stable IDs. Old NEON PUNK and SIGNAL FORGE-as-card names are historical variants. NEON STAGE: dark graphite/black metallic chassis, machined-metal details, electric neon-blue outlines/signal accents, restrained cyan/blue/purple/pink glow, punk paint texture, PPGU crown/monogram, high-contrast typography, professional knobs/switches/LEDs/meters, clean hierarchy across Preset/Library, MELODY/PATTERN, Guitar/Articulation, Amp/Cab, FX, keyboard/status. Categories: RETRO-PUNK, DARK ROCK, VINTAGE, METAL CORE, CLASSIC PUNK, MINIMAL, SIGNAL FORGE, ON STAGE. ON STAGE is a category, NEON STAGE is the theme name.

### Functional decision register
- Target: Windows 10 x64 VST3 instrument, JUCE 7.0.12/C++20, FL Studio 20.
- MELODY: chromatic MIDI notes, riffs, hooks, octave lines and lead.
- PATTERN: section-first Intro/Verse/Chorus/Bridge/Outro, then pattern/rhythm selection; independent assignment per section; non-committing Preview, Apply, Back/Cancel; host sync and clean transitions.
- Keep instrument source, tone/FX, and real performance articulation as separate axes. Techniques include palm mute, open chords, down/up/alternate strokes, staccato power chords, chugs, skank/offbeat upstrokes, straight eighths, gallop, stop-time/stop-start, half-time, double-stops, octave hooks, muted scratches, arpeggios, tremolo picking, sustain/ambient and dynamic build—only when backed by recordings/maps. Tremolo effect is not tremolo picking. Layering optional advanced, not default.
- AUTOCHORDS: ON/OFF, Major/Minor/Custom, Key, progression, mode/Strum, chord/fretboard view, current progression, preview/play and speed.
- SFZ/WAV import, drag/drop/Browse, user libraries, root/key/note ranges, velocity layers, RR, zones, note-off/release/gate, audition/browser and safe preview-before-load. Electric/acoustic/nylon only where actual samples exist. Velocity-sensitive palm mute must select a real muted articulation, not EQ/volume simulation. Meta Open Sound Steel/Nylon are candidates, not cleared assets.
- Real double takes and performance maps for strums/riffs/arpeggios/chord changes; string/fret/voicing/direction/timing/accent/mute/gate/release metadata; DI/mic, pickup/amp/cab/IR. Slide/hammer-on/pull-off/harmonic/tremolo only if recorded. Verse/chorus differences must be performance-based; presets loudness-matched, headroom-safe, mono/phase checked.
- FX: Compressor, EQ, Distortion/Drive, Chorus, Flanger, Delay, Reverb, Cabinet/IR. FX EDIT should only expose functional controls, bypass, mix, graph/meters as applicable.
- Factory contract: 300 definitions = 150 MELODY + 150 PATTERN; playable/validated count tracked separately. Separate request: 30 original presets in the style direction of Blink-182 “ONE MORE TIME...”. Style references include Blink-182, Sum 41, Green Day, Fall Out Boy, Good Charlotte, Simple Plan, New Found Glory, Avril Lavigne, Yellowcard, The All-American Rejects, My Chemical Romance, Less Than Jake, Fenix TX, Midtown, Bowling for Soup, MGK, Mest, Angels & Airwaves, The Offspring, Sugarcult and All Time Low; inspiration only, no copying protected recordings/riffs/assets.
- Curator can recommend/organize but cannot restrict user-added libraries. Factory distribution remains license-gated.
- Gallery: image cards, search, family filters, favorites, large Preview; Preview draft-only, Apply commits, Back/Cancel restores active theme. REV.17 inventory 102 themes; REV.14 51-theme audit is historical.
- Skin/workspace: OverlayManager safe lifecycle, z-order/focus, close/maximize/restore, saved bounds, DPI/work-area; secure .ppguskin archive import, traversal/duplicate/size checks; SVG rejects case-insensitive event handlers, script/foreignObject, external/protocol-relative links and unsafe CSS. Old ZIP OverlayManager prototype had a close defect.
- Auto Discover: internal ON/OFF, transparent opt-in; no external upload. Do not infer destructive Apply All from mockup.
- Settings: General/Audio/MIDI/GUI, MIDI Learn, tooltips, preset autosave, graphics quality, UI scale/reset; do not fake host-owned buffer-size control. Auxiliary windows independent close/maximize, safe lifetime, persisted bounds, compact/portrait/overflow/DPI.
- Sound Hunter: provenance, exact SPDX/redistribution scope, notices, SHA-256, file inventory, technical audition, guitar-only verification; unclear sources quarantined. Audited registry empty; FreePats candidates need archive verification, Nyanshell/fakeguitar YELLOW, Unreal Instruments requires explicit raw-sample rights.
- Multiprocessing and Voices/polyphony requested; bounded worker model outside callback. No I/O/network/decode/waits/allocations/blocking locks/unbounded work in processBlock. Preserve sampler lifetime, MIDI timing, APVTS IDs and audio contracts.
- Performance goals (not measured claims): first sound <=50 ms; preset <=300 ms p95; skin swap ~150 ms p95; CPU <=2% at 4 voices / <=15% at 64; RAM <=60 MB idle / <=512 MB full bank; callback peak <=60% buffer time at 128 samples.
- Portable content root VST3/SOUNDBANK/PRESETS/SKINS/Resources/Assets; safe resolver, relocatable package, manifests/checksums/notices and redistributable assets only. Windows Release, full CTest, pluginval strictness 5, package audit and FL Studio 20 install/scan/load/MIDI/audio/state checks all tied to one exact SHA.

### Conflicts and open gaps
- SIGNAL FORGE / NEON brand plus NEON STAGE primary theme: registry/default mapping remains to reconcile, no silent ID rename.
- 300 factory definitions and 30 Curator presets are distinct.
- Tone/FX and articulation are separate; tremolo effect != picking.
- User import is unrestricted by Curator; factory redistribution is legally gated.
- User-reported FL Studio 20 success is not exact-SHA CI evidence. Green build/package at 8c75820... does not certify later HEAD.
- PATTERN model still needs Processor state, Editor controls, preset migration and scheduler integration.
- Gallery draft transaction incomplete; OverlayManager/SecureSkinImport absent; SVG adversarial tests pending.
- Sound Hunter discovery and Curator integration absent; verified registry empty.
- Full control-to-action/parameter/persistence/automation inventory, 300-preset playability audit, real articulation maps, FX/amp/cab/IR verification, multiprocessing/voice implementation, benchmarks and package/release evidence remain open.
- 769-entry inventory was not a file-by-file content audit of all text, binaries and sample assets; complete manifest-based audit remains open.

**Authority:** latest explicit user decisions + SIGNAL FORGE v10 MASTER/v10.1 + approved control inventory + append-only journals. Priority: safety > realtime > sound > resources > UX. Every production change updates Info/CODE in the same commit with exact evidence.  
**Status:** REQUIREMENTS RECONCILED / DOCUMENTED; implementation and full file-by-file audit remain open.


## 2026-11-14 — USER RATIFICATION: IMMUTABLE / MANDATORY DEVELOPMENT BASELINE

**Explicit user confirmation:** the complete conversation-wide reconciliation immediately above is approved as correct and is hereby made a **mandatory, binding and immutable part of PPGU development**.

This is not a suggestion list, optional roadmap, or visual inspiration-only note. All enumerated product requirements, functions, workflows, visual rules, control expectations, library/legal constraints, performance/realtime rules, and identified omissions are part of the development contract and must be carried into implementation, review, testing, presets, packaging and future handoffs.

### Non-negotiable change-control rules
1. Do not silently delete, weaken, simplify, omit, reinterpret, replace, or revert any ratified requirement or approved design decision.
2. Do not substitute a mockup-only/fake control for the required real function; where a function is not implemented, mark it explicitly REQUIRED / DESIGNED / PARTIAL / BLOCKED / DEFERRED / SOURCE PRESENT / INTEGRATED / TESTED / HOST VERIFIED / RELEASED as evidence warrants.
3. Do not treat documentation as implementation, registered tests as passed tests, a preset definition as a validated playable preset, a named library as a licensed distributable asset, or a user-reported host result as exact-SHA release evidence.
4. All new proposals and variants must be recorded separately as PROPOSED until the user approves them. An approved variant may add detail but may not override this baseline by implication.
5. A change to this immutable baseline is permitted only after the user explicitly authorizes that specific change. Record it as a dated, append-only, versioned **SUPERSEDING AMENDMENT** stating the exact old clause, new clause, reason, affected paths/IDs, migration and tests. Preserve the previous entry unchanged. No silent edits, retroactive rewriting or inferred approval.
6. Every production transaction must check this baseline, preserve APVTS IDs/state/preset/MIDI/audio compatibility, update Info.txt and CODE.txt in the same commit, and provide exact commit/test/CI/host evidence. Any conflict or missing evidence is a blocker, not permission to guess.
7. The approved visual direction is binding: SIGNAL FORGE remains the main theme/brand identity; NEON STAGE is its approved neon visual treatment and the requested primary/default launch presentation. The latest journal label “NEON STAGE primary/default skin” means the default presentation within SIGNAL FORGE, not permission to erase or rename SIGNAL FORGE. Preserve SOUND FORGE and BLACKOUT VINTAGE variants and stable IDs; resolve registry naming only by an explicit tested migration.
8. All previously enumerated requirements and gaps in the conversation-wide decision register remain in scope. An item may be closed only with implementation and the appropriate verification evidence, or changed only through the explicit amendment process above.

**Authority:** this user-ratified baseline joins SIGNAL FORGE v10 MASTER/v10.1, approved control inventory and append-only journals as binding project authority. Conflict priority remains safety > realtime guarantees > sound > resources > UX; this priority governs implementation choices and does not authorize deleting requirements.

**Status:** USER RATIFIED / MANDATORY / IMMUTABLE BASELINE / DEVELOPMENT GATE. No production feature is claimed implemented by this journal entry.
