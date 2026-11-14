# REV.18 — NEON STAGE / GUI COMPONENT MATRIX
Дата preflight: 2026-11-14
Ветка: `POP-PUNK-GUITAR-ULTIMATE-VST-REV.18`
Исходный SHA: `7a7fb55641f36353fe32873c677e5db64142624c`
Тип: Phase 0 — требования ↔ live source reconciliation
Статус: AUDIT COMPLETE / DOCUMENTATION ONLY / PRODUCTION SOURCE UNCHANGED

## 1. Границы проверки

Сверены:
- `docs/PPGU_MASTER_IMPLEMENTATION_REGISTER_RU.md`, разделы 3–4;
- `docs/APPROVED_SKIN_AND_MELODY_PATTERN_ACCEPTANCE_4_0.md`;
- `Info.txt`, раздел K и последние REV.18 записи;
- `SKIN/CONTROL_INVENTORY_MATRIX_RU.md`;
- `PPGU_WINDOWS_PANELS_INVENTORY.md`;
- `Source/PluginEditor.h/.cpp`, `Source/PluginProcessor.cpp`;
- `Source/UI/PPGUMultiWindowWorkspace.h/.cpp`, `ThemeManager.h/.cpp`;
- `SKIN/theme_manifest.json`, первые три skin manifests;
- текущий Windows workflow run `43592701287`.

Это статический source/spec audit. Это НЕ визуальная приёмка, НЕ runtime test, НЕ host test и НЕ подтверждение работы каждой функции.

## 2. Статусы

- **SOURCE + BINDING** — компонент/параметр найден и есть реальная связь.
- **SOURCE ONLY / PARTIAL** — код или окно есть, но функция/контракт неполны.
- **MISSING** — компонент/футь не найден в live source.
- **NOT TESTED** — нет точного runtime/host доказательства на проверяемом SHA.
- **BLOCKED** — нельзя безопасно продолжать до устранения указанного конфликта.

## 3. Component-by-component matrix

| Зона / требование | Live source | Callback / APVTS / persistence | Текущий статус | Недостающее / проверка |
|---|---|---|---|---|
| A — Product header / PPGU identity | `titleLabel`, `subtitleLabel` | UI labels | SOURCE PRESENT | Точное соответствие утверждённому NEON STAGE layout не принято визуально. |
| A — Preset previous / next | Отдельные компоненты не обнаружены в `PluginEditor.h` | Нет подтверждённых callbacks | MISSING | Добавить только после сверки утверждённой геометрии и существующего preset flow. |
| A — Favorite / Save / preset statistics | Отдельные controls не обнаружены | Нет подтверждённого user-state flow | MISSING | Реальное сохранение/избранное/счётчики, без фиктивных кнопок. |
| A — Search | `presetSearch` | `onTextChange` обновляет список | SOURCE + BINDING | Проверить keyboard focus, сохранение фильтра и UI acceptance. |
| A — SKIN | `skinButton`, `openSkinGalleryPanel` | Открывает gallery; stable ID сохраняется в UI Properties | SOURCE + BINDING | Gallery transaction (Preview/Apply/Back), favorites, keyboard navigation, rescan и fade не подтверждены. |
| A — Import Guitar | `loadButton`, `openImportGuitarPanel` | SFZ chooser / async library load | SOURCE + BINDING | Проверить end-to-end импорт, статусы и восстановление после ошибки на exact SHA. |
| A — Auto Discover / Curator / About | Компоненты отсутствуют в editor header | Нет | MISSING | Не рисовать активные заглушки; сначала production S6 pipeline / curator / About implementation. |
| A — Settings | `settingsButton`, `PPGUSettingsPanel` | Workspace scale/gap, restore default skin, open gallery; UI properties | SOURCE + BINDING (UI-only) | Не является полным ENGINE / AUDIO / MIDI / LIBRARY settings contract. |
| B — Preset browser | `presetBox`, `categoryBox`, `presetSearch`, `factoryPresets` | Каталог загружается; фильтр и выбор подключены | SOURCE + BINDING | Нет подтверждения 500+500 READY. 300 legacy definitions не равны playable presets. Нет полной BROWSER/FAVORITES/CATEGORIES навигации и runtime parity acceptance. |
| B — Artwork / description / category counts | Часть preset metadata присутствует в model | Полный card/artwork/count flow не подтверждён | PARTIAL | Сверить каждый UI field с реальными preset metadata; не показывать фиктивные числа/описания. |
| C — MELODY / PATTERN | `melodyTab`, `patternTab`, `modeBox` | `playMode` APVTS attachment | SOURCE + BINDING | Host transport, MIDI selection, full playback and persistence tests pending. |
| C — PUNK GROUP | Отсутствует в live mode selector | Нет | MISSING | Не имитировать третью вкладку; отдельный контракт/engine mapping required. |
| C — 9 performance knobs | `knobs[9]`, `knobAttachments[9]` | `ampGain`, `eqHigh`, `eqMid`, `eqLow`, `velocity`, `dynamics`, `swing`, `humanize`, `accent` | SOURCE + BINDING | Контроль-паритет и точная визуальная семантика сверить с approved layout; отдельные controls не подменять по смыслу. |
| D — FX Chain | `fxButtons[7]`, `fxAttachments[7]` | `compressorOn`, `eqOn`, `distortionOn`, `chorusOn`, `delayOn`, `reverbOn`, `cabinetOn` | SOURCE + BINDING | Пользовательская цель FX EDIT — 10–15 реально реализованных DSP effects; текущий UI показывает 7 slots. Нужен полный DSP-to-control audit. |
| F — FX EDIT | `PPGUFXEditPanel`, `openFxEditPanel` | Панель открывается в workspace; detail controls имеют APVTS attachments | SOURCE / PARTIAL | Подтвердить все реальные DSP modules, 10–15 EFFECT SELECT entries, 8 approved pages, 7 bypass switches, automation/state and tests. |
| G — AMP & TONE | `ampTitle`, nine existing controls, processor amp/EQ parameters | Часть параметров имеет attachments | PARTIAL | Не найдено подтверждение полного amp-model / cabinet / tone-control parity из утверждённого UI. Проверить реальные DSP routes, не добавлять декоративные selectors. |
| E — AutoChords | `PPGUAutoChordsPanel`, `openAutoChordsPanel` | Setup/preview controls; не подтверждён MIDI generator | SOURCE ONLY / DISABLED BY DESIGN for generation | Не заявлять генерацию MIDI. Сначала отдельный integration contract и MIDI/transport tests. |
| H — On-screen keyboard | `juce::MidiKeyboardComponent` | Подключена к `processor.keyboardState`; диапазон 36–84 | SOURCE + BINDING | Проверить external host MIDI note feedback, velocity, note-off and all required key ranges in FL Studio 20. |
| H — Pitch / Mod wheels | В editor header не обнаружены | Нет подтверждённых bindings | MISSING | Нужны реальные pitch-bend / mod-wheel bindings, не декоративные изображения. |
| H — Performance / CPU / RAM / transport status | `PPGUPerformanceDisplay`, `statusLabel`, `startModeBox` | Read-only monitor / syncMode; processor has additional parameters | PARTIAL | Не найдены подтверждённые measured CPU/RAM meters or full tempo/transport UI. Не показывать hard-coded values. |
| Auxiliary windows | `PPGUMultiWindowWorkspace` | Open/close, tiling, resizing, maximize/restore, persistence, theme propagation | SOURCE PRESENT / PARTIAL | Нет production OverlayManager contract for modal blocking, z-order/focus stack and full lifecycle acceptance. |
| Skin Gallery | `PPGUSkinGalleryPanel` in `PluginEditor.cpp` | Preview cards, search, family filter, active marker, click-to-apply, persisted skin ID | SOURCE PRESENT / PARTIAL | No draft Preview/Apply/Back rollback, Favorites, full keyboard navigation, background rescan, 150 ms fade, ECO/cache acceptance. |
| Secure skin import | No live production `SecureSkinImport` module found | None | MISSING | Implement separately with ZIP/path/SVG/security/lifecycle tests; do not copy old ZIP prototype without repairs. |

## 4. Skin identity reconciliation — critical mismatch

The newest approved project decision in INFO says:
- SKIN-1 = **NEON STAGE**, default/startup/fallback;
- SKIN-2 = **SOUND FORGE**;
- SKIN-3 = **BLACKOUT VINTAGE**;
- SKIN-4 onward = next free sequential IDs; preserve all unique IDs.

Live REV.18 source currently says otherwise:
- `SKIN/theme_manifest.json`: `defaultSkin = skin-052`;
- `skin-01 = Neon Riot`;
- `skin-02 = Dark Metal`;
- `skin-03 = Modern Studio`;
- `Source/UI/ThemeManager.h`: default Palette id `skin-052`, name `SIGNAL FORGE`.

**Status: BLOCKED / IDENTITY MISMATCH.** Do not rename, renumber, delete, overwrite or reorder existing skin records until a full 102-entry ID/manifest/assets/gallery migration map is prepared and duplicate/loss checks pass. SIGNAL FORGE remains the engineering standard; it must not be conflated with NEON STAGE or SOUND FORGE.

## 5. Approved visual reference evidence

The approved layout contract describes a 1402×1122 user-supplied reference. The indexed Library register currently contains 38 reference files, each marked NEEDS REVIEW unless separately approved in the decision register. Several visually relevant candidates were inspected, but the exact canonical `01 INTERFACE — NEON STAGE DEFAULT` image could not be uniquely mapped to a Library file and approval record during this preflight.

**Status: BLOCKED for pixel-level visual acceptance.** Preserve all reference originals. Do not promote a candidate screenshot to canonical or change geometry based only on resemblance. The required layout/control matrix is still actionable from the written specification and source.

## 6. Current build evidence

At preflight, GitHub Actions run `43592701287` on source SHA `7a7fb55641f36353fe32873c677e5db64142624c` was `in_progress`:
- x64 Release configure: PASS;
- build VST3/test executables: IN PROGRESS;
- CTest, VST3 locate, pluginval strictness 5 and interim package: NOT YET RUN.

This run does not yet validate the matrix or current host behavior. Any later CI result must be read against its exact SHA.

## 7. Ordered next work (per INFO section K)

1. Resolve exact canonical NEON STAGE reference asset and approved ID/name mapping; create a no-loss migration map for all 102 themes.
2. Reconcile `theme_manifest.json`, Skin_01..03 manifests, ThemeManager default/fallback, Gallery order and persistence without changing IDs until mapping is verified.
3. Complete main window and auxiliary-window component/control parity, real callbacks/APVTS bindings, persistence, keyboard/focus and tests.
4. Implement Gallery draft Preview → Apply / Back rollback, Favorites, enlarged preview and keyboard navigation; then persistence/reset/fallback tests.
5. Continue PATTERN section-first model and MIDI/host integration only after UI baseline is reconciled.
6. Audit all FX DSP-to-control routing, AMP & TONE, PUNK GROUP, sampler articulation/velocity/palm mute/acoustic mappings.
7. Continue per-asset license/provenance/SHA-256 work within the single S6 pipeline.
8. Implement production OverlayManager and SecureSkinImport separately, with lifecycle/security regression tests.
9. Run exact-SHA Windows Release, full CTest, pluginval, package audit; record FL Studio 20 host and CPU/RAM/audio profiling separately.

**Phase 0 result:** matrix completed; production source unchanged. Next coding step is identity/reference reconciliation, not an arbitrary UI redesign.
