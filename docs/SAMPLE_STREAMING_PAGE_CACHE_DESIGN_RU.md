# Sample Page Cache — техническое решение и фактический статус

Дата актуализации: 11 ноября 2026  
Ветка: `rev9-sb1-bank-scanner`  
Статус: streaming WAV path реализован в коде; расширенный BankScanner и unified bank index ещё не реализованы.

## 1. Цель

Воспроизводить большие WAV-библиотеки без декодирования всей библиотеки в RAM. Audio callback не выполняет файловый I/O, декодирование, ожидание worker, блокировки или динамические аллокации. Cache miss не блокирует аудио и обрабатывается безопасным bounded fallback с диагностикой.

## 2. Фактически обнаруженная реализация

В текущем `main` подтверждены следующие компоненты:

- `SampleMetadataIndex` — metadata-only рекурсивное сканирование WAV-заголовков; декодирование аудио не выполняется.
- `SampleStreamingService` — bounded SPSC очередь запросов, один worker на экземпляр сервиса, WAV page decoding и диагностические счётчики.
- `PageCacheSlots` — фиксированные page slots, atomic pin/release, worker eviction только незакреплённых ready pages.
- `WavPageReader` — worker-side чтение WAV-страниц в заранее выделенную память.
- `StreamingVoiceReader` — чтение опубликованных страниц, удержание page leases и Hermite interpolation на границах страниц.
- `SamplePool` — production SFZ path: SFZ parse → WAV metadata index → streaming service → immutable RegionView snapshot.
- `SamplerEngine` — до 64 музыкальных voices; streaming readers объявлены так, чтобы освобождаться раньше владельцев streaming services.

Это не только проектная схема: SFZ/WAV streaming path существует в runtime-коде. Его наличие не доказывает реализацию универсального SoundBank, ImportManager или всех форматов.

## 3. Фактические ограничения и открытые риски

1. `SamplePool::maximumSnapshots = 128`. Старые snapshots удерживаются до уничтожения SamplePool для защиты raw RegionView pointers. Это ограничивает риск use-after-free, но может удерживать metadata и region vectors после многократных reload.
2. `maximumStreamingLibraries = 8`. Каждый уникальный root создаёт отдельный SampleStreamingService и worker thread.
3. Один cache: 256 slots × 4096 frames × 2 channels × float = 8 MiB. Восемь roots могут занять до 64 MiB только под page buffers, без metadata и служебных структур.
4. Streaming request queue имеет SPSC contract. Подключение дополнительных audio producers к одному сервису недопустимо без изменения и повторной проверки очереди.
5. Старые snapshots/services намеренно не освобождаются во время работы SamplePool. Нельзя убирать это удержание до реализации доказанного reader-quiescence/epoch reclamation протокола.
6. Metadata index поддерживает WAV. OGG fallback в SFZ resolver на одноимённый WAV не является прямым OGG streaming.
7. Bank IDs, checksum catalog, source/license evidence, loop catalog, layer/stack graph и единый пользовательский индекс ещё не реализованы.
8. Политика eviction сейчас фиксированная; полноценный bank-aware priority/LRU для множества банков не подтверждён.
9. Worker idle loop содержит периодическое ожидание; оптимизация wake-up допустима только после измерения и проверки корректного shutdown.
10. Нет подтверждения Windows Release/CTest/pluginval результата в рамках SB0-аудита.

## 4. Real-time и lifetime инварианты

- Audio-side request/acquire/release операции должны оставаться bounded и non-blocking.
- Защита чтения страницы обеспечивается page lease/pin, а не предварительным `containsReady()`.
- Worker может переиспользовать только страницу, для которой доказано отсутствие активных readers.
- Cache miss не ждёт диска и не возвращает stale pointer.
- SPSC очередь допускает только одного producer и одного consumer.
- Shutdown worker выполняется после quiescence producers.
- Нельзя освобождать старые snapshots/services, пока audio thread потенциально хранит RegionView/service pointers.
- Любое изменение voice-steal, panic, all-notes-off, reader detach/reattach должно проверяться на release всех page leases.

## 5. Следующие этапы

### SB1 — versioned BankManifest + BankScanner
- Описать schemaVersion, stable bank/sample IDs, относительные пути, file metadata, checksum, source/license evidence и scan diagnostics.
- Сканировать только вне audio thread.
- Проверять canonical root, не следовать symlink наружу, изолировать ошибки отдельных файлов.
- Поддержать отмену и повторное сканирование.
- Использовать временный manifest и атомарную публикацию только после полной валидации.
- Не менять существующий SFZ/WAV playback path на этапе SB1.
- Не включать файлы с неизвестной лицензией в factory distribution; пользовательские локальные файлы могут индексироваться с явным статусом unknown/restricted.

### SB2 — LayerEngine
- До 6 внутренних слоёв на музыкальный voice, заранее выделенные буферы, общий FX path.
- Анализ фазы/моносовместимости только вне audio thread.
- Внешний MIDI voice cap остаётся 64; внутренние readers/DSP cost учитываются отдельно.

### SB3 — ImportManager
- Worker pipeline, progress/cancel, per-file errors, screening и mapping proposal.
- Поддержка WAV/FLAC/OGG/AIFF/SF2/SFZ только после реализации и тестов соответствующих decoder/import paths.
- NKI не заявлять как поддерживаемый формат без законного и технически доступного импорта.

### SB4 — PGP container и Stack/preset integration
- Сравнить sidecar + оригинальные файлы с packed container до выбора формата.
- Сохранять оригиналы; конвертировать только по запросу, в worker, с checksum и отчётом.

### SB5 — stress/regression
- 1000+ файлов, повторные rescan во время playback, missing/corrupt files, cancellation, cache pressure.
- 6 layers × 16 voices, phase/mono checks, no clicks/dropouts.
- Проверить отсутствие RT allocation/locks/I/O и корректный worker shutdown.

## 6. Что запрещено

- Возвращать декодирование целого банка в RAM как основной путь.
- Выполнять файловый I/O, decode, mutex wait, allocation/deallocation или ожидание worker в audio callback.
- Убирать snapshot retention до доказанного безопасного reclamation.
- Считать metadata index подтверждением воспроизводимости любого формата.
- Включать в дистрибутив sample assets без подтверждённых прав коммерческого распространения.

## 7. Acceptance

Каждый этап получает отдельный отчёт: изменённые файлы/API, конфликт-чек, RT checklist, тест-кейсы, фактический CI результат и известные ограничения. Статус TARGET не переводится в IMPLEMENTED до проверки кода; статус VERIFIED — только после успешных тестов на целевой платформе.
