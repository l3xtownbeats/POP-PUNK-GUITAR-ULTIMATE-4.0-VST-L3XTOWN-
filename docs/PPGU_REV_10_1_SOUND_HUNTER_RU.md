# PPGU REV 10.1 — SOUND HUNTER & PRESET CURATION
Дата фиксации: 11 ноября 2026  
Статус: PRODUCT / SYSTEM DESIGN — DESIGNED, НЕ IMPLEMENTED  
Область: Guitar Edition, SoundBank S6 и Preset Browser. Документ дополняет REV 10.0 UNIFIED и не заменяет его.

## 0. Приоритеты и статус
При конфликте действует порядок REV 10.0 §0.1: Safety > Realtime > Sound > Resources > UX. Этот документ фиксирует требования и архитектурные ограничения. Наличие спецификации не означает наличие работающего поискового сервиса, лицензированного контента или новых пресетов.

Метки статуса: PLANNED, DESIGNED, IMPLEMENTED, BUILT, TESTED, HOST-VERIFIED, BLOCKED. В отчётах для каждого элемента отдельно указывать требуемое состояние и доказательство.

## H1. Назначение
SOUND HUNTER — поисково-кураторская подсистема для обнаружения бесплатных гитарных one-shots, single notes, power chords, chugs, harmonics, loops и preset/sample packs, их проверки, классификации и передачи в существующий SoundBank S6 и Preset Browser.

Целевые стилевые профили:
- PP-ROOT — яркие чистые ритм-гитары, открытые аккорды, мелодичные lead.
- PP-NEO2000 — palm-mute, chugs, power chords, двойные ритмы.
- PP-MODERN — high-gain, плотный стек, широкие слои.
- PP-EMO — арпеджио и мелодичные lead с чистым/лёгким overdrive.

Это внутренние метки кураторства, не названия артистов, брендов или UI-имитации. Встроенные источники и factory content не должны создавать впечатление официальной связи с реальными артистами.

## H2. Лицензионная политика
### GREEN — допустим кандидат на AUTO ADD
CC0 / подтверждённый Public Domain и прямой доступ к файлу. Перечисленные каталоги (например, Freesound CC0, OpenGameArt с явной CC0-меткой, Creazilla audio, archive.org) — только места поиска, а не автоматическое доказательство лицензии. Каждый отдельный asset обязан иметь проверяемую лицензию и доказательство на дату импорта.

AUTO ADD разрешён только если одновременно:
1. лицензия конкретного файла подтверждена как CC0/PD и допускает коммерческое использование и распространение;
2. получен сам файл по прямой ссылке, а не HTML-страница;
3. источник, asset ID, URL, дата проверки, license URL/текст и SHA-256 сохранены;
4. файл прошёл технический screening и guitar-content gate;
5. нет copyright-suspect/ручного review флага.

### YELLOW — только ручной импорт
Бесплатные каталоги, требующие аккаунт/регистрацию, или лицензии, где требуется дополнительная проверка. Не AUTO ADD. Статус REQUIRES MANUAL IMPORT; Import Guitar показывает источник, точную лицензию и причину ручного шага. Пользователь подтверждает импорт и самостоятельно соблюдает условия сайта.

### RED — исключить
Платные/подписочные наборы, некоммерческие/ограничительные лицензии, royalty-share/royalty-split условия, запрет на коммерческое использование или перераспространение, обманные/рекламные страницы как единственный путь к файлу, а также неясные/несовместимые права. RED не может быть переопределён обычным curator action.

Никакой домен или категория источника не получает безусловный GREEN status. Лицензия проверяется на уровне каждого asset. Лицензии с attribution, share-alike или иными условиями не считать GREEN автоматически: отдельная юридическая проверка и решение по политике дистрибутива.

## H3. Deep Search — единый конвейер S6.5
Режим DEEP SEARCH расширяет существующий DiscoveryEngine; второй импортный pipeline не создаётся.

Логические этапы:
1. SOURCE ADAPTERS — разрешённые API/каталоги и репозитории; соблюдение robots/API terms, rate limits, авторизации и условий скачивания.
2. QUERY SYNTHESIS — словарь RU/EN по типам гитарных звуков и жанрам; поисковые формулировки описывают звук, не запрашивают копии конкретных защищённых композиций.
3. PAGINATION — обход доступных страниц до исчерпания разрешённого API/каталога, с cursor/checkpoint, лимитами и дедупликацией.
4. SCREEN — определить, что получен файл поддерживаемого формата; HTML/страница/непрямой download помечается REQUIRES MANUAL IMPORT.
5. DOWNLOAD — только разрешённый прямой download в quarantine/temp area, с лимитами размера и безопасной обработкой ошибок.
6. VERIFY — лицензия, источник, SHA-256, формат, целостность, malware/file validation и guitar-only classification.
7. CONVERT — существующий формат S6.5 (включая предусмотренный им .pgp, если этот формат действительно поддерживается кодом); не изобретать второй формат/конвертер.
8. INDEX — существующий индекс SoundBank, включая profile metadata.
9. COMMIT — атомарная публикация только после всех gates; никакого частичного AUTO ADD.

Сетевой поиск, HTTP, файловый обход, скачивание, хэширование и декодирование выполняются вне audio callback и вне realtime voice path. Предпочтительная граница — отдельный worker/service или companion curator process. Сам VST не должен самостоятельно сканировать интернет, блокировать UI/audio или запускать неконтролируемые фоновые сетевые задачи. Плагин получает уже проверенные локальные assets/индекс через существующий Import/Discovery интерфейс.

Deep Search обязан иметь pause/cancel, checkpoint/resume, bounded concurrency, download-size limits, timeouts, retry/backoff, disk quota и понятные диагностические статусы. Никаких бесконечных обходов и неконтролируемых параллельных скачиваний.

## H4. Style Classifier
Каждый guitar-confirmed asset получает один или несколько профилей PP-ROOT, PP-NEO2000, PP-MODERN, PP-EMO, либо UNCATEGORIZED. Классификация может использовать метаданные, аудио-признаки и ручную коррекцию. Автоматическая оценка не является доказательством жанра или лицензии.

UNCATEGORIZED остаётся в общем Guitar SoundBank, но не показывается в профильных разделах. Bass/voice/drums/other assets не попадают в Guitar SoundBank. Расширение классификатора не должно ослаблять guitar-only gate: сомнительный файл получает GUITAR_REVIEW, а не GUITAR_CONFIRMED.

## H5. Quality & rights screening
Технические screening targets:
- SNR >= 40 dB;
- отсутствие клиппинга, DC offset, повреждённых/оборванных файлов и недопустимых шумов;
- присутствие пригодного атака-транзиента для one-shot;
- для loops — достоверный tempo marker, tempo-sync metadata и отсутствие искусственной тишины/артефактов защиты.

Пороговые значения должны быть измеримыми и тестируемыми; метод измерения, окно анализа и допустимые погрешности документируются. Технический pass не означает музыкальную пригодность и не доказывает copyright clearance.

COPYRIGHT SUSPECT: если луп/семпл предположительно содержит узнаваемую защищённую композицию или рифф, asset блокируется от SoundBank commit до ручной проверки. Автоматическое распознавание — только triage, не окончательное юридическое решение. GOLD (A) может присваиваться только после автоматического screening и обязательного человеческого прослушивания/кураторского одобрения. Для спорного материала GOLD запрещён.

## H6. Интеграция с S6
- DEEP SEARCH и PROFILE CLASSIFY — режимы/этапы существующего DiscoveryEngine S6.5, не параллельный pipeline.
- IMPORT GUITAR и AUTO DISCOVER остаются пользовательскими точками входа.
- Profile, license, source, hash, quality, curator status и provenance хранятся в согласованной схеме индекса S6.3; изменение схемы требует versioning и migration tests.
- Curated Stack — композиция подтверждённых слоёв: Rhythm L, Rhythm R, Sub, Air, optional Bass. Каждый слой ссылается на конкретный asset и его provenance. Bass layer не включается в Guitar SoundBank автоматически и требует отдельного подтверждённого разрешения/политики продукта.
- Дедупликация по content hash, а не только по имени/URL; разные license claims для одинаковых байтов сохраняются как provenance evidence.

## H7. Preset Browser — честное наполнение
Целевые количества первого релиза:
- MELODY: не менее 30 playable presets.
- PATTERN: не менее 50 playable presets.
- PUNK GROUP: не менее 40 playable presets.

Цель не является текущим количеством. В счётчик входят только presets, которые ссылаются на доступные и проверенные assets, корректный Stack/pattern data и реально поддерживаемый playback path. Дубликаты и placeholders не засчитываются.

При недостижении цели UI показывает фактическое N/target и состояние «заполняется Sound Hunter'ом». Пустой раздел не должен запускать несуществующее воспроизведение; он показывает понятное объяснение и фильтрованный AUTO DISCOVER. PATTERN presets доступны для игры только при наличии реализованного и протестированного tempo-sync/pattern playback; наличие loop WAV само по себе не доказывает поддержку паттерна.

## H8. Curator mode
Curator — командный инструмент, не обязательная панель обычного пользователя. Очередь кандидатов содержит asset/source/license/hash/quality/profile и превью. Действия: GOLD, KEEP, REJECT, ручная установка профиля и обязательное поле причины для override/reject. RED нельзя одобрить. YELLOW требует ручного импорта и подтверждения пользователя; GOLD не отменяет лицензионный gate.

Curator собирает Stacks и presets только из approved assets. Attribution Report генерируется из фактических provenance records; каждый GOLD asset обязан иметь source, asset ID, license evidence, retrieval date и checksum. Отсутствие любого обязательного поля блокирует release packaging.

## H9. Acceptance tests
1. DEEP SEARCH минимум по трём тестовым GREEN adapters: пагинация, checkpoint/cancel, индексная консистентность и дедупликация по SHA-256.
2. GREEN/YELLOW/RED policy matrix: YELLOW и RED не попадают в AUTO ADD; RED нельзя переопределить.
3. Прямой файл vs HTML landing page: HTML получает REQUIRES MANUAL IMPORT.
4. Copyright suspect: asset не коммитится до ручного review; автоматическая классификация не может выставить GOLD.
5. PROFILE CLASSIFY: UNCATEGORIZED не отображается в профильных разделах; сомнительный guitar asset не проходит guitar-confirmed gate.
6. Browser counts считают только playable, validated presets; N/target соответствует индексу.
7. Attribution completeness: каждый GOLD asset полностью представлен в отчёте.
8. Failure tests: rate limit, timeout, corrupted file, oversized file, hash mismatch, duplicate, disk quota, cancellation и partial download.
9. Realtime/resource tests: во время deep search audio callback не выполняет network/file I/O, не блокируется; CPU/RAM/threads/disk usage измеряются отдельно и сопоставляются с лимитами REV 10.0 §1. Значения лимитов брать из мастер-спецификации, не придумывать здесь.
10. Integration: импорт идёт через один pipeline S6.5; повторный импорт не создаёт второй asset или повреждённый индекс.

## H10. Порядок реализации
1. Зафиксировать режимы DEEP SEARCH и PROFILE CLASSIFY в существующем DiscoveryEngine contract.
2. Реализовать worker/service, StyleClassifier и CuratorPanel поэтапно, с тестами и bounded resource policy.
3. Проверить GREEN candidates на дату релиза и подготовить отчёт находок с карточками: название, источник, точная лицензия, SHA-256, техническое качество, профиль, preview и решение куратора.
4. Реализовать H9 acceptance tests и отчёт верификации по REV 10.0 §9.9.
5. Не включать контент в дистрибутив до юридической/лицензионной проверки и прохождения полного pipeline.

## Текущий статус на момент фиксации
- REV 10.1 требования: DESIGNED (этот документ).
- DiscoveryEngine DEEP SEARCH: PLANNED; наличие production DiscoveryEngine в текущем HEAD не подтверждено.
- StyleClassifier: PLANNED.
- DeepSearchWorker: PLANNED.
- CuratorPanel: PLANNED.
- GREEN source registry / asset-level license verification: PLANNED.
- Автоматическое пополнение SoundBank: PLANNED, не реализовано.
- Разделы MELODY/PATTERN/PUNK GROUP и целевые количества: PRODUCT TARGETS, не подтверждены как достигнутые.
- Набор лицензированных SOUND HUNTER assets и attribution report: отсутствует как подтверждённый результат этого этапа.
- Сборка, CTest, pluginval и FL Studio 20 host verification этим документом не запускались.


## H11. Расширенный охват: только гитарный контент (2026-11-12)

SOUND HUNTER ищет и каталогизирует исключительно guitar assets:
- Электрогитара: clean, DI, crunch, overdrive, rock drive, high-gain, palm-mute, chugs, power chords, octaves, harmonics, slides, bends, pick attacks, string noises.
- Акустика: steel-string, nylon, strummed chords, fingerpicked notes, arpeggios, harmonics и артикуляции.
- Гитарные музыкальные материалы: chord progressions, riffs, lead phrases, rhythm patterns, double-time patterns, palm-muted patterns, loops, fills, turnarounds, transitions, endings, guitar stems.
- Гитарные FX-записи: amp/cab captures, feedback, reverse guitar, flanger/delay-modulated guitar phrases, texture/noise swells.
- Форматы: WAV, AIFF, FLAC, OGG для предпрослушивания; SFZ/SF2 и другие sampler maps — только после проверки поддержки конвертации/воспроизведения текущим S6.
- Free, promo, demo, starter packs и отдельные one-shots рассматриваются как источники поиска. Drum-only kits, drum loops/fills, bass, vocals и полные song mixes исключены из Guitar SoundBank.

### Приоритетные source candidates

| Источник | Гитарный материал | Лицензионный/технический статус |
|---|---|---|
| Meta Open Sound — Steel Guitar | 375 multisamples, SFZ, WAV 24-bit/48 kHz, 3 velocity layers | CC0 1.0; приоритет №1 |
| Meta Open Sound — Nylon Guitar | 348 multisamples, SFZ, WAV 24-bit/48 kHz, 3 dynamic layers | CC0 1.0; приоритет №1 |
| Karoryfer free libraries — Emilyguitar, Shinyguitar, Black And Green Guitars | Электрогитары, артикулляции и многосэмпловые банки | Авторская free-samples page указывает CC0; проверять LICENSE каждого фактически скачанного архива |
| FreePats FSBS Electric Guitar Clean | Clean/jazz-clean, SFZ WAV/FLAC/SF2 | CC0 1.0 указана на страницах конкретных банков |
| FreePats FSBS Electric Guitar Distorted | Distorted/overdriven, SFZ WAV/FLAC/SF2 | CC0 1.0 указана на страницах конкретных банков |
| Selekt/Selektaudio | Отдельные гитарные one-shots/loops | Только asset-level clearance и подтверждённая лицензия конкретного файла |
| Pulse Open Loops | Гитарные лупы и stems | Проверять каждый asset; CC-BY требует корректной атрибуции и отдельного разрешения на bundling |
| SoundShockAudio и официальные промо/free packs авторов | Гитарные one-shots, riffs, loops, fills | Discovery only; YELLOW до подтверждения прав на redistribution исходных файлов |
| MusicRadar SampleRadar | Гитарные loops и hits | Не включать raw samples в SoundBank без отдельного явного разрешения |
| Splice/subscription packs | Коммерческие гитарные loops/stems | Исключены без отдельной лицензии на embedding/redistribution исходных файлов |

### Правила отбора

1. Искать максимально широко, но добавлять в библиотеку только гитарные файлы. Наличие гитар в названии набора недостаточно: каждый файл классифицируется отдельно.
2. Free/promo/royalty-free означает лишь повод рассмотреть материал, но не автоматически разрешает распространять исходные WAV внутри коммерческого VST3.
3. CC0/PD с точным доказательством — приоритет для автоматического включения. CC-BY — только при наличии полного attribution record и совместимой модели распространения. Неясные условия — YELLOW/manual review; запретительные условия — RED.
4. Для каждого файла сохранять источник, asset ID, точную лицензию и её evidence, URL, дату получения, формат, размер и SHA-256.
5. Лупы и прогрессии индексировать отдельно от multisample-инструментов: tempo, key, meter, bars, loop points, tuning и profile. Не считать луп playable preset без подтверждённого playback path.
6. FX-плагины как таковые не являются сэмплами; в SoundBank попадают только гитарные аудиозаписи с соответствующими эффектами. Flanger, delay, crunch и rock drive как DSP-пресеты остаются отдельной задачей движка.
7. Сетевой поиск, скачивание, распаковка, хэширование и анализ всегда вне audio callback.

### Подтверждённые официальные сведения

Meta Open Sound README указывает CC0 для Steel/Nylon, размеры библиотек 375/348 samples, WAV ZIP 24-bit/48 kHz, SFZ maps и SHA256SUMS. Karoryfer official free-samples page заявляет CC0 для бесплатных библиотек, с оговоркой о старых загрузках и исключении Marie Ork. FreePats Clean и Distorted Electric Guitar pages явно указывают CC0 1.0 и публикуют SFZ WAV/FLAC/SF2 варианты.

### Граница текущего этапа

`assets/sound_hunter/registry.json` пока пуст. Ни один новый внешний архив не объявляется скачанным, хэшированным, прослушанным, импортированным или готовым к воспроизведению в этом изменении. Реальный импорт выполняется через единый S6.5 pipeline после всех H2/H5/H6 gates.