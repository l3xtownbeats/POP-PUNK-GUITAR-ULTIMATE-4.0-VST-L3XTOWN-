# Журнал изменений — Sample Page Cache

Дата: 10 ноября 2026
Ветка: feat/m3-transport-scheduler-v2

## Решение принято
Добавлен технический контракт будущего потокового семплера: worker-only decode, фиксированные страницы, bounded cache/request queue, non-blocking audio lookup, prefetch, безопасное поколение BANK и deferred reclamation.

## Причина
Текущий SamplePool декодирует WAV целиком, хранит декодированные данные до уничтожения экземпляра и ограничен 1 GiB/128 snapshots. Увеличение этого лимита не решает масштабирование и создаёт риск исчерпания RAM.

## Изменённые документы
- docs/SAMPLE_STREAMING_PAGE_CACHE_DESIGN_RU.md — архитектура, ограничения real-time audio thread, план S1–S5 и критерии приёмки.
- docs/ЖУРНАЛ_ИЗМЕНЕНИЙ_STREAMING_RU.md — этот журнал.

## Статус проверки
Это архитектурный этап. Runtime-код SamplePool/SamplerEngine не изменён; сборка и тесты не запускались. Не считать streaming/page cache реализованным. Следующий этап — S1: контракт sample source и тестовый WAV page reader, затем bounded worker cache.

## S1 — WAV Page Reader: реализация первого рабочего слоя

- Добавлены `Source/Sampler/WavPageReader.h/.cpp`.
- `WavPageReader` открывает WAV по данным `SampleMetadata`, повторно сверяет размер, время изменения, число кадров, каналов и sample rate.
- Читает страницы фиксированного размера (по умолчанию 4096 кадров); последняя страница корректно ограничивается остатком файла.
- Нулевой размер страницы и запрос за концом файла отклоняются.
- Декодирование и выделение памяти происходят в вызывающем потоке; контракт класса запрещает вызов из `processBlock()`.
- `SampleMetadata` теперь хранит время последнего изменения файла, зафиксированное при индексации.
- Добавлен тест чтения первой и последней страницы, проверки данных stereo WAV, геометрии страниц и отказа для out-of-range/zero-size.
- CMake подключает reader к VST3 target и metadata/page-reader test target.
- Коммиты:
  - `ac1fe495` — API WAV Page Reader.
  - `1033032d` — реализация постраничного чтения.
  - `7cf8caa8`, `888f3d7e` — file identity в metadata index.
  - `b7fcbcde`, `93168bd3` — корректировка состояния и проверки reader.
  - `2d2d0420` — тесты страниц и декодирования.
  - `998c53f3` — CMake wiring.
- Проверки: исходники и тесты добавлены; запуск Windows CI/CTest пока не подтверждён. Поэтому сборочная корректность ещё не объявляется доказанной.
- Ограничение: это S1, не полноценный page cache. Reader сам по себе не реализует worker queue, deduplication, prefetch, eviction, lock-free publication или интеграцию с голосами. Текущий `SamplePool` по-прежнему загружает целые WAV.
- Следующий этап S2: bounded request queue и worker-owned readers/cache; до аудиоинтеграции отдельно проверить shutdown, гонки и время жизни страниц.


## S2a — ограниченная очередь запросов страниц

- Добавлен `BoundedSpscQueue<T, Capacity>`: фиксированный буфер, без heap allocation, mutex и ожидания в `tryPush/tryPop`.
- Очередь требует степень двойки и trivially-copyable элементы; полезная ёмкость равна Capacity - 1.
- Контракт строго SPSC: один producer (предполагается audio thread) и один consumer (worker). Несколько producer-потоков использовать её нельзя.
- При заполнении `tryPush` немедленно возвращает `false`; вызывающая сторона должна учесть пропуск запроса/underrun, а не повторять ожидание на аудиопотоке.
- Добавлен тест на пустую/полную очередь, FIFO, wrap-around и передачу 500 000 элементов между двумя потоками.
- CMake подключает отдельный тест `ppgu_bounded_spsc_queue_tests`.
- Коммиты:
  - `89f0f7e0` — SPSC queue header.
  - `f2ff20ad` — queue tests.
  - `987bc110` — CMake test wiring.
- Проверка: исходники добавлены; фактический запуск CMake/CTest и Windows CI ещё не подтверждён.
- Ограничение: это только bounded transport для запросов. Пока нет deduplication, worker lifecycle, page slots/cache, prefetch, eviction или публикации страниц; аудиодвижок по-прежнему не использует streaming. S2a не считать завершённым S2.


## S2b — фиксированные слоты page cache

- Добавлен `PageCacheSlots`: фиксированное число заранее выделяемых слотов с planar float-буферами. Создание/выделение памяти выполняется при подготовке cache, не в аудиоколбэке.
- Состояния слота: `empty → loading → ready → reading → ready`. Worker резервирует пустой слот или вытесняет только `ready`; слот `reading` нельзя перезаписать до освобождения lease.
- Worker получает заранее выделенный буфер через `writableData`, декодирует страницу и публикует её через release-store. Аудиопоток получает страницу через CAS acquire и читает данные до `release`.
- Добавлены тесты: публикация и чтение planar данных, защита закреплённой страницы, отказ при отсутствии свободного слота, отмена загрузки, вытеснение незакреплённой страницы, cache miss и некорректная конфигурация.
- Коммиты:
  - `c430810f` — PageCacheSlots API.
  - `732bf40c` — реализация слотов.
  - `8b9634d5` — тесты.
  - `c3617ae8` — уточнение проверки pinned slot.
  - `abd50fc9` — CMake wiring и включение реализации в VST3.
- Проверка: изменения опубликованы. Локальный CMake/CTest и Windows CI ещё не запускались/не подтверждены.
- Ограничения: пока нет фонового worker, очереди дедупликации, реестра открытых WAV readers, prefetch и интеграции с voice engine. Это безопасная основа хранения/lease, а не законченный streaming sampler. Контракт рассчитан на одного worker и один audio consumer.
