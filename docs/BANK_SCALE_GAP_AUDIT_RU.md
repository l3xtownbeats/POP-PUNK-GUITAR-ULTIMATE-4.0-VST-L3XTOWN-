# BANK scale gap audit — PPGU 4.0

Дата проверки: 10 ноября 2026  
Ветка: feat/m3-transport-scheduler-v2  
Проверенный HEAD до этого документа: 31e2261db84d3b3d3a94e37bb5475a2272705719

## Подтверждённые факты из текущего SamplePool

В `Source/Sampler/SamplePool.h` сейчас заданы:
- `maximumSampleStorageBytes = 1 GiB` — жёсткий предел декодированных sample buffers на один экземпляр;
- `maximumSnapshots = 128` — жёсткий предел опубликованных snapshot-ов за время жизни экземпляра.

В `Source/Sampler/SamplePool.cpp` WAV загружается целиком в `juce::AudioBuffer<float>`. Для каждого SFZ region разрешены WAV до 120 секунд, 1–2 канала при декодировании; дисковое чтение выполняется в loader thread. Эти ограничения не равны 20 GB release package cap, но фактически не позволяют безусловно загрузить большой пользовательский BANK в RAM.

## Вывод

Заводской контент ≤20 GB — ограничение размера установочного комплекта. Оно не должно превращаться в требование держать все WAV в RAM одновременно. Пользовательские BANK не имеют искусственного лимита по суммарному размеру на уровне формата/каталога, но фактическая доступность ограничена диском, файловой системой, адресным пространством, CPU и настройками памяти.

## Требуемое исправление архитектуры

1. Разделить metadata index и audio residency: при сканировании читать заголовки и строить индекс, не декодируя все WAV.
2. Ввести sample source abstraction (resident buffer / streamed file source / decoded cache page).
3. Реализовать worker-side page cache с заранее выделенными буферами, prefetch и безопасной публикацией страниц.
4. Audio callback только читает заранее подготовленные страницы; никаких disk I/O, decode, heap allocation, mutex/condition wait.
5. При cache miss — безопасное silence/voice starvation diagnostic, не блокировка аудиопотока.
6. Защитить активные страницы от eviction; освобождать неиспользуемые по LRU/clock вне audio thread.
7. Ограничения snapshot count и reload lifetime заменить управляемой моделью generation/refcount/epoch reclamation, чтобы не удерживать все версии банков до уничтожения SamplePool.
8. Добавить тесты: банк больше 1 GiB по суммарному исходному WAV, длительные файлы, repeated reload, cache pressure, simultaneous voices, sample-rate conversion, cancellation and shutdown.
9. Измерить CPU/RAM/disk throughput и отсутствие glitches в FL Studio 20; не заявлять streaming production-ready до этих проверок.

## Текущий статус

- Это аудит и план исправления, не реализация streaming.
- Текущий SamplePool остаётся resident-buffer loader с пределом 1 GiB и 128 snapshots.
- Не удалять предел простым повышением числа: это не решает потребление RAM и lifetime snapshots.
- Не загружать все 20 GB в память.
- Не заявлять отсутствие лимита пользовательских BANK в текущем движке, пока streaming/page cache и безопасное управление snapshot lifetime не реализованы.
