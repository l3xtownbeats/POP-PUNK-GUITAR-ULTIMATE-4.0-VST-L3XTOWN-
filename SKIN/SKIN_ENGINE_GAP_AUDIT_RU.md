# PPGU Skin Engine — актуальный gap audit (REV 14.0)

Дата актуализации: 12 ноября 2026. Ветка: main. Этот документ заменяет устаревшие выводы из аудита 11 ноября; единственный журнал статусов и CI — `Info.txt`.

## 1. Текущее состояние исходников

### Уже присутствует в PluginEditor
- `skinButton` и обработчик `showSkinMenu()`.
- Ctrl+K через `keyPressed()`.
- Вычисление sibling runtime path `Skins/` относительно установленного VST3 bundle на Windows.
- Dev fallback к source `SKIN/` при запуске из дерева разработки.
- Сохранение `skinId` через JUCE ApplicationProperties.
- `PPGUThemeManager` создаётся редактором; выбранная палитра применяется к LookAndFeel и существующим компонентам.
- Popup перебирает фиксированный диапазон 1..51 и загружает манифесты; это не Theme Gallery.

### Текущие ограничения ThemeManager / Editor
- ThemeManager поддерживает старый manifest contract с вложенными группами `tokens.canvas/surfaces/rotary/...`, а не новый плоский набор 21 REV 14 token.
- C++ validator требует ID `skin-XX`, index 1..51; папки строятся как `Skin_%02d`. Это не совместимо с новыми ID `skin-052+` и папками `Skin_052+`.
- Palette содержит только ограниченный поднабор цветов; нет универсального `ThemeManager::token()` для всех 21 tokens.
- В PluginEditor остаются глобальные hardcoded colours и прямые цвета в декоративной графике. REV 14 требует перевести любой визуальный цвет на tokens; эта работа не завершена.
- Нет сканирования всех дочерних каталогов `Skins/`; фиксированный count — 51.
- Popup не показывает графические preview/hero cards, нет family filters, favorites, theme search или полноценной клавиатурной навигации.
- Нет фонового rescan, debounce и live discovery новых каталогов без перезапуска DAW.
- Нет подтверждённого 150 ms hot-swap fade; применение палитры сейчас вызывает немедленную перерисовку.
- Нет ECO animation policy, asset memory budget/LRU или полной проверки asset path containment.
- Fallback default есть на уровне palette, но нет полноценного gallery error state и честного предупреждения о конкретной битой/несовместимой теме.
- Нет автоматического parity test, который сверяет theme control IDs с фактическими JUCE components и attachments.

## 2. Реальный control inventory

Актуальный источник — `CONTROL_INVENTORY_MATRIX_RU.md` и `control_schema.json`. В текущем редакторе реально присутствуют preset selector/category/search, LOAD SFZ, status label, MELODY/PATTERN, 9 rotary, 7 FX bypass toggles, sync selector, MIDI keyboard, SKIN popup, Ctrl+K и сохранение выбора.

PUNK GROUP, dedicated LED, Auto Discover, Curator, Settings и About пока не имеют полноценной runtime-функции/компонента. Их ID включены в parity contract как обязательные, но до реализации их нельзя изображать активными функциями. APVTS parameter IDs без GUI также не превращаются в GUI-контролы через скин.

## 3. REV 14 contract и эталонная тема

- `SKIN/control_schema.json` — формальная JSON Schema для темы + обязательный inventory 27 control IDs + 21 palette tokens + 12×16 layout grid.
- `SKIN/Skin_052/theme.json` — Carbon Relay, family `carbon_studio`, layout `STUDIO_CONSOLE`; 27 обязательных control IDs mapped.
- `preview.png` — 360×240; `hero.png` — 800×450.
- `svg/texture.svg`, `svg/bezel.svg`, `svg/crest.svg` — присутствуют.
- `reduceInEco=true`, `hotSwapFadeMs=150`, `apvtsChanges/layoutChanges/audioChanges=false`.
- Контрастные значения дизайна рассчитаны: primary/window 17.81:1; secondary/window 10.79:1; disabled/window 5.23:1; LCD/window 17.17:1; accent/panel 15.09:1.
- `theme_pack_manifest.json` описывает только один reference theme с индексом 52. Registry 51 пока не менялся; merge не выполнялся.

Пакет является DESIGN / PACK GENERATED кандидатом до фактического прогона валидатора. Нельзя называть его VALIDATION PASS, MERGED или INTEGRATED заранее.

## 4. Validator / merge / CI

- `tools/validate_skin_pack.py` — проверка G1 contrast, G2 control parity, G3 PNG/SVG assets, G4 ID continuity.
- `tools/merge_skin_pack.py` — merge с backup; не запускать до согласования эталонной темы и runtime ID/schema migration.
- Workflow включает pre-build step для G1–G4. Результат ожидается отдельно по commit SHA.
- G5 ECO runtime, G6 audio invariance + 1000 hot swaps, G7 DPI 100/125/150% остаются отдельными runtime/QA gates.

## 5. Безопасность и архитектурные границы

- SkinManager и Gallery должны работать только на message/background worker threads; никакого file I/O, allocation, locks, GUI calls или waits в audio callback.
- Skin switching не меняет APVTS, presets, SoundBank, MIDI, processor state или DSP. Нужен output-buffer equivalence test.
- Runtime root — sibling `Skins/` рядом с установленным `.vst3`; AppData не является источником тем.
- Все asset paths должны быть относительными, canonicalized и containment-checked внутри каталога выбранной темы. Запрет symlink/path traversal escape.
- Invalid JSON, missing assets, decode error, unsupported schema/plugin version => сохранить/вернуть DEFAULT, показать честную ошибку в gallery/status, продолжить звук.
- Тема может задавать только visual tokens/materials/typography/artwork и presentation layout; функциональные hit targets/attachments не исчезают и не меняют назначения.

## 6. Открытые implementation gates

1. ThemeManager: 21-token model + `token()`, legacy manifest compatibility, schema/version checks, secure asset loading.
2. Dynamic catalog discovery: scan sibling `Skins/`, no fixed count, support Skin_01..51 legacy + Skin_052+ zero-padded.
3. Gallery: previews, search, family filters, favorites, keyboard navigation, DEFAULT, honest unavailable state.
4. Hot-swap: 150 ms UI fade; no processor/audio thread mutation; persistent selection by stable ID.
5. Background rescan: debounce and safe async message-thread handoff.
6. PluginEditor: remove hardcoded visual hex values; theme all existing controls, panels, rotary, FX, keyboard, labels and display.
7. Runtime parity: validate required IDs against actual components and parameter attachments; reserved controls remain disabled until implemented.
8. G5/G6/G7 automated/manual acceptance; Windows Release, CTest, pluginval strictness 5; separate FL Studio 20 host check.

**Честный вывод:** в main теперь есть начальная palette switch implementation и reference pack Skin_052. Полноценная Theme Gallery и REV 14 runtime engine пока не закрыты.


---

# Актуализация аудита — 12.11.2026, рабочая ветка REV.17

Этот addendum **заменяет устаревшие snapshot-утверждения выше** о fallback skin-51, registry из 51 темы, отсутствии gallery и непроведённом merge Skin_052/53–102. Исторические наблюдения сохранены как снимок более раннего состояния ветки main.

**Текущая ветка:** `work/ppgu-skin-expansion-53-102`.  
**Gallery/source chain:** `9054f92b4bd5b559a67eb68ed00a72dcd6f23b96`, `065279795c421a9b01e10e288faa1a8bb0eb2a1d`.  
**Overflow workspace fix:** `bc5f753b7bbc1f76c4c8c5cfc578e4b356c1181b`.  
**Skin data commit:** `3a2ceb52062be112429f96d411ae223d87df17a6`.  
**Skin CI:** run `43195205929` — SUCCESS.

## 1. Проверенное текущее состояние

- Canonical registry содержит 102 entries; default/fallback = `skin-052` / SIGNAL FORGE.
- Skin Studio pack 53–102 имеет status MERGED; registry total = 102.
- Для всех 50 новых тем опубликованы theme.json, preview.png, hero.png, bezel.svg, crest.svg и texture.svg; проверка путей дала 0 missing.
- ThemeManager поддерживает legacy Skin_01–51 и Skin Studio Skin_052+; fallback 21-token palette проверяется тестами.
- Gallery находится в `PPGUSkinGalleryPanel` внутри `PluginEditor.cpp`; SKIN и Ctrl+K открывают panel ID 5 в Multi-Window Workspace. Реализованы search, family filter, preview cards, active marker, click-to-apply и сохранение skin ID.
- Multi-window overflow теперь размещает все панели в minimum-size scroll grid; pure geometry tests обновлены для 1- и 2-column overflow, non-overlap и impossible geometry.

## 2. Оставшиеся runtime/QA gaps

1. Favorites, полноценная keyboard navigation в gallery, background watcher/debounce/rescan — NOT IMPLEMENTED.
2. 150 ms theme hot-swap fade — NOT IMPLEMENTED / NOT HOST VERIFIED.
3. ECO animation policy, image memory budget/LRU и lazy loading всех non-preview SVG/texture layers — NOT IMPLEMENTED.
4. Layout metadata `controls.*.grid` пока не управляет JUCE bounds; реальные компоненты остаются в текущем editor layout.
5. Полная runtime parity validation между schema IDs и реальными JUCE Components/attachments не завершена. LED, Auto Discover, Curator, Settings и About остаются NOT IMPLEMENTED; не показывать их как работающие.
6. Полная очистка hardcoded decorative colours по всем UI components не завершена.
7. 100/125/150% DPI visual review и FL Studio 20 host acceptance — PENDING.

## 3. Exact-SHA validation

Skin generation/validation/merge/publish подтверждены run `43195205929`. Windows VST3 workflow `43195592441` на code SHA `bc5f753b7bbc1f76c4c8c5cfc578e4b356c1181b` был IN_PROGRESS на момент этой актуализации; до финального результата нельзя ставить BUILD/CTest/pluginval PASS. Host review не выполняется автоматически этим workflow.

**Итог:** 102 themes + searchable preview gallery SOURCE PRESENT / PACK PUBLISHED; complete Skin Engine acceptance NOT COMPLETE.


## Settings panel correction — 12.11.2026

Settings is no longer a deferred/absent panel in this branch. `settingsButton` opens `PPGUSettingsPanel` (workspace panel ID 6). Implemented real persisted options are auxiliary panel scale, panel gap, restore canonical SIGNAL FORGE and open Skin Gallery. This does not complete product-wide settings: audio/engine settings, MIDI settings, global UI scaling, performance options and About are not represented by this small workspace settings panel.
