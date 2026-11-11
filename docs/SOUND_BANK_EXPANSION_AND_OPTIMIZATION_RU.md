# Sound Bank expansion and optimization

## Current source policy
Factory assets are restricted to recordings whose exact license and provenance permit commercial embedding and redistribution. Unknown, non-commercial, attribution-incomplete, or custom terms remain user-import-only/review-required. Artist and product names in the preset catalog are tonal references, not permission to copy commercial recordings, stems, riffs, or proprietary sample libraries.

## Source priority
1. Meta Open Sound Nylon and Steel: CC0 1.0, 348 and 375 samples respectively; retain upstream SFZ, design spec, LICENSE, manifest and SHA256SUMS.
2. FreePats FSBS electric guitar clean and distortion variants: CC0 per upstream LICENSE; retain SFZ and license files.
3. Additional SFZ catalogue entries only after checking each source's actual license, creator and redistribution terms. A repository being free to download is not enough.
4. Freesound: individual sound license must be exact CC0 before factory candidacy. API search requires an API key; original download requires OAuth2. No key/token is generated automatically by this project.
5. Commission original pop-punk performance recordings with a written grant covering commercial embedding, modification and redistribution.

## Library layout
- 00_DI_Raw
- 01_Acoustic
- 02_Clean_Pickup
- 03_Distortion_PowerChords
- 04_PalmMute
- 05_HighGain_Direct
- 06_Distortion_Sustains
- _SFZ
- _Unclassified

The indexer scans the existing source-library folders without moving or rewriting files. It records SHA-256, file size, category, license evidence, duplicate content and SFZ references in `Samples/SoundBank/catalog.json`. Unknown licenses remain visible but are not factory eligible. CC0 is only a legal candidate flag: listening, tuning, noise, clipping, root-note, articulation and mapping QA are still required.

## Run on Windows
```powershell
python tools/build_sound_bank_index.py --root Samples
```

Outputs:
- `Samples/SoundBank/catalog.json`
- `Samples/SoundBank/BUILD_REPORT.txt`

## Runtime optimization requirements
- Keep the source WAV/SFZ files in their original libraries; do not duplicate samples into each preset.
- Presets reference stable asset IDs or SFZ maps, not copied audio files.
- Scan/hash/index on a background worker; never scan folders, hash files, decode audio or access disk in the audio callback.
- Load only the selected preset's required regions; stream large samples through bounded page caches.
- Deduplicate by SHA-256 in the index; do not delete originals automatically.
- Separate factory assets from user-imported banks. USER presets and favorites reference local library IDs and survive library rescans.
- Expose library states: Installed, Indexed, Review Required, Missing Assets, Playable. Only mark Playable after a real MIDI note-on/audio-output check.
- Melody and Pattern share the same sound-bank IDs and FX chain. Melody selects note/articulation mappings; Pattern selects chord/performance mappings.
- Pattern performance recordings must carry BPM, meter, key, chord/voicing, stroke direction, articulation, transient offset and tail metadata. A single-note multisample must never be labelled as a complete strum/pattern library.

## Important limitation
The indexer creates a deterministic inventory; it does not itself download sample archives, generate missing guitar recordings, prove that every preset is playable, or change the C++ runtime. Build and host validation are still required after runtime integration.


## Дополнительные источники, проверенные для расширения (2026-11-11)

### Можно автоматически загружать в распространяемую библиотеку
Каталог SFZ Instruments указывает CC0-1.0 для трёх бесплатных библиотек Karoryfer; страницы авторов подтверждают назначение и содержимое. Добавлен загрузчик `tools/ppgu_build_library.py`, который:
- сверяет SPDX-лицензию репозитория через GitHub API;
- скачивает строго заданный официальный GitHub Release asset;
- требует SFZ-карты, аудиофайлы и явный CC0 в лицензии внутри архива;
- считает SHA-256 архива, сохраняет `SOURCE_PROVENANCE.json`;
- не перезаписывает уже установленную библиотеку, если URL и лицензия совпадают.

Новые источники:
- `sfzinstruments/karoryfer.black-and-green-guitars` — две hollowbody электрогитары; plucks, staccato, feedback, hammer-ons и behind-the-bridge articulation. Первичная категория: `03_Distortion_PowerChords` (папка источника, не утверждение что все зоны — power chords).
- `sfzinstruments/karoryfer.emilyguitar` — универсальная электрогитара, шесть SFZ-маппингов, включая chord/unison варианты. Категория: `02_Clean_Pickup`.
- `sfzinstruments/karoryfer.shinyguitar` — pickup и микрофонные записи, может давать яркий/агрессивный тембр при смешивании. Категория: `02_Clean_Pickup`.

Команда для Windows / Linux / macOS:
```bash
python3 tools/ppgu_build_library.py --github-only
```
Опциональный GitHub API token передаётся через `--token` или переменную `GITHUB_TOKEN` (необязателен для публичных репозиториев). Workflow Windows VST3 теперь выполняет эту команду перед индексацией Sound Bank и упаковкой артефакта.

Важно: загрузчик добавляет SFZ и исходные multisamples в Sound Bank, но сам по себе не создаёт валидированные музыкальные пресеты и не гарантирует что каждый паттерн уже воспроизводится. Дальше требуется проверить SFZ parser/runtime, реальные зоны и диапазоны нот, а затем привязать asset IDs к Melody/Pattern и выполнить аудио QA.

### Источники, которые нельзя включать в redistributable Sound Bank как есть
- Looperman: разрешает использовать лупы внутри производных музыкальных произведений, но прямо запрещает раздавать/продавать их как отдельные sample packs или выкладывать на собственный сайт. Поэтому оставляем только как источник вдохновения/пользовательского материала, не включаем в установочный Sound Bank.
- Splice — Mike Naran Pop Punk Riffs: страница показывает 65 звуков, но лицензия Splice запрещает сублицензировать/распространять sounds отдельно или использовать их в новом sample pack, конкурирующем со Splice. Не скачиваем и не встраиваем в VST3. Пользователь может применять законно скачанные экземпляры в своих музыкальных проектах согласно своей лицензии, но это не разрешает встраивание в наш продукт.
- Freesound Ax_Grinder pack 14939: проверять лицензию каждого файла отдельно. Включать только записи с точным CC0 и подтверждённым источником; название пакета само по себе не доказывает лицензию. API key позволяет искать/читать метаданные; оригинальное скачивание через API требует OAuth2. Скрипт пока не пытается обходить эту авторизацию.
- Freesound search CC0: это хороший кандидат для следующего этапа, но сначала нужны API credentials и per-sound license/provenance. API key — не OAuth access token.
- Selekt Audio guitar MIDI: каталог заявляет Public Domain, CC0 и CC-BY и выдаёт сертификаты для отдельных downloads. Добавлять MIDI можно только после сохранения сертификата конкретного файла, автора, точной лицензии и проверки права включать/перераспространять MIDI-файл внутри коммерческого продукта. Сам URL страницы гитарных MIDI не является лицензией на весь каталог.

### Параметры Freesound
`--token` в `ppgu_build_library.py` — это GitHub API token. Для Freesound предусмотрен отдельный аргумент `--freesound-token` или переменная `FREESOUND_API_KEY`; в текущей версии он только фиксирует наличие API-ключа и не скачивает оригиналы. Не вставляйте секретные токены в публичные команды, GitHub commits или логи. Не передавайте Freesound token в `--token`.

### Текущий статус
Добавлен автоматический этап загрузки трёх CC0 SFZ библиотек в Windows CI, затем общий Sound Bank indexer формирует каталог и отчёт. До успешного завершения нового workflow run нельзя считать архивы скачанными или сборку прошедшей; до проверки аудио в плагине нельзя объявлять новые пресеты полностью playable.
