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


## 10 ноября 2026 — обрезание элементов при уменьшении окна

- Корень: `setResizeLimits` разрешал высоту 650 px, но `resized()` резервирует фиксированные 756 px внутри окна плюс 32 px внешних отступов.
- Фикс: минимальная высота изменена на 790 px; добавлен комментарий с объяснением ограничения.
- Проверка: геометрия фиксированных областей сверена с допустимой минимальной высотой; фактическая проверка рендеринга в Windows/FL Studio ожидает CI и host validation.
- Статус: FIXED IN SOURCE / AWAITING CI.


## 10 ноября 2026 — SFZ-библиотека терялась при сохранении проекта

- Корень: `getStateInformation` сериализовал только APVTS-параметры; путь к выбранной SFZ-карте не сохранялся и после открытия проекта сэмплер оставался без восстановленной библиотеки.
- Исправление: путь сохраняется как `sfzFilePath` в ValueTree APVTS; при восстановлении существующая карта загружается асинхронно. Доступ к строке пути защищён `CriticalSection`.
- Совместимость: старые state XML без этого свойства принимаются; отсутствующий файл не блокирует восстановление параметров.
- Regression guard: `tools/validate_apvts_layout.py` проверяет критические элементы round-trip и async restore.
- Проверка: статически сверены все ожидаемые участки в `PluginProcessor.h/.cpp` и validator. Windows compiler/CTest и DAW host test пока ожидают выполнения.
- Статус: FIXED IN SOURCE / AWAITING CI.


## 10 ноября 2026 — M2: первая реализация SFZ Round Robin

- Найденный дефект: SFZ parser не распознавал `seq_length`/`seq_position`, а `SamplePool::findRegion()` всегда возвращал первую совпавшую область. Библиотеки с несколькими round-robin takes поэтому постоянно проигрывали один и тот же сэмпл.
- Исправление: добавлены поля sequence metadata в SFZ Region/RegionView; parser принимает и проверяет диапазон `seq_length=1..256` и `seq_position=1..seq_length`. SamplePool выбирает следующий sequence position детерминированным циклом с отдельным курсором на MIDI channel/note; неполный RR set использует безопасный fallback на первую совпавшую область.
- Regression: создан SFZ fixture с тремя sequence positions; проверяется цикл 1→2→3→1→2→3, а также отказ parser при seq_position > seq_length.
- Commits: `e3ccbf428f513d5bd05e6b9b2d29496284d4978f`, `9cd4f47dc7fab6c03dcc811cf23deaa435d8f950`, `ea3fa563642203b88ba99e3b0481f14350e26192`, `99fcc2fda604508fb77e0c17a42571ae3c1c47d7`, `bd313f77a0172b5d80336ddfab2a9775f364725c`.
- Статус: FIXED IN SOURCE / AWAITING WINDOWS CI + CTest. Пока не подтверждены компиляция, исполнение теста и совместимость с реальными SFZ банками. M2 не закрывать: velocity crossfade ещё не реализован; дополнительно требуется проверить RR-группировку на реальных картах и поведение одновременных sequence sets.


## 10 ноября 2026 — M2: дополнительные SFZ проверки и CI

- Расширен SFZ parser: добавлен разбор метаданных velocity crossfade `xfin_lovel`, `xfin_hivel`, `xfout_lovel`, `xfout_hivel`; добавлена валидация пар границ и порядка диапазонов. Это пока только чтение/проверка метаданных: реальное одновременное смешивание velocity-регионов в аудио-движке ещё НЕ реализовано.
- Добавлены parser regression cases для корректных и некорректных crossfade opcode.
- Добавлен тест повторных 100 ударов с меняющейся velocity: проверяет отсутствие non-finite значений, ненулевой выход и ограничение максимального соседнего скачка. Это автоматическая проверка, но не заменяет слуховой/host-тест и не доказывает абсолютное отсутствие кликов на всех библиотеках.
- Поправлены newline escape в SFZ test fixtures после проверки исходников.
- CI run `42778780300` (commit `ffea193c058644bb4f4807eb475707951a269e00`) подтвердил Windows x64 Release build, CTest, обнаружение VST3 и host-validation limitation step; на момент записи packaging ещё выполнялся. Этот run не включает все более поздние изменения.
- CI run `42778986400` (HEAD `7c9dc1ff7453c6bab701a4589af85c96aa29a733`) повторно прошёл configure, build, CTest, locate VST3 и дошёл до packaging; packaging/upload ещё не завершены при последней проверке.
- Ограничения M2 по-прежнему открыты: реальное velocity crossfade, полноценная RR-группировка без конфликтов между разными наборами, потоковый mmap/OVERLAPPED + hot/warm cache/ref-count/preload, измерение 100-hit click criteria на реальном банке и аудио/host-проверка. M2 не закрывать.


## 10 ноября 2026 — M2: изоляция Round Robin наборов

- Устранена общая RR-корзина для всех SFZ регионов одного MIDI-канала/ноты. Теперь при загрузке формируются независимые RR set indices по SFZ group, seq_length, key/velocity/channel диапазонам.
- Счётчики для каждого набора, MIDI-канала и ноты заранее выделяются при загрузке Snapshot. Вызов `findRegion()` не выделяет память и не создаёт контейнеры на аудиопотоке.
- Добавлен regression test с двумя разными SFZ group и разными MIDI-нотами: вызов одного набора не должен сдвигать последовательность другого.
- Проверка исходника выявила и исправила двойное экранирование переводов строк в новой тестовой SFZ fixture.
- Изменения опубликованы в рабочей ветке коммитами: `8c91bc20`, `15960944`, `8d27214e`, `a11f8b88`, `d3336a32`.
- Статус: FIXED IN SOURCE / AWAITING WINDOWS CI. M2 не закрыта: velocity crossfade playback, потоковая загрузка, реальные 100-hit click validation и host validation остаются открытыми.


## 10 ноября 2026 — M2: публикация velocity crossfade metadata в SamplePool

- Четыре разобранных SFZ opcode (`xfin_lovel`, `xfin_hivel`, `xfout_lovel`, `xfout_hivel`) теперь копируются в опубликованный `RegionView`, а не остаются только в промежуточной структуре парсера.
- Добавлен SamplePool regression test, который загружает SFZ с заданными границами и проверяет, что runtime-регион содержит те же значения.
- Исправлено экранирование newline в тестовой fixture.
- Важно: это подготовка runtime-данных, а не готовый velocity crossfade DSP. Два слоя ещё не смешиваются; M2 остаётся открытой.
- Коммиты: `991d05eb`, `1d82e302`, `21d9b576`, `6aa02483`.
- Статус: FIXED IN SOURCE / AWAITING WINDOWS CI.


## 10 ноября 2026 — Проверка release gates и незакрытые M1–M5 критерии

- Подтверждены Windows Release + CTest + pluginval strictness 5 + packaging в run `42841128991`; отдельный MSVC VST3/sampler test run `42841133299` также SUCCESS.
- Это не закрывает ручную DAW-валидацию M1/M3: FL Studio 20 host test не выполнялся.
- M2 blocker: SFZ velocity crossfade metadata уже хранится в runtime region, но смешивание соседних velocity layers в DSP ещё не подтверждено/не завершено; также остаются реальные click/pop, palm mute и concurrency stress проверки.
- M3 blocker: нужны end-to-end тесты scheduler→sampler и DAW MIDI/transport (NoteOn/Off, stop/seek/loop, tempo, simultaneous chords, mode changes).
- M4 blocker: новый AutoChords/FingeringGenerator foundation ожидает завершения Windows build/CTest/pluginval; scale-aware tonic mapping и runtime scheduler/state integration ещё не сделаны.
- M5 blocker: StyleEngine foundation прошёл CI run `42840410645`, но пока не подключён к runtime Pattern event pipeline и preset/UI binding.
- Не помечать M1–M5 полностью готовыми до закрытия перечисленных пунктов. Подробности в `docs/WORK_LOG_M1_M5_INTEGRATION_RU.md`.

## 10 ноября 2026 — обновлённые M2/M3/M4/M5 статусы

- M3 scheduler→SamplerEngine module integration regression добавлен и прошёл CTest в run `42843030370`; FL Studio 20 transport/MIDI validation остаётся открытой.
- M2 velocity crossfade playback реализован в изолированном PR #5, run `42843931173` ожидает завершения. Palm-mute sample mapping и реальные click/pop/concurrency checks остаются открытыми.
- M4 scale-aware AutoChords/FingeringGenerator прошёл run `42843127831`; последующие integer-overflow guards требуют нового CI.
- M5 StyleEngine foundation прошёл run `42840410645`; runtime integration остаётся открытой.
- M1 DAW save/reopen, automation, missing-library recovery, device change и long-playback host test остаются открытыми.


## Open integration risks — 10.11.2026

- M4 scale chord generation must remain optional and must not alter the Melody one-key/one-note path.
- FingeringGenerator currently uses a bounded greedy search; validate difficult inversions, low/high MIDI boundaries, and all guitar positions before relying on it for factory presets.
- M5 StyleEngine currently modifies event velocity/accent only. Articulation enum is not yet connected to SFZ region selection, and strum spread/FX taste are not yet consumed by the processor.
- M2 velocity crossfade has a passing isolated CI run but is not yet in the integration branch; verify summed crossfade level and preserve per-note voice grouping when porting.
- The current sample-bank artifact is not evidence that new electric guitar takes were recorded. New audio needs source/license manifest and checksum.
- The user's FL Studio report confirms plugin load and audible MIDI response only; save/reopen, project state restoration, transport, long playback and cache pressure remain unverified.
