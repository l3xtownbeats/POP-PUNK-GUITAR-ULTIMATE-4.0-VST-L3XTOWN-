# REV 9.0-A — SOUNDBANK ENGINE
Дата фиксации: 11 ноября 2026. Целевая ветка: main.
Статус: ENGINEERING SPECIFICATION; не заявление о реализации.

## 1. Назначение и лицензии
SOUNDBANK объединяет заводской SampleBank, пользовательские банки, импортированные звуки и лупы. Индекс хранит метаданные и ссылки; аудио читается по требованию через существующий page-cache/streaming path.
В дистрибутив включаются только исходные файлы, для которых подтверждено право коммерческого распространения и включения в продукт. CC0 предпочтителен. Бесплатное скачивание само по себе не даёт права перераспространения. Для каждого ресурса сохраняются источник, автор, лицензия, evidence, дата проверки и checksum. Неясная лицензия запрещает включение в дистрибутив; пользовательский импорт остаётся локальным.

## 2. Единый индекс и хранение
Логическая схема index.idx: schemaVersion; banks(bankId/name/source/license/licenseEvidence/fileFormat/rootPath); samples(sampleId/path/checksum/rate/channels/frames/rootKey/keyRange/velocityRange/articulation/string/fret/RR); loops(loopId/tempo/key/bars/beats/category/sampleId); mappings; layers; stacks; scan timestamp и diagnostics. JSON или SQLite выбирается после прототипа и измерений.
Сканирование инкрементальное. Worker строит новый immutable snapshot, валидирует его и публикует атомарно. Старый snapshot живёт, пока его читатели не завершили работу. Нет искусственного лимита на число банков, но RAM индекса и page-cache ограничена измеримыми бюджетами.
Audio thread не открывает файлы, не декодирует, не ждёт worker, не берёт mutex и не выделяет память. Cache miss не блокирует аудио: безопасный fallback/пропуск страницы с диагностикой. Page lease требует безопасного lifetime/reclamation; atomic refcount сам по себе недостаточен.

## 3. Layered Sound Engine
Preset ссылается на Stack. До 6 слоёв на артикуляцию: body, attack, sub, air, double, ambient. Параметры: gain -24..+6 dB, pan -100..100, tune cents, delayStart 0..50 ms, attack/release, transient amount, low/high cut, velocity curve, RR и phase metadata. Базовый слой обязателен.
Слои суммируются в заранее выделенный per-voice buffer до PickupBody и общей FX-цепочки. Они не создают отдельные внешние MIDI voices; лимит остаётся 64 музыкальных voices, но стоимость внутренних readers, фильтров, pitch shift и mixing учитывается отдельно.
Корреляция, mono compatibility и polarity/offset analysis выполняются при импорте/подготовке Stack вне audio thread. Runtime применяет только заранее рассчитанные решения. Автоинверсия не универсальное решение: анализ нескольких участков, предупреждение о фазовом конфликте и журнал решения обязательны. Транзиент и сустейн выбираются по ролям и огибающим.
Цель 6 layers x 16 voices <6% — только измеряемый gate на контрольном i3 2C/2T с фиксированными sample rate/block size, не обещание. Сначала измерить одиночный voice и добавочную цену каждого слоя. При превышении бюджета разрешено прозрачно отключать только необязательные слои по профилям GOOD/NORMAL/NIGHT/ECO; качество исходного сэмпла, velocity/RR и базовый слой молча не ухудшаются. UI сообщает о снижении.

## 4. IMPORT GUITAR
Import работает через WindowManager и worker jobs: scan, decode, screening, analysis, optional conversion, mapping proposal, commit/index publish. Форматы: WAV, FLAC, OGG, AIFF, SF2, SFZ, одиночные звуки и loops. NKI напрямую не импортируется: UI честно сообщает об ограничении, если нет легального конвертера и доступных исходных файлов.
Импорт имеет прогресс, отмену и изоляцию ошибок по файлам. Screening: DC offset, true peak/clipping, silence ratio, duration, channels, sample rate, SNR estimate, corrupt/truncated headers. SNR <40 dB — предупреждение; DC >1e-4 — корректирующая копия с сохранением оригинала; clipping — предупреждение; silence >50% — предложение trim. Все правки обратимы и журналируются.
Pitch detection предлагает root key/range, пользователь подтверждает mapping string/fret/velocity/articulation либо роль layer/loop. Source и license отображаются в карточке банка.

## 5. PGP и конвертация
PGP — проектный контейнер PPGU, но не обязательная промежуточная копия каждого файла. Сначала сравнить sidecar metadata + original files с packed container. 48 kHz/24-bit — профиль совместимости, не требование ресемплировать каждый источник. Оригинал сохраняется; конверсия только worker-потоком, с checksum и отчётом. Никакой деструктивной нормализации, trimming или loop slicing без обратимости/согласия. Tempo/key detection имеет confidence score и ручную коррекцию.

## 6. Preset -> Stack -> Bank
Preset хранит stackId и FX snapshot. Stack разрешает sampleId через индекс. Подготовленный Stack может прогреть ограниченное окно страниц. <10 ms — цель только для подготовленного Stack; cold disk read не блокирует смену пресета, страницы догружаются в фоне. Lifetime index/stack/page leases безопасен при rescan и смене пресета.

## 7. Этапы и acceptance
SB0 — аудит sampler/SFZ/page-cache/worker/lifetime/licensing; call graph, ownership map, RT checklist и baseline.
SB1 — schema + BankScanner: versioned index, incremental scan, immutable snapshot publication, stale/corrupt bank handling.
SB2 — LayerEngine: preallocated per-voice mix buffer, reader scheduling, gain/pan/tune/envelope/filters, off-thread phase analysis, voice invariants.
SB3 — ImportManager: formats, screening, cancel/error isolation, mapping proposal, source/license manifest.
SB4 — PGP prototype, loop metadata, Stack/preset integration.
SB5 — stress/regression: 1000 files, repeated rescans during playback, missing/corrupt files, cancellation, cache pressure, 6 layers x 16 voices, mono correlation, no clicks/dropouts, no RT allocations/locks/disk I/O.
Каждый этап получает verification report: цель, реализовано, не реализовано, тест, результат и ограничения. До тестов статус — TARGET.

## 8. Ограничения и конфликты
Бесконечная библиотека физически невозможна: поддерживается масштабирование без искусственного лимита продукта, при ограниченных cache/index budgets. Шесть слоёв не бесплатны; единый buffer экономит общие FX/voice overhead, но не стоимость чтения и DSP. Корреляция меняется во времени. 48 kHz/24-bit не всегда улучшает источник. Запрещены mutex, file I/O, decode, allocation и ожидание worker в audio callback. Сохраняются лимит 64 voices, режимы GOOD/NORMAL/NIGHT/ECO и существующий sampler/ROMpler core.

## 9. Текущий статус
В main уже присутствуют SFZ parser, metadata index, WAV page reader, page-cache slots, streaming service и StreamingVoiceReader. Это основание для расширения, но не доказательство реализации единого SoundBank, ImportManager, LayerEngine, PGP, unlimited-bank scan или phase analysis. Следующий кодовый шаг после SB0 — карта текущих API и тестов; затем минимальный BankScanner без изменений аудиопути.