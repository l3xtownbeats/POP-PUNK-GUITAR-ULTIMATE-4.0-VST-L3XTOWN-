# PPGU SKIN — инвентарная матрица контролов (REV 14.0)

Дата актуализации: 12 ноября 2026. Источник истины по обязательным ID и layout grid: `control_schema.json`. Источник фактической реализации: `Source/PluginEditor.h/.cpp` и `Source/PluginProcessor.cpp`.

## 1. Фактический GUI в main

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
- Кнопка SKIN, popup selector, Ctrl+K, сохранение skin ID в UI Properties и применение существующей палитры.

Это не полноценная галерея. В main пока нет preview cards, поиска/избранного по темам, background rescan, dedicated gallery, плавного 150 ms fade или ECO policy.

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
| SKIN_button | skinButton | Opens current skin popup; Ctrl+K shortcut | PARTIAL |
| Import_Guitar | loadButton | SFZ file chooser + async library load | IMPLEMENTED |
| Auto_Discover | none | Reserved for production Sound Hunter/S6 discovery | NOT IMPLEMENTED |
| Curator | none | Reserved for production library curation/review | NOT IMPLEMENTED |
| Settings | none | No settings panel in current editor | NOT IMPLEMENTED |
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

Current reference design: Skin_052 / Carbon Relay / carbon_studio / STUDIO_CONSOLE. It maps all 27 mandatory IDs and the existing sync selector. It is DESIGN metadata, not yet runtime-integrated.

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
