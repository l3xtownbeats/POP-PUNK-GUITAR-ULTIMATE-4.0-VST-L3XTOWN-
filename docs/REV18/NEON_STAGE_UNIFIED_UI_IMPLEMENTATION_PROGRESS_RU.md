# REV.18 — NEON STAGE unified UI implementation progress

**Дата:** 2026-11-15  
**Ветка:** `rev18/neon-stage-skin1-implementation`  
**Каноническая тема:** SKIN-1 / skin-053 / NEON STAGE DEFAULT  
**Scope:** UI-only. Audio/DSP, APVTS IDs and values, MIDI routing, sampler, scheduler, preset semantics are out of scope.

## Выполнено в исходниках

1. `Source/PluginEditor.cpp`
   - Main editor applies semantic NEON STAGE palette tokens to labels, selectors, search, buttons, rotary controls, FX toggles and keyboard.
   - Import Guitar panel initializes and refreshes its labels/status/load button from the same palette.
   - FX EDIT panel initializes and refreshes selector, power control, parameter labels, rotary controls, status and graph using shared tokens.
   - AutoChords panel uses shared text/warning/control tokens and preserves its explicit setup-only / generation-not-connected disabled state.
   - Settings copy now says “RESTORE NEON STAGE DEFAULT”.
   - Remaining main editor keyboard and rotary literal colours were replaced by palette tokens.
   - Skin Gallery fallback accent changed to NEON cyan.
2. `Source/UI/PPGUMultiWindowWorkspace.h`
   - Default workspace background, panel, border, heading and text fallback colours now match NEON STAGE.
3. `Source/UI/ThemeManager.h`
   - Existing canonical 21-token NEON STAGE palette remains the shared source of truth.

## Exact source revisions

- `Source/PluginEditor.cpp`: blob SHA `71af87fb8370910c2fe79750887642b7385fe8fc`
- `Source/UI/PPGUMultiWindowWorkspace.h`: blob SHA `c235a819fa380fd35b3150e5cd3784ca5a0190af`
- `Source/UI/ThemeManager.h`: blob SHA `4f095d6a11ea15cd429f968201499acd6f97037b`

Recent commits on the feature branch:
- `8dc3c341b155653cb5baaafec38a3661cfc3cd1b` — auxiliary panels adopt semantic palette tokens.
- `5de46510f8b7d086c8be796cf46a81a726cfefe2` — remove remaining main-editor control colour literals.
- `a347f3e7b9ebafbb1dd28603b8ac6c8068265823` — workspace fallback matches NEON STAGE.
- `59eec1aad334d5a2c59055d6cae9bc9da21870ac` — gallery fallback accent matches NEON STAGE.

## Module status — do not overstate implementation

| Module | Current truth |
|---|---|
| Main Interface | Existing controls now consume shared NEON STAGE palette; full visual QA pending |
| AutoChords | Existing setup panel themed; generation/preview engine remains disabled/not connected |
| Curator | Not implemented; must remain unavailable/disabled, no fabricated GREEN/YELLOW/RED results |
| Sound Hunter | Existing library/registry assets are not proof of a complete UI; end-to-end UI/runtime remains to be audited |
| Auto Discover | Not implemented; no fake scan/progress/results |
| FX EDIT | Existing APVTS-bound controls themed; source-to-control and host behavior QA pending |
| AMP & TONE | Main-editor controls use shared theme; detailed mockup parity and host QA pending |
| Import Guitar | Existing SFZ import panel themed; async success/failure behavior must be verified in build/host |
| Performance | Existing MIDI monitor uses shared palette; no invented telemetry; runtime QA pending |
| Settings | Existing workspace settings themed; only implemented workspace settings are exposed |
| Skin Gallery | Existing gallery themed; hot-swap/DPI/ECO/favorites/navigation acceptance remains partial |
| About | Not implemented; no fabricated product panel |

## Validation status

- GitHub source reads and exact blob-SHA checks: performed for the files listed above.
- Static source review: performed for the touched UI paths.
- Windows x64 Release build: NOT YET VERIFIED for these commits.
- CTest: NOT YET VERIFIED for these commits.
- pluginval: NOT YET VERIFIED.
- FL Studio 20 host acceptance: NOT YET VERIFIED.
- DPI 100/125/150%, resize and visual regression: PENDING.
- PR #42 remains draft/open and is not to be merged before gates pass.

## Reference mockup inspection limitation

The repository tree confirms all eight NEON STAGE PNG reference files are present. The connected GitHub file reader returned empty base64 for those binary PNG files and the blob reader rejected binary UTF-8, so this pass did **not** visually inspect their pixels. The mockups remain the approved visual source of truth; image-by-image visual comparison is still an explicit QA task and no pixel-level parity claim is made.

## Next implementation sequence

1. Obtain a visual-capable read of the eight committed PNG mockups and record component/layout mapping.
2. Continue the source-to-control audit for all existing panels and eliminate remaining unrelated hardcoded visual styles.
3. Implement missing visual shells only where the approved specification permits; keep non-existent backends honestly disabled.
4. Add/complete token parity and panel geometry tests.
5. Run Windows Release, CTest, pluginval and FL Studio 20 host acceptance; record results against exact commit SHA.


## Дополнительный CI-аудит и исправление канонического Skin-1

**Проверенный CI run:** [43678727830](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/43678727830) для HEAD `96098a6740bf35c0fa8e0037704d6cbe400a36cf`.

- Workflow Windows VST3 стартовал и успешно прошёл шаги генерации/проверки legacy artwork, каталога 300 пресетов, REV.18 readiness unit tests, portable preset examples и APVTS registry (97 объявленных ID; 96 стабильных ID сохранены).
- CI остановился на шаге `validate_skin_pack.py`: `SKIN/Skin_053/theme.json: pack manifest name mismatch`.
- Причина: канонический `Skin_053/theme.json` уже назывался NEON STAGE, а два реестра всё ещё содержали старую идентичность Carbon Studio 01. Поэтому CMake build, CTest и pluginval были пропущены; это не ошибка компиляции и не успешная сборка.

**Исправлено на feature branch:**
- `SKIN/theme_manifest.json`: запись skin-053 синхронизирована с NEON STAGE / neon_basement; `defaultSkin` установлен в `skin-053`.
- `SKIN/theme_pack_manifest.json`: соответствующая запись синхронизирована с тем же каноническим именем и семейством.
- `SKIN/Skin_053/theme.json`: исправлено текстовое описание количества привязок с 27 на 28 (сами bindings не менялись).

Исправления внесены отдельными коммитами:
- `78635790fa779a67666f50a1729782a946b8144c` — registry + canonical default.
- `e7a2fd16c23b8533d2c872c4b98723df22943413` — pack manifest parity.
- `a3264a093cc4bd7308bdd9955308f5fe4d441d98` — control-count note.

**Повторная CI-проверка после этих исправлений ещё не подтверждена.** Следующий запуск должен заново пройти manifest validation и только после этого перейти к конфигурации, Windows Release build, CTest и pluginval. Никаких заявлений о runtime/FL Studio acceptance пока нет.


## УТВЕРЖДЕНО — следующий этап NEON STAGE UI (2026-11-15)

Пользователь подтвердил и утвердил следующий план работ. Этот план фиксируется как обязательная последовательность UI-аудита и интеграции. Он не означает, что перечисленные проверки уже выполнены.

### Зафиксированные базовые коммиты

| Коммит | Зафиксированное изменение |
|---|---|
| `8dc3c341` | Единое оформление вспомогательных модулей |
| `5de46510` | Устранение оставшихся цветов контролов |
| `a347f3e7` | Единый стиль Workspace |
| `59eec1aa` | Цветовой эталон Skin Gallery |
| `96098a67` | Журнал интеграции и проверок |

### Утверждённая последовательность по модулям

| Модуль | Следующий этап |
|---|---|
| Main Interface | Проверка соответствия макету |
| AutoChords | Проверка полного визуального соответствия |
| Curator | Реализация интерфейса после готовности функциональной части |
| Sound Hunter | Аудит интерфейса и интеграции |
| Auto Discover | Интерфейс после реализации механизма обнаружения |
| FX EDIT | Проверка всех страниц |
| AMP & TONE | Проверка компоновки и контролов |
| Import Guitar | Проверка импорта и статусов |
| Performance | Проверка MIDI-монитора |
| Settings | Проверка всех существующих настроек |
| Skin Gallery | Проверка переключения тем |
| About | Реализация окна |

### Правила исполнения

1. Работать последовательно, фиксируя результаты по каждому модулю отдельно.
2. Для Main Interface и AutoChords выполнить фактическую сверку с эталонными NEON STAGE макетами; не заявлять визуальное соответствие до просмотра макетов и проверки реализации.
3. Для каждого существующего модуля проверять связь видимых контролов с реальными состояниями/параметрами, размеры, состояния, подписи, навигацию и поведение в доступном коде.
4. Curator и Auto Discover не оформлять как функционально готовые интерфейсы до готовности соответствующих механизмов. Не создавать фиктивные результаты, статусы или прогресс.
5. About создавать только в пределах подтверждённых продуктовых данных и реально существующих возможностей.
6. Не менять DSP/audio engine, APVTS IDs/values, MIDI routing, sampler, scheduler или семантику пресетов в рамках UI-аудита.
7. Любые сборки, тесты, pluginval и host acceptance отмечать только после фактического выполнения на конкретном commit SHA.
8. Сохранять NEON STAGE DEFAULT / SKIN-1 как единый визуальный эталон и не создавать дублирующую тему.

**Статус:** план утверждён пользователем; выполнение следующих проверок остаётся открытым.


## MAIN INTERFACE — FULL VISUAL AUDIT (первый source-аудит)

**Дата:** 2026-11-15  
**Область:** Source/PluginEditor.cpp, Source/UI/ThemeManager.h, Source/UI/PPGUMultiWindowWorkspace.h.  
**Статус:** source-level audit выполнен частично; визуальное сравнение с PNG и host QA остаются открытыми.

### Подтверждено в исходном коде

- Основной редактор имеет трёхзонную компоновку: preset browser / guitar engine & performance / amp & FX; отдельная нижняя полоса содержит MIDI keyboard, performance display и статус библиотеки.
- В центральной зоне размещены MELODY/PATTERN tabs, режим воспроизведения и девять rotary-контролов. Их SliderAttachment создаются к существующим APVTS parameter IDs.
- В правой зоне размещены AMP & TONE и существующие FX toggle-кнопки; для них создаются APVTS ButtonAttachment.
- MIDI keyboard ограничена диапазоном 36–84. Performance display читает реально удерживаемые MIDI-ноты и существующие параметры scale root/mode; это не фиктивная телеметрия.
- Header содержит реальные действия открытия Skin Gallery, Settings, AutoChords и Import Guitar. Панели открываются через общий PPGUMultiWindowWorkspace.
- applyPalette() обновляет семантические цвета основных labels, combo boxes, search, кнопок, tabs, sliders, MIDI keyboard и workspace. Workspace и Performance display получают активную палитру.

### Найденные и исправленные несоответствия

1. **Default skin mismatch:** loadSavedSkin() использовал skin-052 как fallback, хотя канонический SKIN-1 — skin-053. Исправлено на skin-053.
2. **Restore mismatch:** Settings вызывал loadSkinByIndex(52), что восстанавливало не канонический SKIN-1. Исправлено на index 53.
3. **Hardcoded foreground colours:** два белых foreground назначения в главном редакторе заменены на семантический window_bg, согласованный с контрастной схемой NEON STAGE.
4. **Hardcoded rotary shadow:** чёрная тень rotary control переведена на semantic token shadow.
5. **Stale source terminology:** комментарий компоновки SIGNAL FORGE заменён на NEON STAGE; алиасы темы не должны выглядеть отдельным брендом.

### Открытые замечания / blockers

- Эталонные PNG-макеты ещё не удалось визуально прочитать через GitHub connector. Следовательно, соответствие размеров, отступов, типографики, визуальной иерархии и конкретных элементов макету не подтверждено.
- В исходнике видны потенциально спорные точки для визуальной проверки: фиксированные размеры header/footer и трёх колонок; subtitleLabel позиционируется абсолютными координатами; компоновка knob grid фиксирована 3×3. Их корректность должна оцениваться по утверждённому макету и в реальном resize/DPI, а не объявляться дефектом без визуального эталона.
- Полный host-level контроль контрастности, clipping, focus/hover/disabled states, 100/125/150% DPI и resizing не выполнялся.
- Проверка runtime discovery каталога Skins и реального применения skin-053 при старте/restore требует сборки и host-теста.

### Source revision

- Source/PluginEditor.cpp: после исправления fallback/restore/semantic colours и stale comment — blob SHA 97c89f4700b3ce458b85a53239d481dc363160ff.
- Изменения внесены коммитами:
  - 954dd0b9d72480ca261fd741f289dd63cea5bde5 — canonical fallback/restore + semantic colours.
  - dfbaa91db0380465ab27337527c7dba32d593925 — NEON STAGE shell terminology.

**Решение этапа:** source-level несоответствия исправлены; MAIN INTERFACE не закрыт как визуально принятый. Следующий шаг — получить PNG-макеты в читаемом виде, провести pixel/layout comparison, затем исправлять только подтверждённые расхождения.

## 2026-11-15 — Semantic palette enforcement / CTest contract

**Рабочая ветка:** `rev18/neon-stage-skin1-implementation`  
**Production source:** `2712065c2fee672c3a4ee6957be6a8ebad08cfc2`  
**Validator correction:** `9fa3f1c54dd3ec286bae07fd7816537fb068ff8f`

- Удалены дублирующие literal-цвета из Skin Gallery fallback и Workspace member defaults.
- Workspace теперь получает начальные поверхности из canonical `PPGUThemeManager::Palette`; при дальнейшем выборе темы применяется та же семантическая палитра.
- Добавлена проверка отсутствия hard-coded hex colours в UI C++ (кроме определения самой палитры в `ThemeManager.h`).
- Проверка `tools/validate_skin053.py` подключена к CTest как `ppgu_skin053_ui_contract`.
- Source readback: 0 raw hex colour literals в проверенных UI C++ файлах вне ThemeManager palette source.
- Manifest readback: `skin-053 / index 53 / NEON STAGE`, 21 tokens, 28 control bindings.
- GitHub Actions `43680164068` на exact SHA `9fa3f1c54dd3ec286bae07fd7816537fb068ff8f` — IN PROGRESS на момент фиксации; build/test/pluginval/host acceptance пока не присвоены.

**Статус:** SOURCE CONTRACT IMPLEMENTED / CI PENDING / VISUAL QA PENDING.  
**Следующее:** дождаться exact-SHA CI и продолжить аудит реальных source surfaces для всех 12 модулей. Отсутствующие Curator, Auto Discover и About остаются не реализованными; не добавлять им фиктивную функциональность.


## MAIN INTERFACE — повторный source-аудит текущего HEAD

**Дата проверки:** 2026-11-15  
**Ветка:** `rev18/neon-stage-skin1-implementation`  
**HEAD до записи этого отчёта:** `247fa531273dcc2a7325275f821f1d4c67c5f4f4`  
**Проверенный файл:** `Source/PluginEditor.cpp`, blob `bcb43811c9a975aaedd476263d3f654b62c53821`  
**Проверенные общие компоненты:** `ThemeManager.h` blob `4f095d6a11ea15cd429f968201499acd6f97037b`; `PPGUMultiWindowWorkspace.h` blob `7b5cba5f94d7b7895b64704d1627a287b85caac4`.

### Подтверждено повторным чтением текущего исходника

- Main editor создаётся с базовым размером 1024×700, разрешён resize 900×640…1600×1100; восстановленные размеры ограничиваются теми же пределами.
- Три области UI: preset browser, guitar engine/performance и AMP/FX rack; нижняя область содержит performance display, MIDI keyboard и status line.
- Все девять rotary sliders имеют APVTS `SliderAttachment`; FX toggles имеют APVTS `ButtonAttachment`; playback mode и sync mode подключены через `ComboBoxAttachment`. При открытии FX EDIT кнопка также вызывает соответствующую панель.
- Preset search/filter сохраняет выбранный preset, если он остаётся в отфильтрованном списке; фильтрация не должна незаметно загружать другой preset.
- MIDI keyboard отображает диапазон MIDI 36–84; performance display получает активную палитру.
- `applyPalette()` обновляет цвета главных labels, combo boxes, search, кнопок, tabs, rotary controls, FX toggles, performance display и workspace. В `ThemeManager.h` сохранена каноническая палитра из 21 semantic token, id `skin-053`, name `NEON STAGE`.
- Заголовок, workspace и основные панели остаются в границах UI-only слоя; APVTS IDs/values и audio engine этим аудитом не менялись.

### Замечания для следующего этапа проверки

1. Геометрия трёх колонок вычисляется отдельно в `paint()` и `resized()`. Сейчас формулы выглядят согласованными, но дублирование создаёт риск расхождения при будущих правках; не менять без подтверждения макетом/тестом.
2. Центральная область жёстко делит девять контролов на сетку 3×3. Не подтверждено, что это соответствует финальному эталону, поскольку PNG всё ещё не удалось прочитать.
3. Header/footer используют фиксированные высоты 48 px и 132 px, а resize ограничен минимумом 900×640. Нужны проверки на 100/125/150% DPI и реальном host resize; до них нельзя заявлять отсутствие clipping.
4. Визуальные состояния disabled/hover/focus частично заданы через LookAndFeel и semantic colours, но полного обхода всех состояний в работающем VST3 host не было.
5. `subtitleLabel` позиционируется абсолютными координатами относительно editor, а не header-контейнера; это остаётся точкой для resize/scale QA.
6. В текущем source readback не обнаружено оснований менять APVTS bindings или поведение пресетов в рамках этой темы. Любые такие изменения запрещены текущим UI-only scope.

### Решение этапа

**MAIN INTERFACE SOURCE AUDIT: обновлён и зафиксирован.** Кодовая интеграция общей палитры и APVTS-связи подтверждена чтением текущих файлов. **Визуальное соответствие NEON STAGE макетам, runtime skin discovery, DPI/resize, clipping, host behavior и визуальная приёмка остаются OPEN.** PNG-файлы присутствуют в GitHub, однако бинарное содержимое пока недоступно для фактического pixel/layout comparison через текущий коннектор. Следующий шаг по утверждённой очереди — AutoChords source audit; Main Interface остаётся открытым до visual/host QA.


## AUTOCHORDS — source audit и исправление минимальной геометрии

**Дата:** 2026-11-15  
**Исходник после исправления:** `Source/PluginEditor.cpp`, blob `7796376490426f4e891eb49fb21871ca65439c63`.  
**Контекст workspace:** `Source/UI/PPGUMultiWindowWorkspace.cpp`, blob `ab1dfff19983842261f5cf4113f66f66d9c76e69`.

### Подтверждено

- AutoChords — отдельная панель workspace, желаемый размер 900×620, минимальный размер окна 560×420.
- Существующие контролы Key, Scale/Quality и Rhythm Pattern Slot связаны с APVTS параметрами `scaleRoot`, `scaleMode`, `patternSlot` через ComboBoxAttachment.
- Генерация MIDI-аккордов, custom mode и preview engine явно помечены как NOT CONNECTED. Кнопка AUTOCHORDS OFF отключена и сообщает, что MIDI-generation engine ещё не реализован. Никакие фиктивные генерация/preview не добавлялись.
- Панель наследует PPGUThemeAwarePanel; setThemePalette обновляет заголовок, warning/status, подписи, кнопку и ComboBox через общие semantic tokens.
- Открытие AutoChords создаёт панель с ID 2102 в общем workspace.

### Найденная проблема геометрии и исправление

В `PPGUMultiWindowWorkspace.cpp` внутренний контент окна получает высоту окна минус 72 px (рамки, header и отступы). Поэтому при минимальном размере окна 560×420 доступная высота AutoChords content составляет около 348 px. Прежний вертикальный layout формы требовал 376 px — нижний блок Rhythm Pattern Slot мог выходить за пределы панели.

Исправлено только UI-расположение в `PPGUAutoChordsPanel::resized()`:
- при доступной высоте меньше 376 px включается компактная вертикальная раскладка;
- уменьшаются только вертикальные интервалы и верхние размеры заголовка/статусной строки/кнопки;
- размеры и APVTS-связи полей Key, Scale/Quality и Pattern Slot сохранены;
- при обычном размере 900×620 прежние интервалы остаются без изменений.

Расчёт compact layout: 334 px содержимого при примерно 348 px доступной высоты на минимальном размере окна. Это source-level геометрическая проверка, не runtime screenshot/host acceptance.

### Открытые проверки

- Визуальное соответствие AutoChords PNG-макету не подтверждено: бинарные пиксели эталонов всё ещё недоступны через GitHub connector.
- Не выполнены host resize, DPI 100/125/150%, clipping/focus/hover/disabled checks.
- Не выполнялись Windows build, CTest, pluginval или FL Studio 20 acceptance после этого изменения.
- Не менялись APVTS IDs/values, MIDI routing, генератор, sampler, scheduler или preset semantics.

**Статус:** AUTOCHORDS SOURCE AUDIT + COMPACT LAYOUT FIX COMMITTED / VISUAL QA PENDING / BUILD & HOST QA PENDING. Следующий модуль по утверждённой очереди — Curator; он остаётся заблокирован до готовности реальной функциональной части, поэтому вместо создания фиктивного UI выполняется аудит фактического наличия backend/контрактов.


## CURATOR — gate check (UI intentionally not created)

**Дата:** 2026-11-15  
**Проверенная ветка:** `rev18/neon-stage-skin1-implementation`, HEAD до этой записи `ec02a2744929e9b68306368fbb2e2bfd3e669057`.

### Проверка фактической готовности backend

- `PPGU_TASK_REGISTRY.md` фиксирует SH-02 DeepSearchWorker, SH-03 StyleClassifier и SH-04 CuratorPanel как **Not implemented**; SH-05 GREEN registry integration также Not implemented.
- В дереве `Source/` отсутствуют Curator/SoundHunter source modules и классы; есть только UI shell `Source/PluginEditor.cpp`, performance, sampler и общие UI components.
- Спецификация `docs/PPGU_REV_10_1_SOUND_HUNTER_RU.md` прямо помечена PRODUCT / SYSTEM DESIGN — DESIGNED, НЕ IMPLEMENTED.
- `assets/sound_hunter/registry.json` существует, но содержит пустой массив `assets: []`; реальных кандидатов и результатов кураторства нет.
- `tools/validate_sound_asset_registry.py` — metadata/policy validator. В docstring прямо указано, что он не выполняет legal review, downloads, guitar identification или audio-quality measurement.
- `tests/TestSoundAssetRegistry.py` тестирует правила GREEN/YELLOW/RED, provenance, hash и лицензии на фиктивных временных fixtures. Эти unit tests не являются реальным Curator service и не доказывают end-to-end curation.

### Решение

**CURATOR: BLOCKED BY BACKEND READINESS.** Интерфейс не создавался. Нельзя показывать фиктивные GREEN/YELLOW/RED verdicts, scan progress, confidence или curator approvals. Возврат к UI разрешён только после реализации и проверки DeepSearchWorker, StyleClassifier, реального per-asset evidence flow и интеграции GREEN registry. Это соответствует утверждённому порядку пользователя.

## SOUND HUNTER — source / integration audit

### Подтверждённое состояние

- На текущем HEAD в `Source/` нет отдельного Sound Hunter/DiscoveryEngine production module.
- `assets/sound_hunter/registry.json` имеет schemaVersion 1.0.0 и policy PPGU-REV-10.1, но `assets` пуст.
- SH-01 (исправить две упавшие sound registry CTest assertions) — Not implemented в task registry; SH-02 DeepSearchWorker и SH-03 StyleClassifier — Not implemented.
- Registry validator действительно проверяет метаданные, допустимые license basis, local file existence, SHA-256, provenance, screening fields и guitar classification claims. Однако валидатор не проводит само аудиоизмерение и не проверяет юридическую силу лицензии; он проверяет записанные evidence fields.
- Unit tests используют временный тестовый WAV fixture и примерные license files/URLs. Их pass не означает, что в Sound Hunter загружены реальные или лицензированные звуки.
- В спецификации предусмотрен внешний worker/companion boundary: сеть, HTTP, загрузки, hash/decode и screening должны идти вне audio callback/realtime voice path. Production worker и интеграция с SoundBank S6 пока не подтверждены.

### Блокеры и следующие действия

1. Сначала SH-01: получить фактический результат CTest, локализовать и исправить две failing assertions; статус не менять на pass без запуска тестов.
2. Затем реализовать/проверить worker, source adapters, pause/cancel/checkpoint, bounded downloads и quarantine согласно REV 10.1; никаких сетевых операций в audio callback.
3. Реализовать реальный guitar classifier и аудио-screening; metadata claims не заменяют аудиоанализа.
4. Только после этого связать per-asset provenance/license/hash/screening/classification/curator state с реальным registry и SoundBank S6.
5. Curator UI — после backend readiness; Auto Discover UI — после готовности discovery mechanism.

**Статус:** SOUND HUNTER BACKEND NOT IMPLEMENTED / EMPTY REGISTRY / UI NOT READY. Аудит не менял SoundBank, sampler, audio engine, presets или лицензии. Следующий модуль по утверждённой очереди — Auto Discover; он также должен оставаться без фиктивного scan UI, пока discovery mechanism не реализован.


## AUTO DISCOVER — backend readiness gate (UI не создавался)

**Дата:** 2026-11-15  
**Ветка:** `rev18/neon-stage-skin1-implementation`  
**HEAD до записи:** `9f54f5799abf130e6134963be1773239322d6d97`.

### Проверенные источники

- `PPGU_TASK_REGISTRY.md`, blob `9083fc0d995306783dbbf727e9204fc82783ecea`: SH-01…SH-05 и FR-02 (пополнение реестра легально очищенными assets) имеют статус Not implemented.
- `docs/PPGU_REV_10_1_SOUND_HUNTER_RU.md`, blob `d33b290aae900516d5d53050edf7a8056c86ee61`: DiscoveryEngine DEEP SEARCH и PROFILE CLASSIFY — запланированные режимы существующего S6.5; документ прямо говорит, что наличие production DiscoveryEngine в текущем HEAD не подтверждено.
- `Source/PluginProcessor.cpp`, blob `0e3e88fece554f82f0ca8d70fb01ea79be122eef`: найден только отдельный однопоточный `sampleLoadPool` для асинхронной загрузки выбранного SFZ. `loadSfzFileAsync()` принимает конкретный файл, передаёт его sampler и возвращает LoadReport в message thread. Это импорт/загрузка файла, а не поиск источников, discovery, классификация или заполнение каталога.
- `Source/PluginProcessor.h`, blob `d10e12163317bd9eacda876e24c6572682c1b594`: sample loader pool относится к SFZ loading; production DiscoveryEngine API не найден.
- Полное дерево `Source/` на текущем HEAD содержит PluginEditor/Processor, Sampler, Performance и UI, но не содержит DiscoveryEngine, AutoDiscover, DeepSearchWorker или StyleClassifier.
- `CMakeLists.txt`, blob `e6202fedf02391d01c6de1e834f8787c4f62b134`: зарегистрированы sampler/processor tests и sound asset registry contract; отдельного discovery engine/worker target или Auto Discover integration test нет.
- В UI source не найден действующий Auto Discover component. Наличие preset browser и SFZ import panel не является доказательством discovery-функциональности.

### Решение и безопасная последовательность

**AUTO DISCOVER: BLOCKED — discovery backend не реализован/не подтверждён.** Не создавал кнопку сканирования, фальшивый progress, счётчики N/target, найденные результаты или статус-карточки. Не переиспользовал SFZ loader как будто это discovery.

Перед UI требуются:
1. Закрыть SH-01 — исправить и фактически выполнить две failing sound registry CTest assertions.
2. Реализовать DiscoveryEngine S6.5 contract с режимами DEEP SEARCH и PROFILE CLASSIFY; не создавать второй параллельный importer.
3. Реализовать worker/companion boundary вне audio callback и realtime voice path: bounded concurrency, cancel/pause, checkpoint/resume, timeouts/retry, size/disk quotas и диагностические статусы.
4. Подключить реальные GREEN source adapters и asset-level provenance/license/hash/audio evidence; пустой registry не должен выдавать фиктивные результаты.
5. Добавить contract/unit/integration tests и доказательства отсутствия network/file I/O и блокировок в audio callback.
6. Только после backend readiness проектировать Auto Discover UI в общей NEON STAGE дизайн-системе и подключать к реальному состоянию discovery.

**Изменения кода в этом этапе:** не вносились. Это source/tree/spec audit и gate decision. Windows build, CTest и host acceptance не запускались. Следующий разрешённый этап — аудит следующего UI-модуля по очереди (FX EDIT) без изменения DSP/APVTS; backend-блокеры Sound Hunter остаются отдельно зарегистрированными.


## Уточнение названия и утверждённого объёма: AUTO SOUND HUNTER ≠ AUTO DISCOVER

**Дата:** 2026-11-15  
**HEAD перед записью:** `4f042a93055fbe1936870fc9ed180d4ddf6d969a`.

Зафиксировано уточнение пользователя: **AUTO SOUND HUNTER — ранее утверждённый функциональный модуль/направление; его нельзя подменять отдельным AUTO DISCOVER.** Предыдущая запись выше — аудит готовности AUTO DISCOVER и его backend gate, а не отмена и не переименование AUTO SOUND HUNTER.

Утверждённая функциональная основа AUTO SOUND HUNTER зафиксирована в `docs/PPGU_REV_10_1_SOUND_HUNTER_RU.md` (REV 10.1, DESIGNED):
- поиск бесплатных гитарных one-shots, single notes, power chords, chugs, harmonics, loops и preset/sample packs;
- Deep Search через единый DiscoveryEngine S6.5: source adapters, query synthesis RU/EN, pagination/checkpoint, file screening, download в quarantine, verification, conversion, index и атомарный commit;
- лицензии на уровне каждого asset: GREEN только при доказанной CC0/PD и прохождении всех gates; YELLOW — только ручной импорт; RED — исключить;
- SHA-256 дедупликация, provenance, source/asset ID, license evidence, retrieval date, file integrity и guitar-only gate;
- Style Classifier: PP-ROOT, PP-NEO2000, PP-MODERN, PP-EMO и UNCATEGORIZED; сомнительный материал не получает GUITAR_CONFIRMED автоматически;
- технический screening, copyright-suspect triage и обязательное человеческое прослушивание/кураторское одобрение перед GOLD;
- передача только прошедших проверку assets в существующий SoundBank S6 / Preset Browser через единый pipeline;
- поддержка pause/cancel, checkpoint/resume, bounded concurrency, timeouts/retry/backoff, size/disk quotas и диагностических статусов;
- связанные требования к Curator, curated stacks, attribution report и подсчёту только реально playable/validated presets.

**Разделение пользовательских точек входа:** SOUND HUNTER — поисково-кураторская подсистема и каталог гитарных звуков; AUTO DISCOVER и IMPORT GUITAR — отдельные пользовательские точки входа в согласованной архитектуре S6.5. Сходство backend не делает эти названия взаимозаменяемыми.

**Статус честно:** функционал AUTO SOUND HUNTER утверждён на уровне PRODUCT/SYSTEM DESIGN; в REV 10.1 он обозначен DESIGNED, а production DiscoveryEngine, DeepSearchWorker, StyleClassifier, GREEN adapters, asset registry и автоматическое пополнение SoundBank в проверенном HEAD пока не подтверждены как реализованные. Утверждение функционала не равно доказательству готового кода. Нельзя удалять или урезать утверждённые функции; требуется поэтапно реализовать их и тестировать по H9 acceptance tests.

**Следующий аудит:** отдельно проверить AUTO SOUND HUNTER по полному утверждённому функционалу REV 10.1 и фактическому дереву исходников. AUTO DISCOVER остаётся самостоятельным пунктом плана. Не создавать фиктивный UI и не менять Audio Engine/DSP/MIDI/APVTS/control IDs.

## УТВЕРЖДЕНО — пользовательский workflow браузера MELODY / PATTERN (2026-11-15)

Пользователь уточнил обязательную логику переключения режимов и компоновки Preset Browser. Это функционально-компоновочный контракт для связки Sound Hunter / Preset Browser; он не считается реализованным только потому, что описан здесь.

### Режим MELODY

При нажатии кнопки **MELODY** открывается всплывающий Preset Browser с тремя отдельными окнами/колонками:

1. **PRESETS** — список пресетов.
2. **STYLE** — стили/варианты артикуляции.
3. **PATTERN** — ритмические паттерны.

Три области должны быть видимы одновременно и работать как связанные уровни выбора: выбор пресета, стиля артикуляции и ритмического варианта. Не подменять STYLE списком жанров или фиктивными параметрами; конкретные варианты должны поступать из реальных данных каталога.

### Режим PATTERN

При нажатии кнопки **PATTERN** открывается всплывающий Preset Browser с двумя основными окнами:

1. **PRESETS** — список паттерн-пресетов.
2. **STYLE / PATTERNS** — объединённая область выбора стиля и ритмических паттернов.

Над основным браузером располагается отдельная дополнительная панель/область с режимом **AutoChords** в верхней части. Это дополнительный уровень управления над браузером, а не третья колонка внутри него.

### Общие требования к поведению

- MELODY и PATTERN — два явно переключаемых режима; каждый открывает собственную описанную компоновку браузера.
- Открытие браузера не должно сбрасывать выбранный пресет или менять его без действия пользователя.
- Выбранные PRESETS / STYLE / PATTERN должны быть согласованы между собой и отражать реальные доступные данные.
- AutoChords в режиме PATTERN показывается как отдельная верхняя панель; само наличие панели не означает, что MIDI-generation/preview backend уже готов. До реализации backend нельзя показывать фиктивную генерацию, preview или успешный статус.
- Все элементы оформляются общей NEON STAGE DEFAULT / SKIN-1 дизайн-системой, но сохраняют функциональную компоновку каждого режима.
- Счётчики и доступность PATTERN-пресетов должны соответствовать REV 10.1 H7: учитывать только реально playable/validated данные и рабочий tempo-sync/pattern playback; WAV loop сам по себе не является доказательством playable PATTERN.
- Не менять APVTS IDs/values, MIDI routing, sampler, scheduler, preset semantics или audio engine в рамках визуальной интеграции без отдельного утверждённого backend-плана.

**Статус:** USER-APPROVED INTERACTION / LAYOUT CONTRACT — RECORDED; SOURCE IMPLEMENTATION AND RUNTIME BEHAVIOUR NOT YET VERIFIED. Следующий аудит должен сопоставить этот контракт с существующими MELODY/PATTERN controls, preset data model и реальным playback path; не создавать фиктивный браузер или неработающие элементы.

