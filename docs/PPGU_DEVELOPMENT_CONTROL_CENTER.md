
# PPGU DEVELOPMENT CONTROL CENTER
## Permanent session, branch, decision, transaction and evidence protocol
**Protocol revision:** 1.0 — 2026-11-14  
**Canonical development line:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV.17`  
**Ratified product baseline:** SIGNAL FORGE v10 MASTER/v10.1 + user-ratified immutable conversation-wide register.  
**Purpose:** prevent loss of decisions, approved proposals, implementation steps, branch-specific work, commits, pushes, tests and release evidence.

## 1. Binding operating rule
This protocol is mandatory for every future PPGU development task, including source code, GUI/skins, DSP, sampler, patterns, presets, sound libraries, build system, CI, packaging, documentation and project-management changes. It supplements (does not replace) append-only `Info.txt` and `CODE.txt` and the user-ratified immutable baseline.

No task may start from conversational recollection alone. Before work, reload the control center, ratified baseline, current task register, both journals, current branch/HEAD, and relevant source/spec/test files. After each discrete completed item and each repository write/push, record exact evidence and read back the remote result.

## 2. Canonical branch and branch preservation
- `POP-PUNK-GUITAR-ULTIMATE-VST-REV.17` is the canonical code/integration line unless the user explicitly changes that rule.
- Work branches are allowed for isolated experiments, but must declare their base SHA and purpose. No branch is merged wholesale merely because it contains useful work.
- Before every change, fetch canonical REV.17 HEAD and active working branch HEAD; compare ancestry/divergence and changed paths. Use expected-SHA/non-force updates. If the canonical line advanced, stop and reconcile before coding.
- Review alternate branches selectively by relevance and diff. Preserve useful commits as candidates with branch + exact SHA + path inventory + dependencies + test evidence. Never silently discard or blindly cherry-pick/merge divergent work.
- A branch inventory is a snapshot, not proof that every branch's files were audited. Refresh it at each task preflight. Branches not reviewed for the current task remain UNREVIEWED, not approved or merged.
- Do not mass-edit every historical branch just to copy journals: that can rewrite stale histories and create conflicting authorities. The canonical control center is the authoritative pointer; branch-specific changes must be reconciled into REV.17 through reviewed, scoped transactions.

## 3. Mandatory per-task transaction lifecycle
Every discrete task receives a unique transaction ID and follows these gates:

### T0 — PRE-FLIGHT (before implementation)
Record:
- transaction ID, date/time, task title and user request;
- approval status and exact source decision/requirement IDs;
- canonical branch/HEAD and active branch/HEAD, parent SHA, merge-base and ahead/behind/divergence;
- relevant alternate branches/commits examined, ignored/deferred branches and reason;
- files/specifications/tests inspected; current implementation state;
- scope, invariants, risks, dependencies, acceptance criteria and rollback plan;
- expected output and exact validation plan.
No implementation begins while branch/HEAD, requirements or relevant source state is ambiguous.

### T1 — PROPOSAL / APPROVAL
Every assistant-originated suggestion, variant, architecture choice or scope change is entered into the Decision Register as PROPOSED with unique ID, rationale, impact, alternatives and affected files/IDs.
Only explicit user approval moves it to APPROVED. Rejection becomes REJECTED; superseded proposals remain in history. Silence, a general “continue”, or approval of a different item does not approve a proposal. User-originated explicit requirements are recorded as USER-REQUIRED / RATIFIED without waiting for another approval.
No unapproved proposal may silently change the immutable baseline.

### T2 — IMPLEMENTATION
Make one logically scoped change per transaction where practical. Preserve APVTS IDs/semantics, project/session/preset compatibility, MIDI/host timing, sampler lifetime and hard realtime guarantees. Do not mix unrelated fixes.
For each discrete source/documentation milestone, record exact paths, symbols, behavior, migration and status. If work spans multiple commits, each commit/push gets its own record.

### T3 — POST-CHANGE VERIFICATION
Run only checks actually available. Record exact commit SHA tested, commands/workflow IDs, platform/toolchain, results, logs/artifacts, failures, unrun checks and known risks. A test registered is not a test passed. A documentation commit is not implementation. User-reported host testing is labeled separately from exact-SHA host verification.
Read back every remotely written file and ref; compare expected blob/commit SHAs. Any mismatch => JOURNAL INCOMPLETE and stop further dependent work.

### T4 — CLOSEOUT
Append to both `Info.txt` and `CODE.txt` in the same production transaction:
- transaction ID and phase (START / UPDATE / COMPLETE / BLOCKED);
- branch, parent SHA, resulting commit SHA;
- exact changed paths and symbols;
- decision IDs implemented;
- tests/CI/artifact/host results tied to exact SHA;
- risks, deferred items, next transaction ID.
Update the Control Center status matrix and branch/candidate ledger. Close an item only when its acceptance criteria and required evidence are satisfied.

## 4. Live ledgers — never collapse distinct records
Maintain these append-only registers in the Control Center:
1. **Requirements Register:** immutable user requirements and source references.
2. **Decision Register:** every assistant proposal, user approval/rejection, chosen variant, and superseding amendment.
3. **Task Register:** all work packages, dependencies, priority, owner, status and acceptance criteria.
4. **Transaction Ledger:** every PRE-FLIGHT, implementation milestone, commit/push, verification and closeout.
5. **Branch & Commit Ledger:** every relevant branch, observed HEAD, base/merge-base, divergence, candidate commits, review and integration disposition.
6. **Control/GUI Inventory:** every visible control mapped to real action/parameter, persistence, automation/MIDI Learn, enabled state and test.
7. **Asset/License Ledger:** exact source, version, author, license evidence, redistribution scope, notices, SHA-256 and validation.
8. **Preset Ledger:** definition ID, MELODY/PATTERN, soundbank dependencies, playable validation and status.
9. **Test/Release Evidence:** exact SHA, CI run, CTest, pluginval, package checksum, host validation and benchmark.
10. **Risk/Defect Register:** discovered issue, severity, affected paths/branches, workaround, owner and closure proof.

## 5. Status vocabulary
Use only evidence-backed states:
- Requirements: USER-REQUIRED, RATIFIED, PROPOSED, APPROVED, REJECTED, SUPERSEDED.
- Engineering: REQUIRED, DESIGNED, BLOCKED, DEFERRED, SOURCE PRESENT, PARTIAL, INTEGRATED, TESTED, HOST VERIFIED, RELEASED.
- Transactions: NOT STARTED, PREFLIGHT COMPLETE, IN PROGRESS, BLOCKED, VERIFIED, CLOSED.
- Branch disposition: CANONICAL, ACTIVE WORK, CANDIDATE, REVIEWED, CHERRY-PICK APPROVED, MERGED, HISTORICAL, UNREVIEWED, REJECTED.
Never promote status without evidence.

## 6. Branch snapshot at protocol adoption
The GitHub repository branch search returned the following names on 2026-11-14. This is an inventory snapshot only; most branches have not been file-by-file reviewed in this transaction:

```
5.0-audio-integration
5.0-engineering-upgrade
5.0-import-guitar-library-user-presets
5.0-integrated-catalog-samples
5.0-sample-bank
5.0-sample-bank-final
5.0-track-catalog
5.0-track-clean
5.0
File
POP-PUNK-GUITAR-ULTIMATE-VST-REV.17
POP-PUNK-GUITAR-ULTIMATE-VST-REV-17
audit/engine-ui-presets-import-sounds
audit/skin-conflict-resolution-2026-11-13
ci/speed-up-validation-and-packaging
feat/pattern-auto-chords-scale-mode
feat/power-profiles-double-multicore
feat/theme-folder-system
feat/theme-system-20-full-skins
feature/auxiliary-panel-layout
feature/m2-velocity-crossfade-gain
feature/m2-velocity-palm-mute
feature/m4-autochords-foundation
feature/m5-style-engine-foundation
feature/pattern-melody-fx-spec
feature/pop-punk-ultimate-skin-ui
feature/ppgu-5-import-sounds
feature/ppgu-5-premium-shell-skins
fix/m1-m2-m3-release-gates
fix/m2-streaming-runtime-next
fix/page-cache-publish-rollback
integration/ppgu-5-production-audit
integration/rev6-5.0-catalog
main
migration/full-copy-m3-transport-scheduler-v2
perf/m2-hermite-page-acquire-fastpath
release/skin-packs-10
rev9-sb1-bank-scanner
review/full-work-text
test/m2-crossfade-rendered-signal
test/m2-page-cache-lease-pinning
ui/modern-neon-stage-shell
work/integrate-uploaded-theme-manager
work/m2-velocity-crossfade
work/m4-autochords-fingering
work/m5-style-engine-tests
work/m6-m13-quickwins
work/neon-gallery-16-skins
work/pattern-trigger-layout-c0-c1
work/pattern-trigger-map-rev17
work/ppgu-completion-audit-2026-11-13
work/ppgu-completion-integrated-2026-11-13
work/ppgu-skin-expansion-53-102
work/ppgu-toolchain-qa-rev17-1
work/rev17-full-reconciliation-2026-11-14
work/rev17-sampler-root-cap-regression
work/signal-forge-consolidation
work/signal-forge-panel-shell
work/signal-forge-theme-tokens
work/signal-forge-ui-integration
work/skin052-runtime-integration
work/visual-shell-spec-rev17-1
work/5.0-twenty-skin-palettes
```

### Known cross-branch fact at adoption
- Canonical `POP-PUNK-GUITAR-ULTIMATE-VST-REV.17` HEAD observed: `d5c811233725925ea32f10815c9efae6eda2e017`.
- Work branch `work/rev17-full-reconciliation-2026-11-14` HEAD observed: `ff5c8da1ac6f9dd3d6fb0467a2a037b284294a29`.
- GitHub compare reports divergence from merge base `f135b0f5c9609286086772450f69d287d8ff71c5`: work branch is 5 commits ahead and 2 behind canonical. This branch contains PATTERN model and user-ratification journal updates; canonical contains later requirements-consolidation work. **Do not merge either branch wholesale.** Reconcile the two canonical histories and selectively port the missing approved records/code only after path-level review.
- `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17` (hyphen before 17) is a separate branch name from `...REV.17`; never conflate them.

## 7. Always-on operating behavior in this ChatGPT workflow
At the start of every PPGU work request, perform T0 before editing. Before each discrete implementation item, log its start. After each completed item, commit/push the result and append/read back the transaction record before moving on. Every assistant proposal is explicitly marked PROPOSED; after user approval, mark APPROVED and carry the decision ID into implementation. Every user-approved correction is recorded as RATIFIED. No “continue” response is treated as blanket approval of new scope.
At handoff, state exact current branch/HEAD, last completed transaction, current blockers, and next transaction. Resume from these ledgers, not from chat recency alone.

**Capability boundary:** this protocol can be executed whenever the assistant is actively working with the repository in a conversation. It does not create an always-running GitHub daemon and cannot observe commits or branch changes between sessions unless an authorized scheduled automation/connected execution environment is separately configured. Never claim background real-time monitoring that is not actually running.

## 8. Current adoption checkpoint
- Protocol document: newly established in canonical REV.17.
- User-ratified product baseline: established in the parallel reconciliation branch; must be reconciled into canonical REV.17 without overwriting canonical's newer work.
- Current canonical HEAD at preflight: `d5c811233725925ea32f10815c9efae6eda2e017`.
- Current work-branch HEAD at preflight: `ff5c8da1ac6f9dd3d6fb0467a2a037b284294a29`.
- Cross-branch merge: BLOCKED pending selective reconciliation.
- Production implementation from this protocol adoption: none.
- CI/tests for this documentation change: not run unless exact evidence is appended below.

## 9. Append-only transaction entries
New entries go below. Do not rewrite earlier records.

### TX-GOV-001 — 2026-11-14 — ADOPTION
**Status:** PREFLIGHT COMPLETE / GOVERNANCE DOCUMENTATION IN PROGRESS.  
**Canonical base:** `d5c811233725925ea32f10815c9efae6eda2e017`.  
**Active working branch observed:** `work/rev17-full-reconciliation-2026-11-14` @ `ff5c8da1ac6f9dd3d6fb0467a2a037b284294a29`.  
**Scope:** establish preflight, approval, branch comparison, transaction, journal, verification and handoff controls.  
**No source implementation changed.**

## 10. Decision Register — retrospective approval of assistant proposals (2026-11-12 to 2026-11-14)

**User instruction, 2026-11-14:** assistant proposals made during the last two days are to be treated as approved unless they conflict with a newer user decision or a newer approved project record. This is explicit approval of compatible proposals, not approval of contradictory/superseded details. Approval applies to the proposal's compatible scope; it does not claim implementation.

### Approved proposals (decision status APPROVED)

| Decision ID | Proposal / accepted direction | Origin / evidence | Boundaries and implementation gate |
|---|---|---|---|
| D-RETRO-001 | Preserve approved main visual shell geometry, proportions, typography, color treatment, decorative details and functionality; skins may change appearance but not layout/audio engine; use the approved window mockups as visual references. | Assistant proposal 2026-11-12; user approval already recorded. | Subject to later, more specific approved visual corrections and the complete NEON STAGE default UI specification. Mockups are not proof of runtime behavior. |
| D-RETRO-002 | Use REV.17 as canonical integration target; inspect other branches as sources and reconcile selectively; do not restart or overwrite current work. | Assistant proposal 2026-11-13; user repeatedly instructed to continue from existing work and preserve approved changes. | Expanded by this Control Center: inspect all relevant branches, not only `main`; no wholesale merge. Current canonical branch is the dot-name `POP-PUNK-GUITAR-ULTIMATE-VST-REV.17`. |
| D-RETRO-003 | Extend existing SamplePool/SFZ/streaming architecture rather than creating a parallel sampler; preserve live sampler contracts. | Assistant proposal 2026-11-13. | Approved only as an architectural constraint after source/API preflight; preserve sampler lifetime, cache/voice bounds and realtime rules. |
| D-RETRO-004 | Continue Sound Hunter REV 10.1 and Import Workbench Stage 2 using append-only evolution. | Assistant proposal 2026-11-13. | Sound Hunter must remain within the newer single S6 asset pipeline, exact per-asset provenance/license/redistribution checks and quarantine gate. No candidate becomes distributable by proposal approval. |
| D-RETRO-005 | Preserve documentation roles: Info.txt as project history/decisions/status/evidence; CODE.txt as code/transaction chronology; subsystem specifications, tests and workflows in dedicated docs/tests/workflows. | Assistant proposal 2026-11-13. | Must not create competing sources of truth; central Control Center indexes the ledgers. Info/CODE append-only and updated in the same production transaction. |
| D-RETRO-006 | Transfer and review the full-work historical text in an isolated review context, compare it against Info/CODE, and do not disturb the active development branch during review. | Assistant proposal 2026-11-13. | Approved as a review method; the historical text is evidence/input, not authority over newer user decisions or current source. |
| D-RETRO-007 | Stabilize Windows build and ThemeManager first, then establish performance benchmark/allocation audit, optimize against measurements, stress-test and verify FL Studio 20, then expand features. | Assistant performance proposal 2026-11-12; user separately accepted measurable performance/CI requirements. | Approved as the performance-workstream sequence, not as permission to postpone mandatory product requirements indefinitely. Performance targets remain targets until measured on exact SHA. |
| D-RETRO-008 | Use immutable Palette Snapshot / lock-free paint access and concurrent theme tests while preserving the 21-token / 28-property contract and legacy skins. | Assistant proposal 2026-11-12. | Approved design direction subject to live source/API audit, thread-safety proof and regression tests; no implementation is implied. |
| D-RETRO-009 | Follow the engineering loop “CHECK → ANALYZE → FIX → RE-ANALYZE → RESULT”, one logical cause per commit, synchronized Info/CODE, and no assumed/fabricated results. | Assistant proposal 2026-11-12; user acceptance recorded in prior context. | Remains mandatory; this Control Center adds per-task T0–T4 and readback gates. |
| D-RETRO-010 | PATTERN Section → Rhythm/Pattern: distinct Intro/Verse/Chorus/Bridge/Outro model, independent section mapping, separate rhythm catalog, non-committing Preview/Apply/Cancel, backward-compatible persistence and MIDI/transport tests. | Assistant proposal 2026-11-14; user now explicitly approves compatible proposals from the last two days. | Approved functional contract. The implementation order is governed by newer Info.txt section K and current user direction; do not automatically label it global P1. PatternSectionModel on the work branch is foundation only until integrated and tested. |
| D-RETRO-011 | Reimplement/adapt production OverlayManager and SecureSkinImport safely rather than transplanting the defective ZIP prototype; add lifecycle/security regression tests. | Assistant proposal 2026-11-14; approved by current retrospective instruction. | Preserve findings as ZIP-only; live REV.17 lacks production modules. Fix mixed-case SVG event and protocol-relative URI bypasses in any adopted implementation. |
| D-RETRO-012 | Skin Gallery transaction flow: draft-only Preview, explicit Apply, Back/Cancel rollback, Favorites and enlarged preview, with state tests. | Assistant proposal 2026-11-14; approved by current retrospective instruction. | Preserve current theme IDs and ensure skin changes do not touch APVTS/DSP/MIDI/sampler state. |
| D-RETRO-013 | Sound Hunter asset-level audit and legal gate: exact source/version/author/license, redistribution scope, notices, SHA-256, technical inspection and guitar-only verification; quarantine uncertain assets. | Assistant proposal 2026-11-14; approved by current retrospective instruction. | This is a gate, not approval of any particular sample library. Registry remains empty until evidence is recorded. |
| D-RETRO-014 | Component-by-component GUI/control audit; map visible controls to real callbacks/parameters, persistence, MIDI Learn/automation and tests; no fake controls. | Assistant proposal and user-approved control inventory discussions during 2026-11-12/13. | Newer approved complete NEON STAGE UI specification controls visual details; never simplify or redesign without explicit approval. |
| D-RETRO-015 | Complete full Windows x64 Release / CTest / pluginval strictness 5 / package audit / FL Studio 20 validation on one exact SHA, with CPU/RAM/audio measurements recorded separately. | Assistant proposal 2026-11-12/14 and ratified release gates. | Historical green SHA does not certify current HEAD; each result must identify exact SHA and evidence. |

### Partially approved / superseded details

| Decision ID | Earlier proposal | Current disposition |
|---|---|---|
| D-RETRO-016 | Global implementation order `PRESETS → EFFECTS → PATTERN → MELODY → ADDITIONAL FUNCTIONS` (assistant proposal 2026-11-13). | **SUPERSEDED as a global priority order** by the newer consolidated Info.txt section K and subsequent ratified requirements. The listed subsystems remain in scope; only this old sequence is not approved. |
| D-RETRO-017 | Earlier assistant shorthand that only `main` should be treated as a source for additional work. | **SUPERSEDED / broadened**: all relevant repository branches must be inventoried and selectively reviewed; no branch is automatically authoritative just because it is `main`. REV.17 remains canonical. |
| D-RETRO-018 | Earlier proposed priority “PATTERN P1, then O1/S1, Gallery, Sound Hunter”. | **PARTIALLY APPROVED**: the technical work packages D-RETRO-010..013 are approved, but the old priority ranking is superseded by the newer ordered reconciliation plan in Info.txt section K (UI/skin identity and full UI parity, Gallery, PATTERN, AutoChords, other audits, licensing, O1/S1). |
| D-RETRO-019 | Finish OverlayManager then proceed to “DREAMING IN NEON” (assistant proposal 2026-11-12). | OverlayManager lifecycle/security work is covered by D-RETRO-011. The label “DREAMING IN NEON” is retained as a historical proposal label only; its exact deliverable must be mapped to the newer approved NEON STAGE/SIGNAL FORGE UI register before implementation. No separate scope is inferred. |

### Approval rule for this retrospective set
The user has now approved all compatible proposals above through D-RETRO-015. D-RETRO-016..019 preserve the historical proposal while explicitly superseding or limiting the conflicting/ambiguous part. This retrospective approval does not change the immutable baseline, does not mark code as implemented, and does not override any newer user instruction. Any future assistant proposal is still PROPOSED until the user explicitly approves it.

## 11. Append-only transaction entries

### TX-DEC-002 — 2026-11-14 — RETROSPECTIVE PROPOSAL REVIEW
**Status:** VERIFIED / DECISION REGISTER UPDATED.  
**User authority:** explicit instruction to approve assistant proposals from the last two days unless they conflict with newer decisions.  
**Reviewed proposal sources:** conversation history available through 2026-11-12..2026-11-14, prior assistant proposals surfaced in conversation context, Info.txt and CODE.txt current canonical journals.  
**Outcome:** D-RETRO-001..015 APPROVED within stated boundaries; D-RETRO-016..019 PARTIALLY APPROVED / SUPERSEDED as detailed above.  
**Implementation:** none in this transaction; this is a governance/decision-record update only.


## 12. TX-MASTER-REGISTER-001 — 2026-11-14 — MASTER REQUIREMENTS REGISTER

The canonical detailed checklist is `docs/PPGU_MASTER_IMPLEMENTATION_REGISTER_RU.md`. It consolidates the user-ratified baseline, Info.txt sections A–K, CODE.txt evidence and decisions D-RETRO-001..019 into one ordered operational register.

**Current canonical preflight:** `51c68e377ee5905e40684299348a0a4b56acac11`.  
**Parallel reconciliation branch observed:** `work/rev17-full-reconciliation-2026-11-14` @ `ff5c8da1ac6f9dd3d6fb0467a2a037b284294a29`; do not merge wholesale.  
**Transaction scope:** documentation-only; no production source or test behavior changed.  
**Synchronized files:** Info.txt, CODE.txt, this Control Center and the new master register.

### Permanent synchronization rule
- During each active PPGU work session, update Info/CODE/master register/Control Center synchronously: task start before work, each discrete completed item and commit/push, each verification, each closeout.
- At every new session, reload all four documents and current branch refs before implementation.
- Never silently delete or downgrade requirements; corrections/superseding amendments are append-only and cite old IDs/evidence.
- No background watcher/daemon is active between sessions. The assistant cannot autonomously observe GitHub while no active session/tool execution is running. Do not claim continuous background monitoring.
- The term “lifelong” means append-only governance for the entire project lifecycle, not autonomous execution while inactive.

### TX-MASTER-REGISTER-001 closeout
**Status:** DOCUMENTATION COMMIT PENDING READBACK at entry creation. No production source changed. Exact resulting SHA and blob readback are appended after the GitHub transaction completes.


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
