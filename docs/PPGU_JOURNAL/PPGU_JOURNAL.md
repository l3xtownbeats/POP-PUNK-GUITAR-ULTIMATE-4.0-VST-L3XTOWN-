

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


## 2026-11-14 — SOUND HUNTER DOCUMENTATION CORRECTION AND UPSTREAM GUITAR SOURCE RE-AUDIT

**Scope:** documentation correction and source research only. Production C++/Python importer, source catalog, sample assets and VST3 binaries were not changed by this operation.

### Sound Hunter size-rule correction
- Updated `SoundHunter/README.md` to correctly state that the 25 GB cap applies only to newly imported bytes in the current Sound Hunter run.
- Existing/manual content in `Samples` does not consume that run allowance; manual import has no artificial 25 GB cap.
- Clarified that the current-run cap is distinct from the separately approved 25–30 GB final factory Guitar Sound Library target.
- README now explicitly requires revalidation of upstream repository layout, release asset names, licenses, manifests, SFZ maps and checksums before a run.
- README commit: `7b07c887d4094f54f01916b011b59cf3f89ecc3d`; new README blob: `d87f2217cea9ea349bbd02d2ce9f566b6cd4d488`.

### Meta Open Sound upstream findings
Current upstream documentation/catalog identifies:
- Meta Nylon Guitar: 348 multisamples, E2–B5, six string zones, three dynamic layers (two picked and one fingerstyle).
- Meta Steel Guitar: 375 multisamples, E2–C6, six string zones, three velocity layers.
- Full-quality WAV sources are 24-bit / 48 kHz; the repository includes OGG previews, manifests, SFZ maps and per-collection SHA256SUMS.
- The guitar collections are CC0-1.0; the repository has per-collection LICENSE files.
- Nylon documentation explicitly says there are no dedicated muted/palm-muted layers or separate release samples. Palm mute therefore cannot be claimed as natively sampled from this library; it needs another verified source/articulation or a separately designed DSP/envelope technique that is tested and labelled accurately.
- Current upstream structure is `systems/instruments/meta-nylon-guitar` and `systems/instruments/meta-steel-guitar`, with `catalog.json`, collection manifests and SFZ maps as mapping authorities.

**Critical follow-up:** the PPGU catalog/importer was authored against an earlier Meta repository layout and release-asset naming. The current catalog's `release_asset_contains` values and the importer's assumptions must be checked against the current GitHub Releases API and current manifests before rerunning Sound Hunter. Do not assume the previous asset-name match still succeeds. No download or import is claimed.

### Evidence / references reviewed
- https://github.com/facebookincubator/Meta-open-sound
- https://github.com/facebookincubator/Meta-open-sound/blob/main/catalog.json
- https://github.com/facebookincubator/Meta-open-sound/blob/main/AGENTS.md
- https://github.com/facebookincubator/Meta-open-sound/blob/main/systems/instruments/meta-nylon-guitar/README.md
- https://github.com/facebookincubator/Meta-open-sound/blob/main/systems/instruments/meta-steel-guitar/README.md
- https://sfzinstruments.github.io/guitars/

### Verification status
- README update committed and its resulting blob SHA returned by GitHub.
- Upstream facts reviewed from repository documentation/catalog and SFZ Instruments catalog.
- Sound Hunter regression tests: NOT RUN in this operation.
- Sound Hunter workflow: NOT DISPATCHED; no fresh artifact was inspected.
- Meta catalog refresh, release-asset API revalidation, full factory-bank integration, ANGELA playback, preset validation, VST3 build, pluginval and FL Studio 20 host testing: NOT RUN / NOT VERIFIED.


## 2026-11-14 — USER-APPROVED GLOBAL 40 GB SAMPLES CEILING; REMOVE SOUND HUNTER PER-RUN CAP

Пользователь прямо утвердил, подтвердил и поручил зафиксировать:
- Полностью снять прежнее ограничение Sound Hunter в 25 GB на один запуск.
- Не вводить вместо него другой per-run download/import cap.
- Установить единый жёсткий максимальный объём управляемой библиотеки Samples: **40 GB decimal = 40,000,000,000 bytes**.
- Считать в этот объём уже имеющиеся поддерживаемые аудиоактивы внутри `Samples` и новые уникальные аудиоактивы; дубликаты по SHA-256 не увеличивают объём.
- Никакое новое добавление не должно приводить к превышению 40,000,000,000 bytes. При достижении потолка прекратить дальнейшее добавление; существующие файлы не удалять, не перемещать и не перезаписывать.
- Это решение заменяет прежнюю цель 25–30 GB для factory Guitar Sound Library и отменяет прежнее правило об отсутствии общего ограничения для ручного импорта, если ручной импорт копирует файлы внутрь управляемой `Samples` библиотеки. Ручной импорт внешней библиотеки по ссылке без копирования файлов в `Samples` не расходует место в этой папке и не изменяет оригиналы.

### Реализация, выполненная в этом этапе
- `SoundHunter/sound_hunter_real_guitars.py`: удалён per-run 25 GB аргумент/счётчик; добавлен агрегатный учёт поддерживаемых аудиофайлов в `Samples`; предел 40 GB decimal; проверка существующего объёма до создания run directory; SHA-256 dedup выполняется до решения о добавлении; после достижения потолка Sound Hunter прекращает обход новых источников.
- `SoundHunter/real_guitar_sources.json`: `max_library_bytes = 40000000000`, `max_library_gb = 40`.
- `.github/workflows/sound-hunter-real-guitar.yml`: передаёт `--max-library-gb 40`; имя артефакта `PPGU-REAL-GUITAR-SAMPLES-40GB-CEILING`.
- `tests/test_sound_hunter_importer.py`: добавлены проверки граничных значений 39 GB + 1 GB, 39 GB + 2 GB, 40 GB + 1 byte, а также подсчёта аудиофайлов.
- `SoundHunter/README.md`: обновлены правила объёма и описано, что ручной импорт, копирующий файлы в `Samples`, также обязан соблюдать общий потолок.

### Важное ограничение / открытая работа
Текущий C++ `ImportPipeline` в изученном файле является screening/gate skeleton, а `ImportGuitarPanel` — UI оболочкой; фактический production path копирования ручных библиотек не был найден/подтверждён в данном этапе. Поэтому **не утверждать, что ручной импорт уже технически ограничен 40 GB**. Если/когда путь копирования обнаружится или будет реализован, в него необходимо встроить общий capacity guard до записи файлов. Внешние библиотеки, которые SamplePool только читает/стримит с исходного расположения, не копируются в `Samples` и не должны модифицироваться.

### Кодовые коммиты
- `76e13b935582f5e01c0854db34a2af3a7f01965e`
- `33305dee44981d768217fc0685e1675883247bca`
- `f03361e0d3dd9b4870b06fa84563539b2c489b84`
- `53f8179a3e50d51b694c9139571835d1844ccf67`
- `be79683a83ea5d9e223838288793fedba6ec8f3a`
- `e8b97849892438f2ae921dbe6325bfcff1a6524d`
- `42b702ba7e94740c500057a04647aaf3cfdf663a`
- `21e439d9fb200eb3db13d8ff47709e6bb697265f`
- `8ab71c5267c911631f15d9811fec66dbadeb357c`
- `4d171bf48d8bca03600b2148c9b49fc4fb5c4cdd`
- `611e49de9b7651d8fa2ddd3899ddf2ad92faddf6`
- `eecc6cdaa2b6d85e61e07d9d28d15bd929f7f08c`
- `a7d3765cda972efb3af5902f803883a55ce23bef`
- `a1317756daa688dfdc29a2d1dd73cbb46e0fda2e` — CODE journal.

**Проверка статуса:** файлы повторно прочитаны из GitHub после изменений. Python regression tests не запускались; GitHub Actions workflow не запускался; реальный аудиоартефакт не создан/не проверен; manual import guard остаётся не реализованным/не подтверждённым. Не заявлять download/import/QA PASS.


## 2026-11-14 — ANGELA ENGINE CONTINUOUS OPTIMIZATION + ORIGINAL LIBRARY PRESERVATION + PPGU NATIVE SAMPLE FORMAT

Пользователь утвердил и подтвердил как постоянные главные правила:

### A. Непрерывная оптимизация ANGELA Engine
- Постоянно модернизировать, улучшать, оптимизировать и облегчать ANGELA Engine: минимизировать практическую нагрузку на CPU/RAM, дисковое пространство, время запуска и загрузки.
- Развивать текущую архитектуру поэтапно, **не пересобирать движок с нуля** и не ломать существующий функционал.
- Сохранять звучание, пресеты, MIDI, артикуляции, FX, маршрутизацию и пользовательские сценарии.
- Любая оптимизация должна иметь baseline/benchmark и regression/A-B tests; нельзя заявлять улучшение без измерений.
- Realtime audio callback остаётся свободным от файлового I/O, декодирования, ожидания, блокировок и непредсказуемых allocations/deallocations.
- Изменения — небольшие, изолированные, проверяемые и обратимые; приоритет reuse-first и защита существующего ANGELA streaming path.

### B. Оригиналы библиотек не удалять
- Загруженные исходные библиотеки остаются на исходных ресурсах/носителях, где они хранятся.
- Любая конвертация создаёт дополнительную производную копию в новом уникальном пути; не удалять, не перемещать, не переименовывать и не перезаписывать оригиналы.
- Перед конвертацией обязательно подтвердить лицензионное право на преобразование и коммерческое включение производного контента в VST-плагин.
- Сохранять provenance, исходный формат, SHA-256, mapping, velocity/key zones, loops, articulations, round-robin и прочие музыкальные метаданные.
- Никакой конвертации или распространения библиотек с неясными/запрещающими условиями без письменного разрешения.

### C. Собственный контейнер PPGU
Создан проектный документ `docs/ANGELA_ENGINE_CONTINUOUS_OPTIMIZATION_AND_PPGU_SAMPLE_CONTAINER_RU.md` (commit `fd50f0774c2c975c1aebbd3311725e363db89b83`).

Зафиксировано проектное направление **PPGU Sample Container (PSC), предлагаемое расширение .ppgs**:
- chunk-based container, быстрый индекс и random access;
- embedded metadata/provenance/license records, SFZ/PPGU mappings;
- независимые блоки с offsets, codec, decoded size, CRC/checksum;
- первоначально исследовать lossless FLAC в независимых чанках против PCM; не использовать lossy compression для factory guitar sources без отдельного решения и строгого A/B;
- runtime читает только нужные страницы через background worker и bounded cache; audio thread никогда не распаковывает/не ждёт;
- PSC вводится как дополнительный backend рядом с WAV/SFZ, а не заменяет их до полной проверки;
- converter создаёт копию и сохраняет оригинал; предусмотрен rollback.

Это **утверждённая спецификация/направление, а не реализованный формат**. На момент записи PSC, converter и PSC reader ещё не реализованы; benchmark размера/скорости/CPU не проводился. Не заявлять обратное.

### D. Лимит Samples — повторное подтверждение
Пользователь повторно утвердил снятие прежнего лимита 25 GB на один запуск и жёсткий общий потолок **40 GB decimal = 40,000,000,000 bytes** для managed `Samples`.
- Не вводить новый per-run cap.
- Считать физический размер аудиоактивов и контейнеров, фактически размещённых внутри `Samples`, включая будущие PSC файлы.
- Любой Sound Hunter/import/conversion write должен до записи проверить aggregate capacity.
- Оригиналы вне `Samples`, используемые без копирования, остаются на своих ресурсах и не считаются частью managed folder.
- Если оригинал копируется в `Samples`, эта копия считается; внешнее исходное оригинальное хранилище не трогать.
- Дубликаты не должны занимать дополнительное место; при превышении лимита отказать до записи, без удаления/перезаписи существующих данных.
- 40 GB — абсолютный потолок, не обязательная цель заполнения.

### E. Очерёдность реализации
1. Зафиксировать baseline производительности действующего production sampler/streaming path.
2. Аудировать реальные точки входа SamplerEngine/SamplePool; отдельно проверить, какие старые одноимённые root-level prototypes являются заглушками.
3. Спроектировать и протестировать PSC v1 parser/validator, versioning, bounds, checksums и corruption/fuzz tests.
4. Реализовать offline converter в уникальные пути, не меняющий source.
5. Добавить PSC page reader рядом с WAV backend; сравнить A/B до возможного использования в production.
6. Обеспечить единый 40 GB guard для Sound Hunter, manual import и converter до записи.
7. Windows 10 x64 Release build, CTest, pluginval и FL Studio 20 acceptance.

**Статус:** пользовательское правило утверждено и записано; архитектурный документ создан. Реализация PSC, performance benchmark и полный guard для ручного импорта ещё требуют выполнения/проверки.


## 2026-11-14 — Delegated autonomous engineering and guitar-library acquisition mandate

**Тип:** user-approved project governance / scope decision.
**Решение:** пользователь делегировал самостоятельный выбор технических и творческих решений, поиск источников, модернизацию, исправление дефектов, багов, конфликтов и противоречий по генеральному плану. Продолжать работу без повторного запроса одобрения для обычных безопасных инженерных решений.
**Границы:** делегирование пользователя не заменяет разрешения правообладателей и не может отменить лицензии/условия сторонних библиотек. В коммерческий плагин включать только источники с подтверждённым разрешением на коммерческое распространение исходных сэмплов внутри ПО. Не скачивать/интегрировать сомнительные материалы. Не удалять, перемещать, переименовывать или перезаписывать оригиналы.
**Текущий приоритет:** собрать максимально полезную реальную библиотеку электрических и акустических гитар, затем формировать пресеты на реальных мультисэмплах с сохранением SFZ/manifest/velocity/string-zone/articulation provenance. Соблюдать общий лимит Samples ровно 40,000,000,000 bytes, SHA-256 dedup, без per-run cap.
**Актуальная проверка источников:** Meta Open Sound публикует Steel Guitar (375 multisamples, E2–C6) и Nylon Guitar (348 multisamples, E2–B5), CC0-1.0; официальные release assets доступны как WAV 24-bit/48 kHz. FreePats публикует электрические FSBS Clean, Jazz, Direct DI и Distorted банки под CC0-1.0; SFZ FLAC/WAV/SF2 варианты доступны в репозиториях. Это подтверждение источников/лицензионных деклараций на уровне первичных страниц и репозиториев, не подтверждение фактической загрузки или импорта.
**Выявленные открытые проблемы Sound Hunter:** release archive per-file SHA256SUMS ещё не проверяется; per-collection license/manifest/SFZ preservation needs audit; registry `files_added` may undercount release assets; Meta OGG previews currently may be ingested alongside WAV release; verify actual importer before download. No audio has been downloaded/imported in this session.
**Статус:** DECISION ACCEPTED; SOURCE RESEARCH UPDATED; DOWNLOAD/IMPORT PENDING.
