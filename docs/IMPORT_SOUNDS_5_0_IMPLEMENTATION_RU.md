# IMPORT GUITAR / BANK SOUNDS — PPGU 5.1

## Главная особенность продукта

**IMPORT GUITAR — центральная функция POP PUNK GUITAR ULTIMATE.** Пользователь импортирует собственные гитарные сэмплы, multisample-наборы, SFZ-инструменты или целые банки. Плагин не просто запоминает путь к внешней папке: он проверяет исходники, сохраняет оригиналы, преобразует поддерживаемые аудиоданные в оптимизированные playback-ресурсы внутреннего формата Bank Sounds и регистрирует их в собственной библиотеке.

Для пользователя результат выглядит как единая библиотека **BANK SOUNDS**: импортированные банки можно находить, прослушивать, выбирать и использовать в пресетах так же, как factory-звуки. Источник остаётся сохранённым, а производные ресурсы можно пересоздать при обновлении движка.

Внутренний формат — открытая, версионируемая структура банка (manifest + mapping + lossless/float playback cache), а не закрытый контейнер, который привязывает пользователя к одному приложению. Внутри плагина пользователь работает с банками, а не с техническими файлами.

## Пользовательский сценарий

1. Нажать IMPORT GUITAR и выбрать файлы, папку, ZIP или SFZ.
2. Указать имя банка и категорию либо подтвердить предложенную классификацию.
3. Прослушать найденные звуки и проверить назначение нот/velocity layers.
4. Запустить импорт: исходники копируются в Original, аудио декодируется/приводится к внутреннему playback cache, карта инструмента записывается в manifest.
5. После успешной проверки банк появляется в BANK SOUNDS и доступен для выбора в браузере.
6. Выбрать звук, назначить его на пресет и играть; повторный запуск DAW не требует повторного импорта.

Импорт не должен означать автоматическую публикацию пользовательских звуков в Factory Bank или разрешение распространять их вместе с плагином. Локальная пользовательская библиотека и redistributable factory content — два разных хранилища с разными правилами доступа.


Status: import entry point renamed to IMPORT GUITAR and factory bank connection remains available. The full managed importer pipeline below is the implementation contract; file/folder ingestion, transactional copy, format conversion, and User Sounds indexing are still staged work and must not be represented as complete.

## Product decision: preserve originals + build an optimized Bank Sounds cache

Never destructively convert or replace user files. Import copies the source into the bank's immutable Original area, then creates a derived playback cache. The source file remains available for re-import and future decoder improvements. Cache files are disposable and reproducible.

Suggested user-data root (Windows):
`%LOCALAPPDATA%/L3XTOWN/POP PUNK GUITAR ULTIMATE/`

Layout:
- `Library/Factory/` — bundled, license-cleared factory banks; read-only from the plug-in UI.\n- `Library/User/` — all banks imported by the user; shown together in the BANK SOUNDS browser, but clearly marked User.
- `Library/User/<bank-id>/Original/` — copied source files and SFZ documents.
- `Library/User/<bank-id>/Cache/` — normalized playback assets.
- `Library/User/<bank-id>/bank.json` — bank metadata, source hashes, conversion settings, diagnostics.
- `Library/Index/` — rebuildable metadata index.
- `Library/PreviewCache/` — disposable short previews/waveform peaks.

## Supported import formats

- WAV and AIFF: decode and normalize container representation only; never normalize loudness automatically.
- FLAC: lossless decode to cache PCM.
- OGG Vorbis: decode to cache PCM.
- MP3: optional decoder capability; if the build does not include a supported MP3 decoder, report unsupported format instead of pretending import succeeded.
- SFZ: preserve the original SFZ and referenced source files; parse supported opcodes into an instrument map. Unsupported opcodes and missing files are reported in an import report. Do not flatten a mapped instrument into unrelated one-shots.
- ZIP: safe extraction to a temporary staging folder, path traversal protection, archive-bomb limits, then normal file/SFZ discovery. Never execute files.

## Internal Bank Sounds representation\n\nEach imported bank has a versioned manifest describing bank identity, source provenance, instruments, regions, key/velocity maps, root notes, round-robin/sequence groups, loops, articulations, tags and cache references. Each playable region references a normalized cache asset by stable content ID, not by an absolute source path. The manifest records the source hash and conversion version so cache assets can be validated or rebuilt. SFZ maps remain semantic instrument maps; they are not flattened into a pile of unrelated audio files.\n\n## Playback cache format

Preferred cache: WAV PCM/float at the source sample rate and channel count, with 32-bit float internal representation where practical. Do not upsample by default. Preserve sample rate and channel count; resample only when an engine constraint requires it. Keep original bit depth and source hash in metadata. A lossless source (WAV/AIFF/FLAC) must not be lossy-reencoded. OGG/MP3 are necessarily decoded from their lossy source; the cache must not encode them again to a lossy format.

## Import pipeline

1. Choose files/folders/archive; show detected formats and estimated size.
2. Copy to a temporary staging directory; reject unsafe paths, symlinks escaping the bank root, oversized archives, corrupt files and unsupported formats.
3. Parse SFZ and resolve every sample path relative to the SFZ root. Keep key/velocity ranges, root keys, groups, sequence/round-robin and supported articulation/loop data.
4. Hash original files (SHA-256), detect duplicates, and offer reuse of existing assets by content hash.
5. Decode on a worker thread (never on audio callback); write cache files atomically to a temporary name, then rename on success.
6. Write bank.json and an import report including warnings, unsupported opcodes, missing references and conversion details.
7. Add the bank to the metadata index only after the complete transaction succeeds. On cancel/failure, clean temporary files and leave existing banks unchanged.
8. Refresh browser and preview cache. Do not preload the whole library into RAM.

## Audio-thread and performance rules

- No file I/O, decoding, heap allocation, blocking mutexes or directory scans in processBlock/audio callback.
- Background indexing and cache generation with cancellation and bounded worker count.
- Bounded voice count and page/cache memory budget; pin active voice pages; prefetch upcoming pages; safe cache eviction.
- Preview playback uses the same bounded streaming path or a separate low-priority preview voice.
- Import must not interrupt audio playback or alter the currently active preset.
- Re-link missing original paths from bank metadata; cache is rebuildable.

## UI acceptance

The single main button is labelled IMPORT GUITAR. It opens a modal/resizable import panel:
- Add Files, Add Folder, Add ZIP; drag-and-drop.
- Preview selected file; format, duration, sample rate, bit depth, channels and file size.
- Bank name and category; progress, current file, total files, cancel.
- Duplicate detection and "reuse existing sample" behavior.
- Summary with imported/skipped/failed counts and actionable diagnostics.
- User banks can be renamed, rescanned, exported as metadata package, or removed. Removing a bank must never delete Factory assets. Provide an explicit separate confirmation before deleting copied originals.
- Imported bank data and cache persist across DAW/plugin restarts.
- The UI must not claim a sound is categorized correctly unless the user confirms or classification is deterministic and explainable.

## License and provenance

User-imported material is local/private by default and is not automatically added to distributable Factory banks. Factory redistribution requires a separate per-file license review. Store source URL, author/rights holder, license identifier and exact license text/reference where available, retrieval date, archive hash, per-file SHA-256 and approval status. "Free download" is not proof of redistribution rights.

## Acceptance tests

- WAV/AIFF/FLAC/OGG supported imports produce valid cache files and can be previewed.
- MP3 capability is explicitly detected; unsupported decoder gives a clear error.
- SFZ retains mapping and reports missing samples/unsupported opcodes.
- ZIP traversal and oversized archive tests fail safely.
- Duplicate source hashes reuse existing asset without duplicate storage.
- Cancel/failure leaves no partial visible bank.
- Bank index rebuilds from bank.json; deleted cache regenerates.
- Audio thread remains free of disk access and blocking locks.
- User bank survives plugin/DAW restart; Factory bank cannot be removed from UI.
- Importing a bank does not alter version 4.0 assets or currently playing audio.

## 5.1 implementation decision — keep originals, create playback cache

Выбран гибридный вариант: **оригиналы сохраняются в исходном формате и в папке формата, а для движка создаётся отдельный производный playback cache только при необходимости**. Это надёжнее, чем массово перекодировать всё в один формат и терять исходную структуру SFZ.

Рекомендуемая структура пользовательского банка:

```text
%LOCALAPPDATA%/L3XTOWN/POP PUNK GUITAR ULTIMATE/Library/User/<bank-id>/
  Original/WAV/...
  Original/AIFF/...
  Original/FLAC/...
  Original/OGG/...
  Original/MP3/...
  Original/SFZ/...
  Cache/WAV32F/<content-id>.wav
  Instruments/<instrument-id>.sfz
  bank.json
  import-report.json
```

- WAV/AIFF остаются без потерь; WAV не перекодируется без необходимости.
- FLAC декодируется в WAV/float cache только после проверки декодера и успешного теста round-trip metadata.
- OGG/MP3 декодируются в cache для движка; исходный lossy-файл сохраняется отдельно, повторного lossy-кодирования нет.
- SFZ импортируется как карта инструмента: сохраняются относительные пути, key/velocity ranges, root key, round-robin/sequence и поддержанные articulation/loop данные. Нельзя превращать multisample SFZ в набор несвязанных one-shot файлов.
- Одиночный WAV становится самостоятельным Sound Asset с назначаемой пользователем Root Key и диапазоном нот. Автоматически угадывать root key по имени файла запрещено без подтверждённого naming convention; если ключ не определён, импортёр просит назначить его.
- Для набора отдельных WAV без SFZ создаётся черновой инструмент, но карта нот/velocity не публикуется до подтверждения пользователем.
- Дубликаты определяются по SHA-256 и используют общий content asset; названия банков и пользовательские метаданные остаются независимыми.
- В индекс попадает только полностью проверенный банк; отмена или ошибка откатывает staging transaction.
- Текущий playback engine гарантированно читает WAV SFZ assets. Остальные декодеры включаются по capability test; неподдерживаемый формат отображается как «обнаружен, но пока не готов к playback», а не как успешно импортированный звук.

## 5.2 User presets and Favorites

Пресеты пользователя отделены от Factory Bank:

```text
%LOCALAPPDATA%/L3XTOWN/POP PUNK GUITAR ULTIMATE/Presets/
  User/<preset-id>.ppgu.json
  Favorites/favorites.json
```

- В браузере должны быть отдельные источники Factory, User и Favorites; фильтр не должен менять активный пресет.
- Кнопка-звезда на выбранной карточке переключает Favorite; состояние сохраняется по стабильному preset ID, не по display name.
- Save и Save As сохраняют полное состояние параметров, режим MELODY/PATTERN, sample asset references, FX snapshot, style/artist metadata и пользовательские теги.
- Factory-пресет никогда не перезаписывается: Save для Factory означает Save As в User.
- User preset ссылается на bank-id и asset-id, а не на абсолютный путь к исходнику. При отсутствующем банке браузер показывает Missing Asset и предлагает Relink.
- Переименование/удаление User preset не удаляет Sounds Bank и не затрагивает Factory content.
- Favorites — отдельная коллекция ссылок; удаление избранного не удаляет сам пресет.
- JSON валидируется по schemaVersion; повреждённый файл помещается в quarantine и не блокирует запуск плагина.