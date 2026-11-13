# PPGU — СВОДНЫЙ РЕЕСТР ЗАДАЧ (v2, 2026-11-13)

Формат: ID | задача | статус | приоритет | зависимости / доказательство.
Статусы отражают состояние ИНТЕГРИРОВАННОЙ рабочей ветки
work/ppgu-completion-integrated-2026-11-13, а не наличие одноимённых
описаний в других ветках.

## A. Текущий этап и Skin Engine

| ID | Задача | Статус | Приоритет | Зависимости / примечания |
|---|---|---|---|---|
| S1-01 | SoundAssetRegistry CLI policy tests | IMPLEMENTED; exact-SHA CI 18-case rerun pending | P1 | commit 9c40c6a; run 43439206786 |
| S1-02 | CC-BY asset-level attribution accepted | IMPLEMENTED IN VALIDATOR POLICY | P1 | existing validator behavior + CLI tests |
| S1-03 | EXPLICIT_PERMISSION + legalReview=PENDING blocked | IMPLEMENTED IN VALIDATOR POLICY | P1 | existing validator behavior + CLI tests |
| O1-01 | OverlayManager core (stack/modal/listener) | SOURCE NOT VERIFIED IN INTEGRATED BRANCH | P1 | user supplied implementation report; source files absent from inspected working tree |
| O2-01 | SecureSkinImport .ppguskin | SOURCE NOT VERIFIED IN INTEGRATED BRANCH | P1 | user supplied implementation report; source files absent from inspected working tree |
| O2-02 | SHA-256/path/SVG security tests | NOT VERIFIED IN INTEGRATED BRANCH | P1 | source and tests must be located/imported before CI |
| G3.5 | usage.json semantic-token validator | NOT VERIFIED IN INTEGRATED BRANCH | P1 | user report says implemented; validator absent from inspected working tree |
| SB-VAL | tools/validate_soundbank.py CI validator | NOT VERIFIED IN INTEGRATED BRANCH | P1 | user report says build.yml references it; inspect workflow before claiming |
| O1-03 | OverlayManager integration in PPGUMultiWindowWorkspace | NOT STARTED | P1 | O1-01 source + exact-SHA CI |
| O2-03 | juce::ZipFile -> SecureSkinImport production wrapper | NOT STARTED | P1 | O2-01/O2-02 |
| SK-RT | Skin Gallery runtime hot-swap | PARTIAL | P1 | DPI/ECO/runtime gates |
| SK-DPI | DPI/ECO gates | UNVERIFIED | P2 | skin runtime integration |
| SK-GAL | Full Skin Gallery: search/favorites/preview/apply/back | PARTIAL | P2 | runtime and UX acceptance |

## B. REV18.1 implementation queue

| ID | Задача | Статус | Приоритет | Зависимости |
|---|---|---|---|---|
| S2 | PanelManager lifecycle/layout/paging/persistence | NOT STARTED | P1 | S1 |
| S3 | ThemeTokens.h 21 canonical tokens + legacy mapping | NOT STARTED | P1 | S2 |
| S4 | JUCE-free ResourceRootResolver | NOT STARTED | P1 | S3 |
| S5 | Skin_053 assets + contrast/PNG validation | NOT STARTED | P1 | S4 |
| S6 | Runtime skin discovery/hot-swap/gallery | NOT STARTED | P1 | S5 |
| S6.9 | Five flagship factory presets | NOT STARTED | P2 | S6; only approved preset-edit window |
| S7 | AmpChain DSP bounded implementation | NOT STARTED | P2 | S6.9 |
| S7.5 | Risk closure: quiescence/worker/root cap/lifetime/stress | NOT STARTED | P1 | S7 |
| S8 | Windows Release + CTest + pluginval 5 + package | NOT STARTED | P1 | S7.5 |
| S9 | FL Studio 20 host acceptance + metrics | NOT STARTED | P1 | S8 |

## C. Sound Hunter / SoundBank

| ID | Задача | Статус | Приоритет | Зависимости |
|---|---|---|---|---|
| SH-01 | Existing SoundAssetRegistry CLI tests | IN PROGRESS | P1 | current exact-SHA run |
| SH-02 | Extend existing S6.5 ingest pipeline | NOT STARTED | P2 | S1; no second ingest pipeline |
| SH-03 | DeepSearchWorker | NOT STARTED | P2 | SH-02 |
| SH-04 | StyleClassifier | NOT STARTED | P2 | SH-03 |
| SH-05 | Curator GREEN/YELLOW/RED production integration | NOT STARTED | P2 | SH-02..04 |
| SB-01 | guitar_ingest.py extension | NOT STARTED | P2 | SH-02 |
| SB-02 | LICENSES.txt generation | NOT STARTED | P2 | SB-01 |
| SB-03 | B1–B4 SoundBank pipeline | NOT STARTED | P3 | SB-02 |
| SB-04 | 7–10 GB production payload | PENDING | P3 | legal registry + storage + packaging |

## D. Product / engine and host acceptance

| ID | Задача | Статус | Приоритет | Зависимости |
|---|---|---|---|---|
| EN-01 | Common PPGU Audio Engine (Guitar/Bass) | DESIGNED | P2 | quiescence audit |
| EN-02 | Bass Edition | NOT IMPLEMENTED | P3 | EN-01 |
| QA-01 | Windows Release + CTest + pluginval on exact SHA | PENDING | P1 | S8 |
| QA-02 | FL Studio 20 host acceptance on exact binary | NOT VERIFIED | P1 | QA-01 |
| QA-03 | CPU/RAM profiling against measurable targets | NOT MEASURED | P2 | QA-01 |
| QA-04 | Selective rollout of validated work from other branches | IN PROGRESS / NOT VERIFIED | P2 | per-file diff, dependencies, CI |

## Правила

- Статус обновляется только после scoped commit -> exact-SHA CI -> запись Info.txt.
- Implemented без exact-SHA CI остаётся VALIDATION PENDING.
- BUILT, TESTED, HOST-VERIFIED, RELEASED запрещены без соответствующего
  проверяемого evidence.
- Не переносить целые ветки вслепую; только выборочные файлы/коммиты после
  diff, зависимости и проверки на текущей базе.
- Не создавать параллельные ingest pipelines; расширять существующий S6.5.
- Не менять factory presets вне S6.9.
