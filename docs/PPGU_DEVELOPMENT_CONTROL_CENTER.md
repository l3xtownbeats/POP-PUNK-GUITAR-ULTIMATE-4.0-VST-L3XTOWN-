
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
