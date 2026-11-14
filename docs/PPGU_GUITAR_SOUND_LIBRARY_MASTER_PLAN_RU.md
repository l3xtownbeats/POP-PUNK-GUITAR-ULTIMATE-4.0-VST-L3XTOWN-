
# PPGU GUITAR SOUND LIBRARY — MASTER PLAN (RU)
Дата фиксации: 14 ноября 2026
Статус требований: **APPROVED — ОБЯЗАТЕЛЬНО К РЕАЛИЗАЦИИ**
Продукт: POP PUNK GUITAR ULTIMATE 4.0 VST

## 1. Цель
Создать максимально большую, качественную, разнообразную и юридически чистую библиотеку гитарных звуков PPGU. Библиотека включает самостоятельные **One-Shot** звуки, **Sample Loops**, multisample/instrument recordings, performance/articulation recordings и соответствующие factory presets. Количество элементов не имеет заранее установленного верхнего предела ни в общей библиотеке, ни в отдельных категориях.

Цель «максимально большая» означает постоянное расширение качественного каталога, а не наполнение числами пустыми, дублированными, плохо записанными или юридически неразрешёнными файлами.

## 2. Типы контента — раздельные сущности
### A. Guitar One-Shots
- одиночные ноты по всем реально записанным и доступным регистрам;
- отдельные струны/позиции/voicings, если source metadata это подтверждает;
- clean, edge-of-breakup, crunch, overdrive, high gain;
- open chords, power chords, octave notes, single-note leads;
- palm mute, muted/choked notes, staccato, accents, harmonics, slides, bends, tremolo и прочие артикуляции только при наличии корректных записей/mappings;
- electric и acoustic как разные, явно маркированные source families;
- velocity layers, round robins, release/tail samples и alternates — где они реально присутствуют.
One-shot должен иметь корректные root key/note range, sample rate, channels, trim/loop metadata, velocity/round-robin group, articulation, source/license и SHA-256.

### B. Guitar Sample Loops
- электрические и акустические ритмические loops;
- clean/crunch/drive/palm-muted/open-chord families;
- downstroke, alternate/down-up, eighth-note, sixteenth-note, gallop, syncopated, stop-start, half-time, double-time, ska offbeat и другие реально представленные groove families;
- muted verse → open chorus, verse/chorus/bridge/intro/outro performance variations;
- разные BPM, time signatures, bar lengths, rhythmic densities и musical feels;
- dry и processed варианты с явным описанием обработки;
- seamless loop boundaries, exact tempo/length/beat metadata, root key/chord metadata только когда музыкально применимо и достоверно.
Loops не должны автоматически притворяться MIDI-patterns или заменять ното-ориентированный инструментальный движок. Loop playback и MIDI-driven performance — разные типы контента/поведения.

### C. Multisamples / Performance Sets
- ноты, регистры, струны/позиции, velocity layers, round robins, articulations, releases и tails;
- согласованные наборы записей для playable instrument engine;
- отдельные source profiles для electric/acoustic и соответствующих guitar families;
- только валидированные mapping tables и реальные source takes.

### D. Presets
- factory presets MELODY (single-note instrument performance);
- factory presets PATTERN (ритмическое исполнение по утверждённому MIDI/Pattern contract);
- PRESET/GUITAR IDENTITY, GENRE, ARTIST/BAND/PUNK GROUP, SONG/SECTION;
- STYLE, articulation, technique, rhythmic category, pattern/groove, variations;
- tone/sound-character/source/performance profiles и другие утверждённые browser entities.
Каталог всех перечисленных типов не имеет верхнего лимита. 300 (150 MELODY + 150 PATTERN) — только прежний минимум-контрольная точка, не потолок.

## 3. Отдельные библиотеки и единый каталог
- Единый PPGU Guitar SoundBank хранит фактические аудио-активы и их метаданные.
- One-Shots, Loops, Multisamples/Performance Sets и Presets — раздельные типы/коллекции, с cross-reference по стабильным IDs.
- MELODY и PATTERN могут ссылаться на совместимые source assets, но не смешиваются как типы исполнения.
- Не подменять sample loop, MIDI pattern, one-shot и multisample одним типом записи.
- Factory content и user-imported content физически/логически разделены; пользовательские коммерческие библиотеки разрешено использовать локально в рамках их лицензий, но нельзя автоматически включать/распространять их исходные samples в factory build.
- SoundBank только гитарный: не добавлять drum-only, bass-only, vocal-only, full-mix контент как гитарные активы.

## 4. Масштабирование без потолка
- Нет фиксированного максимума по числу файлов, One-Shots, Loops, multisamples, presets, групп, жанров, песен, секций, стилей, артикуляций, техник, groove/pattern variations.
- Не зашивать лимиты в схемы, валидаторы, UI, поиск, фильтры, импортеры или build scripts.
- Stable IDs, schema versioning, backward compatibility, manifest/index, metadata-first scan, lazy loading, virtualized lists, bounded cache and worker-based decoding.
- UI показывает фактическое количество элементов и не скрывает записи из-за произвольной квоты.
- Производительность подтверждается тестами на больших наборах; отсутствие числового лимита в схеме само по себе не доказывает масштабируемость.
- Audio callback не выполняет файловый/сетевой I/O, декодирование WAV, SFZ parsing, импорт, hashing, blocking wait/lock или неограниченные allocations.

## 5. Качество и валидация каждого актива
До допуска в factory bank каждый файл проходит автоматические проверки:
1. физическое наличие, доступность и читаемость;
2. SHA-256, уникальность и обнаружение exact/near duplicates;
3. формат, sample rate, bit depth, channels, duration, file integrity;
4. clipping/peak, silence, DC offset, excessive noise и повреждённые границы;
5. loop seam/crossfade, точность длительности/BPM/bar count для loops;
6. root note, key range, velocity layer, round robin, articulation и mapping consistency для one-shot/multisample;
7. корректность метаданных, стабильного asset ID, source ID, provenance;
8. лицензия, право factory bundling/redistribution и attribution obligations;
9. playable/loadable integration и соответствие реальному engine capability;
10. preset references integrity и отсутствие broken links.
Пороговые значения аудиокачества должны быть заданы в версионируемых правилах валидатора и калиброваться на тестовых наборах; нельзя придумывать универсальные критерии, которые автоматически отбрасывают художественно допустимые тихие/длинные/эффектные звуки.

## 6. Автоматический цикл исправлений — APPROVED
Пользователь утвердил автоматическое исправление недочётов, багов, конфликтов и ошибок в ходе разработки каждого пункта, без постоянных уведомлений и повторного запроса разрешения на рутинные действия.

Для каждой задачи/модуля автоматически выполнять:
- обнаружение compile/configuration errors, warnings, static-analysis findings, failing tests, broken references, invalid metadata, missing assets, schema mismatches, duplicate IDs, license/provenance omissions, UI binding errors, audio-thread violations, performance regressions и documentation drift;
- классификацию severity, scope, root cause и regression risk;
- автоматическое исправление локальных, однозначных, обратимых и не меняющих утверждённое поведение проблем;
- запуск затронутых тестов, затем релевантного полного набора regression tests;
- проверку diff, повторную загрузку/проверку журналов и фиксацию evidence;
- запись причины, исправления, source paths, commit SHA, test commands/results и остаточных рисков в INFO и CODE;
- продолжение разработки следующего пункта только после закрытия найденных локальных дефектов или явного документирования реального блокера.
Не создавать отдельные уведомления пользователю по каждой рутинной ошибке/починке; вести полный внутренний журнал и давать сводный отчёт на контрольных точках/по запросу.

### Границы автономного исправления
Автоматически исправлять без отдельного согласования можно только однозначные non-breaking defects, которые не меняют утверждённые требования, пользовательский функционал, APVTS IDs/semantics, MIDI contracts, DSP behavior, sampler lifecycle, preset schema, asset/license permissions, security boundaries, UI layout/interaction commitments or backwards compatibility.

Если для исправления нужно изменить утверждённое поведение, удалить/заменить утверждённую функцию, изменить публичный contract, выбрать между противоречащими пользовательскими решениями, принять юридически сомнительную лицензию, использовать сомнительные/непроверенные источники, выполнить необратимое действие или затронуть данные пользователя — остановить только эту затронутую часть и запросить одно конкретное решение. Не блокировать независимые безопасные задачи.

Никакой скрытой фиктивной отметки PASS. Если тест/сборка не может быть запущена, записать NOT RUN и точную причину. Если исправление не подтверждено тестом, не объявлять его закрытым.

## 7. Законность и происхождение
- В factory bundle включать только собственные записи, public-domain/CC assets с подходящими условиями либо samples с явным письменным правом на включение/распространение в VST.
- Royalty-free use in finished music не равно праву распространять исходные samples/multisamples внутри VST.
- Не извлекать/не включать samples, stems, loops, presets, riffs, transcriptions, UI/artwork из Session Guitarist, Mike Naran, Ultimate Punk Rock Guitars Soundkit или иных коммерческих продуктов без соответствующих прав.
- Сохранять source URL/vendor, creator, license text/version, acquisition date, permitted use, redistribution status, attribution, proof/reference and SHA-256.
- Пользовательский локальный импорт коммерческой библиотеки не запрещать только потому, что она не разрешена для factory redistribution; права использования определяются её собственной лицензией.

## 8. Текущий честный статус
На момент аудита репозитория main HEAD, registry bootstrap file `assets/sound_hunter/registry.json` содержит assetCount=0 и sourceCount=0 и прямо указывает, что это schema/bootstrap marker, а фактический runtime inventory генерируется только из реально присутствующих файлов под Samples/. Поэтому утверждённая цель большой библиотеки зафиксирована, но наличие большой готовой библиотеки не заявляется. Следующий этап — создать sourcing pipeline, asset schema/registry, quality+license validators, staging layout и наполнять его только реальными проверенными файлами с доказуемыми правами.

## 9. Запреты на ложные статусы
APPROVED = требование пользователя; DOCUMENTED = внесено в документы; VERIFIED = повторно считано; IMPLEMENTED = функция/процесс реально закодированы; TESTED = реально выполнены тесты; LICENSE-CLEARED = права проверены; FACTORY-READY = актив валидирован и может распространяться; COMPLETE = весь обязательный scope реализован и проверен. Эти статусы не взаимозаменяемы.


## 10. Размер SAMPLES и транспортная упаковка — REV.18 SUPERSEDES PREVIOUS TARGET

**У SAMPLES нет искусственного целевого размера или верхнего лимита.** Не ограничивать library 25–30 GB, 40 GB, per-run cap или иным внутренним числом.

- В SAMPLES входят One-Shots, Sample Loops, multisamples/performance sets, mappings/SFZ, MIDI patterns, presets и сопровождающие metadata/resources; в отчёте отдельно показывать аудио payload, пресеты/маппинги, metadata, factory/user split и package overhead.
- Сохранять все уникальные законные и проверенные файлы. SHA-256 deduplication предотвращает повторный импорт, но не удаляет существующие файлы. Никаких overwrite/move/cleanup/automatic delete.
- Если GitHub Actions, artifact storage или передача не принимают крупный архив/файл, делить его на части с SHA-256 manifest, порядком сборки и проверкой полного восстановленного комплекта. Ограничение транспорта не является ограничением библиотеки.
- Реальные ограничения runner disk, quota, artifact size и времени показывать как инфраструктурные; продолжать следующими пакетами/запусками без потери контента. Не заявлять неограниченную физическую ёмкость диска.
- Не набирать объём дублями, пустыми файлами или искусственным перекодированием. Качество, уникальность, права, mapping и аудио-проверка обязательны.
- До фактического inventory и измерения никакой конкретный размер библиотеки не объявляется достигнутым.


## 11. Синхронизация INFO и CODE и единое расположение журналов

- Все журналы разработки и индексы должны быть организованы в одном согласованном месте репозитория после аудита ссылок и зависимостей; не создавать новые разрозненные журналы.
- INFO и CODE — два отдельных, синхронных файла: INFO содержит решения/требования/статусы/проверки; CODE содержит карту исходников и историю реальных изменений кода, paths/symbols/commit SHA/test evidence.
- Полный исходный код остаётся только в соответствующих .cpp/.h; не копировать его целиком в INFO/CODE.
- При каждом изменении кода обновлять INFO и CODE в рамках одной рабочей операции и указывать одинаковый commit SHA, затронутые файлы, тестовые команды и фактические результаты. После записи повторно читать оба файла и проверять соответствие.
- Не перемещать/удалять существующие файлы до аудита всех ссылок и сохранения истории. До выполнения физической консолидации текущие журналы остаются в своих действующих путях.
- Обновлять записи по мере разработки, а не задним числом; не объявлять синхронизацию завершённой, пока обе записи не проверены.



## 12. Обязательный стандарт качества звуковой библиотеки: запрет пластикового тембра

**APPROVED / MANDATORY.** В библиотеку и фабричный каталог нельзя принимать пластиковые, искусственные, дешёвые или нереалистично синтетические гитарные звуки. Критерий относится к чистым, обработанным, акустическим и электрическим источникам. Для каждого нового источника требуется тестовый пресет; для каждого пресета — проверка назначенного реального источника. Источник без соответствующего тестового пресета не считается полностью проверенным, а пресет без проверки реального источника не считается READY.

Допустимы оригинальные DRY-записи, естественно совпадающие с целевым характером (NATIVE TONE MATCH), качественно обработанные записи с уже подходящим характером (PROCESSED SOURCE MATCH) и источники, которым нужный характер придаётся внутри PPGU (WET). Не добавлять лишние эффекты и не дублировать обработку исходной записи. Минимальная DSP-цепь может экономить CPU, но не является оправданием менее реалистичного звучания.

Слуховой контроль обязателен: диапазон нот, velocity, атака/затухание/release, артикуляции, аккорды и переходы, отсутствие артефактов и неестественной одинаковости нот. Любой источник, не прошедший эту оценку, отклоняется или отправляется на исправление; не включать его в коммерческий factory bundle до повторной проверки.


## REV.18 scope synchronization

SAMPLES — без искусственного лимита размера. Factory minimum — 500 READY MELODY + 500 READY PATTERN. 82 Travis Barker-produced-album references входят в общий каталог и учитываются один раз в соответствующих artist/band profiles. Полная нормативная запись: `PPGU_REV18_USER_APPROVED_SUPERSEDING_REQUIREMENTS_2026-11-14_RU.md`.
