# PPGU MASTER SPEC PACKAGE — REV 17.0 BASE / FRONTEND & NEON SKIN

**Дата:** 2026-11-12  
**Статус документа:** DESIGNED / REQUIREMENTS BASELINE. Не является доказательством реализации.  
**Целевая ветка:** `work/signal-forge-theme-tokens`  
**Проверенная база перед документальным изменением:** `03545e718e7276756a56605fad3de0a0e1933dfd` (tree `bcfb84d1c3d04538c426491a4f715574720aae17`).  
**Фактическая версия master prompt на этой базе:** REV 17.1 UNIFIED (minor), файл `docs/PPGU_MASTER_PROMPT_RU.md`. Указание REV 17.0 BASE в исходном пакете трактуется как историческая основа, а не как текущая версия canonical master prompt.

## 1. Назначение и границы

Документ объединяет план фронтенд-разработки, Neon Skin Engine, Overlay Panel System, AutoChords/FX EDIT UI, Secure Skin Import, контрактные тесты и этапы приёмки. Он дополняет, но не заменяет `docs/PPGU_MASTER_PROMPT_RU.md`, `SKIN/control_schema.json`, существующие INFO/CODE governance и утверждённые три визуальных макета.

**Иерархия фактов и требований (уточнение):**
- Canonical source tree и build configuration на точном SHA определяют, что реализовано.
- Тесты/CI подтверждают только реально исполненные проверки на том же SHA.
- `Info.txt` — append-only журнал фактов, решений, статусов и рисков; он не может переопределить исходный код.
- `CODE.txt` — append-only индекс изменений с точными путями/символами/SHA.
- `SKIN/control_schema.json` — контракт контролов в пределах своей версии.
- Master prompt и этот пакет — требования/архитектурные решения, не доказательства реализации.
- Макеты — визуальная цель, не runtime-реализация.
При конфликте сохранить обе исторические записи, исследовать источник и добавить явную Correction/Supersedes запись. Не использовать упрощённую иерархию из исходного пакета «INFO > CODE > schema > prompt» как иерархию доказательства реализации.

## 2. Неприкосновенные инварианты

1. DSP, audio callback, MIDI routing, APVTS IDs и семантика, пресеты, SoundBank и поведение существующих контролов не меняются в UI/skin фазах.
2. UI читает аудио-состояние только через атомарные read-only taps/snapshots; никаких блокировок/аллокаций в audio thread.
3. Audio code не зависит от skin, overlay, JUCE Components или UI lifecycle.
4. Skin/theme/panel changes не изменяют звук, preset, MIDI или audio state.
5. Реальные контролы привязываются к существующим параметрам/логике. Нереализованное отображается disabled/dimmed с честной причиной; фальшивая активность запрещена.
6. JUCE 7.0.12, CMake 3.22, C++20, Windows x64, FL Studio 20 — закреплённая конфигурация; API сверять с pinned JUCE.
7. Никакой PASS не переносится на другой SHA. Не заявлять build/CTest/pluginval/host/release PASS без точного доказательства.

## 3. Продуктовая и аудио-база — заявленные факты, требующие SHA-аудита

Планируемая/описанная база включает WAV/SFZ playback path, metadata index, page streaming/cache, SFZ key/velocity/channel mapping, round-robin, до 128 snapshots, 8 streaming roots и 64 voice slots. Эти числа и функции должны подтверждаться source/tests на выбранном SHA; данный документ не повышает их статус.

Открытые риски для отдельного аудио-плана:
- R1: use-after-free при освобождении старых snapshots.
- R2: память/потоки при нескольких streaming roots.
- R3: восстановление после падения streaming worker.
- R4: 64 voices при cache pressure.
- R5: cancellation/lifetime гонки загрузки и уничтожения.
- R6: полный Windows Release + CTest + pluginval после изменений.
Недостатки аудиодвижка не входят в scope Neon/Overlay документационных и UI фаз. Исправление требует отдельного аудио-ревью и отдельного разрешённого тикета.

## 4. Визуальная цель — Dreaming in Neon

Три утверждённых макета являются визуальным эталоном:
1. Основное окно.
2. AutoChords overlay.
3. FX EDIT + AutoChords одновременно.

Сохранять геометрию, отступы, стекло/свечение, иконки, иерархию и общую композицию максимально близко к макетам. Панели по умолчанию — overlay внутри основного окна, не отдельные ОС-окна. Не заменять утверждённый дизайн собственной компоновкой.

### 4.1 Главное окно

- Header: PPGU logo/crown, preset previous/next, favorites, preset name/genre из metadata, Search, SKINS, Import Guitar (существующий LOAD SFZ flow).
- Auto Discover, Curator, Settings/About до реализации — disabled «In development».
- Browser: BROWSER/FAVORITES/CATEGORIES; реальные счётчики из factory bank; favorites persistent; карточка из metadata.
- Hero: JUCE Graphics/vector guitar + palette-driven splatter/background; не притворяться, что внешние artwork assets существуют.
- MELODY/PATTERN — только если реальные функции присутствуют; PUNK GROUP disabled до реализации.
- dBFS meter — только после отдельного реального post-FX atomic tap.
- Девять ручек варианта A: ampGain=VOLUME, eqLow=BASS, eqMid=MID, eqHigh=TREBLE, velocity, dynamics, swing, humanize, accent. Никаких новых APVTS IDs в этих фазах.
- FX Chain: семь LED только от реального состояния семи bool FX параметров; выбор открывает FX EDIT.
- AMP & TONE: GAIN/BASS/TREBLE отображают ampGain/eqLow/eqHigh; selector модели disabled до реального параметра.
- Keyboard: ноты 36–84; подсветка только при реальном MIDI state. PITCH/MOD — отдельный этап.
- Status bar: Licensed/CPU/BPM только из реальных источников; MIDI Omni и SYNC отражают действующее состояние.

### 4.2 AutoChords overlay

Реально работающая UI-логика: Chord Mode Major/Minor/Custom, Key, Pattern, вычисление текущего аккорда и аппликатуры, progression buttons, loop UI-state, Chord Info и Reset. Tempo читается из transport. ON, Preview, Sync to Tempo, Smart Transpose остаются disabled до соответствующей реальной интеграции. Не рисовать фиктивную waveform.

### 4.3 FX EDIT overlay

Список Compressor/EQ/Distortion/Chorus/Delay/Reverb/Cabinet IR связан с семью существующими bool параметрами. Активная подсветка допустима только при включённом соответствующем FX. Threshold/Ratio/Attack/Release/Makeup/Knee/Mix, GR meter и compressor curve disabled/empty до наличия реальных параметров и taps.

## 5. Overlay Panel System

Базовый контракт:

```cpp
class OverlayPanel : public juce::Component {
public:
    enum class PanelId { AutoChords, FXEdit, Settings, About /* N5 */ };
    void open(juce::Rectangle<int> suggestedBounds);
    void close();
    void toggleCompact();
private:
    std::function<void()> onClose;
    PersistKey persistKey;
};
```

- Панели имеют header/icon/title; ON показывается только при реальном параметре; квадрат — compact/restore; X — закрытие.
- Закрытие не сбрасывает panel state; bounds/compact/open-state persist.
- OverlayManager хранит реестр, управляет независимым open/close, focus/z-order, tiling, paging, сохранением и teardown.
- Последняя открытая/сфокусированная панель сверху; клик поднимает её.
- Lifetime — JUCE SafePointer или эквивалентная проверенная стратегия; никаких висячих ссылок.
- При нехватке пространства AuxiliaryPanelLayout переводит панели в compact/tiled режим; важные элементы не перекрываются. Overflow — paging, не потеря доступности.
- Settings/About и detach-to-OS-window относятся к N5; detach — необязательная опция, не базовое требование.
- Открытие/закрытие/перемещение панели не меняет APVTS, MIDI или аудиосостояние.

## 6. Neon Skin Engine

### 6.1 Manifest model

```cpp
struct SkinManifest {
    juce::String id;       // e.g. skin-053
    int index;
    juce::String family;
    juce::String layoutName;
    Palette palette;       // exactly 21 strict HEX tokens
    SkinLayoutMap layout;  // exact control inventory + sync_selector
    ECOFlags eco;          // reduceInEco, hotSwapFadeMs
    Compatibility compat;  // visual-only flags
    AssetPaths assets;     // preview/hero/texture
    int schemaVersion;
};
```

21 tokens: window_bg, panel, panel_elevated, control_bg, control_face, control_border, text_primary, text_secondary, text_disabled, accent, accent_glow, danger, warning, success, lcd_bg, lcd_text, led_on, led_off, shadow, highlight, lowlight.

Layout parity проверяет точный canonical control-ID inventory из актуального `SKIN/control_schema.json` (27 IDs + `sync_selector`, если именно этот список подтверждён schema на целевом SHA); нельзя заменять проверку точного набора проверкой минимального количества элементов. Индекс/ID/family/layout/schema version/compatibility проходят строгую типовую и диапазонную валидацию. Compatibility-флаги строго visual-only; неверные JSON-типы не коэрцируются.

### 6.2 Resource resolution and runtime

- Единый ResourceResolver: `PPGU_ROOT.ini → environment → marker → legacy Skins/` согласно canonical master prompt и R34. AppData не становится источником тем. Каталог должен соблюдать точное имя/расположение, закреплённое master prompt; не создавать второй конкурирующий resource root.
- ThemeManager текущего 21-token API остаётся совместимым слоем; полный Skin Engine — отдельная реализация, а не синоним palette switch.
- Palette публикуется как immutable atomic snapshot, например `std::atomic<std::shared_ptr<const Palette>>` в C++20. Все readers получают согласованный snapshot; частичная/повреждённая палитра получает deterministic per-token fallback и fromFallback/error diagnostics.
- HEX строго `#RRGGBB`; invalid token не вызывает crash.
- Hot-swap: новый snapshot публикуется атомарно; UI-thread cross-fade 150 ms; SkinListener уведомляет все открытые компоненты. Никакой работы в audio callback.
- ECO выключает дорогие shadow/glow/animation слои, но не меняет layout, текст, контраст или функциональность; G1 проверяется также в ECO.
- Asset cache — bounded LRU, лимиты/eviction/pinning должны быть протестированы.
- Layout engine E3: 12×16 grid, DPI scaling, checked int64 arithmetic; invalid/overflow inputs дают контролируемый отказ, не UB.

## 7. Secure Skin Import — формат и конвейер

Формат `.ppguskin` — ZIP package:
- `theme.json` — manifest по canonical control schema;
- `assets/preview.png` — optional;
- `assets/hero.png` — required согласно package profile;
- `assets/texture.svg` — optional safe subset.

Pipeline строго последовательный:
1. ZIP structure check.
2. Size caps: archive ≤ 50 MB; total decompressed payload ≤ 200 MB; также ограничить число entries, per-entry size, compression ratio, nesting и integer overflow.
3. Parse manifest + schema validation.
4. Asset decode + CRC + dimensions/format validation.
5. G1 contrast validation.
6. G2 exact control parity.
7. G4 identity validation.
8. Quarantine copy + SHA-256 package/asset manifest.
9. Success dialog: name, family, warnings.
10. Register in user-skins list только после успешного завершения всех проверок.

Любой fail — пакет не применяется и не регистрируется; точное сообщение указывает номер стадии; исходник/карантинный материал сохраняется в `import_failed/` в разрешённом resource root, если безопасно сохранить его возможно. До валидации запрещены извлечение в активный skin directory и любые runtime side effects.

Защита: Path Traversal, абсолютные/drive/UNC/extended paths, backslash traversal, symlinks, hardlinks, duplicate normalized names, encrypted/nested archives, malformed central directory, zip bombs, oversized assets, integer overflow — reject. Исполняемые файлы (.dll/.exe/.cmd/.ps1/.jar и любые неизвестные executable content types) — RED. Пакет не исполняет код и не делает сетевые запросы.

SVG parser принимает только документированное безопасное подмножество XML/SVG: без DOCTYPE/ENTITY/XXE, script, event handlers, external href/src, external fonts, CSS imports/URLs, foreignObject, animation и неизвестных активных элементов/атрибутов. Не «очищать» произвольный SVG эвристическим удалением script; отклонять неподдерживаемые конструкции.

PNG: проверять декодирование, CRC всех chunks, допустимые размеры/пиксели, формат и лимиты памяти до выделения больших буферов. CRC — проверка повреждения, не криптографическая аутентификация. SHA-256 фиксирует содержимое карантинной копии; повторная runtime validation при старте с cache keyed by verified hash. Изменённый/повреждённый пакет — fallback на Default Skin с понятным сообщением, без падения.

UI thread отвечает за выбор файла/прогресс/диалоги/регистрацию; ZIP parsing, hashing, image decode и validation выполняются на background worker с cancellation/lifetime safety. Публикация темы происходит только после успешной валидации и атомарного завершения регистрации.

## 8. Фазы реализации и разрешённый scope

Frontend architecture sequence является canonical для этого пакета:
1. **E1 — Neon palette skin / G1–G4**: skin-only pack, exact control parity, честные disabled states.
2. **O1 — Overlay shell**: базовый panel shell/manager, AutoChords UI logic, FX EDIT module list.
3. **O2 — Multi-panel**: tiling, compact, focus/z-order, persistence, paging, DPI/lifetime.
4. **S1 — Secure Skin Import**: .ppguskin pipeline, quarantine, SHA-256, safe SVG/assets.
5. **E2 — Runtime hot-swap + ECO**: atomic snapshots, fade, listeners, ECO.
6. **E3 — Runtime layout engine**: grid/layout tokens, DPI and overflow-safe geometry.
7. **E4/O3 — Deferred integration**: FX parameters/meters, AutoChords ON/Preview, LEDs and PITCH/MOD only under separately approved tickets and only if they do not violate APVTS/audio invariants.

**N0** — обязательный governance preflight before every phase, not a production implementation phase.  
**N1–N6** — legacy roadmap labels from prior product planning; they are not an alternative ordering and are superseded where they conflict with E1→O1→O2→S1→E2→E3→E4/O3. N5 Settings/About/detach and N6 new parameters/AutoChords audio integration remain deferred and separately gated. No new APVTS IDs are authorized by this package.

## 9. Contract tests and CI

All checks must be release-active contract tests, not debug-only asserts. Required suites/targets (names are proposed until CMake proves they exist):
- `ppgu_skin_engine_contract`: atomic publication, concurrent readers during swap, HEX validation, partial JSON fallback, ECO, manifest schema, 52+ themes, exact control parity.
- `ppgu_overlay_manager_contract`: independent open/close, 2–6 panels, compact/tiling/paging, focus/z-order, bounds persistence, SafePointer teardown, DPI.
- `ppgu_autochords_overlay_contract`: all Key×Mode combinations, chord/progression mapping, transport tempo, honest disabled states.
- `ppgu_fxedit_overlay_contract`: exact seven FX-to-bool mapping, active LED state, disabled parameters/meters/curve.
- `ppgu_skin_import_contract`: corrupt ZIP, zip bomb, traversal, duplicate paths, executable entries, invalid HEX, missing/invalid assets, malformed SVG, CRC/dimensions, changed SHA, quarantine, fallback and exact stage errors.

Target names above are acceptance requirements, not claims that targets currently exist. Audit actual CMake/tests before adding or reporting them. Required gates for each phase: Windows x64 Release build; every CTest target; locate VST3; pluginval strictness 5; packaging assertions; FL Studio 20 host checklist and actual host screenshot; COMPLETE ZIP only after all gates pass on same SHA. Host evidence is separate from CI.

## 10. Release and journal transaction

For every phase: preflight branch/HEAD and compare relevant branches; inspect master prompt/schema/source/tests; scope one logical reason per commit; implement; run checks on exact SHA; append INFO and CODE; read both journals back and cross-check. No source/audio/APVTS/SoundBank scope creep. Documentation-only changes explicitly state production source/tests/assets/build files unchanged and CI not run/not claimed.

## 11. Traceability matrix

| Component | Requirement | Current evidence/status at package baseline | Gap | Phase |
|---|---|---|---|---|
| Neon Skin Engine | Full theme system, not palette-only | 21-token ThemeManager API reported SOURCE PRESENT; exact current SHA audit still required | resource resolver, full palette/runtime, ECO, fade, layout, secure import | E1/E2/E3/S1 |
| AutoChords overlay | Approved functioning panel | Chord Transition engine reported present; overlay not integrated | shell, UI logic, layout, state | O1/O2 |
| FX EDIT overlay | Approved panel, honest controls | seven bool FX parameters reported present; panel not integrated | shell, list binding, disabled controls | O1/O3 |
| Multi-panel | Concurrent panels | geometry module reported present; CTest/manager integration unverified | manager, tiling, focus, persist, paging, DPI | O1/O2 |
| Skin Import | Secure .ppguskin install | not implemented at package baseline | archive parser, validation, quarantine, runtime verification | S1 |

Statuses in this table are provisional baseline claims from supplied project context, not fresh source/CI verification. They must be re-audited at N0 on the actual implementation base.

## 12. Explicit non-goals

No DSP/audio callback/MIDI/APVTS ID semantics/preset/SoundBank changes; no new parameter creation; no sample-library changes; no skin package code execution; no separate OS windows before N5; no claim that mockups, specs, manifests or target names are implementation. Existing audio risks remain open and are not silently closed by frontend work.
