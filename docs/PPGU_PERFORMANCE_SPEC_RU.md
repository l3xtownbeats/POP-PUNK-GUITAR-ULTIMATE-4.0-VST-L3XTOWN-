# POP PUNK GUITAR ULTIMATE — PERFORMANCE SPECIFICATION (RU)

**Целевая система:** FL Studio 20 / Windows 10, VST3 x64, JUCE 7.0.12, C++20.  
**Статус:** DESIGNED / PERFORMANCE SPEC. Обязательная спецификация; не является описанием готовой реализации.  
**База:** INFO 2026-11-12; CODE.txt governance; исходная опорная ревизия `work/signal-forge-theme-tokens` / `57fe40044532eeab4bc66ccec37bce865904d455`.  
**Оговорка:** CPU/RAM budgets не подтверждены измерениями.

## 1. Целевые бюджеты

| Метрика | Цель | Проверка |
|---|---:|---|
| CPU, 1 instance, 44.1/48 kHz, 128–512 samples, 4 voices | <= 2% на 4-core desktop CPU | FL Studio PerfMeter / profiler |
| CPU stress, 64 voices | <= 15%, без queue overflow | R4 stress test |
| RAM instance, SoundBank не загружен | <= 60 MB | Working set после старта |
| RAM, полный SoundBank, sliced cache | <= 512 MB | Active memory + cache cap |
| Preset load | <= 300 ms, с учётом длины SFZ | Background timing log |
| First sound after load | <= 50 ms | Latency test |
| Allocations in processBlock | 0 — HARD GATE | Audit build, malloc override / JUCE leak detector |
| Peak audio callback | <= 60% buffer при 128 samples | Test timing |

Каждый пункт должен стать CTest counter или profiling report. Без соответствующего измерения статус цели не повышается.

## 2. Audio core

### 2.1 Предварительное выделение памяти
- 64 voice slots заранее выделены; в realtime voices не создаются и не уничтожаются, только переиспользуются через freelist (lock-free stack).
- Page cache имеет фиксированный бюджет (начальная гипотеза — 256 MB cap) и LRU eviction. Eviction выполняется только background worker, никогда audio thread.
- Snapshots и streaming services используют pools и generation tokens; heap churn исключён из realtime path.

### 2.2 Streaming path
- Cache miss не блокирует audio thread; voice удерживает cursor до поступления страницы.
- Счётчики queue-overflow, cache-full и miss-rate доступны для UI diagnostics.
- SPSC queue и её 500k-transfer coverage не переписывать; только профилировать.
- Hermite interpolation сохранить. SIMD (SSE2 baseline x64, AVX2 с runtime detection) применять к interpolation и gain-staging только при подтверждённом профайлером выигрыше.

### 2.3 Запреты в audio callback
Статическая проверка и review должны запрещать:
- `new/delete`, mutex, аллоцирующие STL containers, logging и системные вызовы;
- исключения через границу callback; worker recovery (risk R3) обрабатывается на стороне worker, не в `processBlock`;
- `sleep/yield` в callback.

## 3. Streaming tuning — Windows 10 / FL Studio 20
- Начальный диапазон размера страницы: 256–512 KB; 8–32 страниц в очереди на root. Финальные параметры выбираются тестами latency vs miss-rate.
- Сохранить известный лимит 8 streaming roots. Risk R2 (совокупные память/потоки) закрывать per-root quotas и глобальным memory/thread cap.
- Один общий worker pool для всех roots; не создавать поток на root.
- Сначала metadata index; тяжёлые страницы загружать по требованию. Preview pages (начала нот) получают приоритет очереди.

## 4. UI performance
- Repaint по регионам: панели/overlays перерисовывают только dirty rects; глобальный repaint допустим при смене скина (fade 150 ms).
- CPU/status bar и meters: 10–15 Hz, не 60 Hz. LED glow animation — фиксированный таймер только при видимом окне; при скрытии suspend.
- Skin Engine ECO mode отключает shadow/glow/blur.
- Palette передаётся через atomic snapshot; paint не блокируется. ThemeManager hardening — применять после зелёного CI.
- Текст и шрифты используют GlyphCache, не пересчитываются при каждом paint.

## 5. Release build configuration
- Release: MSVC `/O2`; `/fp:fast` локально внутри DSP modules, не глобально; LTO `/GL /LTCG`; `/permissive-`; `/arch:SSE2` baseline. Для не-DSP кода — MinSizeRel-like настройки.
- Debug assertions и диагностические проверки production отключать только в Release-конфигурации; Debug сохраняет проверки.
- Подключать только необходимые JUCE modules: `audio_basics`, `dsp`, `gui_basics`, `audio_processors`, `events`, `graphics`, `core`, `data_structures`. Фактические зависимости проверить перед удалением модулей.

## 6. FL Studio 20 host constraints
- Не выполнять тяжёлую работу в `stateChanged/notify` циклах; UI updates доставлять по таймеру, не push из audio thread.
- `setLatencySamples` должен отражать реальную задержку. Pattern scheduler не добавляет скрытую задержку.
- SFZ load не блокирует GUI thread; прогресс передаётся асинхронным ticket.

## 7. Acceptance gates / CI
1. Новый CTest target `ppgu_perf_bench`: processBlock benchmark на synthetic и real SFZ, allocation counters, preset load timing. Запуск на каждом SHA.
2. Perf regression gate: ухудшение любой закреплённой метрики >10% относительно последнего accepted benchmark = FAIL и review причины.
3. Real-world host check: пользователь в FL Studio 20 снимает PerfMeter для startup, MELODY 4 voices, 64-voice stress, SKIN menu и theme hot-swap. Результат записывается в INFO как HOST VERIFIED только для точного binary SHA и конфигурации.

## 8. Порядок внедрения
- **P1:** закрыть CI runs #971/#972 терминальными результатами; применить ThemeManager hardening после зелёного CI.
- **P2:** добавить perf benchmark и allocation-audit build; снять и зафиксировать baseline до оптимизаций.
- **P3:** оптимизировать по данным: page-size tuning, shared streaming thread pool, SIMD zones, UI timers, ECO mode.
- **P4:** R4 stress + FL Studio 20 host check; только измеренные результаты подтверждают CPU/RAM.
- **P5:** новые features/panels и расширение Skin Engine — после подтверждения бюджетов.

## 9. Governance
- DESIGNED нельзя записывать как IMPLEMENTED. Без измерений бюджеты остаются заданными, не достигнутыми.
- Каждый perf result привязан к SHA, CPU, sample rate, buffer size, voice count и FL Studio version.
- Risks R1–R6 не удаляются; обновляется только статус и evidence.
- Production code changes — по одной причине на commit; полный CI на каждом SHA.

## INFO entry template

```md
## 2026-11-12 — Performance specification: FL Studio 20 / Windows 10 budgets

**Статус:** PERFORMANCE SPEC DESIGNED; implementation PENDING (P1–P5).
Вход: требование минимального потребления CPU/RAM при максимальной
скорости и качестве на целевой системе FL Studio 20 / Windows 10.
Production source не менялся. Установлены измеримые бюджеты: CPU <= 2%
(4 voices), <= 15% (64 voices stress); RAM <= 60 MB idle / <= 512 MB
полный SoundBank; загрузка пресета <= 300 мс; первый звук <= 50 мс;
0 аллокаций в processBlock (жёсткий гейт); peak callback <= 60% буфера
при 128 samples. Введены гейты: ppgu_perf_bench, perf regression gate
(10%), HOST VERIFIED только по измерениям в FL Studio 20 для конкретного
binary SHA. Порядок: P1 (CI + hardening) → P2 (baseline) → P3 (оптимизация
по данным) → P4 (stress + host check) → P5 (новые фичи).
Спека: docs/PPGU_PERFORMANCE_SPEC_RU.md.
**Self-check:** документальный этап; CI PASS не присваивается этой спеки;
измерений пока нет — бюджеты заданы, не подтверждены.
```
