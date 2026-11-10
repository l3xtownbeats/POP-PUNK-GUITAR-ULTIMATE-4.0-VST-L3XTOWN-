# Подготовка заводских пресетов 4.0 — статус и план интеграции

Дата аудита: 2026-11-08  
Ветка: release/4.0-production-hardening

## Что уже подготовлено в каталоге

Файл `presets/factory_bank_300.json` содержит ровно 300 описаний:
- 150 Melody — одна MIDI-клавиша запускает одну хроматическую ноту гитары;
- 150 Pattern — описаны ритмические рисунки и предполагаемое поведение исполнения.

В каталоге представлены 15 референсных направлений: Blink-182, Sum 41, Simple Plan, New Found Glory, Good Charlotte, Mest, Angels & Airwaves, The All-American Rejects, MGK, Avril Lavigne, The Offspring, Green Day, Bowling for Soup, Sugarcult и All Time Low.

## Важное ограничение

Эти 300 записей пока являются каталогом звуковых/исполнительских целей, а не доказательством 300 загружаемых пресетов в интерфейсе VST3:
- все 150 Melody помечены `catalog-definition-awaiting-plugin-integration-and-sample-validation`;
- все 150 Pattern помечены `catalog-definition-awaiting-pattern-engine-and-recorded-performance-library`;
- у Pattern пока не задано поле `sampleProfile`;
- для Pattern требуется отдельный движок исполнения и записанные chord/strum-перформансы; одиночные ноты Steel/Nylon/электрических SFZ не заменяют такие записи.

## Подготовка для интеграции

### Melody
Подключить каталог к реальному загрузчику пресетов и параметрам JUCE; проверить root key, диапазоны, velocity layers, round-robin, note-on/off, release и артикуляции open/palm mute. Для каждого профиля должна быть проверена карта WAV/SFZ. Не загружать файлы и не выделять память в audio thread.

### Pattern
Реализовать отдельный scheduler, синхронизированный с BPM/transport хоста. MIDI-аккорд должен выбирать гитарный voicing; рисунок должен реально проигрывать down/up strokes, palm mutes, dead strokes, accents, gate и humanization. AutoChords ON/OFF должен переключать автоматическую гармонизацию и фиксированную карту пресета. Акустические рисунки должны использовать акустические voicing/performance assets, электрические — power-chord/performance assets.

### Browser и пользовательские банки
Нужны реальные фильтры Melody/Pattern, категории/артисты, поиск, избранное, загрузка, Save/Save As, Rename/Delete, Import/Export Bank и сохранение пользовательских пресетов между обновлениями.

## Критерии готовности

Не объявлять 300 пресетов готовыми, пока все 300 не перечисляются в реальном браузере и каждый не загружается без ошибок. Проверить MIDI-клавиатуру и Piano Roll FL Studio 20, BPM/transport, смену аккордов, сохранение/восстановление состояния, отсутствующие библиотеки и производительность. Успех pluginval сам по себе не подтверждает работу в FL Studio.

## Следующий инженерный шаг

Интегрировать JSON-каталог в runtime-модель пресетов, затем отдельно довести Melody sample mapping и Pattern performance engine. Существующий CI может собрать VST3, но не должен трактоваться как подтверждение этих runtime-функций.


## Обновление каталога и sound target — 10 ноября 2026

Каталог обновлён в ветке `feat/m3-transport-scheduler-v2`: 150 Melody + 150 Pattern, 300 всего. Добавлены недостающие style DNA: Fall Out Boy, Yellowcard, My Chemical Romance, Less Than Jake, Fenix TX и Midtown. Теперь каталог охватывает 21 ориентир: Blink-182, Sum 41, Green Day, Fall Out Boy, Good Charlotte, Simple Plan, New Found Glory, Avril Lavigne, Yellowcard, The All-American Rejects, My Chemical Romance, Less Than Jake, Fenix TX, Midtown, Bowling for Soup, MGK, Mest, Angels & Airwaves, The Offspring, Sugarcult, All Time Low.

Для Pattern в каждой записи разведены независимые оси: `soundCharacter` (Clean, Crunch, Drive, Palm Mute, Open Chords, Acoustic, Lead Drive, Muted Chugs) и `playingStyle/groove` (downstrokes, alternate picking, muted verse → open chorus, ska offbeats, syncopated stops, octave hooks, arpeggios, acoustic strums и др.). Это пока catalogue metadata, а не обещание, что все эти паттерны уже исполняются runtime.

Добавлены MGK Bloody Valentine Inspired Lead и Bloody Valentine Inspired Chorus как стилистические sound targets: яркий современный pop-punk drive, приглушённый verse pulse и открывающийся припев. Никакие оригинальные stems, master recordings, ноты/транскрипции или коммерческие сэмплы не включены.

Обновлены README и MASTER_PRODUCT_SPEC: коммерческая цель звучания, критерии сравнения с NI Electric Sunburst Deluxe, требования к профессиональным DI/mic записям, velocity layers, round robin, articulations, performance recordings, amp/cab/IR, phase/mono checks и loudness-matched listening. Это roadmap/acceptance requirements, не текущие реализованные функции.

Обновлён `assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg`: новый векторный product cover в фирменной charcoal/magenta стилистике с макетом интерфейса и блоками Melody/Pattern/Tone/Performance. Растровая AI-обложка не создана в этом сеансе из-за лимита генерации изображений; SVG — готовый самостоятельный векторный баннер.
