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
