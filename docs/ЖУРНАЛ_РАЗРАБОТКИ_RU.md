# POP PUNK GUITAR ULTIMATE REV 6.0 VST3 — журнал разработки

Обновлено: 11 ноября 2026 г.
Репозиторий: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-
Рабочая ветка: main (подтверждена пользователем)

## Цель

Полноценный Windows x64 VST3-инструмент pop-punk guitar для FL Studio 20. Приоритеты: низкая нагрузка CPU и быстрый запуск; MIDI-клавиатура; SFZ/WAV-библиотека с velocity layers; palm mute и другие артикуляции; акустический режим; polyphony/voices; эффекты; удобный браузер сэмплов и превью; factory presets. Все внешние библиотеки проверяются на право включения и распространения сэмплов внутри плагина.

## Подтверждено CI до последней серии тестовых изменений

Workflow run 42703315483:
https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42703315483

Проверенный commit: c91548943f674da6cabc9ab4378403fa4208ef12
Результат: SUCCESS.
- CMake configure Windows / MSVC v143.
- Аудит 84 APVTS parameter IDs.
- Release VST3 build.
- Два CTest executable прошли.
- VST3 bundle verification и upload artifact.

Артефакт run 42703315483:
- Artifact ID: 13713191025.
- SHA-256: 36be5fae619907bc4d58220322bfcbfc069056edd6c89d231caf1c4266553cca.
- Ссылка: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42703315483/artifacts/13713191025

## Что реализовано в M2 sampler core

- SFZ global/group/region inheritance.
- Key, velocity и MIDI channel zones.
- group/off_by metadata для choke-поведения.
- tune, volume, pan, sample offset/end, release.
- WAV decode с ограничением длины до 120 секунд и проверкой sample paths против выхода из папки библиотеки.
- Snapshot-based publication; дисковая загрузка не должна выполняться в аудиопотоке.
- Voice allocation с лимитом до 64 голосов, выбор региона по MIDI note/velocity/channel.
- Playback rate по sample rate и высоте MIDI ноты.
- Loop modes: loop_continuous, loop_sustain, loop_pingpong, one_shot.

## Улучшение тестов после CI 42703315483

Добавлены интеграционные проверки:
- ping-pong region выбирается по MIDI channel 3;
- ping-pong продолжает играть через границы цикла;
- после note-off ping-pong выходит из цикла и освобождает голос;
- one-shot игнорирует note-off и заканчивает воспроизведение на конце WAV.

Добавлена проверка ping-pong loop metadata в SFZ parser tests.

При ревизии обнаружено, что стандартные assert в parser tests могли отключаться в Release build через NDEBUG. Заменены на CHECK-макрос, который проверяет выражения и завершает тест с сообщением об ошибке независимо от конфигурации сборки. Исправлена строка SFZ fixture с newline.

Коммиты тестовых изменений:
- 9108c8a9a50a8b8c4b1de5918feb1de983b38a08
- 68df97c6925d32f76e130f9af5ae768a464beace
- 5e1d5791219fae16fb8e12b21d442dccb988c8b9
- b6a06fc70c9a494ad9d16ba2c55bd1488439a13c
- 5ddccc4431c431674707848e9c8aa83daad22ba7
- 1ed52713bd1a976a52de604706750a8d94dbf002

Эти последние изменения требуют успешного повторного CI. Не считать их прошедшими, пока Actions run для актуального HEAD не завершится SUCCESS.

## Известные ограничения — проект ещё не готов к релизу

- GUI пока отсутствует: hasEditor() возвращает false, createEditor() не предоставляет пользовательский интерфейс.
- Нет пользовательского SFZ browser/load workflow, хотя загрузочный API есть.
- Временный oscillator/sine fallback остаётся в processor и не является гитарным тембром.
- В репозитории пока нет проверенной и включённой в дистрибутив лицензированной гитарной библиотеки.
- Параметры FX присутствуют в APVTS, но production DSP chain не завершена.
- Не завершены пресеты, acoustic mode, palm-mute response по velocity, полноценные гитарные артикуляции, preview и сохранение состояния выбранной библиотеки.
- Нужны дополнительные stress tests для одновременных нот, repeated library reload, file boundary conditions, long playback, CPU, memory и concurrency.

## Следующие этапы

1. Дождаться CI для актуального HEAD, проверить CTest, VST3 bundle и artifact.
2. Устранить все ошибки CI и расширить edge-case tests.
3. Провести аудит thread-safety и поведения при repeated SFZ reload.
4. Проверить доступные SFZ/WAV guitar libraries и их точные лицензии; ничего не включать без явного права на redistribution.
5. Реализовать browser/load/preview workflow.
6. Интегрировать легальные guitar samples и заменить sine fallback корректным поведением.
7. Реализовать velocity layers, palm mute, acoustic mode и articulations.
8. Реализовать FX DSP, voices control, preset system и лёгкий GUI.
9. Проверить в FL Studio 20 на Windows 10 x64, включая project save/restore, overlapping MIDI notes, sustained notes и CPU.


## Дополнительная проверка Windows CI — 10 ноября 2026

Run 42704926004: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42704926004  
Проверенный commit: 655126379811d26af3011601d1d094b714e2b81e  
Статус: SUCCESS.
- Build Release VST3 — успешно.
- Sampler unit/integration tests — успешно.
- Проверка VST3 bundle — успешно.
- Upload artifact — успешно.
- Artifact ID: 13712199557.
- SHA-256: 88c73778ea28b1178f1016d8a69c78f6ac1b533bbd2142912c18805e5c4e4233.
- Ссылка: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42704926004/artifacts/13712199557

Этот CI подтвердил тесты ping-pong и one-shot, а также Release-safe CHECK assertions в SFZ parser tests.

## Новые исправления после run 42704926004 — ожидают CI

- SamplePool::loadSfz теперь декодирует новые WAV-активы во временное хранилище и переносит их в долгоживущее хранилище только после успешной проверки всей SFZ-карты. Это устраняет накопление новых буферов при неудачной загрузке карты, в которой ранние WAV корректны, а последующий файл повреждён или недопустим.
- Добавлен интеграционный сценарий: при попытке загрузить карту с валидным WAV и затем повреждённым WAV загрузка должна завершиться ошибкой, а ранее активная карта должна остаться доступной.
- Удалён повторный include из SFZ parser tests.

Коммиты после последнего успешного CI:
- ef86d28d9c6d668edaadeaed4c32aca53bd05b39 — transactional SFZ asset loading.
- 7e10c0a1204604fce5c00513cdc783a54a2ef83a — regression test for failed reload.
- 389031671e1c7b995c7bec3d2e847729496b92b7 — fixture newline correction.
- 70e1da6fb6f1b5ad9f5db143f6a7e9b66d526765 — duplicate include cleanup.

Статус: эти коммиты ещё не подтверждены CI. Не считать transactional reload исправление принятым до успешного CTest на актуальном HEAD.

## Параллельная проверка полной упаковки

Run 42704820544 использует отдельный workflow vst3-build.yml и исходный архив source_bundle.zip. Run 42704820544 завершился SUCCESS:
https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42704820544
- Валидация каталога 300 factory preset entries и примеров .ppgm/.ppgp — успешно.
- Извлечение source bundle и подготовка preset families — успешно.
- Загрузка Meta Open Sound Steel/Nylon и FreePats Clean/Distortion 1/2 — успешно.
- Windows x64 VST3 build — успешно.
- pluginval strictness 5 — успешно.
- Упаковка и публикация — успешно.
- Итоговый artifact ID: 13713213374.
- Имя: POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES.
- Размер ZIP: 2 243 364 667 байт (около 2,24 GB).
- SHA-256: 66306732d5ca9fa0bcdf6472976494fbe0bbe203d937cbbba50572b6fb1c0dae.
- Ссылка: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42704820544/artifacts/13713213374

Важно: этот workflow собирает source_bundle.zip версии 3.1.0, а не напрямую текущие файлы Source/Sampler из M2-ветки. pluginval strictness 5 подтверждает валидность VST3 по этому тесту, но не доказывает загрузку упакованных библиотек в runtime и не заменяет FL Studio 20 host test. Размер ZIP около 2,24 GB также требует пересмотра упаковки: разделить optional sample packs и не заставлять пользователя скачивать все библиотеки сразу.

Проверка source-review artifact выявила несовпадение путей: source bundle ищет Meta SFZ в AppData/Samples/SourceLibraries и electric WAV в AppData/Samples/Electric/Open и PalmMute, а workflow упаковывает библиотеки в отдельные каталоги Meta-* и FreePats-* рядом с VST3. Поэтому пакет содержит WAV и лицензии, но текущая структура установки не гарантирует, что плагин их обнаружит. Подробный аудит: docs/АУДИТ_ИНТЕГРАЦИИ_RU.md.



## Regression CI и исправление тестового API — 10 ноября 2026

Run 42706103300 завершился FAILURE на этапе сборки тестов, а не самого VST3:
- Release VST3 был успешно скомпилирован и установлен в staging.
- Сборка SFZ parser tests прошла.
- SamplePoolTests не скомпилировались: тест вызывал getRegionCount()/getSampleCount() у SamplerEngine, но эти диагностические методы были доступны только у внутреннего SamplePool.
- Из-за ошибки компиляции CTest и публикация артефакта были пропущены. Этот запуск не является успешной проверкой последних изменений.

Исправление:
- В SamplerEngine добавлены read-only методы getRegionCount() и getSampleCount(), делегирующие запросы внутреннему SamplePool. Это диагностические методы без изменений поведения audio callback.
- Изменения: f4156bdb27e57217bbe741dc81877d4c7eb25ef5 (объявления), 629215a084b3357d7ce448f200b9c1b902e2b2bf (реализация).
- Новый CI run 42706779431 проверяет актуальный commit 629215a084b3357d7ce448f200b9c1b902e2b2bf. Статус при записи: queued; результаты тестов ещё не подтверждены.

Следующий критерий принятия:
- Release VST3 compile;
- parser tests, включая default_path с пробелами и traversal rejection;
- sampler tests, включая failed reload transactional guarantee;
- VST3 bundle verification и artifact upload.



## Дополнительный аудит SFZ/WAV входных данных — 10 ноября 2026

После успешного run 42706779431 продолжена проверка граничных и потенциально опасных значений внешних SFZ/WAV файлов.

Найдены и закрыты в коде следующие риски:
- SFZ `tune` принимал любое конечное float-значение. Экстремальная расстройка могла привести к переполнению `pow()` при вычислении playback increment.
- SFZ `volume` принимал экстремальные конечные значения. При переводе dB в линейный gain это могло привести к бесконечному коэффициенту и испорченному аудиовыходу.
- WAV reader проверял длину и количество каналов, но не валидировал конечность и разумный диапазон sample rate до расчёта лимита длины и выделения буфера.

Исправления:
- `tune` теперь ограничен диапазоном -9600..+9600 cents; выход за границы приводит к parse error.
- `volume` теперь ограничен диапазоном -144..+24 dB; выход за границы приводит к parse error.
- WAV sample rate теперь должен быть конечным и находиться в диапазоне 8–384 kHz; нулевые/неразумные частоты отклоняются до выделения AudioBuffer.
- Добавлены regression cases на tune=9601, volume=25 и tune=nan.

Коммиты:
- 42411db4161a66bb0f118837300c357ff7be3584 — SFZ tune/volume bounds.
- dc3ba36ed80ae3746d183a73576390769cd428b0 — regression tests.
- e9dede484a168df2adbf55718578dce6cd3cb653 — исправление newline-экранирования новых test fixtures.
- 0950219dd2de0054462c900772b392911cdf3f66 — WAV sample-rate validation.

Важное ограничение:
- Эти новые коммиты пока не прошли Windows CI. Run 42706840707 относится к более раннему HEAD 8eca9aaf5945c68b31a26d69bdeea667858f7803 и не подтверждает эти последние изменения.
- В SamplePool snapshots и sample assets намеренно живут до уничтожения SamplePool, чтобы raw pointers голосов были безопасны на audio thread. Это предотвращает use-after-free, но означает рост памяти при каждой успешной перезагрузке SFZ. Нужна отдельная безопасная схема deferred reclamation / bounded snapshot lifetime; освобождать эти буферы прямо на audio thread нельзя.
- WAV ограничен по длине каждого файла 120 секундами, но суммарный RAM budget всей карты ещё не реализован. Это отдельная задача перед большими библиотеками.

Следующие проверки:
1. Дождаться текущего workflow и затем проверить актуальный HEAD с новыми safety guards.
2. Добавить тесты на безопасную обработку symlink escape и malformed WAV metadata, где поддерживается Windows runner.
3. Разработать bounded asset cache / deferred reclamation без аллокаций и освобождения памяти на audio callback.
4. Подключить M2 engine к production PluginProcessor; пока это отдельный тестируемый sampler module, не утверждать, что он уже управляет звуком в конечном GUI-плагине.



## Ограничение RAM и срока жизни сэмплерных snapshot — 10 ноября 2026

Продолжен аудит памяти M2 SamplePool. Ранее обнаруженная проблема: immutable snapshots и WAV assets сохранялись до уничтожения SamplePool, чтобы голоса аудиопотока не ссылались на освобождённую память. Без ограничений это могло приводить к неограниченному росту RAM при повторной загрузке SFZ.

Добавлены защитные пределы:
- Максимум 1 GiB декодированных WAV-данных на экземпляр плагина.
- Максимум 128 успешно опубликованных SFZ snapshots за срок жизни экземпляра.
- Проверка лимита выполняется до выделения AudioBuffer для нового WAV.
- Переполнение бюджета или достижение лимита возвращает ошибку загрузки; действующая карта сэмплов остаётся активной.
- В SamplePoolTests добавлен сценарий многократной перезагрузки одной SFZ-карты и проверки отказа на следующей загрузке после достижения лимита.

Изменения:
- c338287d0a817fb373e31c846fd98e0774acd7c2 — объявлены пределы cache/snapshot.
- e35445583e5933ffc2ad1ad45f9ee9fefbd82da9 — учёт памяти и проверка бюджета перед выделением.
- 543571f689ba27935f54114379f641aed566120b — тест на ограничение количества snapshot.
- bc1320216ff4c46476fe74da627cf27d8c902bbe — исправлены диагностические строки теста.

Это временная безопасная политика, а не окончательное решение управления памятью: после 128 успешных загрузок банк нельзя перезагрузить без перезапуска плагина. Правильное дальнейшее решение — deferred reclamation с аудио-безопасным освобождением старых snapshots, без mutex, allocation и deallocation в processBlock. До внедрения такого механизма нельзя просто удалять старые snapshots при загрузке.

Последняя проверка:
- Run 42707938630 был запущен на коммите 543571f689ba27935f54114379f641aed566120b; следующий commit с исправлением диагностических строк создаёт более новый запуск. Результат тестов на актуальном HEAD ожидается отдельно.


## Первый рабочий экран M2 — загрузка SFZ и основные регуляторы — 10 ноября 2026

Добавлен минимальный JUCE Editor в M2 ветку:
- Кнопка LOAD SFZ открывает системный выбор файла `.sfz`.
- После загрузки показывает количество регионов и WAV-файлов либо первую диагностическую ошибку.
- Добавлены привязанные к APVTS регуляторы MASTER и VOICES.
- Чёрно-красная базовая тема, размер окна 560×300.
- Processor теперь сообщает хосту, что Editor существует, и возвращает новый Editor.
- CMake включает PluginEditor.cpp/.h.

Это функциональный инженерный экран для проверки загрузки SFZ, а не финальный художественный интерфейс и не полный preset browser. Важно: текущая загрузка SFZ выполняется синхронно в callback выбора файла; большие библиотеки могут временно задержать интерфейс. Следующий шаг — перенести декодирование в управляемый worker и публиковать готовый snapshot без блокировки audio callback, затем добавить browser/preset navigation и штатное сохранение выбранного банка.

Изменения:
- 89ee1ffda68387a0a2d601a0b8682dd6f9f3e38e — PluginEditor.h.
- 010908b6b04ba8f7c64c1a1c6c0aee2ed2060f32 — PluginEditor.cpp.
- 028e32ae53b848fca0bf252c51bd59261867f237 — подключение Editor в CMake.
- ddffca1766aa9d0eff759c923d137667d2d84291 — hasEditor=true.
- 09e16ebc6b386d3c9f7ddde4414824d220d17523 — createEditor связывает UI с processor.

Также в WAV metadata validation добавлен отказ от файлов с числом каналов больше 64 до narrowing cast в int (commit a24396e453ef7c9aa184643beae6f800bc8ecf83).

Все эти изменения ожидают проверки Windows CI на актуальном HEAD. Не считать Editor собранным до успешного workflow и ручного открытия GUI в FL Studio 20.


## Удаление синусоидального fallback из production MIDI path — 10 ноября 2026

В ходе аудита обнаружено, что PluginProcessor при отсутствии подходящего SFZ sample zone создавал синусоидальный тон. Это было допустимо только как временная проверка MIDI на раннем M1 skeleton, но недопустимо для гитарного инструмента: пользователь мог принять синус за готовый звук гитары.

Изменения:
- MIDI Note On теперь запускает только SamplerEngine.
- Если для ноты/velocity/channel нет подходящего SFZ region, выход остаётся тишиной — синтетический тон не подмешивается.
- Удалены oscillator phase/increment/gain и связанное с ними состояние.
- Panic fade остаётся на уровне sampler voices.

Коммиты:
- c18c05cabc4b395a0df881cdb85e02105e76191e — удаление sine fallback из processBlock.
- af4d3f8bf029472c1ff12f8da6827bfdbbe2ed78 — удаление неиспользуемых oscillator полей.
- 670b9f768350bfc56e811ea6c14fc4e2bf05b665 — удаление остаточного panic-state assignment и неиспользуемого channel local.

Следствие: для слышимого гитарного звука теперь обязательно загрузить совместимую SFZ-карту с WAV-файлами. Это честное поведение инструмента, а не заглушка, выдаваемая за гитарный тембр.


## Результат Windows CI для SFZ Editor и M2 sampler — 10 ноября 2026

Workflow 42708571627 завершился SUCCESS на commit `f359e338e70e15cc021feecc5767b57d9397032c`.

Подтверждённые шаги:
- CMake configure под MSVC v143 — success.
- APVTS parameter ID audit — success.
- Release Windows x64 VST3 build — success.
- Sampler unit + integration tests — success.
- VST3 bundle verification — success.
- Artifact upload — success.

Артефакт:
- `PPGU-v33-M1-Windows-x64-VST3`
- Artifact ID: 13715276654
- Размер: 2,125,310 bytes
- SHA-256: `c209879e9ec530bbe2ce56e74fcccb49487cd5d324203fb99f2aad775aca2308`
- Workflow: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42708571627

Ограничения результата:
- Это небольшой VST3 build artifact, не полный дистрибутив с гигабайтной библиотекой семплов.
- CI подтвердил компиляцию и тесты sampler/editor, но не ручную проверку GUI и звучания в FL Studio 20.
- Артефакт собран на commit f359e338..., который содержит JUCE 7-compatible font constructor. Последующие изменения документации не меняют бинарный код.
- Следующий build требуется после следующих изменений runtime-кода.



## Начало подключения реального FX DSP — distortion stage — 10 ноября 2026

После успешной сборки UI и сэмплера начато подключение DSP, а не только параметров APVTS.

Реализован первый эффект:
- Distortion On включает/выключает стадию.
- Distortion Drive управляет коэффициентом soft clipping.
- Distortion Mix смешивает dry/wet.
- Обработка выполняется in-place после сэмплера и до Master.
- Внутри processBlock нет выделения памяти; обработка работает по существующему буферу.

Commit:
- d9cf749ecaf3f1e1596abcb262f27819b40857ec — distortion DSP stage.

Ограничение:
- Это первый базовый soft-clipping stage, не моделирование конкретного усилителя/кабинета.
- Distortion Tone пока не подключён.
- Chorus, delay, reverb, compressor, EQ и cabinet IR пока остаются параметрами без завершённой DSP-реализации.
- Изменение ожидает нового Windows CI; не считать его собранным до результата workflow.

## Фоновая загрузка SFZ без блокировки интерфейса — 10 ноября 2026

После успешного Windows CI для distortion stage начата следующая задача: перенос декодирования WAV/SFZ с message thread на отдельный worker.

Изменения:
- В Processor добавлен собственный однопоточный JUCE ThreadPool.
- Editor теперь отправляет загрузку в worker, показывает статус `Loading SFZ in background...` и временно блокирует кнопку повторной загрузки.
- После завершения результат возвращается на message thread через `MessageManager::callAsync`; обновление UI защищено `Component::SafePointer`, поэтому закрытие окна не приводит к обращению к уничтоженному Editor.
- Деструктор Processor дожидается завершения активных задач worker pool до уничтожения SamplerEngine, чтобы фоновый WAV decode не обращался к освобождённой памяти.
- Существующая транзакционная публикация SFZ snapshot и запрет блокировок/аллокаций в processBlock сохранены.

Статус:
- Изменения отправлены в ветку `feat/m2-sfz-loop-playback`.
- Windows CI для этих новых изменений ещё не завершён; не считать фоновую загрузку подтверждённой до успешной компиляции и тестов.
- Это убирает синхронное декодирование с UI thread, но не реализует отмену загрузки при закрытии проекта/плагина: при уничтожении Processor он безопасно дожидается worker.

Следующий шаг:
- Проверить Windows VST3 build и sampler tests, затем добавить regression coverage для повторных запросов/закрытия Editor.
- После этого реализовать реальный chorus/delay/reverb DSP и продолжить артикуляции palm mute.

Последний успешный build до этих изменений:
- Run 42709227702, commit `d9cf749ecaf3f1e1596abcb262f27819b40857ec`, SUCCESS.
- Artifact ID: 13714867260; размер 2,125,361 bytes.
- Ссылка: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42709227702/artifacts/13714867260.
- Подтверждены Release VST3 build, sampler tests, bundle verification и artifact upload. Этот CI не проверяет последующие фоновые изменения.

## Фоновая загрузка SFZ — Windows CI SUCCESS — 10 ноября 2026

Изменения асинхронной загрузки SFZ прошли Windows CI:
- Run 42709935766 — SUCCESS.
- Commit: `e8e4a83ad93af777dca5ec111c14a584b6c9c69f`.
- CMake/MSVC v143 configure — success.
- APVTS ID audit — success.
- Release VST3 build — success.
- SFZ parser + SamplePool unit/integration tests — success.
- VST3 bundle verification — success.
- Artifact upload — success.
- Artifact `PPGU-v33-M1-Windows-x64-VST3`, ID `13716110642`, размер 2,130,870 bytes.
- Скачать: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42709935766/artifacts/13716110642

Подтверждено CI: фоновой SFZ worker pool компилируется и не ломает текущие sampler tests. Отдельный ручной тест с крупной библиотекой в FL Studio 20 всё ещё необходим. Worker выполняет одну задачу за раз; при закрытии Processor дожидается завершения активной загрузки.

## Подключение chorus, delay и reverb DSP — 10 ноября 2026

После успешного CI асинхронной загрузки SFZ добавлены три реальные DSP-стадии в `processBlock`:

- Chorus: модулированная короткая stereo delay line, разная фаза L/R, параметры Chorus On / Rate / Depth / Mix.
- Delay: stereo circular buffer до 2.0 секунд, параметры Delay On / Time / Feedback / Mix, feedback ограничен 0.95.
- Reverb: JUCE Reverb wet-only stage с Room Size / Damping / Mix и отдельным dry/wet blend.
- Все delay/chorus ring buffers и reverb scratch buffers выделяются только в `prepareToPlay`; в audio callback новые буферы не создаются.
- При блоке больше объявленного host maximum reverb/chorus безопасно пропускаются, а delay остаётся на заранее выделенной памяти.

Ограничения и проверки:
- Изменения только что отправлены в ветку; Windows CI ещё должен подтвердить JUCE API и компиляцию.
- Пока нет отдельного слухового теста, спектральных/latency measurements и ручной проверки в FL Studio 20.
- Это стандартные базовые эффекты, не моделирование педалей/студийного оборудования.
- Параметры distortionTone, compressor, EQ, cabinet IR и double-track всё ещё не реализованы как DSP.
- При отключении эффекта его ring buffer не обнуляется немедленно; хвост может сохраниться при повторном включении. Это кандидат на последующую UX-полировку.
- Финальный UI пока показывает только LOAD SFZ, MASTER и VOICES; FX-контролы пока доступны как параметры хоста, но не выведены в собственную панель редактора.

## Windows CI: Chorus / Delay / Reverb DSP — SUCCESS — 10 ноября 2026

Workflow 42710812167 завершился SUCCESS на commit `a5113e1b4a378a04115f7f5acd9075137aded529`.

Подтверждённые этапы:
- CMake configure / MSVC v143 — success.
- APVTS parameter ID audit — success.
- Release Windows x64 VST3 build — success.
- SFZ parser + SamplePool unit/integration tests — success.
- VST3 bundle verification — success.
- Artifact upload — success.

Артефакт:
- `PPGU-v33-M1-Windows-x64-VST3`
- ID: `13715553261`
- Размер: 2,135,249 bytes.
- Workflow: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42710812167
- Скачать: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42710812167/artifacts/13715553261

Это подтверждает компиляцию и существующие тесты. Отдельных unit-тестов звукового отклика новых эффектов и ручного прослушивания в FL Studio 20 пока нет. Эффекты являются базовыми DSP-реализациями, не законченным amp/cab моделированием.

## Интеграция Meta Open Sound CC0 — поддержка WAV-релиза для OGG-ссылок SFZ — 10 ноября 2026

Проверены официальные источники Meta Open Sound:
- Meta Steel Guitar: 375 WAV, шесть струнных зон, E2–C6, три velocity layers.
- Meta Nylon Guitar: 348 WAV, шесть струнных зон, E2–B5, три динамических слоя.
- Оба инструментальных набора помечены CC0 1.0 Universal; официальные страницы указывают возможность коммерческого использования, модификации и распространения.
- Официальные WAV release ZIP assets опубликованы отдельно от OGG preview archives; исходные SFZ карты используют `default_path=ogg/` и имена `.ogg`.

Источники:
- https://github.com/facebookincubator/Meta-open-sound
- https://github.com/facebookincubator/Meta-open-sound/releases
- https://github.com/facebookincubator/Meta-open-sound/blob/main/systems/instruments/meta-steel-guitar/meta-steel-guitar.sfz
- https://github.com/facebookincubator/Meta-open-sound/blob/main/systems/instruments/meta-nylon-guitar/meta-nylon-guitar.sfz

Изменения M2:
- SamplePool теперь преобразует ссылку `.ogg` в поиск same-stem `.wav` в типичных `wav/` и sibling paths.
- Если WAV не найден в стандартных местах, выполняется ограниченный поиском по имени обход каталога библиотеки; при нескольких совпадениях загрузка отклоняется как неоднозначная.
- Пути всё равно canonicalized и проверяются на выход из папки библиотеки.
- Добавлен integration fixture: SFZ с `default_path=ogg/` и `sample=test.ogg` загружает `wav/test.wav`, после чего MIDI нота действительно рендерит ненулевой сигнал.

Ограничения:
- Новый OGG→WAV fallback и regression test ожидают свежего Windows CI; пока не считать их подтверждёнными.
- Meta Nylon использует keyswitch `sw_last` и crossfade opcodes; текущий M2 sampler пока не реализует keyswitch/crossfade. Его default Auto layer может загружаться как упрощённая карта, но смена Finger/Pick keyswitch и точные crossfade curves пока не поддержаны.
- Meta Steel channel-1 first-position map и базовые velocity zones лучше соответствуют текущему поддерживаемому SFZ subset; некоторые amp velocity curve opcodes остаются warning-only.
- CC0 разрешает распространение аудио, но перед включением в конечный пакет всё равно сохранять upstream LICENSE, SFZ и SHA256SUMS и проверять каждый sample hash.

## Отдельный пакет 3.1.0 с VST3 и CC0 sample libraries — SUCCESS — 10 ноября 2026

Отдельный workflow `POP PUNK GUITAR ULTIMATE - Windows VST3` завершился SUCCESS:
- Run 42710820238, commit `a61a3ad8096ecc694ba4bc3da214d840577e9a79`.
- Валидирован каталог 300 factory presets.
- Валидированы примеры portable preset files.
- Распакован отдельный source bundle 3.1.0.
- Скачаны redistributable Meta CC0 guitar sample libraries.
- Windows x64 Release VST3 build — success.
- VST3 bundle найден — success.
- pluginval validation — success.
- Packaging VST3 + sample library — success.
- Artifact upload — success.

Артефакт:
- `POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES`
- Artifact ID: 13716246618
- Размер: 2,243,364,859 bytes (около 2.09 GiB).
- Workflow: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42710820238
- Скачать: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42710820238/artifacts/13716246618

Важно: это отдельный 3.1.0 source-bundle pipeline, не тот же бинарник, что активная M2 sampler ветка. Его не считать заменой текущей M2 разработки и не смешивать его код/артефакт с M2 результатами. Ручной тест в FL Studio 20 по-прежнему не подтверждён.

## Windows CI: Meta SFZ OGG→WAV fallback regression — SUCCESS — 10 ноября 2026

Workflow 42711485254 завершился SUCCESS на commit `668b88f98eb7187a67df54c19c98e4f7b58f9aef`.

Подтверждённые шаги:
- CMake configure / MSVC v143 — success.
- APVTS ID audit — success.
- Release Windows x64 VST3 build — success.
- Sampler parser + SamplePool unit/integration tests — success.
- VST3 bundle verification — success.
- Artifact upload — success.

Новый regression test подтверждён:
- Создаётся SFZ с `<control> default_path=ogg/` и `sample=test.ogg`.
- В тестовой библиотеке существует только `wav/test.wav`.
- SamplePool находит WAV fallback, загружает регион и рендерит ненулевой MIDI-сигнал.
- Проверка не загружает реальные 1.3 GB Meta архивы; это тест механизма разрешения путей на синтетическом WAV fixture.

Артефакт:
- `PPGU-v33-M1-Windows-x64-VST3`
- ID: 13715159277
- Размер: 2,138,792 bytes.
- Workflow: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42711485254
- Скачать: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42711485254/artifacts/13715159277

Следующий шаг — добавить безопасный installer для двух официальных CC0 WAV libraries и затем проверить реальные SFZ карты против извлечённых WAV архивов.

## Installer официальных Meta CC0 WAV библиотек — подготовлен, ожидает Windows проверки — 10 ноября 2026

Добавлен `tools/Download_Meta_CC0_Guitar_Libraries.ps1`:
- Получает официальные GitHub Release assets по закреплённым тегам `meta-steel-guitar-v1.0.0` и `meta-nylon-guitar-v1.0.0`.
- Выбирает WAV ZIP, а не OGG preview archive.
- Проверяет ожидаемые инвентари: 375 Steel WAV и 348 Nylon WAV.
- Требует upstream SFZ, LICENSE и SHA256SUMS.
- Проверяет все checksum entries через SHA256.
- Проверяет, что каждый OGG sample reference в SFZ имеет ровно один same-name WAV файл.
- Сохраняет оригинальные карты/лицензии/checksum и устанавливает библиотеки в AppData.
- При обновлении использует backup старой папки и пытается восстановить её при ошибке перемещения.

Добавлена инструкция: `docs/META_CC0_LIBRARY_INSTALL_RU.md`.

Проверки и ограничения:
- Скрипт ещё не запускался на реальной Windows-машине и не проверен на фактических release ZIP payloads.
- M2 Windows CI проверил C++ OGG→WAV fallback на синтетическом WAV fixture, но не проверил скачивание 2 архивов и их полную распаковку.
- Нужен отдельный Windows PowerShell validation run или ручной запуск на Windows; до этого installer считается подготовленным, но не production-verified.

## Исправление падения Windows CI: тип MIDI Velocity — 10 ноября 2026

Run 42713423216 завершился FAILURE на Release VST3 compile. Ошибка MSVC:
- `PluginProcessor.cpp(261,50): C2672 juce::jlimit: ambiguous template`.
- Причина: `MidiMessage::getVelocity()` передавался в `jlimit(float,float,...)` без явного преобразования, и компилятор видел конфликт между `juce::uint8` и `float`.
- Из-за этого также возникла производная ошибка C2737 для `midiVelocity`.
- APVTS audit 84 IDs прошёл; CMake configure прошёл; компиляция прервалась на PluginProcessor.cpp, sampler tests были пропущены.

Исправление:
- Commit `d6344b7d413c143953387f01e99ff90fcda0eb5c`.
- Явно нормализуем raw MIDI velocity в диапазон 0..1 через `static_cast<float>(getVelocity()) / 127.0f`, затем ограничиваем и преобразуем обратно в MIDI 1..127 для SFZ velocity layers.
- Следующая Windows CI должна подтвердить сборку и тесты. До её завершения исправление считается внесённым, но не проверенным.

## Правило фиксации прогресса

После каждого существенного изменения обновлять журнал и README: дата, branch/commit, что изменено, какие тесты прошли, ссылка на Actions, artifact ID/hash, известные дефекты и следующий шаг. Не выдавать сборку за готовый продукт без CI и проверки в целевом хосте.

## Regression fix: SFZ off_by is MIDI-channel scoped — 10 November 2026

During M2 sampler review, a channel-isolation defect was found in `SamplerEngine::noteOn`:
- The previous choke loop compared only `group == offBy` and could release a matching group voice on another MIDI channel.
- This is incorrect for guitar maps that route strings/parts to separate MIDI channels; a note on one channel must not silence another channel's voice.

Changes committed to `feat/m2-sfz-loop-playback`:
- `16565b4fea339872396482bd72c587428382ba91` — regression test creates two channel zones with the same group/off_by values and verifies that channel 1 does not choke channel 2.
- `bab8c4d07539a9de80e7fe135e480e7d94a16f5f` — choke matching now also requires the active voice MIDI channel to equal the incoming note's channel.

Validation status:
- The test and implementation are committed.
- The workflow is configured to run on pushes affecting `Source/**` and `tests/**` in this branch.
- A completed Windows CI result for commit `bab8c4d07539a9de80e7fe135e480e7d94a16f5f` has not yet been verified; do not mark this regression as CI-passed.

## Интерфейс PPGU 4.0 — перенос утверждённого скина в рабочий редактор

Пользователь подтвердил скриншот PPGU 4.0 как обязательный основной скин. Создана отдельная ветка `feat/ui-approved-skin-functional` от текущей SFZ-ветки, чтобы UI-работа не смешивалась с незавершёнными M2/M3 изменениями.

### Уже подключено в рабочем редакторе
- Тёмная графитовая тема с розовыми акцентами, верхняя панель, hero-область с векторным силуэтом гитары, секции AMP & TONE / FX CHAIN / PLAYBACK.
- Две вкладки Melody/Pattern меняют APVTS playMode.
- LOAD SFZ запускает асинхронную загрузку и показывает фактический статус.
- Шесть rotary controls с APVTS attachments: Gain, Treble, Mid, Bass, Velocity, Dynamics. Dynamics использует кривую velocity до выбора SFZ velocity layer.
- Пять FX-переключателей с APVTS attachments: Distortion, Chorus, Flanger, Delay, Reverb.
- Добавлена четырёхоктавная экранная MIDI-клавиатура; её события проходят в MIDI-путь процессора и могут играть загруженные SFZ-зоны.
- Gain, EQ low/mid/high и Flanger имеют отдельную DSP-обработку с preallocated buffers/filter state.
- Windows CI workflow расширен для этой ветки.

### Честные ограничения текущей итерации
- Верхний баннер пока векторный fallback, а не финальная фотокомпозиция из референса; точное брендированное изображение и текстуры корпуса ещё нужно интегрировать.
- Electric/Acoustic, AutoChords, Key/Scale selectors, Browser/Presets/Settings/About, preset save/load, полноценная pattern sequencer UI и дополнительные ручки параметров ещё не реализованы как рабочие controls.
- Скин адаптирован под resize, но визуальную проверку на разных DPI/масштабах и внутри FL Studio ещё нужно пройти.
- Windows CI run 42723235202 (№140) запущен на commit `d865986320839d1ce0734fcee5441a1eb6beebed`; до его завершения нельзя утверждать, что новая UI-сборка компилируется.

**Статус: skin implementation in progress; UI не объявляется завершённым.**
## M3 — транспортный фундамент Pattern Scheduler — 10 ноября 2026

Основа M3 реализуется поверх ветки с одобренным пользовательским skin и последней проверенной Windows-сборкой, а не поверх устаревшей ветки M3.

Что сделано:
- Добавлен независимый PatternScheduler с фиксированным массивом событий и без выделения памяти во время планирования.
- Введены типизированные структуры снимка транспорта хоста, PPQ-часов блока, события исполнения и MIDI-события с sample offset.
- Реализовано преобразование PPQ/tempo в sample offset с ограничением BPM и длины блока.
- Добавлены host PPQ/tempo/playing snapshot и внутренние PPQ-часы для режима Internal / offline fallback.
- События сортируются детерминированно; NoteOff располагается перед NoteOn на одном sample offset, чтобы повторный триггер одной ноты не зависал.
- В PluginProcessor считывается один снимок AudioPlayHead::PositionInfo на блок; обращения к DAW не переносятся в фоновые потоки.
- Добавлен отдельный dependency-free CTest target ppgu_pattern_scheduler_tests с проверками host PPQ, transport stop, internal clock, quantization, transposition и порядка retrigger.

Важные ограничения:
- Это транспортный/планировочный фундамент, не завершённый Pattern performance engine.
- Runtime chord map, AutoChords, performance-event bank, down/up strums и реальные chord/strum recordings ещё не подключены. Нельзя выдавать их за реализованные только на основании scheduler.
- Поведение Host Start должно быть окончательно закреплено вместе с UI mapping и тестами транспорта; текущая интеграция читает host state и отдельно выбирает Internal.
- Реальная многопоточность аудио-DSP не добавляется фиктивным флажком. До отдельной измеренной реализации GOOD / NORMAL / NIGHT профилей и безопасной параллельной обработки multicore не показывается как работающая функция.

Проверка:
- Новый тестовый target добавлен в CTest.
- Windows CI для новой ветки ожидает запуска; пока не считать код скомпилированным.
- Требуются Windows MSVC build, CTest, pluginval и ручная проверка FL Studio 20.

Следующие задачи M3:
1. Уточнить режимы Host / Host Start / Internal и stop/restart без зависших Pattern-нот.
2. Добавить sample-offset event queue для событий исполнения и quantized pattern switching.
3. Подключить preview transport так, чтобы предпрослушивание не сбрасывало основной transport.
4. Добавить тесты смены темпа, seek/loop, смены pattern slot и transport stop/restart.
5. Затем перейти к M4 runtime preset model/browser и сохранению пользовательских данных.

## M3 — исправление межблочных NoteOff и regression test — 10 ноября 2026

В ходе повторного анализа PatternScheduler выявлен дефект планировщика: NoteOff, расположенный за концом текущего аудиоблока, отбрасывался. Для длинных gate это могло оставлять MIDI-ноту включённой, если событие не попадало в тот же блок, что и NoteOn.

Исправление:
- Добавлена фиксированная очередь pending NoteOff на 512 записей; динамическая память не используется.
- NoteOff, выходящий за конец текущего блока, сохраняется с абсолютной PPQ-позицией и выдаётся в том блоке, в который попадает.
- При остановке транспорта очередь очищается, чтобы после Stop не воспроизводились устаревшие NoteOff.
- При reset очередь также очищается.
- Сортировка событий по sample offset и приоритет NoteOff перед NoteOn сохранены.

Тестирование:
- Добавлен регрессионный сценарий с gate длительностью 0.75 четверти при блоках по 0.25 четверти.
- Тест проверяет, что NoteOn возникает в первом блоке, а NoteOff не теряется и появляется в четвёртом блоке с sample offset 0.
- Тест добавлен в ppgu_pattern_scheduler_tests / CTest.
- Изменения: commit `ac99a7ec78815c095b14bce40432c1f5c1d1fff1` (scheduler), commit `f429e67dee1dd9b01334d3adcbffbe995d196c18` (regression test).

Статус проверки:
- Исходный Windows CI run 42726852431 (№159) оставался in_progress при последней проверке; завершённый результат для последующих commits пока не подтверждён.
- Не объявлять исправление прошедшим MSVC/CTest до завершения новой сборки.
- Реальное исполнение ScheduledEvent в MIDI/audio pipeline пока не подключено: это следующий обязательный шаг M3, до него scheduler является изолированным tested-core кандидатом, а не готовым Pattern playback.


## Сводный отчёт о состоянии проекта — 10 ноября 2026

Контрольная сводка для продолжения разработки в ветке `feat/m3-transport-scheduler-v2`. Это отчёт о текущем состоянии, а не заявление о готовности продукта.

### Что сделано

**M1 / каркас — частично**
- JUCE/CMake-проект, VST3 target для Windows, APVTS-параметры, MIDI/audio processor и Windows CI workflow.
- Успешный CI №155 ранее относился к UI-ветке и не подтверждает полную приёмку M1 или ручное тестирование FL Studio 20.

**M2 / сэмплер — частично**
- SFZ-парсер и WAV playback: key/velocity zones, циклы, tuning, voice cap, MIDI channel mapping, асинхронная загрузка.
- Добавлены тесты декодирования, циклов, reload, channel mapping, voice cap и off_by.
- Исправлен off_by/choke с учётом MIDI-канала.
- Добавлена Hermite-интерполяция.
- Исправлена совместимость velocity clamp с MSVC.
- Полная приёмка M2 не подтверждена. Streaming/mmap, Round Robin, 10 ms velocity crossfade, плавное voice stealing и комплексные стресс-тесты требуют завершения/подтверждения.

**UI — частично**
- На `feat/ui-approved-skin-functional`: тёмный скин, секции AMP & TONE / FX CHAIN / PLAYBACK, вкладки Melody/Pattern, LOAD SFZ, шесть rotary controls, пять FX toggles, экранная MIDI-клавиатура.
- Часть контролов подключена к APVTS и DSP.
- Каталог содержит 300 записей, но наличие записи не подтверждает полноценное звучание каждого пресета.
- Остаются финальная фотокомпозиция/текстуры, дополнительные рабочие селекторы, полный pattern sequencer, пользовательское сохранение пресетов, DPI/scale-проверки и ручной тест в FL Studio.

**M3 / транспорт**
- PatternScheduler с фиксированным буфером событий и без динамической аллокации при планировании.
- Host PPQ/tempo/playing snapshot, внутренние часы, quantization, transpose и sample offset.
- Добавлен CTest target для scheduler.
- Исправлена потеря NoteOff при переходе между аудиоблоками: фиксированная очередь pending NoteOff и регрессионный тест.

### Исправленные дефекты
- Неверное использование JUCE `PositionInfo::getIsPlaying()`: возвращаемое значение обрабатывается как bool.
- off_by не должен глушить голос на другом MIDI-канале.
- MSVC velocity clamp compile issue.
- Дублированный EQ/Flanger участок в UI processor.
- Потеря NoteOff для нот, gate которых пересекает границы аудиоблоков.

### Что осталось сделать

**M2 — довести sampler до приёмки**
- Проверить/реализовать потоковую загрузку и ограничение RAM.
- Round Robin минимум 3 варианта без повторения подряд.
- Velocity-layer crossfade 10 мс.
- Плавный voice stealing без щелчков.
- Проверить release-сэмплы и 100 последовательных нот без кликов.
- Stress/reload и memory tests.

**M3 — реальное исполнение паттернов**
- Подключить ScheduledEvent к реальному MIDI execution path с точными sample offsets.
- Runtime-данные паттернов и восемь pattern slots.
- Quantized switching 1/16 и 1/32.
- Разделить Host / Host Start / Internal.
- Проверить Stop, Restart, Seek, Loop и tempo changes.
- Гарантированное снятие активных нот при stop/смене слота.
- Независимый Preview transport/bus.
- Регрессионные тесты смены слота, темпа, seek/loop и stop/restart.

**Музыкальный движок**
- Chord detection и chord display; AutoChords и FingeringGenerator.
- Pattern/style engine, фразы 1/2/4 такта, downstroke/upstroke.
- Melody articulations: Open, Palm Mute, Flageolet, Tremolo, Hammer-on, Pull-off, slides, legato, fret noise.
- Acoustic mode, требуемые строи и артикуляции.
- Проверить музыкальную полноценность всех factory presets и обязательный Photographs preset.

**Интерфейс**
- Все элементы скина должны реально работать и управлять движком.
- Завершить 8 pattern slots, chord display, FX drag-and-drop reorder.
- MIDI Import/Export, MIDI Learn.
- Save As, favourites, Setlist, preset versions.
- Реальные CPU/RAM meters, HELP, UI scale 75/100/150%, Library Manager/integrity check.
- Проверить внешний вид и функциональность в FL Studio 20 и на разных DPI.

**Оптимизация**
- Реализовать и измерить GOOD / NORMAL / NIGHT.
- Безопасная реальная multicore-обработка без блокировок audio thread и фиктивных переключателей.
- Проверить целевые требования 2 cores / 4 GB RAM, CPU/RAM budgets, старт на HDD, отсутствие dropout/clicks.

**Релиз**
- Windows 10 x64 build, CTest, pluginval и ручная проверка FL Studio 20.
- Стресс-тесты, длительный прогон, автоматизация и смена буфера.
- Аудит лицензий всех включённых sample assets; включать только материалы с разрешённым распространением.
- Финальный VST3 bundle/installer, библиотека, документация и скрипт проверки.

### Правила отчётности и приёмки
- После каждого существенного этапа обновлять этот журнал в этой ветке.
- Для каждого бага фиксировать проявление, первопричину, исправление, regression test и CI result.
- Разделять статусы: код добавлен / локальный тест пройден / Windows CI пройден / проверено в FL Studio 20 / принято.
- Не объявлять функцию готовой только по наличию класса или UI-контрола.
- Не объявлять этап принятым до подтверждения критериев исходного ТЗ.

### Следующий шаг
Проверить актуальный Windows CI после последних коммитов и подключить PatternScheduler к реальному MIDI execution path. Пока это не сделано, Pattern playback остаётся незавершённым.


## Дополнение к критериям оптимизации — 10 ноября 2026

По требованию пользователя приоритет оптимизации — минимально возможное потребление CPU, RAM и других ресурсов на всех этапах работы. Целевые показатели являются верхними бюджетами, а не измеренными результатами:

- Минимальная целевая конфигурация: Intel Core i3, 2 физических ядра, 2 ГБ RAM.
- RAM в простое: целевой диапазон 50–150 МБ.
- RAM при воспроизведении: целевой диапазон 400–700 МБ.
- CPU при типовой согласованной нагрузке: не более 8–10%; стремиться ниже без ухудшения качества звука, задержки и стабильности.

### Правила оптимизации
1. Сначала измерять baseline и профилировать узкие места; не добавлять оптимизацию «на глаз».
2. Audio thread: никаких блокирующих mutex, файлового I/O, выделения памяти и освобождения больших объектов.
3. Сэмплы загружать по требованию, ограничивать кэш и число голосов; предусмотреть освобождение неиспользуемой памяти и предзагрузку только активного пресета.
4. Предусмотреть уровни качества/нагрузки GOOD, NORMAL, NIGHT с реальным влиянием на oversampling, качество/хвосты FX, polyphony и sample cache; настройки не должны ломать тембр неожиданно.
5. Multicore включать только там, где измерения подтверждают выигрыш на 2-ядерном CPU; не создавать лишние потоки и oversubscription.
6. Сравнивать CPU/RAM при фиксированных условиях: один и тот же пресет, темп, число нот/голосов, эффекты, частота дискретизации и размер буфера. Отдельно измерять idle, playback и stress.
7. Не заявлять достижение бюджета по результату CI: нужны профилировочные измерения на целевом i3/2-core/2-GB ПК и длительный тест без dropout/clicks.

До получения измерений статусы CPU/RAM целей остаются «не подтверждены». Оптимизация не должна подменять корректность DSP, качество звука, MIDI-тайминг или устойчивость.


## Повторный аудит M1–M3 и исправление scheduler — 10 ноября 2026

Рабочая ветка этого отчёта: `feat/m3-transport-scheduler-v2`.
Актуальная ревизия: `8abc3f02e86d3d2a231d3d109ee45a8c816a8916`.

### Проверка CI до исправления
Run 42728807262 (run #76) подтвердил:
- Windows x64 / MSVC Release VST3 — сборка успешна.
- SFZ parser tests — успешно.
- SamplePool tests — успешно.
- PatternScheduler tests — провал; CTest итог 2/3.
- pluginval и публикация основного VST3-артефакта были пропущены из-за провала CTest.

Корень дефекта в scheduler: при событии паттерна с onset в предыдущем аудиоблоке код повторно пытался выпустить NoteOff из прохода по исходным событиям, хотя NoteOff уже хранится в pending queue. На блоке, содержащем конец длинной ноты, это приводило к дублированию NoteOff.

### Исправление
В `Source/Performance/PatternScheduler.h`:
- паттерн-событие теперь обрабатывается только в блоке, содержащем его onset;
- NoteOff для короткой ноты выпускается в том же блоке;
- NoteOff длинной ноты ставится в bounded pending queue один раз;
- последующие блоки не повторяют обработку исходного onset.

Регрессионный тест уже существовал в `tests/PatternSchedulerTests.cpp`: long gate spanning multiple blocks. Исправление направлено на этот сценарий; тест не отключался и критерии не ослаблялись.

Commit: `8abc3f02e86d3d2a231d3d109ee45a8c816a8916`.
Windows CI для этого commit запущен: runs 42729490984 и 42729491121. На момент записи оба были `in_progress`; результат пока НЕ подтверждён.

### Важные критерии, которые всё ещё не закрыты
- M1: Release VST3 build и аудит параметров есть в истории CI, но автоматизация и загрузка/сохранение проекта в FL Studio 20 на целевой Windows 10 не подтверждены host-тестом.
- M2: SFZ/WAV parser и SamplePool проходят отдельные тесты. Однако обязательный протокол «100 ударов разной velocity без единого клика», полноценный RR без повторов, velocity crossfade и streaming/mmap + hot/warm cache по v33 не подтверждены. Текущий SamplePool декодирует WAV в память; нельзя заявлять соответствие потоковой архитектуре и RAM-бюджету.
- M3: scheduler unit tests существуют, но реальный runtime-путь pattern events в `processBlock` ещё не подключён: планировщик вычисляет transport clock, однако scheduled events не превращаются в sampler noteOn/noteOff. Не закрыты также квантованная смена слота 1/16, preview bus и host/offline acceptance на 60–200 BPM и при смене block size.

### Строгое правило перехода к M4
Согласно v33, M4 начинается с AutoChords и относится к E5. Не начинать реализацию M4 до фактической приёмки M1, M2 и M3. После прохождения CI scheduler regression следующий приоритет — подключить события M3 к реальному sampler runtime и покрыть headless-тестами; затем закрыть обязательные host/100-hit критерии. Только после этого — AutoChords/FingeringGenerator по плану M4.



## Дополнительная проверка CI и workflow packaging — 10 ноября 2026

### Подтверждённый успешный M1/M2 Windows workflow
Run 42729490984 (#162), commit `8abc3f02e86d3d2a231d3d109ee45a8c816a8916`:
- MSVC x64 Release VST3 — успешно.
- APVTS parameter ID audit — успешно.
- sampler unit/integration tests — успешно.
- VST3 bundle structure verification — успешно.
- Artifact опубликован: `PPGU-v33-M1-Windows-x64-VST3`, artifact ID `13724985195`, SHA256 ZIP `b94c39999b9bc7aa9e65cdffed767a344ab42c576d74730525d16d7e7a080e86`.
- Размер ZIP: 2,317,069 bytes; VST3 module binary: 5,628,928 bytes.
Это подтверждает сборку и упаковку VST3, но не ручную проверку в FL Studio 20.

### Второй workflow: регрессия инфраструктуры
Run 42729491121 (#78) успешно собрал VST3 и прогнал CTest: 3/3 теста прошли (SFZ parser, PatternScheduler, SamplePool). Workflow затем завершился ошибкой, потому что шаг ссылался на отсутствующий файл `tools/validate_windows_vst3.ps1`. В репозитории такого файла нет.

Исправление: commit `b05bc76bda579e8e4713219f022e55b594ac1f10` удалил обращение к несуществующему скрипту и явно фиксирует, что pluginval/DAW host validation не настроены. Это не подменяет тест: host validation остаётся незакрытой. Новый workflow запускается на новом commit; его итог пока не подтверждён.

### Runtime-аудит M3
Повторное чтение `Source/PluginProcessor.cpp` подтвердило:
- host BPM/PPQ/play state считываются;
- `PatternScheduler::beginBlock()` обновляет transport clock;
- в `processBlock()` нет вызова `PatternScheduler::schedule()`;
- scheduled events не преобразуются в `sampler.noteOn/noteOff`;
- реальное проигрывание восьми пользовательских pattern slots отсутствует в audio path.
Следовательно, M3 не завершён. Следующий инженерный этап — определить/реализовать безопасный фиксированный runtime-представитель данных паттерна и event dispatch с разбиением блока по sampleOffset, сохраняя отсутствие heap allocation/locks в audio callback. Нельзя заявлять M3 complete до проверки note lifecycle, stop/panic, block boundaries, slot switching and timing.



## Актуальная ревизия M1–M3 — 10 ноября 2026 (ветка feat/m3-transport-scheduler-v2)

Предыдущие разделы журнала содержат исторические статусы других веток/ревизий. Для продолжения работ источником истины является текущий исходный код и CI именно этой ветки. Старые статусы не следует переносить на текущий HEAD без повторной проверки.

### M1 — подтверждено и остаётся
Подтверждено: JUCE VST3 instrument target Windows x64, stereo output, MIDI input; APVTS parameter layout и успешный CI audit parameter IDs; getStateInformation/setStateInformation через APVTS ValueTree XML; Editor создаётся, hasEditor() true. CI run 42729490984 успешно собрал MSVC Release VST3, прошёл APVTS audit, sampler tests, bundle verification и опубликовал artifact.

Остаётся: FL Studio 20 host acceptance (загрузка, MIDI, automation, project save/reopen, recall параметров); восстановление выбранной SFZ-библиотеки (state methods сохраняют APVTS tree, но текущий код не добавляет путь SFZ в state); реальное подтверждение Panic ≤30 ms в DAW и стабильность при разных host block sizes.

### M2 — подтверждено и остаётся
Подтверждено кодом: SFZ parser zones/inheritance, key/velocity/channel mapping, sample path containment, WAV decode, loops including ping-pong and one-shot; Hermite interpolation; bounded 64-voice sampler; sample load вне audio callback; snapshot/asset caps; transactional sample loading — новые assets удерживаются локально до полной валидации SFZ карты. CI run 42729490984: SFZ parser, SamplePool, VST3 Release and bundle checks passed. Run 42729491121 показал 3/3 CTest, но workflow упал на отсутствующем validator script; script reference позднее удалён.

Остаётся: 100-hit no-click stress acceptance и long-duration playback; полноценный Round Robin без повторов, velocity-layer crossfade, smooth voice stealing (при насыщении chooseVoice возвращает oldest index, fade-steal не реализован); streaming/mmap и demand-loaded bounded cache (WAV декодируется в память; лимит общего sample storage 1 GiB и snapshots 128 — это не streaming и не доказательство RAM target); проверка акустических/электрических библиотек, velocity layers и palm mute по реальным zones; лицензии библиотек и redistribution.

### M3 — повторная проверка и исправление
Подтверждено: scheduler unit tests покрывают host PPQ, stop transition, internal clock, quantization, transposition, ordering and long gate pending NoteOff. processBlock получает host BPM/PPQ/play state и вычисляет BlockClock. Commit c63fbcb добавляет sampler.allNotesOff() при переходе host transport playing→stopped, предотвращая зависшие ноты после очистки pending NoteOff.

Не завершено: processBlock всё ещё не вызывает patternScheduler.schedule(); нет dispatch ScheduledEvent -> sampler.noteOn/noteOff с sample-accurate segment rendering; нет работающего восьмислотового pattern runtime, quantized slot switching и host/offline tests. Factory catalogue pattern definitions помечены как ожидающие pattern engine и записанные chord/strum performance samples. Каталог не является проигрываемой библиотекой паттернов. Host Start vs Host требует отдельной проверки; параметр syncMode сам по себе не доказывает соответствие трём режимам.

Следующая реализация M3 должна использовать фиксированное хранилище событий/слотов, заранее выделенную память, не выполнять allocations/locks/file I/O в audio callback, корректно обрабатывать смену слота, MIDI chord changes, transport stop/reset, длинные ноты и совпадающие NoteOff/NoteOn. Проверки должны охватывать sample offsets и разные block sizes.

### Изменения текущей ревизии
Commit c63fbcb1ecec3ca34b2c36b5ec0199f4a67c55e4: добавлен release всех sampler voices при host transport stop. Изменение требует нового Windows CI на текущем commit; на момент записи новый CI ещё не подтверждён.

### Порядок
M1 acceptance → M2 acceptance → M3 runtime + acceptance → только затем M4 AutoChords/FingeringGenerator согласно v33. Не начинать M4 реализацию до прохождения предыдущих этапов.


## Дополнение — пользовательская проверка и CI, 10 ноября 2026

### M1 — новая host-проверка от пользователя
Пользователь подтвердил фактический запуск VST3 в Windows 10 1809, FL Studio 20, на двухъядерном процессоре с 2 ГБ RAM. Это закрывает ранее отмеченный пункт «плагин загружается в целевой DAW/ОС» как пользовательское ручное подтверждение. Это не является измерением CPU/RAM и не закрывает отдельно автоматизацию APVTS, сохранение/восстановление проекта, panic fade и длительный стресс-тест.

### CI — commit 18713d156ab41054b44b7fa9c310cbb84f2ba089
Workflow run 42731658417 завершился success. Успешно завершены проверки каталога 300 пресетов, portable preset examples, загрузка redistributable sample libraries, CMake configure x64 Release, сборка VST3/test executables, CTest regression suite, bundle discovery, package и artifact upload. Артефакт POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES, ID 13724394789, размер 2,242,835,964 байт, SHA-256 f5d6ea21c49481a91b83fd9552231acc51d96237d74a93e17dd6475754dce1f4, срок хранения до 8 февраля 2027. Ссылка: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42731658417/artifacts/13724394789 . Это CI-артефакт, а не постоянный GitHub Release; host validation в workflow явно отмечена как отдельное ограничение.

### Текущая приёмка без завышения статуса
- M1: сборка, bundle и запуск в FL Studio 20 на Windows 10 1809 подтверждены; автоматизация, state recall и panic fade в DAW остаются отдельными непроверенными критериями.
- M2: базовые SFZ/WAV mapping, decode, playback, Hermite, loops и voice cap проходят автоматические проверки. M2 ещё не принят: нет RR без повтора, velocity crossfade, click-free voice stealing, demand streaming/mmap и 100-hit no-click acceptance.
- M3: scheduler unit tests проходят, но scheduler по-прежнему не подключён к processBlock() как исполняющий Pattern runtime. Нет dispatch ScheduledEvent -> sampler, 1/16 quantized slot switching, восьми рабочих слотов и headless/offline render acceptance. Поэтому M3 не принят.

### M4 — подготовка по v33 FINAL
M4–M6 относятся к шагу «Интеллект»: AutoChords, FingeringGenerator, StyleEngine, MelodyEngine (артикуляции, hammer-on/pull-off, slides) и PatternEngine с фразами 1/2/4 такта. По правилу 0.1 нельзя объявлять начало реализации M4 до фактической приёмки M1–M3. Разрешена предварительная декомпозиция и подготовка тест-кейсов; реализацию не смешивать с незакрытыми блокерами M2/M3.


## Обновление каталога и sound target — 10 ноября 2026

Каталог обновлён в ветке `feat/m3-transport-scheduler-v2`: 150 Melody + 150 Pattern, 300 всего. Добавлены недостающие style DNA: Fall Out Boy, Yellowcard, My Chemical Romance, Less Than Jake, Fenix TX и Midtown. Теперь каталог охватывает 21 ориентир: Blink-182, Sum 41, Green Day, Fall Out Boy, Good Charlotte, Simple Plan, New Found Glory, Avril Lavigne, Yellowcard, The All-American Rejects, My Chemical Romance, Less Than Jake, Fenix TX, Midtown, Bowling for Soup, MGK, Mest, Angels & Airwaves, The Offspring, Sugarcult, All Time Low.

Для Pattern в каждой записи разведены независимые оси: `soundCharacter` (Clean, Crunch, Drive, Palm Mute, Open Chords, Acoustic, Lead Drive, Muted Chugs) и `playingStyle/groove` (downstrokes, alternate picking, muted verse → open chorus, ska offbeats, syncopated stops, octave hooks, arpeggios, acoustic strums и др.). Это пока catalogue metadata, а не обещание, что все эти паттерны уже исполняются runtime.

Добавлены MGK Bloody Valentine Inspired Lead и Bloody Valentine Inspired Chorus как стилистические sound targets: яркий современный pop-punk drive, приглушённый verse pulse и открывающийся припев. Никакие оригинальные stems, master recordings, ноты/транскрипции или коммерческие сэмплы не включены.

Обновлены README и MASTER_PRODUCT_SPEC: коммерческая цель звучания, критерии сравнения с NI Electric Sunburst Deluxe, требования к профессиональным DI/mic записям, velocity layers, round robin, articulations, performance recordings, amp/cab/IR, phase/mono checks и loudness-matched listening. Это roadmap/acceptance requirements, не текущие реализованные функции.

Обновлён `assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg`: новый векторный product cover в фирменной charcoal/magenta стилистике с макетом интерфейса и блоками Melody/Pattern/Tone/Performance. Растровая AI-обложка не создана в этом сеансе из-за лимита генерации изображений; SVG — готовый самостоятельный векторный баннер.

## 10 ноября 2026 — спецификация пользовательских BANK-библиотек

- Ветка: feat/m3-transport-scheduler-v2
- Предыдущий HEAD: fe97f5d0849dc44d92cbc1d0d9362ae216dd0fa7
- Коммит спецификации: 116c2bb01ae999c4f3d8ea719eb7a99f890e94a2
- Файл: docs/BANK_LIBRARY_SYSTEM_RU.md
- Запрос: импорт пользовательских WAV-библиотек без обязательной конвертации.
- Решение: исходные WAV остаются неизменными; SFZ используется как карта при наличии; без SFZ предусмотрены Auto Mapping и ручное подтверждение карты; BANK отделён от пресетов; предусмотрены манифест, SHA-256, лицензии, безопасный ZIP import и атомарная публикация.
- Статус: архитектурная спецификация добавлена в репозиторий. Это НЕ реализация BANK Manager и НЕ подтверждение работы импорта в плагине.
- Тесты: сборка/CTest не запускались; код движка этим коммитом не менялся.
- Следующий шаг: реализовать безопасное сканирование папки и манифест поверх существующего SamplePool, затем тестировать на WAV/SFZ fixtures.

## 10 ноября 2026 — политика источников звуков и лицензий

- Ветка: feat/m3-transport-scheduler-v2
- Коммит документа: 2c13759d8351f1ca8a9ebe43a5a1d803bdb0965f
- Файл: docs/SOUND_ASSET_SOURCES_AND_LICENSE_POLICY_RU.md
- Добавлено: список кандидатов Meta Open Sound (CC0 для Nylon/Steel Guitar), FreePats (кандидат CC0), SFZ Instruments (проверка каждой записи), Stargate Sample Pack (проверка provenance), per-file лицензирование Freesound.
- Добавлено: юридический allowlist/denylist, обязательная фиксация автора, лицензии, атрибуции, версии, URL и SHA-256.
- Архитектура: отсутствие искусственного лимита числа/общего размера BANK при ограничении реальными ресурсами; индексирование, lazy/background loading, кэш, streaming/page cache и bounded voice/RAM budget.
- Статус: исследовательская политика и shortlist зафиксированы. Ни один аудиофайл этим коммитом не скачан, не включён в плагин и не прошёл финальную юридическую проверку.
- Тесты: сборка и CTest не запускались; код движка не менялся.
- Следующий шаг: проверить дерево проекта и API Sampler/SFZ, затем реализовать индекс/manifest и импорт папки с WAV/SFZ.

## 10 ноября 2026 — лимит заводской библиотеки и пресетов 20 GB

- Ветка: feat/m3-transport-scheduler-v2
- Коммит: 4f277efbc34f20dc3f64386752714f95f4271876
- Файл: docs/BANK_LIBRARY_SYSTEM_RU.md
- Решение: общий официальный комплект (заводские звуковые BANK + заводские пресеты + SFZ/метаданные и необходимые файлы) не должен превышать 20 GB.
- Пользовательские внешние BANK, импортируемые самостоятельно, хранятся отдельно и не входят в лимит заводского комплекта.
- Кэш пользователя управляется отдельно и должен быть очищаемым/пересоздаваемым; пресеты не дублируют WAV.
- Статус: спецификация обновлена; код упаковщика/установщика не менялся, размер реальной библиотеки и сборки не измерялся.
- Тесты: не запускались.

## 10 ноября 2026 — аудит масштабирования BANK и ограничений SamplePool

- Ветка: feat/m3-transport-scheduler-v2
- HEAD до аудита: 31e2261db84d3b3d3a94e37bb5475a2272705719
- Коммит аудита: 27a25426dca927275aa16d31da020249d842d5b1
- Файл: docs/BANK_SCALE_GAP_AUDIT_RU.md
- Подтверждено по Source/Sampler/SamplePool.h/.cpp: текущий SamplePool имеет лимит 1 GiB декодированных буферов и 128 snapshots; WAV загружаются целиком в память.
- Вывод: лимит заводского архива 20 GB не означает загрузку 20 GB в RAM. Текущий движок пока не поддерживает неограниченный по размеру пользовательский BANK.
- Решение: следующий крупный инженерный этап — metadata-only index + worker-side streaming/page cache + безопасное освобождение старых snapshot generations. Простое повышение лимита RAM отвергнуто.
- Статус: аудит опубликован; исходный код sampler не изменялся. Streaming не реализован.
- Тесты: не запускались; этот коммит документационный.

## 10 ноября 2026 — первый шаг к масштабируемому BANK: metadata-only WAV index

- Ветка: feat/m3-transport-scheduler-v2
- Коммиты:
  - 92415f9c4c8c03132b348eed87e1f329cdf597ed — API индекса метаданных.
  - 5901e737b546110ddf8462b5a6be89ab5ea36f44 — реализация рекурсивного WAV header scan.
  - 9f6001f18455973ba86075e6a35be8b330fc0297 — тесты metadata index.
  - 217ac5ad5c3f8d932776cb47c2a0851f0ac98da2 — подключение в CMake и plugin target.
- Новые файлы:
  - Source/Sampler/SampleMetadataIndex.h/.cpp
  - tests/SampleMetadataIndexTests.cpp
- Изменение: новый индексатор рекурсивно находит WAV и считывает только метаданные через JUCE AudioFormatReader: sample rate, frames, channels, bit depth, float flag, file size. Данные сортируются детерминированно. Повреждённые/неподдерживаемые файлы попадают в warnings, ошибки каталога — в errors.
- Безопасность: корень каталога canonical; symbolic links пропускаются; canonical path проверяется на принадлежность корню. Индексатор не создаёт AudioBuffer и не декодирует waveform в память.
- Тест fixture: nested stereo WAV 24-bit/48 kHz, контроль количества кадров/каналов/bit depth/file size и отсутствующего каталога.
- Статус: код и тест добавлены, но CI/build ещё не подтверждены. Это индексатор метаданных, не streaming playback; текущий SamplePool по-прежнему загружает WAV целиком и имеет лимиты 1 GiB/128 snapshots.
- Следующий шаг: проверить сборку на Windows CI; затем спроектировать и реализовать page cache/streaming source с тестами cache miss, prefetch, shutdown и voice concurrency.


## 10 ноября 2026 — продолжение M3: прямой dispatch событий PatternScheduler

- Рабочая ветка: `migration/full-copy-m3-transport-scheduler-v2` в целевом репозитории 4.0. Вся дальнейшая разработка, код, тесты и документация ведутся в этой ветке; `main` не изменяется.
- HEAD до изменения кода: `7523be5d1fd620e3a039101598380713819dc50d`.
- Коммит кода: `feb94fc65facf9ef12ad3a43d851e4a4764715f2`.
- Коммит обновления журнала: `ccc5e88c91a21898d519c1f93f72dc4eb7073f9a` (подробная запись также находится в `docs/ЖУРНАЛ_РАЗРАБОТКИ_RU.md`).

### Что проверено
- Повторно просмотрены `Source/PluginProcessor.cpp`, `Source/PluginProcessor.h`, `Source/Performance/PatternScheduler.h`, sampler API и PROMPT v33.
- Уточнён статус M3: текущий `processBlock()` уже вызывает `PatternScheduler::schedule()` и получает события в заранее выделенный массив. Старое утверждение, что вызов scheduler полностью отсутствует, устарело для HEAD перенесённой ветки.

### Что изменено
- `Source/PluginProcessor.cpp`: удалено промежуточное копирование scheduled events в `juce::MidiBuffer`; события обрабатываются непосредственно из фиксированного массива `scheduledPatternEvents`.
- Сохранено разбиение аудиоблока по точному `sampleOffset`: рендерится участок до события, затем вызывается `sampler.noteOn()` или `sampler.noteOff()`.
- Сохранены текущие velocity shaping, dynamics, velocityScale, MIDI channel и global tuning.
- `Source/PluginProcessor.h`: удалён ненужный после изменения член `patternMidiBuffer`.
- Причина: `juce::MidiBuffer::addEvent()` потенциально мог расширять внутреннее хранилище и выделять память в audio callback при плотном потоке событий. Прямой dispatch убирает эту промежуточную буферизацию; используется заранее выделенный массив scheduler.

### Проверка
- Git diff проверен: изменены только `Source/PluginProcessor.cpp` и `Source/PluginProcessor.h`; изменения соответствуют описанию.
- Commit `feb94fc...` опубликован в рабочей ветке без force push.
- Windows CI run `42762319535` запущен для точного commit `feb94fc...`. На момент последней проверки каталог 300 пресетов и portable preset examples прошли; скачивание redistributable sample libraries выполнялось; CMake configure, сборка VST3, CTest и bundle verification ещё ожидали выполнения.
- Сборка/CTest/pluginval на этом commit пока не подтверждены. M3 НЕ принята.

### Оставшиеся критерии M3
1. Дождаться результата CI для `feb94fc...`; ошибки исправлять по первопричине с повторным запуском проверок.
2. Проверить regression cases: точные sample offsets, совпадающие offsets (NoteOff перед NoteOn), границы блока, разные block sizes, длинные gate и отложенные NoteOff.
3. Проверить lifecycle нот: смена MIDI-аккорда, удержание, stop/reset транспорта, panic/All Notes Off, отсутствие зависших нот.
4. Проверить смену pattern slot и квантование; подтвердить восемь работающих runtime slots, а не только каталог metadata.
5. Проверить отсутствие heap allocation, locks и file I/O в audio callback инструментально или тестовым allocator/audit call path.
6. Выполнить headless/offline render acceptance и отдельно ручную проверку в FL Studio 20.
7. Не переходить к M4 до фактической приёмки M1–M3 по PROMPT v33.

### План следующих работ
- Получить полный результат текущего Windows CI.
- Провести аудит scheduler/sampler event lifecycle, добавить недостающие regression tests и устранить обнаруженные дефекты.
- Продолжать M1–M13 строго по PROMPT v33; каждый коммит сопровождать записью в обоих журналах: что сделано, какие файлы изменены, почему, результаты тестов, известные ограничения и следующий план. Не выдавать roadmap за реализованную функцию.


## 10 ноября 2026 — исправление страницы скачивания и compile blocker

- Рабочий репозиторий: `l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-`.
- Единственная рабочая ветка: `migration/full-copy-m3-transport-scheduler-v2`. `main` не изменяется.
- Проверенный HEAD до исправления: `1d3782d03b7b30709e88fa53da8feced67404575`.
- Обнаружена причина 404: `README.md` и `DOWNLOAD.md` содержали ссылки на прежний репозиторий `POP-PUNK-GUITAR-ULTIMATE`, прежний workflow run и artifact ID. Эти адреса не относятся к текущей рабочей ветке.
- Дополнительный блокер: Windows CI run `42762506212` завершился failure на этапе `Build VST3 and test executables`. MSVC C2065 подтвердил три оставшихся обращения к удалённому члену `patternMidiBuffer` в `Source/PluginProcessor.cpp`: строки 189, 190 (`prepareToPlay`) и 382 (`processBlock`). Удаление поля из заголовка уже было выполнено ранее, но эти вызовы остались.
- Исправление исходника: удалены только три устаревших обращения к `patternMidiBuffer`. MIDI-вход хоста и прямой dispatch событий из фиксированного массива `scheduledPatternEvents` сохранены.
- Исправление ссылок: страницы скачивания переведены на workflow текущего репозитория и единственной рабочей ветки. Добавлена инструкция брать только именованный artifact из последнего успешного workflow; несуществующий/устаревший прямой artifact URL удалён.
- Уточнено описание содержимого: `factory_bank_300.json` — каталог из 300 определений пресетов; это не утверждение о полной runtime-интеграции всех 300 пресетов.
- Изменяемые файлы: `Source/PluginProcessor.cpp`, `README.md`, `DOWNLOAD.md`, `docs/ЖУРНАЛ_РАЗРАБОТКИ_RU.md`, `jurnal.md`.
- Проверки до коммита: поиск по исходнику должен вернуть 0 вхождений `patternMidiBuffer`; ссылки в обеих страницах должны вести в целевой репозиторий и рабочую ветку.
- Статус сборки: после исправления требуется новый Windows CI. Пока новый run не завершился успешно, ZIP с VST3 + Samples + Presets не считается опубликованным. M3 остаётся NOT ACCEPTED; переход к M4 запрещён до критериев PROMPT v33.
- Следующий шаг: дождаться нового CI, проверить компиляцию, CTest, наличие VST3 bundle и итогового artifact; затем заменить ссылку страницы скачивания на прямую ссылку к фактически созданному artifact, если API подтвердит его ID.


## 10 ноября 2026 — исправление ссылки VST3 badge и публикация артефакта

- Причина обращения: переход по бейджу VST3 в hero-блоке README открывал сайт Steinberg, поскольку бейдж был ошибочно привязан к https://www.steinberg.net/developers/; это была информационная ссылка, не скачивание.
- Бейдж VST3 и основная кнопка DOWNLOAD теперь ведут прямо на успешный Windows workflow run #5: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42763253767.
- Подтверждён опубликованный артефакт: `POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES-AND-PRESETS`, ID `13738701864`, размер 2,242,880,406 байт; не истёк на момент проверки.
- Проверенный commit сборки: `0572fc214f615542ae594647e318422126f6d49d`; run завершён с conclusion `success`. Шаги компиляции VST3 и CTest успешны; host validation в FL Studio 20 не заявляется как выполненная.
- В `README.md` и `DOWNLOAD.md` удалены вводящие в заблуждение ссылки на Steinberg/старые workflow destinations и обновлён статус доступности. В обоих журналах исправлены устаревшие строки репозитория и ветки; исторические записи сохранены.
- Ограничения: Actions artifact временный, не является постоянным GitHub Release; 300 preset definitions в JSON не доказывают runtime-доступность всех 300 пресетов; M3 остаётся NOT ACCEPTED, M4 заблокирован до прохождения критериев PROMPT v33 FINAL.
- Следующий шаг: продолжить аудит M1–M3 по acceptance checklist, исправлять подтверждённые блокеры в этой ветке и повторять Windows CI/CTest; после этого выполнить отдельную проверку VST3 в FL Studio 20.


## 10 ноября 2026 — упрощение страницы скачивания

- По просьбе пользователя удалены дополнительные ссылки и поясняющие блоки, которые не ведут непосредственно к получению архива: ссылка на список всех запусков, дублирующие описания workflow и лишние инструкции о статусах.
- В `README.md` оставлена одна основная ссылка скачивания на проверенный успешный Windows run: https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42763253767.
- В `DOWNLOAD.md` оставлена одна основная ссылка и короткая последовательность: открыть run → Artifacts → скачать именованный ZIP.
- Имя опубликованного архива: `POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES-AND-PRESETS`, artifact ID `13738701864`, размер около 2.24 GB. Успешная сборка/CTest подтверждены; ручная проверка в FL Studio 20 не заявляется.
- Изменены: `README.md`, `DOWNLOAD.md`, `jurnal.md`, `docs/ЖУРНАЛ_РАЗРАБОТКИ_RU.md`. Исторические записи сохранены.
- Ограничения: ссылка ведёт на страницу успешного GitHub Actions run, где сам ZIP доступен в секции Artifacts; GitHub может потребовать авторизацию. Artifact временный, не постоянный Release. M3 остаётся NOT ACCEPTED; M4 заблокирован.
- Следующий шаг: продолжать аудит и исправления M1–M3 по PROMPT v33 FINAL в этой же ветке.


## 10 ноября 2026 — прямая ссылка на ZIP

- Пользователь запросил прямое скачивание без страницы GitHub Actions и без ручного поиска в Artifacts.
- В README и DOWNLOAD.md основная кнопка заменена на официальный GitHub Actions artifact archive endpoint: https://api.github.com/repos/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/artifacts/13738701864/zip
- Проверено через список артефактов запуска https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42763253767: artifact ID `13738701864`, имя `POP-PUNK-GUITAR-ULTIMATE-Windows-x64-VST3-WITH-SAMPLES-AND-PRESETS`, размер `2,242,880,406` байт, `expired=false`, срок действия до `2027-02-08T05:37:05Z` на момент проверки.
- API отдаёт HTTP redirect на временный URL скачивания (срок действия redirect URL — около 1 минуты); постоянной является сама API-ссылка, пока артефакт и запуск существуют. GitHub может потребовать авторизацию в зависимости от доступности репозитория.
- Изменены: `README.md`, `DOWNLOAD.md`, `jurnal.md`, `docs/ЖУРНАЛ_РАЗРАБОТКИ_RU.md`.
- M3 остаётся NOT ACCEPTED; M4 не начинается.


## 10 ноября 2026 — главная страница и прямые загрузки

- README download section and DOWNLOAD.md were simplified to one-click direct ZIP download, with the package contents and installation steps stated clearly.
- Removed stale CI run links that returned 404 and corrected the Windows build verification link to the actual repository/run.
- VST3 badge now links to the local download section rather than pretending to be a separate VST3 file download.
- Verified artifact listing for run 42763253767: package artifact ID 13738701864, 2,242,880,406 bytes, not expired at check time. Direct archive endpoint: https://api.github.com/repos/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/artifacts/13738701864/zip
- The archive includes the VST3 bundle, Samples folder and preset catalogue. There is no separately published standalone .vst3 asset; the page says so rather than exposing a nonexistent link.
- Manual DAW validation remains outstanding; M3 remains NOT ACCEPTED and M4 remains gated.
- Files changed: README.md, DOWNLOAD.md, jurnal.md, docs/ЖУРНАЛ_РАЗРАБОТКИ_RU.md.


## 10 November 2026 — premium README, promo download and creator contact links

- По просьбе пользователя обновлена главная страница README: более выразительная продуктовая структура, крупный promo download CTA, блоки возможностей, sound engine, Melody/Pattern, preset catalogue, sample licensing, development status, roadmap, документация и контакты.
- Добавлены прямые контакты автора: mailto:l3xtown@yandex.ru, https://beatmaker.tv/user/182573 и https://t.me/l3xtown.
- Добавлена заметная ссылка **DOWNLOAD PROMO VST3 BUILD** в hero-блоке и большая кнопка скачивания в разделе DOWNLOAD. Обе ведут на проверенный ZIP artifact ID 13738701864 (run 42763253767), содержащий VST3 + Samples + Presets, размер около 2.24 GB.
- Добавлены отдельные ссылки на доступные в репозитории JSON-каталоги пресетов и portable Melody/Pattern examples. Не опубликованные отдельно VST3 и Sample Bank не выданы за самостоятельные файлы: указано, что они входят в комплектный ZIP.
- Сохранены точные ограничения проекта: 300 preset definitions не равны 300 готовым runtime presets; M3 остаётся NOT ACCEPTED; M4 заблокирован; ручная проверка в FL Studio 20 и performance benchmarks не заявлены как выполненные.
- Изменены: README.md, DOWNLOAD.md, jurnal.md, docs/ЖУРНАЛ_РАЗРАБОТКИ_RU.md.
- Проверки: структура Git tree подтверждает наличие JSON preset banks и двух portable example files; ссылка на artifact основана на ранее проверенном artifact ID и run. Новая сборка/CTest не запускались, так как изменялись только Markdown-документы.
- Ограничение: GitHub Actions artifact временный; отдельные standalone VST3 и Sample Bank release assets отсутствуют. Не обещать бессрочную публичную загрузку.
- Следующий шаг: продолжить M1–M3 по PROMPT v33 FINAL в этой же рабочей ветке; после acceptance подготовить постоянную Release-публикацию, если доступные права и upload-инструменты позволят.


## 10 ноября 2026 — M1: регрессионная защита APVTS

- В ходе аудита M1 подтверждено: параметры создаются через APVTS в `createParameterLayout()`, аудиопоток читает 47 параметров через `getRawParameterValue()`, а сохранение/восстановление состояния использует JUCE XML binary helpers.
- Выявленный риск: в CI отсутствовала проверка стабильности идентификаторов APVTS. При переименовании или удалении ID старые DAW-проекты и automation lanes могли перестать корректно связываться с параметрами, а несовпадение ID в аудиокоде могло остаться незамеченным до запуска плагина.
- Добавлен `tools/validate_apvts_layout.py`: проверяет сохранение 96 текущих идентификаторов APVTS (включая 7 генерируемых FX slots), отсутствие дублирующихся литеральных ID, наличие каждого `getRawParameterValue()` в реестре и наличие полного пути сериализации/восстановления APVTS state.
- Workflow `.github/workflows/vst3-build.yml` запускает проверку до конфигурации и сборки VST3. Проверка не подменяет ручную проверку automation в FL Studio 20 и не доказывает runtime-поведение хоста.
- Изменённые файлы: `tools/validate_apvts_layout.py`, `.github/workflows/vst3-build.yml`, `jurnal.md`, `docs/ЖУРНАЛ_РАЗРАБОТКИ_RU.md`.
- Проверка на момент записи: исходный код проаудирован по GitHub; CI будет запущен изменением workflow после push. Результат нового CI/CTest ещё не получен. M1 НЕ ПРИНЯТ: ручная проверка автоматизации параметров во FL Studio 20 остаётся обязательной.
- Следующий шаг: дождаться Windows CI, исправить возможные ошибки проверки/сборки, затем провести host automation acceptance. После этого продолжить закрытие M2/M3; M4 остаётся заблокирован.


## M3 — исправление транспорта и sample-accurate смены аккордов — 10 ноября 2026

Проведён повторный разбор M3. Выявлены реальные дефекты production path: held MIDI chord обновлялся после планирования, из-за чего новый аккорд начинал звучать на один блок позже; samplePosition MIDI не учитывался при смене аккорда; смена аккорда не имела отдельного 1/16 quantize; seek/переключение источника транспорта могли оставить устаревшие NoteOff в очереди; тест near-block-edge был недостаточно точным.

Исправления в текущей рабочей ветке:
- MIDI held-state теперь применяется до построения chord snapshot; учитывается samplePosition последнего события изменения аккорда.
- Старая нота/аккорд рендерится до MIDI sample offset, затем отпускается; новый chord cycle стартует в sample-accurate позиции либо на следующей 1/16 границе. Опция Start On First Chord сохраняет немедленный старт первого аккорда; при отключённой опции первый старт остаётся привязан к следующему такту.
- На смене аккорда, transport stop и seek очищаются отложенные NoteOff и отпускаются прежние голоса; при stop также сбрасывается held chord, чтобы не возобновлять его самопроизвольно.
- Scheduler сообщает о host PPQ discontinuity и о переключении Host/Internal источника; stale gates очищаются.
- Sample offset переводится в индекс первого sample не раньше события (floor), вместо прежнего округления, которое могло сдвигать near-tail события на один sample назад.
- Pending NoteOff queue увеличена в четыре раза; при переполнении выставляется флаг, а Processor выполняет all-notes-off fail-safe.
- Добавлены headless regression cases на host seek, tempo/block-size change, near-tail sample mapping и forward 1/16 quantization.

Commit: b3c5e10f6986c31697ad6b66fa2ce035c499bb96 (первый M3 patch); последующий commit содержит корректировки и журналы.

Статус M3: NOT ACCEPTED до успешного Windows CI, проверки integration на реальном PluginProcessor и дополнительных headless сценариев (Host Start/stop/restart, host↔internal fallback, pending gate cleanup, event-capacity overflow, buffer-size changes 64↔1024). Ручная проверка FL Studio 20 также обязательна. M4 остаётся заблокирован.


## M3 follow-up — clock-source handoff and seek tolerance

Дополнительно закрыты два края транспортной логики: переключение Internal → Host теперь тоже считается discontinuity (раньше явно обрабатывался только Host → Internal), а порог определения seek снижен с 0.25 beat до адаптивного 0.01 beat / двух длительностей блока, чтобы короткие перемотки не оставляли устаревшие gate events. Очистка pending NoteOff одновременно сбрасывает overflow flag. Добавлен headless тест обеих сторон handoff.

Эти изменения отправлены отдельным последующим commit; Windows CI ещё требуется.


## 10 November 2026 — M3: apply 1/16 and 1/32 pattern quantization

- Найдена конкретная причина неполной реализации M3: APVTS-параметр `patternQuantize` существовал и предлагал 1/16 и 1/32, но аудиодвижок жёстко использовал только 1/16. Смена pattern slot и pattern length также не перезапускала фразу на квантованной границе.
- Исправлено в `Source/PluginProcessor.cpp`: выбранная сетка теперь читается из APVTS (`0.25` четверти для 1/16, `0.125` для 1/32); последующие смены аккорда, слота и длины паттерна запускаются на ближайшей будущей границе. Событие MIDI по-прежнему учитывает точный sample offset; первый аккорд сохраняет настройку Start on First Chord. Свинг шагов не квантуется повторно.
- В `Source/PluginProcessor.h` добавлено отслеживание предыдущего pattern slot и length; `prepareToPlay` очищает эти поля при повторной подготовке аудиодвижка.
- В `tests/PatternSchedulerTests.cpp` добавлена headless-проверка сетки 1/32 наряду с проверкой 1/16.
- Исправлена причина предыдущего CI failure: в `tools/validate_apvts_layout.py` regex имели двойное экранирование. При повторной статической сверке найдено 97 уникальных объявленных APVTS ID (включая 7 FX slots) и 48 обращений аудиокода; все обращения имеют соответствующие объявления. Это статическая проверка, не замена DAW automation test.
- Изменённые файлы: `Source/PluginProcessor.cpp`, `Source/PluginProcessor.h`, `tests/PatternSchedulerTests.cpp`, `tools/validate_apvts_layout.py`.
- Проверка: на текущем HEAD `c8da085e45fdd11387d01a0f69538e771e92d967` шаги CI для валидатора пресетов и APVTS завершились успешно; Windows build/CTest ещё выполняются. M3 пока NOT ACCEPTED: требуется дождаться полного CI/CTest и закрыть оставшиеся критерии lookahead/preview и проверки в хосте. M4 остаётся заблокирован.


## Журналирование и баг-трекинг — 10 ноября 2026

Добавлены `docs/WORK_LOG_M3.txt` и `docs/ЖУРНАЛ_БАГОВ_RU.md`. В баг-журнале заведены M3-001…M3-012; исправления исходников помечены FIXED — AWAITING CI, непроверенная сборка и DAW integration остаются OPEN. M3 не принят.


## 10 ноября 2026 — Info: единый сквозной журнал и аудит M1–M3

Создан корневой `Info.txt` как единый хронологический журнал разработки от исходного ТЗ до завершения проекта. В него включены исторические записи предыдущего `jurnal.md`, M3 work log, текущее состояние, аудит M1/M2/M3, результаты CI, выявленные дефекты и план последующих исправлений. Отдельный баг-трекер сохранён в `docs/ЖУРНАЛ_БАГОВ_RU.md`.

По run `42769587683` Windows Release build/VST3 bundle прошли, но CTest завершился FAILURE: 5/6, упал `ppgu_pattern_scheduler_tests`. Найдено и исправлено дробное sample offset: 63.6 samples ошибочно усекались до 63 вместо 64; добавлен тест exact integer sample. Исправление ожидает нового CI.

Аудит: M1 — не сохраняется sample-library reference и нет полного state/automation/host acceptance; M2 — WAV preload в RAM, page cache не подключён к voice read path, RR/velocity crossfade/unload не подтверждены; M3 — дробный offset исправлен в исходнике, но остаются потеря нескольких chord transitions в одном блоке и отсутствие явного scheduler output overflow handling. M1/M2/M3 остаются NOT ACCEPTED; M4 BLOCKED.


## 10 ноября 2026 — M3-015: явная обработка переполнения output scheduler

- Ветка: `migration/full-copy-m3-transport-scheduler-v2`
- Изменения кода: `Source/Performance/PatternScheduler.h`, `tests/PatternSchedulerTests.cpp`.
- Root cause: выходной массив scheduler ограничен 512 событиями, но цикл генерации завершался при нехватке двух мест без выставления статуса. Это могло молча отбросить NoteOn/NoteOff; потерянный NoteOff создаёт риск зависшей ноты.
- Fix: при нехватке места выставляется `outputOverflow_`; `consumePendingOverflow()` теперь объединяет переполнение выходного массива и pending queue и сбрасывает оба флага после чтения. Processor уже обрабатывает этот статус через `sampler.allNotesOff()` как fail-safe.
- Regression: плотный pattern из 300 одновременных событий заполняет output до 512 и обязан вернуть overflow status; повторное чтение статуса должно вернуть false.
- Коммиты: `427a68506599b7f8ea8250b253dbc89ede4fe4fb` (scheduler), `6a77e2d1f33f6a4ad730d4c5a6c7997d2a5112cf` (test).
- Проверки: исходники и regression test записаны в рабочую ветку. Windows Release build/CTest для нового HEAD ещё не получен; статус FIXED IN SOURCE / AWAITING CI.
- M3-014 остаётся FIXED IN SOURCE / AWAITING CI; M3-015 теперь FIXED IN SOURCE / AWAITING CI. M1/M2 не затрагивались; M1, M2, M3 не приняты; M4 BLOCKED.
- Следующий шаг: получить Windows CI для актуального HEAD, исправить все обнаруженные ошибки; после зелёного CTest продолжить интеграционные M3 проверки (stop/seek, variable block sizes, dense pattern fail-safe) и затем отдельный DAW acceptance.


## 10 ноября 2026 — M3-015 CI failure: missing timeline helper include и исправление

- Проверены Windows Actions runs `42774950334` (кодовый commit `6a77e2d1...`) и `42774991354` (документационный commit `efb30595...`). Оба завершились `failure` на шаге `Build VST3 and test executables`; CMake configure прошёл, CTest был пропущен, VST3 bundle/artifact не создан.
- Точная ошибка MSVC: `PluginProcessor.cpp(505): processSampleAccurateMidiTimeline is not a member of ppgu::performance` / `identifier not found`.
- Root cause: `PluginProcessor.cpp` вызывает helper из `Source/Performance/ChordTransitionTimeline.h`, но не включал этот заголовок. Тестовый файл включал его напрямую, поэтому дефект не обнаруживался unit-test compilation.
- Исправление: добавлен прямой `#include "Performance/ChordTransitionTimeline.h"` в `Source/PluginProcessor.cpp`. Commit: `6de1db6777a7afa4dc818fab244c06b3dc099b4d`.
- В логах также присутствуют предупреждения C4244 в PluginEditor.cpp и C4458 для локального `playHead`; они не являются причиной текущего failure и остаются отдельными cleanup-кандидатами.
- Статус исправления: FIXED IN SOURCE / AWAITING WINDOWS CI. Новый build и CTest ещё не запускались/не подтверждены. M3-014 и M3-015 не закрывать до успешного Release build + CTest.
- Следующий шаг: дождаться CI на commit `6de1db...`; если появятся новые ошибки — разбирать по первой реальной compiler/test failure, не отключая тесты. После успешного CTest продолжить M3 stop/seek, block-size/tempo stress и fail-safe проверки. M1/M2/M3 остаются NOT ACCEPTED; M4 BLOCKED.


## 10 ноября 2026 — малые исправления качества кода

В рамках параллельного лёгкого аудита без переключения с основной работы M1–M3 исправлены два подтверждённых предупреждения MSVC: явные float-преобразования координат в отрисовке редактора (C4244) и затенение члена класса локальным `playHead` (C4458). Код изменён в `PluginEditor.cpp` и `PluginProcessor.cpp`; проверка свежей Windows CI ещё ожидается. Основной приоритет остаётся за функциональными критериями M1–M3.


## 10 ноября 2026 — небольшой фронтенд-фикс поиска пресетов

- Поиск пресетов расширен на имя, категорию/референс, режим, характер звучания, groove и articulation.
- При изменении фильтра текущий выбранный пресет сохраняется, если он остаётся в результатах; если не остаётся — список очищает выбор, но не переключает звучание на другой пресет автоматически.
- Добавлен явный `<algorithm>` include для `std::find`/`std::distance`.
- Изменение затрагивает только удобство браузера пресетов и не меняет аудиодвижок, параметры звука или основной план M1–M3.
- Статус: FIXED IN SOURCE; ожидается Windows Release build и CTest после коммита.


## 10 ноября 2026 — минимальный размер окна редактора

- Исправлено несоответствие: редактор разрешал уменьшение до 650 px по высоте, хотя текущая фиксированная вертикальная компоновка требует около 788 px с внешними отступами.
- Минимальная высота окна изменена на 790 px, чтобы клавиатура и нижние элементы не обрезались при допустимом размере окна.
- Это ограничение согласовано с текущей геометрией `resized()`; адаптивная перестройка для компактных окон остаётся отдельной задачей, не блокирующей основную работу.
- Статус: FIXED IN SOURCE; ожидается Windows Release build.


## 10 ноября 2026 — сохранение выбранной SFZ-библиотеки в состоянии проекта

- `getStateInformation` теперь сохраняет полный путь выбранной SFZ-карты в свойство `sfzFilePath` дерева APVTS вместе с параметрами.
- `setStateInformation` восстанавливает параметры и запускает загрузку найденной SFZ-карты через существующий фоновый `ThreadPool`; загрузка не выполняется синхронно в callback восстановления состояния хоста.
- Добавлена блокировка `CriticalSection` для доступа к пути из state callback, UI/test API и фонового загрузчика.
- Старые проекты без свойства `sfzFilePath` продолжают загружаться как раньше. Если файл библиотеки отсутствует, восстановление параметров не прерывается, а сохранённый путь остаётся доступен для повторного поиска.
- Расширен `tools/validate_apvts_layout.py`: CI проверяет наличие сериализации, восстановления, проверки файла и асинхронного вызова.
- Статус: FIXED IN SOURCE; ожидает Windows Release build + CTest. Поведение в FL Studio 20 ещё не подтверждено.


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


## 11 ноября 2026 — спецификация премиального интерфейса и Skin-системы

- Принято направление качества «дорого, богато»: цель — премиальный законченный интерфейс с выверенной типографикой, сеткой, отступами, иерархией, материалами, аккуратными состояниями элементов, хорошей читаемостью и корректным масштабированием.
- Для смены оформления оставить одну общую кнопку Skin/Theme. По нажатию открывается выбор одной из 10 отдельных тем из каталога SKIN. Не создавать 10 отдельных кнопок. Тема должна применяться ко всем режимам и сохраняться после перезапуска.
- MELODY и PATTERN имеют собственные независимые обложки в RESOURCE. Обложки не являются Skin. Смена режима меняет обложку режима, но не текущую тему интерфейса.
- Зафиксирована целевая структура папки продукта: POP PUNK GUITAR ULTIMATE/{VST3, BANK, PRESETS, SKIN, RESOURCE, README}; внутри SKIN — Skin_01…Skin_10; внутри RESOURCE — MELODY, PATTERN, Icons, Graphics. Не добавлять самовольно каталоги VST/ или Themes/ и не менять существующие пути до аудита packaging/CMake.
- Проведён предварительный GitHub audit: существует ветка feat/theme-folder-system, но её README не подтверждает готовность 10 тем, единой кнопки выбора, независимых обложек или сохранения темы. Поэтому функциональность не отмечается как реализованная.
- Приоритеты оптимизации зафиксированы: не ослаблять тесты; сначала проверить текущий HEAD/CI и критические M2 streaming + M3 Pattern runtime, затем интегрировать/проверить UI skins и структуру пакета; провести Windows x64 Release, CTest, pluginval и ручную проверку в FL Studio 20.
- Статус: SPECIFIED / IMPLEMENTATION AND QA PENDING. В этой сессии исходный код плагина не изменялся; внесена только запись требований и результатов аудита в журналы.


## 11 ноября 2026 — REV 6.0 VST3: актуальный main и следующий критический gate

- Официальное название релиза: **POP PUNK GUITAR ULTIMATE REV 6.0 VST3**.
- Рабочая ветка подтверждена пользователем: `main`. Устаревшую строку о migration branch в предыдущих исторических записях считать superseded; никакие резервные ветки не менять.
- HEAD проверен: `27abf1313d707f9df4c0da0b77c044c18e8977ab`. Для этого SHA GitHub combined status не содержит проверок, а список workflow runs пуст. Актуальные Windows Release build и CTest не подтверждены.
- M3-014 source path присутствует в `PluginProcessor.cpp`: MIDI timeline разделяет блок по sample offsets, рендерит каждый интервал до применения следующего MIDI event, обновляет held chord и передаёт scheduler events в sampler. Unit regression на sample offsets 16/80/120 проверяет chord snapshots, но не запускает настоящий PluginProcessor→PatternScheduler→SamplerEngine путь.
- Риск для отдельной проверки: несколько chord transitions внутри одной quantize window могут повторно назначать одинаковый `patternCycleStartPpq`; требуется regression, которая фиксирует требуемую музыкальную семантику, а не только состояние held notes. Также проверить очистку pending NoteOff при нескольких сменах аккорда.
- M3-015 source path присутствует: лимит 512 событий, overflow indication и processor fail-safe. Текущий CTest не подтверждён для HEAD.
- CMake регистрирует helper/scheduler tests; полноценного processor-level integration target не обнаружено.
- GUI blueprint и документ PPGU 5.0 UI/UX acceptance не найдены в main. UI/UX implementation остаётся заблокированной до приёмки M1–M3.
- Код плагина в этом цикле не менялся. Документация синхронизирована; не заявлять build/test/runtime completion.
- Следующий шаг по протоколу: M3-014 integration regression + quantize-window/pending-NoteOff tests; затем M3-015 CI gate. После успешного CI пользователь подтверждает запуск workflow; M1/M2 acceptance выполняются до UI/UX 5.0 и M4+.


## 11 ноября 2026 — REV 6.0: ускоренный source audit M3-014

- Повторно подтверждён актуальный `main`: HEAD до этой документационной записи `c0cd94eb897bd22fdcc13f0b78050d0d13a7c5a3`; combined status пуст, workflow runs для commit не возвращены.
- Свежая проверка `PluginProcessor.cpp` показывает production путь MIDI timeline → renderPatternInterval → PatternScheduler::schedule → sampler.noteOn/noteOff. Однако существующий PatternSchedulerTests тестирует helper/timeline и scheduler отдельно; отдельного тестового target, создающего PluginProcessor и проверяющего его sampler dispatch, в CMake нет.
- Найден конкретный риск в `restartForChord`: chord change немедленно обрывает все sampler voices и удаляет pending NoteOff, хотя `patternCycleStartPpq` для следующего аккорда может быть отложен до следующей 1/16 или 1/32 границы. Это может создавать преждевременную тишину до quantize boundary. При нескольких сменах внутри одной сетки переходы могут схлопываться к последнему состоянию.
- Код не менялся: музыкальный контракт не установлен достоверно из доступного в main `Promt.txt v33 FINAL` (файл с таким точным путём не найден). До исправления нужно найти действующий файл ТЗ и выбрать ожидаемую семантику: immediate switch, deferred switch with old pattern continuation, или latest-chord coalescing.
- Следующий инженерный шаг: найти реальный путь к v33 FINAL в репозитории; зафиксировать contract test на нескольких chord transitions внутри quantize window и pending NoteOff; затем внести минимальный source fix и настоящий processor-level integration harness. Не считать helper test end-to-end acceptance.
- Результат этого цикла: source audit + уточнение M3-014 в баг-реестре; production code/build/test не менялись и не запускались. M3-014 OPEN, M3-015 FIXED IN SOURCE / AWAITING CI, M1/M2/M3 не приняты.


## 11 ноября 2026 — M3-014: добавлен regression target

- Добавлен автономный тест tests/ChordTransitionPolicyTests.cpp.
- Проверяются решения policy для первого аккорда, ожидания следующего такта, переходов non-empty → non-empty, повторных переходов в одном окне, перехода к пустому аккорду и невалидного PPQ.
- Добавлен сценарий с реальным PatternScheduler: ранее поставленный NoteOff старого аккорда должен пережить переход к новому аккорду и быть выдан на исходной PPQ-границе.
- Новый CTest target ppgu_chord_transition_tests зарегистрирован в CMakeLists.txt.
- Статус: тестовый код добавлен в main, но не запускался. Нет подтверждённой сборки, CTest или DAW проверки. M3-014 остаётся OPEN; не объявлять результат рабочим до получения Windows CI/CTest и processor-level/host evidence.


## 11 ноября 2026 — THEME-FORGE MAX+ / SKIN REV 5.0 / Skin_12–Skin_21

- Добавлены десять самостоятельных data-only манифестов: Arctic Blue, Toxic Green, Purple Voltage, Blood Red, Ocean Stage, Golden Era, Mono Chrome, Festival Lights, Studio Olive, Neon Flagship.
- Каждая тема содержит собственные HEX-палитры и полный набор токен-групп schema 1.0.0, включая thumbnails.
- Обновлён SKIN/theme_manifest.json; реестр содержит 20 тем Skin_02–Skin_21. Skin_01/Neon Riot остаётся дефолтом.
- AssetSpec описывает preview 480x300 PNG, hero 1200x420 WebP и optional seamless texture. Файлы изображений не созданы.
- В этом цикле production C++ не менялся. Runtime, schema validation, contrast automation, inventory matrix, Windows build/CTest и FL Studio 20 не проверялись.
- Статус: SPEC READY (manifest authored) — pending independent schema/contrast validation, assets, runtime and user acceptance. CI: AWAITING CI.


## 11 ноября 2026 — SKIN REV 5.0 / выравнивание контракта токенов

- Контракт SKIN/theme.schema.json расширен: thumbnails включены в обязательные группы токенов; заданы базовые ограничения верхнего уровня и структуры пакета.
- Skin_02–Skin_11 обновлены группой thumbnails: background, border, selected, caption.
- Повторно прочитаны из GitHub main: Skin_02–Skin_11 и Skin_12–Skin_21 корректно разбираются как JSON; все 18 обязательных групп токенов присутствуют в 20/20 манифестах.
- Проверка пока не равна полной реализации JSON Schema draft 2020-12 validator, автоматическому contrast test или runtime validation.
- Artwork assets отсутствуют; ThemeManager/gallery и полная APVTS inventory matrix не подтверждены. CI: AWAITING CI.


## 11 ноября 2026 — SKIN REV 5.0 / Skin_22–Skin_31

- Добавлены 10 манифестов: Deep Space Studio, Velvet Circuit, Amber Tube Room, Acoustic Atelier, Ivory Session, Chrome Velocity, Clear Signal, Prism Deck, Pure Control, Skate Voltage.
- В каждом присутствуют 18 групп токенов, включая thumbnails; манифесты содержат собственные палитры и assetSpec.
- Реестр theme_manifest.json расширен до 30 записей Skin_02–Skin_31.
- Artwork не создан. Runtime ThemeManager/gallery, полная JSON Schema validation, контрастные тесты, APVTS matrix, Windows build/CTest и FL Studio 20 не проверялись. CI: AWAITING CI.


## 11 ноября 2026 — SKIN REV 5.0 / Skin_32–Skin_41

- Добавлены десять самостоятельных манифестов: Gig Poster Luxe, Checker Riot, Midnight City, Arena Pulse, Sticker Luxe, Carbon Sprint, Electric Boulevard, Aftershow Gold, Signal Redline, Arctic Glasshouse.
- Реестр обновлён до 40 записей Skin_02–Skin_41. Каждая тема содержит 18 token groups, собственные палитры и assetSpec.
- Preview/hero/texture файлы отсутствуют. Полная JSON Schema validation, runtime, matrix, Windows CI и FL Studio 20 acceptance не выполнялись. CI: AWAITING CI.


## 11 ноября 2026 — SKIN REV 5.0 / Skin_42–Skin_50

- Добавлены последние девять манифестов: Moss Circuit, Copper Workshop, Mono Blueprint, Rose Quartz, Solar Bronze, Teal Machine, Concrete Stage, Ultraviolet Club, Platinum Room.
- Реестр содержит Skin_02–Skin_50 (49 новых идентичностей); Skin_01 Neon Riot остаётся дефолтом. Всего 50 слотов.
- Все манифесты data-only, имеют 18 token groups и assetSpec. Artwork отсутствует; runtime, matrix, automated validation, Windows CI и FL Studio 20 acceptance не проверялись. CI: AWAITING CI.


## 11 ноября 2026 — SKIN REV 5.0 / полный каталог и аудит интеграции

- Создан Skin_01/theme.json для Neon Riot; registry содержит 50 уникальных записей Skin_01–Skin_50.
- Создана CONTROL_INVENTORY_MATRIX_RU.md по полному GUI blueprint: отдельно отмечены APVTS IDs и наличие фактического GUI в main. Матрица специфицирована, но runtime полнота не подтверждена.
- Прочитан GUI_UX_BLUEPRINT_RU.md из PR #24 branch. PPGU_5_0_UI_UX_FUNCTIONAL_ACCEPTANCE_RU.md не найден в проверенных деревьях main/PR24.
- PR #24 reviewed: не безопасен для merge as-is. Он добавляет PopupMenu THEMES вместо полноценной SKIN gallery, использует несовместимый embedded Themes manifest, меняет FX labels/min window size и вносит несвязанные изменения PluginProcessor audio path. Аудит сохранён в SKIN/PR24_INTEGRATION_REVIEW_RU.md; merge не выполнялся.
- Подготовлены индивидуальные asset briefs для всех 50 слотов в ASSET_SPECIFICATIONS_RU.md. Графические файлы отсутствуют.
- Статусы: SPECIFICATION AUTHORED 50/50; MANIFEST VALIDATED 0/50; RUNTIME APPLIED 0/50; USER-CONFIRMED 0/50; ACCEPTED 0/50. CI: AWAITING CI.
