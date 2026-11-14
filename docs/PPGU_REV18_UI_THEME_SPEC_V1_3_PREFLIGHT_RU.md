# PPGU REV 18 — UI THEME SPEC v1.3 PREFLIGHT / SOURCE RECONCILIATION

Дата: 2026-11-15  
Ветка: `work/neon-stage-reference-preview-fix`  
Исходный HEAD до документа: `b3cfe00ae459b44d3cb3cae059a886f4886df0fa`  
Тип: C3 / read-only source audit  
Статус: **PREFLIGHT COMPLETE / IMPLEMENTATION BLOCKED BY CONTRACT RECONCILIATION**  
Область: только UI/theme. Audio, DSP, APVTS semantics, MIDI, presets, Sound Hunter и CI не изменялись.

## 1. Применяемый контракт

Получена пользовательская спецификация **PPGU UI THEME SPEC — REV 18 (v1.3 — FULL CONTROL REFERENCE EDITION)**. Для UI/theme она является целевым контрактом этой работы:

- SKIN-1 = NEON STAGE DEFAULT = `skin-053` / `Skin_053`.
- SOUND FORGE и SIGNAL FORGE — только исторические имена той же NEON STAGE, не отдельные темы.
- Релизная галерея — ровно 16 тем.
- Дефолтный режим — МИНИМАЛИЗМ; ЭФФЕКТЫ — второй режим той же темы.
- 21 canonical token и 27 required control-ID берутся из `SKIN/control_schema.json`.
- Не менять APVTS IDs/семантику, MIDI, DSP, пресеты и аудиокод.
- Никаких фиктивных функций, состояний, метрик или анимаций.

**Не найдено:** отдельный файл MASTER PROMPT v6.2 в текущей ветке. `docs/PPGU_MASTER_PROMPT_RU.md` в ней начинается с REV 17.1 UNIFIED. Он не подменяет заявленный пользователем v6.2. До получения/проверки v6.2 любые конфликты между общими правилами проекта и UI v1.3 фиксируются, а не разрешаются догадкой.

## 2. Факты исходников на проверенном HEAD

| Область | Текущий факт | Требуемое действие |
|---|---|---|
| Registry | `SKIN/theme_manifest.json`: 102 записи, индексы 1…102, `defaultSkin=neon-stage-default-skin-1` | Ввести отдельный active ordinal/lifecycle; не переиспользовать исторический index. |
| Skin_052 | ID `neon-stage-default-skin-1`, index 52, name NEON STAGE | Сохранить полностью как историческую запись; не удалять и не перезаписывать. |
| Skin_053 | ID `skin-053`, index 53, name NEON STAGE — LEGACY ARCHIVE | Целевая запись SKIN-1 по v1.3; сохранить stable ID/path/index и исходные assets. |
| Skin_052/053 artwork | PNG hero и preview имеют одинаковые Git blob SHA; SVG bezel/crest/texture различаются | Не копировать artwork поверх Skin_053 и не делать редизайн NEON STAGE. |
| ThemeManager fallback | `Source/UI/ThemeManager.h` задаёт fallback ID `neon-stage-default-skin-1`; parser признаёт его canonical только при index 52 | Перевести default/fallback на skin-053 атомарно с тестами и сохранением legacy parsing. |
| Gallery | `Source/PluginEditor.cpp` получает count из `totalSkins`, перечисляет числовые индексы 1…N и применяет index | Перевести на activeOrdinal; archived entries не показывать. |
| Saved selection | `skinId` сохраняется в JUCE PropertiesFile; Settings restore вызывает `loadSkinByIndex(52, true)` | Миграция старого default ID → skin-053; сохранять stable ID, не ordinal. |
| Validators | Skin validators требуют старый default на index 52 и непрерывные registry indices 1…N; pack generator/merger продолжают эту модель | Обновить registry, validator, generator, merger и migration tests одним контрактом. |
| Runtime layout | Координаты theme controls сейчас являются design metadata; runtime layout engine, применяющий всю сетку 12×16, не подтверждён | Нельзя считать новые координаты реализованными до layout-engine implementation и UI tests. |
| Master prompt | Ветка содержит REV 17.1, а не отдельный v6.2 | Получить и сверить именно v6.2 перед финальным contract arbitration. |

## 3. Control inventory — 27 обязательных ID

Ниже — mapping из `x-ppgu-contract.runtimeControlInventory`. Координаты — текущая сетка Skin_053, формат `x,y,w,h`; это **legacy metadata**, не утверждённая геометрия A–H v1.3.

| Control-ID | APVTS/action/MIDI mapping | Текущий статус | Текущая grid |
|---|---|---|---|
| preset_browser | Preset selection + category/text filtering; не APVTS | implemented | 2,0,5,2 |
| mode_selector | `playMode` | partial | 7,0,3,2 |
| knob01 | `ampGain` | implemented | 0,4,2,2 |
| knob02 | `eqHigh` | implemented | 2,4,2,2 |
| knob03 | `eqMid` | implemented | 4,4,2,2 |
| knob04 | `eqLow` | implemented | 6,4,2,2 |
| knob05 | `velocity` | implemented | 8,4,2,2 |
| knob06 | `dynamics` | implemented | 10,4,2,2 |
| knob07 | `swing` | implemented | 2,7,2,2 |
| knob08 | `humanize` | implemented | 5,7,2,2 |
| knob09 | `accent` | implemented | 8,7,2,2 |
| fx_switch01 | `compressorOn` | implemented | 0,10,3,1 |
| fx_switch02 | `eqOn` | implemented | 3,10,3,1 |
| fx_switch03 | `distortionOn` | implemented | 6,10,3,1 |
| fx_switch04 | `chorusOn` | implemented | 9,10,3,1 |
| fx_switch05 | `delayOn` | implemented | 0,11,4,1 |
| fx_switch06 | `reverbOn` | implemented | 4,11,4,1 |
| fx_switch07 | `cabinetOn` | implemented | 8,11,4,1 |
| midi_keyboard | `juce::MidiKeyboardState`, notes 36–84 | implemented | 0,12,12,2 |
| LCD_display | `statusLabel`; dedicated LCD widget отсутствует | partial | 0,2,8,1 |
| LED_indicator | Нет live MIDI/audio source; не имитировать | not implemented | 8,2,1,1 |
| SKIN_button | Gallery action; Ctrl+K | implemented | 10,0,2,1 |
| Import_Guitar | SFZ chooser → async load | implemented | 9,2,3,1 |
| Auto_Discover | Reserved Sound Hunter/S6 action | not implemented | 0,15,3,1 |
| Curator | Reserved curator action | not implemented | 3,15,3,1 |
| Settings | Current reserved schema entry; actual source has UI-only workspace settings | partial / contract mismatch | 6,15,3,1 |
| About | Reserved; actual build metadata UI not implemented | not implemented | 9,15,3,1 |

`sync_selector` is an additional current service control outside the 27 required IDs. It is explicitly present in `additionalCurrentControls`; do not silently delete it or count it as an extra required ID.

## 4. Canonical palette tokens in current schema

The current `control_schema.json` declares 21 Skin Studio tokens:

`bg_deep`, `bg_panel`, `bg_panel_hi`, `border`, `border_glow`, `text_primary`, `text_secondary`, `text_disabled`, `accent_primary`, `accent_secondary`, `accent_warning`, `accent_danger`, `accent_success`, `lcd_bg`, `lcd_text`, `knob_body`, `knob_ring`, `led_on`, `led_off`, `overlay_scrim`, `focus_ring`.

The schema separately retains a 21-token legacy palette for older Skin Studio manifests. ThemeManager currently supports both generations. The C++ Tokens struct is a compatibility union and must not be treated as permission to add extra JSON tokens.

### Read-only contrast calculation for current Skin_053 palette

| Pair | Measured ratio | v1.3 observation |
|---|---:|---|
| text_primary / bg_deep | 17.56:1 | Meets primary ≥7:1 |
| text_secondary / bg_deep | 9.29:1 | High contrast |
| text_disabled / bg_deep | 3.56:1 | Below the 5.23 reference value quoted in v1.3; threshold must follow canonical G1 rule |
| lcd_text / lcd_bg | 13.49:1 | High contrast |
| accent_primary / bg_panel | 9.38:1 | High contrast |

These are calculations from the current Skin_053 JSON values only. They are **not** a G1/G2/G3/G4 validation result. The v1.3 text describes Skin_052 as Carbon Relay, while the checked branch's Skin_052 is NEON STAGE; therefore the quoted reference ratios cannot be asserted as current source evidence.

## 5. Contract mismatches that block implementation

1. **Release gallery count:** v1.3 requires exactly 16 themes. The prior migration audit proposed 50 active themes; that proposal is superseded by v1.3. The 15 non-default release themes have not yet been explicitly selected/approved. Do not assume that Skin_054…Skin_068 are approved merely because they are consecutive.
2. **Stable identity vs active numbering:** preserve all historical `id`, `path`, and `index`; add a separate `activeOrdinal` and lifecycle/visibility. SKIN-1 maps to Skin_053. Existing IDs are not to be renumbered.
3. **Default migration:** Skin_052 must remain byte-for-byte preserved as archived historical data; Skin_053 becomes default without redesign. Runtime fallback, registry, Gallery, saved selection, Settings restore and tests must switch together.
4. **Control inventory:** 27 required IDs are confirmed. Existing service key `sync_selector` is separate. LED/Auto Discover/Curator/About remain unavailable where source has no implementation.
5. **Settings:** v1.3 calls for exactly six existing APVTS controls, but current source's Settings panel contains UI-only workspace scale/gap, restore-skin and open-gallery actions; no six-control APVTS Settings inventory is evidenced. Do not invent the six controls.
6. **Gallery:** current source applies a theme immediately on card click; v1.3's gallery rules exclude Preview/Apply/Back. Keep immediate apply unless a later approved contract explicitly changes it.
7. **Auxiliary-window count:** section 5 says “five auxiliary windows” but enumerates FX EDIT, AUTOCHORDS, IMPORT GUITAR, SETTINGS, PERFORMANCE and ABOUT (six). Clarify whether ABOUT is a dialog/non-auxiliary surface before window-count acceptance.
8. **Visual reference:** current source audit has no confirmed pixel-level approved reference mapping for the canonical NEON STAGE artwork. No screenshot should be promoted by resemblance alone.

## 6. Safe next implementation sequence

1. Obtain the exact MASTER PROMPT v6.2 and resolve the six-window wording plus the release list of 16 themes.
2. Freeze a no-loss migration model: immutable historical registry fields + separate `activeOrdinal` + lifecycle; SKIN-1 = Skin_053; exactly 16 ACTIVE entries.
3. Add migration tests first: 102 old records preserved; active ordinals unique and continuous 1…16; default resolves to skin-053; old saved default migrates safely; archived IDs fall back according to an explicit policy.
4. Update registry resolver, Gallery, Settings restore and saved-state handling in one UI-only change.
5. Update `validate_skin_pack.py`, `merge_skin_pack.py`, generator and manifest tests to validate stable index separately from active ordinal and 16-theme release count.
6. Rebuild the complete control mapping against v1.3 A–H geometry; do not reuse the legacy grid coordinates as approved final coordinates.
7. Run G1–G4, CTest, Windows x64 Release, VST3 locate, pluginval and packaging on the exact new SHA. Manual FL Studio 20 host acceptance remains separate.

## 7. Explicit non-claims

- No source code, APVTS, MIDI, DSP, presets, sample assets or runtime skin payloads were changed by this audit.
- No migration was implemented.
- No build, CTest, pluginval, FL Studio host test or G1–G4 PASS is claimed.
- This document is a read-only source reconciliation, not a completed theme or release gallery.
