# PPGU Audio Engine — архитектура семейства инструментов
Дата: 11 ноября 2026
Статус: ARCHITECTURE DECISION / DESIGNED
Область: POP PUNK GUITAR ULTIMATE и будущий POP PUNK BASS GUITAR VST

## 1. Принятое продуктовое решение

POP PUNK GUITAR ULTIMATE (Guitar Edition) и POP PUNK BASS GUITAR VST (Bass Edition) строятся на одном общем PPGU Audio Engine и используют общие технологии. Отличаются звуковым материалом и специализированными профилями инструмента.

Это не два независимых аудиодвижка и не один универсальный плагин с переключателем «гитара/бас». Это общая технологическая платформа и два самостоятельных VST-продукта.

## 2. Общая платформа

В общий PPGU Audio Engine входят или поэтапно выделяются:

- Audio lifecycle, realtime-safe processing и voice scheduling.
- Sample streaming: bounded queues, page cache, page leases, background decoding.
- Общие WAV/SFZ reader и metadata infrastructure.
- Layer/multisample rendering primitives.
- Preset/state infrastructure и безопасное переключение состояний.
- MIDI event handling и общие scheduler primitives.
- DSP/FX infrastructure.
- Resource budgeting, worker lifecycle, diagnostics и telemetry.
- UI framework, WindowManager и Theme/Skin infrastructure.
- Общие инструменты QA, тестовые fixtures и packaging infrastructure.

Общий код не должен содержать жёстко зашитые гитарные или басовые артикуляции. Специализированное поведение предоставляется через профиль инструмента и конфигурацию продукта.

## 3. Отдельные продуктовые профили

### Guitar Edition
Собственные:
- Guitar SoundBank и guitar-only bank scanner policy.
- Электрические, акустические и nylon/steel guitar sample sets.
- Guitar articulations: open, palm mute, harmonics/flageolet, slides, bends, hammer-on/pull-off, tremolo и другие гитарные техники.
- Guitar-specific velocity layers, RR и mapping.
- Melody/Pattern guitar presets и guitar performance patterns.
- Guitar factory content, metadata, artwork и product identity.

### Bass Edition
Собственные:
- Bass SoundBank и отдельная политика сканирования/классификации басовых библиотек.
- Bass sample sets и bass-specific mapping.
- Bass articulations: finger, pick, slap, pop, muted/dead notes, slides, harmonics и иные техники по мере реализации.
- Bass-specific velocity layers, RR и mapping.
- Bass presets и bass performance patterns.
- Bass factory content, metadata, artwork и product identity.

Bass-материалы не должны попадать в основной Guitar SoundBank; guitar-материалы не должны автоматически попадать в Bass SoundBank. Форматы и технологии чтения могут быть общими, классификация и продуктовые каталоги — раздельными.

## 4. Граница между Engine и Product Profile

Предлагаемое разделение ответственности:

- Engine: как безопасно и эффективно загружать, кэшировать, читать, смешивать, обрабатывать и воспроизводить аудио.
- Product Profile: какие инструменты, артикуляции, карты нот, velocity/RR слои и музыкальные режимы доступны.
- SoundBank: какие конкретные звуки доступны продукту, откуда они получены, какова лицензия и можно ли их распространять.
- Factory Content: пресеты, паттерны, UI assets и документация конкретного продукта.

Engine получает профиль через явную конфигурацию/интерфейсы. Нельзя определять тип инструмента по имени файла, случайному тегу или эвристике в audio callback.

## 5. Realtime и производительность — общие обязательные требования

- Никакого filesystem/network I/O, блокирующих mutex, ожиданий и динамических аллокаций в audio callback после подготовки.
- Межпоточный обмен — bounded queues, атомарные snapshots и явно определённое владение данными.
- SPSC queue используется только при доказанном единственном producer; объединение потоков не должно незаметно нарушать этот контракт.
- Загрузка/сканирование/декодирование и вычисление хэшей выполняются на worker/control threads.
- Page leases и lifetime ресурсов должны оставаться корректными при voice steal, panic, preset switch, bank reload и shutdown.
- Параметры CPU/RAM/startup являются целями, пока не подтверждены воспроизводимыми измерениями на целевом оборудовании.

## 6. Лицензии и распространение

- Встроенные в дистрибутив звуки допускаются только при подтверждённом разрешении на перераспространение (например, CC0/Public Domain или отдельная явная лицензия).
- User-imported samples остаются внешними пользовательскими данными и не включаются в установщик без отдельного разрешения.
- У каждого продукта — отдельный каталог банков, лицензионные evidence и attribution report.
- Общий loader не означает общую SoundBank и не отменяет проверку лицензии каждого источника.

## 7. Архитектура сборки и идентичность плагинов

Целевое направление CMake:
- Общая библиотека/target PPGU Audio Engine с независимыми от продукта исходниками.
- Отдельный Guitar VST3 target с Guitar Product Profile и уникальными plugin codes/state namespace.
- Отдельный Bass VST3 target с Bass Product Profile и собственными уникальными plugin codes/state namespace.
- Общие тесты Engine и отдельные тесты каждого Product Profile.
- Общие packaging helpers при раздельных наборах Samples, presets, licenses и installer metadata.

Нельзя копировать текущий plugin target целиком и считать это общей платформой. Нельзя менять plugin identity существующего Guitar VST3 при выделении общего Engine. Рефакторинг должен сохранять обратную совместимость состояния Guitar Edition либо включать явную миграцию state.

## 8. Поэтапная реализация

1. Зафиксировать архитектурную границу и инвентаризировать текущие engine/product dependencies.
2. Довести Guitar Edition: streaming lifecycle, SoundBank scanner, license evidence, preset/state integrity, UI, realtime safety и QA.
3. Выделять общие модули из Guitar только после тестов, не переписывая стабильный playback path без необходимости.
4. Создать Guitar Product Profile поверх выделенного Engine и подтвердить идентичность/совместимость VST3.
5. Создать Bass Product Profile и отдельный Bass SoundBank на тех же общих модулях.
6. Проверить, что добавление Bass не увеличивает ресурсы Guitar Edition и не меняет его SoundBank или factory presets.

## 9. Текущий статус реализации

Это архитектурное решение, а не заявление о готовой общей библиотеке или готовом Bass VST.

Подтверждённая в текущем Guitar-коде база включает SFZ/WAV playback path, WAV metadata indexing, background page streaming, fixed page cache и page leases. Полноценный универсальный SoundBank, product-profile abstraction, выделенный PPGU Audio Engine target и Bass Edition ещё не подтверждены как реализованные.

Следующие инженерные шаги должны быть отдельными проверяемыми изменениями с тестами и обновлением статусов:
- определить реальную dependency map Guitar processor → sampler/streaming/DSP/MIDI/UI;
- выделить только доказанно общие низкоуровневые модули;
- сохранить существующий Guitar plugin ID и state compatibility;
- реализовать Guitar-only scanner отдельно от будущего Bass scanner;
- покрыть общие компоненты тестами без подключения Bass-кода к Guitar SoundBank.

## 10. Статусные метки

Использовать строго:
- PLANNED — запланировано;
- DESIGNED — архитектура/контракт описаны;
- IMPLEMENTED — код реализован;
- BUILT — сборка прошла;
- TESTED — соответствующие тесты прошли;
- HOST-VERIFIED — проверено в целевом DAW;
- BLOCKED — есть подтверждённый блокер.

Данный документ имеет статус DESIGNED. Он не переводит ни один модуль в IMPLEMENTED, BUILT, TESTED или HOST-VERIFIED.


## 2026-11-14 — SOURCE TREE / BUILD GRAPH RECONCILIATION ADDENDUM

This addendum records actual repository source locations inspected at main HEAD 1f09611f753702e380d695a5441b6f260f798d3b. It does not replace the earlier architecture decision.

- Processor/editor: Source/PluginProcessor.*, Source/PluginEditor.*; parallel root processor/editor files also exist.
- Streaming sampler: Source/Sampler/ contains SamplePool, SampleMetadataIndex, SampleStreamingService, PageCacheSlots, StreamingVoiceReader, WavPageReader, SfzParser and BoundedSpscQueue.
- Performance: Source/Performance/ contains PatternScheduler, AutoChordEngine, ChordTransitionPolicy, ChordTransitionTimeline and CircleOfFifths.
- UI: Source/UI/ contains AutoChordsPanel, AuxiliaryPanelLayout, AuxiliaryPanelWindowManager, EditorOverlayPanel, FXEditPanel, ImportGuitarPanel, PerformanceDisplay, SettingsPanel and ThemeManager.
- Parallel root-level modules include sampler, PatternScheduler, ChordEngine, FxChain, ImportPipeline, PresetRuntime, ThemeManager, panels, workspace, overlay and security import. They require a deliberate canonical-source decision.

Static comparison found CMake source references that do not exist at the named paths: Source/Engine/*, Source/UI/MultiWindowWorkspace.cpp, Source/UI/Panels.cpp, and tests/EngineContractTests.cpp. CMake also expects a local JUCE/ directory; the audited tree has no JUCE directory/gitlink and the inspected workflow does not provision JUCE via submodule or clone. These are integration blockers to resolve before claiming reproducible build. No source relocation or API migration is authorized by this addendum.

The approved no-upper-limit catalog rule applies to MELODY, PATTERN and every approved browser entity/category. The legacy factory_bank_300.json filename/validator must not impose a maximum of 300. Preset definitions must remain valid, unique, mapped, licensed and loadable; scale through indexing/lazy loading/virtualized UI and bounded audio resources.

See the detailed authoritative inventory and exact audit boundaries in Info.txt, section “MASTER ENGINE / PLUGIN SOURCE INVENTORY, CODE-OF-RECORD AND REPOSITORY INTEGRATION AUDIT”.
