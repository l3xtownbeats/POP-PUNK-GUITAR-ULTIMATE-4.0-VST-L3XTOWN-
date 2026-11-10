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
