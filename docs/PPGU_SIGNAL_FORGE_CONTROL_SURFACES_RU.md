# PPGU SIGNAL FORGE — карта визуальных поверхностей и панелей управления

**Статус:** DESIGN CONTRACT / согласованная визуальная спецификация; не доказательство runtime-реализации.  
**Ветка:** main. **Основание:** единственный журнал Info.txt, MASTER PROMPT REV 17.0, MASTER_PRODUCT_SPEC_4_0_RU.md §§8, 15.9–15.15, SKIN/CONTROL_INVENTORY_MATRIX_RU.md, SKIN/control_schema.json и последние пользовательские уточнения.  
**Граница:** только GUI, компоновка и визуальное отображение. Не менять DSP, audio callback, sampler, APVTS IDs/semantics, MIDI routing, preset catalog, SoundBank или playback engine в рамках визуального этапа.

## 1. Единый художественный стандарт

Все панели используют один дизайн-язык, но собственную информационную иерархию:
- премиальный тёмный корпус: brushed graphite / black carbon / gunmetal, аккуратные металлические фаски;
- основной акцент — electric cyan/blue neon; violet/magenta — вторичный акцент;
- ON/selected/activity различимы не только свечением, но и подписью, формой и контрастом;
- типографика: крупные заголовки, компактные uppercase labels, читаемые значения и единицы;
- иерархия: header → рабочая зона → preview/context → footer/status;
- все цвета берутся из canonical 21-token palette. Не добавлять hardcoded HEX в PluginEditor и panel components;
- единые rotary, combo, sliders, buttons, tabs, chips, meters, scrollbars, focus outlines;
- ECO сокращает декоративную анимацию, не скрывая полезную информацию;
- проверять DPI 100/125/150%, resize и клавиатурный фокус;
- только оригинальные PPGU assets или материалы с подтверждёнными правами. Не использовать чужие логотипы, фотографии артистов и неочищенные artwork.

## 2. Главное окно — PLAY / INSTRUMENT

**Верхняя панель:** PPGU brand/build metadata, Previous/Next preset, Favorite, preset name/category, Search Presets, SKIN, Import Guitar, Auto Discover, Curator, Settings, About. Недоступные функции отображаются disabled с причиной, а не как активные.

**Основная зона:** preset browser с категориями/поиском/избранным; режимы MELODY и RHYTHM/PATTERN. PUNK GROUP не изображать работающим до реализации. Девять rotary строго по mapping:
1. GAIN → ampGain
2. TREBLE → eqHigh
3. MID → eqMid
4. BASS → eqLow
5. VELOCITY → velocity
6. DYNAMICS → dynamics
7. SWING → swing
8. HUMANIZE → humanize
9. ACCENT → accent

Семь FX bypass controls строго по inventory: COMPRESSOR/compressorOn, EQ/eqOn, DISTORTION/distortionOn, CHORUS/chorusOn, DELAY/delayOn, REVERB/reverbOn, CABINET IR/cabinetOn. Flanger остаётся отдельной FX EDIT страницей, но не получает восьмой bypass switch.

**Нижняя зона:** полностью доступная MIDI keyboard, pitch/mod controls, sync selector HOST / HOST START / INTERNAL, performance strip. Клавиатура не перекрывается ни одним окном/overlay. CPU/RAM и meters отображать только при наличии реальной telemetry.

## 3. FX EDIT

**Компоновка:** header с названием выбранного эффекта; слева вертикальный список модулей; в центре реальные параметры и специализированная графика; справа meters/telemetry только при подключённом источнике; footer с кратким назначением и точными значениями.

**Страницы и параметры:**
- Compressor: Threshold, Ratio, Attack, Release, Amount.
- EQ: Low/Mid/High, Low/Mid/High Frequency, Mid Q.
- Distortion: Drive, Tone, Mix.
- Flanger: Rate, Depth, Feedback, Mix; без отдельного enable switch.
- Chorus: Rate, Depth, Mix.
- Delay: Time, Feedback, Mix.
- Reverb: Room, Damping, Mix.
- Cabinet IR: Mix, Low Cut, High Cut; IR browser только после подключения реального каталога.

Каждый control должен иметь существующий APVTS ID/attachment. Не рисовать фиктивные GR/FFT meters, transfer curves, FX presets или Save actions без соответствующего runtime state.

## 4. AUTOCHORDS — согласованная компоновка

AutoChords — workspace гармонии и исполнения. Независимые оси: Harmony/Progression, Playing Style/Articulation, Rhythm Pattern, Playback Mode.

### Верхняя полоса — Harmony / Chord Progression
- ON/OFF и честный backend status.
- Key, Scale (Major / Natural Minor / Custom), progression preset.
- Ряд реальных названий аккордов, например Am – F – C – G, с degree labels i – VI – III – VII.
- Current Chord крупно; chord quality, root, inversion/voicing/formula только при наличии рассчитанных данных.
- Fretboard diagram только при корректной chord/fingering map.
- Circle of Fifths, Harmony Manager и Generation Manager — самостоятельные внутренние views; они не меняют активный режим/пресет без явного Commit.

### Левая колонка — Playing Styles / Articulation
Вертикальный список с группировкой и пояснениями: OPEN CHORDS, PALM MUTE, DOWNSTROKE, UPSTROKE, ALTERNATE DOWN-UP, POWER CHORDS, CHUGS, MUTED BRIDGE, SKANK/OFFBEAT, OCTAVE HOOKS, DOUBLE-STOPS, TREMOLO PICKING, ARPEGGIO/PICKING, SUSTAIN/AMBIENT, STOP-START.
Для каждого пункта — capability badge AVAILABLE / NOT IN THIS LIBRARY / BACKEND PENDING. Доступность зависит от реальных performance maps. TREMOLO PICKING не смешивать с TREMOLO effect.

### Правая колонка — Playback / Rhythm Modes
- Playback: STRUM / ARP / LATCH — активировать только после реализации.
- Rhythm: STRAIGHT / TRIPLET / SYNCOPATED / CUSTOM.
- Tempo Sync, chord duration/gate, velocity/accent, octave/register, swing/humanize, quantize — только при реальной связи с движком/host.
- Различать HOST SYNC, INTERNAL и UNAVAILABLE.

### Нижняя зона на всю ширину — Pattern Browser
Браузер паттернов находится внизу отдельной полноценной зоной, не спрятан в dropdown:
- Search, favorites, сортировка, genre/style filters;
- семейства Pop Punk, Skate Punk, Classic Punk, Alternative, Emo, Rock, Acoustic и др.;
- список: название, meter, subdivision, tempo range, feel, supported articulations, required guitar profile/library, loop length;
- Preview transport, waveform/step-grid и описание — только для реально доступных performance/audio data;
- паттерны оригинальные, без копирования защищённых riffs/loops артистов;
- выбор паттерна не меняет tone preset, guitar profile или проект без явного подтверждения.

### Footer / status
Current Chord, Key/Scale, Style, Pattern ID/name, Playback Mode, Tempo/Sync, source capability. Пока chord/performance MIDI routing не подключён, показывать PREVIEW ONLY — MIDI/PATTERN ROUTING NOT CONNECTED; selectors и preview остаются disabled. Экранная MIDI-клавиатура главного окна всегда полностью видна и доступна.

## 5. IMPORT GUITAR

- Слева: drag-and-drop / Browse, список поддерживаемых файлов и размер.
- Центр: instrument/package metadata — имя, профиль, формат, sample count, note range, velocity layers, round robin, articulations, sample rate/bit depth, размер.
- Справа: preview выбранного реального инструмента/файла, waveform после успешного decode.
- Footer: archive integrity, safe SFZ paths, mapping coverage, license/provenance, SHA-256, duplicate status.
- SCAN / VERIFY / PREVIEW / IMPORT / CANCEL отражают реальные этапы; не показывать Loaded до успешного импорта. Ручной импорт YELLOW не разрешает factory redistribution.

## 6. AUTO DISCOVER / SOUND HUNTER

Offline/non-realtime workspace, не запускается автоматически при открытии VST. Фильтры guitar-only: instrument family, articulation, style, format, license class/source. Результаты содержат source/provenance, конкретную лицензию, clearance basis, SHA-256, technical QA, duplicate status, GREEN/YELLOW/RED. AUTO ADD выключен по умолчанию и разрешён только для проверенных GREEN. Job states показывать только по фактическим worker events. Пока production DeepSearch/S6.5 integration не подтверждены — BACKEND PENDING; запрещены фальшивые radar, progress и найденные библиотеки.

## 7. CURATOR

Таблица ассетов: name, profile, source, license, clearance, SHA-256, sample coverage, articulation map, quality flags, duplicate. Фильтры GREEN/YELLOW/RED/GOLD; GOLD требует human listening approval. Inspect/audition/compare/approve/reject/package actions активны только при существующей реализации. RED нельзя curator-overridden; YELLOW остаётся manual review/quarantine.

## 8. SETTINGS

Существующие APVTS controls:
- POLYPHONY / VOICES → voices
- MASTER LEVEL → master
- GLOBAL TUNING → tuning
- OUTPUT PAN → pan
- OUTPUT GAIN (dB) → output
- PALM MUTE VELOCITY THRESHOLD → palmMuteThreshold

Вкладки GENERAL / AUDIO / MIDI / GUI допустимы как целевая организация только после подключения реальных настроек. Не добавлять фиктивные multiprocessing, multicore, buffer, quality или MIDI Learn controls. Resource paths — через REV 17.0 ResourceRootResolver, не абсолютными путями.

## 9. PERFORMANCE MONITOR

Read-only: last note, detected chord, note set, velocity, MIDI activity. Host BPM и bar:beat:tick — только после безопасного realtime snapshot; иначе HOST SYNC UNAVAILABLE. Никаких декоративных CPU/RAM values. Timer работает только когда панель видима; панель не отправляет MIDI и не меняет параметры.

## 10. SKIN GALLERY

Preview cards, theme name/family/layout, search, family filters, favorites, selected/default marker, Preview → Apply / Cancel. Темы из единого ResourceRoot по REV 17.0; AppData не источник. Ошибка темы вызывает понятное сообщение и fallback. 21 tokens должны доходить до всех панелей. Layout metadata не выдавать за runtime layout engine.

## 11. ABOUT / PRODUCT STATUS

Название, версия/build ID, формат, архитектура, лицензии, acknowledgements, manual/source links — из реальных metadata. Не показывать фиктивные version, Licensed, feature badges или CPU metrics.

## 12. Оконная система и приёмка

Соблюдать S2: X close, expand/restore, tiled layout, focus paging при нехватке места, сохранение normal bounds, восстановление, общая 21-token palette, безопасный editor teardown. Не создавать дубликаты панелей и attachments. AutoChords и все auxiliary windows не перекрывают клавиатуру.
Перед принятием каждого изменения: прочитать актуальный INFO и source; сопоставить controls с IDs/attachments; проверить отсутствие hardcoded HEX; проверить размеры/DPI/keyboard hit targets; запустить validators, Windows Release, полный CTest, pluginval strictness 5 и package на exact SHA; отдельно провести FL Studio 20 visual acceptance. CI PASS не переносится между SHA.

## 13. Честная текущая граница

| Surface | Подтверждено исходниками/INFO | Не считать готовым |
|---|---|---|
| Main | 9 rotary, 7 FX switches, preset browser, MELODY/RHYTHM, sync, SFZ loader, keyboard, 21-token visual propagation | PUNK GROUP, fake meters/CPU, host visual QA |
| FX EDIT | 8 pages, APVTS parameters, 7 approved enable switches | Fake IR browser/FFT/GR/FX preset save |
| AutoChords | Visual refresh; harmony/style/rhythm controls intentionally disabled pending backend | Playable chord generator, preview/MIDI routing, arbitrary chord diagrams |
| Import Guitar | Existing async SFZ loader panel | Unverified full archive/license pipeline UI |
| Settings | Six APVTS-bound controls | Unconnected GENERAL/AUDIO/MIDI/GUI options |
| Performance | Read-only MIDI note/chord/velocity monitor | Host BPM/position and CPU/RAM telemetry without source |
| Auxiliary windows | X/expand, tiled/focus paging, persistence/palette source + CI gates | Full interactive host/DPI/multi-monitor acceptance |
| Skin Gallery | Registry, 21-token decoding/visual propagation foundation | Complete gallery/rescan/hot-swap acceptance |
| Auto Discover / Curator / About | Reserved control IDs | Active scanner/curator/about workflow until runtime implementation |

