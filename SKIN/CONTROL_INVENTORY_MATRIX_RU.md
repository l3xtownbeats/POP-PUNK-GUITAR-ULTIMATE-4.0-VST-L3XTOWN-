# PPGU SKIN — инвентарная матрица контролов (REV 14.0)

Дата актуализации: 12 ноября 2026. Источник истины по обязательным ID и layout grid: `control_schema.json`. Источник фактической реализации: `Source/PluginEditor.h/.cpp` и `Source/PluginProcessor.cpp`.

## 1. Фактический GUI в рабочей ветке REV.17 (`POP-PUNK-GUITAR-ULTIMATE-VST-REV.17`)

В текущем редакторе подтверждены:
- Заголовок PPGU / L3XTOWN AUDIO LAB.
- Каталог factory presets: selector, category filter (ALL / MELODY / PATTERN), search. Фильтрация не должна загружать другой preset.
- LOAD SFZ: выбор файла и асинхронная загрузка через processor.
- Library/preset status label.
- MELODY / PATTERN tabs и mode ComboBox (APVTS `playMode`).
- Девять rotary с APVTS attachments.
- Семь FX bypass toggles с APVTS attachments.
- Start/sync selector (HOST / HOST START / INTERNAL; APVTS `syncMode`).
- On-screen MIDI keyboard.
- Кнопка SKIN открывает Premium Skin Gallery: 102 registry themes, preview cards, search, family filter, active marker, click-to-apply; Ctrl+K открывает ту же панель. Stable skin ID сохраняется в UI Properties.

Gallery implemented with previews/search/family filter. Favorites, full gallery keyboard navigation, background rescan, 150 ms fade and ECO policy remain NOT IMPLEMENTED.

## 2. Обязательные REV 14 control IDs

| Control ID | Реальный компонент / APVTS | Реальная функция | Статус |
|---|---|---|---|
| preset_browser | presetBox + categoryBox + presetSearch | Выбор preset; category/search фильтруют список без смены звука | IMPLEMENTED |
| mode_selector | modeBox + melodyTab + patternTab / `playMode` | Переключение MELODY/PATTERN | PARTIAL: PUNK GROUP отсутствует |
| knob01 | knobs[0] / `ampGain` | Amp Gain | IMPLEMENTED |
| knob02 | knobs[1] / `eqHigh` | Treble EQ | IMPLEMENTED |
| knob03 | knobs[2] / `eqMid` | Mid EQ | IMPLEMENTED |
| knob04 | knobs[3] / `eqLow` | Bass EQ | IMPLEMENTED |
| knob05 | knobs[4] / `velocity` | Velocity response | IMPLEMENTED |
| knob06 | knobs[5] / `dynamics` | Velocity dynamics | IMPLEMENTED |
| knob07 | knobs[6] / `swing` | Pattern swing | IMPLEMENTED |
| knob08 | knobs[7] / `humanize` | Pattern humanization | IMPLEMENTED |
| knob09 | knobs[8] / `accent` | Pattern accent | IMPLEMENTED |
| fx_switch01 | fxButtons[0] / `compressorOn` | Compressor bypass | IMPLEMENTED |
| fx_switch02 | fxButtons[1] / `eqOn` | EQ bypass | IMPLEMENTED |
| fx_switch03 | fxButtons[2] / `distortionOn` | Distortion bypass | IMPLEMENTED |
| fx_switch04 | fxButtons[3] / `chorusOn` | Chorus bypass | IMPLEMENTED |
| fx_switch05 | fxButtons[4] / `delayOn` | Delay bypass | IMPLEMENTED |
| fx_switch06 | fxButtons[5] / `reverbOn` | Reverb bypass | IMPLEMENTED |
| fx_switch07 | fxButtons[6] / `cabinetOn` | Cabinet processing bypass | IMPLEMENTED |
| midi_keyboard | keyboard | On-screen MIDI note input | IMPLEMENTED |
| LCD_display | statusLabel | Preset/library/status feedback; currently not a dedicated LCD widget | PARTIAL |
| LED_indicator | none | Activity/status indicator; no real meter/activity source is wired | NOT IMPLEMENTED |
| SKIN_button | skinButton + `PPGUSkinGalleryPanel` / Ctrl+K | Opens searchable 102-theme preview gallery; applies selected visual palette and persists skin ID | IMPLEMENTED (favorites/keyboard browsing deferred) |
| Import_Guitar | loadButton | SFZ file chooser + async library load | IMPLEMENTED |
| Auto_Discover | none | Reserved for production Sound Hunter/S6 discovery | NOT IMPLEMENTED |
| Curator | none | Reserved for production library curation/review | NOT IMPLEMENTED |
| Settings | settingsButton + `PPGUSettingsPanel` | Persisted auxiliary panel scale and spacing; restore SIGNAL FORGE default; jump to Skin Gallery | IMPLEMENTED (UI-only) |
| About | none | No About panel in current editor | NOT IMPLEMENTED |

A skin may map every required ID, but that does not create the missing processor/UI feature. Missing features must be shown as unavailable/disabled with an honest reason; no fake LED activity, fake scanner, fake curator or active dummy buttons.

## 3. Additional existing controls that must remain visible

| Control | Component | APVTS / local state | Status |
|---|---|---|---|
| Preset category | categoryBox | local UI state | IMPLEMENTED |
| Preset search | presetSearch | local UI state | IMPLEMENTED |
| Transport sync | startModeBox | `syncMode` | IMPLEMENTED |
| Library feedback | statusLabel | diagnostic | IMPLEMENTED |
| Product brand | titleLabel / subtitleLabel | non-APVTS | IMPLEMENTED |

## 4. Nine-knob mapping — exact order

1. GAIN → `ampGain`
2. TREBLE → `eqHigh`
3. MID → `eqMid`
4. BASS → `eqLow`
5. VELOCITY → `velocity`
6. DYNAMICS → `dynamics`
7. SWING → `swing`
8. HUMANIZE → `humanize`
9. ACCENT → `accent`

## 5. Seven FX toggles — exact order

1. COMPRESSOR → `compressorOn`
2. EQ → `eqOn`
3. DISTORTION → `distortionOn`
4. CHORUS → `chorusOn`
5. DELAY → `delayOn`
6. REVERB → `reverbOn`
7. CABINET IR → `cabinetOn`

Other APVTS parameters exist for FX detail and engine behavior, but their existence does not mean there is a corresponding GUI control. Skins must not reassign the seven visible bypass buttons to those detail parameters.

## 6. Further APVTS inventory

The processor declares (among others) `master`, `voices`, `palmMuteThreshold`, `tuning`, `pan`, `output`, `articulation`, `pickStyle`, `pitchBendTarget`, `vibrato`, `slideTime`, `legato`, `hammerPull`, `scaleRoot`, `scaleMode`, `monoMode`, `fretNoiseOnChordChange`, `slideModifier`, `slideVelocitySensitivity`, `modWheelVibrato`, `melodyPatternSlot`, `endingLength`, `palmHitOnEnding`, `stringSlapOnEnding`, `patternSlot`, `patternLength`, `swingBase`, `globalPpqOffset`, `chordSlashBass`, `singleMidiNoteAsChordRoot`, `preserveHumanTiming`, `dnaChugDensity`, `dnaHammerPullProbability`, `dnaStringSpread`, `dnaBeatAccent`, `dnaFxTaste`, `fxSlot1…fxSlot7`, FX detail parameters, `bpm`, `startOnFirstChord`, `patternQuantize`, `doubleTrackOn`, `doubleTrackMix`.

These IDs are processor parameters, not automatically visible controls. The Skin project must not add/rename IDs or claim controls are implemented unless a real component/attachment exists.

## 7. Layout contract

REV 14 defines 12 columns × 16 rows, zero-based x/y with w/h spans. Layout metadata currently describes design placement only; existing JUCE bounds and hit targets remain authoritative until a dedicated layout engine consumes the map. No skin may change APVTS attachments, control meaning, parameter values, MIDI behavior or audio processing.

Current reference design: Skin_052 / SIGNAL FORGE / carbon_studio / STUDIO_CONSOLE. Skin Studio manifests 52–102 are accepted by ThemeManager; their grid metadata is still design metadata and does not reposition real JUCE controls.

## 8. Acceptance state

- Source inventory and exact APVTS mappings: AUDITED.
- REV 14 mandatory ID schema: CREATED.
- Skin_052 parity map: DESIGNED.
- Runtime parity assertion against actual JUCE component tree: NOT IMPLEMENTED.
- Full skin gallery / rescan / 150 ms hot-swap / ECO: NOT IMPLEMENTED.
- FL Studio 20 host acceptance: NOT TESTED.


## 9. Обязательное правило: журнал важнее случайного элемента макета

Состав GUI определяется утверждённым журналом REV.17, master specification и этой описью. Макет является точным визуальным эталоном для предусмотренных элементов, но не разрешает вводить неописанные функции.

- Элемент, который присутствует только на макете и не предусмотрен журналом/спецификацией, разрешено убрать или заменить обязательным элементом.
- Элемент, обязательный по журналу, но отсутствующий на макете или в коде, добавить в соответствующую зону, подключить к реальному компоненту/attachment/action и проверить.
- Не удалять underlying DSP/engine/sampler/MIDI/APVTS функции, IDs или семантику при удалении только GUI-элемента. Переназначение допустимо только на корректный предусмотренный интерфейсом control и без изменения смысла функции.
- Никаких фиктивных элементов. Нет подтверждённого binding/реализации — честный DISABLED/DEFERRED/NOT IMPLEMENTED статус.
- Перед удалением/заменой проверить наличие функции в других панелях/окнах и записать решение в INFO/CODE. При неоднозначности — сначала зафиксировать конфликт, не удалять молча.

Приёмка: каждый видимый control имеет запись в Control Inventory и подтверждённое назначение; каждый обязательный control присутствует; макетные элементы без функционального основания не остаются в интерфейсе.


## 10. Актуализация gallery / skin inventory — 12.11.2026

Рабочая ветка `work/ppgu-skin-expansion-53-102` публикует 102 темы; canonical default — `skin-052` / SIGNAL FORGE. `SKIN_button` больше не открывает PopupMenu: он открывает встроенную Premium Skin Gallery (panel ID 5) с preview cards, поиском, family filter, active marker и применением темы. Gallery — visual-only и не создаёт новых APVTS/engine features. FX EDIT, AutoChords, Import Guitar, Settings и Gallery реализуют `PPGUThemeAwarePanel`; ThemeManager palette передаётся в содержимое открытых окон при смене темы, включая окна, открытые позднее.

Не считать готовыми:
- Favorites и полноценную keyboard navigation внутри gallery;
- LED indicator, Auto Discover, Curator, Settings, About;
- 150 ms fade, ECO policy, background watcher, memory/LRU asset cache;
- runtime consumption of `controls.*.grid` and full schema-to-component parity assertion.

Skin data generation/merge/publish: CI run `43195205929` SUCCESS; generated commit `3a2ceb52062be112429f96d411ae223d87df17a6`. Windows build/CTest/pluginval and FL Studio 20 acceptance remain separately pending.


### Settings panel implementation update — 12.11.2026

`SETTINGS` button opens child panel ID 6 inside `PPGUMultiWindowWorkspace`. Real controls:
- Auxiliary panel scale: 0.50–1.50; applies to workspace tiling and persists as `workspacePanelScale`.
- Panel gap: 0–32 px; applies to workspace and persists as `workspacePanelGap`.
- Restore SIGNAL FORGE default: loads canonical Skin_052 through ThemeManager and persists stable ID.
- Open Skin Gallery: opens the existing gallery panel alongside Settings.

No audio, APVTS, MIDI or processor parameters are changed. About remains NOT IMPLEMENTED; no fake controls are added.


## 11. RECOVERED APPROVED CONTROL CONTRACT — PRESET / PATTERN / AUTOCHORDS / VISUAL REFERENCES (2026-11-14)

### 11.1 Canonical mockup hierarchy
- First approved main-interface mockup = exact appearance/layout/proportions/block and control placement.
- `02 INTERFACE NEON STAGE AUTOCHORDS` = palette reference only; apply its colours to the first mockup. It does not define layout.
- SIGNAL FORGE identity; NEON STAGE default treatment. No redesign, moving, removing or arbitrary replacing approved controls. Every theme shares the same control map and behavior.
- Three earlier approved visual references remain immutable. Other retrieved mockups are supporting reference, not blanket approval of every widget they depict.

### 11.2 Preset Browser contract
| Surface/control | Required behavior | Acceptance invariant |
|---|---|---|
| BROWSER / FAVORITES / CATEGORIES | Separate views of preset catalogue | View switch does not change active sound |
| Factory/User scope | Separate read-only factory from editable user content | Factory cannot be overwritten/deleted |
| Category/tag/filter/sort/search | Eight categories: Clean, Crunch, Drive, Lead, Ambient, Lo-Fi, Acoustic Textures, Punk Rhythm; artist/style/use-case tags | Filter only; no implicit load |
| Preset item/details | Name, category, tags, description, artwork, verified instrument/sample profile | No fabricated metadata/assets |
| Previous/Next | Navigate deterministic current scope | No invalid selection or unexpected load unless explicitly specified by user action |
| Favourite | Persist user favourite state | Independent from factory preset content |
| Preview | Audition available representative real sample | Non-committing; unavailable if no real preview asset/path |
| Load / double-click | Commit selected preset transaction | Valid sample profile, mode, mapping, articulations, tone/FX and state; report missing assets |
| User preset actions | Save, Save As, Rename, Delete | Persist; confirm destructive delete |
| Bank actions | Import/Export Bank, Rescan | Validate paths/schema; preserve originals; no audio-thread file work |

Target factory bank = 300 definitions (150 MELODY + 150 PATTERN), not a claim of 300 playable verified presets.

### 11.3 Pattern Browser / Song Section contract
| Step/surface | Required behavior | Acceptance invariant |
|---|---|---|
| Song Section | Choose Intro, Verse, Chorus, Bridge, Outro or neutral/general first | Section is not a pattern |
| Pattern Browser | Choose the separate performance drawing after section | Independent selection and registry |
| Search/filter/favourites | Browse suitable patterns with categories and favorites | No implicit apply |
| Metadata | Name, purpose/section, feel, bars/duration, time signature, tempo range, articulations/capabilities | Show verified metadata only |
| Preview | Play actual performance map/recorded one-shot via shared path | Non-committing; no synthetic placeholder |
| Apply | Explicitly commit selected section/pattern | Deterministic and state-safe |
| Back/Cancel | Roll back to prior committed state | Preview never becomes accidental commit |

Supported pattern families include down/up/alternate strokes, palm-muted eighths/sixteenths, open chorus, stop-start, backbeat, chugs, octave hooks, ska/offbeat, arpeggio/picking, acoustic strum/picking and dynamic build, subject to actual recorded performance-map support.

### 11.4 AutoChords and independent selectors
Top chord/degree strip; left Playing Styles/Articulation; right Playback Mode and Rhythm Mode; bottom full-width Pattern Browser. Harmony/key/progression, style/articulation, pattern, playback mode and rhythm mode remain independent. Main on-screen keyboard stays visible. Circle of Fifths, progression and Generation Manager are required design scope; preview uses shared sampler path and generation requires deterministic seed/constraints/explicit commit.

### 11.5 Main and auxiliary controls
- Main modes: MELODY / PATTERN / PUNK GROUP.
- Nine rotary controls remain mapped to ampGain, eqHigh, eqMid, eqLow, velocity, dynamics, swing, humanize, accent.
- Exactly seven bypass switches: Compressor, EQ, Distortion, Chorus, Delay, Reverb, Cabinet. Flanger remains an FX EDIT page; no eighth bypass.
- Eight FX EDIT pages: Compressor, EQ, Distortion, Chorus, Flanger, Delay, Reverb, Cabinet IR.
- Keep AMP & TONE, pitch/mod, on-screen MIDI keyboard, status/performance strip and approved header/browser controls.
- Auxiliary surfaces: FX EDIT, AUTOCHORDS, IMPORT GUITAR, AUTO DISCOVER/SOUND HUNTER, CURATOR, SETTINGS, PERFORMANCE, SKIN GALLERY, ABOUT, and prior approved Preset Manager/Sample Browser workflow. Each inherits current skin, has title/X close and managed independent lifecycle. Unimplemented controls must remain disabled/deferred, not fake.
- Every real control requires a documented binding/action, range, value, tooltip, state/automation, reset, focus and loading/error behavior. Do not change APVTS IDs/semantics or engine functions to make a mockup fit.

### 11.6 Archival note
Reference files known from prior context include `Cover Interface.jpg`, `9132a8e5-5c89-45ee-8412-bea39bc4d04c.png`, `5675b332-eb7a-4dca-85fb-873a9682a349 (1).png` (with a duplicate `(2)`) and `02 INTERFACE NEON STAGE AUTOCHORDS`. Retrieved Library image references also include the full PPGU interface, AutoChords, FX EDIT/Signal Forge and Import Guitar examples. They are not all confirmed copied to GitHub; per-file recovery, exact source mapping and SHA-256 remain OPEN.

**Status:** requirements reconciled into inventory; runtime parity and browser behavior require source-level audit and tests. Documentation only; no implementation claim.
