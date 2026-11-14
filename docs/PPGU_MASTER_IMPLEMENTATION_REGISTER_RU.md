# POP PUNK GUITAR ULTIMATE — MASTER IMPLEMENTATION & REQUIREMENTS REGISTER
**Document ID:** PPGU-MASTER-REGISTER-RU  
**Revision:** 1.0  
**Created:** 2026-11-14  
**Canonical branch:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV.17`  
**Purpose:** one complete operational checklist of approved requirements, verified existing work, partial implementations, missing work, sequence, invariants, acceptance gates and journal rules.

> **Authority and status:** This register consolidates the user-ratified product baseline, SIGNAL FORGE v10/v10.1 standards, Info.txt master consolidation (sections A–K), CODE.txt implementation history, Control Center decisions D-RETRO-001..019, and the latest available source audits. It is not evidence that a requirement is implemented. Actual status is always determined from source + tests + exact-SHA CI/host evidence. If a newer explicit user decision conflicts with this register, the newer decision must be recorded as an append-only superseding amendment before the register is revised.

## 0. Current checkpoint and evidence boundary

- Canonical branch HEAD at this document's preflight: `51c68e377ee5905e40684299348a0a4b56acac11`.
- Parallel reconciliation branch observed: `work/rev17-full-reconciliation-2026-11-14` @ `ff5c8da1ac6f9dd3d6fb0467a2a037b284294a29`. It is a separate work line; do not merge wholesale. Review path-level and reconcile only after comparison.
- No production source change is part of this register transaction. This is documentation/governance only.
- Latest documented exact-SHA CI success in the consolidated journal: SHA `8c75820cab1b909ff43c55736f25150338e384d0`, run `43463167899`: Windows x64 Release build, CTest, pluginval strictness 5 and package creation passed for that SHA. It does **not** certify later canonical HEAD `51c68e377ee5905e40684299348a0a4b56acac11`.
- Artifact recorded for that run: ID `14058390907`, SHA-256 `0742df5ba217a330ffe2617444c2267af64da9e34aa07e1cf0913d8f41bc1162`. Artifact package is historical evidence, not proof that current HEAD is released.
- User reported successful loading/operation in FL Studio 20 on Windows 10 x64 on 2026-11-14. Label this USER-REPORTED SMOKE TEST; it is not a per-feature exact-SHA host acceptance report.
- Current exact-SHA build/CTest/pluginval/package/FL Studio 20 verification for `51c68e377ee5905e40684299348a0a4b56acac11`: NOT CONFIRMED in this preflight.

## 1. Status vocabulary — mandatory

Use only:
- **USER-REQUIRED / RATIFIED** — user requirement or explicit approval.
- **PROPOSED** — assistant suggestion not yet accepted.
- **APPROVED** — accepted direction/contract; implementation may still be absent.
- **DESIGNED** — specification/architecture exists.
- **SOURCE PRESENT** — code exists, but integration or validation may be incomplete.
- **PARTIAL** — some behavior exists; named gaps remain.
- **INTEGRATED** — connected to production execution path.
- **TESTED** — relevant tests actually ran and passed on stated SHA.
- **HOST VERIFIED** — behavior manually verified in named DAW/version against identified build.
- **BLOCKED / DEFERRED / SUPERSEDED / REJECTED** — reason must be recorded.
- **RELEASED** — release gates and package identity confirmed.
Never infer one state from another. UI presence, parameter ID, mockup, manifest, test registration or CI on a different SHA is not proof.

## 2. Product, platform and architecture

### 2.1 Product identity
- [RATIFIED] POP PUNK GUITAR ULTIMATE is a real playable guitar VST instrument, not a MIDI-only pattern utility or decorative interface.
- [RATIFIED] Target: Windows 10 x64, VST3, FL Studio 20; JUCE/C++20 project baseline as recorded by the canonical build configuration.
- [RATIFIED] Fast startup, low CPU/RAM, immediate playable presets and FLEX/Nexus-like ease of use are product goals; no proprietary code/content copying.
- [RATIFIED] Preserve future Guitar/Bass shared low-level engine architecture as a design direction; Guitar and Bass products keep separate identity, articulations, banks, presets and UI profiles. Bass Edition remains planned, not a completed product.

### 2.2 Priority and engineering invariants
- [RATIFIED] Conflict priority: **Safety → Realtime guarantees → Sound → Resources → UX**.
- [RATIFIED] No file I/O, network access, decoding, waits, unbounded work, heap allocation or blocking locks in audio process callback.
- [RATIFIED] Worker threads must be bounded, lifecycle-safe and outside the audio callback; handoff to audio thread must be immutable/lock-free or otherwise proven realtime-safe.
- [RATIFIED] Preserve APVTS IDs and semantics, project/preset state compatibility, MIDI routing, transport timing, sampler lifetime, DSP routing and existing control behavior unless an explicit superseding amendment approves a change.
- [RATIFIED] UI/skin work must not change sound, DSP, MIDI, sampler, APVTS or preset semantics.
- [RATIFIED] Keep a single existing SamplePool/SFZ/streaming architecture; no parallel sampler.
- [RATIFIED] Use CHECK → ANALYZE → FIX → RE-ANALYZE → RESULT; one logical cause per commit where practical.

## 3. Visual identity, themes and complete interface

### 3.1 Identity separation
- [RATIFIED] SIGNAL FORGE is the engineering/master standard and development protocol.
- [RATIFIED] NEON STAGE is the primary visual theme and default launch/reset/fallback skin: SKIN-1.
- [RATIFIED] SOUND FORGE is independent SKIN-2.
- [RATIFIED] BLACKOUT VINTAGE is independent SKIN-3.
- [RATIFIED] SKIN-4 onward use next free sequential IDs. Never renumber/reuse existing IDs; deleted IDs remain gaps. Do not invent names for unconfirmed slots.
- [RATIFIED] NEON STAGE ≠ SIGNAL FORGE ≠ SOUND FORGE. Do not conflate names, artwork, palettes or identity.
- [PARTIAL] Registry/theme inventory count of 102 is recorded by audit, but does not prove all manifests/assets/runtime themes are correct or complete.

### 3.2 NEON STAGE design contract
- [RATIFIED] Follow the approved “01 INTERFACE — NEON STAGE DEFAULT” as the canonical complete layout and control reference. No unauthorized redesign, simplification, geometry/proportion/typography changes or return to obsolete UI.
- [RATIFIED] Dark black/graphite/metallic stage chassis, controlled electric neon-blue/cyan/purple/pink accents, signal lines/LEDs/meters, high contrast, professional typography, coherent knobs/switches/panels, readable DPI scaling.
- [RATIFIED] Preserve all approved controls, panels, proportions, decorative details and functionality. A skin changes appearance only; it does not create a new layout or engine.
- [RATIFIED] All auxiliary windows inherit the active NEON STAGE palette/style and preserve functional parity.
- [RATIFIED] Key-reactive visual feedback must reflect actual MIDI/playback state, never be fake animation.
- [PARTIAL] Full exact visual parity against the approved reference and all auxiliary windows is not yet end-to-end accepted.

### 3.3 Mandatory GUI inventory and mapping
Every visible control must be inventoried with: stable ID; panel; real callback/action or APVTS parameter; default/range/units; persistence; automation/MIDI Learn applicability; keyboard/focus behavior; enabled/disabled/error states; tests; status. No fake/decorative controls.

Required zones:
1. **TOP BAR:** PPGU crown/logo/product identity; preset previous/next/name/category/favorite/search/statistics; SKINS, Import Guitar, Auto Discover, Curator, Settings, About.
2. **PRESET BROWSER:** BROWSER/FAVORITES/CATEGORIES; eight real categories/counters; artwork/title/description/info; search/filter/select/preview; honest playable state.
3. **PLAYING CORE:** MELODY / PATTERN / PUNK GROUP; hero art; nine principal rotary controls and actual APVTS mapping.
4. **FX CHAIN:** Compressor, EQ, Distortion, Chorus, Delay, Reverb, Cabinet IR; real bypass/on state and actual DSP routing.
5. **AUTOCHORDS:** on/off, chord mode, key, pattern editor, speed, current chord, fretboard, progression pads, preview/options; until MIDI generator is integrated, clearly label setup-only.
6. **FX EDIT:** effect selector, power/bypass, effect presets, parameters, meters/GR, graph/mix; all controls connected to DSP.
7. **AMP & TONE:** amp model, Gain/Bass/Treble and level indication, connected to actual signal chain.
8. **PERFORMANCE / KEYBOARD:** Pitch/Mod, playable MIDI keyboard, real pressed/sounding key feedback, chord/note/transport/tempo/sync/volume/status.
- [REQUIRED] Complete component-by-component matrix: required / source exists / callback connected / persisted / automation / tested / missing.
- [REQUIRED] Verify all approved controls across all five approved skin slots and all auxiliary windows; do not use skin count as evidence of parity.

## 4. Skin Engine, Gallery, overlays and import security

### 4.1 Theme contract
- [SOURCE PRESENT / PARTIAL] ThemeManager palette infrastructure has a canonical 21-token model and legacy mapping; 28-property exact Skin Studio control contract is recorded in prior code history. Verify current source and tests before extending.
- [APPROVED] Immutable Palette Snapshot / lock-free paint access and concurrent theme tests, preserving 21 tokens, 28 properties and legacy compatibility.
- [REQUIRED] Validate manifest schema/version, stable IDs, palette, exact controls, asset dimensions/types, path safety, resource completeness and fallback.
- [REQUIRED] Hot-swap, persistence, first-run default, reset/fallback, DPI, memory/LRU/ECO and visual/audio invariance tests.

### 4.2 Gallery lifecycle
- [PARTIAL] Live Gallery has cards, search, family filter, count and active marker. Audited click currently commits immediately and closes.
- [REQUIRED] Implement testable selection state; draft-only Preview; explicit Apply; Back/Cancel rollback to prior active theme; enlarged preview; Favorites; keyboard navigation; persistence/reset/fallback.
- [REQUIRED] Preview must not mutate active theme or any audio/APVTS/preset state.

### 4.3 OverlayManager / auxiliary windows
- [BLOCKED] Production OverlayManager and associated tests absent in live REV.17 audit.
- [REQUIRED] Safe open/close/X, z-order/focus, drag/resize, maximize/restore, independent panel closure, SafePointer/lifetime, no use-after-free, saved bounds, work-area/DPI/compact/portrait/overflow behavior.
- [REQUIRED] Re-audit any alternate-branch implementation; a historical ZIP prototype had a close() listener bug. Do not transplant blindly.

### 4.4 Secure skin import
- [BLOCKED] Production SecureSkinImport absent in live REV.17 audit.
- [REQUIRED] Secure .ppguskin ZIP import with traversal protection, duplicate-entry handling, size/count limits, schema/manifest validation, safe extraction, checksum and rollback.
- [REQUIRED] SVG sanitizer must reject script/foreignObject, case-insensitive event handlers, external/protocol-relative href/src, unsafe CSS/url references and malformed payloads.
- [REQUIRED] Adversarial tests include mixed-case ONLOAD, protocol-relative URI, path traversal, zip bombs/oversized files, duplicate paths, symlinks if supported, rollback and checksum mismatch.
- [NOTE] Known mixed-case SVG and protocol-relative URI bypasses were reproduced only in an uploaded ZIP prototype, not in live REV.17 (where the production module was absent).

## 5. Musical workflows

### 5.1 MELODY
- [RATIFIED] Chromatic MIDI note performance for riffs, octave lines, hooks and leads.
- [REQUIRED] Each playable key triggers its mapped instrument note/sample; keyboard is not merely a preset selector.
- [REQUIRED] Articulation/velocity mapping must match real samples and avoid false UI promises.

### 5.2 PATTERN
- [RATIFIED] Section-first workflow: choose Intro / Verse / Chorus / Bridge / Outro, then choose a rhythm/playing pattern within that section.
- [RATIFIED] Each song section has its own section→pattern assignment; section and rhythm catalog are distinct concepts.
- [RATIFIED] Preview is non-committing; Apply commits; Back/Cancel discards draft.
- [REQUIRED] Persist assignments in project/preset state with backward-compatible migration for old global patternSlot behavior.
- [REQUIRED] Preserve PPQ/host transport, quantization, swing/accent, sample-accurate transitions, MIDI note safety, no clicks/stuck notes, bounded fixed event arrays and audio-thread guarantees.
- [PARTIAL] Existing scheduler path is present: held MIDI chords, host/internal PPQ clock, quantization, swing/accent, eight fixed 16-step masks, sample-accurate chord transitions and sampler note events. Current processor reads one global patternSlot 0..7; this is not section workflow.
- [SOURCE PRESENT / UNINTEGRATED] PatternSectionModel foundation was added on work branch commit `7366fca02a5ee92d8729411b04176e066c94baf6` (SongSection Intro/Verse/Chorus/Bridge/Outro, fixed five-slot assignment, legacy -1 fallback, preview/apply/cancel, schema v1/legacy schema0). Not integrated into canonical Processor/Editor/state serialization/scheduler; tests registered but execution not verified. Reconcile path-level; no wholesale merge.
- [REQUIRED] Separate rhythm-pattern catalog and selection UI; Section → Pattern; section association; preview isolation; state/preset serialization; MIDI/transport integration tests.

### 5.3 PUNK GROUP
- [RATIFIED] PUNK GROUP remains a distinct required mode; do not delete or treat it as complete because a tab exists.
- [BLOCKED / AUDIT REQUIRED] Production completeness, mappings, UI behavior and playable factory content are not confirmed. Audit and specify missing behavior before claiming implementation.

### 5.4 AUTOCHORDS
- [RATIFIED] Intended user experience includes chord progression, key/scale, chord mode, rhythm/speed, fretboard, progression pads, preview/options.
- [PARTIAL / SETUP ONLY] Current panel is recorded as setup-only; MIDI generation/playback is disabled or unconnected.
- [REQUIRED] Define explicit generator/processor/MIDI integration contract; generate and audition real MIDI with transport/state safety; tests for notes, chord mapping, host sync, cancellation and no stuck notes. Until then never label as completed.

## 6. Audio engine, sampler and articulation

### 6.1 Existing architecture to preserve
- [SOURCE PRESENT / PARTIAL] Existing SamplePool/SamplerEngine, WAV metadata index, background page streaming, fixed page cache and SFZ/WAV playback mechanisms are recorded in Info. Exact completeness must be checked at current SHA.
- [SOURCE PRESENT] SamplerEngine has 64 preallocated voice slots; this is a capacity detail, not a guarantee of 64 fully playable musical voices.
- [SOURCE PRESENT / RISK] Prior audit recorded up to 128 snapshots and 8 streaming library roots; snapshots/services are held through SamplePool lifetime to protect raw pointers; cache miss must not block audio thread; worker drains queue on normal stop but auto-recovery after worker exception is not implemented/confirmed.
- [REQUIRED] Retain transactional SFZ load/publish, generation checks, lifetime safety, bounded queues/cache/voice limits, non-blocking cache miss, worker shutdown and cancellation guarantees.

### 6.2 Required playback features
- [RATIFIED] WAV/SFZ import and playable sample zones; root/key/note ranges; velocity layers; round robin; velocity curve/response; note-off/gate/release; voice stealing; mono/stereo validation; preview.
- [RATIFIED] Import Guitar workflow: visible Browse and drag-drop target with explicit drop zone/instructions; safe Load/Cancel, metadata, sample preview, mapping and status. Do not leave ambiguous “where do I drop?” behavior.
- [RATIFIED] Sample audition must not modify active preset/project state or active playback mapping.
- [RATIFIED] Palm mute must be a real articulation/sample layer; lower velocity may select muted layers through a documented response map. Do not fake palm mute with volume/EQ alone.
- [RATIFIED] Acoustic/nylon variants of approved structures where suitable, legally distributable samples and mappings exist.
- [REQUIRED] Down/up/alternate strokes, open chords, staccato power chords, chugs, ska/offbeat upstrokes, straight eighths, gallop, stop-time/stop-start, half-time, double-stops, octave hooks, muted scratches, arpeggios, tremolo picking, sustain/ambient, dynamic build, fast skate-punk, palm-muted verse/open chorus and acoustic strum only where recordings/maps support them.
- [REQUIRED] Distinguish tremolo picking articulation from tremolo effect.
- [REQUIRED] Real performance variation/double takes; DI/mic/pickup/amp/cab/IR routing only where sample/content/source supports.
- [REQUIRED] Verse/chorus differences must be based on performance/articulation as well as tone, not gain/EQ alone.

### 6.3 Voice and multiprocessing
- [RATIFIED] Provide user-facing Voices/polyphony control and multiprocessing function/setting.
- [REQUIRED] Define musical voice count separately from physical slot capacity; bounded worker count, no thread explosion, immutable data handoff and deterministic shutdown.
- [REQUIRED] Measure CPU, RAM, voice stealing, cache pressure and latency at defined polyphony levels; report actual results rather than estimates.

## 7. Effects, amp and tone
- [RATIFIED] Required effects scope: Compressor, EQ, Distortion/Drive, Chorus, Flanger, Delay, Reverb, Cabinet/IR.
- [REQUIRED] Each block must be truly connected to DSP chain, have correct on/off/bypass state, actual parameter ranges, persistence, automation where appropriate and test coverage.
- [REQUIRED] Compressor controls include threshold, ratio, attack, release, makeup, mix and gain-reduction indication.
- [REQUIRED] Distortion/drive, chorus, flanger, delay, reverb, EQ, cabinet/IR parameters and routing must be documented; meters/graphs must display real signal state.
- [REQUIRED] Audit FX Chain and FX EDIT parity, preset switching, wet/dry, bypass, order/routing, mono/phase/headroom/clipping and loudness-matched A/B.
- [REQUIRED] Amp model/Gain/Bass/Treble UI controls must affect a real tone stage; no decorative amp controls.

## 8. Presets, banks and Curator
- [SOURCE PRESENT / PARTIAL] Catalog contains 300 factory preset definitions (target split 150 MELODY + 150 PATTERN in the project specification). Definitions are not proof of playable, validated presets.
- [REQUIRED] Validate each preset for stable ID, mode, soundbank dependencies, root/key ranges, articulation maps, parameter compatibility, output/no clipping, loudness match, playable MIDI test and host test where appropriate.
- [REQUIRED] Keep separate counts: definitions / dependency-resolved / loadable / playable-tested / host-verified.
- [REQUIRED] User preset save/load, factory/user separation, import/export, stable IDs, migration/backward compatibility and recovery from corrupt state.
- [APPROVED] Curator is separate from ordinary browser, helps classify/recommend, but cannot restrict user libraries or silently delete/alter assets.
- [REQUIRED] Curator outcomes GOLD/KEEP/REJECT with reason and provenance; GOLD requires technical screening and human listening; copyright suspect blocks commit.
- [SEPARATE SCOPE] User-requested 30-preset Curator set inspired by Blink-182 “ONE MORE TIME...” must be tracked separately from 300 factory definitions and validated for originality/licensing.

## 9. SoundBank, Import Guitar and Sound Hunter
- [APPROVED] One S6/S6.5 pipeline; Sound Hunter REV 10.1 and Import Workbench Stage 2 evolve that pipeline, not a parallel one.
- [PARTIAL] A policy validator, empty versioned registry and 12 unit cases were historically added on another revision; this is tooling/policy foundation only, not production search or an approved sound library.
- [REQUIRED] Per asset record: exact source URL, archive/version, author, license identifier and saved license evidence, allowed commercial use/modification/redistribution/embed scope, required notices/attribution, SHA-256, file format/rate/channels/size, technical screening, guitar-only classification, human listening and disposition.
- [REQUIRED] GREEN only if every legal/technical gate passes; AUTO ADD only for confirmed CC0/PD or equivalent explicit permission meeting project policy. YELLOW manual-only/quarantine; RED excluded. Unknown license or unclear redistribution => quarantine.
- [REQUIRED] No sample/library may be bundled because its product/site name sounds permissive. Check actual raw-sample redistribution rights, not just use in a musical work.
- [REQUIRED] Sound Hunter search/network/decode/analysis occurs off audio thread; no fake search results.
- [REQUIRED] Candidate targets MELODY 30 / PATTERN 50 / PUNK GROUP 40 are targets, not attained counts.
- [REQUIRED] Check Meta Open Sound Steel Guitar and Nylon Guitar exact licenses and redistribution rights before inclusion; no automatic approval by mention.

## 10. Settings, additional windows and product quality
- [REQUIRED] General/Audio/MIDI/GUI settings; MIDI Learn; tooltips; preset autosave; graphics quality; UI scale; reset; correct persistence.
- [REQUIRED] Do not expose fake host buffer-size controls; host owns buffer size.
- [REQUIRED] Additional windows: independent open/close/maximize/restore, safe lifetime, z-order/focus, saved bounds, DPI/work area, compact/portrait/overflow; theme propagation and no visual artifacts.
- [REQUIRED] Auto Discover internal ON/OFF and explicit opt-in; no external upload unless user explicitly enables it.
- [REQUIRED] About/help, status/errors and diagnostics must be truthful and actionable.

## 11. Performance and release acceptance

### 11.1 Performance targets (targets, not measured results)
- First sound ≤50 ms.
- Preset switch ≤300 ms p95.
- Skin swap approximately ≤150 ms p95.
- CPU ≤2% at 4 voices / ≤15% at 64 voices.
- RAM ≤60 MB idle / ≤512 MB full bank.
- Audio callback peak ≤60% of buffer time at 128 samples.
All targets require reproducible methodology, machine/host/build metadata, warm/cold conditions and exact SHA.

### 11.2 Release gates
For one exact candidate SHA:
1. Windows x64 Release configure/build all production/test targets.
2. Full CTest suite; record every test and result.
3. VST3 bundle discovery and package structure audit.
4. pluginval strictness 5.
5. Package contents, licenses/notices, manifests and checksums; no non-redistributable samples.
6. Install/scan/load/MIDI/audio/state/preset/skin workflows in FL Studio 20, separately labeled HOST VERIFIED.
7. CPU/RAM/latency/audio-quality/stress report.
8. Exact SHA, CI run IDs, artifact IDs, SHA-256, logs and known limitations recorded in Info/CODE/Control Center.
Historical green builds never certify later SHAs.

## 12. Work already completed or present — evidence-qualified inventory

| Area | Evidence-backed state | What is actually present / what is not implied |
|---|---|---|
| VST3/JUCE project | SOURCE PRESENT | Production project and VST3 target exist. Current canonical HEAD build not yet confirmed. |
| ThemeManager | SOURCE PRESENT / PARTIAL | 21-token palette API and legacy mapping recorded; verify exact current source/tests and concurrent paint safety. |
| Skin manifest validation | SOURCE PRESENT / PARTIAL | 28-property exact control contract and validator hardening recorded in branch history; confirm live canonical state and exact tests. |
| Theme registry | SOURCE PRESENT / PARTIAL | 102 themes recorded by later audit; not proof of all assets/manifests or canonical identity migration. |
| Main UI | PARTIAL | Existing editor/panels and palette propagation exist; full NEON STAGE approved mockup parity/control mapping not accepted end-to-end. |
| Gallery | PARTIAL | Cards/search/family/count/active marker; immediate commit on click; transactional Preview/Apply/Cancel etc missing. |
| Pattern scheduler | SOURCE PRESENT / INTEGRATED | Existing PPQ/quantize/swing/eight masks/chord transition/sampler event path; exact-SHA CI integration test passed at historical SHA. |
| Pattern section workflow | REQUIRED / NOT INTEGRATED | Model foundation exists on separate work branch commit `7366fca...`; not canonical Processor/Editor/state integrated; test execution unverified. |
| AutoChords | PARTIAL / SETUP ONLY | Setup panel; MIDI generation/playback not connected. |
| PUNK GROUP | REQUIRED / AUDIT REQUIRED | Completeness and playable content unconfirmed. |
| Sampler | SOURCE PRESENT / PARTIAL | SamplePool/SamplerEngine/SFZ/WAV/streaming/cache; lifecycle, roots, cancellation, worker failure and 64-voice stress gaps remain. |
| FX/DSP | SOURCE PRESENT / PARTIAL | DSP components exist; full product routing, UI binding and host acceptance not confirmed. |
| Factory presets | SOURCE PRESENT / PARTIAL | 300 definitions recorded; playable validated count not certified. |
| Sound Hunter | DESIGNED / POLICY FOUNDATION | Empty registry and validator history; no production deep search/autofill, no approved distributable candidates confirmed. |
| OverlayManager | BLOCKED | Production module absent from live REV.17 audit; old ZIP prototype defective. |
| SecureSkinImport | BLOCKED | Production module absent from live REV.17 audit; ZIP prototype security bypasses; implement and test safely. |
| Build/CI | HISTORICAL PASS ONLY | Full pipeline passed on SHA `8c75820...`; not on current HEAD `51c68e377ee5905e40684299348a0a4b56acac11`. |
| FL Studio 20 | USER-REPORTED SMOKE TEST | User reported plugin loaded/operated; exact installed binary SHA and per-feature acceptance not linked. |
| Performance | NOT MEASURED | Targets exist; current exact-SHA benchmark report absent. |
| Release | NOT RELEASED | Historical package exists; current canonical SHA not certified/released. |

## 13. Ordered execution roadmap

The order below is the current approved order from Info.txt section K, expanded into dependency-aware work packages. Do not substitute an older global ordering. Re-evaluate after each closeout only by a recorded decision.

### Phase 0 — Governance and source reconciliation
- T0 preflight current canonical HEAD, active branch and relevant branches.
- Re-read baseline, Control Center, Info/CODE, NEON STAGE reference, relevant source/spec/tests.
- Build branch/path matrix; reconcile relevant candidates without wholesale merge.
- Build implementation matrix and mark every GUI/control/feature state.
- Close only with source-backed status and synchronized journals.

### Phase 1 — UI identity and NEON STAGE parity
- Audit SKIN-1..3 names/default and sequential ID policy against registry/manifests/assets/gallery.
- Prepare non-destructive migration plan for historical naming conflicts; preserve IDs/resources.
- Compare “01 INTERFACE — NEON STAGE DEFAULT” component by component against live editor.
- Map every visible control to actual callback/APVTS/DSP/persistence/automation/test.
- Complete main and auxiliary windows without altering approved geometry or audio semantics.

### Phase 2 — Theme infrastructure and Gallery
- Verify 21-token/28-property contracts and legacy compatibility.
- Implement immutable palette snapshot / lock-free paint access and concurrent tests.
- Add gallery state model; draft-only Preview, Apply, Back/Cancel, Favorites, enlarged preview, keyboard navigation.
- Verify persistence, reset/fallback, DPI, memory, hot-swap and audio invariance.

### Phase 3 — PATTERN section-first integration
- Review/reconcile PatternSectionModel from work branch at exact SHA; compare API and tests to current canonical scheduler/state.
- Integrate model into Processor, Editor, project/preset state serialization and scheduler without breaking global legacy patternSlot.
- Add separate rhythm catalog and Section → Pattern UI.
- Add non-committing Preview / Apply / Cancel.
- Test old state migration, persistence, MIDI playback, PPQ/host transport, transitions, no clicks/stuck notes, no callback allocation/locks.

### Phase 4 — AUTOCHORDS and PUNK GROUP
- Specify exact MIDI generator contract and integration points before implementation.
- Implement AutoChords only with tests for chord/key/scale/progression/rhythm/preview/transport/state.
- Audit PUNK GROUP code, UI, soundbank and presets; list exact missing parts; implement without removing MELODY/PATTERN.

### Phase 5 — Audio/sampler/articulation/FX/presets
- Audit SamplePool lifetime, 8-root limit, snapshot cap, cancellation, queue drain, worker failure/recovery, processor destruction, 64 voices under cache pressure.
- Add missing lifecycle/stress tests before ownership changes.
- Verify velocity/palm-mute/acoustic/articulation mappings against actual samples.
- Audit all FX DSP-to-UI and amp/tone routing; complete controls and regression tests.
- Separate 300 definitions from validated playable preset inventory; resolve dependencies and test all presets; track Curator 30 separately.
- Implement Voices/polyphony and bounded multiprocessing; benchmark resource impact.

### Phase 6 — Licensing, Import Guitar and Sound Hunter
- Verify each sample/library's exact raw redistribution/embed rights, provenance, notices and SHA-256.
- Keep unknown assets quarantined; maintain GREEN/YELLOW/RED per asset.
- Extend existing S6/S6.5 pipeline only; build production discovery only after legal and lifecycle gates.
- Validate import preview/load/cancel and sample audition isolation.

### Phase 7 — O1 OverlayManager and S1 SecureSkinImport
- Design against live JUCE/editor lifecycle; do not copy faulty ZIP sources.
- Implement safe overlay open/close/z-order/focus/drag/resize/bounds/lifetime.
- Implement secure package import and SVG/ZIP/path validation.
- Run adversarial security and lifecycle tests.

### Phase 8 — Performance, QA and release
- Stabilize build and ThemeManager first; benchmark/allocation audit; optimize from measured bottlenecks.
- Run stress and exact-SHA Windows Release + full CTest + pluginval strictness 5 + package audit.
- Perform FL Studio 20 install/scan/load/playback/state/preset/skin checks on the exact package.
- Record CPU/RAM/latency/audio-quality and artifact checksums.
- Release only after all required gates pass; unresolved items remain explicit.

## 14. Mandatory transaction procedure for every work item

1. **START record:** unique TX ID, UTC timestamp, request, decision IDs, current branch/HEAD, scope, dependencies, files, risks, acceptance tests, rollback.
2. **Preflight:** inspect latest canonical and relevant work branch refs; compare divergence/diff; inspect exact source/spec/tests; no implementation from chat memory alone.
3. **Proposal:** assistant-generated scope changes are PROPOSED until user approval. Approved retrospective decisions D-RETRO-001..015 apply only within documented boundaries; D-RETRO-016..019 remain limited/superseded. Future suggestions are not blanket-approved.
4. **Implement one scoped item:** no unrelated mixing; preserve invariants.
5. **Verify:** exact SHA; actual build/test logs; host tests separate; failures and unrun gates explicitly recorded.
6. **Journal:** append matching transaction summary to Info.txt and CODE.txt; update this register and Control Center ledgers/statuses. Production change is not closed if either journal is missing.
7. **Remote readback:** fetch every changed file/ref and verify commit/blob SHAs. If mismatch, mark JOURNAL INCOMPLETE and stop dependent work.
8. **Closeout:** status, exact changed paths/symbols, results, blockers, next TX ID and handoff. Keep all historical records.

## 15. Three-document synchronization contract

These three documents must be maintained together for the life of the project:
1. **Info.txt — project history and status journal:** decisions, user requirements, proposals/approvals, task roadmap, audits, completed work, failures, defects, tests, CI, package/release evidence and current status.
2. **CODE.txt — implementation/change journal:** every code/test/build/config/docs transaction with branch, parent/result SHA, exact paths/symbols, rationale, invariants, tests and outcomes. For docs-only transactions explicitly say no production source changed.
3. **PPGU_MASTER_IMPLEMENTATION_REGISTER_RU.md — complete requirements and execution register:** canonical checklist of all requirements, status inventory, missing work, sequence, acceptance criteria and non-negotiable constraints. Link subsystem specs rather than duplicating their detailed implementation API.
4. **PPGU_DEVELOPMENT_CONTROL_CENTER.md — process/index authority:** branch, decision, task, transaction, GUI, asset/license, preset, test/release and risk ledgers; it indexes the three documents above. It must remain synchronized too.

Append-only rule: never silently rewrite history. Corrections and superseding decisions are new dated entries referencing prior IDs and evidence. Update the current status matrix only with explicit change notes.

## 16. Real-time and lifelong journal rule — capability boundary

- During every active PPGU repository work session, maintain the four documents synchronously: record task start before work; record every discrete completed step, proposal/approval, commit/push, test and closeout before moving to the next dependent step.
- At the start of each later session, reload all four documents and current branch refs; reconcile any unseen commits/changes before work.
- Do not claim an always-running background watcher, daemon or continuous monitoring between sessions. This chat workflow cannot autonomously observe repository changes while no active session/tool execution is running. True between-session monitoring would require a separately configured authorized GitHub Actions/automation service; it is not active as part of this transaction.
- “Lifelong” means append-only project governance for the entire project lifetime, not a promise that ChatGPT executes in the background when inactive.
- If a future session cannot access GitHub or the journals, explicitly mark sync BLOCKED and do not pretend the documents were updated.

## 17. Current open-work checklist

- [ ] Current HEAD exact-SHA build/test/pluginval/package verification.
- [ ] Complete UI/control inventory and NEON STAGE reference parity.
- [ ] Skin identity migration audit SKIN-1..3 and SKIN-4 onward.
- [ ] ThemeManager 21-token/28-property and immutable palette concurrency proof.
- [ ] Gallery Preview/Apply/Back/Favorites/enlarged preview/keyboard/persistence.
- [ ] PatternSectionModel selective reconciliation and full Processor/Editor/state/scheduler integration.
- [ ] Separate rhythm-pattern catalog and section assignment UI.
- [ ] AutoChords MIDI generation.
- [ ] PUNK GROUP completeness audit and implementation.
- [ ] Sampler lifecycle/root cap/cancellation/worker recovery/64-voice cache stress.
- [ ] Palm-mute/velocity/acoustic/articulation maps verified against real samples.
- [ ] FX chain, FX EDIT and AMP/TONE full DSP/UI/persistence/automation parity.
- [ ] 300 factory definitions validated into honest playable counts; Curator 30 separate.
- [ ] Voices/polyphony and bounded multiprocessing.
- [ ] Asset-level license/provenance/redistribution audit; approved assets inventory.
- [ ] Sound Hunter production pipeline inside S6 only.
- [ ] Import Guitar end-to-end workflow and audition isolation.
- [ ] Production OverlayManager and lifecycle tests.
- [ ] Production SecureSkinImport and adversarial security tests.
- [ ] Settings/additional-window/accessibility/DPI/state tests.
- [ ] Reproducible CPU/RAM/latency/audio quality/stress reports.
- [ ] Exact-SHA FL Studio 20 host acceptance and release package.

**Closing rule:** no checkbox is marked complete without its evidence. New requirements are appended, never silently dropped. This register is the project's working map, not a substitute for source code, tests, licenses or host acceptance.


## TX-MASTER-REGISTER-001 — CLOSEOUT / REMOTE READBACK
- Documentation commit: `ad0a1a9e328e04d8c79e3167889e80d8e352880b`.
- Parent: `51c68e377ee5905e40684299348a0a4b56acac11`.
- Canonical branch: `POP-PUNK-GUITAR-ULTIMATE-VST-REV.17`.
- Verified remote HEAD equals documentation commit.
- Verified remote blob SHAs:
  - `docs/PPGU_MASTER_IMPLEMENTATION_REGISTER_RU.md`: `1ff17aafb9e0d016b7683096f81c4af14dee7343`
  - `Info.txt`: `66bd39412ac23aa900cf19a76206c1ce339e3ac8`
  - `CODE.txt`: `462335838b9a51652afc9a8be610bd37a093515a`
  - `docs/PPGU_DEVELOPMENT_CONTROL_CENTER.md`: `1f67af814cfc6ba33a1b98e693d588264172e4e0`
- All four remote files were fetched at the resulting commit; content presence and blob identities verified.
- Tests/build/CI/host validation: NOT RUN (documentation-only transaction).
- Production source changed: NO.
- Status: DOCUMENTATION TRANSACTION VERIFIED / CLOSED.
