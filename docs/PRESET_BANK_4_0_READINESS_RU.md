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


## 2026-11-14 — USER APPROVAL: CANONICAL PUNK GROUP PRESET CATALOG / SOUND DNA
Пользователь явно утвердил обязательное внесение в factory preset system всех ранее обсуждавшихся и отражённых в development journals групп, их жанров, звуковых описаний, ритмики, стилей исполнения и музыкальных ориентиров. Каноническая подробная карта: `docs/PPGU_CANONICAL_PUNK_GROUP_PRESET_SOUND_MAP_RU.md`.

### Обязательный охват — 21 PUNK GROUP / ARTIST PROFILE
Blink-182; Sum 41; Simple Plan; New Found Glory; Good Charlotte; Mest; Angels & Airwaves; The All-American Rejects; MGK; Avril Lavigne; The Offspring; Green Day; Bowling for Soup; Sugarcult; All Time Low; Fall Out Boy; Yellowcard; My Chemical Romance; Less Than Jake; Fenix TX; Midtown.

### Обязательные sound DNA / стилистические профили
- Blink-182: bright modern pop-punk crunch, palm-muted verse → wide open chorus, driving eighths, power-chord accents, octave hooks.
- Sum 41: aggressive skate-punk attack, dense rhythm drive, articulated leads, fast downstrokes, palm-muted gallop, alternate picking, stop-start power-chord hits.
- Simple Plan: polished melodic pop-punk, bright attack, controlled low end, clean arpeggios, open chords and melodic hooks.
- New Found Glory: fast melodic punk, sharp downstrokes, tight palm-muted verses and open power-chord chorus.
- Good Charlotte: mid-forward punchy pop-punk, compact rhythm, octave-lead hooks and controlled open-chord accents.
- Mest: melodic pop-punk lead and energetic rhythm; preserve `Photographs` (2005), 0:13–0:37 as a sound-character reference only.
- Angels & Airwaves: spacious layered guitars, modulation, ambient sustain, arpeggios and wide atmospheric textures.
- The All-American Rejects: radio-ready pop-rock/pop-punk, defined crunch, tight eighths, clean arpeggio → crunch chorus; `Dirty Little Secret` and `Move Along` are style references.
- MGK: modern saturated pop-punk, aggressive bright rhythm, muted verse pulse → explosive open chorus; `Bloody Valentine`, `Papercuts`, `Tickets to My Downfall` are style references; preserve Bloody Valentine Inspired Lead/Chorus sound targets.
- Avril Lavigne: bright melodic pop-punk, crunchy rhythm, fast picking and open-chord strums; `Bite Me` is the late-era style reference.
- The Offspring: fast aggressive punk, tight distorted rhythm, continuous downstrokes and stop-start accents.
- Green Day: classic punk midrange crunch, focused power chords, straight eighth-note downstrokes.
- Bowling for Soup: bright upbeat pop-punk, lighter crunchy rhythm, upbeat down/up eighths and open strums.
- Sugarcult: glossy alternative pop-punk, wide rhythm layers, octave pulse, arpeggios and dynamic builds.
- All Time Low: modern melodic pop-punk, polished clean-to-crunch, down/up eighths and melodic octave accents.
- Fall Out Boy: syncopated emo-pop-punk, octave hooks, stop-start accents and polished high-mid crunch.
- Yellowcard: melodic pop-punk with acoustic/electric layering, bright open chords, picked arpeggios and ringing upper strings.
- My Chemical Romance: dramatic emo-punk, forceful downstrokes, octave lines and quiet-to-explosive dynamics.
- Less Than Jake: upbeat ska-punk, crisp offbeat upstrokes, short chord chops and bright clean-to-crunch.
- Fenix TX: fast melodic skate-punk, tight eighth-note downstrokes, punchy palm-muted verses and open choruses.
- Midtown: urgent melodic emo-pop-punk, ringing octave leads, tight rhythm attacks and energetic dynamic changes.

### Жанровые и ритмические оси
Сохранить жанровые фильтры Classic Punk / Punk Rock, British Punk, Pop Punk, Hardcore / Skate Punk, Ska Punk, Post-Punk; продуктовые категории Pop Punk Essentials, Modern Pop Punk, Retro Punk, Emo Vibes, Energetic Rock, Clean & Bright, Crunchy Power, Dreamy Atmosphere.
Ритмический словарь включает straight/continuous downstrokes, down/up eighths, alternate picking, palm-muted rhythm/gallop, muted verse → open chorus, open chord/power-chord strums, ska offbeats, syncopated stops, double hits, ghost notes, staccato, accents/rests, sixteenth-note wall, octave hooks, arpeggios, tremolo picking, d-beat, half-time, acoustic strums/fingerpicking. Последние техники показываются как playable только при наличии совместимых реальных source/performance mappings.

### Правила распределения по пресетам
- Сохранить 300 как минимум: 150 MELODY + 150 PATTERN; каталог открыт для append-only расширения.
- Все 21 профиля должны иметь согласованные связи с реальными MELODY и PATTERN preset definitions, а не только текстовые browser tags.
- В каждом preset record согласовать stable ID, mode, group/profile ID, genre tags, soundCharacter, sampleProfile, tone/FX, артикуляции, rhythm/groove, section/variation, source/performance maps, provenance/license и runtime status.
- MELODY остаётся one-key/one-pitched-note; PATTERN остаётся chord/voicing + rhythmic performance. PRESET identity, tone, articulation и rhythm не смешивать.
- Электрические и акустические исполнения требуют соответствующих самостоятельных source/performance mappings; нельзя выдавать EQ-перекраску за другой тип исполнения.
- В браузере можно показывать каталоговую цель, но playable статус разрешён только при наличии проверенных assets, mappings и playback path. Placeholders и дубликаты не засчитываются.

### Данные, лицензии и аудит
- Имена групп/композиций и sound DNA — только оригинальные стилистические ориентиры; не копировать master recordings, stems, riffs/transcriptions, коммерческие пресеты, samples или artwork без явных прав.
- Существующий `presets/factory_bank_300.json` уже содержит 21 group profile и 150+150 definitions, но требует отдельного data-integrity audit: в частности, проверить соответствие referenceArtist/referenceTracks, styleProfileId, sampleProfile (electric/acoustic), soundCharacter, playingStyle/groove, section/variation и mappings. Не считать случайные cross-artist song references или несогласованные acoustic/electric labels утверждёнными характеристиками.
- Отсутствие подтверждённого названия песни означает пустое поле, а не выдуманный или чужой пример.
- До аудита не объявлять 300 записей playable, implemented, tested или полностью согласованными.
- Жанровые семейства British Punk и Post-Punk не связывать с конкретными группами догадкой; точные group mappings добавляются только по подтверждённым записям.

### Приоритет и защита от конфликтов
Это дополнение расширяет каталог и описывает sound DNA; оно не отменяет и не ослабляет ранее утверждённые строгие правила, UI/окна, семантику MELODY/PATTERN, MIDI/APVTS/DSP/audio-thread contracts, SoundBank lifecycle и license policy. При любом конфликте остановить затронутую часть и запросить личное решение пользователя. Более поздняя запись меняет старое требование только в явно обозначенной части.
**Статус записи:** APPROVED by user; DOCUMENTED after commit; VERIFIED только после повторного fetch. IMPLEMENTED / TESTED не присваиваются документированием.


## УТОЧНЕНИЕ ПОЛЬЗОВАТЕЛЯ — КОЛИЧЕСТВО FACTORY PRESETS НЕ ИМЕЕТ ВЕРХНЕГО ПРЕДЕЛА
Дата: 14 ноября 2026.
Статус: **APPROVED BY USER — MUST IMPLEMENT**.

Пользователь уточнил и утвердил: количество пресетов PPGU может быть больше любого ранее указанного числа; **верхнего ограничения на общее количество заводских пресетов нет**.

- Число 300 (не менее 150 MELODY и 150 PATTERN) — это только ранее установленный минимальный контрольный ориентир/минимальная планка, а не фиксированный размер банка и не потолок.
- Factory preset catalog является открытым, расширяемым, append-only каталогом: допускается любое дальнейшее количество MELODY и PATTERN presets, а также дополнительных согласованных категорий, без искусственного ограничения по общему числу.
- Новые пресеты добавляются по мере появления проверенных, уникальных и качественных sound profiles, source assets, performance mappings и музыкально обоснованных вариаций. Не создавать пустые placeholders, дубликаты или фиктивные пресеты ради увеличения счётчика.
- Расширение каталога не должно ухудшать запуск, производительность, память, поиск, навигацию или стабильность. Использовать подходящие индексы/ленивую загрузку/кэширование; не загружать весь контент и samples в audio callback.
- Любые количественные счётчики в UI и документации должны показывать фактическое количество записей и не представлять 300 как максимум.
- Это уточнение не отменяет требования к стабильным ID, backward compatibility, лицензиям, реальным mappings, разделению MELODY/PATTERN, качеству и тестированию.
- Если старый текст допускает неоднозначное прочтение числа 300 как лимита, применяется настоящее уточнение: **никакого верхнего лимита нет**. Остальные требования остаются в силе.

**Статусы:** APPROVED / DOCUMENTED после внесения; VERIFIED только после повторного fetch. IMPLEMENTED / TESTED этим документированием не подтверждаются.


## РАСШИРЕННОЕ УТОЧНЕНИЕ — НЕОГРАНИЧЕННЫЙ РОСТ ВСЕХ КАТЕГОРИЙ PRESET BROWSER В MELODY И PATTERN
Дата: 14 ноября 2026.
Статус: **APPROVED BY USER — MUST IMPLEMENT**.

Пользователь уточнил, что отсутствие верхнего ограничения относится не только к общему числу factory presets, но и ко всем соответствующим каталогам и категориям браузера — как для одиночной игры MELODY, так и для режима PATTERN.

### 1. Неограниченное количество элементов
Без фиксированного верхнего предела могут расширяться:
- MELODY / single-play presets;
- PATTERN presets;
- PRESET / GUITAR IDENTITY профили, GENRE, ARTIST / BAND / PUNK GROUP, SONG, SECTION;
- STYLE presets и playing-style профили;
- articulation presets, артикуляции, техники исполнения и их варианты;
- rhythmic categories, PATTERNs, groove profiles, pattern variations;
- tone / sound-character / guitar-source профили и поддерживаемые акустические/электрические варианты;
- performance options, chord/voicing options и другие предусмотренные спецификацией пункты браузера;
- SEARCH/FILTER/FAVORITES/RECENT/SORT/CARDS/CONTEXT/SONG STRUCTURE/SMART FILTERS должны находить и обслуживать весь фактический расширяемый каталог, а не ограниченный фиксированный список.

### 2. Независимость MELODY и PATTERN
- Каталоги MELODY и PATTERN могут расти независимо и не имеют искусственного лимита количества записей.
- Расширение одного режима не должно ограничивать или изменять содержимое другого.
- Сохраняется функциональное разделение: MELODY — обычная одиночная игра нотами; PATTERN — управляемое ритмическое исполнение по утверждённому MIDI/chord contract.
- STYLE, articulation и rhythm остаются отдельными смысловыми уровнями; не смешивать их в один список и не подменять друг другом.

### 3. Без искусственных квот на группы и категории
- Не устанавливать лимиты количества пресетов на одну группу, жанр, композицию, секцию, стиль, артикуляцию, технику или ритмическую категорию.
- Допускается любое количество обоснованных пресетов и вариаций в пределах каждой категории.
- Ранее установленный минимум 300 (150 MELODY + 150 PATTERN) сохраняется только как нижний контрольный ориентир, но не ограничивает ни общий каталог, ни отдельную категорию.
- Добавление новых групп/категорий не должно требовать удаления, замены или переименования уже утверждённых элементов.

### 4. Требования к масштабируемости
- Browser и preset data model должны поддерживать большие каталоги без жёстко зашитых размеров массивов, UI-списков, лимитов на категории или фиксированных верхних счётчиков.
- Применять стабильные IDs, индексацию, lazy loading, pagination/virtualized lists, кэширование и асинхронную подготовку данных там, где это соответствует архитектуре.
- Никакой disk/network I/O, parsing, blocking или неограниченных allocations в audio callback.
- Масштабирование не должно ухудшать время запуска, отзывчивость UI, поиск, переключение пресетов, CPU/RAM или стабильность аудиодвижка.
- Все browser tools должны работать на полном фактическом каталоге; UI показывает реальные количества и не скрывает элементы из-за произвольного внутреннего лимита.
- Производительность и память должны быть измерены на репрезентативно больших каталогах; нельзя считать масштабируемость подтверждённой только по отсутствию UI-потолка.

### 5. Контроль качества и честный статус
- Неограниченный каталог не означает автоматическое добавление пустых, повторяющихся, невалидных или неподдерживаемых записей.
- Каждый элемент должен иметь стабильную идентичность, корректные связи с профилями, источниками и mappings, лицензионный статус и проверяемую совместимость.
- Неподдерживаемые комбинации показываются как недоступные с понятной причиной; нельзя имитировать playable status.
- Нельзя считать новый preset/style/articulation/pattern реализованным до реализации его реального поведения и прохождения соответствующих тестов.

### 6. Приоритет
Это уточнение расширяет предыдущее правило «без верхнего лимита» на все перечисленные сущности и категории обоих режимов. Оно не отменяет и не ослабляет ранее утверждённые правила структуры браузеров, MELODY/PATTERN, MIDI/APVTS/DSP/audio-thread contracts, лицензий, визуальной системы и качества. При обнаружении конфликта остановить затронутую часть и запросить личное решение пользователя.

**Статусы:** APPROVED / DOCUMENTED после внесения; VERIFIED только после повторного fetch. IMPLEMENTED / TESTED не присваиваются этим документированием.
