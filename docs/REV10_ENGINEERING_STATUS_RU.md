# REV 10.0 — инженерный статус и конфликт-чек

Дата: 11 ноября 2026  
Рабочая ветка: `rev9-sb1-bank-scanner`

## Принятая спецификация
REV 10.0 UNIFIED является текущим мастер-контрактом проекта и заменяет предыдущие требования REV 9.0/9.0-A там, где они расходятся. Приоритет: безопасность → real-time → звук → ресурсы → UX.

## Фактическое состояние
- Production WAV/SFZ streaming path существует: metadata index, WAV page reader, fixed page cache, bounded SPSC request queue, background streaming service, streaming voice reader, SamplePool.
- Текущий playback path ограничен WAV mono/stereo; это не означает поддержку FLAC/OGG/AIFF/SF2/SFZ импорта как единого PGP формата.
- BankManifest: первая реализация schema/JSON serialization/validation закоммичена вместе с регрессионными тестами; Windows CI ещё не подтвердил компиляцию/тесты. BankScanner (filesystem traversal, SHA-256 file hashing, cancellation, incremental scan, atomic manifest replacement) ещё не реализован.
- Layered Sound Engine, phase correlation, PGP packer, Import Guitar, Auto Discover, attribution report: не подтверждены как реализованные.
- Ранее созданные UI skin assets/validators в репозитории не равны 61 завершённой коммерческой теме; текущие CMake comments/validators указывают на 50 skin manifests. Требуется аудит и согласование с REV 10.
- Текущий VST3/host acceptance не подтверждён.

## CI — найденная блокирующая ошибка
Windows x64 Release job завершился failure на компиляции `ppgu_processor_pattern_tests`:
`JuceHeader.h: Cannot open include file: 'BinaryData.h'`.
Остальные sampler/test targets в этом job успели скомпилироваться, но CTest был пропущен, так как общий build завершился с ошибкой. Поэтому тесты не считаются пройденными.

Исправление в CMake: integration-test target теперь явно линкует `PPGUPresetData`, чтобы JUCE-generated BinaryData dependency была доступна тестовой цели. Изменение закоммичено отдельно; необходимо дождаться нового Windows CI.

## Следующий SB1 контракт
SB1 должен быть control-thread-only и не менять SamplePool/SamplerEngine playback path. Manifest обязан хранить schema version, stable bank/sample IDs, relative paths, SHA-256, WAV metadata и provenance/license fields. Redistribution разрешено только при явном доказательстве; default = unknown/false. Отмена и ошибка записи не должны заменять последний валидный manifest. Symlink/path traversal запрещены. Checksum не доказывает лицензию.

## Конфликт-чек REV 10
1. §0.4/§0.5: scanner/manifest/hash/JSON/file I/O только вне audio thread — совместимо с существующим streaming design.
2. §3: автопоиск в IDLE спит; сканирование только по запросу/расписанию worker — необходимо внедрить отдельно.
3. §6.3: один unified index — текущий SampleMetadataIndex пока только WAV metadata scanner; нельзя объявлять его полноценным index.idx.
4. §6.4: слойность до 6 слоёв — пока не доказана; не добавлять обработку каждого слоя отдельно в FX.
5. §6.5: import/discovery — форматы и сеть пока не реализованы; не притворяться поддерживаемыми.
6. §7: 61 тема — проверить фактическое количество и контракты; текущая репозитория документация содержит расхождение 50 vs 61.
7. §9.9: CI failure блокирует статус TESTED и дальнейшее объявление SB1 завершённым.

## Изменения этой ревизии
- Исправлена зависимость BinaryData для processor integration test target в CMake.
- Этот документ фиксирует фактологический baseline и список конфликтов.


## SB1 implementation increment (CODED / UNVERIFIED)
- Added `Source/Sampler/BankManifest.h/.cpp`: versioned manifest data model, JSON serialization/deserialization, schema validation, mono/stereo metadata bounds, SHA-256 string shape validation, relative-path traversal rejection.
- Added `tests/BankManifestTests.cpp`: round-trip, default-deny redistribution, traversal, malformed hash, unsupported schema preserving destination.
- Added the module to the plugin source list and a dedicated CTest target.
- Important: this is the manifest data-contract layer only. It does not yet calculate hashes, scan directories, write atomically, cancel jobs, or publish index snapshots.
- The first CI run for this increment was superseded/cancelled by subsequent pushes; the latest run must complete before status can be upgraded.


## Manifest hardening increment (IMPLEMENTED / CI PENDING)
- `BankManifest::fromJson` now rejects non-positive fileBytes/frames/channels and negative bits-per-sample before unsigned conversion, preventing malformed negative JSON integers from wrapping to huge unsigned values.
- `BankManifest::validate` now rejects duplicate sample IDs, duplicate relative paths and negative modification timestamps.
- Added regression cases for duplicate records and negative fileBytes.
- Commits: `4217a644baa1b302f96a0b57c60ecfa8ba2fe8f9` (validation hardening), `7fdaaaec31057bd578d6a48d057cbdc628e3b0bb` (tests).
- Status remains IMPLEMENTED / CI PENDING; no successful Windows build or CTest result is claimed.
- Previous workflow run `43011601549` conclusively ended CANCELLED: configure passed, build was cancelled, CTest/pluginval/package steps were skipped. It is not a passing run.
