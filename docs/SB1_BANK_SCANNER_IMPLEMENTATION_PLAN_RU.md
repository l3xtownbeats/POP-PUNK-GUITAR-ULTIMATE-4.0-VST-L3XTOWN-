# SB1 — BankManifest и BankScanner: план реализации

Дата: 11 ноября 2026  
Базовый коммит: `c6bd48fc0c0c7a03438e9b61cd9c19f76027725a`  
Ветка: `rev9-sb1-bank-scanner`  
Статус: инженерный контракт перед кодовой реализацией; не заявляет SB1 IMPLEMENTED.

## 0. Конфликт-чек

Приоритеты REV 9.0: безопасность → real-time guarantees → звук → ресурсы → UX.

SB1 не меняет текущий SFZ/WAV playback path и не меняет ownership существующих SamplePool snapshots. Scanner работает только в control/worker контексте. Manifest не становится владельцем sample bytes и не открывается audio thread. Это минимизирует риск регрессии sampler core.

## 1. Область SB1

Реализовать версионированный sidecar manifest и отдельную задачу сканирования библиотеки. На первом этапе индексируются WAV-файлы, поскольку именно этот формат подтверждён текущими SampleMetadataIndex/WavPageReader. Другие форматы остаются явными будущими расширениями, пока decoder и playback path не реализованы.

### Входит
- stable Bank ID и Sample ID;
- schema version и generator version;
- canonical bank root и относительные sample paths;
- размер файла, modification time, sample rate, channels, frames, bit depth, float flag;
- SHA-256 каждого sample;
- source, author, license identifier, license URL/evidence, verification timestamp и redistribution status;
- diagnostics по отдельным файлам;
- cancellation;
- incremental rescan с проверкой metadata fingerprint и сохранённого checksum;
- временная запись manifest + валидация + атомарная замена sidecar;
- immutable result object для control-thread публикации.

### Не входит
- декодирование новых форматов;
- изменение SamplePool/SamplerEngine playback;
- освобождение старых SamplePool snapshots;
- LayerEngine, phase analysis, loops, Stack, PGP;
- автоматическое присвоение CC0 или иного статуса лицензии;
- удаление/перемещение исходных файлов;
- сканирование сети/внешних symlink targets.

## 2. Предлагаемая модель данных

`BankManifest`
- `schemaVersion` — обязательное целое число;
- `bankId` — стабильный ID, полученный из нормализованного bank identity, не из display name;
- `displayName`;
- `canonicalRoot` — только для локального индекса, не использовать как portable identity;
- `source`: `sourceUrl`, `author`, `licenseId`, `licenseUrl`, `licenseEvidencePath`, `verifiedAtUtc`, `redistributionAllowed`, `evidenceNote`;
- `scan`: timestamp, scanner version, file count, error/warning counts;
- `samples[]`: `sampleId`, relative path, SHA-256, bytes, mtime, frames, sample rate, channels, bit depth, floating-point flag, scan status.

`sampleId` детерминирован из bankId + normalized relative path + content SHA-256. Переименование или изменение содержимого приводит к новому ID; display name не участвует в identity.

`bankId` создаётся один раз и сохраняется в manifest. При первом импорте используется deterministic namespace identity (canonical root + source identity), но manifest ID остаётся главным после его создания. Дубликат bankId с другим root/content требует явного разрешения, а не silent merge.

## 3. Инкрементальное сканирование

1. Валидировать root, получить canonical path и проверить, что это директория.
2. Не следовать symlink directory/file; каждый кандидат повторно проверять на containment внутри canonical root.
3. Прочитать существующий manifest только после проверки schema version и целостности структуры.
4. Для каждого WAV получить быстрый fingerprint: relative path + file size + modification time.
5. Если fingerprint совпал с предыдущей записью и включён режим trust-local-metadata, разрешено повторно использовать SHA-256, но это должно быть обозначено в отчёте как reused checksum. Для строгой проверки — пересчитать SHA-256.
6. Новый/изменённый файл: проверить заголовок и ограничения текущего mono/stereo path, вычислить SHA-256 в worker.
7. Ошибка одного файла не прерывает сканирование всего банка; она записывается в diagnostics.
8. Отменённая задача не публикует частичный manifest.
9. Перед commit повторно проверить, что файл не изменился между чтением header и checksum (size + mtime before/after; при изменении — повторить ограниченное число раз либо отметить unstable).
10. Создать временный файл рядом с целевым sidecar, закрыть и проверить его, затем атомарно заменить manifest. На платформах, где atomic replace отличается, использовать безопасный platform wrapper; не удалять валидный старый manifest до подтверждения нового.
11. Публиковать immutable scan result на control thread. Audio thread не читает JSON, не делает filesystem calls и не получает владение manifest writer.

## 4. Лицензирование и безопасность

- `redistributionAllowed` по умолчанию false/unknown.
- Наличие URL, скачивание без оплаты или локальная доступность не доказывают право перераспространения.
- Для factory package необходимы license file/evidence и проверка разрешения на коммерческое распространение, модификацию и bundling.
- Unknown/restricted assets могут быть перечислены в пользовательском локальном индексе, но не должны попадать в factory distribution.
- Scanner не исполняет скрипты, не распаковывает архивы и не следует ссылкам.
- Все относительные пути нормализуются; абсолютные пути и traversal segments запрещены.
- Checksum служит проверкой целостности, а не подтверждением лицензии.

## 5. Ownership и real-time

- BankScanner job владеет временными данными до завершения.
- Published manifest snapshot неизменяем.
- Control thread владеет публикацией и сменой активного index snapshot.
- Audio thread не владеет manifest и не удерживает pointers на объекты scanner.
- Не вводить общий worker service в SB1: SPSC topology текущего streaming service не меняется.
- Не удалять/перестраивать SamplePool snapshots.
- Отмена проверяется между файлами и во время hash chunks; hash chunk size bounded.
- Очереди progress/diagnostics ограничены; UI может пропускать промежуточные progress events, но не terminal result.

## 6. Тестовая матрица

| Тест | Ожидаемый результат |
|---|---|
| 1000 WAV файлов | Полный индекс без потери записей |
| Повторный неизменённый scan | Stable IDs; checksum reuse явно отмечен |
| Изменённый файл | Новый checksum и sampleId |
| Переименованный файл | Новый relative path и sampleId |
| Дубликаты имён в разных папках | Нет коллизии sampleId |
| Повреждённый WAV | Per-file error, остальные файлы индексируются |
| Пустой/короткий WAV | Диагностика, отсутствие invalid region |
| 4+ channel WAV | Явный unsupported status для текущего playback path |
| Permission denied | Диагностика; scan продолжается |
| Root symlink / child symlink escape | Не следовать за ссылкой |
| Path traversal в manifest | Manifest отклоняется |
| Unsupported schemaVersion | Не перезаписывать старый manifest |
| Отмена до конца scan | Старый manifest остаётся целым; partial не публикуется |
| Изменение файла во время hash | Retry или unstable-file diagnostic |
| Повторный scan 100 циклов | Нет роста неограниченных ресурсов |
| Audio callback во время scan | Не меняются allocation/lock/I/O invariants |
| Disk full / temp write failure | Старый manifest не повреждён |
| Checksum mismatch | Запись помечена stale/corrupt, не silent accept |

## 7. CI и приёмка

- Добавить отдельный JUCE-backed test executable, не смешивать с audio callback тестами.
- Включить SB1 тесты в Windows x64 Release CTest.
- Проверить предупреждения MSVC (/W4) и отсутствие новых compiler warnings.
- Проверить cancellation, повторные scans и manifest atomic replace на Windows.
- После CI дополнительно выполнить pluginval и host test; scanner unit tests не заменяют проверку VST3.
- До успешного CI результат обозначается CODED/UNVERIFIED, после успешных тестов — TESTED (CI), после ручного FL Studio test — HOST-VERIFIED.

## 8. Выходные артефакты SB1

Ожидаемые кодовые компоненты:
- `BankManifest.h/.cpp` — schema, serialization, validation;
- `BankScanner.h/.cpp` — cancellable worker job;
- `BankManifestTests.cpp` и `BankScannerTests.cpp`;
- CMake/CTest integration;
- актуальный verification report с commit SHA и ссылками на CI.

Критерий завершения: валидный manifest можно безопасно создать, прочитать, обновить и отменить без изменения существующего SFZ/WAV playback path и без нарушения real-time инвариантов.
