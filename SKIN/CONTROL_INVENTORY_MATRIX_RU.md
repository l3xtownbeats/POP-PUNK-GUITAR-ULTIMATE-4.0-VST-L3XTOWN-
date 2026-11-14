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


## 13. PRESET / PATTERN BROWSER — ADDITIONAL APPROVED CONTROL INVENTORY (2026-11-14)

### 13.1 Preset Browser
| Control | Required behavior | Must not do |
|---|---|---|
| BROWSER / FAVORITES / CATEGORIES | Switch catalogue views | Load or modify active preset |
| FACTORY / USER scope | Separate read-only factory and editable user content | Permit factory overwrite/delete |
| SEARCH / FILTER / SORT | Filter/order by category, artist/style/use-case tags and metadata | Commit sound or change active Pattern |
| PRESET NAME / CATEGORY | Show selected record's verified identity | Fabricate metadata |
| PREV / NEXT | Navigate deterministic current result set | Escape active search/filter/scope unexpectedly |
| FAVORITE | Persist user favorite state | Modify factory preset definition |
| PREVIEW | Audition real preview asset via safe shared path | Commit active preset/project state |
| SELECT / LOAD | Explicit transactional preset load; double-click may invoke it | Load on highlight/filter/preview |
| SAVE / SAVE AS / RENAME / DELETE | Manage USER presets; confirm destructive delete | Mutate FACTORY records |
| IMPORT / EXPORT / RESCAN | Validate and manage user banks/library index | Do filesystem work in audio callback |

### 13.2 Pattern Browser hierarchy and controls
Context order: **GENRE → ARTIST/BAND → SONG → SECTION → STYLE → PATTERN**. These are independent context fields/filters; selecting them does not silently load a tone preset or commit a Pattern. Section list is **Intro, Verse, Pre-Chorus, Chorus, Bridge, Outro**; Pre-Chorus is additive to the previously approved set. Neutral/General only if explicitly present in registry.

| Control | Required behavior | Acceptance invariant |
|---|---|---|
| GENRE / ARTIST-BAND / SONG / SECTION / STYLE | Narrow relevant pattern results | No hidden Load/Apply; fields remain distinct |
| SEARCH / FILTER / SORT / FAVORITES | Browse current pattern registry | No committed-state change |
| PATTERN item/details | Show real section/use, feel, bars, meter, tempo, capabilities and asset availability | No fabricated metadata/maps |
| PREV / NEXT (if present) | Traverse current filtered scope | Do not change committed pattern |
| PREVIEW | Play actual performance map/recording through shared path | Non-committing; no transport/project mutation |
| APPLY | Commit selected section + pattern | Explicit, deterministic, state-safe |
| BACK / CANCEL | Restore last committed selection | Roll back preview/draft only |

**Required workflow:** SECTION → PATTERN → PREVIEW → APPLY or BACK/CANCEL. Pattern types/techniques (down/up/alternate, palm mute, open chorus, stop-start, backbeat, chugs, octave hooks, ska/offbeat, arpeggio, acoustic strum/picking, dynamic build) are enabled only where real performance maps/events/recordings support them. Temporary hard-coded masks do not certify a complete pattern library.

### 13.3 Global control invariants
- Browse-only operations never commit; Preview is non-committing; Load/Apply is explicit commit; Back/Cancel rolls back.
- Controls must be functional, keyboard/mouse accessible, focusable, have tooltips/ranges and truthful loading/empty/error/missing-asset/disabled states. No dummy buttons/meters/cards.
- Preset Browser, Pattern Browser and Sample/Library Browser remain separate workflows.
- Existing nine rotary/APVTS IDs, seven main FX bypasses, eight FX EDIT pages, 21 theme tokens, 27 controls + sync_selector and all stable IDs/semantics remain unchanged.
- This inventory records approved requirements only; runtime implementation and test evidence remain separately tracked.


## 14. STYLE / PATTERN / AUTOCHORDS — ADDITIONAL CONTROL AND WINDOW INVENTORY (2026-11-14)

### 14.1 Separate windows and ownership
| Window | Owns | Must remain separate from | State invariant |
|---|---|---|---|
| STYLE Browser | Genre/group/artist/song/section/playing-style/articulation navigation | PATTERN Browser and Preset Browser | Independent navigation/selection; no hidden tone or pattern load |
| PATTERN Browser | Rhythm/performance pattern categories, section slots, preview/apply | STYLE Browser and Preset Browser | Independent navigation; commits rhythm mapping only |
| AutoChords overlay | Chord/harmony selectors and progression controls | STYLE/PATTERN selection state | Changes harmony only; never covers MIDI keyboard |
| Preset Browser | Instrument/source/tone preset selection and explicit Load | STYLE/PATTERN browser state | Load is explicit and transactional |

### 14.2 STYLE Browser inventory
Navigation: GENRE → GROUP/ARTIST → SONG SELECTION → SONG SECTION → PLAYING STYLE/ARTICULATION. Controls: SEARCH, FAVORITES, CATEGORIES, FILTER, SORT, PREVIOUS/NEXT (within current category/filter scope), PREVIEW, APPLY, CANCEL, SELECT. PREVIEW is non-committing; APPLY/SELECT commits style/articulation context only. It must not change Pattern, chord/harmony, guitar source or Tone Preset.

### 14.3 PATTERN Browser inventory
Approved pattern categories: DOWNSTROKES, DOWN/UP, PALM MUTE, POWER CHORDS, ALTERNATE PICKING, CUSTOM PATTERNS. Controls: SEARCH, FAVORITES, CATEGORIES, FILTER, SORT, PREVIOUS/NEXT, PREVIEW, APPLY, CANCEL, SELECT. Six sections: INTRO, VERSE, PRE-CHORUS, CHORUS, BRIDGE, OUTRO. Each section has eight Pattern Slots (48 logical slots total); this is capacity, not a populated/playable asset count. Each slot mapping is section-scoped and independently persisted. PREVIEW never commits; APPLY commits only the selected section/slot/pattern mapping.

### 14.4 Independent state dimensions
| Dimension | Owns | Must not silently modify |
|---|---|---|
| CHORD / HARMONY | Chord/key/progression state | STYLE, PATTERN, source, Tone Preset |
| ARTICULATION / PLAYING STYLE | Performance technique context | Chord, Pattern, source, Tone Preset |
| RHYTHM PATTERN | Rhythm/performance events | Chord, STYLE, source, Tone Preset |
| GUITAR SOURCE / INSTRUMENT | Sample/source instrument | Chord, STYLE, Pattern, Tone Preset |
| TONE PRESET | Tone/amp/FX profile | Chord, STYLE, Pattern, source identity |

Unsupported combinations must be shown as unavailable/degraded; no silent substitute or destructive cross-selector update.

### 14.5 AutoChords and MIDI keyboard geometry
Preserve ON/OFF, Major/Minor/Custom, Key, Pattern, Current Chord, progression editor, inversions/voicings, duration, rhythm, performance method, Sync to Tempo, Smart Transpose, Preview and Reset as specified in the product spec. Selectors remain independent from STYLE/PATTERN/source/tone. The AutoChords overlay must never cover the MIDI keyboard or hit-test area. Preview uses the real audio/MIDI path and is non-committing; Reset changes only AutoChords state. Controls without runtime backend remain DISABLED/PENDING.

### 14.6 DAW/MIDI acceptance
Host tempo/transport sync, deterministic note lifecycle and no stuck notes, duplicate triggers, orphan note-offs or timing jumps are required through start/stop/seek/loop/tempo changes, section/pattern/chord changes, browser switching, preview/cancel, source/preset change, state recall and plugin deactivation. These rows are requirements; do not mark TESTED without exact-SHA host/automated evidence.

### 14.7 Visual baseline lock
First supplied interface reference = authoritative appearance/layout/dimensions/control placement. 02 INTERFACE NEON STAGE AUTOCHORDS = palette only. Skins may change visual styling only and must not alter control inventory, positions, dimensions, functions, bindings, APVTS semantics or sound. No approved control/window may be merged, removed, hidden, replaced or simplified by an agent or generator.

**Status:** APPROVED REQUIREMENTS / RUNTIME IMPLEMENTATION AND HOST VALIDATION TRACKED SEPARATELY.


### 14.8 Same label, different control — Pattern selector separation
AutoChords.Pattern and PATTERN Browser.Pattern are distinct controls with separate state/ownership. The AutoChords selector remains inside the AutoChords chord/harmony workflow; the Browser selector chooses section/slot rhythm-performance content. No shared binding, implicit synchronization or cross-overwrite is allowed.


### 14.9 Preset Browser category taxonomy
The approved Preset Browser categories are CLEAN, CRUNCH, DRIVE, LEAD, AMBIENT, LO-FI, ACOUSTIC TEXTURES and PUNK RHYTHM. They classify tone/preset content, not rhythm-performance patterns. Category selection/filtering never loads or applies a preset. Keep this taxonomy separate from PATTERN Browser categories DOWNSTROKES, DOWN/UP, PALM MUTE, POWER CHORDS, ALTERNATE PICKING and CUSTOM PATTERNS.
