# SOUNDBANK — входящие источники гитарных сэмплов

Дата регистрации: 2026-11-12

Пользовательские источники для пополнения существующей библиотеки PPGU. Это очередь импорта, а не подтверждение наличия аудиофайлов.

| Источник | Предоставленный адрес | Результат попытки загрузки |
|---|---|---|
| Freesound | https://freesound.org/s/123456.wav | Загрузка не удалась; аудиофайл не получен |
| SoundBible | https://soundbible.com/1234567.wav | Загрузка не удалась; аудиофайл не получен |
| SampleSwap | https://sampleswap.org/sample/123456.wav | Загрузка не удалась; аудиофайл не получен |
| CC0Samples | https://cc0samples.com/guitar/pop-punk/1.wav | Загрузка не удалась; аудиофайл не получен |
| NoCopyrightSounds | https://nocopyrightsounds.com/123456.wav | Адрес восстановлен из переноса строки пользователя; загрузка не удалась |

## Состояние импорта

- Получено WAV-файлов: 0 из 5.
- Добавлено в Soundbank как аудио: 0.
- Ни один WAV не был подменён заглушкой или созданным фиктивным файлом.
- Ссылки сохранены для повторной загрузки; после получения файлов они проходят существующую проверку формата, целостности, метаданных и лицензии.
- Не создавать новые подсистемы: импортировать в существующую архитектуру Soundbank.



## Newly researched candidates — 2026-11-12

These are source leads and licensing findings, not downloaded or imported files. Keep them in the existing SoundBank intake queue and apply the existing REV 10.1 asset-level registry gate. Never mark GREEN from a landing-page claim alone.

| Library | Official source / download | Format / size | License and clearance | Decision now |
|---|---|---|---|---|
| Meta Steel Guitar | https://github.com/facebookincubator/Meta-open-sound/releases/tag/meta-steel-guitar-v1.0.0 | WAV 24-bit/48 kHz + SFZ; 375 samples; release ZIP SHA-256 caaa8a9c07217f4a82d1dfd9f5657c286155464b66bf4dd27797811c2558a909 | CC0 1.0; existing licenses/META_OPEN_SOUND_CLEARANCE.md | First acoustic/engine candidate; binary not downloaded in this pass |
| Meta Nylon Guitar | https://github.com/facebookincubator/Meta-open-sound/releases/tag/meta-nylon-guitar-v1.0.0 | WAV 24-bit/48 kHz + SFZ; 348 samples; release ZIP SHA-256 d76a23d438c7e6002aad61c564173141daac3a89f7bae5400f996d1a59b4eb76 | CC0 1.0; existing clearance record notes unsupported keyswitch/velocity-crossfade opcodes | Integration blocked until SFZ semantics pass; binary not downloaded |
| FreePats FSBS Electric Guitar Clean #1 | Page: https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html ; full WAV archive: https://github.com/freepats/electric-guitar-FSBS-clean/releases/download/2026-08-07/EGuitarFSBS-clean-SFZ%2BWAV-20260807.7z ; small WAV: https://github.com/freepats/electric-guitar-FSBS-clean/releases/download/2026-08-07/EGuitarFSBS-clean-bridge-small-SFZ%2BWAV-20260807.7z | Full WAV 456 MiB; small WAV 4.5 MiB; SFZ | Page identifies CC0 1.0; archive license/checksums still need local verification | Priority electric candidate; pending binary retrieval and screening |
| FreePats FSBS Electric Guitar Clean #2 (Jazz) | https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html | WAV 193 MiB; FLAC 63 MiB; SFZ | CC0 1.0 on source page; archive evidence pending | Pending download and listening |
| FreePats FSBS Electric Guitar Direct | https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html | WAV 153 MiB; FLAC 74 MiB; SFZ | CC0 1.0 on source page; archive evidence pending | Pending download and DI/amp-chain screening |
| Karoryfer Emilyguitar v1.001 | https://github.com/sfzinstruments/karoryfer.emilyguitar/releases/tag/v1.001 | 323 WAV, 24-bit/44.1 kHz, six SFZ maps; ZIP 103,484,350 bytes; https://github.com/sfzinstruments/karoryfer.emilyguitar/releases/download/v1.001/Karoryfer.Emilyguitar.v1.001.zip | CC0; creator page: https://shop.karoryfer.com/pages/free-emilyguitar | Cleared at source level; pending download, archive hash and technical screen |
| Karoryfer Black And Green Guitars v1.000 | https://github.com/sfzinstruments/karoryfer.black-and-green-guitars/releases/tag/v1.000 | 1,500+ samples; ZIP 481,838,512 bytes; https://github.com/sfzinstruments/karoryfer.black-and-green-guitars/releases/download/v1.000/Karoryfer_Black_And_Green_Guitars_1000.zip | CC0; creator page: https://shop.karoryfer.com/pages/free-black-and-green-guitars | Cleared at source level; pending download/listening |
| Karoryfer Shinyguitar v1.002 | https://github.com/sfzinstruments/karoryfer.shinyguitar/releases/tag/v1.002 | ZIP 368,411,844 bytes; https://github.com/sfzinstruments/karoryfer.shinyguitar/releases/download/v1.002/Karoryfer.Shinyguitar.v1.002.zip | CC0; creator free-samples page: https://shop.karoryfer.com/pages/free-samples | Cleared at source level; pending download/listening |
| Blue Jeans And Moonbeams | https://sfzinstruments.github.io/guitars/ | Electric/acoustic; SFZ catalogue entry | Catalogue says MIT; exact asset license scope/evidence not yet inspected | YELLOW / manual legal review |
| DAOWAVES 33 Rock Riffs Amped | https://daowaves.com/en/loops.html | 33 WAV riffs, 44.1 kHz/32-bit, 90/120/150 BPM, loop+tail; ZIP about 590 MB | Website says free commercial use; no proof of embedding/redistributing raw WAV inside a VST found | YELLOW; do not bundle without explicit permission |
| Pop Punk Loop Pack | Previously identified in research notes | 3,600+ loops, processed+DI WAV, one-shots/patterns | Royalty-free claim does not establish raw sample-library redistribution | YELLOW; require exact license/permission before bundling |

### Download result for this research pass

Direct release-asset transfer was blocked by the current file-transfer environment. Downloaded: 0; extracted: 0; converted: 0; added as audio: 0. Links and license findings are retained for the next transfer attempt. No fake or placeholder audio is created. For any retrieved archive, preserve source originals and SFZ; compute SHA-256; verify archive integrity; inspect per-file metadata/audio and license evidence; then register eligible assets in assets/sound_hunter/registry.json through the existing REV 10.1 validator. Conversion to the library's internal format is allowed only as a reproducible derived cache and does not remove source-license obligations.
