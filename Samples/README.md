# PPGU Samples — portable natural-guitar library

This directory is the source-controlled inventory and layout for the PPGU guitar banks. The actual WAV/FLAC source payloads are fetched by the Windows VST3 CI workflow into its managed `Samples/` workspace, verified, and included in the portable package; large audio payloads are not committed as opaque Git blobs.

## Portable package contract

The portable distribution contains the VST3 bundle and its adjacent `SOUNDBANK/` directory. No installer, Setup.exe, mandatory AppData install, or external account is required. The plugin must resolve its bundled bank relative to its own portable package location. User banks remain separate from factory-cleared content.

## Current factory source batch — 11 natural guitar collections

All eleven collections are CC0 / public-domain candidates with source LICENSE and SFZ map retained. The latest source workflow successfully completed download and license-presence validation at commit `98e92674dfa2097e8ebd1caf34b97136fe06ee1a`; the Windows VST3 build from that run was still in progress at the time of this inventory. This is not a claim that the final plugin package has passed.

1. Meta Open Sound — Steel Guitar
2. Meta Open Sound — Nylon Guitar
3. FreePats — FSBS Electric Guitar Clean
4. FreePats — FSBS Electric Guitar Distortion 1
5. FreePats — FSBS Electric Guitar Distortion 2
6. FreePats — FSBS Electric Guitar Direct
7. FreePats — FSBS Electric Guitar Jazz
8. FreePats — Spanish Classical Guitar
9. Karoryfer — Black And Green Guitars
10. Karoryfer — Emilyguitar
11. Karoryfer — Shinyguitar

See `SOURCE_CATALOG.json` for canonical upstream repositories, license class, intended sound role, and verification state.

## Content families

- `Melody/`: chromatic single-note multisamples, velocity layers, round robins, sustain/release and articulations.
- `Pattern/`: natural recorded riffs and performances with tempo, key, meter, tuning, chord/voicing, stroke direction, articulation and timing metadata.
- `OneShots/`: isolated natural guitar attacks, power chords, muted hits, slides, harmonics, string/pick noises and transitions.
- `Loops/`: complete natural guitar phrases and rhythmic loops, dry and optionally processed, with BPM/key/meter and source provenance.
- `Acoustic/`: nylon, steel-string and classical guitar material.
- `Electric/`: clean, direct/DI, jazz, crunch, overdrive and distorted electric-guitar material.

These are content classifications, not claims that every category is already populated. Empty categories are filled only after source, license, technical and listening gates pass.

## Import and curation gates

- Natural recorded guitar audio only for the factory sample library; no generated/synthetic guitar substitutes.
- Guitar confirmation required; preserve the original file and mapping whenever practical.
- Record creator, source URL, version, retrieval date, exact license/evidence, file list and per-file SHA-256.
- Technical GREEN gate: SNR >= 40 dB, no clipping, DC offset <= 0.0001, integrity PASS.
- GREEN: eligible for automatic factory inclusion after evidence checks; GOLD: requires human listening approval.
- YELLOW: manual review only; RED: excluded from factory content. Looperman remains RED until explicit rights-holder permission covers commercial VST embedding and redistribution as a sample library.
- CC-BY requires exact attribution and an aggregate notices file. CC-BY-SA requires compatibility review. NC, incompatible ND, unclear custom terms and no-redistribution licenses are not factory eligible.
- A royalty-free music-use license does not itself permit redistribution of raw samples inside a VST.
- Do not extract or redistribute audio from commercial Kontakt / Session Guitarist products without explicit written permission.

## Scale and runtime

There is no artificial product-level cap on the number of user BANKs or total library bytes. Actual use is bounded by disk, host, CPU and memory. Index and scan in the background; stream/page large files; keep the audio callback free of disk I/O, decoding, blocking locks and heap allocation; use bounded preallocated voices/cache and safe eviction.


## Постоянное правило сохранности SAMPLES

**Samples/ — каноническая постоянная библиотека звуков проекта. Её нельзя удалять, очищать целиком, заменять пустой структурой или пересоздавать с нуля.** Разрешены только добавление новых банков и безопасная реорганизация с сохранением всех исходников.

- Сохранять аудио, SFZ, LICENSE/ATTRIBUTION, manifests и SHA-256.
- Сомнительные/дублирующиеся материалы изолировать для проверки, не удалять автоматически.
- CI cache может ускорять восстановление, но не заменяет библиотеку; ingest добавляет отсутствующие банки, существующие проверенные каталоги сохраняются.
- Portable release собирает SOUNDBANK/ непосредственно из Samples/.
- Удаление допустимо только для временных вложенных .git metadata клонированных upstream-репозиториев, не для аудио или provenance.
- Право на factory inclusion требует разрешения на коммерческое встраивание и распространение raw samples.

## Утверждённая последовательность: сначала заводская библиотека, затем пресеты

1. Сначала формируется и проверяется заводская библиотека из реальных гитарных аудиофайлов Samples/.
2. Каждый банк получает подтверждённый источник/лицензию, SFZ mapping, диапазон нот, velocity/articulation metadata, SHA-256 и результаты технического аудита.
3. Только после этого создаются заводские пресеты, непосредственно назначенные на реальные sample zones/banks из Samples/. Пресет не считается готовым, если он содержит только описание тембра или неподключённые параметры.
4. Каждый пресет проходит проверку: sample references разрешаются, ноты/velocity работают, звучание прослушано, уровни и FX согласованы, сохранение/загрузка проверены.
5. Factory preset library остаётся закреплённой за проверенной заводской Sample Library. Новые банки и пресеты пополняются добавочно; уже утверждённые материалы не удаляются.

