# POP PUNK GUITAR ULTIMATE 4.0 — ГЕНЕРАЛЬНЫЙ ПЛАН РАЗРАБОТКИ

**Дата консолидации:** 14 ноября 2026  
**Продукт:** POP PUNK GUITAR ULTIMATE 4.0 / L3XTOWN AUDIO LAB  
**Целевая платформа:** Windows 10 x64, VST3, FL Studio 20  
**Статус документа:** APPROVED / DOCUMENTED — генеральный план и критерии приёмки. Это не отметка о завершении реализации.

---

## 0. Назначение и порядок авторитетности

Этот документ объединяет утверждённые продуктовые решения, дополнения и последовательность реализации. Он не заменяет исходный код, тесты и подробные спецификации.

Приоритет источников истины:
1. Последние прямые решения пользователя и этот генеральный план в части их консолидации.
2. Более подробные действующие спецификации: `docs/MASTER_PRODUCT_SPEC_4_0_RU.md`, `docs/PRODUCTION_PRIORITIES_4_0.md`, `docs/PPGU_GUITAR_SOUND_LIBRARY_MASTER_PLAN_RU.md`, `docs/PPGU_JOURNAL/PPGU_JOURNAL.md`, preset/sound-map и ANGELA/PSC design docs.
3. Реальная production-реализация в `Source/`, схемы данных и build configuration.
4. Автоматические тесты, CI, pluginval и фактическая проверка в DAW.

Документы фиксируют намерение; только код показывает реализацию, а только пройденные проверки доказывают корректность. Старые противоречивые формулировки не переписывать задним числом: выявлять их, применять последнее явно утверждённое решение и документировать устранение расхождения.

### Разделение статусов
- **APPROVED** — пользователь утвердил требование.
- **DOCUMENTED** — требование записано в проектных документах.
- **VERIFIED** — запись и исходные материалы повторно проверены.
- **IMPLEMENTED** — функция существует в production path.
- **TESTED** — необходимые тесты реально выполнены и прошли.
- **LICENSE-CLEARED** — права на конкретное использование/распространение проверены.
- **FACTORY-READY** — актив или пресет прошёл техническую, звуковую и лицензионную приёмку.
- **RELEASE-READY** — вся release-матрица пройдена на одном зафиксированном source SHA.

Эти статусы не взаимозаменяемы.

---

## 1. Видение продукта

PPGU — полноценный, быстрый, выразительный и ресурсно-экономный гитарный VSTi. Пользователь выбирает пресет и получает готовый к игре инструмент: корректную карту нот, подходящие реальные гитарные записи, динамику, артикуляции, гитарные приёмы, усилитель/кабинет и обработку.

Это не проигрыватель одной ноты на всех клавишах и не только проигрыватель заранее записанных риффов. Инструмент должен одинаково уверенно поддерживать:
- хроматическую игру отдельных нот и аккордов в Melody;
- управляемое гитарное ритмическое исполнение в Pattern;
- работу из MIDI-клавиатуры, FL Studio Piano Roll и экранной клавиатуры мышью;
- быстрый выбор пресета и предсказуемую загрузку только нужных ресурсов.

Ориентир по рабочему процессу и качеству — профессиональные guitar VSTi, включая Native Instruments Session Guitarist / Electric Sunburst и Impact Soundworks Shreddage. Это функциональные ориентиры, а не разрешение копировать их код, семплы, пресеты, брендинг или графику.

---

## 2. Целевая платформа и ограничения совместимости

- Windows 10 x64.
- VST3 — основной и единственный текущий формат выпуска; VST2 не входит в текущую цель.
- FL Studio 20 — обязательная DAW для реальной host-проверки.
- Быстрый старт, минимальные CPU/RAM/disk spikes, отсутствие зависаний интерфейса и аудио.
- Сохранять совместимость сохранённых проектов, preset/state, MIDI, параметров, routing и существующих пользовательских workflow.
- Существующие APVTS parameter IDs/semantics, sampler lifecycle, voice contracts и asset IDs менять только при доказанной необходимости и с regression coverage.
- Сначала аудит существующей архитектуры и повторное использование подходящих компонентов; не переписывать рабочие части с нуля.

---

## 3. Два согласованных режима игры: MELODY и PATTERN

### 3.1 MELODY — каждая клавиша является своей нотой

Это базовое поведение обычного VST-инструмента:
- C3 играет C3, D3 — D3, E3 — E3 и так далее.
- Каждая клавиша поддерживаемого диапазона воспроизводит собственную высоту тона.
- MIDI note number выбирает корректную ноту/root-key zone и/или допустимое транспонирование.
- Запрещено запускать один и тот же фиксированный звук/неизменённую высоту на всех клавишах.
- Если точного multisample для каждого полутона нет, использовать корректный root-key mapping и качественное транспонирование в проверенном диапазоне либо другой совместимый source zone.
- Нельзя оставлять неожиданные тихие/пустые клавиши, бесконтрольно растягивать сэмпл или подставлять несвязанный звук.
- Note-on/note-off, release/tails, velocity, sustain, retrigger, overlapping notes и полифония должны работать правильно.
- Внешняя MIDI-клавиатура, Piano Roll FL Studio и экранная клавиатура мышью должны давать одинаковую высоту ноты.

### 3.2 PATTERN — каждая клавиша выбирает музыкальный аккорд/исполнение

Pattern — отдельный режим, а не ошибка Melody:
- MIDI-клавиша выбирает chord-map entry/voicing и запускает полноценный гитарный ритмический рисунок.
- Разные клавиши дают соответствующие разные аккорды/voicings; нельзя игнорировать сыгранную клавишу и проигрывать один и тот же аккорд.
- Реальное исполнение включает подходящие down/up strokes, strums, palm-muted strokes, dead/muted strokes, accents, rests, stops и другие поддерживаемые артикуляции.
- Pattern scheduler синхронизируется с BPM/transport хоста; корректно обрабатывает смену аккорда, note-off, stop, seek и сохранение состояния.
- Не подменять Pattern одним sustained chord, произвольным loop или MIDI-pad поведением.
- Acoustic и Electric Pattern используют соответствующие source/performance profiles.

### 3.3 AutoChords в Pattern

- Key/Scale ON: выбранные tonic/root и scale задают детерминированную, документированную гармонизацию.
- Key/Scale OFF: используется прямой chord map выбранного пресета без scale remapping.
- LOCK фиксирует порядок нот и voicing, но не должен случайно фиксировать выбор функции/лада.
- Смена тональности/лада и chord transitions не должны создавать stuck notes, glitches или негармоничные случайные аккорды.

---

## 4. Гитарный sampler и реалистичное исполнение

Сохранять и развивать существующий ANGELA/SamplerEngine production path:
- текущий 64-voice contract — исходная контрольная точка; есть отдельный пользовательский контроль Voices/polyphony;
- SFZ parser и WAV metadata/index;
- background WAV page streaming;
- bounded SPSC request queue, fixed page cache, per-voice page reader;
- correct root keys, note/key zones, velocity layers, round-robin, articulations, loops, releases и tails;
- sample browser/preview и безопасный импорт;
- поддержка MIDI CC/keyswitch mappings, где предусмотрено.

Реалистичные guitar-performance функции по мере наличия записей и реализации:
- sustain/open notes, palm mute, staccato, dead notes/chokes;
- harmonics/pinch harmonics, slides, bends, vibrato, hammer-ons/pull-offs, legato;
- finger/pick variants, fret/string/position-aware voicing;
- strumming/picking patterns, chord transitions, fret noise и humanization;
- double-tracking/width как управляемый эффект/режим, не как обязательное удвоение каждой ноты.

Не показывать артикуляцию как готовую, если нет соответствующей записи или достоверной DSP-реализации. Отличать записанный palm mute от синтезированного/обработанного decay.

---

## 5. Интеллектуальный выбор исходников и multi-source tone assembly

Пресет описывает желаемый музыкальный результат, а не зависит от одного обязательного WAV-файла или файла с названием группы.

Алгоритм выбора:
1. Найти самое точное доступное совместимое исполнение/мультисэмпл.
2. Проверить тип инструмента, электрический/акустический класс, DI/clean/processed состояние, диапазон, tuning, root keys, velocity layers и артикуляции.
3. Если одного источника недостаточно, подобрать совместимые источники из нескольких библиотек или слоёв.
4. Использовать зоны/слои/артикуляции выборочно, а не складывать несколько полных банков на каждую ноту без необходимости.
5. Создать детерминированную карту и сохранить provenance/source IDs, mappings, blend weights, routing, license status и причину fallback.

Обязательные ограничения:
- Acoustic не подменяет Electric; DI не выдаётся за готовый amp tone; processed source не выдаётся за DI.
- One-shot не используется как sustained multisample, если это не отдельная специально обозначенная артикуляция.
- Несовместимый source нельзя произвольно растянуть по всей клавиатуре.
- Dry/original recordings хранятся как отдельные source/dry presets. Они могут быть входом для подходящей amp-processed цепи, но не должны выдаваться за готовый артистический тон.
- Если ни один валидный source/комбинация не подходит, пресет не должен молча играть неверно или молчать: показать понятный статус и nearest valid alternative; не считать его factory-ready до исправления.
- Каждая комбинация проходит реальный аудио-A/B контроль: тембр, pitch, phase, transient, gain, velocity и transitions.

---

## 6. SOUND BANK и Samples

Раздельные сущности:
1. Guitar One-Shots.
2. Guitar Sample Loops.
3. Multisamples / Performance Sets.
4. MELODY factory/user presets.
5. PATTERN factory/user presets.
6. Metadata, mappings, licenses, manifests, checksums.

SoundBank — гитарный: не маркировать drum-only, bass-only, vocal-only или full-mix assets как guitar samples. Acoustic/Electric и Factory/User должны иметь ясное разделение.

### 6.1 Масштабирование и наполнение
- Нет искусственного потолка по числу пресетов, артистов, групп, жанров, стилей, one-shots, loops, mappings и библиотек.
- не менее 300 уникальных READY-пресетов MELODY + PATTERN суммарно; 500–600 — расширенная цель (stretch goal); Travis Barker 82 ведётся отдельной категорией без дублей.
- UI/browser должен показывать фактическое число ресурсов и использовать virtualized lists/lazy loading.
- Пользовательские библиотеки могут подключаться локально в рамках лицензий, даже если их исходные файлы нельзя включить в factory bundle.

### 6.2 Ёмкость и сохранность — обязательные правила
- Managed `Samples` имеет общий жёсткий предел **40,000,000,000 bytes (40 GB decimal)**.
- Старое ограничение 25 GB на один запуск Sound Hunter снято; нового per-run cap нет.
- Manual Import не получает отдельного искусственного 25 GB per-run cap; реальная нехватка дискового пространства сообщается фактически.
- Утверждённая цель установленной factory guitar library — **25–30 GB фактических аудиоактивов**; это целевой размер factory package, а не отдельный лимит managed Samples. Package overhead, metadata и user content учитываются отдельно.
- 40 GB ceiling managed Samples всегда имеет приоритет как физический предел размещения; если добавление/конверсия превысит его — остановиться до записи и сообщить о ёмкости, никогда не освобождать место удалением старых ресурсов.
- Всё успешно помещённое в managed Samples сохраняется постоянно: не удалять, не перемещать, не переименовывать, не перезаписывать, не чистить и не заменять. Новые версии/конверсии — только в новые уникальные пути.
- Перед скачиванием/импортом/конверсией инвентаризировать текущие файлы и SHA-256; exact duplicates повторно не загружать.
- Не использовать пустые файлы, padding, дубли или искусственное перекодирование для достижения target size.

### 6.3 Лицензии и происхождение
- Для factory redistribution требуется явное право включить/распространять исходные reusable samples внутри коммерческого VST3 либо письменное разрешение правообладателя на этот конкретный способ использования.
- Royalty-free для музыки не означает разрешение распространять исходные WAV/multisamples внутри другого плагина.
- Коммерческие библиотеки (включая Session Guitarist, Shreddage, Mike Naran и другие) — ориентиры; не извлекать/не копировать их аудио, код, пресеты или artwork без лицензии.
- Локальный user import не должен автоматически блокироваться лишь потому, что библиотека не допускается в factory package; пользователь сам отвечает за лицензию локального использования.
- Для каждого factory asset хранить URL/vendor/creator, license text/version, permitted uses, redistribution proof, attribution, acquisition date, file format, SHA-256, source ID и mapping metadata.

---

## 7. Sound Hunter, Import Guitar и AUTO DISCOVER

### Sound Hunter
- Искать и получать реальные гитарные записи из законных источников.
- Проверять лицензию до допуска в factory bank, контрольные суммы, целостность архива, безопасные пути, format, mappings и duplicate hashes.
- Сохранять оригиналы и metadata; никакой destructive cleanup.
- Workflow acquisition, per-file registry и staging должны быть воспроизводимыми.
- Не считать артефакт CI автоматически интегрированным в runtime или установленную Samples папку.

### Manual Import / Import Guitar
- Поддержать импорт одиночных файлов и банков в реально поддерживаемых форматах.
- Проверять ошибки формата, пути, metadata, лицензии, mappings, дубликаты, отмену и нехватку места.
- Импортировать добавочно; никогда не менять исходник пользователя.
- Прогресс/отмена/результат и причина отказа должны быть понятны.

### AUTO DISCOVER
- Полная рабочая функция, а не декоративная кнопка: обнаружение совместимых библиотек, корней, форматов, SFZ/maps, недостающих/повреждённых references и source profiles.
- Metadata-first scan на фоне; безопасные root/path checks, stable IDs и incremental rescan.
- Не сканировать и не декодировать весь звук в audio callback.
- Предложенные исправления показывать прозрачно; автоматизировать только однозначные non-destructive операции.

### CURATOR / validation
- Каталогизировать источники и presets по жанру, группе/артисту, альбому/эре, инструменту, sound character, articulation, groove и роли.
- Проверять физические файлы, SHA-256, duplicates, format, clipping/silence/DC/noise, loops, pitch maps, velocity/RR/articulation zones, license, provenance и broken preset references.
- Любой source/preset получает явный readiness/license status.

---

## 8. FACTORY PRESET SYSTEM

### 8.1 Общая цель
- Обязательный релизный минимум: не менее 300 уникальных READY-пресетов MELODY + PATTERN; расширенная цель — 500–600; Travis Barker 82 — отдельная категория.
- Далее каталог может расширяться без искусственного верхнего лимита.
- Каждый пресет обязан быть реально загружаемым, слышимым, правильно маппированным и звуково отличимым.
- Название/описание/FX labels не доказывают готовность. Нужны source IDs, sample map, signal chain, articulation behavior и audio evidence.

### 8.2 Группы/стили
Сохранять весь утверждённый канонический scope из 21 профиля:
Blink-182; Sum 41; Simple Plan; New Found Glory; Good Charlotte; Mest; Angels & Airwaves; The All-American Rejects; MGK; Avril Lavigne; The Offspring; Green Day; Bowling for Soup; Sugarcult; All Time Low; Fall Out Boy; Yellowcard; My Chemical Romance; Less Than Jake; Fenix TX; Midtown.

Для каждой группы различать роли: rhythm, palm-muted rhythm, open chorus, clean/ambient, lead/single-note, octave hooks, arpeggios, accents/stops и другие подтверждённые музыкальные роли.

### 8.3 Обязательные альбомные эпохи — отдельные sound families

**BLINK-182**
- 1999 — *Enema of the State*.
- 2001 — *Take Off Your Pants and Jacket*.
- 2003 — *blink-182* (self-titled/untitled).

**SUM 41**
- 2001 — *All Killer No Filler*.
- 2002 — *Does This Look Infected?*.
- 2007 — *Underclass Hero*.

Каждая эпоха должна иметь собственные tone recipes и различимый реальный звук; нельзя просто переименовать один preset или слегка сдвинуть EQ. Изучать подтверждённые gear/production references, отделять документированные факты от тональной интерпретации. Не копировать оригинальные master recordings/stems или защищённые коммерческие семплы.

### 8.4 Preset readiness
У каждого пресета есть:
- stable preset ID, category, description и sound target;
- instrument/source profile и selected source IDs;
- note/root-key/velocity maps, tuning/range, articulations/releases;
- fallback/multi-source decisions и source provenance/license;
- amp/cab/IR/effects routing и deterministic parameter values;
- CPU/RAM/load profile;
- automated test evidence и rendered-audio approval.

---

## 9. MELODY / PATTERN Browser и музыкальные функции

- Раздельные браузеры/фильтры MELODY и PATTERN, категории/жанры/artist-inspired profiles, поиск, preview, избранное, загрузка и пользовательские банки.
- Preset Save / Save As / Rename / Delete / Import / Export должны работать безопасно и сохранять состояние между обновлениями.
- Preview/preview-on-key не должен подменять полноценный playable instrument.
- PATTERN: pattern browser, chord voicing, rhythm/groove variations, host tempo sync, note-off/stop, chord transition, humanization, AutoChords ON/OFF.
- MELODY: хроматическая игра, артикуляции, velocity, note transitions, release, round robin.
- Один общий понятный workflow, без скрытых несовместимых режимов.

---

## 10. FX EDIT и звукорежиссёрская цепь

FX EDIT — самостоятельное окно/модуль с общим стилем активной темы.

- EFFECT SELECT содержит **10–15 реально реализованных эффектов**, не визуальные заглушки.
- Поддержать утверждённые эффекты, включая Reverb, Flanger, Chorus, Delay, Distortion; расширить набор полезными guitar tools (например EQ, compressor, gate, overdrive/fuzz, modulation, tremolo и другие только если реально реализованы).
- Отдельный корректный cabinet/IR selection и amp/cab routing; кабинеты не выдавать за эффекты, если это отдельный DSP block.
- Контролировать signal order, gain staging, wet/dry, bypass, preset save/restore и automation.
- Проверить phase/mono compatibility, clipping, denormals, latency/PDC где нужно, sample-rate changes и стабильность параметров.
- Каждый FX module должен иметь реальный DSP, tests и слышимый результат. Никаких неработающих placeholder controls.

---

## 11. ANGELA ENGINE — производительность и надёжность

Главный принцип: непрерывно оптимизировать существующий движок, не переписывая его с нуля и не ломая звук/функции.

- Сначала baseline: CPU, RAM, startup, preset load, page-cache hit/miss, disk I/O, audio underruns, voice/polyphony stress.
- Reuse existing SamplePool, SFZ parser, streaming service, fixed page cache, per-voice reader и другие production components.
- Audio callback: никакого file/network I/O, decoding, SFZ parsing, hashing, blocking lock/wait, непредсказуемых allocations/deallocations.
- Background workers подготавливают data; audio thread читает только готовые данные через bounded non-blocking structures.
- Lazy loading: при выборе пресета подготовить только необходимые mappings/assets; не загружать всю библиотеку в RAM.
- Shared caches/pages, bounded memory, prefetch только соседних нужных страниц.
- Multi-source layers использовать лишь при доказуемом слышимом выигрыше и в рамках CPU/RAM budget.
- Voice budgeting/polyphony и multiprocessing/background work не должны вызывать CPU spikes или ухудшать realtime.
- Все оптимизации — маленькие, изолированные, обратимые; benchmark before/after и audio A/B mandatory.

---

## 12. PSC — собственный Sample Container (.ppgs)

PSC пока является проектной спецификацией, а не реализованной заменой WAV/SFZ.

Порядок:
1. Зафиксировать baseline текущего WAV/SFZ streaming.
2. Спроектировать versioned header, manifest/index, chunk table, independent chunks, checksums, bounds checks и metadata.
3. Offline converter создаёт дополнительный файл в новом уникальном пути; оригиналы остаются нетронутыми.
4. Сравнить lossless PCM/FLAC chunk options по размеру, CPU, seek/random access, decode latency и quality.
5. Реализовать PSC reader как дополнительный backend рядом с WAV/SFZ.
6. Background page requests + bounded cache; audio callback только получает ready pages.
7. Протестировать corruption/truncation/fuzzing, maps, loops, RR, velocity, articulations, source integrity, audio A/B и resource budgets.
8. Только после полного regression pass решать, какие factory assets переводить в PSC. WAV/SFZ остаются поддерживаемым fallback.
9. PSC bytes внутри managed Samples учитываются в 40 GB; никакого автоматического удаления WAV/оригиналов ради освобождения места.

---

## 13. UI, skins и окна

- Основная approved skin: **NEON STAGE DEFAULT**; использовать утверждённый первый mockup как reference layout, второй — как palette reference; остальные skins производны от этой основы.
- Все окна/модули используют единый стиль активной темы. FX EDIT, browser, import, settings, about, discover, curator и другие окна не должны выглядеть отдельными несогласованными приложениями.
- Skins меняют graphics/palette only: не менять координаты, controls, functions, bindings, DSP, sound, parameter IDs или preset behavior.
- Default skin загружается автоматически.
- SKINS GALLERY/skin selection и hot swap считаются готовыми только после реальной реализации/тестов; не показывать декоративные неработающие элементы.
- Поддержать корректный DPI scaling, resize/window focus, keyboard interaction, contrast, accessibility и сохранение состояния.
- UI не должен блокировать audio thread; измерять paint time и память skins отдельно.
- Проверить все семь ранее утверждённых UI reference examples и доступность исходных mockups/assets до pixel/layout acceptance.

**Примечание по согласованию документов:** в старых разделах MASTER_PRODUCT_SPEC встречается более раннее описание фиксированного charcoal/magenta editor. Последнее утверждённое направление NEON STAGE DEFAULT должно быть приведено в соответствие через отдельный UI reconciliation audit с проверкой mockups и source tree, а не через слепое изменение старых исторических записей.

---

## 14. Остальные утверждённые продуктовые модули

Включить в единый release scope и закрыть по реальной реализации:
- AUTO DISCOVER — полноценный scanner/resolver.
- CURATOR — управление библиотекой, метаданными, совместимостью и качеством.
- IMPORT GUITAR / Samples import — форматы, лицензии, mapping, progress/cancel, no-overwrite.
- SAMPLE BROWSER — поиск, категории, preview, audition.
- FX EDIT / EFFECT SELECT / cabinets.
- MELODY / PATTERN / AutoChords.
- SKINS GALLERY и единая skin/theme система.
- SETTINGS — реальные настройки и persistence.
- ABOUT — корректная версия, лицензии, credits и ссылки.
- DOUBLE — реальная функция двойного трекинга/удвоения с контролем ширины/задержки/фазы.
- Voice/polyphony control и background/multiprocessing function с realtime-safe архитектурой.
- MIDI mappings, keyboard interaction, state save/restore и preset management.
- Portable content-root mode и стандартная VST3 installation.
- Documentation, license notices, diagnostics, error states и recovery paths.

Каждый пункт должен быть проверен по фактическому коду и отдельным acceptance tests; список сам по себе не означает готовность.

---

## 15. Тестовая стратегия и обязательные release gates

### G0 — repository/source audit
- Определить canonical production files, duplicate/legacy implementations, CMake/JUCE paths, ownership/lifecycle.
- Составить module map и gap list; не изменять код до определения реального execution path.

### G1 — baseline and contracts
- Windows x64 Release build.
- Full CTest baseline.
- APVTS/state/preset schema/MIDI contracts.
- Sampler/streaming lifecycle, 64-voice baseline, RT callback audit.
- Baseline CPU/RAM/load/underruns.

### G2 — SoundBank and import
- Inventory, hash, metadata, license and source manifest.
- Importer/Sound Hunter/Auto Discover tests.
- Capacity guard and permanent no-delete/no-overwrite regression tests.
- Preset-to-source resolver tests and broken-reference diagnostics.

### G3 — playable MELODY instrument
- Chromatic note sweep across entire supported range.
- Verify fundamental pitch per MIDI note, semitone and octave intervals.
- Detect repeated fixed pitch, wrong root key, silence, note gaps and excessive transposition artifacts.
- Velocity sweeps, layers, RR, note-off, release, overlapping notes, 64-voice stress.
- Test external MIDI, Piano Roll and on-screen keyboard.

### G4 — PATTERN and AutoChords
- Different chord map entries produce correct distinct chords/voicings.
- Strum/picking event timing and guitar-specific articulations.
- BPM/transport sync, note-off, stop, seek, chord transitions.
- AutoChords ON/OFF, tonic/scale changes, LOCK semantics and save/restore.

### G5 — sound quality and preset factory
- Every factory preset loads and resolves valid assets.
- Each preset rendered; compare declared style and source identity.
- Artist/album families are audibly distinct, not only different labels.
- Check gain/phase/mono, noise, clipping, transitions and articulation.
- No preset can be marked ready if it is silent, missing mappings, wrong-pitch or license-ineligible.

### G6 — DSP/FX and UI
- All 10–15 FX are actual DSP implementations.
- FX routing, parameter automation, state save/restore, bypass, cabinet/IR and host behavior.
- Skin reference comparison, UI windows, DPI/resize, keyboard and control bindings.
- Performance/paint time and no regressions.

### G7 — performance/reliability
- CPU/RAM, startup, preset-switch time, cold/warm load, streaming/cache, disk I/O, underruns.
- Single-source vs multi-source resource profile.
- Polyphony and sustained playback stress.
- No audio-thread I/O, locks, waits or unpredictable allocations.
- A/B audio identity before/after ANGELA changes.

### G8 — release/package/host
- Windows 10 x64 VST3 build from one exact commit SHA.
- CTest full suite and pluginval strictness 5.
- Package manifest and hashes tied to exact source SHA.
- Package contains only legally redistributable factory samples and complete required runtime resources.
- Relocation/install smoke test and archive/path audit.
- Manual FL Studio 20 test: scan, instantiate, preset load, MIDI/Piano Roll, note range, Pattern sync, save/reopen project, plugin state, GUI and long playback.
- CI success alone is not FL Studio host acceptance.

---

## 16. Порядок генеральной разработки

Выполнять небольшими закончиваемыми этапами; после каждого этапа обновлять журнал и evidence.

**Phase A — Consolidation & audit**
1. Reconcile all current docs and conflicting legacy wording.
2. Inventory repository branches/source tree/build config and exact latest main SHA.
3. Map production paths for plugin, sampler, presets, UI, FX, import, SoundBank and workflows.
4. Audit all existing legacy catalogue definitions (including the 300-entry set) against the updated 500-minimum/~600-target scope and real preset integration status.
5. Audit current Samples cache/artifact/registry and exact license/checksum/mapping coverage.

**Phase B — Core playable instrument**
6. Lock down MIDI note-to-pitch mapping and Melody sample zones.
7. Ensure preset selection resolves a valid instrument map and readiness state.
8. Implement/validate compatible source fallback and deterministic multi-source maps.
9. Add full-range chromatic/velocity/note-off/chord/pitch regression suite.
10. Validate in DAW Piano Roll, MIDI controller and on-screen keyboard.

**Phase C — SoundBank production**
11. Finish non-destructive Sound Hunter and Import Guitar audit.
12. Implement/complete AUTO DISCOVER, Curator, sample browser and validation.
13. Populate licensed electric/acoustic one-shots, multisamples, performance sets and loops.
14. Preserve dry/original source banks and provenance; validate all sources.
15. Complete factory/user separation and capacity enforcement.

**Phase D — Guitar performance and patterns**
16. Finish guitar articulation and transition engine.
17. Finish PATTERN scheduler, chord voicing, strumming/picking and AutoChords.
18. Implement double tracking and performance controls.
19. Validate pattern data and all MIDI/transport edge cases.

**Phase E — Sound design and factory presets**
20. Complete 10–15 effect DSP modules plus amp/cab/IR routing.
21. Build at least 300 unique, fully implemented and audio-validated MELODY + PATTERN factory presets as a hard release minimum; expand toward 500–600 only as a stretch goal; track Travis Barker 82 separately without duplicates or double counting.
22. Create the 21 approved group/style families.
23. Create six album-era families for Blink-182 and Sum 41.
24. Render/A-B/validate every preset and record exact source/chain/evidence.

**Phase F — UI and product finish**
25. Reconcile and implement NEON STAGE DEFAULT against approved mockups.
26. Finish unified theme support and all windows/modules.
27. Complete Settings, About, Skins Gallery, browser, Import, FX Edit and diagnostics.
28. Verify persistence, accessibility, DPI and interaction consistency.

**Phase G — Optimization and packaging**
29. Benchmark ANGELA Engine and optimize only measured bottlenecks.
30. Prototype and validate PSC as an optional backend; no forced migration.
31. Run full release matrix and package audit.
32. Perform actual FL Studio 20 acceptance and publish final verified VST3 + lawful soundbank + install/readme.

---

## 17. Автономная работа и пределы разрешения

Пользователь делегировал полное разрешение на качественную разработку. В рамках этого разрешения:
- самостоятельно выбирать лучший технический способ, проводить исследования, интегрировать подходящие открытые решения после проверки лицензий, исправлять однозначные локальные дефекты;
- не запрашивать подтверждение для каждого безопасного, обратимого, non-breaking шага;
- вести полные журналы, CI, tests, benchmarks и сводные отчёты;
- продолжать независимые безопасные задачи, если один участок заблокирован.

Обязательные пределы:
- не удалять/не перезаписывать/не перемещать Samples и пользовательские данные;
- не обходить лицензии, авторские права, доступы или условия сторонних ресурсов;
- не выдавать documentation-only за implementation, а CI за host validation;
- не менять утверждённые контракты/поведение, не удалять функции и не ухудшать звук ради простоты;
- необратимые действия, юридически сомнительные решения, конфликтующие пользовательские утверждения и риск потери данных требуют остановить только соответствующую часть и запросить конкретное решение;
- никакой фиктивной отметки PASS.

---

## 18. Проверенный статус на момент консолидации (14.11.2026)

### Подтверждено документами/CI
- Основная спецификация 4.0, production priorities, SoundBank master plan, preset readiness, ANGELA/PSC design и журнал решений существуют в репозитории.
- Пользовательские дополнения по интеллектуальному fallback, multi-source tone assembly, стандартному VSTi preset loading, отдельной ноте на каждой клавише, профессиональным guitar VSTi workflow и шести album-era families внесены в журнал.
- В последнем проверенном Windows VST3 workflow run **#1546 / 43584870207**, source SHA `7681c945959afa9da437a35eec3dc3e5cfc20d66`, шаги Configure x64 Release, Build VST3/tests, CTest, locate VST3 и pluginval strictness 5 завершились success. Cache restore, SoundBank/license audit и registry generation также прошли.
- Тот же workflow завершился с общим conclusion **cancelled** на этапе package; upload package был skipped. Поэтому это не готовый релиз и не опубликованный установочный пакет.
- Шаг workflow, фиксирующий ограничение host validation, не является запуском FL Studio. Реальная ручная проверка FL Studio 20 не подтверждена.
- Предыдущий Sound Hunter acquisition workflow #13 завершился success и опубликовал ZIP artifact около 2.97 GB; наличие артефакта не доказывает его runtime integration, полного содержимого, лицензий каждого файла или установки в Samples без отдельной инспекции.
- PSC `.ppgs` — design only; implementation/converter/backend/benchmarks pending.
- Предыдущая readiness-документация указывает, что 300 preset entries — catalogue definitions, а не 300 подтверждённо интегрированных и playable runtime presets. Не считать их готовыми без повторной проверки актуального main source и registry.

### Следующие обязательные проверки
1. Проверить, что актуальный main HEAD включает последние journal commits и согласовать master plan.
2. Инспектировать текущий SoundBank artifact/cache registry: фактические файлы, лицензии, SHA-256 coverage, mappings, capacity и source list.
3. Провести свежий Windows Release + CTest + pluginval run на exact main SHA и довести package job до upload.
4. Выполнить source-to-preset audit для каждого factory preset.
5. Провести full-range MIDI pitch tests и реальное воспроизведение.
6. Провести FL Studio 20 host validation.
7. Только после этого менять статусы на IMPLEMENTED / TESTED / FACTORY-READY / RELEASE-READY.

**Текущий общий статус:** генеральный план и требования консолидированы; разработка продолжается. Продукт не объявляется завершённым до прохождения всех release gates.


## 19. Angels & Airwaves / Tom DeLonge — approved research and preset expansion (2026-11-14)

Detailed research and implementation criteria: [PPGU_ANGELS_AIRWAVES_TOM_DELONGE_PRESET_RESEARCH_RU.md](./PPGU_ANGELS_AIRWAVES_TOM_DELONGE_PRESET_RESEARCH_RU.md).

Mandatory album eras: *We Don't Need to Whisper* (2006), *I-Empire* (2007), *Lifeforms* (2021). Add separate MELODY and PATTERN families for each era and dedicated Tom DeLonge technique families in both modes: resonant-note riffs/supporting movement, melodic hooks, octave and upper-register lines, chiming/arpeggiated phrases, open power-chord choruses, palm-muted drive, sustained/echoing phrases and sparse-to-wide builds. Preserve exact chromatic MIDI pitch in MELODY and chord-aware performance generation in PATTERN.

Research public interviews, gear/effect-chain breakdowns, tone recipes and free preset examples. Record confirmed facts separately from tonal interpretation. Public information may inform independently authored DSP recipes. Third-party preset files, IRs and samples may only enter the commercial factory bundle where the specific license expressly permits redistribution; otherwise independently reproduce the general tone concept without copying protected assets, code, riffs or recordings.

**Preset count correction:** the approved hard release minimum is at least 300 unique, fully implemented and audio-validated MELODY+PATTERN presets total; 500–600 remains a stretch goal after the minimum is met. Travis Barker 82 is a separate category of 82 unique complete presets; do not merge, duplicate or double-count it. The old 300-entry catalogue remains a legacy audit baseline. Counts never replace functional and audio validation.

Research evidence includes Ernie Ball String Theory (resonant-note approach and echo in A&A), Guitar World interview on *I-Empire* (chiming upper-frequency parts, compression/delay/flanger/stereo echo), and Guitar.com/GRAMMY interviews on *Lifeforms* (broader/rawer guitar palette and gear context). Exact track-specific chains remain subject to further research. No implementation or audio validation is claimed by this scope update.



## Обязательное аудиотабу: никакого пластикового и нереалистичного звучания

**Статус: APPROVED / MANDATORY QUALITY GATE.** Для PPGU 4.0 категорически недопустимы пластмассовые, пластиковые, искусственные, дешёвые, стерильные или явно синтетические гитарные тембры, если они воспринимаются как нереалистичная имитация гитары. Это безусловное табу для фабричных пресетов, сэмплов, акустических и электрических режимов, демо, предпрослушивания и финального продукта. Целевое позиционирование — дорогой инструмент по качеству звука; количество пресетов, эффектов и гигабайтов не может оправдывать компромисс по реалистичности.

Каждый источник и каждый пресет должны оцениваться на слух в реальном игровом контексте: ноты по всему заявленному диапазону, разные velocity, атака/затухание, note-off/release, артикуляции, одиночные ноты и аккорды, переходы и динамика. Проверять через DRY/NATIVE TONE MATCH, PROCESSED SOURCE MATCH и WET-пути. Допускается минимальная или нулевая дополнительная DSP-обработка, когда исходная запись уже убедительно соответствует требуемому тону; качественно записанный обработанный источник также допустим, если он реалистичен и не требует бессмысленного дублирования эффектов.

Любой пресет с пластиковым/синтетическим оттенком, неестественной атакой или затуханием, одинаковым «клавишным» тоном на разных нотах, слышимыми артефактами растяжения либо неправдоподобной динамикой получает **REJECTED / NOT READY**. Его нельзя выпускать в фабричной библиотеке до исправления источника, маппинга, артикуляции, velocity-слоёв или DSP и повторного прослушивания. Автоматические тесты и pluginval не заменяют слуховую проверку. Критерий должен быть отражён в acceptance tests и чек-листах релиза; никаких заявлений об аудиопроверке без фактически выполненного теста.



## Обязательный минимум фабричного каталога: не менее 300 полноценных пресетов

**Новое прямое решение пользователя: APPROVED / HARD MINIMUM / RELEASE REQUIREMENT.** В релизной фабричной библиотеке PPGU 4.0 должно быть не менее **300 уникальных, полностью реализованных и проверенных MELODY + PATTERN пресетов суммарно**. Это жёсткий нижний предел, а не желательная цифра и не количество строк каталога. Ни один из 300 не может быть заглушкой, пустым пресетом, переименованным дублем или неработающей комбинацией параметров. Каждый должен иметь легальный реальный источник/маппинг, корректную игровую функцию, музыкально оправданное звучание и пройденную слуховую проверку на отсутствие пластикового/нереалистичного тембра.

Каталог строится с учётом всех утверждённых групп и artist-inspired профилей, жанров и поджанров, акустических/электрических вариантов, чистых/перегруженных характеров, а также музыкальных ролей и частей песни. Отдельно планировать пригодные для INTRO, VERSE, PRE-CHORUS, CHORUS, POST-CHORUS, BRIDGE, BREAKDOWN, MAIN RIFF, LEAD/HOOK, ARPEGGIO/PICKING, FILLS/TRANSITIONS и OUTRO исполнения/тембры, где это музыкально уместно. Это не означает механически размножать один пресет по названиям секций: варианты засчитываются отдельно только при реальном различии звучания, партии/исполнения, артикуляции или назначения и самостоятельной проверке.

Предыдущий расширенный ориентир 500–600 сохраняется как **stretch goal / цель расширения**, но не заменяет и не повышает обязательный релизный минимум: сначала обеспечить минимум 300 качественных READY-пресетов; далее расширять каталог до 500–600 только при сохранении всех аудиокритериев. Travis Barker 82 остаётся отдельной категорией из 82 уникальных полных пресетов и ведётся отдельно от основного счёта, без дублей и двойного подсчёта. Для каждого пресета вести учёт: профиль/группа, жанр/поджанр, MELODY или PATTERN, акустика/электро, характер/усиление, музыкальная роль/секция песни, sample/source ID, лицензия, артикуляции, эффекты, статус реализации и аудиотеста.

