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

> **ОБЯЗАТЕЛЬНОЕ ПРАВИЛО РОЛЕЙ ТЕМ (13.11.2026):** SIGNAL FORGE / NEON — единственная основная тема продукта, DEFAULT/startup/fallback. **BLACKOUT VOLTAGE (`skin-51`) — только дополнительная тема**: не DEFAULT, не startup и не fallback. Сохранить её ID, имя, папку и ресурсы без переименования/перенумерации. Это правило действует для всех разделов проекта, включая реестр, ThemeManager, галерею, тесты, документацию и сборку. Текущий `skin-51` default/fallback в исходниках/манифесте — зафиксированное несоответствие, исправлять после завершения аудита Фазы 0; до этого код и манифесты не менять.
