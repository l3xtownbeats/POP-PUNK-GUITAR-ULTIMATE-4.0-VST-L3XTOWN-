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

## REV.18 live follow-up — 2026-11-15, branch `ui/neon-stage-skin1-completion`

Latest inspected implementation commits:
- `775cd7901ab628579a9f6ce9c7a02924ed72d143` — shared LookAndFeel hover/focus/disabled rendering for rotary controls, buttons and ComboBoxes.
- `ba9eadf6af2c50abc8317a3e0389e6c2f2244d2b` — palette access corrected to use canonical token lookup for shadow/disabled/lowlight.
- `8bb05d7d88fd06c7815882e79ee8b2c3022d771e` — pack manifest index 53 corrected to match NEON STAGE DEFAULT / special.

### Current implementation boundary
The runtime editor is a single large `Source/PluginEditor.cpp`; the inspected tree does not contain separate production source modules named Curator, Sound Hunter, Auto Discover, AMP & TONE or About. Existing source-backed UI surfaces include the main editor, Skin Gallery, Settings, Import Guitar, FX EDIT, AutoChords, Performance display and shared workspace. Do not count absent modules as integrated and do not add active-looking mock controls.

### CI finding and correction
Windows workflow run `43679932917` failed at the G1–G4 skin validator before configure/build. Exact error: `merged registry entry mismatch at index 53: name`. The mismatch was between `SKIN/theme_pack_manifest.json` (Carbon Studio 01 / carbon_studio) and the canonical registry/theme manifest (NEON STAGE DEFAULT / special). The pack entry was corrected in commit `8bb05d7d88fd06c7815882e79ee8b2c3022d771e`. Re-run result must be checked against the new exact HEAD; no build/test pass is inferred.

### Next implementation/validation gate
Continue component-level palette propagation and typography/state audit on actual components; then require successful exact-HEAD G1–G4, Windows x64 Release configure/build, CTest, VST3 asset/package audit and pluginval strictness 5. FL Studio 20 remains a separate manual host check.


---

# REV.18 ADDENDUM — NEON STAGE DEFAULT runtime integration status (2026-11-15)

**Branch:** `ui/neon-stage-skin1-completion`  
**Audit base HEAD:** `e68250720c74f3bb8bb904708532c29c7c730e97`  
**Purpose:** supersede outdated statements in the historical preflight above where they conflict with the current REV.18 branch. Historical findings remain preserved as historical evidence; this addendum records the current source inspection.  
**Audit type:** source-level inventory; not visual acceptance, compilation, runtime or DAW host validation.

## A. Binding visual contract

The user has explicitly approved and fixed one shared design system for all 12 named surfaces:

1. Main Interface
2. AutoChords
3. Curator
4. Sound Hunter
5. Auto Discover
6. FX EDIT
7. AMP & TONE
8. Import Guitar
9. Performance
10. Settings
11. Skin Gallery
12. About

Shared visual language:
- canonical NEON STAGE DEFAULT / SKIN-1 palette;
- typography and text hierarchy;
- rotary controls, buttons and toggles;
- icons and indicators;
- panels, borders and separators;
- Active / Inactive / Hover / Focus / Disabled states;
- animation and visual feedback.

A shared visual language does not authorize identical layouts. Each existing module keeps its functional composition and workflows. A module absent from production source is not to be represented by a decorative or active-looking placeholder.

## B. Non-negotiable constraints

1. No fake buttons, fake functions, simulated meters or decorative elements represented as functional.
2. No changes to Audio Engine, DSP, MIDI processing or APVTS definitions/state.
3. No renaming or changing existing control IDs, parameter IDs, control bindings or attachment meaning.
4. No breaking existing functions, panel workflows or module layouts.
5. User-uploaded NEON STAGE DEFAULT GitHub mockups remain the primary visual reference; this source audit is not pixel-level visual acceptance.
6. The theme must be consumed by the actual plugin runtime, not merely exist as artwork or JSON.

Scope is visual/UI integration only.

## C. Current source-backed module inventory

| Module | Current source evidence | Current status | Remaining work / guardrail |
|---|---|---|---|
| Main Interface | `PopPunkGuitarUltimateAudioProcessorEditor`, `PPGUPremiumLookAndFeel`, preset browser, 9 attached knobs, 7 attached FX toggles, MELODY/PATTERN, MIDI keyboard | SOURCE PRESENT / THEME CONNECTED / PARTIAL VISUAL AUDIT | Complete typography, text editor, tabs, status and component-state audit; compare layout to approved mockup. Preserve all existing attachments and geometry until approved. |
| AutoChords | `PPGUAutoChordsPanel` in `PluginEditor.cpp`; existing setup/preview controls | SOURCE PRESENT / VISUAL PALETTE CONNECTED / GENERATION UNAVAILABLE | Keep MIDI generation explicitly unavailable; do not add a fake Generate action. Audit all text/control states. |
| Curator | No production Curator panel/class established in inspected editor/source inventory | MISSING / NOT INTEGRATED | First define against real library-analysis/Curator functionality. No decorative cards or fake GREEN/YELLOW/RED results. |
| Sound Hunter | No production Sound Hunter UI/pipeline established in inspected source inventory | MISSING / NOT INTEGRATED | Must use the existing approved S6/Sound Hunter architecture; no second pipeline or fake search results. |
| Auto Discover | No production Auto Discover panel established in inspected source inventory | MISSING / NOT INTEGRATED | Integrate only when backed by a real discovery pipeline and source callbacks. |
| FX EDIT | `PPGUFXEditPanel`; effect selectors, power control and APVTS slider/button attachments | SOURCE PRESENT / THEME PALETTE CONNECTED / PARTIAL | Audit all labels, text fields, graph/status areas and all enabled/disabled/focus states. Never invent effects or alter parameter IDs. |
| AMP & TONE | Existing main-editor amp/EQ controls and parameter attachments; no separate complete AMP & TONE module established | PARTIAL / NO SEPARATE COMPLETE MODULE CONFIRMED | Reuse only real amp/tone parameters and current bindings. Do not create model selectors or cabinet controls without real DSP routes. |
| Import Guitar | `PPGUImportGuitarPanel`; SFZ chooser and asynchronous load path | SOURCE PRESENT / THEME PALETTE CONNECTED / PARTIAL | Audit loading, success/error states and keyboard/focus styling; no fake import status. |
| Performance | `PPGUPerformanceDisplay` and editor palette propagation | SOURCE PRESENT / PARTIAL | Audit real MIDI/transport display states. Do not show unmeasured CPU/RAM or fabricated activity indicators. |
| Settings | `PPGUSettingsPanel`; workspace scale/gap, restore default theme and open gallery | SOURCE PRESENT / UI-ONLY | Palette and wording are theme-aware. Do not imply these controls are full audio/MIDI/library settings. |
| Skin Gallery | `PPGUSkinGalleryPanel`; registry cards, preview images, search, family filter, active state and click-to-apply | SOURCE PRESENT / PARTIAL | Draft Preview → Apply/Back rollback, favorites, full keyboard navigation, rescan and transition/fade remain unaccepted. Verify real image files and runtime paths. |
| About | No production About panel established in inspected source inventory | MISSING / NOT INTEGRATED | Add only a factual product-information panel; no dummy buttons or invented version/build data. |

**Important distinction:** “not established in inspected source” is not proof that no related design document exists. It means the module is not accepted as a production runtime surface based on the inspected live source.

## D. Shared-system implementation evidence

| Shared element | Current source evidence | Status |
|---|---|---|
| Canonical palette | `PPGUThemeManager::Palette`, 21 canonical tokens and NEON STAGE fallback | IMPLEMENTED IN SOURCE; exact runtime asset packaging still pending |
| Runtime theme application | Editor theme manager and palette refresh; workspace receives palette and updates workspace chrome/open theme-aware panel contents | SOURCE CONNECTED |
| Typography | Palette defines UI/display typefaces, weight, scale and letter spacing; LookAndFeel sets default sans-serif typeface | PARTIAL — many labels still set explicit fonts locally; global hierarchy/scale/letter-spacing parity not demonstrated |
| Rotary controls | `PPGUPremiumLookAndFeel::drawRotarySlider` | SHARED RENDERING: hover, focus, disabled states present |
| Buttons | `drawButtonBackground` | SHARED RENDERING: active/toggle, hover, pressed, disabled and keyboard focus present |
| ComboBoxes | `drawComboBox` | SHARED RENDERING: hover/pressed, disabled and keyboard focus present |
| Text editors | `PPGUPremiumLookAndFeel::fillTextEditorBackground` and `drawTextEditorOutline` (JUCE 7.0.12 API) | SHARED background + hover/focus/disabled outline states implemented; text colour/font hierarchy remains component-level |
| Icons / indicators | Theme tokens include LED colours; no complete shared icon/indicator component system established | NOT COMPLETE |
| Panels / borders / separators | Workspace chrome uses palette-derived background, panel, border, heading and text colours | SOURCE CONNECTED; per-panel parity audit pending |
| Animation / feedback | No complete shared animation system or timing contract established by this source audit | NOT COMPLETE |
| Hover / Focus / Disabled | Shared rotary/button/ComboBox renderer | PARTIAL — not yet proven for every interactive component in every module |

## E. Protected control inventory

The existing nine main knobs and seven FX bypass controls remain fixed:

- `ampGain`, `eqHigh`, `eqMid`, `eqLow`, `velocity`, `dynamics`, `swing`, `humanize`, `accent`.
- `compressorOn`, `eqOn`, `distortionOn`, `chorusOn`, `delayOn`, `reverbOn`, `cabinetOn`.

This addendum authorizes no ID changes, remapping, new APVTS parameters or changed audio/MIDI behavior.

## F. Ordered implementation and acceptance gates

1. Finish component-level visual audit of all existing real controls: fonts, hierarchy, text editors, tabs, labels, status, hover, focus, disabled and pressed states.
2. Add/reuse shared UI-only drawing/style helpers only where the real component inventory supports them; do not change module geometry/workflow without reference comparison.
3. Locate and integrate Curator, Sound Hunter, Auto Discover, AMP & TONE and About only against real product functionality. If the production functionality is absent, record the module as blocked/missing rather than fabricate it.
4. Verify all runtime theme assets and packaging paths from the actual VST3 bundle.
5. Run G1–G4 skin validators and PNG CRC/dimension checks.
6. Run exact-HEAD Windows x64 Release configure/build, full CTest, locate packaged VST3 and pluginval strictness 5.
7. Perform separate manual FL Studio 20 visual/interaction acceptance.

## G. Current validation status

At audit base HEAD `e68250720c74f3bb8bb904708532c29c7c730e97`:
- PR #43 remains Draft and unmerged.
- GitHub Actions workflow lookup returned no runs associated with this exact HEAD at the time of inspection.
- No exact-HEAD Windows build, CTest, pluginval, VST3 runtime asset-load check or FL Studio 20 host pass is claimed.
- Source inspection is not a substitute for visual or runtime acceptance.

**Decision:** continue source-backed NEON STAGE integration incrementally. Do not merge or mark release-ready until every applicable gate above has exact-SHA evidence.


## 2026-11-15 — UI entry-point naming correction

The UI entry point previously called **Auto Discover** in historical audit rows now has the approved display name **AUTO SOUND HUNTER**. This is a naming correction only. Keep the separate **Sound Hunter** subsystem/pipeline and **Curator** module distinct. Preserve the existing schema key `Auto_Discover`; do not rename stable control IDs. The current button remains disabled until a real production discovery/search/import backend is integrated and validated. No fake functionality is permitted.

 
## 2026-11-15 — REV18 approved-theme audit integration

The binding UI references are `PPGU_UI_THEME_SPEC_FULL.txt` v1.3 and the canonical `01 INTERFACE  NEON STAGE DEFAULT .png`, with 02–07 as supporting module/workspace references. MINIMALISM remains the default mode within NEON STAGE, not a separate skin; cheap/simplified legacy themes do not count among the 16 active premium themes.

The REV18 audit now validates the 16 approved manifests, names, stable IDs, 21 palette tokens, required control IDs, shared typography, visual-only compatibility, unique texture/bezel/crest resources, effects-path parity, safe asset paths, SVG active/external content and PNG integrity/dimensions. Nine security tests pass locally. Current non-strict audit: 0 errors / 45 expected warnings. Outstanding warnings are the missing preview/hero PNG pair and pending registry promotion for SKIN-2 through SKIN-16. No gallery registration or ID migration is claimed.


## 2026-11-15 — Playing Core hero artwork: first implementation checkpoint

**Working branch:** `ui/neon-stage-playing-core`  
**Base:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV.18`  
**Source commits:** `05ec031e2ad8e081aaa2611c91bf4b6c9fc8d3cc`, `2ef6cfd8aec0b2974c88ad32158a6da577326e7c`, `0f3022d8f4e855159771fdbad6d6fa51ce689f35`, `0199d4dcebcc02ac8fcbe959dbedee9dfb8168bb`.

Implemented:
- `PluginEditor.h` now owns a JUCE image and source-file cache for the active skin's hero artwork.
- `PluginEditor.cpp` resolves `assets.hero` from the active skin manifest, rejects absolute/traversal paths, checks that the resource remains inside the skin directory, and loads the image only when the active hero changes.
- Main Playing Core reserves a dedicated artwork strip and paints the active theme's hero image with a theme-derived frame.
- The center section title is now `PLAYING CORE`; the gallery entry point is labeled `SKINS`.
- NEON STAGE's existing `Skin_053/assets/hero.png` is present in the branch and is 800×450. No new artwork or placeholder was fabricated.
- All 9 approved rotary APVTS bindings and 7 approved FX bypass bindings remain unchanged. No DSP, MIDI, processor state or audio callback was touched.

**Validation boundary:** source change only. Exact-HEAD Windows build, CTest, pluginval strictness 5, portable VST3 packaging, pixel-level comparison and FL Studio 20 acceptance remain PENDING. This is the first bounded implementation slice, not completion of the NEON STAGE interface.


## 2026-11-15 — AUTOCHORDS REQUIRED HARMONY TOOLS (USER CONFIRMED)

| Требование | Live source | Статус | Следующий шаг / ограничения |
|---|---|---|---|
| Circle of Fifths | PPGUAutoChordsPanel: не обнаружен | REQUIRED / NOT IMPLEMENTED | Интерактивный квинтовый круг для выбора тональности/гармонического контекста; связать с chord vocabulary и chord progression. |
| Chord Management | PPGUAutoChordsPanel: не обнаружен | REQUIRED / NOT IMPLEMENTED | Управление аккордами и последовательностью: добавление, удаление, перестановка, отображение и сохранение набора. Точная interaction model — отдельный следующий design/code slice. |

Обе функции подтверждены пользователем как обязательные. Это реальные функциональные инструменты, не декоративные элементы. Сохранять 27 control-ID, APVTS IDs и audio/MIDI boundaries. Наличие Circle/Manager UI не означает готовность MIDI generation/playback: backend остаётся отдельным gate до интеграции и тестов.
