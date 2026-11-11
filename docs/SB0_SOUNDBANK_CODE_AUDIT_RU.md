# SB0 — SOUNDBANK: аудит текущей реализации
Дата: 11 ноября 2026 | Ветка: main
Статус: аудит исходного кода; не приёмка SB1–SB5.

## 1. Подтверждённые компоненты
В main обнаружены и прочитаны:
- Source/Sampler/SampleMetadataIndex.{h,cpp}: рекурсивный metadata-only scan WAV, метаданные кадров/частоты/каналов/битности/размера/mtime; не декодирует аудио.
- Source/Sampler/SampleStreamingService.{h,cpp}: фиксированная SPSC очередь запросов, один worker thread на экземпляр сервиса, WAV page decoding, статистика запросов/cache hit/miss/decode errors/cache full.
- Source/Sampler/PageCacheSlots.{h,cpp}: фиксированные слоты, атомарный pin/release, worker eviction только unpinned ready pages, ограничение 256 MiB на экземпляр cache.
- Source/Sampler/StreamingVoiceReader.{h,cpp}: page-backed reads, prefetch соседних страниц, Hermite interpolation через границы страниц.
- Source/Sampler/SamplePool.{h,cpp}: SFZ parse -> metadata index -> streaming service -> RegionView; audio thread получает RegionView из опубликованного Snapshot.
- Tests: SampleMetadataIndexTests, SampleStreamingServiceTests, PageCacheSlotsTests, SamplePoolTests.
Это реальный streaming путь для SFZ/WAV, а не только проектная документация. Заметка SAMPLE_STREAMING_PAGE_CACHE_DESIGN_RU.md частично устарела: её утверждение, что SamplePool всё ещё загружает целые WAV в AudioBuffer и ограничен 1 GiB, не соответствует текущему SamplePool.cpp. Заметку следует синхронизировать отдельным документальным коммитом после полной проверки legacy path.

## 2. Текущие конкретные ограничения
- SamplePool.h: maximumSnapshots = 128; snapshots удерживаются до уничтожения SamplePool. Это защищает raw RegionView pointers, но повторные загрузки банков могут удерживать память метаданных и region snapshots.
- maximumStreamingLibraries = 8; каждый новый уникальный root создаёт отдельный SampleStreamingService и отдельный std::thread.
- streamingCacheSlots = 256, pageFrames = 4096, maxChannels = 2. Один cache занимает 8 MiB; восемь roots — до 64 MiB только под sample page buffers.
- Один SampleStreamingService имеет один SPSC producer/consumer contract. Нельзя подключить к нему несколько audio producers без изменения queue design/ownership.
- loadSfz выполняется под loaderMutex, сканирует каталог, строит новый snapshot и запускает worker вне аудиопотока. Публикация activeSnapshot — release-store. Старые snapshots/services удерживаются до destruction.
- SampleMetadataIndex сейчас принимает WAV, не является универсальным индексом FLAC/OGG/AIFF/SF2/loops.
- В SamplePool есть fallback для SFZ OGG reference к одноимённому WAV, но это не означает прямую поддержку OGG streaming.
- Нынешний metadata index содержит file, frames, sampleRate, channels, bitsPerSample, floatingPoint, fileBytes, modificationTimeMs. Нет bank/license/source evidence, stable bank IDs, loop catalogue, layer/stack graph.
- PageCacheSlots фиксирован и ограничен; полноценная bank-aware LRU/priority policy для большого числа банков не подтверждена. Worker выбирает пустой или незакреплённый ready slot.
- Worker idle loop использует sleep_for(1ms), что может создавать периодические пробуждения; перед оптимизацией требуется измерение и корректный wake-up/shutdown протокол.

## 3. RT / lifetime оценка
Положительные признаки: audio-side requestPage / tryAcquire / release имеют noexcept и bounded operations; нет file I/O/decode/mutex в этих методах. Page read lease pins page while reading; worker can evict only when slot access word is exactly ready/unpinned. StreamingVoiceReader хранит фиксированный набор leases. Audio cache miss не ждёт диск.
Открытые риски:
- Lifetime SampleStreamingService и RegionView зависит от удержания snapshots/libraries до destruction. Это предотвращает раннее освобождение при reload, но создаёт рост памяти.
- Нельзя удалять старые snapshots до внедрения доказуемого reader-quiescence / epoch reclamation handoff между audio thread и control thread.
- SPSC допустима только при одном producer. Перед общим worker service нужно доказать топологию producer threads либо ввести bounded MPSC с RT-доказательством.
- containsReady() — подсказка duplicate suppression, не защита lifetime страницы. Истинная защита чтения — tryAcquire lease.
- Проверить release/reattach/voice-steal/panic paths StreamingVoiceReader на отсутствие утечек page leases.
- Проверить queue overflow/cache full: счетчики доступны QA/UI, underrun policy не даёт зависаний или stale samples.
- Worker shutdown требует producer quiescence; порядок уничтожения processor/voices/pool должен быть проверен отдельно.

## 4. SoundBank gap matrix
| Требование | Текущее состояние | Статус |
|---|---|---|
| WAV metadata scan | Есть | IMPLEMENTED, базовый |
| WAV page streaming | Есть | IMPLEMENTED, SFZ path |
| Bounded page cache + pin leases | Есть | IMPLEMENTED, фиксированный cache |
| Incremental unified index | Нет | TARGET |
| Bank IDs/source/license evidence | Нет | TARGET |
| UserBank discovery/rescan | Нет | TARGET |
| FLAC/OGG/AIFF direct import | Не подтверждено; scanner WAV-only | TARGET |
| SF2 import | Не подтверждено | TARGET |
| Loop catalogue/tempo/key/bars | Нет | TARGET |
| Layer/Stack schema | Нет | TARGET |
| LayerEngine pre-allocated voice mix | Нет | TARGET |
| Phase/mono analysis | Нет | TARGET |
| ImportManager + UI + cancel/progress | Нет | TARGET |
| PGP container | Нет | TARGET |
| 1000-file stress and live rescan tests | Нет подтверждения | TARGET |

## 5. План SB1 — безопасный минимальный шаг
1. Зафиксировать интерфейсы SampleMetadataIndex, SamplePool, SampleStreamingService и ownership graph.
2. Спроектировать versioned bank manifest sidecar, не меняя текущий SFZ/WAV playback path.
3. Реализовать BankScanner как non-audio worker job: canonical root validation, WAV metadata scan, stable IDs/checksum, source/license metadata, per-file errors.
4. Scanner пишет временный manifest; после полной валидации публикует immutable snapshot на control thread.
5. Сначала только один активный snapshot/scan result без попытки освобождать существующие SamplePool snapshots.
6. Добавить тесты: 1000 WAV metadata files, duplicate IDs, modified/deleted files, corrupt headers, permission errors, symlink escape, cancellation, repeated scan, unchanged audio callback invariants.
7. После baseline — отдельно решить unified worker и lifetime reclamation. Не снимать 8-root/128-snapshot caps до прохождения этих тестов.

## 6. Acceptance checklist
SB0 закрывается только после проверки code path и ownership map, RT checklist, issue/test plan, синхронизации устаревшей документации и получения Windows x64 Release + CTest baseline.
На дату отчёта SB0 аудит выполнен по указанным исходникам, но Windows Release/CTest результаты здесь не заявляются. SB1 ещё не реализован.