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
- **Fallback one-shot pattern playback (approved clarification, 2026-11-12):** если для выбранной стилистики/темпа/артикуляции не найден подходящий готовый guitar loop, Pattern/AutoChords может собрать ритмический рисунок из отдельных записанных гитарных one-shots. Нажатие MIDI-клавиши выбирает аккорд/voicing и тональную высоту исполнения; далее Pattern Engine по MIDI-паттерну запускает последовательность one-shot событий (downstroke/upstroke, palm-muted stroke, dead/muted hit, power-chord hit, single-note accent и т. п.) в заданном ритме, с velocity/accent/gate/humanization и синхронизацией с host tempo.
- One-shot source selection must match instrument profile, articulation and style. Prefer dedicated chord/strum one-shots or coordinated per-string/per-note recordings with authored voicing maps. A single unrelated isolated note must not be pitched arbitrarily and presented as a realistic full chord. If only compatible single-note strokes exist, use a musically valid mapped voicing/arrangement; otherwise report the missing articulation and fall back to a suitable recorded loop/performance, never synthetic guitar.
- MIDI pattern is the performance/rhythm instruction; audio one-shots are the recorded guitar timbres. The selected MIDI key determines the chord/voicing (and corresponding note/register) while the pattern determines when and how the recorded strokes are triggered. This is an alternate source path within the existing Pattern/S6.5 pipeline, not a second asset-import pipeline.
- Preserve separate Melody behavior: each key plays its corresponding pitched guitar note. Pattern/AutoChords behavior: each mapped key triggers a chord-specific rhythmic performance, assembled from a matching recorded loop or compatible recorded one-shots.


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


### 15.7 Guitar → tone preset → performance articulation workflow (approved clarification, 2026-11-12)

- The PATTERN workflow has three distinct user decisions, in this order:
  1. **Guitar / instrument source:** choose the actual instrument profile or recorded guitar (electric, acoustic, and available model/ pickup/recording variant). This determines the compatible sample pool and physical performance capabilities.
  2. **Tone preset:** choose the sound identity and processing setup (for example Pop Punk Lead, Punk Rock Rhythm, Clean, Crunch, Drive, Flanger Lead, Tremolo Lead). Tone presets may configure amp/cab/IR, pickup/EQ and effects, but must not pretend to change the underlying performance articulation.
  3. **Performance / articulation:** choose how the guitar is played. Expose multiple variations per compatible guitar/tone preset, with clear labels and audition/selection. Examples: OPEN STRUM, LEAD / OCTAVE HOOK, PALM MUTE / TIGHT, MUTED BRIDGE, SUSTAIN / AMBIENT, TREMOLO, ARPEGGIO / PICKING, STOP-START / SYNCOPATED, DOWNSTROKE, ALTERNATE DOWN-UP. Exact options depend on recorded content and the selected instrument.
- The user should be able to switch performance variations while playback is running. Switching must be quantized/configurable where appropriate, remain locked to host tempo/transport, preserve musical phase, and avoid stuck notes, abrupt sample tails or timing resets. Provide a safe immediate-switch option only where it can transition cleanly.
- Tone/effect controls and performance articulation controls are separate axes. Flanger, tremolo effect, delay, reverb or distortion are tone/processing choices; tremolo picking, palm muting, open strumming and muted bridge are performance techniques. Do not conflate a modulation effect with a recorded playing technique. Where both exist, expose them independently.
- Multiple articulations/variations may be offered as a curated set for each guitar and tone preset. A primary variation is selected by default; optional compatible layering (e.g. open rhythm plus sparse octave/lead accents) may be supported as an advanced mode, with independent level/mute and clear CPU/polyphony limits. Do not layer incompatible full performances by default.
- Style references: use the musical vocabulary and broad performance characteristics associated with Tom DeLonge's work in Blink-182 and Angels & Airwaves as an inspiration target (driving downstroke power-chord rhythms, octave/lead hooks, palm-muted verse patterns, open chorus lift, sustained/ambient lead textures and suitable modulation). Do not copy recordings, exact riffs, proprietary presets, or claim an artist-authentic recreation. Create original patterns and use only legally cleared genuine guitar recordings.
- Sunburst Deluxe and Session Guitarist Vintage are primary UX/functionality references; Session Guitarist Neon is an additional reference for variation and performance organization. They are references, not dependencies and not sources of samples.
- Every visible articulation must map to an actual recorded loop/one-shot/performance map and pass audibility/runtime tests. If no compatible recording exists, disable or clearly mark the option as unavailable; never fake it with an unrelated sample or synthetic guitar.


### 15.8 Базовые гитарные техники и ритмические рисунки — пользовательское дополнение (2026-11-12)

В PATTERN catalog / articulation library предусмотреть следующие техники и музыкальные рисунки, если для них доступны подходящие реальные записи и корректные performance maps:

**Артикуляции и приёмы**
- Palm mute; open chords / открытые аккорды; downstroke; upstroke; alternate down-up.
- Power chords, включая staccato power-chord hits.
- Chugs / короткие приглушённые ритмические удары.
- Double-stops; octave playing / октавные акценты и хуки.
- Muted-string scratches / приглушённые удары по струнам между аккордами.
- Skank / offbeat upstroke.
- Tremolo picking как техника исполнения (отдельно от tremolo effect).

**Ритмические рисунки**
- Straight eighth-note strumming / ровные восьмые.
- Skank / offbeat pattern.
- Gallop rhythm.
- Stop-time / stop-start accents.
- Half-time feel.
- Чередование приглушённых и открытых аккордов, включая verse-to-chorus lift.
- Октавные хуки и double-stop fills между аккордами.

Термины из голосового сообщения, распознанные неоднозначно («crank», «debit», «stocktime», «how-fo-time»), сохранить как pending terminology; не создавать под ними отдельные функции до уточнения точного музыкального термина. Все названия в интерфейсе должны соответствовать реальному исполнению, а не только MIDI-ритму. Каждый элемент включается в доступные пресету варианты лишь при наличии подходящих записей/one-shots/performance data; отсутствие материала явно отмечается, без синтеза гитары.


- **AutoChords display / keyboard non-obstruction (approved clarification, 2026-11-12):** Display AutoChords must never cover, obscure, intercept, or reduce the usable area of the plugin's playable on-screen piano/MIDI keyboard. The keyboard remains fully visible and operable in every AutoChords display state, window size, panel layout, and theme. Prefer a dedicated non-overlapping region above/beside the keyboard or a separate/resizable auxiliary window; if space is constrained, AutoChords content must reflow, scroll, collapse, or resize without overlaying the keys. Pointer hit-testing and MIDI-key interaction must remain unambiguous. Verify minimum editor size, resize extremes, tiled/expanded auxiliary windows, DPI scaling, keyboard boundaries, and open/close transitions. This is a UI layout and interaction requirement; it does not change AutoChords MIDI/chord-generation semantics.


### 15.9 AutoChords — утверждённая компоновка раздела (2026-11-12)

Пользователь уточнил расположение основных элементов AutoChords:
- **Левая колонка — PRESETS:** вертикальный список/браузер пресетов AutoChords. Здесь выбираются сохранённые chord/progression presets и связанные музыкальные заготовки; поиск и категории размещать в верхней части колонки, если они предусмотрены.
- **Правая колонка — RHYTHM PATTERNS / MODES:** выбор режима исполнения и ритмического рисунка. Показывать варианты наглядными карточками или компактным списком с названием, кратким описанием feel/акцентов и доступностью для выбранного инструмента.
- Выбор пресета слева и рисунка справа — независимые решения. Смена ритмического рисунка не должна самопроизвольно менять выбранную гармоническую прогрессию; смена пресета не должна сбрасывать рисунок без явного правила/индикации.
- Над двумя колонками — заголовок AUTOCHORDS и понятный статус выбранного инструмента/тона; внизу — компактная область текущей последовательности/аккордов и транспортных действий, если это предусмотрено текущим макетом.
- Не перекрывать экранную MIDI-клавиатуру. Раздел AutoChords занимает отдельную область выше/рядом с клавиатурой либо открывается отдельным изменяемым окном. При уменьшении окна колонки адаптивно сжимаются/перестраиваются/прокручиваются, но клавиши остаются полностью видимыми и кликабельными.
- Состояния выбранного пресета и ритмического рисунка должны быть визуально различимы; предусмотреть audition/preview, если соответствующие данные доступны. Недоступные для выбранной гитары варианты явно помечать, не имитировать отсутствующие исполнения.
- Это утверждённое требование к визуальной структуре и UX; оно не означает, что соответствующий GUI уже реализован. Проверить реализацию в JUCE, hit-testing, resize, DPI и сохранение выбора.


### 15.10 AutoChords — группировка стилей и техник исполнения (2026-11-12)

Уточнение пользователя: в правой части AutoChords должны отображаться не только названия ритмических рисунков, но и стили/способы гитарного исполнения — например **OPEN CHORDS**, **PALM MUTE** и другие.

Правило организации правой колонки:
- Верхний уровень — категории **PLAYING STYLE / ARTICULATION** (как исполняется гитара) и **RHYTHM PATTERN** (какой ритмический рисунок исполняется). Их можно показывать вкладками или двумя последовательно расположенными группами, но нельзя смешивать смысл.
- Примеры стилей/техник: OPEN CHORDS, PALM MUTE, DOWNSTROKE, UPSTROKE, ALTERNATE DOWN-UP, POWER CHORDS, CHUGS, MUTED BRIDGE, SKANK / OFFBEAT UPSTROKE, OCTAVE HOOKS, DOUBLE-STOPS, TREMOLO PICKING, ARPEGGIO / PICKING, SUSTAIN / AMBIENT, STOP-START / SYNCOPATED.
- Примеры ритмических рисунков: STRAIGHT 8THS, FAST SKATE-PUNK, GALLOP, HALF-TIME, STOP-TIME ACCENTS, VERSE → OPEN CHORUS, SKANK OFFBEATS, OCTAVE-HOOK GROOVE, ARPEGGIO PATTERN.
- Доступные варианты зависят от выбранной гитары и реально имеющихся записей/performance maps. Не путать **TREMOLO PICKING** (техника игры) с **TREMOLO EFFECT** (эффект обработки), а **PALM MUTE** (артикуляция) — с тембровым пресетом. Тон/эффекты, техника исполнения и ритмический рисунок — независимые параметры.
- Выбор техники/стиля может ограничивать список совместимых ритмов; интерфейс должен ясно показывать совместимость и не подменять отсутствующие исполнения синтетической имитацией.


### 15.11 Справочник панк-ритм-гитары — обязательная база AutoChords / Pattern Engine (2026-11-12)

Пользователь предоставил полный справочник по ритм-гитаре панк-рока. Использовать его как исходную музыкальную таксономию для проектирования AutoChords, Pattern Browser, MIDI pattern data, performance maps и factory pattern presets. Это не означает, что паттерны уже реализованы или что каждый из них доступен для каждой библиотеки.

**Принципы исполнения:** плотная и ровная пульсация; короткие повторяемые 1–2-тактовые фразы; точность и синхронизация с kick/snare важнее предельной скорости; динамика строится контрастом palm-muted/open, паузами, акцентами и длительностью нот. Паузы должны сохранять метрическую позицию; ghost notes тише основных атак.

**Техники / articulations (отдельная ось от ритма и тембра):**
- DOWNSTROKE; UPSTROKE; ALTERNATE DOWN-UP;
- PALM MUTE с уровнями/вариациями приглушения только если это поддерживается исходными samples/articulation maps;
- OPEN CHORDS; POWER CHORDS; STACCATO / LEFT-HAND MUTE;
- GHOST NOTES; MUTED BRIDGE / CHUGS;
- SKANK / OFFBEAT UPSTROKE;
- OCTAVE / FIFTH MOVES; DOUBLE-STOPS; TREMOLO PICKING;
- ARPEGGIO / PICKING; SUSTAIN / AMBIENT; STOP-START / SYNCOPATED.
Не смешивать palm-mute articulation с тембровым пресетом/эффектом, tremolo picking с tremolo FX, а mute/staccato с обычной короткой MIDI-длительностью без соответствующей артикуляции.

**Базовые ритмические паттерны (4/4; восьмые позиции 1-&-2-&-3-&-4-&):**
- A — STRAIGHT MUTED WALL: восемь равномерных приглушённых атак.
- B — POP-PUNK VERSE: приглушённые восьмые с запланированными rests; исходная таблица пользователя: P P . P P P . P.
- C — BRITISH PUNK / STOP-START: open/muted contrast + rests; исходная таблица: V . P P . P V .
- D — BACKBEAT / GHOST ACCENTS: тихие ghost attacks и выделенные акценты на 2 и 4; исходная таблица: g g V g g V g g. При реализации проверить, что акценты действительно попадают на выбранные beat positions и соответствуют сетке.
- E — HARDCORE SIXTEENTHS: шестнадцать равномерных атак на сетке 1-e-&-a; continuous downstroke/mute только если артикуляция доступна.
- F — SKA-PUNK OFFBEATS: rests на долях и атаки на восьмых offbeats (&).
- G — SIXTEENTH-PAIR DRIVE: пары шестнадцатых с последующей восьмой; исходную запись пользователя нормализовать в точные MIDI onset/duration, не трактовать двусмысленную текстовую группировку как готовую MIDI-таблицу.

**Подстили / groove families:** CLASSIC PUNK (straight eighth-note wall); BRITISH PUNK (stop-start, staccato, open/muted contrast); POP-PUNK (muted verse → open chorus, accents); HARDCORE / SKATE-PUNK (fast sixteenths, high density); SKA-PUNK (offbeat upstroke/skank); POST-PUNK (dry/mechanical eighths, bass-led minimalism). BPM ranges из пользовательского справочника — ориентиры, не жёсткие ограничения и не замена tempo sync.

**Arrangement / section patterns:** VERSE MUTED → CHORUS OPEN; STOP-TIME ACCENTS; HALF-TIME; GALLOP; STRAIGHT 8THS; FAST SKATE-PUNK; OFFBEAT SKANK; OCTAVE-HOOK GROOVE; ARPEGGIO PATTERN. Связь с барабанами: kick ориентировочно 1/3, snare 2/4, но pattern data должен позволять вариации и не жёстко привязываться к одному drum groove.

**Data / UI требования:** каждый паттерн хранить как проверяемую сетку событий (bar, beat/subdivision, duration, velocity/accent, articulation, rest, humanization bounds), иметь счёт/визуализацию, BPM feel tag, substyle tag и совместимость с конкретными guitar sample maps. В AutoChords отдельно выбирать progression, playing style/articulation и rhythm pattern. Добавлять preview/play только после подключения настоящего MIDI/audio routing. Нельзя выдавать текстовую карточку за реализованное исполнение.

**QA:** проверять длину цикла, отсутствие событий в rests, точность сетки, корректность 1/8 и 1/16, loop seam, MIDI note-off/choke, velocity/accent, tempo scaling, swing/humanize bounds и синхронизацию с kick/snare. Проверять, что DOWNSTROKE/UPSTROKE, palm mute, ghost note и open chord поддерживаются реальными performance data. Темп и техника должны быть редактируемыми, не запечёнными в название паттерна.


## 15.12. SOUND QUALITY GATE — ORGANIC GUITARS ONLY (USER-MANDATED)

**Цель — максимально высокий, профессиональный и музыкально убедительный гитарный звук.** Пользователь прямо запретил «пластмассовые» синтезаторные гитарные тембры. Основой инструмента должны быть реальные записи настоящих электрических и акустических гитар, а не oscillator/wavetable/FM/physical-model имитации гитары и не синтетические guitar one-shots, выдаваемые за живые инструменты.

- Приоритет источников: качественные DI-записи реальных электрогитар; реальные усилители/cab/mic или качественные импульсы кабинетов; реальные акустические steel/nylon-string записи; полноценные multisamples и performance phrases/one-shots, записанные музыкантами.
- Для pop-punk искать настоящие downstroke power-chords, palm-muted eighths/sixteenths, open chords, octave hooks, chugs, single-note riffs, harmonics и stop-start performances. Не подменять отсутствующие articulations синтетической имитацией.
- Предпочитать velocity layers, round robins, вариативные атаки/релизы, ноты по диапазону, естественные sustain/decay и корректные note-off. Проверять noise floor, clipping, phase, tuning, timing, transient quality, consistency и пригодность к amp/cab processing.
- Синтезаторные/искусственно сгенерированные звуки допустимы только как FX/design layer или тестовые сигналы, но не как основной гитарный источник. Явно искусственный/«пластмассовый» характер не проходит GOLD.
- GOLD требует прослушивания на нескольких нотах и velocity, в solo и в миксе, с clean/crunch/high-gain цепями; проверять циклическую машинность, одинаковые атаки и заметные швы.
- Коммерческие банки с неясной лицензией можно исследовать и заносить в отдельную очередь потенциальной покупки/лицензирования. Отсутствие запрета не является разрешением. До письменного подтверждения прав на включение и распространение исходных аудиофайлов внутри коммерческого sample-based VST3 такие материалы остаются YELLOW/QUARANTINE.
- Качество, натуральность, динамика, артикуляции и миксовая пригодность важнее бесплатности и размера библиотеки.

**Acceptance gate:** ни один новый основной гитарный asset не получает GOLD/production status без подтверждения реального гитарного источника, точной лицензии, provenance, SHA-256, технической проверки и прослушивания. Не выдавать синтезаторную имитацию за записанную гитару.


## 15.14. SOUND FORGE — SIMPLE-FIRST IMPLEMENTATION PLAN (2026-11-12)

### Принцип
Не строить большую платформу и не вводить второй формат/движок. Реализовать короткий надёжный путь поверх существующих JUCE audio formats, SFZ parser, SampleMetadataIndex, streaming sampler и SoundBank/Discovery contracts. Сначала полезный offline CLI-инструмент; GUI — тонкая оболочка над теми же операциями позже. Не создавать отдельный сервис, базу данных, daemon, plugin-in-plugin или обязательную AI-инфраструктуру.

### Единый простой pipeline
1. ADD — пользователь выбирает папку/архив; исходники не меняются.
2. SCAN — рекурсивно читаются поддерживаемые форматы и метаданные; без полного декодирования в RAM.
3. CHECK — проверка читаемости, длительности, частоты, каналов, пикового уровня, clipping/DC, тишины и SHA-256; fingerprint/dedup запускается по запросу или при импорте.
4. PREVIEW — один временный preview stream с ограниченным буфером.
5. PREPARE — только выбранные операции: trim silence, gain/peak management, fade/click repair, resample, channel conversion, loop analysis. Никакая обработка не включена автоматически, если она может изменить тембр.
6. BUILD — сформировать/обновить SFZ map или существующую внутреннюю карту, используя уже поддерживаемые поля. Не создавать новый playback format без доказанной необходимости.
7. VERIFY — повторное чтение результата, checksum, metadata, A/B preview и smoke playback через существующий sampler.
8. PUBLISH — атомарно поместить готовый пакет в SoundBank; неуспешные/неясные файлы остаются в quarantine.

### Минимальный первый релиз
- Один исполняемый offline tool ppgu-soundforge (или существующая утилита проекта, если она уже есть) и команды scan, prepare, build, verify.
- Один manifest на пакет: source path, original hash, output hash, format, sample rate, channels, frames, transformations, warnings, tool version.
- Один профиль подготовки PPGU_PLAYBACK_DEFAULT; остальные профили добавлять только при реальном запросе.
- WAV и SFZ — первый поддерживаемый маршрут. SF2/AIFF/FLAC/OGG подключать по одному только через существующие JUCE codecs и тесты; неизвестные форматы — безопасно отклонять/помечать manual import.
- Сохранить исходники и результаты раздельно; повторный запуск идемпотентен и не создаёт дубликаты.

### Что НЕ делать в первой версии
- Не делать AI denoise, source separation, neural upscaling, автоматический amp modelling или тяжёлый spectral repair.
- Не делать массовый auto-mastering и loudness normalization всех сэмплов.
- Не выполнять network search/download внутри плагина.
- Не добавлять постоянные фоновые потоки, GPU requirement, database server или сложную многопоточную очередь.
- Не переписывать sampler и не менять его кэш/voice limits ради SOUND FORGE.

### Ресурсная политика
- Offline worker по умолчанию один; обработка чанками; ограниченное число одновременно открытых файлов; preview максимум один.
- VST3 во время playback не выполняет scan/hash/convert/resample/fingerprint и не ждёт файловых операций.
- Low Power mode: один worker, меньшая длина чанка/паузы между заданиями, пауза по команде; никакого непрерывного polling.
- Все кэши и временные файлы ограничены; preflight свободного места; cancel удаляет незавершённые временные файлы, но не оригиналы.
- Для каждого job измерять wall time, peak RAM, CPU time, threads, bytes read/write и размер результата. Сравнивать с базовой сборкой на одном устройстве.

### Порядок работ — короткие вертикальные этапы
A. Baseline: измерить текущий VST idle, 1/16/32/64 voices, bank load, cache pressure; записать точные результаты и настройки.
B. Scan + manifest: добавить безопасный сканер WAV/SFZ и воспроизводимый manifest; тесты malformed files, huge files, symlinks, cancellation.
C. Verify + dedup: SHA-256, metadata checks, exact duplicate detection; perceptual fingerprint оставить отдельной опцией.
D. Prepare: реализовать 2–3 безопасные операции, каждая с A/B, сохранением оригинала и unit tests; не добавлять «улучшение» без измеримой пользы.
E. Build: создавать корректную карту инструмента через текущий SFZ/sampler path; проверять note/velocity coverage и missing samples.
F. Package: атомарная публикация и отчёт; затем простая GUI-оболочка, если CLI workflow подтверждён.

### Definition of Done
- Полный pipeline проходит на тестовом наборе и реальных тестовых библиотеках.
- Оригиналы не меняются; повторный запуск не портит и не дублирует результаты.
- Peak RAM offline ограничен и измерен; VST audio callback не делает I/O и не блокируется.
- VST idle CPU не получает дополнительной постоянной нагрузки; realtime CPU/RAM не ухудшаются сверх заранее установленного измеримого допуска.
- В отчёте есть фактические benchmark-данные, а не обещания.
- Build, unit tests, CTest и pluginval проходят; FL Studio 20 проверяется отдельно на реальной Windows-системе.

Статус: SPECIFICATION ONLY до реализации и тестов. План минимизирует число компонентов, повторно использует существующий код и ставит простую работающую вертикальную версию выше большой, но незавершённой архитектуры.

## 15.15. ЕДИНЫЙ SOUND BANK ДЛЯ RHYTHM MODE И MELODY MODE

**Пользовательское требование:** все подготовленные SOUND FORGE гитарные инструменты должны быть доступны внутри PPGU как в RHYTHM MODE, так и в MELODY MODE. Один и тот же качественный источник/инструмент используется в обоих режимах без создания двух копий библиотеки.

### Общая архитектура
- SOUND FORGE создаёт единый Guitar Instrument Package: audio assets + sample map/SFZ + velocity/round-robin metadata + articulation tags + note range + tuning + provenance/quality manifest.
- RHYTHM MODE и MELODY MODE — два способа управления одним инструментом и одной картой сэмплов. Они не должны иметь независимые дубли SoundBank.
- Общие ресурсы загружаются через существующий SamplePool/streaming sampler. Переключение режима меняет правила исполнения/MIDI mapping, а не повторно загружает все аудиофайлы.
- Для каждого инструмента хранится capability map: какие ноты, velocity layers, round robins и артикуляции реально присутствуют. Режим не должен предлагать неподдерживаемую технику.

### RHYTHM MODE
- Исполнение аккордов, power chords, downstroke/upstroke, palm mute, chugs, octave hooks, skank/offbeat, stop-start и готовых ритмических рисунков.
- Rhythm engine генерирует MIDI/voice events по сетке темпа и выбранной артикуляции; сэмплы остаются теми же, что доступны в Melody Mode.
- Для аккордов использовать доступные ноты/voicings и articulation samples; при отсутствии настоящего strum performance допускается аккуратное распределение атак по времени только как fallback, явно не выдавая его за записанный strum.
- Ритм-паттерн, стиль исполнения и тембр/пресет — независимые параметры.

### MELODY MODE
- Игра отдельными нотами с MIDI-клавиатуры/пиано-ролла, мелодических линий, риффов, лид-фраз, арпеджио и соло.
- Поддерживать note range, velocity layers, round robin, sustain/release, note-off и доступные артикуляции инструмента.
- При наличии соответствующих записей разрешать phrase/lick trigger и one-shot performance; не растягивать фразы по клавиатуре как обычные ноты без явной настройки.
- Ноты и артикуляции должны звучать естественно в пределах реальной карты; не создавать отсутствующие слои искусственно.

### Переключение и совместимость
- Переключение RHYTHM ↔ MELODY не меняет SoundBank asset IDs, не дублирует сэмплы в памяти и не сбрасывает выбранный инструмент.
- Параметры режима сохраняются отдельно: rhythm pattern/strum settings отдельно от melodic articulation/legato settings; общий preset может хранить оба набора настроек.
- MIDI input, host automation, tempo sync и note-off/choke должны работать предсказуемо в обоих режимах.
- Одновременно активные голоса учитываются общим voice budget; режимы не получают по отдельному скрытому лимиту голосов.
- UI должен явно показывать, какие артикуляции/паттерны поддерживает выбранный инструмент, и скрывать либо отключать недоступные функции.

### Проверки качества
- Один и тот же инструмент тестируется в обоих режимах: одиночные ноты, аккорды, velocity transitions, round robin, note-off, быстрые ритмы, смена режима во время/между фразами.
- Проверять отсутствие двойной загрузки ресурсов, лишних аллокаций в audio callback, clicks при переключении и превышения общего voice budget.
- Сравнивать тембр в solo и в миксе; ритм-режим не должен звучать как механическое повторение одного сэмпла, melody mode — как транспонированный неподходящий аккорд.

**Definition of Done:** один подготовленный пакет импортируется один раз и корректно используется в обоих режимах; отображается честная карта возможностей; playback и MIDI проходят тесты; память не удваивается из-за наличия двух режимов.

Статус: SPECIFICATION APPROVED; реализация и проверка в VST3 ещё не подтверждены.

## 15.16. SOUND FORGE FIRST IMPLEMENTATION SLICE — 2026-11-12

Added `tools/soundforge/ppgu_soundforge.py`: dependency-free offline CLI with `scan` and `package` commands. It reads common SFZ `<global>`, `<master>`, `<group>`, `<region>` inheritance; checks referenced mono/stereo WAV headers, path traversal, missing assets, sample rate and frame count; computes SHA-256 in 1 MiB chunks; reports note range, velocity ranges/layer count, round-robin length and conservative articulation-name hints; writes `instrument-package.json`; packages referenced SFZ/WAV into a staging directory and publishes separately. It does not modify source assets or run inside VST3.

Added `tests/TestSoundForge.py`, registered as `ppgu_soundforge_contract` in CTest, and `tools/soundforge/README.md`.

Runtime integration finding: `PopPunkGuitarUltimateAudioProcessor` already owns one `SamplerEngine sampler`. Melody mode sends direct MIDI notes to it; Pattern/Rhythm mode sends scheduled MIDI events to the same sampler. Thus one loaded SFZ bank is already shared between both modes; only event generation differs. The visible mode label is now RHYTHM while the internal parameter ID/index and preset category remain backward-compatible.

New source discovery: Boyss-Sound-e-Scapes Guitar DI Lite (96 WAV, 24-bit/44.1 kHz, 2 round robins, 3 velocity layers, dry DI) is a strong candidate for Melody mode and amp/cab processing. Official developer page: https://www.boyss-sound-e-scapes.com/7th-year-free-pack . Current environment could not retrieve binary downloads (DNS/network unavailable); asset is NOT recorded as downloaded or integrated. Existing CI workflow already clones FreePats and Karoryfer CC0 sources and downloads Meta Open Sound releases.

Status: CLI implementation committed; automated CI verification pending. Full denoise/enhancement, audio editing, GUI and automatic velocity/RR authoring remain future work. The first slice validates and packages existing maps; it does not invent unsupported articulations or generate new recorded performances.

---

# Addendum 2026-11-14 — TX-UI-PRESET-GUARD-004: утверждённый browser и PATTERN workflow

**Приоритет:** это более позднее прямое пользовательское решение. При конфликте с более ранними разделами данного документа применяется этот Addendum; совместимые требования сохраняются. Этот Addendum не отменяет MASTER PROMPT REV 13.0 FINAL и не разрешает менять строгие realtime/audio invariants.

## A. Каноническая навигация preset browser

Основной порядок выбора:

**GENRE → ARTIST/BAND → SONG/SECTION → STYLE → PATTERN**

- GENRE — жанровая категория.
- ARTIST/BAND — artist-inspired стилевой ориентир, не источник чужих записей или коммерческих пресетов.
- SONG/SECTION — музыкальный контекст и раздел композиции.
- SECTION — Intro, Verse, Pre-Chorus, Chorus, Bridge, Outro.
- STYLE — тип/манера исполнения.
- PATTERN — конкретный ритмический рисунок или слот.

Уровни не объединять в один список и не менять порядок без прямого утверждения пользователя.

## B. Browser control inventory

Обязательные реальные операции/контролы:
- BROWSER;
- FAVORITES;
- CATEGORIES;
- SEARCH;
- FILTER;
- SORT;
- PREV / NEXT;
- FAVORITE;
- PRESET NAME;
- CATEGORY;
- PREVIEW;
- SELECT.

PREV/NEXT перемещаются внутри текущего результата поиска/фильтра/сортировки и сохраняют контекст. Избранное, категория, поиск и сортировка должны быть согласованы, а не сбрасывать друг друга. Все визуально активные controls должны иметь рабочий callback и понятное состояние.

Factory/User presets, Pattern entries и banks — разные типы сущностей с собственными метаданными и разрешёнными операциями. Не выдавать preset definition за playable preset без доступного ресурса и runtime playback.

## C. Preview / Apply / Cancel

- PREVIEW — временное audition; не меняет active preset, project state, persistent state или DAW project.
- APPLY — подтверждает и применяет выбранный элемент.
- CANCEL — отменяет транзакцию и возвращает состояние до начала выбора.
- Preview/переключение/закрытие не должны оставлять частичные изменения или зависшие временные голоса.
- Та же транзакционная модель применяется к Theme Gallery: Preview временный, Apply постоянный, Cancel откатывает.
  
## D. PATTERN sections and slots

Каждая секция (Intro, Verse, Pre-Chorus, Chorus, Bridge, Outro) содержит 8 Pattern Slots. После выбора секции пользователь выбирает STYLE, затем конкретный PATTERN/slot. Preview неразрушающий, Apply фиксирует, Cancel возвращает предыдущий рисунок. Сохраняется legacy compatibility field `patternSlot`; любые миграции требуют явного mapping и regression tests.

Переключение рисунка во время воспроизведения должно быть host-synchronized и не создавать stuck notes, duplicate triggers, timing jumps или abrupt cuts. Уже утверждённое разделение сохраняется: guitar/instrument source, tone preset/FX и performance articulation/pattern — независимые параметры.

PATTERN/AUTOCHORDS: chord key выбирает chord/voicing; MIDI pattern задаёт timing, gate, velocity, accent и ритм; совместимые записанные guitar one-shots могут использоваться как fallback только в существующем Pattern/S6.5 pipeline и с корректной картой. MELODY остаётся отдельным режимом: каждая клавиша запускает соответствующую ноту. Отсутствующую реальную артикуляцию нельзя заменять синтетической гитарой и выдавать за запись.

Display AutoChords не должен перекрывать экранные MIDI-клавиши, перехватывать их hit-testing или уменьшать доступную область клавиатуры. При нехватке места использовать перестройку, scroll/collapse или отдельное окно.

## E. Factory catalog, user content and file formats

- Factory catalog расширяется только добавлением. Удаление, замена существующих записей и silent overwrite запрещены.
- Stable IDs и опубликованные данные сохраняются. Конфликты ID при импорте показываются явно; разрешение требует явного выбора/правила, не скрытой перезаписи.
- Factory presets read-only; user presets/banks хранятся отдельно и переживают обновления.
- Последние утверждённые форматы: `.ppgm` = MELODY preset; `.ppgp` = PATTERN preset; `.ppgbank` = bank.
- Import/export проверяет schema version, IDs, bank/sample dependencies, ресурсы и целостность. Пресет ссылается на BANK/sample IDs, не дублирует WAV и не полагается на абсолютные локальные пути.
- Восемь QA profiles — покрытие тестов, а не автоматическое требование восьми дополнительных factory presets.

## F. UI/skin immutable guard

- SIGNAL FORGE / NEON — основная визуальная система; default theme — **NEON STAGE**.
- Первый reference-файл задаёт внешний вид, layout и placement; второй — палитру.
- TX-UI-PRESET-GUARD-004 требует exact approved layout parity: skin меняет только графику/палитру в разрешённых границах, не координаты/геометрию контролов, hit areas, bindings, функции, параметры или звук.
- Все обязательные controls остаются представленными. Не удалять engine functions; не менять APVTS IDs/semantics, DSP, MIDI routing или sampler behavior.
- Никаких декоративных/фиктивных активных controls. Нереализованное обозначать disabled/pending до реальной интеграции.
- Более раннее разрешение на произвольную перестановку controls между skins не применяется там, где противоречит этому более свежему guard.

## G. Acceptance matrix

Обязательны отдельные проверки:
1. Browser hierarchy and navigation.
2. Search/filter/sort/favorites and stable context for PREV/NEXT.
3. Preview non-destructiveness; Apply commit; Cancel rollback.
4. Factory catalog immutability and extension-only behavior.
5. Explicit import conflict handling and stable-ID preservation.
6. Pattern section → style → pattern/slot selection; all eight slots per section.
7. Host sync and glitch/stuck-note-free pattern transitions.
8. AutoChords display never overlaps or blocks MIDI keyboard.
9. GUI-to-function parity for every active control.
10. Skin layout/control parity and audio invariance.

**Статус:** APPROVED REQUIREMENTS / IMPLEMENTATION NOT IMPLIED. Source-to-control audit, exact-SHA Windows Release, full CTest, pluginval strictness 5, packaging and FL Studio 20 host acceptance remain separate evidence gates.




---

# Addendum 2026-11-14 — TX-UI-SURFACE-005: подтверждённые пользовательские требования к окнам и браузерам

**Статус:** USER-REQUESTED UI CONTRACT / DESIGN PRESERVATION. Требования фиксируют ожидаемое отображение и поведение, но не доказывают наличие реализации. Addendum подчиняется MASTER PROMPT REV 17.0 FINAL, R01–R34 и I1–I8. Он не меняет APVTS IDs, control inventory, DSP, MIDI routing, sampler, SoundBank policy или realtime invariants.

## 1. Общий закон интерфейса

- Единственный визуальный язык всех окон — SIGNAL FORGE / NEON. Каждое дополнительное окно должно выглядеть частью одного продукта, но сохранять утверждённую компоновку именно своего макета.
- Макеты пользователя являются визуальным эталоном: не редизайнить, не упрощать, не менять порядок и смысл элементов без согласования.
- Все активные кнопки, переключатели, поля, карточки, папки, списки и элементы навигации обязаны выполнять заявленную функцию. У кнопок открытия окон должны быть корректные open/close, повторное открытие без дублей и X close. Toggle-контролы должны иметь ясные ON/OFF состояния и реально включать/выключать функцию.
- Не подключённая функция отображается disabled / BACKEND PENDING с понятной причиной. Нельзя оставлять визуально активную фиктивную кнопку.
- UI-only этап не меняет звук, APVTS, MIDI, preset semantics, sampler или audio callback. Для аудио-функций — отдельный review и тесты по R10.

## 2. SETTINGS — Resource Profile

В Settings требуется отдельный хорошо оформленный селектор профиля потребления ресурсов с четырьмя точными названиями:
- GOOD
- NORMAL
- NIGHT
- ECO

Отображать четыре самостоятельные карточки/сегмента с выбранным состоянием, кратким назначением и реальным статусом. Предварительный UX-смысл: GOOD — максимальное качество/полнота необязательных визуальных и фоновых возможностей; NORMAL — сбалансированный режим; NIGHT — сниженная фоновая активность и спокойное визуальное поведение; ECO — минимизация необязательной фоновой и декоративной нагрузки. Это смысловые ориентиры, а не утверждённые численные лимиты.

До появления измерений и технической матрицы запрещено приписывать режимам конкретные проценты CPU/RAM, частоты сканирования, размеры кэша, число голосов, качество ресэмплинга или изменения звука. Профили не могут нарушать realtime safety, менять обязательное звучание/артикуляции или незаметно переписывать пользовательские параметры. Для каждого режима позже зафиксировать точный перечень разрешённых resource/UI policies, измеримые бюджеты, persistence и тесты переключения.

## 3. CURATOR — управление пользовательскими библиотеками без запрета локального использования

**Непереговорное пользовательское требование:** CURATOR — центр управления библиотеками; он НЕ запрещает и НЕ ограничивает загрузку или локальное использование пользователем любых библиотек, которые пользователь сам добавляет в экземпляр плагина.

Назначение CURATOR: единый обзор и организация подключённых библиотек — список, поиск/фильтры, метаданные, источник, формат, доступность, sample/note coverage, артикуляции, дубликаты, checksum/provenance, preview/audition, статус индексации, ошибки и восстановление отсутствующих файлов. Операции организации и статусы должны быть прозрачны и обратимы.

Лицензионная классификация GREEN/YELLOW/RED/GOLD относится к решению о включении/автоматическом добавлении в заводской дистрибутив PPGU, а НЕ к праву пользователя локально загрузить и использовать собственный файл. RED/YELLOW не должны блокировать локальный импорт или воспроизведение в пользовательском экземпляре. Curator не имеет права молча удалять, отключать, карантинить или подменять библиотеку пользователя. Предупреждение о лицензии — информационное; запрет на распространение в factory package остаётся отдельным gate.

Технические ограничения диска, ОС, адресного пространства, памяти, CPU, файловой системы и существующих engine limits не маскировать под лицензионный запрет. Известные текущие пределы sampler (8 streaming roots / 128 snapshots на экземпляр) остаются инженерным ограничением и требуют отдельного аудио-ревью/планирования масштабирования; UI не должен ложно обещать неограниченное одновременное активное стриминг-использование. Это не даёт Curator права отказывать пользователю в регистрации/локальном использовании библиотеки по её лицензии.

## 4. AUTO DISCOVER

- Переключатель ENABLE AUTO DISCOVER ON/OFF находится ВНУТРИ окна Auto Discover.
- OFF — discovery выключен; никаких периодических/постоянных поисков, фонового сетевого трафика или автоматических повторных сканирований.
- ON — функция разрешена, но сам по себе ON не запускает бесконечный цикл. Отдельная команда SCAN / SEARCH запускает одну ограниченную задачу; пользователь может остановить её. Повторный запуск — только по явной команде или отдельно согласованному расписанию.
- Сканирование и анализ — только вне audio callback; статус и прогресс отображают реальные worker events.
- Auto Discover расширяет существующий S6.5, второй pipeline запрещён (R04). AUTO ADD остаётся отдельной policy-операцией; ON не означает автоматическое добавление или распространение файлов.

## 5. IMPORT GUITAR — сохранение макета и Preview

Сохранить утверждённый пользователем макет окна IMPORT GUITAR полностью, включая центральную область загрузки библиотеки и область PREVIEW. Не заменять макет другим браузером и не убирать Preview.

Зона drag-and-drop и BROWSE FILES — равноправные способы выбора; BROWSE FOLDER — если предусмотрен макетом. Состояния должны честно отражать IDLE → FILE SELECTED → LOADING → LOADED / ERROR. Preview относится к реально выбранному/декодированному файлу или инструменту; waveform не рисовать до успешного decode. Отдельно показывать библиотечные метаданные, mapping/velocity/RR/articulation coverage и текущий статус. Нельзя сообщать LOADED до успешной публикации библиотеки в sampler. Import не должен менять factory bank и не должен обходить license/provenance gate для factory redistribution.

## 6. SKINS GALLERY — макет, категории и поиск

Сохранить визуал Skins Gallery в точном соответствии с утверждённым макетом/образцом. Сохранить названия категорий без переименования:
- RETRO-PUNK
- DARK ROCK
- VINTAGE
- METAL CORE
- CLASSIC PUNK
- MINIMAL
- SIGNAL FORGE
- NEON STAGE

В разделе обязателен поиск скинов. Поиск и фильтрация не должны менять тему сами по себе. Preview временный; Apply фиксирует; Cancel возвращает исходную тему. Все карточки должны соответствовать реальным skin manifests/assets; отсутствующий skin не изображать доступным. Визуальный макет не даёт права менять layout/control geometry, IDs, hit areas или функции (TX-UI-PRESET-GUARD-004).

**Терминология:** SIGNAL FORGE / NEON — имя общего визуального языка продукта; NEON STAGE — пользовательское название главной визуальной темы/макета. Это не основание молча менять канонический default skin ID/name, закреплённый инвариантом I7; несоответствие должно быть разрешено отдельным формальным изменением инварианта, а не обходом.

## 7. FX EDIT

Пользователь одобрил текущий визуальный дизайн FX EDIT и потребовал оставить его без изменений. Сохранить композицию, визуальную иерархию, стиль модулей и их расположение как на утверждённом макете. Не делать редизайн/упрощение. Допустимы только необходимые функциональные подключения, исправления читаемости/доступности без изменения утверждённого вида и согласованные bug fixes. Каждый видимый control должен быть связан с существующим параметром и реальным DSP; фиктивные meters/кривые/кнопки не допускаются.

## 8. AUTOCHORDS и PATTERN BROWSER

Сохранить одобренный пользователем визуальный стиль AutoChords по предоставленному макету. В нижней части окна должен находиться отдельный Pattern Browser, визуально и функционально разделённый на:
- PLAYING STYLE / ARTICULATION — КАК исполняется гитара;
- RHYTHM PATTERN — КАКОЙ ритмический рисунок играет.

Каждая категория/папка/подпункт должна открываться, закрываться, прокручиваться и позволять выбирать вложенные элементы. Списки не должны быть статичными картинками. Сохранить верхнюю гармоническую область и независимость chord progression, playing style, rhythm pattern и playback mode. Не перекрывать MIDI-клавиатуру.

Канонический маршрут PATTERN browser из TX-UI-PRESET-GUARD-004: GENRE → ARTIST/BAND → SONG → SECTION → STYLE/ARTICULATION → PATTERN. SECTION: INTRO, VERSE, PRE-CHORUS, CHORUS, BRIDGE, OUTRO. Пользователь сначала выбирает музыкальный раздел (например Verse, Chorus или Bridge), затем способ исполнения и конкретный ритмический рисунок. Сохраняются все восемь pattern slots на каждую секцию, Preview / Apply / Cancel и host-synchronized transitions. Guitar source и Tone preset остаются отдельными осями согласно §15.7; не вставлять их в иерархию так, чтобы смешать звук, источник инструмента и способ игры.

**Конфликт строгого правила:** MASTER PROMPT REV 17.0 §11/§15 запрещает имена групп/артистов/чужих продуктов в UI. Поэтому видимый уровень ARTIST/BAND с именами, включая Blink-182, пока BLOCKED до формального MAJOR-изменения мастер-промпта и соответствующего инвариантного/правилового review. До такого решения сохранять hierarchy slot, но не публиковать запрещённые имена в UI. Внутренние style-DNA references допустимы в пределах действующих ограничений; запрещены копирование песен, риффов, транскрипций, master recordings и коммерческих пресетов. Цель 30 оригинальных preset/style variations, вдохновлённых широкими музыкальными характеристиками альбома *One More Time…*, не означает 30 копий композиций и не подтверждает готовность playable assets.

## 9. DOUBLE TRACK

Пользователь указал, что не нашёл кнопку DOUBLE и требует функциональности всех кнопок. Обеспечить понятный доступ к DOUBLE TRACK в утверждённой зоне настроек/управления, не создавая новые APVTS IDs и не меняя control inventory. Использовать только существующие параметры `doubleTrackOn` и `doubleTrackMix`; параметры ширины/detune/delay отображать только при наличии подтверждённого backend и существующих разрешённых IDs.

Аудит 06 ранее выявил, что `doubleTrackOn` / `doubleTrackMix` объявлены, но не читаются в audio path. Поэтому до отдельного аудио-ревью, реализации и тестирования toggle обязан быть disabled / BACKEND PENDING, а не активным фиктивным переключателем. Проверить отсутствие phase cancellation, mono compatibility, latency/offset, voice/CPU budget и стабильность при включении/выключении; изменения аудио — только по R10.

## 10. Blink-182 / One More Time… sound-target request

Зафиксирован запрос на 30 оригинальных guitar tone/performance preset variations в качестве общего стилевого sound target, опирающегося на музыкальные характеристики альбома *One More Time…*. Не использовать оригинальные stems, master recordings, точные мелодии, riffs, табулатуры, транскрипции или коммерческие закрытые пресеты. Названия пользовательских factory presets должны быть оригинальными и не создавать впечатление официальной связи с группой/альбомом. Каждая вариация должна ссылаться на реально доступный, очищенный по лицензии guitar source/performance map; пока такого материала нет — definition/design only, не playable.

## 11. Acceptance / no silent substitution

Перед приёмкой окон проверить:
1. pixel/layout parity с каждым утверждённым макетом;
2. SIGNAL FORGE / NEON единообразие всех панелей;
3. все кнопки, toggle, папки, selectors, search, scrolling, open/close и preview;
4. рабочую транзакцию Preview → Apply / Cancel;
5. отсутствие фиктивных активных controls и fake telemetry;
6. отсутствие перекрытия MIDI keyboard;
7. корректное сохранение состояния и отсутствие дубликатов окон;
8. отсутствие влияния UI/theme на audio output;
9. resource profile behavior только по измеренной policy matrix;
10. Curator local-user-library non-blocking rule;
11. Auto Discover OFF-by-default / one-shot explicit scan behavior;
12. exact-SHA Windows Release, все CTest targets, pluginval strictness 5, complete package и отдельную FL Studio 20 host acceptance.

**Статус:** APPROVED USER DESIGN DIRECTIONS / IMPLEMENTATION AND SOME POLICY CONFLICTS PENDING. Этот документ не отменяет MASTER PROMPT REV 17.0, I1–I8 или R01–R34.

---
## 15.16. TX-UI-BROWSER-SEPARATION-006 — два самостоятельных браузера и master-conflict guard (2026-11-14)

Эта секция уточняет TX-UI-PRESET-GUARD-004 и TX-UI-SURFACE-005. Она не меняет MASTER PROMPT REV 17.0 FINAL, I1–I8, R01–R34 и не является подтверждением runtime реализации.

### STYLE и PATTERN — самостоятельные окна
STYLE и PATTERN — два независимых окна/браузера. Их нельзя объединять, заменять друг другом или превращать в единый плоский список. Сохранять утверждённые макеты STYLE, PATTERN и AutoChords, а также progressive disclosure. Общий контекст может фильтровать результаты второго браузера, но данные и состояния выбора остаются раздельными.

**STYLE navigation:** GENRE → GROUP/ARTIST → SONG SELECTION → SONG SECTION → PLAYING STYLE / ARTICULATION. Секции: INTRO, VERSE, PRE-CHORUS, CHORUS, BRIDGE, OUTRO. Последовательно раскрывать совместимые дочерние элементы. GROUP/ARTIST остаётся зарезервированным уровнем, но видимые имена групп/артистов/чужих продуктов запрещены MASTER REV 17.0 §11/§15; до MAJOR изменения prompt этот уровень BLOCKED для публикации именованного каталога. Не обходить запрет псевдонимами/логотипами.

**PATTERN categories:** DOWNSTROKES, DOWN/UP, PALM MUTE, POWER CHORDS, ALTERNATE PICKING, CUSTOM PATTERNS. PATTERN browser отвечает только за ритмический рисунок; STYLE/ARTICULATION — за способ исполнения. На каждую секцию — 8 Pattern Slots. Секция/стиль могут фильтровать паттерны, но не смешиваются с Tone Preset, Guitar Source или FX.

Каждая папка/категория/вложенный элемент должен открываться, закрываться, прокручиваться и выбирать реальные данные. SEARCH, FAVORITES, FILTER, SORT, PREV/NEXT сохраняют текущий scope. Предыдущий/следующий работают внутри текущего результата. Недоступные в карте библиотеки артикуляции/паттерны показываются unavailable/disabled, без синтетической подмены записанной гитары.

### Transaction and playback
PREVIEW — временное audition без изменения active preset, project/DAW persistent state или сохранённого Pattern Slot. APPLY/SELECT фиксирует выбор; CANCEL полностью откатывает незавершённую транзакцию. Pattern transitions во время playback должны быть host-synchronized и исключать stuck notes, duplicate triggers, timing jumps и abrupt cuts. Пока backend не обеспечивает это, соответствующие controls disabled/BACKEND PENDING.

Chord/voicing, Guitar Source, Tone Preset/FX, Playing Style/Articulation и Rhythm Pattern — отдельные оси. Chord key выбирает chord/voicing; pattern управляет timing/gate/velocity/accent/rhythm. MELODY сохраняет одну ноту на клавишу. Ни одно окно/overlay не перекрывает MIDI keyboard и не блокирует hit-testing.

### NEON STAGE / I7 conflict
NEON STAGE — утверждённый visual reference/direction, SIGNAL FORGE / NEON — общий visual language. Но runtime default skin ID/name строго определяется MASTER REV 17.0 I7/R18: 'DEFAULT POP PUNK GUITAR ULTIMATE SKIN' ('default-ppgu'). Поэтому NEON STAGE нельзя назначить runtime default вместо 'default-ppgu' без MAJOR-изменения master prompt и явного изменения I7. До такого изменения I7/R18 имеют приоритет; никакого молчаливого переименования, смены fallback или обхода registry.

### Invariants
Сохраняются factory catalog extension-only, stable IDs, read-only factory presets, отдельное хранение user presets/banks, явные import conflicts, форматы '.ppgm' / '.ppgp' / '.ppgbank', отсутствие фиктивных активных controls, I1/I2, R04 (один S6.5 pipeline), R05/R10 (вне audio callback и аудио-review), R07/R23 (честные статусы и PASS только exact SHA). Никаких изменений APVTS, DSP, MIDI, sampler, SoundBank policy или realtime callback в рамках UI-only этапа.

**Статус:** APPROVED REQUIREMENTS / IMPLEMENTATION NOT IMPLIED. Отдельная приёмка STYLE, PATTERN и AutoChords, browser behavior, 8 slots/section, transaction rollback, host sync, keyboard hit-testing, I7/R18 preservation, exact-SHA CI и FL Studio 20 остаётся обязательной.


---

# 2026-11-14 MAJOR amendment — FX/DSP, NEON STAGE DEFAULT, AUTO DISCOVER, factory presets and library rights

This addendum records the user's explicit MAJOR approval and supersedes earlier conflicting requirements only in the named areas. The full normative text and acceptance rules are in [PPGU_MAJOR_USER_REQUIREMENTS_AMENDMENT_2026-11-14_RU.md](PPGU_MAJOR_USER_REQUIREMENTS_AMENDMENT_2026-11-14_RU.md).

- FX EDIT: up to 15 fully implemented effect/processors plus cabinets. DSP minimum is 10, target 15. Exact module names must be reconciled against the earlier approved FX list; no invented filler. Real DSP, parameter mapping, state, automation where supported, truthful states and tests required. Existing APVTS IDs/semantics remain immutable.
- AUTO DISCOVER: full production implementation required. Explicit ON/OFF; OFF means no discovery; ON only permits; explicit SCAN starts bounded cancellable work; no scheduler/automatic repeat; use the existing pipeline outside the audio callback.
- Default skin: **NEON STAGE DEFAULT**. First supplied mockup is exact layout/appearance reference; second is palette reference. Additional themes are graphics-only derivatives. Convert artwork into correctly sliced component/state PNG asset sets. No changes to geometry, controls, functions, bindings, MIDI, DSP, sampler or sound. Auto-load on first launch/fallback. Preserve stable default ID `default-ppgu` unless a reviewed migration proves otherwise. This explicitly amends the earlier I7/R18 default display-name contract; unrelated invariants remain.
- Factory presets: 300 minimum, open-ended and append-only. Each must map to the user's canonical PUNK GROUP list and real source/performance data. Internet research/work may be reused only where rights permit; no copying proprietary/closed presets, samples or protected compositions.
- Preset Browser first category: **Pop Punk Essentials**. Preserve GENRE → ARTIST/BAND → SONG/SECTION → STYLE → PATTERN and section list. STYLE and PATTERN remain separate. Preserve browser controls, non-destructive PREVIEW and context-aware PREV/NEXT. Existing visible artist-name restriction remains unless separately amended.
- SOUND HUNTER: no restriction on lawful user-local download/import/load/use of commercial libraries. Licensing status governs factory inclusion and redistribution only; never silently block, hide, disable, delete or quarantine a user's library solely because it is commercial or not redistributable by PPGU.

**Status:** APPROVED REQUIREMENTS / DOCUMENTATION ONLY. No production source, DSP, APVTS, preset payload, soundbank, skin assets, runtime UI, tests or build outputs were changed by this addendum.


## MAJOR UI clarification (2026-11-14): MELODY / PATTERN preset browser split

This approved clarification supplements the earlier general browser specification.

### MELODY — three-part browser
The MELODY browser contains exactly three coordinated areas:

1. **PRESET / GUITAR IDENTITY:** pop-punk guitar/genre, the user's PUNK GROUP or band-style identity, and individual song part/section. Selects which guitar/source and section character are active.
2. **STYLE / PLAYING ARTICULATION:** how notes are performed; examples include Open Chord, Palm Mute and Muted. Other articulations (such as tremolo/picking) are exposed only when supported by mapped recordings. STYLE does not select the rhythm sequence.
3. **PATTERN / RHYTHM VARIATION:** optional rhythmic figure/variation associated with the current guitar/section/style. It can be activated within MELODY mode without changing ordinary melody-note semantics.

### MELODY MIDI behaviour
Ordinary MELODY keys play one corresponding pitched guitar note and decay naturally; a single melody note does not start a strumming loop. The optional PATTERN layer is activated by additional pattern-trigger MIDI notes/events in Piano Roll or by a dedicated on-screen PATTERN button/control. The agreed key-zone convention reserves C0–C1 for pattern-trigger keys and C1+ for ordinary melody notes; pattern-trigger keys do not double as chromatic guitar notes. Holding a trigger sustains the selected pattern under its gate/host-sync rules; release ends it cleanly. The user can switch rhythm variations in the PATTERN area/button, with safe host-synchronised transitions and no stuck/duplicate triggers.

### PATTERN mode
PATTERN remains a separate performance mode: mapped keys select chord/voicing and trigger rhythmic guitar performance. Guitar/source identity, PLAYING STYLE / ARTICULATION and RHYTHMIC PATTERN remain separate dimensions. MELODY and PATTERN have separate performance semantics/state but share the approved sample library/cache. Preserve the already approved PATTERN categories and eight slots per section.

### Common safeguards
PREVIEW does not change active preset/project/persistent DAW state or saved slots; APPLY/SELECT commits; CANCEL restores previous state. PREV/NEXT and search/filter/sort/favorites preserve current result context. Do not expose unsupported articulations or patterns as playable; do not invent missing PUNK GROUP names. This direct approval allows the user's PUNK GROUP identities and song sections to be shown in the MELODY PRESET area, superseding any earlier conflicting display restriction only for that area.

**Status:** approved requirement; documentation only, not evidence of source implementation or validation.


## 15.17. PATTERN mode — two-window browser layout (2026-11-14)

PATTERN mode uses **two separate browser windows**:

**Window 1 — PRESET / GUITAR IDENTITY**
Hierarchical selection of pop-punk genre/guitar type → PUNK GROUP/band identity → song-based selection → song section and compatible nested sub-items. Group/band and song/section dimensions are required; do not omit them or replace them with generic labels. Use canonical user-supplied names and actual source mappings only.

**Window 2 — PLAYING STYLE / RHYTHMIC PERFORMANCE**
Selects the playing style/articulation (how the guitar is played) and, as a distinct dimension, the rhythmic category/pattern/variation (what rhythmic figure is played). Preserve the existing canonical categories: DOWNSTROKES, DOWN/UP, PALM MUTE, POWER CHORDS, ALTERNATE PICKING, CUSTOM PATTERNS. Preserve P1–P10 only where present in the canonical catalog and eight Pattern Slots per section. No invented IDs or unsupported playable choices.

PATTERN remains separate from MELODY and retains approved AUTO CHORDS ON/OFF behaviour. PRESET window owns guitar/source and song context; performance window owns style and rhythm. Chord/voicing, source identity, tone preset/FX, articulation and rhythm remain separate axes. Filtering may constrain compatible combinations but cannot collapse the windows or merge their state. Preserve non-destructive PREVIEW, explicit APPLY/SELECT, CANCEL rollback, context-aware navigation/search/filter/sort/favorites, keyboard visibility and all realtime/MIDI/APVTS/sampler/stable-ID invariants.

**Status:** approved requirement; documentation only. No runtime implementation or tests are claimed.


## PATTERN — ЗАФИКСИРОВАННАЯ СТРУКТУРА ДВУХ ОКОН

**СТАТУС: УТВЕРЖДЕНО ПОЛЬЗОВАТЕЛЕМ. ТЕКСТ И СТРУКТУРА ЗАФИКСИРОВАНЫ БЕЗ ИЗМЕНЕНИЙ.**

Два самостоятельных окна.
Первое — выбор PRESET и музыкальной идентичности: жанр, группа, композиция, секция и соответствующие подпункты.
Второе — стиль исполнения, артикуляция, ритмическая категория, PATTERN и вариации.
AUTO CHORDS сохраняется как отдельная функция режима.
Не объединять выбор источника звучания, артикуляцию и ритмический рисунок в одну категорию.

**Правило приоритета:** эта формулировка является прямой утверждённой фиксацией пользователя. При реализации и дальнейшей редакции журналов сохранять её смысл и состав без переименования, упрощения, объединения окон или подмены назначений. Более поздние явно утверждённые пользователем изменения могут дополнять её, но не считаются разрешением на молчаливое изменение.

**Граница статуса:** зафиксировано и утверждено как требование. Само по себе это не подтверждает реализацию интерфейса, браузеров, AUTO CHORDS или backend и не является результатом сборки/тестирования.


## MELODY — подтверждение финальной структуры и поведения (пользовательское утверждение)

**СТАТУС: УТВЕРЖДЕНО ПОЛЬЗОВАТЕЛЕМ.** Эта запись фиксирует подтверждение пользователем ранее согласованной финальной версии; не является новым проектным предложением.

### MELODY — три отдельные области
- **PRESET** — выбор гитарного пресета.
- **STYLE** — артикуляция и способ исполнения мелодии.
- **PATTERN** — ритмическая вариация внутри MELODY.
- Одна обычная MIDI-клавиша соответствует одной мелодической ноте. Обычное нажатие не должно самопроизвольно запускать strumming loop или PATTERN.

### Включение PATTERN внутри MELODY
- PATTERN внутри MELODY является дополнительной функцией.
- Для запуска используется дополнительная MIDI-прорисовка/триггер в Piano Roll.
- Зона C0–C1 выделяется для PATTERN-триггеров; выше располагается обычный мелодический диапазон. Триггерная зона не должна одновременно звучать как обычные хроматические ноты.
- Предусмотрено переключение вариаций PATTERN.
- Обычная MELODY и основной режим PATTERN остаются разными режимами.
- Без дополнительного запуска PATTERN MIDI исполняется обычно: одна нота звучит как нота, несколько одновременно заданных нот образуют соответствующий аккорд. Дополнительная MIDI-команда запускает выбранный PATTERN; основные MIDI-ноты задают исполняемую ноту или аккорд.
- PATTERN не должен самопроизвольно изменять обычное мелодическое исполнение.

### Главная структура интерфейса и расположение браузера
- Главные области интерфейса: **MELODY**, **PATTERN**, **FX EDIT**.
- Браузер располагается слева; его раскрытие и сворачивание не должно смещать основные области интерфейса.

**Правило приоритета и совместимости:** это пользовательское утверждение финальной версии. Сохранять все перечисленные пункты вместе с ранее утверждёнными подробными спецификациями MELODY и PATTERN. При реализации трактовать одновременные обычные MIDI-ноты как заданную пользователем ноту/аккорд, не смешивая их с дополнительными PATTERN-триггерами. Если обнаружится противоречие с более поздним явно утверждённым требованием, остановить изменение и запросить разрешение, а не исправлять молча.

**Граница статуса:** APPROVED REQUIREMENT / DOCUMENTATION ONLY. Эта запись не подтверждает реализацию UI, MIDI routing, pattern engine, browser, сборку или тестирование.


## ИСТОРИЧЕСКОЕ УТОЧНЕНИЕ И ПОДТВЕРЖДЕНИЕ РАНЕЕ СОГЛАСОВАННОЙ СТРУКТУРЫ MELODY И PATTERN — APPROVED

Статус: APPROVED BY USER; DOCUMENTED BY APPEND-ONLY JOURNAL UPDATE. Это уточнение восстанавливает ранее согласованные решения и не создаёт новую структуру вместо них. Оно не отменяет более поздние утверждённые изменения, кроме явного исправления ошибочной трактовки истории.

### 1. MELODY — ранее согласованные три графы
В обсуждении 13 ноября были согласованы три самостоятельные графы:
- PRESETS — звуки, гитары, артисты/группы и выбор соответствующего звучания;
- MELODY / ARTICULATION — способы исполнения, включая OPEN, MUTED, PALM MUTE, TREMOLO и другие артикуляции, только если они реально поддержаны источниками и mapping;
- PATTERN — ритмическая последовательность/вариация.

Это ранее согласованная структура, а не новая реконструкция. Термины и назначения граф сохраняются раздельными; не смешивать выбор источника/звуковой идентичности, артикуляцию и ритмический рисунок.

### 2. MELODY — ранее согласованное MIDI-поведение PATTERN
- Дополнительная MIDI-прорисовка/команда в Piano Roll активирует ритмическое исполнение выбранного PATTERN.
- Одиночная обычная MIDI-нота без дополнительной команды PATTERN звучит один раз как нота.
- При активном PATTERN удерживаемая нота исполняется согласно выбранному ритмическому рисунку.
- Обычные MIDI-ноты сохраняют свою мелодическую семантику; PATTERN trigger не должен случайно дублироваться как обычная нота.
- Session Guitarist — Electric Sunburst Deluxe используется только как функциональный ориентир поведения, без копирования защищённых материалов, кода, сэмплов, пресетов или UI.

### 3. MELODY и PATTERN — самостоятельные режимы
Ранее согласовано, что MELODY и PATTERN являются двумя самостоятельными режимами/инструментами:
- настройки, состояния и пресеты режимов хранятся раздельно;
- библиотеки исполнения и структуры режимов не объединяются;
- общая PPGU SAMPLE LIBRARY допустима как общий пул доступных аудиоисточников, но режимы сохраняют раздельную логику исполнения и собственные параметры.

Согласованное распределение измерений:
- MELODY: PRESET / STYLE / PATTERN;
- PATTERN: PRESET / PLAY STYLE / ARTICULATION / RHYTHMIC PATTERN / AUTO CHORDS.

Общая библиотека сэмплов не означает объединение режимов или их preset/state schemas.

### 4. Границы утверждения по MIDI trigger range
Диапазон C0–C1 не подтверждён приведённой исторической записью как ранее окончательно утверждённое пользователем решение. Поэтому данный диапазон НЕ получает статус APPROVED на основании этого подтверждения. Существующие записи о нём сохраняются как техническое предложение/требование, нуждающееся в отдельном подтверждении, и не должны выдаваться за исторически утверждённый факт. Любое применение диапазона в реализации требует проверки актуального канонического MIDI-контракта и отдельного согласования, если оно меняет утверждённую семантику.

### 5. Приоритет и неизменность
Это подтверждение фиксирует ранее согласованные структуры и разделение режимов. Оно не разрешает менять APVTS IDs/semantics, MIDI contracts, DSP contracts, sampler lifecycle, SoundBank, engine behavior, preset schema или другие строгие контракты без предусмотренного аудита и согласования. Документирование не означает IMPLEMENTED, TESTED или VERIFIED в исходном коде.
