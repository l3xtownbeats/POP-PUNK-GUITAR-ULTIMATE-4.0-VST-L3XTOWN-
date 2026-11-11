# M6–M13 — аудит фронта и быстрые задачи (10.11.2026)

## Принцип выполнения
M1–M5 остаются главным релизным потоком. M6–M13 выполняются только как изолированные низкорисковые задачи: документация, валидаторы, тестовые проверки и аудит контрактов. Не менять активные DSP/Processor файлы M1–M5 и не запускать тяжёлые параллельные сборки без необходимости. Ни одна задача M6–M13 не объявляется завершённой только по наличию документа или каталога.

## Сводный аудит M6–M13

| Модуль | Фактическое состояние на момент аудита | Быстрая задача | Статус |
|---|---|---|---|
| M6 Melody Engine | APVTS IDs для voices, velocity, palmMuteThreshold, articulation и melody modifiers уже присутствуют; реальный выбор артикуляционных сэмплов и FL Studio MIDI lifecycle ещё не доказаны | Сохранить параметрический контракт; добавить отдельные regression tests для note-on/off, velocity/articulation routing после стабилизации M2 | Частично; без ложного закрытия |
| M7 Factory Presets | Каталог 150 Melody + 150 Pattern встроен в проект; это authored definitions, не 300 готовых playable presets | Усилен валидатор: non-empty/unique IDs, обязательная палитра исполнителей, согласованность declared minimums, finite/ranged parameters, обязательные Pattern performance/sample requirement fields | Реализовано в этой ветке; ждёт CI |
| M8 Sample Library / streaming | Bounded cache, immutable snapshots, page leases, worker streaming и concurrent reader/eviction stress уже существуют | Не переписывать архитектуру; использовать существующие stress tests и расширять только при обнаружении конкретного пробела/дефекта | Базовые автоматические проверки есть; real-bank/DAW stress открыт |
| M9 Style Engine | StyleEngine foundation и Mest Photographs catalog targets есть в отдельной PR #3; runtime binding ещё не подтверждён | Сохранить изоляцию; интегрировать только после прохождения M1–M5 и через отдельный unified test | Частично; PR #3 |
| M10 GUI / FX | APVTS содержит FX switches/slots и параметры; утверждённая skin спецификация существует; фактическая функциональность всех контролов/эффектов и visual QA не подтверждены | Низкорисковый следующий шаг — статическая таблица parameter→UI control→processor binding; не менять GUI пока активна основная сборка | Аудит/документирование; runtime QA открыт |
| M11 Browser / Bank Manager | Форматы .ppgm/.ppgp примеров валидируются; полноценный browser, favourites, import/export, MIDI Learn и drag/drop не подтверждены | Усилен portable preset validator: object shape, non-empty unique IDs/names, bool/NaN/Inf rejection, parameter ranges, non-empty pattern steps | Реализовано в этой ветке; ждёт CI |
| M12 Release / CI | Windows x64 workflow, CTest, Pluginval strictness 5 и artifact packaging уже настроены; отдельные PR и integration builds ещё выполняются | Продолжать использовать существующие release gates, не дублировать тяжёлые workflows и не запускать лишние builds | Частично; CI проходит по разным heads, unified build открыт |
| M13 Clean install / minimum PC | Windows 10 x64 1809+ и FL Studio 20 — зафиксированный baseline; чистая установка и измерения на слабом ПК не подтверждены | Подготовить reproducible install/performance checklist; выполнить только на реальной Windows 10/FL Studio машине | Открыто; документация подготовлена |

## Изменения этой ветки

### M7 — Factory catalog validator
- Проверяет JSON root/array/object shape, declared minimum counts 150/150/300.
- Проверяет непустые уникальные artist names и наличие согласованной основной палитры: Blink-182, Sum 41, Simple Plan, New Found Glory, Good Charlotte, Mest, Angels & Airwaves, The All-American Rejects, MGK, Avril Lavigne.
- Проверяет непустые ID/name, уникальность ID, корректный mode, известного reference artist.
- Отклоняет bool, NaN/Inf и значения вне диапазона для drive/tone/reverb.
- Для Pattern требует groove, performance и sampleRequirement. Это не утверждает, что сами performance assets уже поставляются.

### M11 — Portable preset validator
- Проверяет JSON object, extension↔mode, format identifier, непустые ID/name и дубли ID.
- Проверяет numeric parameters без bool/NaN/Inf и границы значений.
- Для Melody требует melody object; для Pattern — непустой steps array.
- Это проверка формата, не доказательство завершённого GUI browser/import/export.

## Синхронизация с M1–M5
- M1: не трогаем APVTS state/lifecycle; M6 audit использует существующий параметрический контракт. Host save/reopen, automation, missing-library recovery и device-change остаются release gates.
- M2: streaming/cache stress остаётся частью основного sampler pipeline. Velocity crossfade PR #5 должна пройти собственный Windows build/CTest/Pluginval до переноса. Palm-mute sample mapping и click/pop stress остаются открытыми.
- M3: PatternScheduler→SamplerEngine module integration test уже включён в CTest и прошёл основной Windows run; FL Studio 20 transport/MIDI validation остаётся открытой.
- M4: AutoChords/FingeringGenerator foundation находится в PR #4; последние overflow-hardening commits ожидают свежий CI.
- M5: StyleEngine foundation находится в PR #3; runtime binding не считать завершённым.

## Правило интеграции
Не объединять M6–M13 ветку с основной, пока:
1. M7/M11 validator checks не пройдут CI на текущем head;
2. активные M1–M3, M2 crossfade и M4 runs не дадут итоговый статус;
3. PR #3/#4/#5 не будут портированы/проверены на одной интеграционной базе;
4. unified Windows x64 Release + full CTest + VST3 locate + Pluginval strictness 5 + package run не пройдёт.

После CI можно перенести только проверенные коммиты валидаторов и документации; не делать merge M6–M13 как будто это готовые runtime features.


## CI/PR snapshot — 11.11.2026

| Target | Current head | Windows workflow | Gate result | PR state |
|---|---|---|---|---|
| M1–M3 integration | `98d7c65f86427449cbd83968306f0388054b1aa3` | [42963660168](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42963660168) | Release configure/build, CTest, VST3 locate, Pluginval 5, cached library/license validation, package/upload: PASS | Base CI green |
| M4 PR #4 | `30e970f657fdeb6ee229004e9022149ed948b50b` | [42844161921](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42844161921) | All listed Windows gates: PASS on exact head | Draft; dirty base conflict |
| M5 PR #3 | `0cc36c7455edb3adcc5bde1f5bd690c831acf3cb` | [42840410645](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42840410645) | All listed Windows gates: PASS on exact head | Draft; dirty against main |
| M2 velocity PR #5 | `ef1ee5b04c3dc6653f749d7602b92a683253d059` | [42843931173](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42843931173) | All listed Windows gates: PASS on exact head | Draft; mergeable clean |
| M6–M13 PR #6 | `79df94351ccfee7b5e12b02608d6f98194cdb3ae` | [42879024401](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42879024401) | All listed Windows gates: PASS on exact head | Draft; dirty against current integration |

Main HEAD `cf851de3b6d63a19080d319cedd404e7e0440789` run [42996098554](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42996098554) had passed configure/build, full CTest, VST3 locate and Pluginval 5; package was still running and upload pending at snapshot time. Do not record final success until completion.

### Reviewed PR #6 diff
Exactly six paths: this audit, the M13 clean-install checklist, bank-library requirements, M1–M5 work log, factory-bank validator and portable-preset validator. No active DSP/Processor source is included. The integration branch has newer content in the bank-library and work-log files; those changes must be retained during refresh. The validator files were unchanged on integration relative to PR #6's base. PR #6 validator does not yet include the M5 Photographs assertion from PR #3, so combine the checks when M5 is integrated.

### Integration decision
Do not merge while PR #6 is dirty. Refresh its head on current integration without force-push, preserve newer documentation, then require a new full Windows workflow on that refreshed SHA. PR #3 and #4 remain blocked by dirty base state; PR #5 is independently green and clean but remains unmerged. No runtime milestone is closed by these validator/docs checks.

### Runtime gates still open
- M6: articulation-to-sample routing and MIDI note-on/note-off lifecycle.
- M8: real redistributable/user libraries and sustained streaming-cache stress.
- M9: StyleEngine runtime binding to Pattern scheduler and sample groups.
- M10: UI/FX parameter-to-control-to-processor QA.
- M11: actual browser, import/export, favorites and MIDI Learn behavior.
- M13: clean Windows 10 x64 installation and performance smoke test in FL Studio 20.

The Windows CI package run restores the cached redistributable sample library and validates its manifest/licenses; the library-download step is skipped. This is not evidence that new external libraries were downloaded. CI is not a substitute for manual FL Studio 20 validation.
