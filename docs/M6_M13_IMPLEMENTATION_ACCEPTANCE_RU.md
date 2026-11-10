# M6–M13 — план реализации и критерии приёмки POP PUNK GUITAR ULTIMATE 4.0

Дата аудита: 10.11.2026. Источник требований: `Promt.txt`, части 0, 3–14, 18–24; `docs/MASTER_PRODUCT_SPEC_4_0_RU.md`. Этот документ превращает шаги M6–M13 из Части 18 в проверяемые задачи. Он не повышает статус функций без кода и тестов.

## Общие правила
- Не ослаблять требования M1–M5 и не переносить незакрытые release gates в «готово».
- Каждый модуль: реализация → unit/regression tests → Windows x64 Release/CTest → Pluginval strictness 5 → интеграционные проверки.
- Аудиопоток: никаких блокировок, heap allocations, файлового I/O, декодирования и ожидания worker.
- Все данные, сэмплы и пресеты распространяются только при подтверждённой лицензии на redistribution; для каждого asset — источник, лицензия, attribution и SHA-256.
- Не выдавать каталог/макет/metadata за аудио-функцию. Любая функция UI должна управлять реальным runtime.
- Не переходить к финальному M13, пока предыдущие gates не закрыты.

## M6 — MelodyEngine + PatternEngine (ИНТЕЛЛЕКТ)
**Объём**
1. MelodyEngine: chromatic MIDI note mapping, velocity layers/crossfade, RR без повтора подряд, open/palm-muted и доступные артикуляции, legato, hammer-on/pull-off и slides только при наличии соответствующих записей/метаданных, release и voice ownership.
2. PatternEngine: фразы длиной 1/2/4 такта, chord recognition, AutoChords, FingeringGenerator, StyleEngine, chord transitions, down/up, strum/picking, accents, mutes, fills и stop.
3. MIDI Groove Player остаётся частью PATTERN; host BPM/transport, quantized switching, sample-accurate offsets, PreviewBus отдельно от основного транспорта.
4. Pattern event articulation и strum timing должны доходить до SamplerEngine/voice commands, а не останавливаться на метаданных.
**Приёмка:** тесты melody note mapping/articulation/legato/release/voice stealing; Pattern chord-to-fingering-to-scheduler-to-sampler E2E; 1/2/4-bar loops, tempo/seek/stop, no stuck notes; deterministic playback; host MIDI regression. До этого: NOT COMPLETE.

## M7 — Звуковая библиотека
**Объём**
1. Электрогитары минимум двух тембров и акустика; E standard, Eb, Drop D, Drop Db.
2. Для заявленных артикуляций: open chords, muted chugs, palm-muted 8th/16th, power chord sustains, single-note runs, slides, harmonics, tremolo, skank upstroke, HO/PO, fret noise и release — только реально записанные и размеченные assets.
3. Цели качества Части 6: диапазон E2–C6+, до 5 velocity layers и минимум 3 RR вариации для критичных зон, если это подтверждено исходными записями. Не создавать искусственные «слои» копированием одного WAV.
4. Библиотека должна быть потоковой, с lazy loading, bounded cache, preview и проверкой отсутствующих файлов.
5. Steel Guitar и Nylon Guitar — независимые банки только при наличии лицензии на распространение.
**Приёмка:** инвентаризация каждого WAV, диапазона, root, velocity, RR, articulation, tuning; SHA-256 + license manifest; прослушивание и MIDI playback; stress/cache tests; размер заводского пакета ≤20 GB. Статус текущего importer MVP: PARTIAL, не полноценная библиотека.

## M8 — Валидатор и каталог 300 пресетов
**Объём**
1. Не менее 300 валидных пресетов: 150 MELODY + 150 PATTERN, с корректными файлами .ppgm/.ppgp и привязками к BANK, Style DNA, performance metadata и FX/tone snapshot.
2. Проверка schema/version, уникальных ID, режима, существующих библиотек/сэмплов, карт/артикуляций, диапазонов параметров и целостности ссылок.
3. Не загружать битые пресеты; сообщать причину и сохранять предыдущее состояние.
4. Не включать фиктивные дубликаты для достижения количества. Каждый пресет должен иметь реальный доступный источник звука и проверяемую конфигурацию.
**Приёмка:** validator показывает ≥300, ровно 150/150 по режимам; batch-load всех 300 через runtime loader; нет missing references/schema errors; smoke audition для каждого; проверка лицензий и соответствия стилю. Каталожный JSON без реальных связей не засчитывается.

## M9 — Style DNA и референсный PHOTOGRAPHS preset
**Объём**
1. Style DNA для всех категорий из Части 7 и 15.6: tone, tuning, articulation weights, performance families, velocity/accent behaviour, strum/picking feel, FX taste.
2. Разделить Tone selector и Performance selector; выбор стиля должен управлять реальным event/sample pipeline.
3. Создать оригинальный жанровый пресет PHOTOGRAPHS Lead/Rhythm как два самостоятельных sound/performance варианта; не использовать запись/сэмплы/транскрипцию защищённого произведения.
4. Привязать каждый пресет к конкретному Style DNA и проверять совместимость с доступной библиотекой.
**Приёмка:** deterministic style tests; preset-to-DNA binding tests; runtime E2E от preset selection до артикуляции и тайминга; loudness/headroom checks. Одна только подпись стиля не засчитывается.

## M10 — GUI и FX EDIT
**Объём**
1. Реализовать утверждённый тёмный skin, MELODY/PATTERN tabs, browser, категории, поиск, избранное, previous/next, preview, основные tone/performance controls.
2. FX EDIT: Amp/Distortion, Chorus, Flanger, Delay, Reverb; параметры подключены к DSP, smoothed, bounded, сохраняются и автоматизируются через APVTS.
3. UI показывает только реально работающие controls; недоступные функции disabled/явно обозначены.
4. UI не блокирует загрузку и аудио; preview работает через отдельный PreviewBus.
**Приёмка:** GUI interaction tests где применимо; APVTS persistence/automation tests; no audio callback locks/allocations; визуальная проверка в FL Studio 20, масштабирование и редактор open/close.

## M11 — BANK Manager, MIDI Learn и Drag & Drop MIDI
**Объём**
1. BANK Manager: Import Folder/ZIP, Rescan, Remove, Export only if license allows, active bank, status Ready/Missing/Needs Mapping/Invalid/License Review.
2. Preview waveform/audition, import cancellation, checksums, duplicate detection, atomic staging/publish, missing-bank recovery.
3. Mapping Editor: root/low/high key, velocity ranges, articulation, RR, loop/release, overlap/gap warnings; safe immutable snapshot for audio thread.
4. MIDI Learn для назначаемых параметров с сохранением в проекте и защитой от конфликтов.
5. Drag & Drop MIDI: импорт MIDI groove/performance в PATTERN, проверка формата/tempo/chord mapping и явное подтверждение перед изменением пользовательских данных.
**Приёмка:** folder/ZIP security tests (path traversal, symlink, zip slip, size limits), checksum/license tests, cancel/rollback/reimport, preview isolation, MIDI Learn persistence, MIDI drag-drop tests, runtime mapping tests. Не экспортировать чужие WAV без redistribution rights.

## M12 — Полный QA и оптимизация слабого ПК
**Объём**
1. Полный тест-план Частей 20–24: 60-second render, повторные атаки, buffer sizes 64/256/1024 и hot buffer changes, seek/loop/tempo/transport, preset/bank switching, missing/corrupt assets.
2. Windows 10 1809+ x64, MSVC v143, SSE2 baseline; FTZ/DAZ; CPUID before optional SIMD; no mandatory AVX.
3. Измерить cold start (цель ≤2 s HDD), preset switch (цель ≤10 ms без clicks), CPU (цель ≤10% на обозначенной 2-core test rig при 64/48k), RAM и dropout count; не заявлять бюджет без опубликованной конфигурации/логов.
4. Проверить 2 cores / 4 GB RAM / HDD, long playback, device/buffer changes, UI responsiveness, preview vs main transport.
5. Исправить каждый дефект с regression test; не отключать тесты и не снижать thresholds.
**Приёмка:** ноль необъяснённых провалов тестов и ноль dropouts/clicks в заявленном тестовом сценарии; прикреплённые логи/метрики. Если нет реального слабого ПК/VM, статус остаётся UNVERIFIED.

## M13 — Release / packaging
**Объём**
1. Чистая Windows 10 1809+ x64 сборка VST3, Pluginval strictness 5, полный CTest и интеграционные тесты.
2. Installer/package: .vst3, легально распространяемые factory BANK assets, presets, license/attribution/SHA-256 manifests, версия, changelog и RU installation guide.
3. Проверка размера полного официального комплекта ≤20 GB, свободного места, целостности, повторной установки и восстановления после прерывания.
4. Скрипт clean-room validation на Win10 1809: установка, plugin scan, загрузка, MIDI, save/reopen, отсутствующие библиотеки, uninstall/reinstall.
5. FL Studio 20 x64 ручная проверка: automation, Melody/Pattern, host transport, chord changes, preset/bank switching during playback, audio-device change, long playback.
6. Опубликовать artifact и отчёт с commit SHA, workflow run, test results, compatibility matrix и известными ограничениями.
**Приёмка:** все gates M1–M12 закрыты; чистая установка и DAW acceptance подтверждены. До этого продукт не маркируется FINAL/PRODUCTION READY.

## Начальный статус аудита (10.11.2026)
| Веха | Наличие в текущей ветке | Статус |
|---|---|---|
| M6 | AutoChords/StyleEngine/Scheduler foundations есть; MelodyEngine и полный PatternEngine/articulation routing не подтверждены | PARTIAL |
| M7 | Folder importer MVP, базовый SFZ/WAV mapping; полноценный licensed multi-articulation library не подтверждён | PARTIAL |
| M8 | Каталог 300 JSON существует; полный runtime load/validation/real audio coverage не доказаны | UNVERIFIED / PARTIAL |
| M9 | StyleEngine и часть style labels есть; полное Style DNA runtime binding и PHOTOGRAPHS acceptance не доказаны | PARTIAL |
| M10 | Основной GUI существует; полный утверждённый UI и FX EDIT/DSP integration не доказаны | PARTIAL |
| M11 | Import Sounds MVP и MappingEditorModel есть; BANK Manager, secure ZIP, MIDI Learn и MIDI drag/drop не подтверждены | PARTIAL |
| M12 | CI workflows и unit tests есть; weak-PC metrics, VM, full stress and FL Studio regression not complete | OPEN |
| M13 | VST3 packaging CI существует; clean Win10 1809 install, full legal asset bundle and complete release acceptance not complete | OPEN |

## Текущая блокировка основной линии
На SHA `b768d5fc0fe1a6537c9ea3230c578a744fad87f3` run `42854695505` завершился FAILURE на шаге **Build Release VST3**; CTest был skipped. Run `42854695900` ещё выполняется. До выяснения build failure нельзя считать текущую линию стабильной или начинать расширять её несвязанными изменениями. Следующее действие — получить полный compiler log, исправить корень, добавить regression test, повторить Windows build/CTest/pluginval.
