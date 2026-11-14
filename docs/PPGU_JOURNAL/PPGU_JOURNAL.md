

### Samples — дополнительное утверждение пользователя (2026-11-14)

Пользователь утвердил расширение поиска источников: **искать ссылки и загружать любые форматы и любые файлы с гитарными записями/сэмплами, при условии что звук относится к настоящей записанной/исполненной гитаре; искусственные, синтетические и моделируемые гитарные источники исключать из этого класса поиска.**

Правила остаются: SOURCE & LICENSE REVIEW до импорта; не копировать/распространять материалы без соответствующих прав; сохранять provenance, лицензию, формат, размер, число файлов, SHA-256 и статус; additive-only, без удаления/перезаписи; лимит Samples 25 GB; оригиналы не трогать; ANGELA Engine NON-REGRESSION; существенные операции автоматизации подлежат пользовательскому контролю согласно глобальной политике.

**Статус этой записи:** требование утверждено и зафиксировано. Фактический поиск/скачивание/импорт этим коммитом не выполнялись.


### Sound Hunter — initial real-recorded guitar acquisition implementation (2026-11-14)

Пользователь дал прямое разрешение реализовать утверждённый процесс поиска и загрузки настоящих гитарных записей. Добавлены:

- `SoundHunter/real_guitar_sources.json` — начальный каталог 11 источников Meta Open Sound, FreePats и Karoryfer с классом `REAL_RECORDED`, репозиториями, ожидаемой лицензией и примечаниями.
- `SoundHunter/sound_hunter_real_guitars.py` — Python-импортёр на стандартной библиотеке: проверка исходного license file, загрузка репозиториев и release WAV архивов, обработка Git LFS pointer objects с проверкой размера/SHA-256, поддержка распространённых аудио/сэмплерных форматов, дедупликация по SHA-256, уникальные additive-only пути, per-run registry.
- `.github/workflows/sound-hunter-real-guitar.yml` — отдельный ручной workflow_dispatch, который создаёт импорт и публикует артефакт `PPGU-REAL-GUITAR-SAMPLES-25GB-MAX`; он не коммитит бинарные сэмплы в Git и не меняет VST3 workflow.
- `SoundHunter/README.md` — источники и инструкции запуска.

Защиты: только каталогизированные реальные записи; ожидаемая лицензия CC0-1.0 сверяется с фактическим LICENSE внутри загруженного репозитория; максимум 25 GB; существующие файлы не удаляются/не заменяются; каждый импорт в уникальный run-id; хеши проверяются; сомнительные/непрошедшие проверку ресурсы останавливают импорт.

**Ограничения статуса:** добавлен initial curated source set, не exhaustive Internet crawler. Сам workflow ещё не запускался; скачивание/импорт аудио не подтверждены. Плейбек через ANGELA Engine, runtime scan roots и интеграция в VST3 package не подтверждены. Синтаксическая проверка/CTest/pluginval/FL Studio host validation не выполнялись. Артефакт требует ручного запуска через GitHub Actions → Sound Hunter — Real Guitar Samples → Run workflow.


#### Дополнительная проверка реализации

Перед фиксацией проверены GitHub repository metadata и фактические LICENSE-файлы для всех девяти репозиториев FreePats/Karoryfer из каталога: в каждом выбранном файле обнаружен текст CC0 1.0 Universal. Для FreePats Spanish Classical Guitar исправлено имя license-файла на `LICENSE`. Проверены названия Meta Steel/Nylon release assets; каталог указывает на WAV assets. Добавлена обработка Git LFS pointer downloads с проверкой размера и SHA-256, а Meta OGG-предпрослушивания фильтруются по конкретной гитарной коллекции, чтобы не ошибочно атрибутировать их другому источнику. GitHub workflow ограничен read-only permission для contents.


### Лимит объёма: ручной импорт и Sound Hunter (2026-11-14)

Пользователь прямо утвердил и поручил зафиксировать следующее разделение лимита:

- **Ручной импорт** через `Import Guitar` и `Samples → Import Sample Bank`: лимит 25 GB не применяется. Не вводить вместо него другой искусственный лимит объёма на ручной импорт.
- **Sound Hunter / скачивание из интернета:** лимит 25 GB действует только для текущего процесса скачивания/импорта Sound Hunter, как временное ограничение этой загрузки. Он не является общим лимитом размера пользовательской библиотеки Samples и не распространяется на ручной импорт.
- Оригиналы, существующие файлы и папки не удалять, не перемещать и не перезаписывать; импортировать добавочно в новые уникальные пути.
- Сохранить проверки целостности, SHA-256, дедупликацию, проверку лицензий и прав на использование/распространение, защиту от небезопасных архивных путей и требования ANGELA Engine NON-REGRESSION. Отмена лимита размера не отменяет эти проверки и не означает автоматическое разрешение лицензии.
- Если для ручного импорта требуется свободное место на диске, показывать фактическую ошибку/предупреждение о нехватке места, а не искусственно ограничивать импорт правилом 25 GB.

**Статус:** требование утверждено и зафиксировано. На момент записи существующие Sound Hunter importer/workflow/README всё ещё содержат 25 GB ограничения; ручной импорт и код Sound Hunter ещё не изменялись в рамках этой записи. Требуется отдельный безопасный этап реализации с проверкой соответствующих путей импорта и workflow. Эта запись сама по себе не подтверждает изменение поведения программы.


### Глобальное правило повторного использования готовых решений и оптимизации ресурсов (2026-11-14)

Пользователь прямо утвердил, подтвердил и поручил зафиксировать это правило для **всего функционала POP PUNK GUITAR ULTIMATE REV.18**, включая уже утверждённые и будущие функции.

1. **Сначала исследовать готовые аналоги и компоненты.** Для каждого утверждённого функционального элемента изучать существующие профессиональные VST-инструменты, аудиопрограммы, открытые библиотеки, SDK и готовые движки, которые решают ту же или близкую задачу.
2. **Выбирать лучшее готовое решение.** Сравнивать качество результата, полноту функций, зрелость и поддержку, лицензию, коммерческие права, совместимость с Windows 10 x64 / VST3 / FL Studio 20, интеграционную сложность, стабильность, CPU, RAM, энергопотребление и влияние на время запуска.
3. **Предпочитать интеграцию разработке с нуля.** Если готовый компонент или целая система подходит и проходит проверки, отдавать предпочтение полной или частичной интеграции вместо повторного изобретения. Не создавать параллельные дублирующие движки и подсистемы без необходимости.
4. **Создавать собственную реализацию только при отсутствии подходящего готового решения** либо если готовые варианты не проходят обязательные критерии проекта. В таком случае выбирать наиболее простую, лёгкую, поддерживаемую и реалистичную архитектуру, избегая ненужной сложности и технически невыполнимых схем.
5. **Лицензии и права обязательны до интеграции.** Проверять лицензии исходного кода, SDK, библиотек, моделей и данных; совместимость с коммерческим закрытым VST3; условия распространения, атрибуции и модификации. Не интегрировать компонент, если права неясны или несовместимы, пока вопрос не разрешён.
6. **Главный технический приоритет — низкое энергопотребление и минимальная нагрузка.** Стремиться к низкому CPU и RAM, быстрому запуску, малой задержке, эффективному использованию потоков и ресурсов, отсутствию ненужной фоновой активности и масштабированию нагрузки по фактической потребности.
7. **Сохранять все ранее утверждённые функции и совместимость.** Интеграция/замена не должна молча удалять, ослаблять, скрывать или менять утверждённое поведение, preset/state/parameter IDs, MIDI, routing, articulations, FX или контракты ANGELA Engine. Любые изменения защищённых контрактов требуют отдельного согласования.
8. **Никаких операций диска/сети/распаковки и тяжёлой работы в audio callback.** Фоновая работа допустима только при сохранении безопасной передачи готовых данных в аудиопоток.
9. **Проверять измерениями, а не обещаниями.** Для каждой интеграции проводить подходящие сборочные, функциональные, лицензионные, совместимые и регрессионные проверки; измерять CPU/RAM и запуск на целевых сценариях. Не объявлять компонент готовым, пока проверки не пройдены.
10. **Прозрачность статуса.** Различать найденный аналог, оценённый компонент, одобренную интеграцию, реализованную интеграцию и прошедшую тестирование функцию. Само наличие аналога или запись правила не означает, что компонент уже интегрирован.

Пример направления для SFZ — исследовать зрелые готовые SFZ-движки (в том числе sfizz) как кандидатов, но не интегрировать автоматически: сначала проверить актуальную лицензию, техническую совместимость, функциональное покрытие и производительность.

**Приоритет выбора:** готовое подходящее решение → безопасная интеграция → измерения и тесты → собственная реализация только при необходимости.

**Статус:** глобальное правило утверждено и зафиксировано. Эта запись сама по себе не означает, что сторонние компоненты уже выбраны, лицензии проверены, код интегрирован или показатели производительности измерены.


### Обязательная оптимизация всех интегрируемых решений под ANGELA Engine и целевую систему (2026-11-14)

Пользователь прямо утвердил и подтвердил, что все найденные, адаптируемые, интегрируемые и самостоятельно разрабатываемые компоненты проекта должны быть оптимизированы под POP PUNK GUITAR ULTIMATE REV.18 и его ANGELA Engine. Цель — чтобы движок уверенно справлялся со всеми утверждёнными задачами без чрезмерной нагрузки на систему.

1. Низкая нагрузка — обязательный критерий выбора и реализации, а не необязательная последующая оптимизация. Компонент, который функционально подходит, но создаёт чрезмерную нагрузку, не считается автоматически пригодным к интеграции.
2. Оценивать не только средний CPU, но и пики/худшие сценарии, RAM и пиковые аллокации, время запуска/загрузки пресета и библиотеки, задержку, количество потоков, disk I/O и поведение при одновременной работе ключевых функций.
3. Проектировать нагрузку с учётом реальных сценариев: многоголосие, потоковое чтение сэмплов, velocity layers, articulations, palm mute, пресеты, эффекты и одновременное воспроизведение. Не допускать, чтобы сочетание функций приводило к неконтролируемому росту CPU/RAM или блокировкам аудиопотока.
4. Предпочитать лёгкие зрелые компоненты, ленивую загрузку, ограниченные кэши и очереди, переиспользование ресурсов и фоновую подготовку данных там, где это безопасно и совместимо с утверждёнными контрактами. Не загружать и не обрабатывать то, что не требуется текущему сценарию.
5. Не выполнять disk/network I/O, распаковку, тяжёлый анализ, блокировки или непредсказуемые аллокации в realtime audio callback. Передача ресурсов в аудиопоток должна быть потокобезопасной и ограниченной по времени.
6. При необходимости адаптировать готовое решение: отключать/не включать ненужные подсистемы только если это не удаляет утверждённую функциональность и не меняет её поведение; применять оптимизированные сборочные настройки и архитектуру интеграции. Нельзя маскировать чрезмерную нагрузку отключением утверждённых возможностей.
7. Проверять производительность на целевой конфигурации Windows 10 x64 / VST3 / FL Studio 20, включая стресс-сценарии и одновременную работу функций. Фиксировать методику, конфигурацию, измеренные результаты и выявленные ограничения. Не заявлять, что движок легко справляется со всеми задачами, пока это не подтверждено тестами.
8. Если готовый компонент не удаётся довести до приемлемой производительности без потери утверждённых функций, искать более лёгкую альтернативу или реализовывать недостающую часть самостоятельно; решение обосновывать сравнением и измерениями.

**Критерий приёмки:** все интегрированные и собственные компоненты должны сохранять утверждённую функциональность и совместимость, не нарушать ANGELA Engine NON-REGRESSION и демонстрировать приемлемую измеренную нагрузку в целевых и стресс-сценариях. Конкретные численные бюджеты CPU/RAM/latency следует установить после измерения целевой системы, не придумывая неподтверждённые показатели.

**Статус:** требование утверждено и зафиксировано. Запись не подтверждает, что оптимизация, интеграция или измерения уже выполнены.


## 2026-11-14 — SOUND HUNTER DOWNLOAD RUN #2 FAILED: ROOT CAUSE VERIFIED

**Run:** GitHub Actions run #2, ID `43573746837`.
**Workflow:** `.github/workflows/sound-hunter-real-guitar.yml`.
**Source commit:** `ebbe772d53fc076dbaa4fe1413a56f13c5f4bcb9`.
**Result:** FAILED; no artifact was produced. Steps 1–3 succeeded; the download/import step failed; packaging and artifact upload were skipped.
**Exact error:** `Refusing path collision: .../Samples/SoundHunterImports/<run-id>/meta-open-sound-steel/release`.
**Verified cause:** repository audio import already created a `release/` directory, then the separate Meta release-asset extraction attempted to use the same destination path. The importer's collision guard correctly stopped rather than overwrite data.
**Second confirmed policy defect:** the importer initializes its 25 GB counter by summing every file in the whole `Samples` tree. This incorrectly allows pre-existing/manual sample libraries to consume the Sound Hunter process limit, contrary to the approved rule that 25 GB applies only to the current Sound Hunter run. Manual imports remain without an artificial size cap.
**Data safety:** this failed run produced no artifact and did not alter the repository's sample assets. Its runner workspace was ephemeral. No successful download is claimed.
**Planned corrective scope:** (1) place release archive content in a distinct `release_assets/` subdirectory to avoid collision with repository content; (2) count only bytes imported during the current Sound Hunter run while retaining SHA-256 deduplication against existing samples; (3) add/execute focused tests and re-run workflow. No approved function or source library is to be removed or overwritten.


## 2026-11-14 — SOUND HUNTER RUN #2 ROOT CAUSE FIXED; RETEST PENDING

Run #2 (`43573746837`) failed in download/import with a verified destination collision between source-repository `release/` content and the separately downloaded Meta release ZIP. No artifact was produced. The same audit identified that the prior importer counted the entire existing `Samples` tree toward the Sound Hunter 25 GB limit, contrary to the approved rule that the cap applies only to the current Sound Hunter run.

Corrective implementation is now committed:
- `6adca0b36cf394bb76dd45906158f2e8c3fbedd8`: isolated release asset destination and current-run-only byte accounting.
- `54f4aeeff68ae68e892ea89265f4427fb551560f`: regression tests for path separation, traversal rejection, and current-run cap accounting.
- `50202631ff427bcaef356f3b164c2155b2de5c61`: workflow executes regression tests before download; current main HEAD at time of logging.

All three changed files were re-fetched and SHA-verified. Automated regression tests have not yet run; the fixed download workflow has not yet been rerun; no sample artifact is available or claimed. A fresh manual workflow dispatch on main is required to validate the fix and produce the download artifact.

The user instruction to maintain INFO, CODE, and the main journal during work is now being followed: INFO is active again for operational status, CODE records implementation commits and test evidence, and PPGU_JOURNAL records decisions and end-to-end status. Historical journal content is retained.


## 2026-11-14 — PRELIMINARY SFZ / SAMPLER COMPONENT COMPARISON

The current PPGU codebase already contains a custom SFZ parser, a 64-voice SamplerEngine, a background WAV page-streaming service, bounded SPSC request queue, fixed-size page cache and per-voice streaming reader. These existing ANGELA components must be protected; the external-engine search is to improve SFZ coverage without regressing bounded streaming, voice lifecycle, CPU/RAM or realtime safety.

| Candidate | Preliminary finding | Current disposition |
|---|---|---|
| `sfizioso` | BSD-2-Clause fork of sfizz; adds MPE handling and host-provided sample-reader callbacks. Dependencies include permissive licenses plus LGPL/WTFPL components requiring a full dependency audit. | Highest-priority candidate for an isolated SFZ compatibility/performance prototype; not yet approved/integrated. |
| `dusk-fizz` | Maintained BSD-2-Clause sfizz fork; static-library use, SFZ/SF2 coverage and regression tests; upstream archive avoided. | Alternative candidate; audit API/dependencies and benchmark against sfizioso. |
| `sfizz` upstream | BSD-2-Clause but archived 2026-06-21. | Prefer maintained forks unless specific feature/compatibility evidence favors upstream. |
| `Just a Sample` | MIT licensed complete JUCE VST3 sampler; strong waveform/looping/UX reference. Not a drop-in SFZ engine; Bungee time-stretch is explicitly CPU-heavy. | Reference selected ideas/algorithms only; no full plugin integration. |
| `AkiFX` | GPLv3 VST3/CLAP effects rack. | Excluded from direct integration under the planned closed-source/commercial distribution model unless licensing is separately resolved. |

**Status:** preliminary source/README/license metadata inspection only. No component is integrated, no benchmark is claimed, and full transitive-license audit remains open for sfizioso/dusk-fizz. Next evaluation must compare SFZ opcode coverage, sample-reader integration with `SampleStreamingService`, allocation/lock behavior, voice limits, memory footprint, build complexity, Windows 10 x64/VST3 compatibility and CPU under 64-voice stress.


## 2026-11-14 — Расширение Factory Preset Bank до 500 (утверждено пользователем)

**Решение:** расширить каталог с прежних 300 до 500 авторских определений; цель пользователя — не менее 300, предпочтительно 500. Зафиксировано 250 MELODY + 250 PATTERN.

**Охват каталога:**
- 21 артистический style profile и 16 семейств жанров/стилей (pop-punk, skate-punk, punk rock, melodic punk, emo pop-punk, ska-punk, alternative/pop-rock, ambient alternative, acoustic rock и родственные направления).
- 20 Melody archetypes: clean/crunch/high-gain/DI, palm mute, octave hooks, tremolo, harmonics, slides/legato, hammer-on/pull-off, sustain, ambient delay, acoustic steel/nylon, arpeggios и др.
- 20 Pattern archetypes в шести утверждённых категориях: DOWNSTROKES, DOWN/UP, PALM MUTE, POWER CHORDS, ALTERNATE PICKING, CUSTOM PATTERNS.
- Все 250 Melody definitions содержат articulation/performance targets; все 250 Pattern definitions — компактную 16-step последовательность tuple-v1.
- Ссылки на reference tracks нормализованы по artist profile: чужие треки больше не приписываются другим артистам.

**Архитектурное уточнение:** активный APVTS предоставляет host modes Melody/Rhythm. PUNK GROUP оформляется как подтип PATTERN, а не как выдуманный третий host mode.

**Файлы:** `presets/factory_bank_500.json`, `tools/validate_factory_bank.py`, `CMakeLists.txt`, `.github/workflows/vst3-build.yml`, `PPGU_APPROVED_PARAMETERS.md`, `docs/PRESET_FACTORY_500_RU.md`.

**Статус:** 500 preset definitions созданы и прошли структурную проверку в подготовительном прогоне. Runtime loader, реальные sample-zone mappings, артикуляционная маршрутизация, Pattern event integration, слуховая проверка каждого preset и exact-SHA Windows/CTest/pluginval CI остаются PENDING. Наличие записи в каталоге не объявляется доказательством воспроизведения.

**Коммиты:** каталог/metadata — `79d4e2ab0d382b84bb862ee3e03370c0a67129e3`; build/validator/docs integration — `99577e84744c1de13bdd8c7dd8db84cf79fc7829`.

**Проверка до CI:** 500 записей; 250+250; уникальные ID и имена; 21 artist profile; reference tracks сверены с соответствующим artist profile; все Pattern entries содержат 16-step tuple-v1 events; диапазоны drive/tone/reverb и поля performance проверены. Результат структурного preflight: PASS. GitHub Actions result: ожидает проверки отдельно.


## 2026-11-14 — ПОЛНОЕ ТЕХНИЧЕСКОЕ ДЕЛЕГИРОВАНИЕ И СВОДНОЕ ПОДТВЕРЖДЕНИЕ ОБЯЗАТЕЛЬНОГО ОБЪЁМА PPGU

**Статус:** APPROVED BY USER — MUST IMPLEMENT / DOCUMENTED. Пользователь подтвердил максимальную техническую автономию в пределах доступных инструментов для организации разработки, выбора наиболее качественных решений, исправления дефектов, оптимизации, написания и изменения кода, тестирования и использования внешних технических ресурсов. Это подтверждение дополняет, а не отменяет генеральный план и ранее утверждённые контракты.

### 1. Полномочия на самостоятельную инженерную работу
- Самостоятельно выбирать лучший из нескольких технических вариантов после оценки рисков, совместимости, качества звука, производительности, надёжности, сопровождения и стоимости владения.
- Использовать доступные GitHub/CI, локальные инструменты, открытые технические материалы, документацию SDK/DAW/VST, публичные reference-проекты и совместимые библиотеки/алгоритмы, чтобы не переписывать заново то, что можно безопасно и законно переиспользовать.
- Автоматически исправлять очевидные, обратимые, локальные и неразрушающие дефекты; добавлять регрессионные тесты; обновлять документацию и журналы; продолжать независимые безопасные задачи без ожидания подтверждения на каждый мелкий шаг.
- Перед существенными изменениями проверять зависимости, лицензии, SBOM, совместимость с JUCE/VST3/Windows/FL Studio, realtime safety, размер бинарника, CPU/RAM, старт и обновление проекта.
- Не копировать закрытый исходный код, защищённые реализации, proprietary UI/assets, записи, риффы или банки. Использовать идеи и общие механики как ориентиры; прямое включение стороннего кода/ресурсов допускается только после проверки совместимости лицензий и прав на распространение.
- Не обходить paywall/login/DRM/CAPTCHA/rate limits; не совершать платные покупки, публикации от имени пользователя, действия с личными данными/учётными записями и иные необратимые внешние действия без отдельного согласования.
- При конфликте утверждённых контрактов, неоднозначных правах, необратимом действии, риске потери пользовательских данных или необходимости изменить MIDI/APVTS/публичную совместимость — остановить только затронутую часть и задать один конкретный вопрос. Остальную независимую работу продолжать.
- Не обещать непрерывную фоновую работу между сессиями и не утверждать, что ресурс/инструмент использован, если фактического вызова не было.

### 2. Обязательная библиотека реальных гитарных аудиоматериалов
Обязательные раздельные типы SoundBank: **One-Shots; Sample Loops; Multisamples; Performance Sets; прочие утверждённые гитарные аудиоактивы**. Это расширяет прежний scope, не сливая разные типы контента в одну сущность.
- Никаких искусственных верхних лимитов количества записей, файлов, источников или категорий.
- Стабильные IDs, версионирование схем, provenance/source URL, права/лицензия, атрибуция, SHA-256, metadata/index/registry и дедупликация по содержимому.
- Metadata-first indexing, lazy loading, виртуализированные списки, асинхронная загрузка/декодирование, prefetch по необходимости и ограниченный кэш с вытеснением.
- Никаких файловых операций, сетевых запросов, тяжёлого декодирования/анализа, блокирующих mutex или неограниченных аллокаций в Audio Callback.
- Не ухудшать CPU/RAM, время запуска и latency: подтверждать это профилированием и тестами. Большой каталог не означает одновременную загрузку всех аудиоданных в RAM.
- Loop/phrase/one-shot не маркировать как chromatic multisample; неизвестные mappings/права — в карантин/на review, не объявлять playable.
- Общая детализация и правила SoundBank продолжают жить в docs/PPGU_GUITAR_SOUND_LIBRARY_MASTER_PLAN_RU.md.

### 3. Полный утверждённый набор 21 punk/pop-punk reference profiles
Ни один профиль не должен теряться в браузере, sound map, каталоге или QA:
Blink-182; Sum 41; Simple Plan; New Found Glory; Good Charlotte; Mest; Angels & Airwaves; The All-American Rejects; MGK; Avril Lavigne; The Offspring; Green Day; Bowling for Soup; Sugarcult; All Time Low; Fall Out Boy; Yellowcard; My Chemical Romance; Less Than Jake; Fenix TX; Midtown.
Для каждого профиля обязательны согласованные genre/subgenre, guitar tone, gain/drive, rhythm approach, techniques/articulations, verse/chorus/section context, melodic hooks и применимые acoustic/electric варианты — только в пределах подтверждённых данных и совместимых реальных источников. Названия групп служат стилевыми ориентирами, не заявлением аффилиации и не разрешением копировать защищённые записи/риффы.

### 4. AUTO CHORDS — подтверждённое дополнение
Выполнять требования раздела «ДОПОЛНИТЕЛЬНОЕ ПОДТВЕРЖДЕНИЕ» §1 в docs/PPGU_JOURNAL/PPGU_MAJOR_USER_REQUIREMENTS_AMENDMENT_2026-11-14_RU.md:
- ROOT NOTE — все 12 хроматических нот.
- MODE — поддерживаемые музыкальные лады/режимы.
- Экранная клавиатура показывает ноты текущего аккорда/режима с обновляемой подсветкой при изменении ROOT NOTE, MODE, входящего MIDI-аккорда и музыкальных параметров.
- Визуал SIGNAL FORGE / NEON, синхронизация с активным skin.
- AUTO CHORDS остаётся частью PATTERN, отдельной функцией, взаимодействующей с PRESET и PLAYING STYLE; не создаёт лишнее верхнеуровневое окно.
- Нельзя отображать неподдерживаемые аккорды/лады как рабочие; реальное voicing/mapping и корректное завершение нот обязательны.

### 5. Принцип «APPROVED = обязательство»
Любое пользовательское APPROVED требование обязательно к реализации и соответствующей проверке; документация или JSON definition не доказывают runtime-функциональность. Статусы не смешивать:
APPROVED → DOCUMENTED → IMPLEMENTED → TESTED → VERIFIED → COMPLETE.
COMPLETE разрешён только после выполнения всех обязательных требований и релизных gate. Сохранять append-only историю; более позднее явное утверждение меняет только свой предмет, не стирая несвязанные требования.

### 6. Организация и приоритет работ
Приоритет реализации: архитектурные блокеры и build graph → реальные источники/лицензии и SoundBank ingestion → runtime sampler/mappings/articulations → preset loader и валидируемый каталог → PATTERN/MIDI/AUTO CHORDS → UI/skin/window contracts → FX/DSP → CPU/RAM/latency/performance → regression/host acceptance/package. Порядок можно менять при доказанном dependency blocker, фиксируя причину в журнале. Для каждой задачи — acceptance criteria, exact commit SHA, targeted tests, regression tests и evidence.

**Статус текущей проверки:** GitHub Actions run 43577554118 наблюдался как in_progress на момент записи. Это не PASS; окончательный результат и артефакты требуют отдельной проверки. SoundBank registry ранее был зафиксирован как пустой; наличие утверждённого scope не означает, что библиотеки уже загружены или интегрированы.


## 2026-11-14 — PPGU REV.18: ПОЛНОЕ ПОДТВЕРЖДЕНИЕ FX / SKINS / SETTINGS / SAMPLES / PRESETS / RELEASE PACKAGE

**Статус:** USER APPROVED / MUST IMPLEMENT. Запись дополняет все ранее утверждённые спецификации и контракты; не отменяет ANGELA Engine, MIDI/APVTS/state compatibility, лицензирование, realtime safety, quality gates и прежний порядок главных разделов.

### 1. FX EDIT — единый самостоятельный модуль
- Все ранее утверждённые описания FX EDIT, если они относятся к одному окну/системе, объединить в одну согласованную реализацию, не создавать дублирующие FX окна или параллельные несовместимые цепочки.
- В EFFECT SELECT должно быть доступно от 10 до 15 ранее утверждённых эффектов. Состав брать из существующего утверждённого FX-журнала/спецификации; не подменять его придуманным списком и не ограничивать систему прежними 7–8 слотами.
- Каждый эффект — реально реализованный DSP-процессор с рабочими параметрами, маршрутизацией, сохранением состояния, preset integration, корректным bypass/enable и тестами. Cabinets/amp-cab processing сохраняются как отдельная утверждённая часть системы.
- Проверить порядок/перестановку, serial/parallel routing там, где это предусмотрено контрактом, уровни, отсутствие клиппинга/щелчков, realtime safety и нагрузку. Название/GUI без DSP не считается реализацией.

### 2. SKIN GALLERY — 16 законченных тем
- Полностью реализовать утверждённую галерею из 16 тем, включая главную **NEON STAGE** (загружается по умолчанию).
- Остальные 15 тем должны иметь самостоятельную дорогую, коммерчески качественную визуальную идентичность; при этом сохраняются единые layout, размеры/координаты, элементы, подписи, bindings, управление, поведение, функциональность и отображение всех контролов.
- Различаться могут визуальные ресурсы, материалы, палитра, фон, декоративные элементы и графическое оформление; тема не может скрывать/перемещать/отключать функции или менять звук.
- Все 16 тем должны быть доступны из Skin Gallery, корректно применяться/сохраняться, иметь fallback и проверяться на всех окнах/панелях и разрешениях.

### 3. SETTINGS — ресурсные профили и фиксаторы
- В SETTINGS реализовать оптимизационные профили **GOOD, NORMAL, NIGHT, ECO**. Если историческая запись содержит вариант написания GOOD/NIGHT/ECHO, нормализовать отображаемое имя ECO только после проверки исходного контекста; смысл — четыре явных режима экономии/качества, а не скрытые функции.
- Параметры профилей управляют безопасными ресурсными политиками: качество/oversampling, preload/cache, фоновые задачи, CPU scheduling/worker policy и другие поддерживаемые параметры. Не отключать утверждённую функциональность и не допускать аудиоартефактов.
- Добавить режимы **2x / 4x / 8x / 16x** как выбор качества/oversampling только для тех DSP-блоков, где он технически применим. Не выдавать это за увеличение polyphony или sample rate. Предупреждать о росте нагрузки.
- Добавить выбор Voices с уровнями **2 / 4 / 8** как минимум; архитектура может поддерживать больше голосов, если это нужно утверждённым сценариям. Контроль должен функционально и безопасно ограничивать активную полифонию.
- Для Voices, профиля GOOD/NORMAL/NIGHT/ECO и режимов 2x/4x/8x/16x предусмотреть независимые фиксаторы-замки по образцу Serum: при включённом Lock значение сохраняется при смене пресета/сценария согласно явному контракту; lock-state сохраняется и восстанавливается.
- Профили/oversampling/voices должны быть измерены на целевой системе, а не только визуально переключаться.

### 4. ABOUT / документация пользователя
- ABOUT показывает: создатель **L3XTOWN**, продукт **POP PUNK GUITAR ULTIMATE**, версия продукта **REV.18**, год разработки **2026**, название и версию звукового движка, сведения о продукте и юридические notices/атрибуции.
- Название/версия движка должны соответствовать реальному engine build metadata; не выдумывать номер версии. Продуктовая версия REV.18 фиксируется как пользовательское требование.
- В комплект входят полные руководства пользователя на русском и английском языках, с иллюстрациями/скриншотами всех частей интерфейса, окон, секций, контролов, статусов и основных рабочих процессов. Документы должны совпадать с реальной финальной сборкой, иметь навигацию, содержание, объяснения функций, примеры и troubleshooting.
- Скриншоты должны быть получены из фактического интерфейса проверенной сборки; концепт-мокапы не выдавать за скриншоты продукта.

### 5. SOUNDBANK переименован в SAMPLES; целевой объём без искусственного лимита
- Во всём UI, документации, путях экспорта и пользовательских операциях каноническое пользовательское название раздела — **SAMPLES** (вместо SOUNDBANK). Исторические внутренние имена/пути можно сохранять для совместимости, если миграция небезопасна.
- Общий размер SAMPLES, число файлов, источников, типов и категорий не ограничиваются искусственным программным лимитом. Фактические ограничения — доступное место на диске, файловая система, память/производительность и ограничения GitHub/CI/artifact hosting; показывать фактическую причину, не подменять её произвольным лимитом.
- Это более позднее прямое решение пользователя заменяет ранее записанные целевые/временные ограничения 25 GB и 25–30 GB как глобальный или библиотечный лимит, в том числе для Sound Hunter. Сохраняются безопасное разбиение крупных загрузок/артефактов на части, возобновление/проверка целостности, additive-only импорт, SHA-256, дедупликация, provenance и SOURCE/LICENSE REVIEW.
- Отмена лимита объёма не разрешает распространение аудиоматериалов без прав. Для включения в заводской пакет необходимы права на коммерческое распространение исходных samples; неизвестные/несовместимые лицензии — quarantine/review. Пользовательские локальные библиотеки не обязаны быть redistributable, чтобы использоваться локально в рамках закона.
- Большие каталоги обслуживаются индексом, стабильными ID, lazy loading, streaming, ограниченным кэшем и фоновыми задачами; никаких тяжёлых операций в Audio Callback.

### 6. Factory Presets и музыкальные ориентиры
- Требование к factory catalog: **не менее 500 полностью функциональных пресетов MELODY и не менее 500 полностью функциональных пресетов PATTERN** (минимум 1,000 суммарно). Это минимум, не верхний предел; каталог расширяемый.
- Ранее созданный factory_bank_500.json (250+250) является промежуточным структурным каталогом, не удовлетворяющим новому минимуму 500 на каждый режим. Не объявлять его конечным банком.
- Добавить отдельный curated-набор из **82 пресетов в стилистическом направлении Travis Barker**, охватывающий музыкальные подходы и проекты/релизы разных периодов, включая новые релизы, с опорой на законно доступные сведения о звучании и технике. Эти 82 — дополнительное обязательное покрытие; учитывать их в общем минимуме только при фактическом соответствии режиму и QA.
- Сохранить все ранее утверждённые 21 музыкальный профиль и расширить каталог pop-punk/punk группами и релизами периода **2000–2007**, не теряя ранее перечисленные группы. MGK и новые релизы/современные направления также обязательны.
- Имена/референсы — стилистические ориентиры. Не копировать защищённые записи, оригинальные stems, фирменные риффы или неразрешённые sample packs. Каждый пресет должен иметь собственные settings, артикуляции, паттерны/ноты, FX/amp chain, совместимые assets и доказуемую функциональность; placeholder presets не засчитываются.

### 7. Финальная структура поставки
Финальный комплект должен содержать:
1. POP PUNK GUITAR ULTIMATE VST3 — Windows 10 x64 VST3.
2. Samples — заводская законно распространяемая библиотека и поддержка пользовательских библиотек.
3. Factory Presets — оба режима, каждый не менее 500 полностью проверенных пресетов.
4. User Presets Support — создание, сохранение, импорт/экспорт, резервирование и восстановление пользовательских пресетов.
5. Skin Gallery — все 16 тем.
6. Resource — необходимые runtime/resources и сведения о зависимостях.
7. License — лицензии, notices, attribution и права на распространяемые материалы.
8. Documents — документация/спецификации для поставки.
9. Руководства RU/EN с иллюстрациями всех частей UI.
10. Контрольные суммы (SHA-256 manifest) для файлов и частей пакета.
11. Полный ZIP-пакет; если лимит платформы/GitHub препятствует одной загрузке, разделять на последовательные части с manifest, порядком сборки, проверкой SHA-256 и возможностью восстановления/повторного скачивания.

### 8. Сохранение основной структуры
Порядок верхнего уровня остаётся каноническим и не переставляется без отдельного согласования:
MAIN EDITOR → MELODY → PATTERN → AUTOCHORDS → FX EDIT → AMP & TONE → DOUBLE → SOUND ENGINE → SAMPLES → PRESETS → IMPORT GUITAR → AUTO DISCOVER → CURATOR → SKIN GALLERY → SETTINGS → PERFORMANCE → ABOUT → TESTING → RELEASE.
Внутренние названия AUTO DISCOVER / AUTO SOUND HUNTER должны оставаться согласованными с последним утверждённым unified naming contract; пользовательский раздел в главной структуре сохраняет указанную позицию.

### 9. Acceptance / запрет фиктивной готовности
Каждый пункт проходит цепочку APPROVED → DOCUMENTED → IMPLEMENTED → TESTED → VERIFIED → COMPLETE. Наличие записи, UI-макета, каталога имён или JSON не означает, что функция реализована. FX требуют DSP tests; skins — UI/state tests; settings/locks — persistence/host tests; presets — runtime playback/audition/asset coverage; SAMPLES — license/integrity/import/playback tests; manuals — сверка со скриншотами фактической сборки; release ZIP — install/manifest/checksum verification. Целевой acceptance: Windows 10 x64, VST3, FL Studio 20.

**Conflict resolution:** эта запись является более поздним прямым утверждением пользователя и supersedes прежние conflicting size caps/targets and the 250+250 bank minimum. Остальные ранее утверждённые требования продолжают действовать. Точный список эффектов и названия 16 тем брать из исходных утверждённых спецификаций, не изобретать новые списки при отсутствии первичного источника.
