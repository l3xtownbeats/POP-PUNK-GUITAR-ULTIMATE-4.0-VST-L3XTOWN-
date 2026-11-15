# Журнал выполнения M1–M5 и интеграции — 10.11.2026

## Назначение и правило интеграции

Этот файл фиксирует фактические изменения, CI-доказательства, незакрытые критерии и порядок объединения модулей. Основная ветка работ M1–M3: `fix/m1-m2-m3-release-gates`. M4 и M5 выполняются отдельно, чтобы не менять исходники активной основной сборки и не добавлять нагрузку на её аудиопоток. Объединение разрешено только после прохождения собственных тестов каждого модуля и повторного общего Windows Release/CTest/pluginval.

## Подтверждённые результаты M1–M3

### Основная Windows VST3 сборка
- Workflow run: [42841128991](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42841128991)
- Commit: `2159d5fb05dba22f84d312310953422ffbb4eea5`
- Итог: SUCCESS.
- Подтверждены: x64 Release configure, VST3 и тестовые executable build, полный CTest workflow step, обнаружение VST3 bundle, pluginval strictness 5, packaging и upload.
- Artifact: `POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES-AND-PRESETS`, размер 1,664,195,877 bytes; дополнительный source-review artifact 586,529 bytes.
- Сборка скачивала библиотеку из cache; шаг нового download был skipped. Лицензионная проверка восстановленного sample library cache завершилась success.
- В workflow явно записано ограничение: тест в FL Studio 20 выполняется отдельно. Следовательно, CI success не доказывает host compatibility.

### Дополнительная M1/M2 Windows сборка
- Workflow run: [42841133299](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42841133299)
- Commit: `34f158e5e6fc162ca76bae9f9fc90eee36c6114d`
- Итог: SUCCESS.
- Подтверждены MSVC v143 configure, APVTS ID audit, Release VST3 build, sampler unit/integration tests, проверка VST3 bundle и upload.
- Artifact: `PPGU-v33-M1-Windows-x64-VST3`, 2,359,701 bytes.

### Остаточные критерии M1
- CI и исходниковые проверки состояния/параметров прошли.
- Ручной FL Studio 20 host test ещё не выполнен: save/reopen, missing-library recovery, automation, editor-close while loading, repeated library changes, device change и long playback.
- Поэтому M1 имеет статус **CI PASS / HOST VALIDATION OPEN**, а не полностью закрыта.

### Остаточные критерии M2
- CI сборка и sampler/streaming tests прошли; streaming page cache, bounded worker decode и существующие cache/streaming tests включены в проверенный Windows build.
- SFZ Round Robin parsing/selection и независимость RR sets уже имеют регрессионные тесты.
- Velocity crossfade opcode пока разобраны и опубликованы в runtime metadata, но это само по себе не означает DSP-смешивание двух velocity layers. Реальное velocity-layer crossfade playback остаётся открытым.
- Требуется отдельно доказать реальное поведение palm mute по velocity/articulation на целевой библиотеке, click/pop критерии на 100+ повторных атаках, concurrency/cache pressure и отсутствие audio-thread allocations/locks/I/O.
- Статус: **STREAMING/RR CI PASS; VELOCITY CROSSFADE + AUDIO/LOAD/HOST VALIDATION OPEN**.

### Остаточные критерии M3
- Scheduler unit/regression suite и Windows build прошли.
- В исходниках имеется transport-aware scheduling и передача событий sampler с sample offsets.
- Полный scheduler-to-sampler integration proof ещё требуется: реальные note ownership, chord transitions, stop/seek/loop, tempo changes, mode/slot switch, simultaneous input и MIDI NoteOff.
- FL Studio 20 validation не выполнен.
- Статус: **SCHEDULER CI PASS; END-TO-END MIDI/DAW VALIDATION OPEN**.

## M4 — AutoChords / FingeringGenerator

- Изолированная ветка: `work/m4-autochords-fingering`.
- Pull request: [#4](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/pull/4), draft/WIP, base `fix/m1-m2-m3-release-gates`.
- Commit на момент записи: `df722401d726a81ef3498607e68b4cd0f1b6268e`.
- Добавлен dependency-free `Source/Performance/AutoChords.h`: Major, Minor, Diminished, Augmented, Sus2, Sus4, Dominant7, Major7, Minor7, Power; bounded MIDI voicing и инверсии.
- Добавлен `FingeringGenerator` для стандартного строя E-A-D-G-B-E, ограниченного диапазона ладов и bounded назначения нот по струнам.
- Добавлены `tests/AutoChordsTests.cpp` и CTest target `ppgu_auto_chords_tests`.
- CI run [42842050531](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42842050531) на момент записи: configure success, build in progress; CTest/pluginval/package ещё не завершены.
- Это фундаментальный алгоритмический модуль, не завершённая интеграция AutoChords в Pattern UI/host MIDI. Не считать M4 закрытым до тестов scale/tonic mapping, voicing edge cases, scheduler integration и state persistence.

## M5 — Style Engine

- Изолированная ветка: `work/m5-style-engine-tests`.
- Pull request: [#3](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/pull/3), draft/WIP; исходный PR направлен в `main`, поэтому перед интеграцией нужно перенести изменения в согласованную интеграционную ветку M1–M3, а не сливать его напрямую в main.
- Проверенный commit: `0cc36c7455edb3adcc5bde1f5bd690c831acf3cb`.
- Workflow run [42840410645](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42840410645): SUCCESS, включая Windows x64 configure/build, CTest, VST3 locate, pluginval strictness 5 и packaging.
- Добавлены `Source/Performance/StyleEngine.h`, `tests/StyleEngineTests.cpp`, CTest target; обновлены M006/P006 catalog labels на Mest — PHOTOGRAPHS Lead/Rhythm и validator для обоих режимов.
- Engine применяет ограниченные velocity/accent transforms, chug/palm mute articulation, hammer-on/pull-off candidate rules, strum spread и FX taste.
- M5 пока не подключён к реальному Pattern Scheduler/processor/UI; требуется integration tests, determinism tests и preset-to-StyleDNA binding. Статус: **FOUNDATION CI PASS / RUNTIME INTEGRATION OPEN**.

## Правило объединения

1. Не сливать M4/M5 в основную ветку, пока их собственные CI gates не завершены успешно.
2. Перед merge исправить выявленные build/test/pluginval ошибки и повторить CI.
3. Перенести M5 изменения из PR #3 на ветку, основанную на `fix/m1-m2-m3-release-gates`; проверить diff, не затрагивая посторонние файлы.
4. Дождаться завершения M4 run; затем перенести M4 на ту же интеграционную линию.
5. Реализовать и протестировать runtime integration: AutoChords → Fingering → Pattern Scheduler → Sampler; StyleEngine → Pattern event/articulation pipeline.
6. Обновить русскоязычные журналы, blocker audit, production priorities, final checklist и release notes.
7. Запустить единую Windows x64 Release сборку, полный CTest, pluginval strictness 5, проверить package/artifact и сохранить commit/run/artifact identifiers.
8. Только после этого объединять в единый релизный branch. Отдельный ручной FL Studio 20 test остаётся release gate.

## Принцип достоверности

CI SUCCESS фиксируется только для перечисленных run/commit. Непроверенные DAW, слуховые, runtime-интеграционные и библиотечные критерии остаются OPEN. Наличие класса, заголовка, каталожной записи или успешной компиляции не считается доказательством законченной пользовательской функции.

## Update — 10.11.2026 (latest CI and implementation work)

### Main M1–M3 branch
- Windows x64 Release run `42843030370`, commit `4d13943f15352b87f3569524e2e5b4b8f9bbafda`: SUCCESS, including build, full CTest, VST3 locate, pluginval strictness 5, packaging and upload.
- Separate MSVC v143 run `42843030228`: SUCCESS.
- Added `tests/PatternSamplerIntegrationTests.cpp` / CTest target `ppgu_pattern_sampler_integration_tests`: generated WAV/SFZ fixture, host transport scheduling, sample-accurate NoteOn/NoteOff and audible SamplerEngine output.
- M1 remains CI PASS / FL Studio host validation OPEN. M2 streaming/RR tests pass; velocity crossfade is isolated in PR #5 pending CI. Palm mute and real-library stress remain open. M3 module integration test passes; FL Studio transport/MIDI validation remains open.

### M4 and M5
- M4 PR #4: AutoChords qualities/inversions, FingeringGenerator and Major/Natural Minor scale mapping passed run `42843127831` on commit `4327e54988bf42c87a86185b39a2ecd40609c3ae`. Subsequent integer-overflow guards in commits `eccb48010d0fe2f87109ed6074afecf89f0ab659` and `30e970f657fdeb6ee229004e9022149ed948b50b` require fresh CI.
- M5 PR #3: StyleEngine foundation passed run `42840410645` on commit `0cc36c7455edb3adcc5bde1f5bd690c831acf3cb`; runtime Pattern/Processor/UI binding remains open.
- M2 PR #5 branch `work/m2-velocity-crossfade`, latest commit `ef1ee5b04c3dc6653f749d7602b92a683253d059`, run `42843931173` in progress.

No secondary branch is merged yet. Unified integration requires current branch heads to pass, runtime binding to be completed, and a combined Release/CTest/pluginval/package run. FL Studio 20 remains a separate release gate.


## Ручная проверка пользователя — FL Studio 20 / VST3 (10.11.2026)

Пользователь подтвердил фактический smoke test на своей системе:
- FL Studio 20 запускается.
- VST3-файл плагина обнаруживается.
- Плагин загружается в FL Studio 20 и добавляется в проект.
- При нажатии MIDI-клавиши/ноты слышен звук инструмента.

**Результат:** базовый host-load + MIDI-trigger + audible-output smoke test пройден по подтверждению пользователя. Это важное подтверждение работоспособности реального VST3-хоста, но не закрывает все release gates M1–M3: сохранение/восстановление state, смену/перезагрузку библиотеки, повторные атаки и cache pressure, velocity crossfade/palm-mute articulation, транспорт/seek/loop и длительное воспроизведение ещё требуют отдельных проверок. Не приписывать этому тесту проверку Windows 10, если версия ОС отдельно не подтверждена.


## Update — 10.11.2026: M4/M5 runtime integration pass 1

- Перенесены Source/Performance/AutoChords.h и tests/AutoChordsTests.cpp из проверенной M4 ветки на интеграционную линию. Подключены тесты ppgu_auto_chords_tests в CMake.
- Перенесены Source/Performance/StyleEngine.h и tests/StyleEngineTests.cpp из M5 ветки. Подключены тесты ppgu_style_engine_tests в CMake.
- M4 первый runtime binding: в Pattern mode одиночная MIDI-нота при включённом singleMidiNoteAsChordRoot и выбранной Major/Minor scale преобразуется в scale-aware triad; voicing проходит через FingeringGenerator и ограничивается доступными позициями грифа.
- M5 первый runtime binding: Style DNA преобразует velocity/beat accents детерминированно в Pattern scheduler dispatch. Все данные фиксированные; дополнительной аллокации в audio callback не вводится.
- Важный незакрытый пункт M5: StyledEvent::articulation пока не маршрутизируется к отдельным palm-mute/hammer/pull sample groups; strumSpreadMs и fxTaste пока не управляют физическим таймингом голосов/FX. Не считать полную M5 интеграцию завершённой.
- Новый CI для интеграционного commit 66a6644dab84e4d34d0016d8f2933e880a72423c: run 42848061281 (на момент записи in progress). Итог CTest/Pluginval/package будет записан после завершения.
- M2 velocity crossfade branch run 42843931173 passed отдельно, но её реализация ещё не перенесена в интеграционную ветку. M1–M3 CI и пользовательский базовый FL Studio 20 audible MIDI smoke test остаются подтверждёнными; state/reopen, library reload и stress gates открыты.

## Следующие обязательные задачи перед объединением M1–M5

1. Получить текущий CI результат интеграционной ветки и исправить любые ошибки.
2. Закончить M4: тесты mapping/voicing edge cases и сохранить поведение MIDI root key; затем подключить chord/fingering к UI и state.
3. Закончить M5: articulation sample routing, event strum scheduling с bounded offsets, preset StyleDNA binding и end-to-end PatternSampler integration tests.
4. Перенести M2 crossfade на интеграционную ветку, включить тест суммарного сигнала crossfade midpoint и проверить velocity-zone/RR взаимодействие.
5. Запустить unified Windows x64 Release + полный CTest + VST3 locate + pluginval strictness 5 + package; только затем объединять M1–M5 в единую release ветку.
6. Провести дополнительные ручные тесты FL Studio 20: project save/reopen, MIDI multi-note, Pattern host transport, preset/library reload, долгий playback.


## 10.11.2026 — Audio Library Importer MVP

- Добавлен Source/Sampler/SampleLibraryImporter.h/.cpp: сканирование выбранной папки, распознавание декодируемых форматов через JUCE Basic AudioFormatManager и chunked conversion в IEEE float32 WAV.
- Конвертация сохраняет исходную sample rate и mono/stereo layout; исходники остаются нетронутыми. Все операции идут в worker pool.
- Добавлен async processor entry point importAudioDirectoryAsync() и UI-кнопка IMPORT AUDIO.
- Результат складывается в пользовательский Libraries folder, staging + atomic rename, manifest с provenance/license reminder, generated SFZ для файлов с note-name в filename.
- Файлы без root note сохраняются в BANK и отображаются как needing mapping; не назначаются автоматически на общий key, чтобы не создавать многосэмпловый конфликт.
- Добавлен тест tests/SampleLibraryImporterTests.cpp: WAV + AIFF fixtures, float32 output, key parsing, SFZ path validation, manifest and repeat import.
- CI runs на SHA e905ec43710b3a44232d6b839f35e306edf3c451: runs 42850028375 (build) и 42850028356 (Windows VST3) на момент записи ещё выполняются. Итог не подтверждён.
- Следующий этап: проверить текущий CI, исправить compile/test issues; затем добавить ручной Mapping Editor (key/velocity/articulation/RR), preview, rescan/remove, checksum and import cancellation. Не обещать поддержку форматов, не декодированных фактической сборкой.


## Build correction — 10.11.2026

Первый Windows CI importer выявил несовместимости с используемой версией JUCE: старый AudioFormatWriter не имеет Options API, а read() требует AudioBuffer overload. Исправлено:
- выбран совместимый 24-bit PCM WAV как внутренний формат (не IEEE float32);
- используется legacy WavAudioFormat::createWriterFor(stream, sampleRate, channels, 24, ...);
- AudioFormatReader::read(&buffer, ...) вызван через поддерживаемую сигнатуру;
- исправлены явные преобразования juce::String/std::string;
- кнопка переименована в IMPORT SOUNDS, основной UX — выбор целой папки через системный browser.
Повторный Windows CI запущен на новом HEAD; результат будет внесён отдельно. Предыдущий run 42850028356 FAILED и считается диагностическим, не релизным.


## Расширение форматов BANK SAMPLES — 10.11.2026

- Включены декодеры JUCE для FLAC, OGG Vorbis и MP3; WAV и AIFF/AIF остаются базовыми форматами.
- На кнопке IMPORT SOUNDS добавлена подсказка со списком поддерживаемых форматов и пояснением по NCW.
- В импортере NCW теперь получает отдельное предупреждение с маршрутом экспорта из Kontakt в WAV/AIFF; не считается поддерживаемым декодером PPGU.
- В тест добавлена проверка регистрации шести расширений: WAV, AIFF, AIF, FLAC, OGG, MP3.
- Внутреннее представление — IEEE Float 32-bit WAV; это преобразование выполняется в worker-потоке.
- Следующий gate: текущие Windows VST3 и build/CTest runs на новом HEAD. До успешного CI не отмечать поддержку как release-validated.


## 10 ноября 2026 — IMP-001: SFZ importer publication path fix
- Branch: `fix/m1-m2-m3-release-gates`.
- Commit: `39626076a8ded975bdfb280a39880bad66f42b4c`.
- Исправлена причина, по которой импортёр возвращал устаревший staging path после атомарного перемещения библиотеки в final directory. При наличии mapped regions итоговый SFZ path теперь строится от final directory.
- Существующий importer test покрывает existence/parse/region sample paths; требуется повторный Windows CI/CTest, результат пока не подтверждён.
- M1/M2/M3 остаются НЕ ПРИНЯТЫМИ; остальные gates без изменений.

## Единое правило соответствия промту и правкам пользователя
- Основной источник: `Promt.txt` v33 FINAL + `docs/MASTER_PRODUCT_SPEC_4_0_RU.md` + все более поздние прямые правки пользователя. Более поздняя пользовательская правка дополняет/уточняет план, но не отменяет обязательные технические критерии промта, если явно не сказано обратное.
- Ресурсный план: M1–M5 — главный этап; M6–M10 — только лёгкая независимая работа в фоне; M11–M13 — только безопасная подготовка до завершения этапа 1.
- Зависимости v33 сохраняются: M1 первой; зависимые milestones не перепрыгиваются; M4+ не интегрируются в основной runtime до приёмки M1–M3.
- Ни один пункт не отмечается готовым по наличию кода: нужны тесты, актуальный Windows Release/CTest/pluginval и ручная проверка там, где она предписана.


## 10.11.2026 — baseline обязательной реализации согласованных функций

Пользователь подтвердил, что весь ранее согласованный функциональный объём должен быть реализован. Создано единое ТЗ `docs/ТЗ_ПОЛНАЯ_РЕАЛИЗАЦИЯ_СОГЛАСОВАННЫХ_ФУНКЦИЙ_RU.md`; README, полный перечень функций, jurnal.md, Info.txt и bug/requirements journal синхронизированы. Это фиксация требований, не runtime-изменение и не приёмка milestone.

Для основной линии M1–M5 сохраняется прежний порядок и gate. Все новые реализации должны получать regression tests и проходить Windows x64 Release/CTest; pluginval и ручные проверки FL Studio 20 — по применимости. M6–M10 остаются только лёгкой подготовкой, M11–M13 не интегрировать до принятия M1–M5.

Открытая техническая сверка для следующего аудита: продуктовый контракт требует Float32 WAV после импорта, тогда как историческая запись фиксирует 24-bit PCM workaround. Проверить writer и fixture на реальном HEAD, согласовать код/тесты с Float32, не менять формат молча.


## 10 ноября 2026 — IMP-002: контрольные суммы и метаданные импорта

- В importer добавлен SHA-256 исходного файла и конвертированного WAV для каждой успешно импортированной записи manifest.
- Manifest теперь фиксирует алгоритм checksum, исходное расширение, sample rate, число каналов, количество frames и duration, а также source/converted byte sizes.
- Хеширование выполняется во время фонового импорта; аудиопоток не затрагивается.
- Расширен regression test: проверяются присутствие и фактическое совпадение SHA-256 для source/converted sample, аудиометаданные manifest и предупреждение об отклонении NCW.
- Статус: IMPLEMENTED IN SOURCE / AWAITING WINDOWS CI. Проверка не закрывает поддержку реальных FLAC/OGG/MP3 fixture playback, Mapping Editor и license verification.


## 10.11.2026 — производственный порядок и CI-blocker IMP-003
- Основная линия: M1–M5, без переключения приоритета на тяжёлые M6–M10 работы.
- Параллельная работа M6–M10 ограничена лёгкими аудитами/подготовкой; M11–M13 ждут приёмки M1–M5.
- Объединять связанные кодовые изменения, regression tests и журналы в компактные пакеты. Проверки не отключать.
- HEAD `2a0f15b007e222ae5cbcfb05377b957d1d91ad04`: оба Windows workflows 42868571549 и 42868571630 упали на compile step из-за `juce::SHA256` (C2039) в SampleLibraryImporter.cpp. CTest/pluginval/package пропущены.
- IMP-003 открыт на исправление SHA-256 API. IMP-001 остаётся OPEN, так как тест импортера не запускался. IMP-002 не принят до зелёного Windows build и regression suite.
- Следующий шаг: исправить IMP-003 без удаления checksum; добавить известный SHA-256 vector test; повторить Windows Release build + CTest. После успеха продолжить M1→M2→M3, затем M4/M5 и host acceptance в FL Studio 20.

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


## 10 ноября 2026 — M6–M13 lightweight parallel work

- Создана изолированная ветка `work/m6-m13-quickwins` от актуальной M1–M3 integration line; активные DSP/Processor файлы не затрагивались.
- M7: `tools/validate_factory_bank.py` усилен проверками 150/150/300 declared minimums, core artist palette, уникальных непустых IDs, структуры и mode, конечных числовых параметров/диапазонов, Pattern performance/sample requirement metadata.
- M11: `tools/validate_preset_files.py` усилен проверками object shape, extension/mode, ID/name, дублей, bool/NaN/Inf и диапазонов параметров, Pattern steps.
- M8: аудит подтвердил, что bounded page cache уже имеет concurrent reader/worker eviction stress test; новую конкурирующую реализацию не добавляли.
- M6: APVTS contract уже содержит voices/velocity/palmMuteThreshold/articulation IDs; реальное articulation-to-sample routing и FL Studio validation остаются открытыми.
- M9: StyleEngine foundation остаётся в PR #3, runtime binding не объявлен готовым.
- M10: утверждённый skin и FX APVTS controls зафиксированы; parameter→UI→processor QA остаётся открытым.
- M12: существующий Windows Release/CTest/pluginval/package pipeline используется повторно без запуска лишних сборок.
- M13: добавлен `docs/M13_CLEAN_INSTALL_SMOKE_CHECKLIST_RU.md` для Windows 10 x64 1809+/FL Studio 20, измерений cold start, RAM, CPU, preset switching и sustained playback.
- Полная карта статусов: `docs/M6_M13_QUICK_WINS_AUDIT_RU.md`. Изменения ждут CI и не объединяются до прохождения проверок M1–M5 и unified release gates.


## 11.11.2026 — CI/PR audit before M6–M13 integration

### Exact branch heads and CI evidence
- Integration branch `fix/m1-m2-m3-release-gates`: HEAD `98d7c65f86427449cbd83968306f0388054b1aa3`; Windows workflow run [42963660168](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42963660168) — SUCCESS on this exact SHA. Windows x64 Release configure/build, full CTest, VST3 bundle locate, Pluginval strictness 5, redistributable sample-library/license validation, package and artifact upload all completed successfully. Artifact `POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES-AND-PRESETS` is present (1,664,278,986 bytes); source-review artifact is also present (699,739 bytes). Library download step was skipped because the redistributable cache was restored.
- Main branch HEAD `cf851de3b6d63a19080d319cedd404e7e0440789`: run [42996098554](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42996098554) was still packaging at the time of this snapshot. Configure/build, CTest, VST3 locate, Pluginval 5 and license validation had passed; package step was in progress and upload had not completed. Do not record main as final PASS until the run concludes.
- PR #3 M5: head `0cc36c7455edb3adcc5bde1f5bd690c831acf3cb`; run [42840410645](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42840410645) — SUCCESS on exact head, all listed Windows release gates passed. PR remains draft and `mergeable_state=dirty`; base is `main`, whose current head has advanced. Do not merge until base/head conflict is resolved and CI reruns on the refreshed head.
- PR #4 M4: head `30e970f657fdeb6ee229004e9022149ed948b50b`; run [42844161921](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42844161921) — SUCCESS on exact head, all listed Windows release gates passed. PR remains draft and `mergeable_state=dirty`; current diff is limited to CMake test registration and AutoChords/FingeringGenerator header/tests. Refresh/resolve base before merge.
- PR #5 M2 velocity crossfade: head `ef1ee5b04c3dc6653f749d7602b92a683253d059`; run [42843931173](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42843931173) — SUCCESS on exact head, all listed Windows release gates passed. PR remains draft; GitHub currently reports `mergeable_state=clean`. Its diff is limited to SamplePool/SFZ velocity-crossfade implementation and regression tests. No merge performed in this audit.
- PR #6 M6–M13 quick wins: head `79df94351ccfee7b5e12b02608d6f98194cdb3ae`; run [42879024401](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42879024401) — SUCCESS on exact head, all listed Windows release gates passed. PR remains draft and `mergeable_state=dirty` against current integration head. Current PR diff has exactly six files: `docs/BANK_LIBRARY_SYSTEM_RU.md`, `docs/M13_CLEAN_INSTALL_SMOKE_CHECKLIST_RU.md`, `docs/M6_M13_QUICK_WINS_AUDIT_RU.md`, `docs/WORK_LOG_M1_M5_INTEGRATION_RU.md`, `tools/validate_factory_bank.py`, `tools/validate_preset_files.py`. No DSP/Processor source files are in this PR.

### Conflict and preservation findings
- Integration HEAD is `98d7c65f86427449cbd83968306f0388054b1aa3`; PR #6 was based on an older integration snapshot. Its bank-library and M1–M5 work-log documents have newer independent additions on the integration branch. These must be preserved during base refresh; do not overwrite them with the older PR versions.
- The two validator files are unchanged between PR #6's recorded base and current integration HEAD, so their M6 hardening can be transferred without replacing newer integration edits. The new M6 audit and M13 checklist are absent from integration and can be added as new documentation files.
- PR #6 factory validator does not include the separate M5 “Mest Photographs” assertion from PR #3. Preserve/merge that requirement when M5 is integrated; do not silently treat the M6 validator as a replacement for M5 coverage.
- PR #3 and PR #4 are currently dirty; PR #5 is clean. No PR was closed or merged in this audit. No branch was deleted and no force-push was used.

### Decision and remaining gates
- Refresh PR #6 against the exact current integration HEAD, preserving newer documentation and carrying only its six reviewed documentation/validator paths. Re-run the complete Windows workflow on the refreshed PR head before merging.
- Keep PR #3/#4 isolated until their dirty base conflicts are resolved and fresh-head CI passes. PR #5 remains separately validated but unmerged.
- Do not describe M6–M13 runtime work as complete. Open runtime/host gates: M6 articulation-to-sample routing and MIDI lifecycle; M8 real-library and streaming-cache stress; M9 StyleEngine runtime binding; M10 UI/FX parameter-to-control QA; M11 real browser/import/export/favorites/MIDI Learn; M13 clean install and performance smoke test on Windows 10 x64 + FL Studio 20.
- CI success does not replace FL Studio 20 manual validation. No redistributable library was downloaded in this run; only the cached library manifest/license validation completed.
