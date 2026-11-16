# PPGU — СВОДНЫЙ РЕЕСТР ЗАДАЧ (task registry, v1, 2026-11-13)

Формат: ID | Задача | Область | Статус | Приоритет | Зависимости.
Статусы: Implemented / Partial / Unverified / Not implemented / Blocked.
Приоритеты: P1 (критично) > P2 > P3. Дополняется по мере закрытия этапов.

## Область: Skin Engine (приоритет №1)

| ID | Задача | Статус | Приоритет | Зависимости |
|---|---|---|---|---|
| SE-01 | OverlayManager контракт (стек, z-order, focus, modal) | Implemented (VALIDATION PENDING) | P1 | — |
| SE-02 | SecureSkinImport .ppguskin (allowlist, SVG, SHA-256) | Implemented (VALIDATION PENDING) | P1 | — |
| SE-03 | G3.5 validator usage.json | Implemented | P2 | — |
| SE-04 | Интеграция OverlayManager в workspace | Not implemented | P1 | SE-01 |
| SE-05 | juce::ZipFile -> SecureSkinImport production-обвязка | Not implemented | P1 | SE-02 |
| SE-06 | Runtime skin hot-swap проверка | Unverified | P1 | SE-04, CI |
| SE-07 | DPI/ECO gates | Unverified | P2 | SE-04 |
| SE-08 | Skin Gallery runtime (полная) | Partial | P2 | SE-06 |

## Область: Инфраструктура ресурсов

| ID | Задача | Статус | Приоритет | Зависимости |
|---|---|---|---|---|
| IR-01 | ResourceRootResolver C++ module | Not implemented | P2 | spec MERGED |
| IR-02 | Marker parser/validation + env resolver | Not implemented | P2 | IR-01 |
| IR-03 | Root fallback UX | Not implemented | P3 | IR-02 |
| IR-04 | EXPLICIT-PERMISSION ingest workflow | Not implemented | P2 | IR-01 |

## Область: Sound Hunter / SoundBank

| ID | Задача | Статус | Приоритет | Зависимости |
|---|---|---|---|---|
| SH-01 | Fix 2 упавших sound registry CTest assertions | Not implemented | P1 | — |
| SH-02 | DeepSearchWorker | Not implemented | P2 | SH-01 |
| SH-03 | StyleClassifier | Not implemented | P2 | SH-02 |
| SH-04 | CuratorPanel | Not implemented | P2 | SH-02 |
| SH-05 | GREEN registry integration в S6.5 | Not implemented | P2 | SH-01..04 |
| SB-01 | guitar_ingest.py | Not implemented | P2 | — |
| SB-02 | LICENSES.txt generation | Not implemented | P2 | SB-01 |
| SB-03 | B1–B4 SoundBank pipeline | Not implemented | P3 | SB-02 |

## Область: Движок / продукты

| ID | Задача | Статус | Приоритет | Зависимости |
|---|---|---|---|---|
| EN-01 | Общий PPGU Audio Engine (Guitar/Bass) | DESIGNED | P2 | quiescence audit |
| EN-02 | Bass Edition | Not implemented | P3 | EN-01 |

## Область: Приёмка

| ID | Задача | Статус | Приоритет | Зависимости |
|---|---|---|---|---|
| QA-01 | Windows Release + CTest + pluginval на актуальный SHA | Unverified | P1 | следующий commit |
| QA-02 | FL Studio 20 host acceptance свежего SHA | Unverified | P1 | QA-01 |
| QA-03 | CPU/RAM profiling против целей | Unverified | P2 | QA-01 |
| QA-04 | Branch rollout (R33) всех недавних этапов | Not verified | P3 | QA-01 |

## Правила реестра

- Статус задачи меняется только записью в INFO (+CODE при изменении кода).
- Implemented без exact-SHA CI не продвигается дальше VALIDATION PENDING.
- Заблокированные задачи помечать Blocked с причиной, не удалять.
- Очередь следующего прогона: SE-04 -> SE-05 -> SH-01 -> QA-01/QA-02.


## REV.18 Factory readiness audit
| ID | Task | Status | Priority | Dependencies |
|---|---|---|---|---|
| FR-01 | Separate 500+500 READY evidence audit | Implemented (VALIDATION PENDING) | P1 | CI exact SHA |
| FR-02 | Populate legally cleared Sound Hunter asset registry | Not implemented | P1 | source/legal/audio review |
| FR-03 | Runtime-map factory MELODY sources | Not implemented | P1 | FR-02, sampler |
| FR-04 | Build PATTERN performance asset mappings | Not implemented | P1 | FR-02, performance engine |
| FR-05 | Human audio QA for each READY preset | Not implemented | P1 | FR-03/FR-04 |




## REV.18 UI / NEON STAGE Phase 0 — 2026-11-14

| ID | Task | Status | Priority | Dependencies |
|---|---|---|---|---|
| UI-00 | Requirements-to-live-source component matrix A–H | Implemented (AUDIT COMPLETE; docs only) | P1 | INFO K |
| UI-01 | Resolve exact canonical NEON STAGE reference and approved visual asset | Blocked (38 reference assets indexed; canonical image/approval mapping unresolved) | P1 | reference decision/asset evidence |
| UI-02 | Reconcile SKIN-1/2/3 identity, default/fallback, manifests and Gallery without ID loss | Blocked (live manifest/ThemeManager mismatch) | P1 | UI-01; 102-entry no-loss map |
| UI-03 | Complete main-window control parity and real callbacks/APVTS/state | Partial | P1 | UI-01/UI-02 |
| UI-04 | Gallery Preview/Apply/Back/Favorites/keyboard navigation | Partial | P1 | UI-02; testable selection state |
| UI-05 | Auxiliary OverlayManager lifecycle/focus/z-order integration | Not implemented | P1 | workspace API audit; lifecycle tests |

**REV.18 priority override:** follow INFO.txt section K (reference + skin identity → main UI parity → Gallery → PATTERN → AutoChords → subsystem audits → legal/S6 → O1/S1 → exact-SHA gates). This supersedes the older generic queue where it conflicts; no existing task is deleted.



## 2026-11-14 — TX-ENGINE-OPT-APPROVAL-001 — новые утверждённые инженерные направления

| ID | Задача | Статус | Приоритет | Зависимости |
|---|---|---|---|---|
| ENG-01 | AUDIO SIGNAL FLOW: SMART GAIN / PRESET LEVEL / DRY-WET / FX ROUTING / OUTPUT PROTECTION | APPROVED REQUIREMENT | P1 | source DSP audit |
| ENG-02 | VOICE MANAGEMENT: allocation / priority / stealing / reclamation / transitions | APPROVED REQUIREMENT | P1 | SamplerEngine lifecycle audit |
| ENG-03 | MIDI ENGINE robustness and articulation processing | APPROVED REQUIREMENT | P1 | MIDI source/API audit |
| ENG-04 | AUDIO QUALITY consistency and style acceptance | APPROVED REQUIREMENT | P1 | audio QA harness |
| ENG-05 | SAMPLE PLAYBACK quality and click-free transitions | APPROVED REQUIREMENT | P1 | sampler/streaming audit |
| ENG-06 | PARAMETER MANAGEMENT and host automation/state | APPROVED REQUIREMENT | P1 | APVTS audit |
| ENG-07 | PRESET SYSTEM integrity, compatibility and recovery | APPROVED REQUIREMENT | P1 | preset/catalog audit |
| ENG-08 | ERROR RECOVERY and safe degraded operation | APPROVED REQUIREMENT | P1 | lifecycle/error-path audit |
| ENG-09 | LATENCY CONTROL across buffer sizes/sample rates | APPROVED REQUIREMENT | P2 | measurement harness |
| ENG-10 | RELEASE SYSTEM and exact-SHA packaging gates | APPROVED REQUIREMENT | P1 | Windows CI/release audit |
| ENG-11 | REAL-TIME AUDIO SAFETY | APPROVED REQUIREMENT | P1 | callback/thread audit |
| ENG-12 | SAMPLE FIDELITY ENGINE | APPROVED REQUIREMENT | P1 | resampling/audio comparison |
| ENG-13 | INTELLIGENT MEMORY MANAGEMENT | APPROVED REQUIREMENT | P1 | allocation/lifetime profiling |
| ENG-14 | DSP QUALITY OPTIMIZATION | APPROVED REQUIREMENT | P1 | DSP tests/benchmarks |
| ENG-15 | SMART RESOURCE SCHEDULING | APPROVED REQUIREMENT | P2 | worker/queue audit |
| ENG-16 | PRESET INTELLIGENCE | APPROVED REQUIREMENT | P1 | preset/sample dependency audit |
| ENG-17 | AUDIO DIAGNOSTICS | APPROVED REQUIREMENT | P2 | low-overhead telemetry design |
| ENG-18 | AUDIO CONSISTENCY CONTROL | APPROVED REQUIREMENT | P1 | loudness/style acceptance |
| ENG-19 | HOST COMPATIBILITY | APPROVED REQUIREMENT | P1 | VST3/FL Studio host matrix |
| ENG-20 | AUTOMATED QUALITY TESTING | APPROVED REQUIREMENT | P1 | CTest/CI/host test strategy |

Все записи отражают утверждение требований, не реализацию. Следующий технический рабочий пакет — MIDI ENGINE после завершения согласованной последовательности и проверки актуального source state.


## 2026-11-16 — REV.19 single canonical workline + SIMPLE CORE implementation queue

**Only active line:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV.19-Build.03`  
**Decision-time source HEAD:** `1f58f25378992c0ddbfe5d0d38598a2034adb3e3`.

REV.19 is the sole place for active development, source integration, synchronized records, builds and tests. REV.18 is the prior approved baseline/history, not a parallel workline. REV.17 and earlier are archive only. Do not create competing development branches for the same work; consolidate accepted decisions in REV.19.

| ID | Task | Status | Priority | Dependencies |
|---|---|---|---|---|
| REV19-ARCH-01 | Keep one shared SamplerEngine/SamplePool/streaming/cache/voice core for MELODY and PATTERN | SOURCE PRESENT at recorded HEAD | P1 | ongoing regression tests |
| REV19-ARCH-02 | Establish reproducible REV.19 CPU/RAM/callback/dropout/cache baseline | NOT MEASURED | P1 | fixed Windows test profile |
| REV19-ARCH-03 | Extract MELODY orchestration into a small, clear component without changing note/MIDI behavior | NOT IMPLEMENTED at inspected HEAD | P1 | REV19-ARCH-02; MIDI/APVTS audit |
| REV19-ARCH-04 | Extract PATTERN orchestration into a small, clear component using the same shared sampler | NOT IMPLEMENTED at inspected HEAD | P1 | REV19-ARCH-02; PatternScheduler audit |
| REV19-ARCH-05 | Add isolation, mode-switch, note lifecycle, AUTOCHORDS routing and shared-sampler contract tests | OPEN | P1 | ARCH-03/04 |
| REV19-ARCH-06 | Optimize only profiled hotspots and compare measurements on identical conditions | OPEN | P2 | ARCH-05 |

Guardrails: no second sampler; no duplicate sample bank/cache; no UI/browser layout changes; no APVTS/control-ID changes; no AUTOCHORDS in MELODY; no claims of performance gain without comparable measurements. All tasks execute on REV.19.


## 2026-11-16 — TX-REV19-UNIFIED-PRESET-ENGINE-FAST-INDEX-NO-UI-CHANGE-001

| ID | Task | Status | Priority | Dependencies |
|---|---|---|---|---|
| PSET-06 | Preserve approved UI exactly; no visual redesign for preset organization | APPROVED / LOCKED | P1 | UI parity baseline |
| PSET-07 | Build reusable metadata index for the unified 500-preset source | NOT IMPLEMENTED | P1 | PSET-01 / PSET-02 |
| PSET-08 | Serve MELODY and PATTERN browser views by indexed mode filtering and stable IDs | NOT IMPLEMENTED | P1 | PSET-07 |
| PSET-09 | Avoid repeated filesystem scans; keep indexing off audio thread; measure lookup/switch latency | NOT IMPLEMENTED | P1 | PSET-07; performance harness |

Acceptance: unchanged approved UI; one source; exact 250+250; cached/indexed mode filtering; no full scan per browser action/preset selection; no blocking or disk I/O in audio callback. Requirement approval is not implementation completion.


## 2026-11-16 — REV.19 PRIORITY OVERRIDE: FACTORY PRESETS + SAMPLES FIRST

Пользовательское решение: дальнейшее исполнение общего журнала ведётся последовательно, но до возвращения к SKIN-1…SKIN-16 необходимо завершить два основных направления продукта — FACTORY PRESETS и SAMPLES. Работа над скинами приостановлена; новые skin increments не начинать, пока эти два направления не пройдут свои критерии готовности.

| ID | Workstream | Priority | Required completion evidence |
|---|---|---|---|
| PSET-01 | Factory preset source/catalog audit and safe migration from legacy 300 to one unified source of 500 | P1 / FIRST | No-loss audit; stable unique IDs; exact 250 MELODY + 250 PATTERN; schema/validator aligned |
| PSET-02 | Factory preset metadata/index and existing browser integration | P1 | One internal source; cached metadata index; two filtered views; unchanged approved UI; no full scan on each selection; no audio-thread I/O |
| PSET-03 | Sample/profile dependency mapping and real playable preset implementation | P1 | Each preset maps to approved sample/library IDs and valid key/root/velocity/articulation data; runtime playback, audition, save/load and performance evidence per preset |
| SAMP-01 | Establish exact current Sample Library and Import Sample Guitar inventory | P1 / FIRST | Actual paths, counts, sizes, formats, existing-content preservation and duplicate/corruption report |
| SAMP-02 | Full sample/SFZ/provenance/license/SHA-256 audit | P1 | Every factory asset traceable; commercial embedding/raw redistribution rights verified; unresolved references and ambiguous licenses isolated |
| SAMP-03 | Complete stable registry and safe import/scan behavior | P1 | Copy-to-Import → SCAN/RESCAN → available; no overwrite/delete of existing content; stable IDs; scan off audio thread |
| SAMP-04 | Runtime playback and performance validation | P1 | SFZ/key/root/velocity/round-robin/articulation mappings; streaming/cache/RAM/startup/callback/dropout tests; real listening QA |

### Locked product decisions carried forward
- Exactly 500 factory preset targets: 250 MELODY + 250 PATTERN; one unified physical source, two existing browser views.
- Existing legacy 300 definitions are preserved until audited and migrated; no blind deletion or overwrite.
- Sample layout remains simple: `PPGU PORTABLE/Samples/Sample Library/` and `PPGU PORTABLE/Samples/Import Sample Guitar/`.
- User import remains ordinary file copy followed by SCAN/RESCAN; imported material is not silently copied into or mixed with the factory redistributable set.
- Factory Sample Library target is 10–12 GB or more if needed; expanded library 40 GB+; no hard upper size cap. Do not trade sample quality/diversity for an arbitrary cap.
- Streaming and cache are required; library size must not imply loading all audio into RAM.
- Every factory sample requires provenance, SHA-256 and explicit license review for commercial embedding and redistribution. Unclear rights = exclude from factory distribution.
- Only genuinely playable, listened-to and validated presets can be marked READY. A JSON count is not readiness.

### Execution order
1. SAMP-01 current inventory and preservation audit.
2. SAMP-02 licensing, provenance, checksums and SFZ reference audit; repair real missing references only after verifying intended assets.
3. SAMP-03 stable registry and safe simple import/scan behavior.
4. SAMP-04 actual streaming/playback and resource validation.
5. PSET-01 audit and no-loss consolidation of legacy 300 into the single 500-entry target.
6. PSET-02 schema/validator/index and existing browser views, with no UI redesign.
7. PSET-03 map, implement and audition presets against the verified library; READY evidence per preset.
8. Resume the remaining master journal items in their recorded order; skins resume only after PRESETS and SAMPLES acceptance.

**No completion claim:** these are the approved execution priorities and acceptance gates, not evidence that the library or 500 presets are complete.


### Clarification — minimum factory Sample Library size (2026-11-16)
The factory **Sample Library itself must be at least 10 GB**. 2.97 GB is the size of the previously reported portable QA build artifact, NOT the Sample Library size and NOT an acceptable library target. The 10 GB minimum applies to the actual factory-distributed sample content, excluding the plugin binary, source code, documents, tools, presets metadata and other package overhead. The intended library may exceed 10 GB whenever quality, variety and licensing justify it; no upper cap is imposed. The expanded library remains 40 GB+ as previously approved. Do not mark SAMPLES complete or report its size based on the overall QA artifact size.


### Binding clarification — Standard 10 GB library is the complete preset baseline (2026-11-16)
- The **Standard Factory Sample Library is at least 10 GB** and is the complete required content baseline for the product.
- **Every one of the 500 factory presets (250 MELODY + 250 PATTERN) must work using only the Standard 10 GB library.** No factory preset may require an asset that exists only in the Expanded Library.
- Every preset's sample references must resolve to assets included in the Standard library; validate playback and mappings against that baseline.
- The **Expanded Library (40 GB+) is optional additional content**. It may add alternate takes, extra articulations, layers, variations or additional sound choices, but must not be a prerequisite for any factory preset to load and play.
- When expanded assets are present, they may enhance or extend a preset; when absent, the preset must remain fully playable from the Standard library without missing-sample errors or silent notes.
- Keep stable sample IDs and explicit Standard/Expanded availability metadata. Do not duplicate the whole Standard library inside the Expanded library unless a concrete technical requirement justifies it.
- Acceptance must include a clean Standard-only install test: install only the Standard library, scan it, load and audition all 500 presets, and confirm there are no unresolved sample references. Expanded-library testing is additional, not a substitute.


### Expanded Library purpose — sound variation, enhancement and optional preset-bank growth (2026-11-16)
The Expanded Library (40 GB+) is an optional enhancement layer, not a second mandatory foundation:
- It broadens sound selection and variation for existing factory presets, allowing richer alternate samples, takes, articulations, velocity/layer options and performance variations where supported.
- It can improve perceived realism, depth and sound quality by providing higher-variety or additional recording material; it must not silently change the identity or break the Standard-only version of a preset.
- It may also support an additional/expanded preset bank beyond the 500 Standard-baseline factory presets. Additional presets must be clearly identified as Expanded-dependent and must never be counted among the 500 Standard-compatible factory presets.
- Existing 500 factory presets remain fully playable from Standard Library alone. Expanded assets may enrich/extend their available sound options, but absence of Expanded must preserve valid playback.
- Keep the preset browser's existing MELODY/PATTERN views and approved UI structure; represent expanded availability through existing metadata/filtering mechanisms rather than inventing new controls or panels.
- Expanded content must pass the same provenance, licensing, stable-ID, mapping, playback and performance validation as Standard content.
