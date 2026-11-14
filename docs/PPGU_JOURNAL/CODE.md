PPGU — CODE DEVELOPMENT JOURNAL
================================

Назначение
---------
CODE.txt — хронологический индекс всех изменений исходного кода POP PUNK GUITAR ULTIMATE.
Журнал ведётся append-only: новые записи добавляются в конец; исторические записи не стираются и не переписываются.

Где находится полный код
------------------------
Полный исходный код хранится в канонических файлах репозитория, а не дублируется вручную в этом журнале:
- Source/ — production C++ / JUCE code, processor, UI, DSP и runtime modules;
- tests/ — автоматические тесты и contract tests;
- tools/ — validators, generators, merge/build helpers;
- CMakeLists.txt, .github/workflows/ и остальные build/config files — конфигурация сборки и CI;
- SKIN/, assets/, presets/, licenses/ и docs/ — соответствующие ресурсы, данные и документация.

CODE.txt обязан указывать точный commit SHA и пути затронутых файлов. Полный код на любой ревизии восстанавливается из Git по указанному SHA. Не хранить второй копией исходники в журнале: такая копия быстро расходится с рабочим кодом и становится опасным источником устаревших правок.

Обязательные поля каждой записи
-------------------------------
1. Дата и тип изменения.
2. Ветка, commit SHA и parent SHA.
3. Точные пути изменённых файлов.
4. Затронутые классы, методы, функции, структуры, параметры или тесты.
5. Что изменено и зачем.
6. Совместимость и запрещённые области, которые не затрагивались.
7. Добавленные/изменённые тесты и их результат на этом SHA.
8. CI run ID, ссылки/артефакты и статусы, если доступны.
9. Открытые дефекты, ограничения и следующий шаг.

Правила достоверности
---------------------
- Перед изменением сверять branch HEAD, исходные файлы и незакоммиченные/параллельные ветки, если они доступны.
- Не переносить старый код поверх более нового. Cherry-pick/merge только после сравнения изменений и проверки конфликтов.
- Не объявлять BUILD PASS, TESTED, HOST VERIFIED или RELEASE READY без результата на точном commit SHA.
- Старый CI результат не является подтверждением нового SHA.
- Каждое production code изменение должно иметь соответствующую запись здесь и в Info.txt.
- Если этап только документальный, прямо указывать, что production source code не менялся.
- Аудиобезопасность PPGU: Skin Engine не должен менять DSP, audio callback, sampler, MIDI, APVTS IDs/semantics, presets, SoundBank или поведение существующих контролов.

Хронология
==========

## 2026-11-12 — ThemeManager canonical 21-token palette API

**Тип:** UI/theme infrastructure code change.
**Ветка:** work/signal-forge-theme-tokens.
**Commit:** 57fe40044532eeab4bc66ccec37bce865904d455.
**Parent:** 71c2d852c1dc1d138e903fb577533a17ca540002.
**Commit message:** feat(ui): expose canonical 21-token theme palette with legacy mapping.

**Изменённые области кода (по commit diff и соответствующей INFO-записи):**
- Source/UI/ThemeManager.h — canonical Palette token model / API.
- Source/UI/ThemeManager.cpp — token lookup, JSON palette loading, legacy mapping and fallback compatibility.
- Source/PluginEditor.cpp — перенос существующих editor color assignments с отдельных hardcoded palette constants на ThemeManager token API.
- Tests/ThemeManagerTests.cpp — contract coverage для canonical tokens и legacy mapping (точное имя файла/наличие проверить на выбранной ветке перед дальнейшим изменением).

**Суть изменения:**
- Введён единый набор из 21 canonical Skin Studio tokens: window_bg, panel, panel_elevated, control_bg, control_face, control_border, text_primary, text_secondary, text_disabled, accent, accent_glow, danger, warning, success, lcd_bg, lcd_text, led_on, led_off, shadow, highlight, lowlight.
- Legacy skin 1–51 сохраняют совместимость через mapping существующих цветов и детерминированные fallback values для отсутствующих semantic colors.
- Существующий editor использует ThemeManager token API для ряда видимых компонентов и панелей.
- Геометрия существующего окна и APVTS attachments в этом изменении не менялись.

**Границы изменения:** Audio Engine, DSP, audio callback, MIDI routing, APVTS IDs/semantics, presets, SoundBank и Skin_052 artwork/assets/registry/schema не менялись. Изменение не является полной реализацией Gallery, dynamic resource discovery, layout engine, secure asset pipeline или runtime gates G5–G7.

**Проверки:** INFO на момент записи сообщал CI run #971 (ID 43146185476) и #972 (ID 43146540434) как IN_PROGRESS на родительских SHA соответственно; эти статусы не доказывают PASS для commit 57fe400. Полный Windows Release/CTest/pluginval/host status для SHA 57fe400 здесь не подтверждён. Не помечать как BUILD VERIFIED.

**Дальнейшая работа:** продолжить Skin Engine как приоритет №1; сначала повторно проверить актуальную ветку и HEAD, затем проверить token API и тесты на точной ревизии, после чего выполнять gallery/runtime/resource loading/control parity/hot-swap/DPI/ECO gates из SKIN specification.

## 2026-11-12 — INFO/CODE journal bootstrap

**Тип:** documentation only; создание постоянного CODE journal.
**Ветка:** work/signal-forge-theme-tokens.
**INFO commit:** 2d308afdb8990234a35c55b715567581c459c4f9.
**Production source code:** НЕ менялся этим документальным этапом.
**Что зафиксировано:** append-only правила ведения INFO.txt и CODE.txt; обязательная привязка каждой записи к точному commit SHA, путям, символам, тестам и статусам; правило сохранять полный код в канонических source files и не заменять новые изменения старыми.
**Skin Engine priority:** зафиксирован как первостепенная задача до завершения полной интеграции и проверки.
**Важное замечание:** на момент фиксации рабочая ветка имела code commit 57fe400; main имел HEAD 6174041. Эта запись не утверждает, что ветка work/signal-forge-theme-tokens является новейшей среди всех веток репозитория. Перед следующим code change требуется branch/HEAD preflight.

## TEMPLATE — новая запись (копировать в конец файла)

## YYYY-MM-DD — Краткое название изменения

**Тип:** production code / test / build / docs / revert.
**Ветка:**
**Commit SHA:**
**Parent SHA:**
**Файлы:**
- path/to/file — классы/методы/функции и суть изменения.

**Причина:**
**Изменения:**
**Совместимость / non-goals:**
**Тесты:**
**CI run / artifacts:**
**Статус:** IMPLEMENTED / VALIDATION PENDING / PASS / FAIL / REVERTED.
**Следующий шаг:**


## 2026-11-12 — MASTER JOURNAL GOVERNANCE / FULL PRODUCT CHANGE LEDGER

**Type:** documentation / process contract only. No production source, tests, assets, presets, SoundBank, skins, audio or UI code changed.
**Branch:** `work/signal-forge-theme-tokens`
**INFO policy commit:** `f29eac4725b55f0d732b0132f907f614d2af2c9d`
**INFO blob:** `95c16e6b121fd02b474cd5b2f0828759a8a6e830`

**Scope now covered by the mandatory ledger:** product identity and VST3 platform; audio engine/core/voice and realtime architecture; sampler/SFZ/WAV/streaming/cache; MIDI, Melody, Pattern, chord/performance systems; DSP and FX; banks, libraries, samples, articulations, layers, loops, IR and licensing; factory/user presets and browser; GUI shell, controls, windows, settings and state; Skin Engine/Skin Studio/themes/layouts/assets/gallery; import/discovery/curation; diagnostics, performance budgets, tests, CI, pluginval, DAW host verification, packaging/release; defects, risks, rejected designs, roadmap and technical debt.

**Mandatory source-of-truth hierarchy:** current canonical source tree and build config prove implementation; tests/CI prove only checks actually run on exact SHA; specifications define intended behavior; INFO is the append-only project chronology and status ledger; CODE is the append-only exact code/change index; schemas/manifests/license evidence govern only their declared scope. Conflicts must be investigated and reconciled explicitly, never silently overwritten.

**Mandatory per-change transaction:** branch/HEAD preflight and compare all relevant newer work; inspect related specs, schemas, code and tests; make a scoped change preserving invariants; run checks on exact SHA; update INFO and CODE with identical branch/commit/parent/path/symbol/test/status references; read both files back and cross-check consistency. If either journal is missing or inconsistent, status remains JOURNAL INCOMPLETE.

**Correction policy:** preserve historical entries. Fix inaccurate/obsolete facts with a dated Correction/Supersedes entry identifying the wrong statement, evidence, corrected fact and affected documents. Mark superseded/rejected/invalid material explicitly. Never replace newer work with older code. A later timestamp alone is insufficient; compare actual file changes, merge-base, compatibility and tests.

**Status vocabulary:** REQUIRED / DESIGNED / SOURCE PRESENT / INTEGRATED / TESTED / HOST VERIFIED / RELEASED, plus PARTIAL / BLOCKED / DEFERRED / SUPERSEDED / REJECTED / INVALID as applicable. Never infer implementation from a spec, parameter ID, UI placeholder, manifest or asset. Never claim BUILD/CTest/pluginval/FL Studio/release PASS without exact-SHA evidence.

**Per-change record must include:** UTC timestamp, product/revision, branch/base HEAD/parent/result SHA; change class; defect/requirement and reason; exact paths and symbols; before/after; unchanged invariants; compatibility/migration; test commands/configuration/results/logs/artifacts; CI run IDs; host verification separately; known issues/risks; next action. Documentation-only changes must say production code was unchanged.

**Product-wide reconciliation obligation:** keep INFO, CODE, master prompt, architecture docs, audits, control schema, theme manifests, bank/preset schemas, licensing records, build files and tests mutually consistent. Avoid competing specifications; designate one canonical source per domain and link supporting documents. Before release, reconcile all records and actual package/binary on one release candidate SHA.

**Completion rule:** every accepted decision, code/content/config change, correction, revert, failed test and discovered defect is recorded. Journal updates are part of the change, not optional administration.

## 2026-11-12 — PPGU Master Spec Package / Neon Skin + Overlay + Secure Import

**Тип:** documentation / architecture specification; production source change отсутствует.
**Ветка:** `work/signal-forge-theme-tokens`.
**Base / parent SHA:** `03545e718e7276756a56605fad3de0a0e1933dfd`.
**Документ:** `docs/PPGU_MASTER_SPEC_PACKAGE_RU.md`.
**Изменённые production paths/symbols:** none.
**Суть:** зафиксированы frontend phase sequence E1→O1→O2→S1→E2→E3→E4/O3; OverlayPanel/OverlayManager contracts; SkinManifest 21-token + exact control parity; .ppguskin secure import, ZIP/asset/SVG protections, quarantine and SHA-256; contract-test acceptance matrix.
**Compatibility / non-goals:** DSP, audio callback, MIDI, APVTS IDs/semantics, presets, SoundBank, existing control behavior unchanged. No implementation or test target is claimed by this documentation entry.
**Tests/CI:** not run; no BUILD/CTest/pluginval/host/release PASS claimed.
**Статус:** DESIGNED; implementation PENDING.
**Следующий шаг:** N0 preflight; implement E1 as a separate scoped change and run full exact-SHA gates.


## 2026-11-12 — REV.17 E1 validator hardening (overflow + control-map regression coverage)

**Тип:** production source + tests.
**Ветка:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`.
**Production/test commit SHA:** `d42f495bb2228ab9e08295c7b0ecca07c70ba8dc`.
**Parent SHA:** `a66c46d58c88e8248535af235a53ad9cb6348a53`.
**Файлы и символы:**
- `Source/UI/ThemeManager.cpp` — `validateAssetSpec()`: заменено небезопасное narrowing-сравнение int64 dimensions на сравнение исходного значения с ожидаемым размером в int64.
- `tests/ThemeManagerTests.cpp` — `main()`: добавлены проверки hostile width wraparound, отсутствующего обязательного control ID и лишнего control ID.

**Причина/результат:** устранён подтверждённый статическим анализом integer wraparound в skin asset dimensions; добавлены regression cases, демонстрирующие недостаточность старого простого size>=27 gate.
**Совместимость:** только visual theme manifest validation; Audio/DSP/MIDI/APVTS/presets/SoundBank и plugin editor layout не затронуты.
**Тесты:** добавлены; execution NOT RUN. CI exact SHA: NOT CONFIRMED. Pluginval/FL Studio: NOT RUN.
**Статус:** SOURCE PRESENT; TESTED PENDING; HOST VERIFIED NO; RELEASED NO.
**Следующий шаг:** завершить G1/G2/G4 validator и затем O1 overlay implementation.


## 2026-11-12 — REV.17 exact Skin Studio control parity

**Тип:** production source correction.
**Ветка:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`.
**Commit SHA:** `43523a6998e17fdf25999c1df2c26e53f9e81c79`.
**Parent SHA:** `a893cbaa419691fb83801d0d846218aeacd54a52`.
**Файл/символ:** `Source/UI/ThemeManager.cpp` — `validateSkinStudioManifest()`.
**Изменение:** exact control-set enforcement: 27 canonical required IDs plus `sync_selector`, exactly 28 properties; missing/unknown IDs rejected.
**Связанные tests:** missing About / unapproved extra ID regressions in `tests/ThemeManagerTests.cpp`, introduced in prior production/test commit `d42f495bb2228ab9e08295c7b0ecca07c70ba8dc`.
**Совместимость:** UI theme manifest validation only; audio and preset contracts unchanged.
**Тесты/CI:** execution NOT RUN; exact-SHA CI pending; pluginval/host NOT RUN.
**Статус:** SOURCE PRESENT; TESTED PENDING; HOST VERIFIED NO; RELEASED NO.
**Следующий шаг:** verify exact-SHA workflow and continue G1/G4.


[2026-11-12] TEST FIX — ThemeManager JUCE 7 API
Branch: POP-PUNK-GUITAR-ULTIMATE-VST-REV-17
Base SHA: f490ab2abb41f9325189181601699e4bb8e0a634
Files: tests/ThemeManagerTests.cpp, Info.txt, CODE.txt
Change: Correct DynamicObject::getProperty call in extra-control contract test to JUCE 7.0.12 one-argument API.
Reason: CI run 43166352726 failed compiling ppgu_theme_manager_tests with MSVC C2660 at ThemeManagerTests.cpp:128.
Scope: test source and append-only journals only; no production audio/UI behavior changed.
Verification: pending CI on resulting commit SHA; no passing build claimed.


## 2026-11-12 — REV.17 CI correction: SIGNAL FORGE canonical default skin

**Тип:** test-contract correction + append-only journal.
**Ветка:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`.
**Parent SHA:** `d05c7128ab8f53b4087fce47ff9c52f229017114`.
**Причина:** Windows x64 CI run `43167387194` compiled all targets successfully, then CTest failed only `ppgu_skin_manifest_contract`: `tests/ValidateSkinManifests.py` hard-coded `DEFAULT_ID = "skin-51"`, while the canonical registry contains 52 entries and correctly declares `defaultSkin = "skin-052"` (SIGNAL FORGE). This was a stale validator expectation, not a manifest defect.
**Изменение:** update the validator's expected default ID to `skin-052`; retain the manifest's SIGNAL FORGE default and all 52 registered skins. No production UI/audio behavior changed.
**Пути:** `tests/ValidateSkinManifests.py`, `Info.txt`, `CODE.txt`.
**Совместимость / invariants:** no DSP, processBlock, sampler, MIDI, APVTS IDs/semantics, presets, SoundBank or runtime skin code changed.
**Проверка до исправления:** run `43167387194`, SHA `d05c7128ab8f53b4087fce47ff9c52f229017114`: configure PASS; VST3 and test executables BUILD PASS; CTest 13/14 PASS; sole failure was stale `DEFAULT_ID` assertion; pluginval and packaging skipped because CTest failed.
**Статус:** SOURCE FIXED IN THIS COMMIT; exact-SHA CI / CTest rerun PENDING; pluginval/host verification NOT CLAIMED.
**Следующий шаг:** verify new SHA CI; do not merge or start dependent implementation waves until green.


## 2026-11-12 — SIGNAL FORGE v7 master standard accepted; baseline pipeline verified

**Тип:** specification/documentation + exact-SHA CI record; production source unchanged.  
**Ветка:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`.  
**Base SHA:** `881732c44214ea9160150fc69cd77ad7d85d5bc0`.  
**Документ:** `docs/PPGU_SIGNAL_FORGE_V7_MASTER_RU.md`.  
**Затронутые production symbols:** none.  
**Причина:** сохранить принятый пользователем обязательный стандарт Skin System v7 как канонический документ для дальнейших F3–F10 работ.  
**Изменение:** добавлена полная структурированная спецификация терминологии, инвариантов, зон A–H, модулей, initial five skins, A6, фронтов, гейтов и запретных областей.  
**Проверка:** A6 cycle completed for documentation transaction. Baseline CI run `43169004397` on exact base SHA: Windows x64 Release configure/build PASS; CTest 14/14 PASS; pluginval strictness 5 PASS; package/upload PASS.  
**Артефакт:** ID `13922345092`; SHA-256 `5c41c1f7024211edd47edf2c2abf19a934e5b071d7087bb9147f38518dd9e25a`.  
**Совместимость:** no changes to DSP, processBlock, sampler, MIDI, APVTS, presets, SoundBank or production UI source.  
**Ограничения:** FL Studio 20 host verification NOT performed; CPU/RAM/paint-time/hot-swap zero-residue not measured; full Skin Gallery not implemented.  
**Статус:** SPECIFICATION ACCEPTED; BASELINE CI GREEN; SKIN SYSTEM PARTIAL.  
**Следующий шаг:** implement and test the Skin Gallery and theme persistence/asset pipeline in isolated changes; resolve initial five-skin registry mapping before changing default identity.


## 2026-11-12 — SIGNAL FORGE v10.1 specification clarification

**Тип:** documentation / engineering contract; no production symbols changed.
**Ветка:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`.
**Parent SHA:** `7a4abc486038ad820fba6af04d5fdbd5c2238797`.
**Файл:** `docs/PPGU_SIGNAL_FORGE_V10_1_ENGINEERING_CLARIFICATION_RU.md`.
**Суть:** уточнены audio-thread hard guarantees vs measurable UI cost; CPU/RAM/hot-swap/key latency methodology; bounded asset cache and immutable skin snapshots; unlimited library vs resident quotas; portable content-root resolution and path traversal protections; SPDX/license review; single-source-of-truth schemas; package manifest and tiered acceptance gates.
**Совместимость:** production source unchanged; DSP/processBlock/sampler/MIDI/APVTS/presets/SoundBank untouched.
**Тесты:** documentation-only. No compile, CTest, pluginval, performance, host or release pass claimed. Exact-SHA CI pending.
**Статус:** SPECIFICATION CLARIFIED; implementation status unchanged.


## 2026-11-12 — README product page coherence refresh

**Тип:** documentation page rewrite; no production symbols changed.
**Ветка:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`.
**Parent SHA:** `700f56dbbeeab0e7e02559a29dfa9a5d92772b23`.
**Пути:** `README.md`, `Info.txt`, `CODE.txt`.
**Суть:** полностью обновлена публичная главная страница GitHub: шапка и навигация, технические характеристики, visual identity NEON PUNK, обязательная skin collection, interface inventory zones A–H, performance targets, one-folder content model, licensing, acceptance tiers and documentation index. Удалены устаревшие/противоречивые сведения и ссылки на main; статусы DESIGN/SOURCE/TEST/HOST/RELEASE разведены.
**Границы:** source code, DSP, processBlock, sampler, MIDI, APVTS, presets and SoundBank untouched.
**Проверка:** входной HEAD `700f56dbbeeab0e7e02559a29dfa9a5d92772b23`; README/Info/CODE fetched from the REV.17 branch. CI run `43171568092` for parent SHA was IN_PROGRESS at edit time; exact resulting SHA requires its own CI confirmation. No local compile-check performed for documentation-only change.
**Статус:** DOCUMENTATION UPDATED; CI PENDING; implementation unchanged.


## 2026-11-12 — REV.17 Multi-Window automatic tiling

**Branch:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`  
**Parent:** `60f3b12dad135cb7aed37c48b4a5dde919365247`  
**Source commits:** `0b8096d55ea26f01489d4ff257dee39b6022f23d`, `d4c0e4b7addb622ef2dc2b77fb2dac01634e24f6`  
**Files / symbols:**
- `Source/UI/PPGUMultiWindowWorkspace.h` — `layingOutPanels` reentrancy state.
- `Source/UI/PPGUMultiWindowWorkspace.cpp` — `layoutPanels()` now consumes `calculatePanelLayout()`; `handleGeometryChanged()` persists manual geometry without forcing re-tile; `Window::setPanelBounds()` restores normal maximize state before automatic placement.
- `Source/UI/AuxiliaryPanelLayout.cpp` — approved maximum two-column layout (up to 2×3).
- `tests/AuxiliaryPanelLayoutTests.cpp` — assertions for 1–6 panel grid shape.
- `Source/UI/PPGUMultiWindowWorkspace_README_RU.md` — current integration scope and verification boundary.

**Scope:** UI geometry/workspace only. Audio callback, DSP, sampler, MIDI, APVTS IDs/semantics, presets and SoundBank untouched.  
**Verification:** static source review only; local Windows build/CTest not run. CI run `43173372464` targets the first source commit only and was IN_PROGRESS at journal time; final source SHA CI pending. FL Studio 20 host/DPI acceptance not run.  
**Status:** SOURCE PRESENT; TESTED PENDING; HOST VERIFIED NO; RELEASED NO.  
**Next:** exact-SHA CI, then visual checks for 1–6 panels and 100/125/150% DPI.


## 2026-11-12 — REV.17 FX EDIT responsive layout correction

**Branch:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`  
**Parent:** `fccdf9647d7088246a0bfe129c5ddd943ec274a1`  
**File / symbols:** `Source/PluginEditor.cpp` — `PPGUFXEditPanel::paint()`, `PPGUFXEditPanel::resized()`.  
**Change:** FX EDIT graph height adapts to panel height; graph is hidden/omitted at compact sizes; control cell height is calculated from visible control count, columns, rows and available client area. Fixes potential clipping of the 7-control EQ group at the 620×440 minimum panel size.  
**Boundaries:** no APVTS IDs/attachments, FX enable mappings, DSP, processBlock, sampler, MIDI, presets or SoundBank changes.  
**Verification:** static source/patch review only; compile, CTest, pluginval, performance and FL Studio 20 host checks pending on resulting SHA.  
**Status:** SOURCE CHANGE PREPARED; TESTED PENDING; HOST VERIFIED NO; RELEASED NO.


## 2026-11-12 — REV.17 Factory Bank: защита согласованного набора из 300 пресетов

**Files:**
- `presets/factory_bank_300.json` — основной согласованный каталог; сохраняется без замены восьмипрофильным JSON.
- `Source/PluginProcessor.cpp` — APVTS `playMode`: Choice `Melody` / `Pattern`; допустимые индексы только 0 и 1.
- `Info.txt` — полная запись продуктового решения и ограничений.

**Контентный контракт:**
- ровно 300 заводских пресетов; структура 150 MELODY + 150 PATTERN остаётся базовым требованием;
- поп-панк-направления, стилистики групп/исполнителей, группировки и звуковые профили сохраняются;
- 8 присланных профилей — только reference/QA material для сверки и возможного точечного сопоставления с существующими слотами; не отдельное расширение каталога и не замена;
- значения `playMode` вне диапазона 0..1 запрещены при текущем APVTS;
- не менять художественную концепцию, названия/направления/структуру без отдельного явного согласования.

**Интеграционные границы:** не менять DSP, sampler, MIDI, APVTS ID/семантику, sample library или skin inventory в рамках этой записи. Не заявлять, что восемь профилей уже внедрены в 300 позиций или что все 300 пресетов полностью воспроизводимы, пока не выполнены schema/APVTS/sample mapping и runtime tests.

**Проверка:** сверены существующий каталог и APVTS playMode declaration. Полная проверка 300 записей, диапазонов каждого параметра, ссылок на sample profiles и фактического воспроизведения не выполнялась в рамках этой journal-only фиксации.

**Status:** REQUIREMENT RECORDED; PRESET DATA UNCHANGED; IMPLEMENTATION/VALIDATION PENDING.


## 2026-11-12 — REV.17 outer shell section chrome

**Branch:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`  
**Parent SHA:** `6ea8d5873706890f29496465d6078bd235fee42b`  
**Files / symbols:**
- `Source/PluginEditor.h` — `drawPanel(Graphics&, Rectangle<int>, String sectionTitle)`.
- `Source/PluginEditor.cpp` — `drawPanel()`, `paint()`, `resized()`.

**Change:** добавлены theme-token-driven captions and separators for the three approved shell zones: PRESET LIBRARY, GUITAR ENGINE / PERFORMANCE, SIGNAL CHAIN. Each zone reserves a 34 px header row so painted labels cannot overlap live controls. Existing panel components and APVTS attachments remain the owners of interaction.

**Boundaries:** no DSP, processBlock, sampler, MIDI, APVTS ID/semantics, preset, SoundBank, theme manifest or audio behavior changes. No new controls or fake functionality.

**Verification:** source-level replacement checks passed; build, CTest, pluginval, DPI and FL Studio 20 visual checks pending for resulting SHA. CI run #1043 is on parent SHA `6ea8d5873706890f29496465d6078bd235fee42b` and does not validate this change.

**Status:** SOURCE CHANGE; VALIDATION PENDING.  
**Next:** exact-SHA CI, then continue only with the approved external shell/theme presentation.



## Journal closure — outer shell section chrome

**Implementation commit:** `4381c1d1d60e9769173b89922d52bb0967b98bcf` (parent `6ea8d5873706890f29496465d6078bd235fee42b`).
**Exact-SHA CI:** run #1044 / ID `43180948804`, PENDING at closure. The source patch and INFO/CODE entries were fetched back and checked; no build or host pass is claimed. This commit is a journal-only follow-up and leaves production source unchanged.


## 2026-11-12 — REV.17 Zone F specification addendum

**Change class:** documentation/specification only; no production source or test code changed.  
**Branch:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`.  
**Specification commit:** `c6fccfbf6d960fb0fcb362eaa4128bd4b461e637`.  
**INFO journal commit:** `ebe8987922c83fb748d0cffa377c9dacc21d41a0`.  
**Path:** `docs/PPGU_MASTER_SPEC_PACKAGE_RU.md`, section `4.4 ZONE F — STRUMMER & GROOVE / PATTERN CONTROL STRIP`.

**Added contract:** Zone F is a logical central-workspace section for Pattern/Strummer controls, not a fourth external column. It binds only existing IDs `patternSlot`, `patternLength`, `syncMode`, `bpm`, and `startOnFirstChord`; Pattern-only controls are disabled in Melody mode. Tempo is editable only for Internal sync; Host tempo is read-only when playhead tempo is available. Existing C controls swing/humanize/accent are not duplicated.

**Deferred controls:** `swingBase`, `patternQuantize`, `preserveHumanTiming`, `articulation`, `pickStyle`, `endingLength`, `palmHitOnEnding`, `stringSlapOnEnding` remain unavailable/disabled until runtime binding and scheduler behavior are verified.

**Compatibility boundary:** no APVTS IDs/semantics, scheduler, MIDI, DSP, preset schema, SoundBank, or production UI source changed. No implementation or test pass is claimed.

**Status:** SPECIFICATION UPDATED; UI IMPLEMENTATION AND EXACT-SHA VALIDATION PENDING.



## 2026-11-12 — REV.17 GUI control authority / mockup reconciliation

**Change class:** specification + journal only; no production source, tests, build files, assets or binaries changed.  
**Branch:** `work/visual-shell-spec-rev17-1`.  
**Parent HEAD before CODE update:** `114c5f4ffec36575be6787913d05a763eda8cae7`.  
**INFO journal commit:** `114c5f4ffec36575be6787913d05a763eda8cae7`.  
**Specification commits:** master spec `060a21d2a29c70b689a5e138a910712c92622175`; Control Inventory `8f85573b85623fbac6ff20a9f08d2178c7389e7e`; Theme Visual Identity `ce38a33e5cf104d28a2d95a81d599fada7afbf25`.

**Mandatory rule:** approved development journal + master specification + Control Inventory define the functional GUI contract. The mockup defines visual treatment/placement of those controls, not additional functions. A mockup-only element not required by the approved contract may be removed or replaced with a required control. A required control missing from the mockup/editor must be added to its specified zone and connected to a real component/attachment/action with genuine state and feedback.

**Safety boundary:** removal/replacement is GUI-only. Do not remove working engine/DSP/sampler/MIDI/APVTS functionality, change parameter IDs/semantics, or alter preset/SoundBank data. Do not create fake controls. Unimplemented/unverified functionality must be marked DISABLED / DEFERRED / NOT IMPLEMENTED. Resolve ambiguous conflicts in INFO/CODE before removal. Applies to all five skins.

**Verification:** documentation readback required; no source implementation or runtime test is claimed. UI component parity, exact visual match, build, CTest, pluginval and FL Studio 20 host verification remain pending.

**Status:** REQUIREMENT RECORDED; IMPLEMENTATION/VALIDATION PENDING.


## 2026-11-12 — Skin expansion 53-102 automation and default-skin contract repair

**Type:** test correction + CI workflow; generated theme payload not yet present.  
**Branch:** `work/ppgu-skin-expansion-53-102`  
**Base:** `work/visual-shell-spec-rev17-1` at `9042cd1ccc325f4aca130fa917d310df9dd965f0`.  
**Commits:** `2f485a358e9d99efd26a0276a81701b332535d18`, `58cd62a2b3444ad22c6d5d4f91751c55a603fac5`.  
**Journal follow-up:** `Info.txt` update commit `e3a60d0047afe6d5ef8306c2889e2b18a78bbcd3`.

**Files / symbols:**
- `tests/ValidateSkinManifests.py` — `DEFAULT_ID` remains the single expected default; stale literal assertion `skin-51` replaced by a message derived from `DEFAULT_ID`, matching canonical `skin-052`.
- `.github/workflows/ppgu-skin-expansion.yml` — new `generate-validate-merge` workflow for Skin Studio indices 53–102. Runs preflight, `tools/generate_skin_batch.py`, `tools/validate_skin_pack.py`, `tools/merge_skin_pack.py`, and `tests/ValidateSkinManifests.py`; verifies 102 registry entries and preserves Skin_052 as default; commits generated payload only after all gates pass.

**Before/after:** the existing test incorrectly rejected the registry's documented `defaultSkin: skin-052`. It now checks the canonical default constant. The next 50-theme batch has a reproducible CI path instead of being manually represented as completed.

**Invariants:** no production C++/JUCE editor, ThemeManager, APVTS, audio/DSP, sampler, MIDI, preset, or existing skin data changed in these commits. No generated `Skin_053`–`Skin_102` folders are claimed at this point.

**Tests / CI:** workflow file and commits confirmed by GitHub readback. No Actions run ID, job logs, G1–G4 result, merge result, generated asset commit, Windows build, pluginval or FL Studio host result available at journal time. Status: **PIPELINE COMMITTED; EXECUTION/VALIDATION PENDING**.

**Next:** inspect the workflow run and generated commit; verify 102 manifests, exact folder parity, PNG dimensions/CRC, safe SVG, contrast and control parity. Then implement/verify the runtime Theme Gallery and transition/ECO/DPI gates.


## 2026-11-12 — Canonical Skin_052 fallback alignment

**Тип:** UI/theme production code + regression test.
**Ветка:** `work/ppgu-skin-expansion-53-102`.
**Commits:** `1b112ae2eeec837af4493f54dd61406433162a2b`; `b006b78add4250daa0cd83a41f5b50df6730a981`.
**Файлы:**
- `Source/UI/ThemeManager.h` — `PPGUThemeManager::Palette`: default `id/name`, 21 token defaults, legacy color aliases.
- `tests/ThemeManagerTests.cpp` — canonical fallback ID/name assertions; legacy Skin_51 fixture label.

**Причина:** fallback identity was stale (`skin-51` / Blackout Voltage) while the canonical registry default is `skin-052` / SIGNAL FORGE.
**Изменения:** fallback values now match the exact Skin_052 palette from `SKIN/Skin_052/theme.json`; tests assert the canonical fallback identity.
**Совместимость / non-goals:** visual-only; no APVTS IDs/semantics, processor, DSP, sampler, MIDI, preset or audio-thread changes.
**Тесты:** diff and GitHub readback inspected. C++ build/CTest/pluginval/FL Studio not run on these commits. **Статус:** IMPLEMENTED / VALIDATION PENDING.
**Следующий шаг:** restore reliable workflow execution for Skin_053–102; then add visual gallery and runtime asset previews after validating generated pack.


## 2026-11-12 — SIGNAL FORGE fallback + multi-window overflow grid

**Type:** production UI fixes, tests, CI reliability.
**Branch:** `work/ppgu-skin-expansion-53-102`.
**Implementation chain:**
- `e0d0b4f5f996d188dddf6533f50a072cb23d33dd` — `Source/UI/ThemeManager.h`: `Palette::Tokens::controlFace` corrected from `#1D2930` to canonical `#26343A`; fallback comment now names SIGNAL FORGE.
- `c62b1f39befffd1b4b7248915828a3e1ff0eb227` — `Source/UI/AuxiliaryPanelLayout.cpp`: overflow placements become a minimum-size scroll grid with all panels visible.
- `6bbae523d1f28905699636d8d1984776edd37d84` — `Source/UI/AuxiliaryPanelLayout.h`: overflow contract documents all-visible scroll grid.
- `9d71f922d25478d8fb7a6fb335fb452011b972c0` — `tests/AuxiliaryPanelLayoutTests.cpp`: tests 1/2-column overflow visibility, non-overlap and impossible geometry.
- `291f49b98d5eadb2236cc364275c93aa8b1de524` — `Source/UI/PPGUMultiWindowWorkspace.cpp`: maps overflow placements to JUCE child windows and sizes the scroll surface.
- `60eddf7420df03908bca177246db6b7b312ccb73` — `tests/ThemeManagerTests.cpp`: asserts all 21 default fallback tokens.
- `da62bd8db1d91de33aebba499f069a2af8fc9a3d` — `.github/workflows/ppgu-skin-expansion.yml`: rebase/retry generated pack commit to avoid push rejection when branch advances during generation.

**Defect / reason:** fallback `controlFace` diverged from Skin_052 manifest. Overflow layout previously marked only one panel visible and workspace did not consume grid placements in overflow mode.

**Behavior:** fallback now matches canonical Skin_052's 21 semantic tokens. If viewport cannot fit all panels, panels are tiled at minimum size on a scrollable one- or two-column surface; no panel is silently hidden by the layout contract.

**Compatibility:** no DSP, processBlock, audio callback, sampler, MIDI, APVTS IDs/semantics, presets or SoundBank changes.

**Tests:** test source updated and GitHub readback checked. Exact-SHA C++/Windows build and CTest are pending. Skin generator run `43194302212` passed generation, G1–G4 validation, merge and final 102 registry verification, but generated commit push failed with non-fast-forward because the branch advanced concurrently. Workflow retry fix commit `da62bd8...`; run `43195205929` was IN_PROGRESS at journal time. No Windows build/pluginval/FL Studio PASS claimed.

**Status:** SOURCE CHANGES COMMITTED; EXACT-SHA VALIDATION PENDING; 50-SKIN PUBLICATION RETRY RUNNING.


## 2026-11-12 — Searchable Premium Skin Gallery and Skin_053–102 pack

**Type:** production GUI feature + generated visual theme assets.
**Branch:** `work/ppgu-skin-expansion-53-102`.
**Source commits:**
- `9054f92b4bd5b559a67eb68ed00a72dcd6f23b96` — `Source/PluginEditor.cpp`: new `PPGUSkinGalleryPanel`, card grid, preview images, search, family filter, active theme marker; `openSkinGalleryPanel()` builds the gallery from validated runtime manifests.
- `065279795c421a9b01e10e288faa1a8bb0eb2a1d` — gallery card hover styling, active badge cleanup and stable theme count after moving the card collection.
- `bc5f753b7bbc1f76c4c8c5cfc578e4b356c1181b` — `Source/UI/PPGUMultiWindowWorkspace.cpp`: saturating scroll-surface extent arithmetic.
- `7b56be7f202edd07c46b3ba8dbf7a64718d9f07a` — `Source/PluginEditor.h`: gallery entry point declaration.
**Generated data commit:** `3a2ceb52062be112429f96d411ae223d87df17a6`.
**CI:** skin expansion run `43195205929` SUCCESS.

**Files / symbols:** `PPGUSkinGalleryPanel::{Skin,Card,Grid,rebuildCards,resized}`; `PopPunkGuitarUltimateAudioProcessorEditor::openSkinGalleryPanel`; `getSkinCount`, `getSkinManifest`, `loadSkinByIndex`; `PPGUMultiWindowWorkspace::layoutPanels`.

**Behavior:** SKIN button and Ctrl+K open a child gallery panel in the approved multi-window workspace. Cards are generated from the registry/manifests and expose theme name, family/layout, preview, active status and click-to-apply. Search and family filters operate on the 102-item registry. Preview decoding is deferred to card paint. Applying a theme uses the existing ThemeManager and preference persistence path, then closes the gallery.

**Skin pack verification:** generated commit readback confirms 102 registry entries, canonical default `skin-052`, pack state `MERGED`, indices 53–102, merged total 102. For each of 50 new skins, theme manifest + preview PNG + hero PNG + bezel/crest/texture SVG paths were present (0 missing). Workflow run `43195205929` completed successfully through generate, G1–G4, merge, manifest validation, final registry assertions and push.

**Compatibility:** no processor, DSP, audio callback, sampler, MIDI, APVTS IDs/semantics, preset or SoundBank changes.

**Tests:** gallery and arithmetic source readback checked. Windows VST3 workflow `43195592441` was IN_PROGRESS on code SHA `bc5f753b7bbc1f76c4c8c5cfc578e4b356c1181b`; no build/CTest/pluginval/host PASS claimed at this record. FL Studio 20 visual acceptance remains pending.

**Status:** 102-THEME PACK PUBLISHED; GALLERY SOURCE IMPLEMENTED; EXACT-SHA BUILD / TEST / HOST VALIDATION PENDING.


## 2026-11-12 — Persistent auxiliary Workspace Settings panel

**Type:** production UI feature.
**Branch:** `work/ppgu-skin-expansion-53-102`.
**Commits:**
- `359f4269f5fe56b24384af89ddc930b6b77fc04a` — `Source/PluginEditor.h`: declares `openSettingsPanel()` and `settingsButton`.
- `19e3f9f3d61f19582e18cdab7ae0ba6446607d46` — `Source/PluginEditor.cpp`: adds `PPGUSettingsPanel`, settings top-bar action, persisted workspace preferences and reset/open-gallery actions.
- Documentation updates: `546d11b80947307b7aff1636bfb2e979fce714a7`, `ad9d04c5d79147d75e019cb53cc8c0892d1101f0`, `591e2251b66f94add4d94bae6c2e3a0b97f82f2c`, `ebf9f7965c80053ca1a1b7e1033889cbe5de6d3a`.

**Symbols:** `PPGUSettingsPanel`; `PopPunkGuitarUltimateAudioProcessorEditor::openSettingsPanel`; `settingsButton`; `PPGUMultiWindowWorkspace::setPanelScale`, `setPanelGap`.

**Behavior:** settings panel ID 6 exposes real controls for auxiliary panel scale (0.50–1.50) and spacing (0–32 px), with persisted values `workspacePanelScale` and `workspacePanelGap`. It can restore canonical SIGNAL FORGE (Skin_052) or open the theme gallery. The panel is hosted by the existing multi-window workspace and uses safe editor callbacks.

**Compatibility:** UI/workspace only. No APVTS IDs/semantics, processor, DSP, audio callback, sampler, MIDI, preset or SoundBank changes. No global engine/device settings or About panel claimed.

**Tests:** source/readback inspected. Windows VST3 workflow `43196006789` was IN_PROGRESS on SHA `591e2251...`, including settings source commit `19e3f9...`; exact-SHA build/CTest/pluginval and FL Studio host result pending. `ebf9...` is workspace documentation only.

**Status:** SETTINGS PANEL SOURCE IMPLEMENTED; VALIDATION PENDING.


## 2026-11-12 — Mixed-size auxiliary panel layout and fallback alias parity

**Type:** production UI geometry + regression tests.
**Branch:** `work/ppgu-skin-expansion-53-102`.
**Commits:**
- `c94bffc7bef6c4d1f9b4309b5e79f7a4827ce9f7` — `Source/UI/AuxiliaryPanelLayout.cpp`: independent per-column desired/minimum widths and per-row desired/minimum heights; binary-search compact scale; deterministic 1/2-column fit; scroll-grid overflow fallback.
- `0d66dd171f42441fc4e955eda96aa06101095f72` — `tests/AuxiliaryPanelLayoutTests.cpp`: six heterogeneous panels fit in a 1092×812 workspace without overflow/overlap and each panel retains its own minimum dimensions.
- `58d0c52e37927723571c2a287ad6ad17c01b36db` — `Source/PluginEditor.cpp`: gallery/settings min heights changed to 500×320 and 420×260.
- `f7583c7bcfd1533530f0677751c64031da57fb4e` — Settings panel compact geometry.
- `5631d680e225fe2cd7e20331a8fe9f4319e7f27c` — overflow candidate selection prefers width-fit and rejects unrepresentable geometry.
- `bc40e2955658986e4430d0a27f725a9111758271` — gallery preview/cards compacted for simultaneous 2×3 display.
- `4b012497cde0cfffc77e16d107f8d6a2ebbb9267` — `Source/UI/ThemeManager.h`: fallback `Palette::focus` now equals canonical Skin_052 `highlight`.
- `8b6089c8196cf1e2ec293daa0e39c018def9094d` — `tests/ThemeManagerTests.cpp`: focus/highlight alias parity assertion.
- `b1e1054bd6dd0020074b440e97b95ae58189d08e` — `SKIN/theme_manifest.json`: registry status reflects 102 merged themes and gallery source implementation; validation still pending.
- `68b0fa29443855f2e126c2e0511d014ffbbdb37f` — `Source/UI/PPGUMultiWindowWorkspace.cpp`: reset viewport scroll to origin after opening a panel.

**Reason:** global maximum panel minimums unnecessarily constrained every child window and prevented the approved five/six-panel workspace from shrinking efficiently. The fallback compatibility alias also diverged from the loaded Skin_052 mapping.

**Behavior:** rows and columns now size independently based on the panels placed in them. The 2×3 test case covers FX/AutoChords/Import + Gallery + Settings and a sixth panel in the default editor workspace. If the full set cannot fit, every panel remains reachable on a minimum-size scroll grid; width-fit is preferred to avoid needless horizontal scrolling. New panels open at the top of the viewport. Gallery and Settings compact layouts fit their declared minimum heights.

**Compatibility:** no audio, processor, APVTS, MIDI, sampler, preset or SoundBank changes.

**Tests:** source/readback inspected; new pure geometry and fallback alias assertions added. Windows workflow `43196750439` was PENDING on source commit `68b0fa...`; exact-SHA build/CTest/pluginval and FL Studio host acceptance pending. Skin generation/validation run `43195205929` SUCCESS.

**Status:** SOURCE IMPLEMENTED; TESTS ADDED; EXACT-SHA CI PENDING.


## 2026-11-12 — Live ThemeManager palette propagation for workspace children

**Type:** UI architecture / runtime theming.
**Branch:** `work/ppgu-skin-expansion-53-102`.
**Commits:**
- `81aecb1620124d80cfdd62304b73471c84f43f5e` — `Source/UI/PPGUMultiWindowWorkspace.h`: introduces `PPGUThemeAwarePanel` contract and workspace `setThemePalette`.
- `fbe79dfdd6c387cf8d4462930e2f4ba4f9bf2c06` — `Source/UI/PPGUMultiWindowWorkspace.cpp`: stores active palette, updates open panel content and applies palette to newly opened panels via interface dispatch.
- `21137e2cb81198bb0ef773c2936d14417f60097e` — `Source/PluginEditor.cpp`: FX EDIT, AutoChords, Import Guitar, Settings and Skin Gallery implement `PPGUThemeAwarePanel`; token colors applied to their labels, buttons, combos, sliders, graph surfaces, status and cards. `applyPalette()` propagates the new palette through workspace.
- `ff027621afee2f5cfb551ca5423eb8fb332cae52` — gallery active card state tracks the live palette ID and APPLY badge is compact.

**Behavior:** theme changes recolour all open auxiliary panels; panels opened later inherit the current theme automatically. No panel needs to be closed/reopened to update its visual tokens.

**Compatibility:** no processor/DSP/audio callback/APVTS/MIDI/sampler/preset/SoundBank changes. Main editor still has remaining hardcoded decorative colors; full token-only parity is not claimed.

**Tests:** source/readback inspected. Windows VST3 build, CTest and pluginval still pending on the latest branch head after this source chain. Skin expansion workflow `43195205929` SUCCESS.

**Status:** LIVE AUXILIARY THEME PROPAGATION SOURCE IMPLEMENTED; EXACT-SHA VALIDATION PENDING.


## 2026-11-12 — Canonical REV.17 branch

`POP-PUNK-GUITAR-ULTIMATE-VST-REV.17` is now the canonical work branch, created from `e5000be105fa059d5f903f6efc17f530ccfee412`. The previous `work/ppgu-skin-expansion-53-102` branch remains as historical source lineage. All further code changes and fixes must be committed to REV.17 only.


## 2026-11-12 — Fast artwork generation and REV.17 skin workflow

**Type:** build-time performance / CI branch migration.
**Branch:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV.17`.
**Commits:**
- `86f88acd2091f122d6ab8aba03a81aaa645d9927` — `tools/T2_generate_missing_artwork.py`: Pillow native gradient resize and blurred mask replace Python per-pixel rendering.
- `0dc6cde8441896e05ac9115831dc7679b5995965` — radial concentric bloom mask restores the intended soft accent glow.
- `9efe5e01efb227b303811e9213df99ebc4afa18b` — skin expansion workflow branch, checkout and push target migrated to REV.17.
- `962e0992ba68012b1ae518f9b11080fc72980d37` — correct GITHUB_OUTPUT line emission.
- `7da4593d6906e29a22044cd553689efacbbaa490` — idempotent 102-theme preflight; validation still runs when pack already merged.
- `1e21cd712279254ee306f5a253745fd61e75b5b2`, `6a51c592c2b08111f8fe0492e25b7eb8efe11ba2`, `00442768e786d727a7a7627be7afa8c3ce206e52` — branch references in skin docs updated.

**Behavior:** legacy preview/hero generation no longer loops over every pixel in Python. Skin workflow no longer targets the historical work branch or assumes only 52 skins; it validates the existing merged 102-theme registry without regenerating it.

**CI:** REV.17 skin workflow run `43198014508` SUCCESS. Windows VST3 workflow `43198145160` was IN_PROGRESS on source SHA `0dc6cde...`; final C++ build/CTest/pluginval/package pending.

**Status:** artwork generator and skin CI source committed; exact-SHA Windows validation pending.

## 2026-11-13 — Восстановление дерева из FULL WORK TEXT + финальная упаковка REV.17

**Тип:** recovery + packaging. Production source не переписывался; вычищены артефакты текстового экспорта.
**Ветка:** POP-PUNK-GUITAR-ULTIMATE-VST-REV-17.
**Файлы:** все 171 файл дерева (вычистка разделителей/отступов), SKIN/Skin_001..052/assets/bg.png (перегенерированы реальные PNG), Resources/TestData/TestProbe.wav (реальный PCM WAV), Info.txt, CODE.txt, README.md.
**Тесты:** 7 валидаторов G0-G6 — прогон на восстановленном дереве (результаты в Info.txt 6.9).
**Статус:** IMPLEMENTED / VALIDATION PENDING.

## 2026-11-14 — TX-UI-PRESET-GUARD-004 documentation consolidation

**Type:** documentation-only requirements consolidation; no production source change.  
**Base main HEAD:** `cab7dea6609fd02d41725f0c0a643a5860ae146a`.  
**Canonical requirements record:** `Info.txt` entry dated 2026-11-14; detailed product requirements appended to `docs/MASTER_PRODUCT_SPEC_4_0_RU.md`.

**Consolidated scope:**
- Preset browser hierarchy: GENRE → ARTIST/BAND → SONG/SECTION → STYLE → PATTERN.
- Sections: Intro, Verse, Pre-Chorus, Chorus, Bridge, Outro; eight Pattern Slots per section.
- Browser operations: BROWSER, FAVORITES, CATEGORIES, SEARCH, FILTER, SORT, PREV/NEXT, FAVORITE, PRESET NAME/CATEGORY, PREVIEW, SELECT. PREV/NEXT preserve the active search/filter context.
- Preview is non-destructive; Apply commits; Cancel rolls back. Preview must not change the active preset/project or persistent DAW state.
- Factory catalog is extension-only; stable IDs and existing entries are immutable; import conflicts require explicit handling; no silent overwrite.
- Latest formats: .ppgm MELODY, .ppgp PATTERN, .ppgbank bank. Eight QA profiles are coverage material, not eight automatic factory presets.
- Latest visual guard: SIGNAL FORGE / NEON, default NEON STAGE; first reference file = layout/appearance/placement, second = palette; skins change graphics only and preserve exact control geometry, placement, bindings, functions and audio.
- No decorative/fake controls; no engine-function deletion or changes to APVTS IDs/semantics, DSP, MIDI or sampler.
- PATTERN/AutoChords section/slot workflow, legacy patternSlot compatibility, host-synced transitions, recorded-articulation availability and AutoChords keyboard non-overlap are preserved as requirements.

**Validation status:** DOCUMENTATION CONSOLIDATED ONLY. No source files changed; no build, CTest, pluginval, package or FL Studio 20 acceptance is claimed by this entry. Implementation status remains whatever the exact source/CI evidence establishes.



## 2026-11-14 — Documentation change index: MAJOR user-approved PPGU requirements amendment

**Type:** documentation-only index entry. **Production source changed:** no.  
**Canonical requirements:** `docs/PPGU_MAJOR_USER_REQUIREMENTS_AMENDMENT_2026-11-14_RU.md`.  
**Updated specification:** `docs/MASTER_PRODUCT_SPEC_4_0_RU.md`.  
**Updated engineering journal:** `Info.txt` (single authoritative status/decision journal).  
**Scope:** FX EDIT up to 15 implemented processors plus cabinets; DSP minimum 10/target 15; full AUTO DISCOVER; NEON STAGE DEFAULT as default skin with mockup 1 as exact layout reference and mockup 2 as palette reference; graphics-only derivative skins and sliced PNG assets; open-ended 300+ factory presets mapped to the canonical PUNK GROUP list; first preset category Pop Punk Essentials; commercial libraries unrestricted for lawful user-local use, with licensing clearance applying to PPGU redistribution/factory inclusion only.  
**Compatibility:** existing APVTS IDs/semantics, MIDI, sampler, realtime callback, secure import and release invariants preserved. Earlier I7/R18 default display-name rule explicitly amended only for the default skin name; stable ID `default-ppgu` retained pending reviewed migration.  
**Validation:** no production source changed; no build, CTest, pluginval, package or FL Studio 20 host test run.  
**Status:** REQUIREMENTS APPROVED / IMPLEMENTATION NOT IMPLIED.


## 2026-11-14 — MASTER ENGINE SOURCE INVENTORY / BUILD GRAPH AUDIT

Type: documentation-only source inventory and static integration audit.
Audited main HEAD: 1f09611f753702e380d695a5441b6f260f798d3b.
Production source changed: NO.
Matching detailed INFO record: Info.txt, “MASTER ENGINE / PLUGIN SOURCE INVENTORY, CODE-OF-RECORD AND REPOSITORY INTEGRATION AUDIT”.

Canonical code locations recorded:
- JUCE processor/editor: Source/PluginProcessor.{h,cpp}, Source/PluginEditor.{h,cpp}; parallel root PluginProcessor.* / PluginEditor.* remain to be compared.
- Sampler: Source/Sampler/{SamplePool,SampleMetadataIndex,SampleStreamingService,PageCacheSlots,StreamingVoiceReader,WavPageReader,SfzParser}.{h,cpp} plus BoundedSpscQueue.h; parallel root sampler files remain.
- Performance: Source/Performance/{PatternScheduler,AutoChordEngine,ChordTransitionPolicy,ChordTransitionTimeline,CircleOfFifths}; parallel root PatternScheduler.* / ChordEngine.* remain.
- UI: Source/UI/{AutoChordsPanel,AuxiliaryPanelLayout,AuxiliaryPanelWindowManager,EditorOverlayPanel,FXEditPanel,ImportGuitarPanel,PerformanceDisplay,SettingsPanel,ThemeManager}.{h,cpp}; parallel root panel/window/theme/security files remain.
- Other root modules: FxChain.*, ImportPipeline.*, PresetRuntime.*, WavIndex.*, PageCache.*, SecureSkinImport.*, OverlayManager.*, MultiWindowWorkspace.*, AuxiliaryPanelLayout.*.
- Tests, tools, presets, SoundBank policy, skins/assets and CI paths are inventoried in the matching INFO appendix.

Static integration discrepancies found: CMake references missing Source/Engine/* files, missing Source/UI/MultiWindowWorkspace.cpp and Source/UI/Panels.cpp, and tests/EngineContractTests.cpp while corresponding/parallel files are at other paths. CMake also calls add_subdirectory(JUCE), but the audited tree has no JUCE directory/gitlink and the inspected workflow contains no submodule/JUCE clone step. Source/UI/FXEditPanel.h exposes eight module slots versus approved scope up to 15 fully implemented FX processors plus cabinets. The workflow's legacy factory_bank_300.json validation must accept an open-ended catalog; 300 is not a maximum.

Important: static path audit only; no Windows configure/build/CTest/pluginval was run. No actual build failure is asserted. Do not move/rename duplicate code before API/namespace/APVTS/MIDI/lifecycle/test/history comparison. Production implementation and test status remain unchanged.

Next: canonical source graph map → CMake/JUCE provisioning reconciliation → test path reconciliation → end-to-end processor/sampler/pattern/chord/FX integration tests → exact-SHA Windows Release/CTest/pluginval/FL Studio 20 validation.


## 2026-11-14 — SOUND LIBRARY SCOPE + AUTONOMOUS DEFECT REMEDIATION POLICY (APPROVED)

- New canonical requirements: `docs/PPGU_GUITAR_SOUND_LIBRARY_MASTER_PLAN_RU.md`.
- Scope: unlimited guitar One-Shots, Sample Loops, multisamples/performance sets, SoundBank assets and all MELODY/PATTERN/browser preset/style/articulation/technique/rhythm/variation categories.
- Entity separation: One-Shot ≠ Loop ≠ Multisample ≠ MIDI Pattern ≠ Preset. Electric/acoustic and factory/user assets stay correctly typed and separated.
- No arbitrary global or per-category caps. Stable IDs, metadata manifests, lazy loading, bounded cache, background decoding, exact hashes, source/license evidence and quality/mapping validators required.
- Factory bundling requires explicit redistribution rights. Local user import of commercial libraries is not prohibited solely because factory redistribution is not allowed.
- User approved automatic detection and repair during each development item for clear, reversible, non-breaking defects, followed by targeted/regression tests and evidence logging in INFO/CODE. No repetitive per-defect notifications; report at milestones or on request.
- Autonomy stops only for decisions that alter approved behavior/contracts, conflicting approvals, uncertain rights/provenance, irreversible actions or user-data risk. Pause affected scope, ask one precise decision; continue independent safe work. Never fake PASS; unrun tests are NOT RUN.
- Baseline registry audit: assets/sound_hunter/registry.json has assetCount=0 and sourceCount=0. This is a bootstrap marker; no populated/FACTORY-READY library is claimed.
- Documentation only in this commit; no audio assets or production code added. Status: APPROVED / DOCUMENTED / VERIFIED after refetch; not IMPLEMENTED / TESTED / FACTORY-READY.


## 2026-11-14 — STRICT 25–30 GB LIBRARY TARGET + REAL-TIME INFO/CODE SYNCHRONIZATION

**Status:** USER-APPROVED / DOCUMENTATION ENTRY. Production source and audio assets changed: NO.

- Required installed factory Guitar Sound Library size: **25–30 GB inclusive** of actual guitar audio assets (One-Shots, Sample Loops, multisamples/performance sets and approved guitar SoundBank content).
- This is a strict content-size target, not a preset/file-count cap. No padding, empty files, duplicates, artificial re-encoding, placeholders or uncleared assets may be used to reach it.
- Measure actual bytes at staging, SoundBank assembly and final package stages. Report category totals, factory/user split and package overhead separately. The installed factory library itself must fall within 25–30 GB; compressed installer size may differ.
- If verified content exceeds 30 GB, stop packaging and report a safe optimization proposal; do not silently discard approved content or reduce quality. If below 25 GB, continue lawful, quality-controlled sourcing; do not fabricate assets.
- Existing registry baseline is empty (assetCount=0/sourceCount=0); therefore target is APPROVED/DOCUMENTED only and is not yet size-verified.
- Journal organization: keep all development journals in one canonical repository location after dependency/reference audit. INFO and CODE remain separate synchronized records. INFO tracks requirements/status/decisions/verification; CODE tracks source paths/symbols/change summaries/commit SHAs/test evidence. Full code remains in .cpp/.h; never duplicate source code in logs.
- Update INFO and CODE during each development operation, with matching commit SHA, affected paths, test commands/results and unresolved risks. Do not mark documentation complete until both records have been re-fetched and verified. Avoid blind moves/deletions; preserve history and references.


## 2026-11-14 — JOURNAL ARCHIVE CONSOLIDATION / CODE-OF-RECORD UPDATE

Documentation-only operation; production source and audio assets changed: NO.
Canonical journal directory: docs/PPGU_JOURNAL/.
Copied journal/specification set: INFO.md, CODE.md, PPGU_GUITAR_SOUND_LIBRARY_MASTER_PLAN_RU.md, PPGU_MASTER_PROMPT_RU.md, PPGU_MAJOR_USER_REQUIREMENTS_AMENDMENT_2026-11-14_RU.md, PPGU_AUDIO_ENGINE_PRODUCT_FAMILY_RU.md, MASTER_PRODUCT_SPEC_4_0_RU.md, PRESET_BANK_4_0_READINESS_RU.md, SAMPLE_LIBRARY_SOURCING_AND_LICENSE_POLICY.md, PPGU_CANONICAL_PUNK_GROUP_PRESET_SOUND_MAP_RU.md, plus README.md index.
All ten copied content files were re-fetched from main and their content lengths/blob SHAs recorded during verification; copies preserve exact source blob content at time of consolidation.
Future journal edits go to this canonical folder. INFO.md and CODE.md must be updated together with matching operation/commit evidence. Source implementation remains exclusively in .cpp/.h.
On exact user request ALL, create a fresh ZIP from current main contents of docs/PPGU_JOURNAL/ and attach it.
Legacy original paths were intentionally retained; no deletion/move was performed because repository-wide references and workflow consumers have not yet been exhaustively audited. No build/CTest/pluginval/host test was run; no production source changed.



## 2026-11-14 — JOURNAL ARCHITECTURE: SINGLE NON-CODE JOURNAL

**Type:** documentation governance update; production source unchanged.
**Canonical non-code journal:** `docs/PPGU_JOURNAL/PPGU_JOURNAL.md`.
**Separate code journal:** `docs/PPGU_JOURNAL/CODE.md` (this file).
**Migration:** INFO history and the complete PRESET_BANK_4_0_READINESS history were consolidated into PPGU_JOURNAL.md. Legacy INFO and preset-readiness paths are retained as archival pointers for link compatibility.
**Rule:** append all non-code project events to PPGU_JOURNAL.md as work is performed; record source changes here with exact SHA, paths/symbols and test evidence. No additional active journals.
**Initial consolidation commit:** 2dcbbc49acc112abdb184a72ac505b6608e9db55.
**Build/tests:** NOT RUN; documentation-only change.


## 2026-11-14 — CMAKE / BUILD GRAPH RECONCILIATION

**Build/config commits:** 
- `bee7f9a07c6fb505780da81097b98fe47a625575` — replaced stale `Source/Engine/*` and absent resource paths with canonical `Source/Sampler/*` and `Source/UI/*`; added JUCE 7.0.12 FetchContent pin; established sampler-core library and actual existing test paths.
- `8b07c9cf3485f990bc1653b031e3b611ba74de04` — satisfied binary-data contract with `PPGUPresetData` / `PPGUTestData`, unique generated headers, renamed plugin target to `PopPunkGuitarUltimate` as required by validator, and added `ppgu_processor_pattern_tests` using the real Processor/Editor source path.

**Paths:** `CMakeLists.txt`; source paths included by the CMake graph: `Source/PluginProcessor.cpp`, `Source/PluginEditor.cpp`, `Source/Sampler/*.cpp`, `Source/UI/*.cpp`, `Source/Performance/*.h`; tests under `tests/`; root standalone security/overlay test sources; embedded data from `presets/factory_bank_300.json` and `tests/assets/ppgu_test_data.txt`.

**Symbols/targets:** `ppgu_sampler_core`, `PopPunkGuitarUltimate`, `PPGUPresetData`, `PPGUTestData`, `ppgu_processor_pattern_tests`, and independent CTest contract executables.

**Reason/root cause:** existing root CMake referenced paths not present on main, expected an unvendored JUCE folder without provisioning, and violated the repository's BinaryData target/header/link contract. Prior CI baseline run `43561654587` on SHA `27db171bd62828fccf479982dd402d20a9717830` failed at the BinaryData checker with missing `PPGUPresetData` / `PPGUTestData` and missing HEADER_NAME declarations.

**Verification:** GitHub Contents API confirmed the referenced source/test paths exist. Windows run `43561929542` on SHA `8b07c9cf3485f990bc1653b031e3b611ba74de04` was QUEUED when this entry was prepared. Configure/build/CTest/pluginval/host results are NOT RUN / NOT VERIFIED; no PASS is claimed. Production C++ implementation and audio assets were not changed.


## 2026-11-14 — CMAKE FOLLOW-UP AND WINDOWS CI EVIDENCE

**Build/config commits (all in CMakeLists.txt):**
- `bee7f9a07c6fb505780da81097b98fe47a625575`: replace absent legacy Source/Engine and Resources paths with existing canonical sampler/UI sources; pin JUCE 7.0.12 via FetchContent; establish sampler core and valid source/test paths.
- `8b07c9cf3485f990bc1653b031e3b611ba74de04`: declare `PPGUPresetData` / `PPGUTestData` with required generated headers and link consumers; add real Processor/Editor integration test.
- `d69fa83ae68a096a5595e738d6af8ce1f0a38d8f`: use JUCE console app for processor integration target, generate its JUCE header, set valid `BUNDLE_ID`.
- `428cefd926bbd4a6aaad74e98a38083a6d74de76`: use JUCE console app + generated JUCE header for `ppgu_theme_contracts` and `ppgu_panel_contracts`.

**Verified CI evidence:**
- Run `43561967979`, SHA `ae96569f151c82a60f2864e291cd77f585b72fe8`: CMake configure FAILED because JUCE header generation was called on a non-JUCE executable. Fixed in `d69fa83...`.
- Run `43562370660`, SHA `b7633e5c0547947c7b13402bc92de0d7d905f25a`: configure PASS; build FAILED because `ppgu_theme_contracts` lacked generated `JuceHeader.h`. Other sampler/integration targets compiled in the same build log. Fixed in `428cefd...`.
- Run `43563213274`, SHA `428cefd...`: configure PASS; build CANCELLED by later push. No final build result.
- Run `43563855541`, SHA `ca1839f...`: cancelled during build by later push. No final build result.
- CTest, pluginval strictness 5, FL Studio 20 host tests: NOT RUN on the corrected CMake configuration.

**Additional audit results:** SoundBank audit reports 3.306 GiB, 4,353 audio assets / 11 sources; this passes the current validator but fails the user's strict 25–30 GB minimum target. AUTO DISCOVER is not present in the current PluginEditor; Import Guitar only exposes manual SFZ browsing. FX compact inventory remains below the approved 15-processor upper scope plus separate cabinets; exact remaining module names must be recovered from the user's earlier approved list, never invented.

**Next:** obtain one uninterrupted Windows Release configure/build/CTest run on the latest stable HEAD; then continue feature implementation. This entry documents observed outcomes only; it does not claim product readiness.


## 2026-11-14 — SOUND HUNTER DOWNLOAD FAILURE FIX / REGRESSION COVERAGE

**Source change:** `SoundHunter/sound_hunter_real_guitars.py`.
**Source commit:** `6adca0b36cf394bb76dd45906158f2e8c3fbedd8`.
**Test commit:** `54f4aeeff68ae68e892ea89265f4427fb551560f`.
**Workflow commit:** `50202631ff427bcaef356f3b164c2155b2de5c61`.

**Exact changes:**
- Added `release_asset_target()`; files from Meta release ZIPs now go under a distinct `release_assets/` subtree, separate from the source repository's own `release/` directory. The helper rejects unsafe archive paths.
- Added `within_run_limit()` and changed the Sound Hunter byte counter to start at zero for each run. Only bytes newly imported by that run consume the 25 GB Sound Hunter cap. Existing/manual files in `Samples` remain eligible for SHA-256 deduplication but no longer consume the current run's cap.
- Added `tests/test_sound_hunter_importer.py` with regression tests for the release path collision, path traversal rejection, and run-local size-cap accounting.
- Added a GitHub Actions step to execute these tests before any network downloads.

**Post-change verification:** GitHub Contents API re-fetched all three changed files from `main`; source blob SHA `636763590f2964088eea1897c1a69f7f982892f5`, test blob SHA `12c8fa138417907fe3f975037682607dee42e090`, workflow blob SHA `cf7b11fecdf7fbaabfe8df031fc91d71a18e7940`. HEAD at verification: `50202631ff427bcaef356f3b164c2155b2de5c61`.

**Tests:** regression tests are committed and wired into the workflow but have NOT YET BEEN EXECUTED on GitHub Actions. A successful download, artifact, sample count, or size is NOT claimed. The user must start a fresh workflow run on the corrected HEAD; the previous failed run cannot validate these changes.

**Safety:** no pre-existing sample library files were overwritten, moved, or deleted; no source samples or VST3 DSP code were modified. This is a focused Sound Hunter bug fix only.


## 2026-11-14 — Factory Preset Bank 500: catalog/build gate

**Commits:** catalog `79d4e2ab0d382b84bb862ee3e03370c0a67129e3`; integration `99577e84744c1de13bdd8c7dd8db84cf79fc7829`.

**Changed paths:**
- `presets/factory_bank_500.json` — 500 definitions, 250 MELODY + 250 PATTERN; compact tuple-v1 16-step pattern events; artist-specific reference-track cleanup; genre/style and articulation metadata.
- `CMakeLists.txt` — embeds `presets/factory_bank_500.json` in PPGUPresetData.
- `.github/workflows/vst3-build.yml` — validates/packages the 500 bank; removes legacy `factory_bank_300.json` from release PRESETS folder after copying legacy preset files.
- `tools/validate_factory_bank.py` — exact 250+250/500 gate; finite parameter ranges; unique IDs/names; artist/reference-track integrity; Melody articulation metadata; Pattern tuple schema/ranges.
- `PPGU_APPROVED_PARAMETERS.md` — registry advanced to v2 and factory count updated to 500.
- `docs/PRESET_FACTORY_500_RU.md` — schema, archetypes, coverage and validation limits.

**Structural preflight:** PASS (500 total, 250 per mode, unique IDs/names, 21 artist profiles, 16 genre families, 20 melody archetypes, 20 pattern archetypes; all Pattern definitions have tuple-v1 events). This was a deterministic structural check, not execution of the Python validator, Windows build, CTest, pluginval, or host acceptance.

**Important runtime boundary:** the catalog is authored data only. Presets remain runtime-validation-pending until actual loader/APVTS binding, sample mappings, articulation routing and Pattern event integration are implemented and auditioned. No BUILD VERIFIED or 500-playable claim.


## 2026-11-14 — Engineering delegation, 500-bank package correction, and CI status

**User authority:** explicit approval for autonomous technical decisions, implementation, optimization, reversible defect repair, use of available tools and reuse of public/compatible technologies, bounded by the approved master plan, legal rights, realtime contracts, data safety and existing public API/MIDI/APVTS compatibility. Full scope and operating guardrails are recorded in docs/PPGU_JOURNAL/PPGU_JOURNAL.md.

**Code/data changes observed on branch soundhunter-unbounded-library:**
- presets/factory_bank_500.json: normalized performance.patternLengthBeats=4 for all 250 PATTERN records, satisfying the validator's 4/4 pattern-cycle contract.
- .github/workflows/vst3-build.yml: corrected the active release-package validation path to PRESETS/factory_bank_500.json, while retaining cleanup of the obsolete PRESETS/factory_bank_300.json. This prevents the release check from looking for the retired 300-bank as the active catalog.
- Commit for package-path correction: 0913642e313aeb2d0fae82e57439c99699dcd682; follow-up correction preserving legacy cleanup while checking the 500 bank: d0c67a7481572debbc2324e162a053af9932d0d8.
- The 500-bank catalog/build integration commits remain 79d4e2ab0d382b84bb862ee3e03370c0a67129e3 and 99577e84744c1de13bdd8c7dd8db84cf79fc7829.

**CI:** GitHub Actions run 43577554118 was observed in_progress (build job). No successful completion, artifact, Windows/FL Studio host acceptance or release is claimed here. Job-log retrieval returned GitHub BlobNotFound; this is a tooling/log availability issue, not evidence of pass/failure. Re-check run status and artifacts before stating a result.

**Testing boundary:** the JSON structural preflight was previously PASS; the workflow's final build/test outcome is pending. The run is not a substitute for separate exact-SHA Windows 10 x64 build, CTest, pluginval and FL Studio 20 acceptance.


## 2026-11-14 — REV.18 FULL SCOPE CONFIRMATION (DOCUMENTATION ONLY)

**User-approved scope:** FX EDIT consolidated into one window/system with 10–15 previously approved, genuinely implemented DSP effects plus cabinet processing; 16 complete skins with NEON STAGE default and graphics-only variation; SETTINGS resource profiles GOOD/NORMAL/NIGHT/ECO, applicable 2x/4x/8x/16x oversampling modes, selectable 2/4/8 voices minimum and Serum-style persistence locks; ABOUT product/creator/version/engine metadata; full illustrated RU+EN manuals; SAMPLES replaces SOUNDBANK as the user-facing name and has no artificial size/file-count cap; minimum 500 functional MELODY plus 500 functional PATTERN presets; 82 additional Travis Barker-style presets; full Windows 10 x64 VST3/FL Studio 20 release package with licenses, resources, user preset support, SHA-256 manifest and multipart ZIP recovery.

**Canonical order retained:** MAIN EDITOR → MELODY → PATTERN → AUTOCHORDS → FX EDIT → AMP & TONE → DOUBLE → SOUND ENGINE → SAMPLES → PRESETS → IMPORT GUITAR → AUTO DISCOVER → CURATOR → SKIN GALLERY → SETTINGS → PERFORMANCE → ABOUT → TESTING → RELEASE.

**Superseded constraints:** new no-artificial-cap SAMPLES instruction supersedes earlier 25 GB and 25–30 GB size caps/targets; minimum 500 per mode supersedes the interim 250+250 catalog as a final acceptance target. Legal redistribution, provenance, hashes, deduplication, additive-only imports, actual disk/filesystem/platform limits and realtime performance protections remain mandatory.

**Important implementation boundary:** this commit records approved scope only. It does not implement DSP processors, skins, locks, documentation, sample assets, preset playback or packaging. The exact previously approved FX list and 16 skin names must be reconciled from their source specifications rather than guessed. The current 500-entry JSON is an interim structural catalog, not the final 500-per-mode playable bank. No build/test/host PASS is claimed.
