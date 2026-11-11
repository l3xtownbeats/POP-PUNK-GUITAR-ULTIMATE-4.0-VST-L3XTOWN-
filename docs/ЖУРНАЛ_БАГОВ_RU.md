# Журнал багов — POP-PUNK-GUITAR-ULTIMATE

Дата обновления: 10 ноября 2026
Ветка текущего цикла: `fix/m1-m2-m3-release-gates`
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
| M3-010 | High | Build verification | Ранее были промежуточные неуспешные Windows CI. | Последний полный Windows Release VST3 workflow 42864523781 на source commit fcf8c56 завершился SUCCESS: CTest, pluginval strictness 5, VST3 package и artifact upload. | FIXED — CI SUCCESS; host acceptance отдельна |
| M3-011 | High | DAW integration | Не подтверждены playback/stop/seek/pattern transitions в FL Studio 20. | Ручной acceptance test на Windows 10 / FL Studio 20. | OPEN |
| M3-012 | Medium | Block-size stress | Полный runtime стресс при 64–1024 samples и BPM automation не подтверждён. | Проверить CTest/DAW при разных block sizes и изменениях темпа. | OPEN |
| M3-013 | High | Sample-accurate mapping | Fractional PPQ offset 63.6 samples после ceil прижимался к последнему sample текущего блока (63), хотя первый допустимый sample 64 уже в следующем блоке. | Не clamp к blockSamples-1; bounded pending queue переносит NoteOn/NoteOff в следующий блок. Regression: defer + NoteOn@0 + NoteOff@2 и exact 60→60. Windows CI run 42772172657 / commit bdd540e: Release build, CTest, VST3 package и artifact upload SUCCESS. | FIXED — CI SUCCESS |
| M3-014 | High | MIDI chord transitions | Несколько chord changes внутри одного блока могли сводиться к одному финальному chord snapshot. | Allocation-free sample-accurate timeline walker интегрирован в PluginProcessor; regression проверяет интервалы 0–16, 16–80, 80–120 и 120–160 samples с разными chord snapshots. Release build + CTest run 42864523781 SUCCESS. | FIXED — CI SUCCESS; DAW acceptance OPEN |
| M3-015 | High | Scheduler capacity | Переполнение fixed scheduler output array могло молча отсекать события и оставлять зависшие ноты. | Scheduler выставляет outputOverflow_, consumePendingOverflow() передаёт fail-safe; PluginProcessor вызывает sampler.allNotesOff(). Dense-pattern и extreme-PPQ regression tests проверяют overflow signal. Release CTest run 42864523781 SUCCESS. | FIXED — CI SUCCESS; плотность/host stress ещё требуется |
| M1-001 | High | State persistence | Активная sample library path/ID не входит в APVTS state; DAW restore не гарантирует возврат выбранной библиотеки. | Добавить versioned library reference и безопасное восстановление на worker thread. | OPEN |
| M1-002 | High | State/automation acceptance | Нет автоматизированного round-trip теста параметров/state и DAW automation acceptance для текущего HEAD. | Добавить state serialization tests и проверить FL Studio automation/save/reopen. | OPEN |
| M1-003 | High | Host validation | Ручная FL Studio 20 acceptance не подтверждена; pluginval strictness 5 ранее не был зафиксирован для текущего цикла. | Pluginval strictness 5 и VST3 bundle прошли в run 42864523781. Открытым остаётся ручной FL Studio 20 checklist: load, MIDI, automation, save/reopen, panic и recovery. | OPEN — DAW ACCEPTANCE |
| M2-001 | Critical | Sample streaming | Исторически SamplePool декодировал WAV целиком; требуется bounded page streaming/lazy loading. | Текущий runtime индексирует WAV metadata, публикует streaming regions; SamplerEngine voices читают через StreamingVoiceReader/page service с Hermite interpolation. Многополосный playback regression проходит CTest run 42864523781. Остались Windows mmap/OVERLAPPED и production stress acceptance. | FIXED — CI SUCCESS (core streaming path); platform/stress gates OPEN |
| M2-002 | Critical | Streaming integration | Риск отсутствия реальной интеграции page reader в voice lifecycle. | Подтверждено в Source/Sampler/SamplePool.cpp: noteOn attaches StreamingVoiceReader, prefetches pages; render reads Hermite samples from stream service. Multi-page audible playback regression проходит CTest run 42864523781. Safe snapshot retirement/cache lifecycle остаются отдельным M2-004 gate. | FIXED — CI SUCCESS (voice-path integration) |
| M2-003 | High | Sample selection | Velocity crossfade DSP blending не подтверждено; RR runtime требует изоляции наборов и regression coverage. | Runtime round-robin set indexing/counters и независимые sequence tests добавлены; CI coverage проходит. SFZ xfin/xfout metadata парсится и публикуется в RegionView, но одновременное DSP-смешивание velocity layers ещё не подтверждено/не закрыто. | PARTIAL — RR CI SUCCESS; CROSSFADE DSP OPEN |
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


## 10 ноября 2026 — IMP-001: importer regression expectation mismatch

- При CI run 42855790840 (SHA e8f3baa9b2b85007bab085bcc046fa0dc8721280) сборка VST3 прошла до CTest; 11/12 тестов прошли.
- Упал только ppgu_sample_library_importer_tests. Сообщение: ожидание отчёта не совпало с фактическим: Imported=3, mapped=2, needsMapping=1. Файл Unknown_take.wav не содержит note/root key в имени и был импортирован с Needs Mapping.
- Вероятная причина — устаревшее или неверное ожидание тестового fixture относительно корректного поведения importer. Это пока гипотеза; сначала проверить fixture, assertions и классификацию, затем изменить код/тест по установленному контракту.
- Требование исправления: тест должен проверять mapped и needsMapping отдельно, в том числе Unknown_take.wav → Needs Mapping; не подменять неизвестный root догадкой и не удалять такой файл.
- Статус: OPEN — ROOT CAUSE REVIEW REQUIRED. Исправление не внесено, тест не закрыт.


## IMP-001 — устаревший SFZ путь после atomic publish
- Root cause: `report.generatedSfz` ссылался на файл внутри staging directory; после успешного directory rename staging path исчезал, а условная проверка `existsAsFile()` не перепривязывала путь к final directory.
- Исправление: после rename путь генерируется как `finalDirectory/Imported_Instrument.sfz`, если есть хотя бы один mapped region.
- Regression: `ppgu_sample_library_importer_tests` проверяет существование SFZ по возвращённому пути, парсит его и валидирует ссылки на samples.
- Commit: `39626076a8ded975bdfb280a39880bad66f42b4c`.
- Статус: **FIXED — AWAITING CI**. Не закрывать до прохождения актуального Windows build и CTest.


## 10 ноября 2026 — актуализация по Windows VST3 workflow 42864523781

- Source commit: `fcf8c56e9bea635134d00f0c7a4d4e01a4def370`; branch HEAD после документационных commits: `85a812902b236a7be287401b9a4633ece9fd99a5`.
- Полный Windows x64 Release workflow завершился SUCCESS: build, CTest regression suite, VST3 bundle, pluginval strictness 5, package и artifact upload.
- Исправление пути SFZ после atomic publish ранее прошло M1/M2 workflow `42860937017` на commit `39626076a8ded975bdfb280a39880bad66f42b4c`; полный workflow также проходит importer regression tests.
- M2: streaming в production voice path и round-robin покрыты CI; velocity crossfade пока только metadata, DSP blending остаётся OPEN. Windows mmap/OVERLAPPED, safe snapshot retirement, отдельные palm-mute SFZ zones, 100-hit no-click и sustained stress остаются OPEN.
- M3: sample-accurate chord timeline и scheduler output overflow fail-safe покрыты CTest. Ручная FL Studio 20 transport/MIDI acceptance и расширенные host stress остаются OPEN.
- M1: pluginval strictness 5 закрыт автоматизированным CI, но DAW save/reopen, automation, library reference restore и panic acceptance остаются OPEN.
- Ни M1, M2, ни M3 не объявляются полностью принятыми.


## REQ-BASELINE-001 — обязательная реализация всех утверждённых требований

- Дата: 10.11.2026.
- Тип: базовое требование продукта (не дефект кода).
- Решение пользователя: все дополнения и изменения, ранее согласованные в этой ветке, должны быть реализованы.
- Источник полного перечня и критериев: `docs/ТЗ_ПОЛНАЯ_РЕАЛИЗАЦИЯ_СОГЛАСОВАННЫХ_ФУНКЦИЙ_RU.md`.
- Область: importer/formats, Float32 normalization, manifest/licensing/checksum, SFZ/mapping/preview, sampler/streaming/mmap/snapshot safety, Voices/multiprocessing, Melody/Pattern, AutoChords/Fingering, Style DNA, MIDI/transport/automation, FX/acoustic, presets/banks, GUI, state restore и FL Studio 20 acceptance.
- Статус: OPEN — REQUIREMENTS BASELINE; это не означает, что перечисленные функции реализованы.
- Приёмка: реализация + regression tests + Windows Release/CTest + pluginval + host test по применимости. Для каждого требования вести отдельный статус и доказательства.
- Форматный конфликт: Float32 WAV является согласованным требованием, но INFO содержит временную запись о 24-bit PCM. Проверить код и исправить несоответствие; до проверки не закрывать.


## IMP-002 — manifest SHA-256 и аудиометаданные

- Тип: улучшение целостности импортируемых библиотек.
- Изменение: SHA-256 для исходного файла и converted WAV; source extension, sample rate, channels, frames, duration, byte sizes и hash algorithm записываются в manifest.
- Regression: проверка вычисленных checksum, manifest metadata и NCW unsupported warning в SampleLibraryImporterTests.
- Статус: IMPLEMENTED IN SOURCE / AWAITING WINDOWS CI.
- Ограничения: ещё не реализованы UI для проверки целостности/repair, полное license inventory и тестовые аудиофайлы всех decoder formats.


## IMP-003 — OPEN — Windows compile failure при добавлении SHA-256
- **Дата:** 10.11.2026
- **Milestone:** M1/M2, importer integrity metadata
- **Commit:** `2a0f15b007e222ae5cbcfb05377b957d1d91ad04`
- **Симптом:** MSVC C2039: `SHA256` не является членом `juce` в `Source/Sampler/SampleLibraryImporter.cpp` (строка 376); последующие ошибки C3861/C2737/C3536/C2088 являются каскадными.
- **Доказательства:** Windows workflows 42868571549 и 42868571630 завершились FAILURE на compile step; CTest и pluginval не запускались.
- **Предполагаемая причина:** выбран API `juce::SHA256`, которого нет в используемой конфигурации/версии JUCE 7.0.12 или он не включён соответствующим заголовком. Root cause нужно подтвердить проверкой JUCE API и include/target configuration.
- **Исправление:** выбрать доступный переносимый SHA-256 API либо добавить самостоятельную проверенную реализацию; сохранить checksum source и converted WAV. Не отключать checksum и не удалять тесты.
- **Regression acceptance:** известный SHA-256 test vector; одинаковый результат для source/converted bytes; Windows x64 Release build; полный CTest; затем pluginval strictness 5.
- **Статус:** OPEN.

## IMP-001 — статус проверки
IMP-001 остаётся OPEN. В runs 42868571549 и 42868571630 сборка упала до CTest, поэтому ожидаемый результат importer test (3 imported / 2 mapped / 1 NEEDS MAPPING, Unknown_take.wav без выдуманного root key) не был перепроверен. Не закрывать до прохождения соответствующего regression test.

## IMP-002 — статус реализации
SHA-256 и аудиометаданные manifest добавлены в commit `2a0f15b007e222ae5cbcfb05377b957d1d91ad04`, но Windows compile gate не пройден из-за IMP-003. Статус: IN PROGRESS / CI BLOCKED; не считать принятым.

## Последняя подтверждённая Windows CI-проверка — 11.11.2026

Текущая ветка: `fix/m1-m2-m3-release-gates`  
Проверенный HEAD: `2e9356b729e3daba6a5c62c31fed7257072c6f12` — `test(import): verify SHA-256 against known vector`.

- Основной Windows VST3 workflow: [run 42871881934 — SUCCESS](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42871881934).
- Дополнительный Windows VST3 M1/M2 workflow: [run 42871881954 — SUCCESS](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42871881954).
- Основной workflow: Windows 2022 / MSVC x64 Release configure и VST3 build — SUCCESS; все 12 CTest regression tests — PASS (0 failed); VST3 bundle найден; pluginval strictness 5 — SUCCESS; package и artifact upload — SUCCESS.
- Дополнительный M1/M2 workflow: APVTS audit — PASS (91 уникальный ID, обязательные M1 controls, 7 FX slots); Release VST3 build, sampler unit/integration tests, bundle verification и artifact upload — SUCCESS.
- CTest включает SFZ parser, Pattern scheduler, Pattern/Sampler integration, SamplePool, page-cache slots, bounded SPSC queue, streaming service, sample metadata index, AutoChords, StyleEngine, SampleLibraryImporter и Mapping Editor model.
- Importer regression на этом HEAD проверяет ровно 3 imported / 2 mapped / 1 needs mapping; SFZ содержит только две автоматически сопоставленные regions (C3 и D4), а файл без root note остаётся без автоматически придуманного ключа. Также проверяются Float32 WAV conversion, SHA-256 известный вектор, source/converted hashes в manifest, аудиометаданные и NCW warning.
- Несколько более ранних runs со статусом `cancelled` были остановлены concurrency при новых push; это не считать failure. Исторические compile failures на HEAD `2a0f15b` исправлены и перекрыты успешными runs выше.

**Границы доказанного результата:** это подтверждает зелёную автоматизированную Windows build/test/package цепочку на HEAD `2e9356b`, но не закрывает M1–M5 как milestones. Остаются отдельные runtime/stress и host acceptance gates: полное сохранение/восстановление состояния и библиотеки, FL Studio 20 automation/save-reopen и transport acceptance, M2 velocity crossfade DSP/palm-mute/audio stress, а также полная M4/M5 runtime/preset binding. M3 остаётся NOT ACCEPTED. Stage 3 (M11–M13) не запускать до формальной приёмки M1–M5.

### Баги импортера после успешной проверки
- **IMP-001 — FIXED / REGRESSION PASS:** fixture подтверждает 3/2/1, два валидных SFZ regions и отсутствие выдуманного root key для unmapped take; CTest importer test прошёл в run 42871881934.
- **IMP-003 — FIXED / WINDOWS CI PASS:** недоступный `juce::SHA256` заменён переносимой реализацией SHA-256; известный вектор `abc` проверяется регрессионным тестом; Windows build прошёл.
- **IMP-002 — IMPLEMENTED / WINDOWS CI PASS:** manifest source/converted SHA-256 и аудиометаданные проверяются importer regression test. Это не означает, что все возможные внешние decoder files и license provenance уже прошли полноценную проверку.
