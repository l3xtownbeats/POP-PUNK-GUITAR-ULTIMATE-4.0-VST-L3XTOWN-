# POP PUNK GUITAR ULTIMATE 5.0 — Engineering Upgrade Contract
Дата: 2026-11-11
Базовая ветка: `5.0-audio-integration`
Рабочая ветка: `5.0-engineering-upgrade`
Назначение: инженерные корректировки требований и порядок безопасного улучшения существующей реализации.

> Этот документ не объявляет функции готовыми. Реализация подтверждается конкретным исходником, тестом, Windows x64 CI и отдельной проверкой в FL Studio 20. Цели CPU/RAM ниже являются измеряемыми целями, а не обещанием.

## 1. Решение по архитектуре

Сохраняем текущий JUCE 7.0.12 / C++20 / VST3 каркас, APVTS parameter IDs, текущую схему SampleBank/SFZ, утверждённую структуру MELODY/PATTERN и визуальный контракт 5.0. Не переписываем движок целиком и не переносим крупные feature-ветки вслепую.

Приоритеты конфликтов (сверху вниз):
1. Корректность и безопасность real-time audio thread.
2. Сохранность проектов, стабильность APVTS IDs и совместимость VST3/FL Studio.
3. Правильная музыкальная логика и достоверное отображение состояния ресурсов.
4. Ограничение CPU/RAM и быстрая реакция.
5. Расширение функций и визуальные эффекты.

Если красивое UI-действие конфликтует с audio-thread safety — UI упрощается, аудиопуть не блокируется. Если preset требует отсутствующей артикуляции — показывается unavailable, другой сэмпл молча не подставляется.

## 2. Что подтверждено в базовой 5.0 ветке по исходникам

- VST3/JUCE/C++20 и APVTS находятся в проекте.
- MELODY обрабатывает MIDI note-on/note-off как отдельные ноты.
- PATTERN-путь содержит sample-accurate MIDI timeline, вызов PatternScheduler и передачу ScheduledEvent в sampler noteOn/noteOff.
- AutoChords включается только в PATTERN chord reader; одиночная нота может преобразовываться в triad, многонные MIDI-аккорды передаются как входные ноты.
- SFZ import создаёт metadata index и SampleStreamingService; RegionView содержит streaming service + sample ID.
- Присутствуют фиксированный page cache и bounded SPSC request queue.
- В процессоре заранее выделяются рабочие буферы для части FX; часть эффектов выполняется в processBlock.
- В UI реализованы каталог пресетов, фильтрация и FX/skin controls.

Это подтверждение наличия кода, но не полная приёмка звучания или стабильности.

## 3. Обнаруженные архитектурные риски / ограничения

### 3.1 Lifetime и reclamation SampleBank
Текущий SamplePool сохраняет опубликованные snapshots и streaming libraries до уничтожения SamplePool. Это предотвращает раннее освобождение сырых указателей, но приводит к накоплению памяти при повторных загрузках. Также имеются явные ограничения количества snapshots и корней библиотек.

Решение:
- Не удалять лимиты, пока не внедрён безопасный reclamation.
- Следующий шаг — immutable BankGeneration с монотонным generation ID.
- Audio thread получает стабильный handle на поколение; освобождение старого поколения выполняется только на control/worker thread после подтверждённого quiescent state.
- Запретить shared_ptr последнего владельца уничтожать тяжёлые объекты на audio thread.
- Добавить тесты многократной загрузки/отмены/переключения и проверки отсутствия use-after-free.
- Лимит кэша оставить жёстким и конфигурируемым; число библиотек ограничивать общим RAM budget, а не произвольным числом корней.

### 3.2 Стриминг и cache miss
Наличие PageCache и StreamingVoiceReader само по себе не доказывает отсутствие underrun. На cache miss аудиопоток обязан немедленно вернуть безопасный результат и только поставить bounded request.

Решение:
- Никакого ожидания worker, mutex, file I/O, декодирования, аллокации или освобождения памяти в processBlock/voice render.
- Страницы, которые читаются аудиопотоком, должны быть pinned/leased до окончания чтения блока.
- Worker не имеет права переиспользовать pinned page.
- При miss: тишина только для недоступного участка + счётчик underrun + приоритетный запрос; без блокировки аудиопотока.
- Проверить границы файлов, короткие WAV, mono/stereo, PCM 16/24/32-bit, float WAV, повреждённые файлы, loop и seek через границы страниц.
- Сравнить Hermite и linear interpolation по CPU и качеству; оставить Hermite только если измеримо оправдан.

### 3.3 PATTERN и AutoChords
В базовой ветке уже присутствует путь планирования и dispatch событий. Однако программно сгенерированные rhythm masks не равны готовой профессиональной библиотеке исполнений.

Решение:
- Отдельно тестировать transport start/stop, seek, loop wrap, tempo changes, chord change в середине блока, одинаковый sample position NoteOff/NoteOn, panic.
- Разделить ownership Melody voices и Pattern voices; transport stop не должен останавливать удерживаемые Melody ноты.
- Pattern data хранить как версионированные музыкальные события/артикуляции, а не выдавать простую маску ритма за 200+ готовых исполнений.
- AutoChords: тестировать major/minor/chromatic, scale degree, инверсии, slash bass, диапазон MIDI 0–127, смену тональности и удержание нескольких нот.
- До появления лицензированных/авторских performance data помечать встроенные ритмы как базовые алгоритмические patterns.

### 3.4 Preset identity и Sound Bank resolution
Каталог пресетов не является доказательством, что пресет реально звучит нужной гитарой.

Каждый preset должен иметь versioned manifest:
- stable preset ID + schema version;
- mode compatibility (MELODY/PATTERN);
- library ID + sample profile ID;
- sample/articulation IDs и список доступных артикуляций;
- velocity/RR mapping;
- FX snapshot по стабильным APVTS IDs;
- стиль/характер/теги для поиска;
- missing-resource policy.

Загрузка транзакционная: подготовить ресурсы вне audio thread → проверить зависимости → атомарно опубликовать готовое состояние → старое поколение освободить безопасно. Не подменять активный пресет в момент audition.

### 3.5 Async loading и state restore
- Сохранять логический library ID и относительный путь/manifest reference, а не только абсолютный SFZ path.
- При restore валидировать path и checksum/manifest; не начинать файловую работу в audio thread.
- Все async операции маркировать generation token; устаревшие completion игнорировать.
- Callback не должен обращаться к уничтоженному editor. Processor lifetime и editor lifetime — разные сущности.
- Ошибка загрузки не должна менять текущий активный банк или оставлять UI в ложном состоянии.

### 3.6 FX, idle и UI
- FX prepare/allocate только вне audio thread; переключение bypass не должно менять память/структуру цепочки в callback.
- Параметры читаются атомарно/APVTS; сглаживание коэффициентов — в audio thread без mutex.
- В Idle host может продолжать вызывать processBlock. Поэтому цель — ранний выход/пропуск DSP, а не обещание, что callback вообще не вызывается.
- UI polling ограничить разумной частотой; никакого repaint/audio meter timer с чрезмерной частотой.
- Skin — ресурсный пакет с manifest, шрифтами, SVG/bitmap, layout tokens и проверкой fallback. Не декодировать большие изображения на UI каждый кадр.
- Все кнопки, которые ещё не имеют реальной функции, показывать disabled/coming soon; не маскировать заглушки под работающие функции.

## 4. Производительность: корректная методика

Нельзя использовать только процент CPU из диспетчера задач: он зависит от DAW, числа ядер, планировщика и фоновых задач. Основная метрика — время исполнения processBlock относительно длительности аудиоблока.

При 48 kHz:
- 64 samples = 1.333 ms на блок;
- 128 samples = 2.667 ms;
- 256 samples = 5.333 ms.

Измерять wall-clock/steady-clock duration вокруг тестового render harness (не добавлять таймеры в production audio callback без отдельного доказательства RT safety), median, P95, P99 и worst observed. Параллельно фиксировать DAW CPU как дополнительную метрику.

Сценарии:
- Idle, 1 voice, 4-note chord, 16 voices, 32 voices;
- PATTERN 1/2/4 bars;
- FX off, amp/drive, modulation, delay/reverb, full chain;
- streaming cold cache / warm cache / forced cache miss;
- preset load, preset switch, preview on/off;
- 30-minute soak и повторная смена банков.

Цели остаются:
- idle DSP work практически нулевой;
- 1 voice без FX — ориентир <2% общей загрузки CPU в согласованном тестовом окружении;
- 16 voices + FX + PATTERN — ориентир <6%, но приёмка также требует P99 времени блока существенно ниже deadline;
- RAM idle <100 MB и playing <500 MB — считать Private Bytes, Working Set, cache и библиотечный disk footprint отдельно;
- cold ready <3 s только для уже известного/подготовленного банка; первый индекс большого банка измерять отдельно;
- подготовленный preset swap <10 ms только если все зависимости готовы; иначе честный loading state.

## 5. Рекомендуемая реализация по этапам

### E0 — Baseline / regression map
- Зафиксировать SHA исходника, ветку, compiler, JUCE, OS, DAW, CPU, sample rate, block size.
- Снять текущий CI и CTest.
- Составить таблицу всех реальных функций UI: working / partial / disabled.
- Не менять поведение до baseline.

### E1 — Real-time safety and ownership
- Проследить весь call graph processBlock → sampler → voice → streaming reader → FX.
- Статически и тестами проверить отсутствие file I/O, allocation, deallocation, mutex wait и GUI calls на audio thread.
- Ввести BankGeneration / lease protocol и stress tests на concurrent load/unload.
- Сохранить старые snapshot lifetime ограничения до доказанной замены.

### E2 — Streaming runtime
- Проверить lease/pin страницы на весь render block.
- Измерить cache hit/miss/underrun, request queue overflow и prefetch effectiveness.
- Проверить multi-page, loop, reverse/ping-pong, long file и sample-rate conversion.
- Лишь после этого менять RAM/snapshot limits.

### E3 — Musical correctness
- Закрыть transport edge cases.
- Закрыть Melody/Pattern source separation.
- Тестировать AutoChords и chord transitions.
- Добавлять только реальные pattern definitions с корректной лицензией/авторством.

### E4 — Preset transaction and preview
- Manifest resolver, stable IDs, versioning, migrations.
- Background resource preparation and atomic state handoff.
- Preview временный и не меняет активный preset; один preview voice pool, старый preview останавливается перед новым.
- State restore включает логический SoundBank identity.

### E5 — DSP/FX and UI
- Профили GOOD/NORMAL/NIGHT/ECO должны иметь измеряемые настройки и сохранять musical correctness.
- Multicore toggle активировать только после реальной реализации и измерения; на 2C/2T — не более одного вспомогательного worker для подготовки/стриминга.
- Полные skin packs; одинаковая функциональность и layout contract во всех темах.

### E6 — Release acceptance
- Windows x64 Release VST3 build.
- CTest + pluginval strictness 5.
- Long session, cache stress, state restore, MIDI stress.
- Ручная проверка FL Studio 20: загрузка, note on/off, аккорды, transport, automation, save/reopen, смена аудиоустройства.
- CPU/RAM report с исходными условиями.
- Отдельно отмечать CI, автоматические тесты и ручные host tests.

## 6. Release gates — что запрещено объявлять готовым

- Streaming — пока voice render не читает безопасно page leases и cache miss не доказан тестами.
- Pattern library — пока нет реальных версионированных pattern definitions и проверки host transport.
- 300 presets — пока каждый preset не проходит manifest/resource resolution и playable smoke test; каталог из 300 JSON entries сам по себе не равен 300 готовым звукам.
- Preview — пока доказано, что audition не меняет активное состояние и не создаёт наложение голосов.
- 60 skins — пока существуют 60 полных проверенных resource packs, а не только палитры.
- Multicore — пока не показан реальный throughput win на 2C/2T без audio-thread blocking.
- CPU/RAM цели — пока нет воспроизводимого профиля на целевой машине.
- FL Studio acceptance — только после реального ручного теста в FL Studio 20.

## 7. Правило для каждого следующего PR/commit

Каждый change-set должен содержать:
1. Причину и ожидаемый выигрыш.
2. Затронутые модули и контракты.
3. Конфликт-check по audio thread, ownership, state, MIDI и UI.
4. Тест, который падает до исправления и проходит после него.
5. Windows x64 CI result.
6. Честный статус: implemented / partially implemented / planned / manually verified.

Не объединять крупные несвязанные функции в один коммит. Не удалять работающий код или данные ради «очистки» без подтверждённого unused-аудита.

## 8. Ближайший инженерный приоритет

Первый следующий технический шаг — E1: провести call-graph audit реального audio path и устранить/зафиксировать ownership/reclamation risks. Затем E2 streaming stress. Не начинать тяжёлое физическое моделирование, 60 skin assets или расширение числа пресетов, пока эти основы не доказаны.

---
Документ является надстройкой над существующим 5.0 product/UI contract, а не заменой утверждённых пользовательских требований.
