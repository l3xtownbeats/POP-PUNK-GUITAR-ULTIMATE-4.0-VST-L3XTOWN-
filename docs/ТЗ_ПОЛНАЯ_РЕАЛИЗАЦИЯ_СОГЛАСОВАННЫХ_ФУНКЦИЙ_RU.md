# ТЕХНИЧЕСКОЕ ЗАДАНИЕ — ПОЛНАЯ РЕАЛИЗАЦИЯ СОГЛАСОВАННЫХ ФУНКЦИЙ
## POP PUNK GUITAR ULTIMATE 4.0

Дата фиксации: 10.11.2026  
Ветка: `fix/m1-m2-m3-release-gates`  
Статус: **ОБЯЗАТЕЛЬНО К РЕАЛИЗАЦИИ. Документ фиксирует объём работ, а не заявляет о готовности функций.**

## 1. Обязательное решение и область действия

Все функции, дополнения, форматы, режимы, элементы интерфейса, звуковые возможности, параметры, библиотеки, пресеты и критерии, которые пользователь согласовал в этой ветке и которые описаны в данном ТЗ и документе «Полное описание функций и изменений ветки», являются обязательным объёмом конечного продукта. Их нельзя считать просто идеями, справкой или необязательным списком пожеланий.

Это ТЗ объединяет уже согласованные предложения, в том числе импорт звуков и форматы, SFZ, Melody/Pattern, AutoChords/Fingering, Style DNA, эффекты, банки/пресеты, производительность, GUI, MIDI/automation, сохранение состояния и host acceptance. Новые утверждённые пользователем дополнения автоматически добавляются в этот обязательный backlog и фиксируются во всех соответствующих журналах.

Каждый пункт проходит путь: **аудит исходников → реализация → regression tests → актуальный Windows x64 Release build + полный CTest → pluginval → ручная проверка FL Studio 20, где применимо → фиксация commit SHA, CI run и доказательств в журналах**.

Наличие класса, заголовка, APVTS-параметра, пункта меню, каталожной записи, пустого DSP-модуля или успешной компиляции не считается готовой функцией.

## 2. Порядок работ и защита основной линии

- **Этап 1 — M1–M5:** максимальный приоритет; исправления и интеграция выполняются первыми.
- **Этап 2 — M6–M10:** пока Этап 1 активен, разрешены только лёгкие независимые подготовительные задачи (аудит, лицензии, каталогизация, спецификация и проверки без тяжёлых runtime-интеграций).
- **Этап 3 — M11–M13:** полноценная реализация после подтверждённой приёмки M1–M5; заранее допускается только безопасная подготовка.
- Сохранять зависимости и порядок из Promt.txt v33 FINAL и MASTER_PRODUCT_SPEC_4_0_RU.md.
- Не отключать тесты, не снижать строгость CI и не маскировать ошибки ради скорости.
- Сначала устранять root cause; объединять связанные изменения компактными пакетами, не создавая конкурирующие тяжёлые CI-запуски.
- Не переводить функцию в DONE без всех применимых доказательств и приёмки.

## 3. Импорт звуков и управление библиотеками

### IMP-SND-01 — входные форматы
Обязательные форматы импорта: **WAV, AIFF, AIF, FLAC, OGG Vorbis, MP3**. Каждый формат должен иметь реально включённый decoder в Windows Release build и отдельный fixture-тест чтения, декодирования и успешной публикации.

**NCW не поддерживается** как входной формат. Для NCW выводить отдельное понятное предупреждение и инструкцию экспортировать материал из Kontakt в WAV или AIFF. Нельзя показывать NCW среди поддерживаемых форматов.

### IMP-SND-02 — внутренний аудиоформат
Согласованное требование продукта: преобразовывать импортируемые аудиофайлы во внутренний **WAV IEEE Float 32-bit**, сохраняя sample rate и mono/stereo layout, если ограничения движка не требуют документированного исключения. Проверять bit depth по заголовку и чтением обратно.

В текущей истории INFO/work log присутствует противоречивая запись о временном переходе на 24-bit PCM из-за совместимости старого JUCE API. Это противоречие требуется разрешить: проверить реальный writer/build и привести код, тесты и документацию к согласованному Float32 требованию либо до принятия альтернативы запросить явное решение пользователя. До этого пункт не считать закрытым. Исходные файлы не изменять.

### IMP-SND-03 — безопасный импорт
- Системный folder browser и понятная кнопка IMPORT SOUNDS.
- Сканирование и декодирование в worker pool; никакого файлового I/O в audio callback.
- Staging и атомарная публикация целой библиотеки.
- Path traversal/symlink escape protection, проверка типов файлов, лимиты и безопасная обработка больших наборов.
- Manifest с исходным именем, форматом, sample rate, каналами, длительностью, root-key/mapping status, provenance, лицензией и checksum.
- Отмена импорта, отчёт ошибок, повторный импорт без повреждения существующих данных, rescan, безопасное отключение/удаление.
- SHA-256 для проверки целостности, обнаружение отсутствующих/изменённых samples.
- Защита от дубликатов, повреждённых файлов, частично скопированных банков и некорректных имён.

### IMP-SND-04 — mapping и SFZ generation
- Автоматически распознавать root key только при однозначных данных из имени/метаданных.
- Sample без известной ноты импортировать, сохранять и помечать NEEDS MAPPING; никогда не назначать случайный root key.
- Генерировать SFZ для mapped samples и проверять каждый sample path после atomic publish.
- Реализовать Mapping Editor для root key, key range, velocity range/layers, articulation, round-robin group, tune, gain, pan, offsets, loop/release.
- Добавить sample preview до выбора/назначения и понятное отображение unmapped samples.
- SFZ должен ссылаться на финальную опубликованную папку, а не staging path.

### IMP-SND-05 — лицензии и sample assets
Проверять точные версии и лицензии Meta Open Sound Steel/Nylon, FreePats и других кандидатов. Хранить источник, точную лицензию/версию, attribution, разрешённые способы распространения и SHA-256. В дистрибутив включать только материалы, лицензия которых явно разрешает такое распространение. Пользовательские локальные samples не включать автоматически в общий installer. Коммерческие библиотеки, песни и stems без прав не использовать.

## 4. SFZ, sampler, streaming и polyphony

### SMP-01 — SFZ runtime
Реализовать согласованный используемый поднабор SFZ: global/master/group/region inheritance, sample, lokey/hikey, pitch_keycenter, lovel/hivel, tune, volume, pan, offset/end, loop modes, release, key/velocity tracking, group/trigger semantics. Для каждого opcode зафиксировать поддержку, игнорирование с диагностикой или отказ с понятной ошибкой. Полную совместимость со SFZ не заявлять без тестового покрытия.

### SMP-02 — зоны и артикуляции
Реальное воспроизведение key zones, velocity layers и crossfade, round robin, отдельных palm-mute/open/clean/crunch/acoustic/lead articulations при наличии подходящих записей. Palm mute, в том числе управление снижением velocity/threshold, должен переключать артикуляцию или sample zone по согласованной логике, а не только менять громкость или индикатор. Проверить RR-группы, velocity boundaries и отсутствие конфликтов.

### SMP-03 — streaming и безопасность
- Background page streaming больших библиотек и bounded cache/worker queue.
- Никакого дискового I/O, блокирующих mutex и динамических аллокаций в аудиоколбэке.
- Полноценный Windows mmap/OVERLAPPED путь там, где он предусмотрен архитектурой, с проверенным безопасным fallback.
- Безопасная публикация и освобождение старых snapshots без use-after-free.
- Корректное поведение cache miss, voice stealing, смены/выгрузки банка во время воспроизведения.
- Проверка размера банков до согласованного лимита, RAM отдельно от размера банка.
- Стресс: минимум 100 последовательных атак, длительное воспроизведение, cache pressure, повторные reload/unload; измерить CPU, RAM, dropouts и clicks/pops.

### SMP-04 — Voices и multiprocessing
Реализовать Voices/polyphony control 1–64 с корректным voice stealing и NoteOff. Multiprocessing должен реально использовать безопасную параллельную обработку вне аудиоколбэка (сканирование, декодирование, подготовка страниц), с ограниченными очередями, отменой и отсутствием гонок. Не добавлять фиктивный переключатель; подтверждать выигрыш профилированием.

## 5. Игровые режимы, музыкальная логика и MIDI

### PERF-01 — MELODY
Одна MIDI-клавиша запускает соответствующую отдельную ноту, как в sample-based guitar instrument. Обеспечить key zones/root keys, velocity/dynamics, palm mute, round robin и доступные артикуляции. Legato, slide, hammer-on, pull-off, vibrato и pitch bend должны иметь реальный слышимый результат. Проверить NoteOn/NoteOff, overlap, MIDI channels, sustain, panic и отсутствие stuck notes.

### PERF-02 — PATTERN
Клавиша запускает аккорд/voicing и музыкальный паттерн. Реализовать down/up strums, alternate picking, chugs, palm-muted rhythms, dead/muted hits, arpeggios, picking и acoustic strumming. Обеспечить chord changes, host tempo/transport sync, start/stop/continue/seek/loop, swing, pattern slots/length и bounded strum offsets. Scheduler events должны доходить до sampler и создавать реальное звучание, а не только тестовые события.

### PERF-03 — AutoChords и Fingering
Реализовать scale/root-aware chord generation, major/minor и согласованные типы аккордов, inversions, guitar voicings/fingering с учётом диапазона грифа и строя. Проверить MIDI boundaries, сложные аппликатуры и fallback. Режим «одна нота задаёт root аккорда» не должен менять стандартный Melody one-key/one-note path.

### PERF-04 — Style DNA
Chug Density, Hammer/Pull Probability, String Spread, Beat Accent и FX Taste должны влиять на реальные события, выбор артикуляций, timing/strum spread и FX. Нельзя оставлять вычисленные значения только в структурах/GUI без подключения к runtime. Проверить детерминированность и отсутствие audio-thread allocations.

### PERF-05 — MIDI, transport, automation
Стабильные APVTS IDs и корректная host automation для всех публичных controls. Проверить pitch bend, modulation, MIDI channels, CC, panic, tempo/transport, stop/seek/loop и согласованный CC111 pattern offset, если он остаётся в master spec. События внутри одного audio block должны обрабатываться sample-accurately; overflow должен иметь fail-safe all-notes-off.

## 6. DSP и тембры

Реализовать и подключить к аудиопути с проверяемым звуковым результатом:
- Distortion;
- Compressor;
- EQ;
- Chorus;
- Flanger;
- Delay;
- Reverb;
- Cabinet IR/cabinet stage;
- Acoustic profile/mode.

Для каждого эффекта: controls, bypass, defaults/reset, state persistence, automation, stereo correctness и DSP regression/audio tests. Проверить порядок цепи, gain staging, clipping, latency reporting (если применимо), денормалы и clicks. Acoustic mode должен иметь реальное отличие в звуке/сэмплах, а не только название пресета. Совместная работа palm mute + reverb и других эффектов должна быть протестирована.

## 7. Presets, banks и factory content

- Реализовать форматы .ppgm, .ppgp, .ppgbank, .ppgstate с версионированием, validation, безопасным import/export и migration.
- Целевой каталог: **300 полноценных factory presets — 150 Melody + 150 Pattern**, привязанных к существующим звукам и параметрам. Не считать пустые записи или шаблоны готовыми пресетами.
- Сохранить согласованные стилевые направления и ориентиры (Blink-182, Sum 41, Simple Plan, New Found Glory, Good Charlotte, Mest, Angels & Airwaves, All-American Rejects, MGK, Avril Lavigne и другие ранее утверждённые ориентиры), не копируя защищённые записи/фирменные материалы.
- Browser: категории, tags, поиск, preview, recent/favorites, статус missing samples и управление библиотеками.
- Подготовить и проверить style profiles, если предусмотрены master spec; пресеты должны реально звучать на включённых лицензированных sample assets.

## 8. GUI/UX

Тёмный/чёрный интерфейс, быстрый и понятный по принципу простоты FLEX (без копирования фирменного оформления):
- переключение Melody/Pattern;
- preset/library browser, categories, tags, search;
- IMPORT SOUNDS, progress, cancel, error report;
- Mapping Editor и sample audition/preview;
- Voices, velocity/dynamics, palm mute, tuning, articulation;
- все согласованные FX controls;
- Pattern controls и Style DNA;
- missing-library/sample warnings, rescan/remove;
- resizable editor и читаемые состояния;
- сохранение и восстановление состояния проекта.

## 9. State, надёжность и FL Studio 20

Обеспечить сохранение/восстановление APVTS, режима, пресета, библиотеки/SFZ path и mapping. Отсутствующая библиотека не должна препятствовать восстановлению остальных параметров; показывать понятное сообщение и путь восстановления. Проверить project save/reopen, automation, editor close/reopen, повторные load/unload, device changes, panic, MIDI multi-note, Pattern transport, stop/seek/loop и длительное воспроизведение в FL Studio 20 x64 / Windows 10 x64.

## 10. Definition of Done — критерии приёмки

Требование получает DONE только при выполнении всех применимых условий:
1. Функция действительно реализована в продукте и подключена end-to-end.
2. Есть unit/integration/regression tests, включая негативные и граничные сценарии.
3. Windows x64 Release build и полный CTest прошли на commit, который содержит изменение.
4. pluginval strictness 5 прошёл, если применимо.
5. Проверены package/install/sample assets, если они затронуты.
6. Проведена ручная FL Studio 20 host-проверка для host-visible поведения.
7. В README и журналах указаны commit SHA, workflow run, тесты, ручная проверка и известные ограничения.

Допустимые промежуточные статусы: NOT STARTED / IN PROGRESS / CI PASS / HOST VALIDATION OPEN / BLOCKED. Успешная сборка сама по себе не закрывает milestone. Никакие M1–M5 не объявлять принятыми без соответствующих gates.

## 11. Обязательное ведение документации

Каждое новое согласованное предложение/изменение фиксировать как требование с ID, приоритетом, milestone, зависимостями, статусом и тестом приёмки. Каждую реализацию/исправление записывать с commit SHA и результатом CI. Обновлять:
- `README.md` — главная страница разработки и актуальный статус/приоритеты;
- `jurnal.md` — хронология решений, утверждений, изменений, реализации и CI;
- `Info.txt` — полный технический контекст и история;
- `docs/ЖУРНАЛ_БАГОВ_RU.md` — дефекты, root cause, fix, regression и доказательства; требования без дефекта вести отдельным реестром требований;
- `docs/WORK_LOG_M1_M5_INTEGRATION_RU.md` — детальный прогресс основной линии M1–M5;
- `docs/ПОЛНОЕ_ОПИСАНИЕ_ФУНКЦИЙ_И_ИЗМЕНЕНИЙ_ВЕТКИ_RU.md` — перечень всего согласованного объёма;
- данный файл — единое обязательное ТЗ.

Не записывать согласованное предложение как уже реализованную функцию. Не терять ранее утверждённые пункты при обновлении документов. Журналы обновлять в том же пакете изменений, что и код, если это практически возможно.


## Дополнение к ТЗ: производственная дисциплина и CI gate (10.11.2026)
1. M1–M5 — абсолютный приоритет основной линии.
2. M6–M10 во время основной разработки допускают только небольшие изолированные подготовительные задачи, не мешающие сборке, тестам и интеграции M1–M5.
3. M11–M13 запускаются полноценно только после подтверждённого завершения M1–M5.
4. Связанные изменения, regression tests и документация объединяются в компактные пакеты.
5. Каждая найденная ошибка получает отдельный ID, статус, root-cause analysis, исправление и regression acceptance.
6. После исправления фиксируются commit SHA, Windows CI run, тесты и фактический результат.
7. Запрещено отключать, пропускать, ослаблять или обходить проверки ради ускорения.

Текущий CI blocker на HEAD `2a0f15b007e222ae5cbcfb05377b957d1d91ad04`: IMP-003 — недоступный `juce::SHA256` приводит к Windows compile failure; IMP-001 остаётся открытым, поскольку CTest не запускался.