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