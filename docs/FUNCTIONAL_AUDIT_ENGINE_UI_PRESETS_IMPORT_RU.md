# PPGU 4.0 — функциональный аудит Engine / UI / Presets / Import Sounds

Дата аудита: 10 ноября 2026  
База аудита: `feat/power-profiles-double-multicore`, HEAD `2c04b766aaf90b60798c5d7cc0a37c3960f9b6dd`.  
Основание: исходники `PluginProcessor.*`, `PluginEditor.*`, `SamplePool/SFZ`, CMake, preset catalog и утверждённые UI/preset/library specifications.

> Это статический аудит исходного кода и архитектуры. Он не заменяет сборку, CTest, pluginval и ручной тест в FL Studio 20. Функция считается готовой только после теста.

## Итог

| Область | Фактический статус | Вывод |
|---|---|---|
| Engine / MIDI / sampler | Частично реализовано | Есть реальный SFZ/WAV sampler и MIDI/performance code; полный гитарный production engine ещё не подтверждён |
| UI / Cover | Частично реализовано | Есть тёмно-розовый JUCE editor, но он не повторяет утверждённую композицию Cover Interface |
| Factory presets | Каталог есть, применение частичное | 300 записей читаются из JSON; текущий loader применяет только drive/tone/reverb/output и mode |
| User presets | Не реализовано в UI | Нет Save, Save As, Rename, Delete, Import/Export bank и реального user preset browser |
| Preset switching | Частично реализовано | ComboBox выбирает каталог и меняет ограниченный набор параметров; prev/next, audition и полное применение параметров отсутствуют |
| Import Sounds | Частично реализовано | Есть выбор одного SFZ и асинхронная загрузка; Folder/ZIP/WAV import, mapping editor, library browser и конвертация в библиотечный формат отсутствуют |
| Save/restore DAW state | Есть базовый APVTS state | Сохранение состояния проекта не равнозначно сохранению отдельного пользовательского preset-файла |

## 1. Engine

### Подтверждено исходниками
- VST3 instrument с MIDI input и stereo output.
- APVTS содержит параметры voices, play mode, articulation, velocity response, palm mute threshold, tuning, pan, scale/key, pattern transport, amp/tone и FX controls.
- SFZ parser / SamplePool / streaming page cache / streaming voice reader присутствуют в проекте.
- SFZ загрузка может выполняться через worker pool, вне audio callback.
- PatternScheduler и host transport clock подключены к processor.
- В PR #13 добавлены профили GOOD/NORMAL/NIGHT с voice caps 64/32/12 и preallocated DOUBLE short-delay thickener. DOUBLE — это widening/thickening, не независимый второй дубль.

### Что нельзя считать полностью готовым
- Наличие APVTS-параметра FX не доказывает, что каждый FX реализован и звучит как законченный DSP effect.
- Параметр Articulation/Palm Mute не доказывает полную выборку отдельных palm-mute sample zones во всех банках.
- Полноценные acoustic/electric/hybrid переключаемые банки не подтверждены текущим UI.
- Реальный пользовательский multicore control отсутствует. Нельзя запускать/ожидать потоки внутри audio callback.
- GOOD/NORMAL/NIGHT пока подтверждают voice caps, но не измеренную экономию CPU/энергии.
- Требуются audio regression tests: note-on/off, overlapping notes, sustain, pitch bend, velocity layers, palm mute, bank reload, underrun, clipping/phase и длинное воспроизведение.

## 2. Интерфейс и Cover Interface

### Уже есть
- Тёмный фон, magenta акцент, белые подписи.
- Header с preset/category/search, power profile, DOUBLE и Load SFZ.
- Melody/Pattern selector, amp/tone knobs, FX toggles, playback controls и MIDI keyboard.
- Hero зона нарисована векторной графикой.

### Несоответствия утверждённому Cover
- Текущая компоновка и размер editor 1040×790, а не эталонная широкая композиция 1402×1122.
- Вместо исходной hero artwork используется упрощённый векторный силуэт.
- Нет верхних Previous/Next, Save, Browser, CPU/RAM telemetry и Settings.
- Нет AutoChords, Key/Scale UI, Guitar Type Electric/Acoustic, полноценного Browser/Presets/Settings/About нижнего меню.
- Нет pitch/mod wheels.
- UI не должен показывать CPU/RAM числа, пока не подключены реальные измерения.
- Текущая кнопка LOAD SFZ не является полноценным Import Sounds manager.

**Решение:** Cover Interface.jpg закрепляется как основной визуальный эталон. Перестроить главный editor под его иерархию и пропорции; не заменять его generic JUCE layout. Все элементы — реальные контролы, фиктивные метрики/кнопки запрещены.

## 3. Factory presets, switching и сохранение

### Фактическое состояние
- CMake встраивает `presets/factory_bank_300.json` в BinaryData.
- Editor загружает массивы melodyPresets и patternPresets в каталог.
- Текущий `applyFactoryPreset()` применяет только четыре значения: distortionDrive, tone, reverbMix, output; затем переключает playMode.
- Для Pattern UI прямо показывает сообщение `PERFORMANCE ENGINE REQUIRED`; каталог сам по себе не доказывает, что все pattern presets задают реальные паттерны.
- В UI нет отдельного сохранения пользовательского пресета. APVTS host state (если сохранён в DAW) — это не preset browser и не файл .ppgm/.ppgp.

### Требуемая реализация
- Полный параметрический preset schema с validation и versioning.
- Загрузка factory/user presets атомарно: при ошибке прежнее состояние остаётся.
- Previous/Next, категории, поиск, Favorites, audition/preview, double-click load.
- Save / Save As, Rename, Delete только для User; Factory read-only.
- Файлы .ppgm/.ppgp и .ppgbank по утверждённой спецификации; хранить sampleProfile/libraryId, не абсолютные пути к сэмплам.
- Проверить 300 factory definitions: каждая запись должна быть валидной, менять ожидаемые параметры и соответствовать Melody/Pattern engine.

## 4. Import Sounds — целевой pipeline

Пользователь выбирает Import Sounds; система должна провести активы через безопасную подготовку, а не просто скопировать их в SamplePool.

1. **Выбор источника:** Import Folder, Import ZIP, Add SFZ Instrument; поддержка WAV, OGG, MP3, FLAC, AIFF/AIF как пользовательских источников. NCW не поддерживается.
2. **Безопасный scan:** ограничение размера/количества файлов, проверка путей, ZIP traversal, symlinks, повреждённых файлов и дубликатов.
3. **Metadata:** codec, sample rate, bit depth, channels, duration, size, SHA-256, source/license, errors.
4. **Decode/prepare:** background workers читают/декодируют source assets; нормализованный внутренний PCM/cache строится вне audio thread и помечается версией формата + hash исходника. Lossy imports помечаются как lossy.
5. **Mapping:** SFZ maps читаются как есть. Для raw WAV Auto Mapping — только предложение. Неоднозначные файлы требуют подтверждения в Mapping Editor: root/low/high key, velocity range, articulation, round-robin, gain/pan/tune/release.
6. **Validation:** проверка gaps/overlaps в key/velocity zones, отсутствующих файлов, лицензий, checksum и лимитов.
7. **Atomic publish:** сначала staging, затем только после полной проверки добавление библиотеки в Library Browser. При ошибке текущая активная библиотека не меняется.
8. **Engine integration:** создать manifest/libraryId и внутренний индекс; engine загружает готовую карту/кэш, а не выполняет файловое сканирование или decode в processBlock.
9. **Preset binding:** preset ссылается на libraryId/profile; если банк отсутствует — warning, настройки пресета сохраняются.
10. **Export:** разрешать только при подтверждённом redistribution permission для каждого asset.

### Что есть сейчас
- Один SFZ file chooser и асинхронный вызов `loadSfzFileAsync`.
- В текущем UI отсутствуют Import Folder/ZIP, список библиотек, preview, metadata, auto-mapping, Mapping Editor и отдельный staging/publish UI.
- Следовательно, Import Sounds нельзя рекламировать как готовую функцию.

## 5. Проверочный план и критерии закрытия

### Engine
- [ ] CTest + отдельные regression tests на MIDI, sampler, pattern dispatch, note-off/release, voices и concurrent reload.
- [ ] Проверка velocity layers / palm mute / articulations на реальных лицензированных sample maps.
- [ ] Проверка всех включённых FX на аудиовыходе, отсутствие NaN/clipping/denormals.
- [ ] CPU/RAM/underrun benchmark при 1/8/16/32/64 voices и GOOD/NORMAL/NIGHT.
- [ ] DOUBLE: phase/mono compatibility, transient, clipping и latency tests.
- [ ] Безопасное управление фоновыми worker threads; никакого blocking/thread spawn в audio callback.

### UI / Cover
- [ ] Перестроить editor по Cover Interface.jpg.
- [ ] Проверить 100%, 125%, 150% scaling и resize; ничего не обрезается.
- [ ] Проверить каждую кнопку/ручку/селектор: реальная APVTS или реальное действие.
- [ ] CPU/RAM только измеренные.
- [ ] Основные действия доступны без отдельного Settings окна.

### Presets
- [ ] Все 300 factory entries parse/validate.
- [ ] Полный preset application, включая mode, performance/style data и все supported parameters.
- [ ] Previous/Next, поиск, фильтры, preview, favorites.
- [ ] Save As -> close/reopen -> reload с побитово/численно эквивалентным parameter state.
- [ ] Rename/Delete user preset, Factory read-only.
- [ ] Import/export bank и missing-library warning.
- [ ] DAW project save/reopen.

### Import Sounds
- [ ] WAV/SFZ folder import, затем ZIP import с ZIP-slip tests.
- [ ] metadata + SHA-256 + duplicate detection.
- [ ] mapping editor и подтверждение auto mapping.
- [ ] corrupt/unsupported/missing sample не заменяет активную библиотеку.
- [ ] импорт и preview не делают disk I/O/allocations/locks в audio callback.
- [ ] Повторный импорт не дублирует SamplePool.
- [ ] Проверка лицензии и происхождения каждого распространяемого factory asset.

### Release validation
- [ ] Windows x64 Release build.
- [ ] CTest.
- [ ] VST3 bundle verification.
- [ ] pluginval strictness 5.
- [ ] Проверка установки/загрузки/звучания в FL Studio 20 x64 на Windows 10.
- [ ] Запись результатов отдельно; CI не заменяет DAW test.

## Приоритет следующей реализации

1. Preset service: полный loader + Save As/Load User preset + prev/next, с тестами round-trip.
2. Cover UI: переразметить основной editor и добавить реальные действия Browser/Presets/Settings.
3. Import Sounds phase 1: folder scan + SFZ/WAV metadata + staging + validation + atomic library publish.
4. Mapping Editor / raw WAV auto-mapping.
5. ZIP import/export, checksum/manifest, license reports.
6. Дальнейшие engine benchmarks и аудио-регрессии.

Статус документа: **аудит выполнен по исходникам; перечисленные незакрытые функции не считаются реализованными.**
