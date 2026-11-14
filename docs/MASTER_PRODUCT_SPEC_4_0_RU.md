> **Актуализация 2026-11-12:** этот документ сохраняется как базовая спецификация PPGU 4.0. Для приоритетов, release gates, текущих технических решений и фактического статуса использовать принятую MASTER PROMPT REV 13.0 FINAL и единственный журнал Info.txt. Более поздние прямые решения пользователя имеют приоритет над этим документом.
>
> **UI decision:** текущий продуктовый интерфейс — один фиксированный JUCE editor в палитре dark charcoal / magenta. Не считать отдельные SVG-концепты или skin manifests готовыми runtime skins; не показывать несуществующий skin selector/gallery/hot-swap.

# POP PUNK GUITAR ULTIMATE 4.0 — полное техническое задание

**Статус документа:** сводное утверждённое описание требований пользователя.  
**Назначение:** единый источник требований для реализации, интеграции, тестирования и выпуска.  
**Важно:** наличие требования в этом документе не означает, что функция уже реализована. Статус каждой функции подтверждается кодом, тестами и проверкой в DAW.

## 1. Цель продукта

POP PUNK GUITAR ULTIMATE — лёгкий VST3-инструмент на основе сэмплированных электрогитар и акустических гитар для создания поп-панк партий. Основной ориентир по удобству игры и организации инструмента — Native Instruments Session Guitarist / Electric Sunburst Deluxe; по организации сэмплерной библиотеки и загрузке инструментов — Kontakt 6.

Это ориентир по пользовательскому сценарию и принципам работы. Запрещено копировать закрытый код, интерфейсные материалы, фирменные сэмплы, пресеты или коммерческие библиотеки NI и других правообладателей.

Главный приоритет: плагин должен быстро запускаться, быстро переключать пресеты и библиотеки, сразу реагировать на MIDI и не создавать заметной нагрузки на компьютер.

## 2. Платформа и совместимость

- Windows 10 x64, начиная с версии 1809 (сборка 17763), и более новые версии Windows 10.
- Формат: VST3 x64.
- Основная обязательная DAW для проверки: FL Studio 20 x64.
- Совместимость с более новыми версиями FL Studio желательна, но не заменяет проверку на FL Studio 20.
- Проверить установку, сканирование VST3, открытие/закрытие, MIDI-клавиатуру, Piano Roll, автоматизацию параметров, сохранение проекта и восстановление состояния, смену аудиоустройства и длительное воспроизведение.
- Сборка и зависимости не должны незаметно повышать минимальную версию ОС выше Windows 10 1809.

## 3. Основная логика инструмента: два режима

Пользователь выбирает пресет, затем играет с MIDI-клавиатуры или из Piano Roll. В обоих режимах каждая клавиша имеет своё назначение. Результат зависит от выбранного режима.

### 3.1 Melody — клавиша играет отдельную ноту

- Каждая MIDI-клавиша запускает соответствующую хроматическую ноту гитары: например, C4 играет C4, D4 — D4.
- Это полноценный playable multisampled guitar instrument, а не запуск готовой фразы и не обычный синтезаторный звук.
- Правильная карта root key / key range, транспонирование только в разумных пределах, мультисэмплы по диапазону грифа.
- Пресет задаёт гитару, библиотеку, тембр, артикуляции, velocity response, усиление/обработку и другие настройки.
- Поддержать электрические и акустические профили.
- Поддержать velocity layers, round robin при наличии нескольких дублей, open/picked, palm mute, dead/muted articulations, release/sustain. Legato, slide, hammer-on и pull-off включать только при наличии соответствующих записей.
- Velocity может переключать или усиливать palm mute по настраиваемому порогу. Пользователь ранее отдельно утвердил palm mute при уменьшении velocity.
- MIDI note-on/note-off, sustain, перекрывающиеся ноты, pitch bend/modulation — с документированным и протестированным поведением.
- Настраиваемые voices/polyphony, ограничение максимального числа голосов и предсказуемое voice stealing без щелчков.

### 3.2 Pattern — клавиша выбирает аккорд и запускает ритмическое исполнение

- Каждая клавиша имеет собственное назначение в карте аккордов: нажатие клавиши выбирает соответствующий аккорд/гитарный voicing и запускает полноценный ритмический рисунок этого аккорда.
- Соседние клавиши вызывают другие аккорды согласно карте выбранного пресета. Карта должна быть понятной, редактируемой/документированной и согласованной с MIDI-диапазоном.
- В отличие от Melody, клавиша не просто запускает одну одиночную ноту: она вызывает аккордовое исполнение на акустической или электрогитаре в стиле поп-панк.
- Pattern должен реагировать на смену клавиши/аккорда без зависших нот, обрывов и сбитого ритма.
- Поддержать синхронизацию с темпом и транспортом хоста, корректное начало/остановку, note-off/gate behavior, смену аккордов, humanization и акценты.
- Виды исполнений: открытые down/up strums, alternate down-up, tight palm-muted rhythms, dead/muted/percussive strokes, power-chord rhythms/riffs, arpeggios и picking patterns, акустические strums и подходящие акустические переборы.
- Электрический пресет должен использовать соответствующие электрогитарные артикуляции и voicings; акустический — акустические strums/picking и соответствующие записи. Нельзя просто менять EQ одного и того же паттерна и выдавать это за разные исполнения.
- Использовать реальные гитарные performance recordings и/или детально размеченные performance events: chord voicing, string/fret, stroke direction, timing, velocity, accent, mute/articulation, gate и переходы.
- Не подменять Pattern универсальным MIDI-пэдом, одним удерживаемым аккордом, случайно транспонированной MIDI-фразой или лупом, который не реагирует на выбранную клавишу.
- Одних одиночных нотных мультисэмплов недостаточно для реалистичных аккордовых strum-паттернов: нужны соответствующие chord/strum/performance-записи либо специально подготовленные performance-данные.

## 4. Пресеты — 300 заводских

- Всего 300 заводских пресетов: 150 Melody и 150 Pattern.
- Две основные вкладки/категории: MELODY и PATTERN.
- Пресеты должны быть реально загружаемыми и играть на соответствующем материале. Запись в каталоге без доступной карты сэмплов и runtime-загрузчика не считается готовым пресетом.
- Организация по жанру, характеру гитары, усилению, назначению и artist-inspired стилям.
- Обязательные стилевые ориентиры: Blink-182, Sum 41, Simple Plan, New Found Glory, Good Charlotte, Mest (включая ориентир на “Photographs”, 2005), Angels & Airwaves, The All-American Rejects, MGK, поздний поп-панк период Avril Lavigne. Также ранее обсуждались The Offspring, Green Day, Bowling for Soup, Sugarcult и All Time Low.
- Это описания музыкального направления и характера звучания, не копирование конкретных мастер-записей, партий, коммерческих пресетов или записей артистов.
- В браузере: категории/папки, artist/style tags, поиск, избранное, сортировка, предыдущий/следующий, предпрослушивание, загрузка двойным нажатием, Save, Save As, Rename, Delete пользовательского пресета, Import/Export Bank, Rescan.
- Заводские пресеты только для чтения; пользовательские пресеты должны сохраняться между обновлениями.

## 5. Библиотека и сэмплерная архитектура

- Модель работы сэмплерная, в духе Kontakt 6: библиотеки/инструменты/пресеты, карты нот и velocity, артикуляции, предпрослушивание, загрузка выбранного инструмента и немедленная игра.
- SFZ/WAV loader с обработкой key range, root key, velocity layers, round robin, артикуляций, loop points, sample rate, каналов и относительных путей.
- Ранее указанные приоритетные материалы для проверки и интеграции: Meta Open Sound Steel Guitar и Nylon Guitar; также изучить FreePats clean/distorted варианты.
- Steel Guitar и Nylon Guitar по возможности должны иметь независимые профили/банки и явное переключение/загрузку/выгрузку.
- Реализовать sample browser и audition/preview: пользователь может прослушать звук/материал до выбора.
- Проверять лицензии каждого файла. В поставку включать только материалы, чьи условия прямо разрешают включение и распространение внутри плагина/пакета. Сохранять attribution, license texts и SHA-256 hashes.
- Не включать коммерческие сэмплы Session Guitarist, коммерческие riff packs, чужие записи песен или артистов без соответствующих прав.
- Пресеты должны ссылаться на логический sampleProfile/library ID, а не на абсолютные пути конкретного компьютера.
- При отсутствии библиотеки показывать понятное предупреждение, сохранять настройки пресета и не подменять отсутствующие записанные гитары синтезатором, выдавая его за оригинальный звук.

## 6. Быстрая загрузка и низкая нагрузка на ПК

Это один из главных критериев качества продукта.

- Быстрый cold start плагина.
- Быстрый выбор и переключение пресетов без долгих зависаний.
- Быстрый запуск звука после выбора пресета.
- Индексация библиотек и сканирование файлов вне аудиопотока.
- Lazy loading, предварительная загрузка нужных участков, prefetch и ограниченный кэш недавно используемых сэмплов.
- Безопасная выгрузка неиспользуемых банков и освобождение памяти без повреждения активных голосов.
- Аудиоколбэк не должен выполнять disk I/O, выделять память или ждать блокирующие mutex/locks.
- Подготовленное состояние банка/пресета передавать аудиодвижку безопасным неблокирующим способом.
- Ограничить polyphony, буферы и потребление RAM; voice stealing должен быть детерминированным и по возможности click-free.
- Эффекты должны быть экономными по CPU, с разумными настройками качества.
- Не добавлять фиктивный переключатель multicore/multiprocessing. Такая функция допустима только при реальной безопасной параллельной обработке и измеримом выигрыше.
- Предусмотреть низкую/обычную настройку качества, если это даёт пользователю реальный контроль над нагрузкой.
- Обязательно измерять cold-start, время переключения пресета, idle/playback CPU, peak/steady RAM и отсутствие аудиоглитчей на тестовой системе. Не заявлять “не нагружает ПК” без измерений.

## 7. Эффекты и формирование тона

Встроенные эффекты/контроли:
- Distortion / overdrive.
- Amp/tone controls и характер усилителя.
- Chorus.
- Flanger.
- Delay.
- Reverb.
- Acoustic mode/profile для акустических вариантов звучания структур и пресетов.
- Palm mute должен корректно работать вместе с выбранной обработкой, включая reverb, если это предусмотрено пресетом.
- Цепочка обработки должна быть предсказуемой, экономной и не создавать ненужной нагрузки. Параметры должны автоматизироваться из DAW и сохраняться в состоянии проекта.

## 8. Интерфейс и удобство

- Простой и привлекательный интерфейс, ориентир по быстроте и ясности — FLEX/Nexus по удобству, но с собственным дизайном.
- Визуальная тема/направление: тёмный/чёрный интерфейс; ранее обсуждалось использование гитарного изображения или поп-панк визуального направления, в том числе Blink-182 как стилистического ориентира, без использования защищённых изображений без прав.
- Поддержать сменные skins на основе предоставленного пользователем архива POP_PUNK_GUITAR_L3XTOWN_SKINS_SOURCE.zip; интеграцию и работоспособность каждого скина отдельно проверить.
- Смена skin не должна менять функциональность, MIDI mapping, звук или сохранение пресетов.
- Браузер должен ясно показывать режим, категорию, выбранный пресет, статус библиотеки/сэмплов и предупреждения об отсутствующих файлах.
- Sample preview должен позволять услышать материал до выбора.

## 9. Форматы и хранение пользовательского контента

- .ppgm — один Melody preset.
- .ppgp — один Pattern preset.
- .ppgbank — архив банка с manifest и файлами пресетов.
- .ppgstate — необязательный полный snapshot состояния плагина для DAW-проекта.
- Форматы должны иметь версию схемы, стабильный ID, имя, mode, tags, параметры, артикуляции/performances и логическую ссылку на sample library.
- Предполагаемый пользовательский корень Windows: %APPDATA%/L3XTOWN/POP PUNK GUITAR/.
- Заводские пресеты — read-only; пользовательские пресеты и банки — в AppData, не перезаписываются обновлением плагина.
- Импортировать банк через предварительный просмотр содержимого; отклонять path traversal, неподдерживаемые версии, некорректные и чрезмерно большие файлы.
- Не перезаписывать пользовательские файлы без подтверждения. Загрузка должна быть атомарной: при ошибке старое рабочее состояние сохраняется.
- Сохранение/восстановление DAW-проекта должно восстанавливать выбранный пресет, режим, параметры и ссылки на доступные библиотеки.

## 10. MIDI, voices и управление

- MIDI-клавиатура и FL Studio Piano Roll — основные способы игры.
- В Melody: одна клавиша = одна соответствующая нота.
- В Pattern: одна клавиша = назначенный аккорд и его ритмическое гитарное исполнение.
- Поддержать настраиваемый лимит voices/polyphony и корректное завершение/переключение нот.
- Поддержать host tempo/transport sync в Pattern.
- Параметры должны быть доступны для автоматизации FL Studio и сохраняться в проекте.
- Реальный multiprocessing не является обязательным релизным требованием, если он ухудшает стабильность или не даёт измеримого выигрыша.

## 11. Проверки и критерии готовности

Нельзя объявлять продукт завершённым только по наличию документации, каталога из 300 записей, макета UI, статического аудита или успешной компиляции.

Обязательные этапы:
1. Чистая Windows x64 VST3 сборка с минимальной поддержкой Windows 10 1809.
2. Успешный Pluginval, включая строгий режим 5.
3. Тесты нотной карты, root key, velocity layers, round robin, артикуляций, palm mute threshold, note-on/off, release, voice limits и voice stealing.
4. Тесты Pattern: chord map, voicings, string/fret legality где применимо, performance events, down/up strokes, chord changes, tempo/transport sync, note-off/stop и отсутствие зависших нот.
5. Тесты сериализации пресетов, сохранения/загрузки, импорта/экспорта банков, повреждённых/неподдерживаемых файлов и отсутствующих сэмплов.
6. Проверка всех 150 Melody и 150 Pattern через реальный runtime browser и loader.
7. Проверка лицензий, атрибуций и SHA-256 каждого распространяемого sample asset.
8. Ручная проверка в FL Studio 20 x64 на Windows 10 1809+: установка, сканирование, загрузка, MIDI, Piano Roll, автоматизация, смена пресетов во время воспроизведения, Melody и Pattern, смена аккордов, tempo sync, save/reopen, смена аудиоустройства и длительная работа.
9. Измерения cold-start, preset-switch latency, RAM, CPU и аудиоглитчей на обозначенной тестовой системе.
10. Чистая установка на машине без ранее установленных библиотек; понятное восстановление при отсутствующих библиотеках.
11. Поставка VST3, необходимых легально распространяемых sample assets, лицензий/атрибуций, версии, инструкции установки и списка проверенной совместимости.

## 12. Правила отчётности по статусу

Для каждой функции указывать один из статусов:
- Implemented — реализована в коде и проверена соответствующим тестом.
- Partial — реализована частично; явно перечислить недостающие части.
- Unverified — код/сборка существуют, но корректность в целевом хосте или на целевой ОС не подтверждена.
- Not implemented — ещё не реализована.
- Blocked — реализация заблокирована конкретной технической или лицензионной причиной.

Не называть сборку финальной POP PUNK GUITAR ULTIMATE 4.0, пока не пройдены все релизные критерии. Не выдавать документ, каталог пресетов или успешный CI за доказательство полноценной работы всех функций.


## 15. Коммерческая планка звучания и критерии сравнения

**Цель качества:** коммерческий, плотный, дорогой и музыкально убедительный гитарный звук, пригодный для готового микса поп-панк/альтернативного рока. Это целевой критерий, а не заявление о текущем превосходстве над NI Session Guitarist — Electric Sunburst Deluxe.

### 15.1 Референсный функциональный уровень

В официальной документации Electric Sunburst Deluxe описаны 237 паттернов, отдельный инструмент свободно играемых мелодий, open/muted/flageolet/tremolo артикуляции, записанные slides, fret noises, автоматические hammer-on/pull-off, вибрато, delay/reverb и stompbox-эффекты. Наш продукт должен стремиться к сопоставимой глубине и дополнить её поп-панк-ориентированными стилями и рабочими профилями звучания. Функциональный ориентир не означает копирование закрытого кода, сэмплов, пресетов или дизайна NI.

### 15.2 Обязательные звуковые свойства

- Удар медиатора должен быть читаемым, но не колким; palm mute — плотным и контролируемым, без картонного щелчка.
- Открытые power chords должны иметь широкий, стабильный корпус и различимые струны, не превращаться в грязную стену низких частот.
- Clean — объёмный и детальный; Crunch — упругий; Drive — насыщенный, но не пережатый; Acoustic — с естественной атакой и корпусом.
- Куплет/припев должен меняться не только громкостью или EQ, но и реальной артикуляцией/исполнением.
- Пресеты должны быть сведены по воспринимаемой громкости, иметь безопасный headroom и не клипповать на типичных аккордах.
- Стерео должно оставаться совместимым с моно; многомикрофонные/DI-сигналы проверять на фазу.

### 15.3 Источник реализма

1. Несколько velocity layers и Round Robin на каждую критичную ноту/артикуляцию.
2. Реальные дубль-тейки и performance recordings для strums, riffs, arpeggios и chord transitions.
3. Разметка струны/лада/voicing, направления удара, тайминга, velocity, акцента, mute, gate и release.
4. Реальные записи slide, hammer-on/pull-off, harmonic и tremolo; если их нет — не показывать соответствующий контроль как готовую артикуляцию.
5. Профессионально записанные DI и микрофонные источники, управляемые pickup/amp/cab/IR и FX цепочки.

### 15.4 Проверка уровня относительно коммерческого референса

Сравнивать один и тот же набор MIDI-фраз и аккордов на совпадающем темпе, с выровненной воспринимаемой громкостью, без нечестного выигрыша за счёт уровня. Проверять одиночные ноты, быстрые восьмые, palm-muted куплет, открытый припев, арпеджио, переходы аккордов и длительные ноты. Слепое прослушивание и CPU/RAM замеры обязательны. До успешного теста нельзя рекламировать PPGU как звучащий лучше или полностью заменяющий Electric Sunburst Deluxe.

### 15.5 Разделение звукового характера и исполнения в Pattern

Tone selector: Clean, Crunch, Drive, Palm Mute, Open Chords, Acoustic, Lead Drive, Muted Chugs. Performance selector: downstroke eighths, alternate down/up, fast skate-punk, palm-muted verse → open chorus, ska upstrokes, syncopated stop-start, octave-hook groove, arpeggio picking, acoustic strum и dynamic build. Это разные оси; выбор тона не должен подменять выбор ритмического рисунка.

### 15.6 Расширенный список style DNA

Blink-182, Sum 41, Green Day, Fall Out Boy, Good Charlotte, Simple Plan, New Found Glory, Avril Lavigne, Yellowcard, The All-American Rejects, My Chemical Romance, Less Than Jake, Fenix TX, Midtown, Bowling for Soup, MGK, Mest, Angels & Airwaves, The Offspring, Sugarcult, All Time Low. MGK / “Bloody Valentine” — особый современный pop-punk reference: яркая насыщенная ритм-гитара, приглушённая verse-пульсация и раскрывающийся открытый припев. Это стилистическое описание, не транскрипция или копирование записи.


## 16. RECOVERED USER-APPROVED UI AND BROWSER WORKFLOWS (2026-11-14)

This section is a dated addendum and supersedes any older conflicting UI wording in this document. The first approved mockup is the canonical complete appearance/layout/control-placement reference. `02 INTERFACE NEON STAGE AUTOCHORDS` is palette-only and must be applied to the first mockup; it is not a layout reference. SIGNAL FORGE is the product identity and NEON STAGE is the approved default treatment. The approved interface is immutable unless the user explicitly authorizes a dated amendment.

### 16.1 Preset Browser
Provide BROWSER, FAVORITES and CATEGORIES views; factory/user separation; eight categories (Clean, Crunch, Drive, Lead, Ambient, Lo-Fi, Acoustic Textures, Punk Rhythm); artist/style/use-case tags; counts; search, filters and sorting. Search/filter/sort only change visible results and never load a preset. Preset entries expose verified name/category/tags/description/artwork/instrument profile. Previous/next navigate deterministically; favourite state persists. Preview auditions real available material without committing. Load or double-click commits a complete valid preset state. Factory presets are read-only. User presets support Save, Save As, Rename, Delete; banks support Import/Export and Rescan. User content persists across updates. Validate sample/profile references, FX chain, articulations, parameter ranges, APVTS migration and state restore. Report missing assets honestly; no synthetic or unrelated substitution. 300 target factory entries are definitions until playable validation proves otherwise (150 MELODY / 150 PATTERN).

### 16.2 Pattern Browser — section-first
The user first chooses a Song Section (Intro, Verse, Chorus, Bridge, Outro or neutral/general), then independently selects a performance pattern in Pattern Browser. Section and pattern must never be one combined selector. Browser supports search, categories/filters, favourites and verified metadata (purpose, feel, bars/duration, time signature, tempo range and supported articulations/capabilities). Preview plays the real performance map/recorded one-shot through the shared playback path and is non-committing. Apply explicitly commits the selected section/pattern. Back/Cancel rolls back to the last committed state. Harmony, preset/tone, Playing Style/Articulation, Pattern, Playback Mode and Rhythm Mode remain separate axes. No invented performance recordings or MIDI-generated substitute may be presented as recorded guitar content. Timing must preserve host transport, tempo, rests, accents, note-off/gate, loop/bar boundaries, chord transitions, swing/humanize and half/double time without stuck notes or phase resets.

### 16.3 AutoChords
Layout: top harmonic strip with real chord names/degrees; left Playing Styles/Articulation; right Playback Mode and Rhythm Mode; bottom full-width Pattern Browser. Harmony/key/progression, style/articulation, pattern, playback and rhythm selectors are independent. Keep the main on-screen MIDI keyboard unobstructed. Circle of Fifths (12 major keys plus relative minor), Harmony/Progression Manager, progression templates/reordering, Generation Manager, deterministic seed/constraints, preview and explicit commit are required scope. Audition uses the existing shared sampler/audio path, not a new synth/bank. Controls stay disabled until backend and routing are real.

### 16.4 Main controls and auxiliary windows
Preserve MELODY/PATTERN/PUNK GROUP; nine rotary IDs (ampGain, eqHigh, eqMid, eqLow, velocity, dynamics, swing, humanize, accent); exactly seven bypass switches (Compressor, EQ, Distortion, Chorus, Delay, Reverb, Cabinet); eight FX EDIT pages including Flanger (no eighth bypass). Preserve AMP & TONE, pitch/mod, MIDI keyboard, status/performance strip, preset header and utility controls. FX effects have individual professional styling and characteristic controls; preset selection can load the appropriate style FX chain. Auxiliary panels inherit the active skin, show title/X close and use managed independent windows. No fake controls or fabricated telemetry; every active element must bind to a real action/backend and retain state/automation/reset/focus/error semantics.

### 16.5 Visual source inventory
Prior approved references named in the record: `Cover Interface.jpg`, `9132a8e5-5c89-45ee-8412-bea39bc4d04c.png`, `5675b332-eb7a-4dca-85fb-873a9682a349 (1).png` (duplicate `(2)` noted), and `02 INTERFACE NEON STAGE AUTOCHORDS` (palette only). Additional retrievable Library images for main PPGU, AutoChords, Signal Forge/FX EDIT and Import Guitar are supporting visual examples, not blanket approval of every depicted control. Full historical binary recovery, exact mockup-to-file mapping, deduplication, SHA-256 and repository archival are still OPEN; this document does not claim every attachment has been copied or hashed.

**Status:** APPROVED REQUIREMENTS RECOVERED / DOCUMENTED; source implementation and runtime acceptance remain separately tracked.


## 17. PRECEDENCE ADDENDUM — APPROVED BROWSER / CONTROL CONTRACT (2026-11-14)

This section supplements the earlier product specification and does not erase historical text. Where earlier sections conflict with a newer direct user correction or the binding SIGNAL FORGE v10 rules, the newer direct decision and v10 rules prevail.

- Main UI geometry/layout is taken from the first approved main-interface mockup. `02 INTERFACE NEON STAGE AUTOCHORDS` is palette-only; it is not a layout or appearance reference. Apply that palette to the first mockup. SIGNAL FORGE is product identity/engineering standard; NEON STAGE is the approved visual treatment.
- Main modes are MELODY / PATTERN / PUNK GROUP; older two-mode wording is superseded. Requirement does not imply runtime completion.
- Keep nine existing rotary/APVTS controls and their IDs/semantics unchanged: ampGain, eqHigh, eqMid, eqLow, velocity, dynamics, swing, humanize, accent.
- Keep exactly seven main FX bypass switches: Compressor, EQ, Distortion, Chorus, Delay, Reverb, Cabinet. Flanger is an FX EDIT page, not an eighth main bypass. FX EDIT has eight pages: Compressor, EQ, Distortion, Chorus, Flanger, Delay, Reverb, Cabinet IR.
- Preset Browser and Pattern Browser are separate products/workflows. Preset Browser selects sound/instrument/tone; Pattern Browser selects guitar performance/rhythm. Neither filtering nor preview may silently commit.
- Preset Browser contract: BROWSER/FAVORITES/CATEGORIES; separate factory/user scopes; eight sound categories (Clean, Crunch, Drive, Lead, Ambient, Lo-Fi, Acoustic Textures, Punk Rhythm); metadata/search/tags/filter/sort/counts; deterministic Previous/Next; persistent favourites; real non-committing Preview; explicit transactional Load/double-click; user Save/Save As/Rename/Delete and bank Import/Export/Rescan; factory read-only; truthful missing-asset handling; safe state/voice/streaming/transport transitions. 300 definitions (150 MELODY + 150 PATTERN) are a target, not a verified playable count.
- Pattern Browser workflow: choose Song Section (Intro/Verse/Chorus/Bridge/Outro, neutral where applicable) → choose Pattern → Preview → Apply or Back/Cancel. Section and Pattern are independent. Pattern metadata must reflect actual maps/recordings. Preview is non-committing and uses shared playback; Apply commits; Back rolls back. Timing, bars, rests, accents, gate/note-off, chord changes, transport/loop/seek, tempo, swing and humanize require deterministic behavior and tests. Do not fake guitar performances with generic masks.
- AutoChords is a separate surface: chord/degree strip; Playing Styles/Articulation; independent Playback Mode and Rhythm Mode; full-width Pattern Browser below. Harmony/progression, articulation, pattern, playback and rhythm selectors remain independent. Circle of Fifths, progression tools and deterministic Generation Manager require preview and explicit commit. Keep the main MIDI keyboard visible.
- FX and auxiliary windows must use actual backends. Each FX may have its own professional UI identity, but no fake DSP. Preset style defaults may initialize FX; manual edits persist until explicit preset reload. Auxiliary surfaces include FX EDIT, AUTOCHORDS, IMPORT GUITAR, SOUND HUNTER/AUTO DISCOVER, CURATOR, SETTINGS, PERFORMANCE, SKIN GALLERY, ABOUT and prior Preset Manager/Sample Browser workflows; listing is requirement scope, not implementation evidence.
- Skin implementation must use canonical SKIN manifests/ThemeManager/validator and preserve 21 token names, 27 controls + sync_selector, legacy split, stable IDs and preflight/PENDING validations. No parallel theme system. Skins alter visuals only, never layout/control count/IDs/attachments/audio behavior. Older theme counts are separate snapshots until reconciled.
- All visible controls require real action/parameter binding, documented range/value, tooltip, focus, state/reset/automation as applicable, and truthful disabled/loading/error states. Never change APVTS IDs/semantics or remove engine functions to match a mockup.
- Status labels must distinguish APPROVED/DESIGNED, SOURCE PRESENT, WIRED, TESTED, HOST VERIFIED and RELEASED. Current known gaps and test limits are in the sole engineering journal `Info.txt`.

