# Полный аудит архива и исходников PPGU — 11.11.2026

## Проверенная база

- Репозиторий: `l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-`
- Ветка: `integration/ppgu-5-production-audit`
- Проверенный исходный commit до очистки поставки: `454dd1e1b0b0d39d9f15d7bde81db28af3929450`
- Windows workflow run: `42919036253`
- Итог workflow: SUCCESS.
- Сборка: Windows x64 Release, VST3.
- CTest: 12/12 passed.
- pluginval: strictness level 5 passed.
- FL Studio 20: ручная проверка не выполнялась; DAW acceptance остаётся открытой.
- Полный CI artifact: 2,964,743,575 bytes; GitHub artifact ZIP SHA-256: `480389265b11f1d469121d93ca2838aa7cd4076f13614c5f507360187c1dcef1`.

## Проверки, выполненные повторно на source-review snapshot

Успешно выполнены:
- `python tools/validate_factory_bank.py presets/factory_bank_300.json` — 150 MELODY + 150 PATTERN = 300 каталоговых записей.
- `python tools/validate_preset_files.py presets/examples` — 2 переносимых примера.
- `python tools/validate_apvts_layout.py` — APVTS layout/state round-trip проверка.
- `python tools/audit_m1_parameters.py` — обязательные M1 IDs присутствуют; 7 FX слотов генерируются.
- CTest из Windows CI — 12/12.
- pluginval strictness 5 — успешно.

В CI присутствуют предупреждения компилятора C4244 (сужение double→float в тестах) и C4324 (выравнивание SPSC структуры). Они не остановили сборку, но остаются задачами по чистоте предупреждений.

## Устранено в упаковке

Из source-review и release-поставки исключены устаревшие/дублирующие материалы:
- `build.zip` — архив старого проекта с другой архитектурой Source/Engine; не участвует в текущем CMake/VST3 build.
- `Cover Interface.jpg` — не используется текущим UI; вместо него применяются два утверждённых изображения в `Resources/Artwork`.
- `presets/factory_preset_bank_v1.json` — старый schema v3 каталог; runtime загружает `factory_bank_300.json`, поэтому старая копия не должна вводить в заблуждение.
- Release-папка теперь собирается по allow-list: VST3, Samples, активный `factory_bank_300.json`, два portable preset examples, `Resources/Artwork`, installation notes.
- Два утверждённых artwork-файла добавляются отдельно в архив; они также встроены в BinaryData плагина.

Историю и журналы разработки не удаляли из Git. Исключение устаревших файлов относится к пользовательской поставке и review archive, а не к переписыванию истории проекта.

## Обнаруженные функциональные пробелы — не скрывать как готовые

### Блокирующие/высокие

1. **MIDI Piano Roll отсутствует в runtime UI.** Кнопка MIDI отключена и прямо сообщает, что редактор не подключён.
2. **MULTICORE отсутствует.** Кнопка отключена; фоновой параллельной обработки DSP нет. Не заявлять поддержку multicore.
3. **VOICES control отсутствует в GUI.** APVTS параметр `voices` и voice cap существуют в DSP, но видимого пользовательского регулятора в Editor нет.
4. **External SKIN folder не реализована.** В UI есть десять встроенных цветовых палитр, хранимых в коде. Это не десять сменных высококачественных skin assets и не runtime-loaded папка SKIN.
5. **300 factory entries — каталог, не 300 полноценных звуковых пресетов.** JSON прямо помечает ряд определений как ожидающих интеграции и проверки сэмплов. Runtime применяет лишь ограниченный набор параметров (drive, tone, reverb, output, patternSlot, articulation и несколько enable flags); не все поля performance/sample requirements реально управляют движком.
6. **PATTERN не использует все данные каждого JSON pattern preset.** Текущий scheduler строит ритм по внутренним маскам/слотам. Данные `palmMuteMap`, `accentMap`, часть humanize/strum description из каталога не полностью связаны с runtime.
7. **SFZ compatibility неполная.** Parser поддерживает базовые секции/opcodes и default_path, но не полноценную рекурсивную обработку `#include`, переменных/макросов и всего набора расширенных opcode сторонних карт.
8. **FLAC runtime path неполон.** Импортёр может использовать JUCE-форматы для конвертации, но streaming SamplePool построен вокруг WAV metadata/page reader. FreePats Jazz FLAC нельзя считать напрямую проигрываемым SFZ-банком без импорта/конвертации и проверки.
9. **Palm mute пока DSP-эмуляция.** При низкой velocity/force articulation применяется фильтр и короткая огибающая. Это не гарантирует выбора отдельного записанного palm-muted sample/articulation layer.
10. **Pattern event overflow и chord timeline.** Известные M3-014/M3-015 остаются открытыми: нужно доказать корректную обработку нескольких chord changes в одном block и безопасную реакцию на превышение event capacity без потери NoteOff/stuck notes.
11. **Банк-менеджмент и unload/reclaim.** Нужны стресс-тесты последовательных загрузок/отмен, освобождения памяти и длительных сессий с несколькими банками.

### Средние/технические

- C4244/C4324 предупреждения компилятора следует устранить либо локально документировать.
- Нужны runtime-тесты для каждого отдельного Sound Bank: parse map → validate all samples → load → note-on/off → rendered non-silent audio → memory/time.
- Нужна проверка WAV/FLAC file count, лицензий, SFZ maps и SHA256 внутри реально скачанного release ZIP, а не только успешного шага упаковки.
- Нужно добавить тесты для сохранения/восстановления выбранного SFZ и восстановления банка после перемещения/отсутствия файла.
- Нужны host tests в FL Studio 20: scan, load, MIDI, automation, save/reopen project, audio output, CPU/memory, stop/seek, multiple instances.

## Архитектурная сверка

- CMake собирает VST3 и 12 regression targets; JUCE 7.0.12 фиксирован через FetchContent.
- Sample streaming worker и bounded page cache присутствуют; аудиопоток не должен выполнять файловый I/O.
- Velocity crossfade selection и Round Robin механизмы присутствуют, но должны продолжать проверяться на реальных SFZ картах, а не только синтетических fixtures.
- FX Rack в текущем UI содержит Compressor, EQ, Amp/Distortion, Chorus, Flanger, Delay, Reverb, Cabinet и Double Track. Поэтому ранее встречавшееся описание «7 эффектов» устарело.
- Processor quality profiles ограничивают полифонию (GOOD/NORMAL/NIGHT/ECO), но отдельный UI-контроль VOICES всё ещё отсутствует.
- Сохранение APVTS state включает путь SFZ; при восстановлении существующий SFZ загружается асинхронно. Если путь недоступен, он сохраняется, но библиотека не может быть автоматически восстановлена.

## Текущий статус

Уже подтверждено: Windows Release build, 12/12 CTest, pluginval strictness 5, создание полного CI ZIP.

Не подтверждено: FL Studio 20 acceptance, полноценная работоспособность каждого из 300 preset entries, совместимость всех SFZ банков, direct FLAC streaming, external skin files, MIDI Piano Roll, multicore, отдельный VOICES UI control.

Следующий Release должен быть обозначен как **Windows x64 VST3 + expanded source-cleared sample banks, with documented runtime compatibility limits**, а не как полностью завершённый продукт до закрытия перечисленных блокеров.
