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
