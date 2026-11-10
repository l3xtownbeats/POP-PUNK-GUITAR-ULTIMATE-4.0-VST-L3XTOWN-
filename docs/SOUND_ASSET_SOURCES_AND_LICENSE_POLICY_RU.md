# Sound Asset Sources and License Policy — PPGU 4.0

Date: 2026-11-10
Status: research shortlist and integration policy. No third-party audio files are bundled by this document.

## Product policy

The user library must not have an artificial product-level cap on number of BANKs, presets, samples, or total library size. Actual use remains bounded by available disk space, filesystem limits, address space, RAM, CPU, and host stability. The engine must scale through lazy/background loading, indexing, cache, and bounded voice/memory budgets rather than loading every installed sample into RAM.

## Candidate public sources

### Meta Open Sound
- Repository: https://github.com/facebookincubator/Meta-open-sound
- Relevant instrument collections: Meta Nylon Guitar (348 samples) and Meta Steel Guitar (375 samples).
- Published collection information: WAV 24-bit / 48 kHz releases, SFZ maps, sample design specs, manifests, checksums.
- The two instrument collections are identified as CC0 1.0; confirm each downloaded release's own LICENSE/manifest before packaging.
- Oculus Audio Pack is CC BY 4.0 and is primarily sound effects, not a guitar instrument. If used, preserve exact attribution; do not mix its license status with CC0 instruments.

### FreePats
- Website: https://freepats.zenvoid.org/
- Candidate: FSBS Electric Guitar Clean, including SFZ WAV and FLAC variants.
- The cited page identifies the bank as CC0 1.0.
- Before bundling, archive the exact bank page, LICENSE/README, version/date, download URL and checksum. Verify that the particular downloadable package matches the CC0 statement.

### SFZ Instruments catalogue
- Catalogue: https://sfzinstruments.github.io/guitars/
- Discovery index only; entries have different licenses (CC0, MIT, custom, commercial, etc.).
- Do not treat catalogue inclusion or a free download as redistribution permission.
- Per-library allowlist: accept CC0 / public domain and permissive licenses after reviewing full terms; CC BY only when attribution and other obligations can be met; exclude NC, ND when incompatible with required modifications, no-redistribution, unclear/custom restrictive, and commercial assets unless explicit written permission covers bundling and redistribution.

### Stargate Sample Pack
- Repository: https://github.com/stargatedaw/stargate-sample-pack
- Project describes its sample pack as public domain / unrestricted redistribution.
- Treat as candidate pending review of repository LICENSE, individual included files, contributor provenance and actual audio contents. Do not infer rights beyond the actual license and provenance.

### Freesound and similar per-file archives
- Each file must be reviewed individually.
- CC0 may be eligible; CC BY may be eligible with exact attribution; CC BY-SA requires compatibility review; CC BY-NC is not eligible for the commercial/distributable plugin bundle.
- User-imported files can be used locally by that user, but the plugin must not repackage or redistribute them without appropriate rights.

## License acceptance workflow

For every candidate asset/library:
1. Record canonical source URL, creator/rights holder, title, version, retrieval date and file list.
2. Store the complete license text or a stable copy/reference, SPDX identifier where valid, and exact attribution text.
3. Confirm rights cover commercial use, modification/conversion, embedding in a VST3, redistribution to end users, and redistribution as part of a sample library. These are separate questions.
4. Check third-party dependencies, embedded metadata, contributor terms and any exceptions.
5. Calculate SHA-256 per file and archive checksum.
6. Mark one of: Approved Bundled, Approved User-Import Only, Attribution Required, Review Required, Rejected.
7. Never ship Review Required / Rejected assets in factory bundles.
8. Recheck license and source before each release; keep provenance manifest with the shipped content.

## Formats and conversion

- Preserve original WAV whenever practical.
- Read SFZ as the primary mapping format where supplied.
- Support WAV PCM and IEEE float, mono/stereo, differing sample rates and bit depths through decoding/resampling.
- FLAC may be supported as an optional storage format if the decoder is included with compatible license; do not force lossy conversion.
- Optional derived cache is reproducible and keyed by source SHA-256 + cache format version.
- Conversion does not remove source license obligations.

## Unlimited-library architecture (not unlimited resource consumption)

- No hard-coded cap on total registered BANK count or aggregate library bytes.
- Index metadata and sample headers first; do not preload all audio.
- Background scan, hash, decode/cache and waveform generation.
- Bounded audio voice count and per-instance RAM budget, configurable by user/host constraints.
- Stream large samples or page them into preallocated buffers; audio callback must not perform disk I/O, decoding, heap allocation or blocking locks.
- Evict unused cache pages safely; pin active voices and prefetch likely next samples.
- Detect missing files, changed checksums, invalid paths and duplicate content.
- Keep factory, user-imported and licensed downloadable libraries in separate roots.
- Provide storage usage, cache usage and missing-file diagnostics; user can add as many libraries as their storage permits.

## Initial sound-library strategy

1. Electric guitar core: prioritize legally distributable electric-guitar multisamples (FreePats and individually approved SFZ catalogue entries).
2. Acoustic layer: Meta Steel Guitar and Meta Nylon Guitar as CC0 acoustic foundations.
3. Expand with original recordings commissioned/recorded for PPGU, with written assignment/license covering commercial redistribution.
4. User BANK import: permit locally used libraries regardless of whether they can be redistributed by PPGU; show license state and never silently include them in factory installer.
5. Create original pop-punk performance recordings (strums, palm mutes, power chords, chugs, slides, harmonics, articulations) under explicit PPGU distribution rights. Single-note samples alone do not provide authentic performance patterns.

## Status and acceptance

This file is a research and engineering policy, not proof that any source has been downloaded, legally cleared, integrated, or tested. Every bundled library requires a separate completed asset manifest and legal review. No audio files are added by this commit.


## Расширение звуковой библиотеки: production quality roadmap

### Целевой набор материалов

**Melody Electric — обязательный приоритет**
- 6-струнная электрогитара, стандартный строй EADGBE; chromatic multisampling с плотным покрытием нот в рабочем диапазоне.
- Минимум 4 velocity layers на основные ноты, 3–5 round-robin дублей на слой там, где доступны независимые исполнения.
- Отдельные артикуляции: picked/open sustain, palm mute (несколько степеней длины/плотности), dead note, release/noise; hammer-on, pull-off, slide и harmonic добавлять только при наличии записанного материала.
- Раздельные источники/профили Clean, Crunch и High Gain. Сырой DI и микрофонный/усилительный звук хранить как отдельные слои, если права на оба варианта позволяют.

**Acoustic — отдельный банк**
- Nylon и steel-string, не смешивать их в одну карту тембра.
- 24-bit/48 kHz исходники предпочтительны; проверка фазовой согласованности, хвостов, шума, root key, tuning и loop points.

**Pattern / Performance**
- Оригинальные записанные downstroke/upstroke, power chords, open chords, palm-muted eighth/sixteenth, arpeggios, stops и chord transitions.
- Метаданные: BPM, meter, chord/voicing, key, stroke direction, articulation, velocity, transient offset, tail length и license ID.
- Не подменять multisampled performance phrase простым MIDI-генератором; каталог пресетов должен честно маркировать, какие рисунки требуют записанных chord/strum assets.

### Критерии качества записи и приёмки

- 24-bit/48 kHz WAV как production master; не повышать искусственно sample rate у источника с меньшим разрешением.
- Одинаковая высота/строй по всему диапазону; настройка каждой ноты, удаление DC offset, аккуратные fades без срезания атаки и хвоста.
- Контроль клиппинга, щелчков, фазовых проблем, шума и нежелательных резонансов; исходник сохранять отдельно от обработанной версии.
- Velocity layers должны звучать как разные реальные исполнения, а не только разный gain; RR должны быть независимыми take, без повторяющихся копий.
- Слепое сравнение на одинаковой громкости в чистом, crunch и high-gain трактах; отдельно тестировать palm mute и быстрые повторные ноты.
- Нормализация громкости на уровне банка без чрезмерного peak normalization; сохранять естественную динамику атаки.

### Правовой шлюз

- Meta Open Sound Nylon/Steel (CC0) и FreePats FSBS Clean (CC0) — кандидаты для законного акустического/clean baseline только после сверки LICENSE, manifest и checksum конкретного архива.
- Для профессионального pop-punk electric high-gain материала предпочтителен заказ оригинальных сессий у гитариста/студии с письменной передачей прав на embedding, переработку, коммерческое распространение и включение исходных сэмплов в пакет плагина.
- Session Guitarist/Electric Sunburst, Kontakt, коммерческие soundkits и чужие рифф-паки не копировать и не извлекать. Покупка или royalty-free для музыки не даёт автоматически права на перепаковку сэмплов в VST.
- Ни один новый сторонний WAV не считается включённым в release до license review + manifest + SHA256 + provenance.

### Реальное состояние

Этот roadmap расширяет план качества и покрытия. Он сам по себе не означает, что новые электрогитарные multisamples уже записаны или добавлены в текущий 1.66 GB artifact. До появления подтверждённых файлов и manifest их нельзя рекламировать как готовую библиотеку.


## Приоритетный production-bank: современный melodic pop-punk / alternative pop-rock

Цель — широкий набор собственных тембров, подходящих для ярких мелодичных поп-панк партий и современного альтернативного поп-рока. Упоминания Avril Lavigne и Machine Gun Kelly — только ориентиры по общим музыкальным характеристикам; не копировать конкретные записи, партии, фирменные пресеты или вокально-гитарные stems.

### Электрогитары — первоочередной набор
1. Bright clean: звонкий верх, чистая атака, умеренный компресс.
2. Edge-of-breakup: лёгкий crunch для куплетов и динамичных арпеджио.
3. Tight rhythm crunch: плотные power chords, контролируемый низ, быстрый palm mute.
4. Modern pop-punk drive: насыщенный midrange, яркая атака, читаемость в плотном миксе.
5. High-gain lead: sustain без чрезмерной резкости, octave hooks и мелодические лиды.
6. Wide chorus layer: отдельные дубли/вариации для ширины; не имитировать стерео простым копированием моно.
7. Muted textures: несколько длин и динамических слоёв palm mute, dead strokes и приглушённые акценты.

### Акустика и гибридные слои
- Steel-string strums: down/up strokes, открытые аккорды и приглушённые удары.
- Nylon fingerpicked/arpeggio: отдельный мягкий слой для куплетов и переходов.
- Acoustic + electric hybrid: параллельные слои, доступные как отдельные пресеты/parts.
- Не выдавать одиночный акустический multisample за записанный аккордовый PATTERN.

### Минимальная структура factory sample profiles
Для каждого электрического профиля стремиться к хроматическому покрытию, нескольким velocity layers, независимым round robins, release/noise и раздельным артикуляциям. Начальные целевые значения: 4 velocity layers и 3 RR-дубля на ноту для основных sustained/picked зон; palm mute — отдельные короткие выборки с несколькими динамическими слоями. Это производственная цель, не утверждение о наличии уже записанных файлов.

Для PATTERN записывать/приобретать отдельные performance assets с метаданными: строй, аккорд/voicing, BPM или диапазон темпа, размер, down/up stroke, articulation, velocity, длительность и точка синхронизации. Нужны вариации куплет/припев, стопы, fills и переходы. Растягивание аудио должно сохранять транзиенты; при больших отклонениях темпа выбирать соседнюю запись или отказывать в запуске с понятным статусом.

### Производственный порядок
- P0: проверить и зафиксировать реальные лицензии, исходные архивы, manifest и SHA-256 для каждого factory asset.
- P1: закончить лицензированную Melody Electric основу; отдельно проверить mono compatibility, фазу, пики и loudness-matched DI/processed прослушивание.
- P2: записать/заказать оригинальные электрические артикуляции и performance наборы под PATTERN.
- P3: расширить acoustic/hybrid bank на основе проверенных CC0 материалов и собственных записей.
- P4: только после реального подключения файлов показывать профиль как Installed/Playable; каталожные определения должны оставаться помеченными как Targets/Needs Assets.

### Неизменяемые правила
- Не использовать извлечённые аудио из Avril Lavigne, Machine Gun Kelly, Blink-182, NI Session Guitarist или других коммерческих записей/библиотек.
- Не считать 300 preset definitions эквивалентом 300 готовых playable sounds.
- Не загружать всю библиотеку в RAM; индексировать метаданные, подгружать страницы в фоне, ограничивать активные голоса и кэш.
- Пользовательские импортированные файлы остаются локальными и не попадают в factory installer без отдельной проверки прав.
