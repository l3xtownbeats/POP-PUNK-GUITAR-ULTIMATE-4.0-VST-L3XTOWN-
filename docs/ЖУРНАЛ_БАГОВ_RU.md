# Журнал багов — POP-PUNK-GUITAR-ULTIMATE

Дата обновления: 10 ноября 2026
Ветка: `migration/full-copy-m3-transport-scheduler-v2`
HEAD до записи CI: `bdd540eb30aeef8d2f51768ef5bb7e63129a9920`

Статусы: **FIXED — AWAITING CI** означает, что правка внесена, но успешный актуальный build/test не подтверждён; **OPEN** — исправление или приёмка не завершены.

| ID | Приоритет | Область | Дефект / риск | Исправление или действие | Статус |
|---|---|---|---|---|---|
| M3-001 | High | MIDI chord runtime | Held chord обновлялся после планирования, новый аккорд запаздывал на один audio block. | Held MIDI state применяется до chord snapshot. | FIXED — AWAITING CI |
| M3-002 | High | MIDI timing | Не учитывался MIDI samplePosition при смене аккорда. | Старый сегмент рендерится до sample offset; смена применяется на точной позиции события. | FIXED — AWAITING CI |
| M3-003 | High | Pattern quantize | UI/APVTS предлагал 1/16 и 1/32, runtime использовал фиксированную 1/16. | Runtime читает patternQuantize, использует 0.25/0.125 PPQ; добавлены тесты. | FIXED — AWAITING CI |
| M3-004 | High | Pattern configuration | Смена pattern slot/length не перезапускала фразу на квантованной границе. | Отслеживаются предыдущие slot/length; конфигурационные переходы квантуются. | FIXED — AWAITING CI |
| M3-005 | High | Transport | Host seek мог оставить pending NoteOff и зависшие ноты. | PPQ discontinuity очищает pending gates; Processor отпускает sampler voices. | FIXED — AWAITING CI |
| M3-006 | High | Transport source | Переход Host→Internal и Internal→Host мог продолжить старую временную шкалу. | Оба направления handoff считаются discontinuity; добавлен regression test. | FIXED — AWAITING CI |
| M3-007 | Medium | Event timing | Округление sample offset могло поставить событие раньше фактического времени у границы блока. | Используется первый sample не раньше события; добавлен near-tail test. | FIXED — AWAITING CI |
| M3-008 | High | Pending gate capacity | Переполнение очереди NoteOff могло потерять событие и вызвать hanging note. | Увеличена bounded queue; overflow вызывает all-notes-off fail-safe; тест добавлен. | FIXED — AWAITING CI |
| M3-009 | Medium | APVTS validation | Regex валидатора неверно экранированы и давали ложные ошибки. | Экранирование исправлено; статическая проверка ID ранее прошла. | FIXED — AWAITING CI |
| M3-010 | High | Build verification | Промежуточные Windows CI запуски завершились failure; успешная сборка не подтверждена. | Исправления внесены; run 42769587683 ожидает завершения на момент записи. | OPEN |
| M3-011 | High | DAW integration | Не подтверждены playback/stop/seek/pattern transitions в FL Studio 20. | Ручной acceptance test на Windows 10 / FL Studio 20. | OPEN |
| M3-012 | Medium | Block-size stress | Полный runtime стресс при 64–1024 samples и BPM automation не подтверждён. | Проверить CTest/DAW при разных block sizes и изменениях темпа. | OPEN |
| M3-013 | High | Sample-accurate mapping | Fractional PPQ offset 63.6 samples после ceil прижимался к последнему sample текущего блока (63), хотя первый допустимый sample 64 уже в следующем блоке. | Не clamp к blockSamples-1; bounded pending queue переносит NoteOn/NoteOff в следующий блок. Regression: defer + NoteOn@0 + NoteOff@2 и exact 60→60. Windows CI run 42772172657 / commit bdd540e: Release build, CTest, VST3 package и artifact upload SUCCESS. | FIXED — CI SUCCESS |
| M3-014 | High | MIDI chord transitions | Несколько chord changes внутри одного блока сводятся к одному финальному chord snapshot; промежуточные аккорды теряются. | Общий allocation-free sample-accurate timeline walker рендерит каждый интервал до MIDI события и применяет событие в его sample; regression проверяет chord snapshots и точные границы интервалов. | FIXED IN SOURCE — AWAITING CI |
| M3-015 | High | Scheduler capacity | Фиксированный output capacity 512 может молча отсечь события плотного pattern; нет отдельного overflow flag для output array. | Явный overflow status + all-notes-off fail-safe и regression test. | OPEN |
| M1-001 | High | State persistence | Активная sample library path/ID не входит в APVTS state; DAW restore не гарантирует возврат выбранной библиотеки. | Добавить versioned library reference и безопасное восстановление на worker thread. | OPEN |
| M1-002 | High | State/automation acceptance | Нет автоматизированного round-trip теста параметров/state и DAW automation acceptance для текущего HEAD. | Добавить state serialization tests и проверить FL Studio automation/save/reopen. | OPEN |
| M1-003 | High | Host validation | Pluginval и ручная FL Studio 20 acceptance не подтверждены. | Запустить pluginval strict и ручной host checklist; записать версии/настройки. | OPEN |
| M2-001 | Critical | Sample streaming | SamplePool декодирует полные WAV в RAM; это не требуемый page streaming/lazy loading. | Подключить indexed WavPageReader + bounded PageCache к voice read path. | OPEN |
| M2-002 | Critical | Streaming integration | PageCacheSlots/WavPageReader/MetadataIndex существуют отдельно; SamplerEngine::Voice читает полный AudioBuffer. | Реальная интеграция page request/prefetch/cache pinning в voice lifecycle. | OPEN |
| M2-003 | High | Sample selection | Runtime round-robin и velocity crossfade не подтверждены; findRegion возвращает первый matching region. | Добавить RR groups/counters и velocity crossfade с тестами. | OPEN |
| M2-004 | High | Library lifecycle | SamplePool ограничен 1 GiB и 128 snapshots; нет безопасного unload/reload и возврата памяти. | Ввести ref-counted immutable bank snapshots и audio-safe retirement. | OPEN |

## Правила закрытия багов
1. Не закрывать баг только на основании изменения исходника.
2. Для runtime-багов нужны regression tests; для build-багов — успешный Windows CI на актуальном commit.
3. Для DAW integration нужны ручные результаты FL Studio 20 с версией хоста, sample rate, buffer size и сценарием.
4. Если CI обнаруживает регрессию, добавить новый ID либо обновить существующий статус и указать commit/run.

## Аудит M1–M3 — 10 ноября 2026

См. полный технический разбор и план исправлений в корневом `Info.txt`. M1, M2 и M3 остаются NOT ACCEPTED. M3-013 исправлен в исходнике и ожидает повторного CI; M1-001…003, M2-001…004, M3-014…015 остаются OPEN.


## M3-013 — уточнение root cause после run 42770853146
- Run 42770853146: Windows Release configure и VST3 compilation успешны; CTest 5/6, failed: ppgu_pattern_scheduler_tests. Package steps skipped.
- Причина: ceil(63.6)=64 был затем clamp'нут до 63 из-за диапазона [0, blockSamples-1]. Ожидание sample 64 внутри первого блока было ошибочным: индекс 64 — это sample 0 следующего блока.
- Внесён source fix: deferred bounded queue теперь хранит NoteOn и NoteOff; выходящие за границу события не отправляются преждевременно.
- Новая regression: в первом блоке count=0; во втором блоке NoteOn sample 0 и NoteOff sample 2. Статус остаётся FIXED IN SOURCE / AWAITING CI.


## M3-013 — подтверждение исправления CI
- Commit: bdd540eb30aeef8d2f51768ef5bb7e63129a9920.
- Windows Actions run 42772172657 завершился SUCCESS (2026-11-10 07:06:15 UTC).
- Configure x64 Release, Build VST3 and test executables, Run CTest regression suite, Locate VST3 bundle, Package VST3/sample library/factory presets и Upload artifact — все шаги SUCCESS.
- Artifact ID 13741929896, SHA-256 4cf7f5f59d6f04a12e7238b1c1c379ba2ae666ca8c096a940cc53cebd4e0b519.
- M3-013 закрыт по автоматизированной CI-проверке. Ручная FL Studio 20 host acceptance не подразумевается и остаётся отдельным открытым критерием.


## M3-015 — исправление внесено, ожидает CI
- Root cause: цикл PatternScheduler завершался при заполнении output array без отдельного overflow-флага; часть событий могла быть отброшена незаметно.
- Fix: добавлен `outputOverflow_`; `consumePendingOverflow()` объединяет output и pending-queue overflow. Существующий processor fail-safe вызывает `sampler.allNotesOff()`.
- Regression: 300 событий в одном блоке должны заполнить 512-элементный output и выставить overflow; флаг потребляется ровно один раз.
- Commits: `427a68506599b7f8ea8250b253dbc89ede4fe4fb`, `6a77e2d1f33f6a4ad730d4c5a6c7997d2a5112cf`.
- Статус: FIXED IN SOURCE — AWAITING WINDOWS CI. Не закрывать до успешного CTest.


## 10 ноября 2026 — M3-015 CI failure: missing timeline helper include и исправление

- Проверены Windows Actions runs `42774950334` (кодовый commit `6a77e2d1...`) и `42774991354` (документационный commit `efb30595...`). Оба завершились `failure` на шаге `Build VST3 and test executables`; CMake configure прошёл, CTest был пропущен, VST3 bundle/artifact не создан.
- Точная ошибка MSVC: `PluginProcessor.cpp(505): processSampleAccurateMidiTimeline is not a member of ppgu::performance` / `identifier not found`.
- Root cause: `PluginProcessor.cpp` вызывает helper из `Source/Performance/ChordTransitionTimeline.h`, но не включал этот заголовок. Тестовый файл включал его напрямую, поэтому дефект не обнаруживался unit-test compilation.
- Исправление: добавлен прямой `#include "Performance/ChordTransitionTimeline.h"` в `Source/PluginProcessor.cpp`. Commit: `6de1db6777a7afa4dc818fab244c06b3dc099b4d`.
- В логах также присутствуют предупреждения C4244 в PluginEditor.cpp и C4458 для локального `playHead`; они не являются причиной текущего failure и остаются отдельными cleanup-кандидатами.
- Статус исправления: FIXED IN SOURCE / AWAITING WINDOWS CI. Новый build и CTest ещё не запускались/не подтверждены. M3-014 и M3-015 не закрывать до успешного Release build + CTest.
- Следующий шаг: дождаться CI на commit `6de1db...`; если появятся новые ошибки — разбирать по первой реальной compiler/test failure, не отключая тесты. После успешного CTest продолжить M3 stop/seek, block-size/tempo stress и fail-safe проверки. M1/M2/M3 остаются NOT ACCEPTED; M4 BLOCKED.


## 10 ноября 2026 — cleanup предупреждений MSVC после успешной сборки

- В `Source/PluginEditor.cpp` устранены неявные преобразования координат `int` → `float` в `ColourGradient` и `Path::addQuadrilateral` явными `static_cast<float>`; причина — C4244 в отрисовке UI.
- В `Source/PluginProcessor.cpp` локальная переменная `playHead` переименована в `hostPlayHead`, чтобы не скрывать член класса; причина — C4458.
- Изменения внесены в рабочую ветку `migration/full-copy-m3-transport-scheduler-v2`.
- Статус: FIXED IN SOURCE. Повторная Windows Release build/CTest для этих изменений ожидается; предупреждения пока не объявляются устранёнными до проверки свежего лога.


## 10 ноября 2026 — поиск пресетов не должен менять активный звук

- Проблема: обновление списка после ввода поискового запроса/смены категории выбирало первый результат с `sendNotification`, из-за чего интерфейс мог незаметно применить другой пресет.
- Исправление: запоминать выбранный preset index, сохранять выбор при наличии результата; при отсутствии — снимать выбор без уведомления. Поиск дополнен метаданными soundCharacter, groove, articulation и mode.
- Проверка: исходный код просмотрен после изменения; автоматическая Windows-сборка/CTest ещё не получена.
- Статус: FIXED IN SOURCE / AWAITING CI.
