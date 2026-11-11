# PPGU SKIN — инвентарная матрица контролов (REV 5.0)

## Статус и границы достоверности

Источник функционального состава: `docs/GUI_UX_BLUEPRINT_RU.md`, прочитанный в открытой PR #24 ветке `ui/modern-neon-stage-shell` (commit `dfd4968a4b600c6b6c0d59601c7c7115cb9f2e1a`). Сам документ помечает описанные блоки как спецификацию, а не доказательство реализации. Отдельный `PPGU_5_0_UI_UX_FUNCTIONAL_ACCEPTANCE_RU.md` в проверенном дереве main и PR #24 не найден.

Текущий main `PluginEditor.h/.cpp` подтверждает только ограниченный ранний редактор. Матрица ниже намеренно отличает наличие APVTS ID от наличия видимого GUI-контрола. Тема не может считать отсутствующий в базовом редакторе элемент «готовым»; она обязана сохранять его после появления функциональной реализации.

## Базовый инвентарь

| Область | Контрол / функция | APVTS ID / тип | Состояние в main |
|---|---|---|---|
| Header | Product title / version | `non-APVTS` | Current editor: PPGU title present; version/build status not confirmed |
| Header | Unified preset selector | `catalog action` | Present |
| Header | Preset category filter | `local UI state` | Present |
| Header | Preset search | `local UI state` | Present |
| Header | Favourites / previous / next / dirty-state | `local UI state` | Not present in current editor |
| Header | PREVIEW audition | `action` | Not present |
| Header | LOAD SFZ | `action` | Present as LOAD SFZ |
| Header | Library status / missing-library warning | `diagnostic` | Present as status label |
| Header | SKIN selector | `UI action` | Not present in main; PR #24 has partial THEMES popup only |
| MELODY | Articulation selector | `articulation` | APVTS ID exists; GUI missing |
| MELODY | Pick / Finger selector | `pickStyle` | APVTS ID exists; GUI missing |
| MELODY | Mono / Poly | `monoMode` | APVTS ID exists; GUI missing |
| MELODY | Legato | `legato` | APVTS ID exists; GUI missing |
| MELODY | Hammer-on / Pull-off | `hammerPull` | APVTS ID exists; GUI missing |
| MELODY | Slide modifier / time / sensitivity | `slideModifier, slideTime, slideVelocitySensitivity` | APVTS IDs exist; GUI missing |
| MELODY | Vibrato / Mod-wheel vibrato | `vibrato, modWheelVibrato` | APVTS IDs exist; GUI missing |
| MELODY | Melody pattern slot | `melodyPatternSlot` | APVTS ID exists; GUI missing |
| MELODY | Ending length / palm hit / string slap | `endingLength, palmHitOnEnding, stringSlapOnEnding` | APVTS IDs exist; GUI missing |
| MELODY | Velocity / dynamics / palm mute threshold | `velocity, dynamics, palmMuteThreshold` | velocity and dynamics present; threshold GUI missing |
| MELODY | On-screen MIDI keyboard | `keyboardState` | Present |
| PATTERN | Eight pattern slots / pattern selector | `patternSlot` | APVTS ID exists; full eight-slot GUI not present |
| PATTERN | Pattern length | `patternLength` | APVTS ID exists; GUI missing |
| PATTERN | Sync mode | `syncMode` | Present |
| PATTERN | Swing base / quantize / BPM | `swingBase, patternQuantize, bpm` | APVTS IDs exist; BPM/quantize controls missing |
| PATTERN | Swing / Humanize / Accent | `swing, humanize, accent` | Present |
| PATTERN | Start on first chord / preserve human timing | `startOnFirstChord, preserveHumanTiming` | APVTS IDs exist; GUI missing |
| PATTERN | AutoChords root / scale / slash bass | `scaleRoot, scaleMode, chordSlashBass` | APVTS IDs exist; GUI missing |
| PATTERN | Single MIDI note as chord root | `singleMidiNoteAsChordRoot` | APVTS ID exists; GUI missing |
| PATTERN | Chord/held-note display / voicing preview | `live display` | Not present in main editor |
| PATTERN | Transport / position / panic | `transport actions` | Not present in main editor |
| AMP & TONE | Master / Voices | `master, voices` | APVTS IDs exist; GUI missing |
| AMP & TONE | Gain / Tone / Bass / Mid / Treble / Output | `ampGain, tone, eqLow, eqMid, eqHigh, output` | ampGain and EQ controls present; tone/output missing |
| AMP & TONE | Tuning / Pan | `tuning, pan` | APVTS IDs exist; GUI missing |
| AMP & TONE | Double Track On / Mix | `doubleTrackOn, doubleTrackMix` | APVTS IDs exist; GUI missing |
| FX CHAIN | Seven FX slots / reorder | `fxSlot1…fxSlot7` | APVTS IDs exist; slot UI/reorder missing |
| FX CHAIN | Compressor controls | `compressorOn, compressorThreshold, compressorRatio, compressorAttack, compressorRelease, compressorAmount` | Bypass present; detail controls missing |
| FX CHAIN | EQ controls | `eqOn, eqLow, eqMid, eqHigh, eqLowFreq, eqMidFreq, eqMidQ, eqHighFreq` | Bypass and three gain knobs present; frequency/Q controls missing |
| FX CHAIN | Distortion controls | `distortionOn, distortionDrive, distortionTone, distortionMix` | Bypass present; detail controls missing |
| FX CHAIN | Chorus controls | `chorusOn, chorusRate, chorusDepth, chorusMix` | Bypass present; detail controls missing |
| FX CHAIN | Flanger controls | `flangerOn, flangerRate, flangerDepth, flangerFeedback, flangerMix` | APVTS IDs exist; GUI missing |
| FX CHAIN | Delay controls | `delayOn, delayTime, delayFeedback, delayMix` | Bypass present; detail controls missing |
| FX CHAIN | Reverb controls | `reverbOn, reverbRoomSize, reverbDamping, reverbMix` | Bypass present; detail controls missing |
| FX CHAIN | Cabinet IR controls | `cabinetOn, cabinetMix, cabinetLowCut, cabinetHighCut` | Bypass present; detail controls missing |
| PLAYBACK | Start mode / sync mode | `syncMode` | Sync mode selector present; dedicated Start Mode not confirmed |
| PLAYBACK | MIDI activity / active voices / output meter | `diagnostics` | Not present/confirmed in main editor |
| PLAYBACK | CPU/RAM readouts | `measured diagnostics` | Not present; must not be faked |
| BOTTOM RAIL | Library/indexed sample count / progress | `diagnostics` | Only basic status label present |
| BOTTOM RAIL | Active mode / preset / chord / pattern slot | `diagnostics` | Only basic mode/preset controls present |
| BOTTOM RAIL | Help / Panic | `actions` | Not present in main editor |
| SECONDARY | Preset Browser | `window` | Not present |
| SECONDARY | Sample Browser | `window` | Not present |
| SECONDARY | Pattern Editor | `window` | Not present |
| SECONDARY | FX Detail | `window` | Not present |
| SECONDARY | Theme Gallery | `window` | Not present |
| SECONDARY | Settings / MIDI | `window` | Not present |
| SECONDARY | About / Help | `window` | Not present |

## APVTS ID inventory, подтверждённые в PluginProcessor.cpp main

`master`, `voices`, `velocity`, `dynamics`, `palmMuteThreshold`, `tuning`, `pan`, `output`, `playMode`, `articulation`, `pickStyle`, `pitchBendTarget`, `vibrato`, `slideTime`, `legato`, `hammerPull`, `scaleRoot`, `scaleMode`, `monoMode`, `fretNoiseOnChordChange`, `slideModifier`, `slideVelocitySensitivity`, `modWheelVibrato`, `melodyPatternSlot`, `endingLength`, `palmHitOnEnding`, `stringSlapOnEnding`, `patternSlot`, `patternLength`, `syncMode`, `swingBase`, `globalPpqOffset`, `chordSlashBass`, `singleMidiNoteAsChordRoot`, `preserveHumanTiming`, `dnaChugDensity`, `dnaHammerPullProbability`, `dnaStringSpread`, `dnaBeatAccent`, `dnaFxTaste`, `fxSlot1…fxSlot7`, `compressorThreshold`, `compressorRatio`, `compressorAttack`, `compressorRelease`, `eqLowFreq`, `eqMidFreq`, `eqMidQ`, `eqHighFreq`, `distortionMix`, `chorusRate`, `chorusDepth`, `delayFeedback`, `reverbRoomSize`, `reverbDamping`, `cabinetMix`, `cabinetLowCut`, `cabinetHighCut`, `bpm`, `swing`, `humanize`, `accent`, `startOnFirstChord`, `patternQuantize`, `ampGain`, `tone`, `compressorOn`, `compressorAmount`, `eqOn`, `eqLow`, `eqMid`, `eqHigh`, `distortionOn`, `distortionDrive`, `distortionTone`, `chorusOn`, `flangerOn`, `flangerMix`, `flangerRate`, `flangerDepth`, `flangerFeedback`, `chorusMix`, `delayOn`, `delayTime`, `delayMix`, `reverbOn`, `reverbMix`, `cabinetOn`, `doubleTrackOn`, `doubleTrackMix`.

Наличие ID не доказывает наличие GUI, корректного Attachment или доступность в текущей сборке. Новые ID добавлять/переименовывать в рамках Skin нельзя.

## Матрица применения ко всем 50 темам

Каждая тема обязана наследовать полный функциональный инвентарь из таблицы выше. Skin меняет только visual tokens и artwork. Отдельная строка на каждую тему фиксирует единый контракт; runtime-проверка пока не выполнена.

| Слот | Идентичность | Инвариант | Статус |
|---|---|---|---|
| Skin_01 | Neon Riot | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_02 | Theme manifest 02 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_03 | Theme manifest 03 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_04 | Theme manifest 04 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_05 | Theme manifest 05 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_06 | Theme manifest 06 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_07 | Theme manifest 07 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_08 | Theme manifest 08 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_09 | Theme manifest 09 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_10 | Theme manifest 10 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_11 | Theme manifest 11 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_12 | Theme manifest 12 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_13 | Theme manifest 13 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_14 | Theme manifest 14 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_15 | Theme manifest 15 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_16 | Theme manifest 16 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_17 | Theme manifest 17 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_18 | Theme manifest 18 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_19 | Theme manifest 19 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_20 | Theme manifest 20 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_21 | Theme manifest 21 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_22 | Theme manifest 22 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_23 | Theme manifest 23 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_24 | Theme manifest 24 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_25 | Theme manifest 25 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_26 | Theme manifest 26 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_27 | Theme manifest 27 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_28 | Theme manifest 28 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_29 | Theme manifest 29 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_30 | Theme manifest 30 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_31 | Theme manifest 31 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_32 | Theme manifest 32 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_33 | Theme manifest 33 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_34 | Theme manifest 34 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_35 | Theme manifest 35 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_36 | Theme manifest 36 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_37 | Theme manifest 37 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_38 | Theme manifest 38 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_39 | Theme manifest 39 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_40 | Theme manifest 40 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_41 | Theme manifest 41 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_42 | Theme manifest 42 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_43 | Theme manifest 43 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_44 | Theme manifest 44 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_45 | Theme manifest 45 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_46 | Theme manifest 46 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_47 | Theme manifest 47 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_48 | Theme manifest 48 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_49 | Theme manifest 49 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |
| Skin_50 | Theme manifest 50 | Same shared control contract; no theme may hide or replace any row above | SPEC-LOCKED / RUNTIME NOT VERIFIED |

## Критерий закрытия

- `MATRIX COMPLETE` возможно только после source/runtime проверки каждого видимого контрола, его подписи, hit area, tooltip, disabled reason и APVTS attachment.
- Если параметр имеет APVTS ID, но не имеет GUI, он остаётся `GUI MISSING`, а не считается закрытым темой.
- Ни одна тема не может скрывать недостающие элементы художественным оформлением.
- Для каждой темы требуется тест на одинаковый состав компонентов и стабильные ID при переключении Skin.
- Статус текущей матрицы: `SPEC READY / RUNTIME NOT VERIFIED`.
