# NEON STAGE — единая дизайн-система полного интерфейса PPGU 4.0

**Назначение:** единый визуальный контракт для главного окна, всех вспомогательных окон, панелей, кнопок, ручек, селекторов, браузеров, списков, папок, индикаторов и состояний POP PUNK GUITAR ULTIMATE 4.0.

**Статус:** DESIGN CONTRACT / READY FOR COMPONENT-BY-COMPONENT IMPLEMENTATION. Документ определяет оформление и правила отображения; он не объявляет отсутствующие функции реализованными.

## 1. Единственная визуальная идентичность
- Имя всей графической системы: **NEON STAGE**.
- NEON STAGE — одна общая оболочка продукта: main GUI, все дочерние окна, панели, меню, диалоги, Gallery, Browser, Import, FX Edit, Settings, AutoChords и будущие окна.
- Не вводить второе название оболочки, отдельный бренд интерфейса или конкурирующую тему.
- Канонический пользовательский label: **NEON STAGE**; технический ID default skin: `neon-stage-default-skin-1`.
- Skin изменяет только визуальное представление. Запрещено менять DSP, audio callback, APVTS IDs/semantics, sampler, presets, MIDI, transport, articulation, automation или project state.

## 2. Визуальное направление
- База: глубокий чёрный, midnight graphite и тёмный металлический корпус.
- Свет: холодный electric cyan/blue как главный сигнал, restrained violet и magenta как вторичные сценические акценты; никаких сплошных ярких заливок.
- Панели: многослойные graphite surfaces, тонкие металлические кромки, аккуратные разделители, мягкая глубина.
- Контролы: аппаратные rotary с читаемой шкалой, металлическим ободом и световым указателем; кнопки с ясными OFF/ON/FOCUS/DISABLED состояниями; селекторы и текстовые поля с единым корпусом.
- Hero artwork: широкая концертно-гитарная зона; изображение является брендинговым слоем и не перекрывает интерактивные элементы.
- Glow используется только для активного состояния, фокуса и реальной активности. Не анимировать фиктивную аудиоактивность.
- Не использовать чужие логотипы, скриншоты, фирменные шрифты, фотографии или копии UI других продуктов.

## 3. Палитра — фиксированные 21 semantic tokens
Все цвета JUCE-компонентов и кастомной графики берутся только из ThemeManager tokens. Никаких новых hard-coded hex в PluginEditor.

| Token | Роль в NEON STAGE |
|---|---|
| `window_bg` | внешний фон окна / рабочая сцена |
| `panel` | базовые панели и секции |
| `panel_elevated` | приподнятые карточки, панели и активные области |
| `control_bg` | поля, селекторы, неактивные кнопки |
| `control_face` | металлическая поверхность ручек и переключателей |
| `control_border` | рамки, делители, шкалы и контуры |
| `text_primary` | главные подписи, названия пресетов, значения |
| `text_secondary` | вторичные подписи и подсказки |
| `text_disabled` | только реально недоступные функции |
| `accent` | основной cyan/blue акцент и активные элементы |
| `accent_glow` | мягкий ореол и hover/focus highlight |
| `danger` | ошибки, destructive action, invalid asset |
| `warning` | pending, setup-only, missing dependency |
| `success` | подтверждённая загрузка/готовность |
| `lcd_bg` / `lcd_text` | дисплеи статуса, значения и читаемые readouts |
| `led_on` / `led_off` | индикаторы только при наличии реального источника состояния |
| `shadow` / `highlight` / `lowlight` | глубина, блик и нижняя кромка материалов |

Контраст: соблюдать пороги G1 Skin Studio contract. Не снижать контраст ради декоративного свечения. В ECO mode свечение и декоративная анимация уменьшаются/отключаются.

## 4. Типографика и сетка
- Основной UI font: Segoe UI или системный sans-serif fallback; display/readout: Consolas или моноширинный fallback.
- Иерархия: product/title → panel heading → control label → value/readout → helper/status.
- Короткие uppercase labels допустимы для аппаратных секций; описания и ошибки пишутся читаемо, без чрезмерного tracking.
- Основной reference viewport: 1402×1122. Сетка масштабируется, сохраняет hit targets, не обрезает названия, значения и нижнюю клавиатуру.
- Минимальная геометрия и существующие bounds/hit areas не меняются ради косметики. Grid metadata не считается runtime layout, пока layout engine реально не применяет её.
- Поддержать resize, сохранение размеров, DPI scaling, compact/overflow и keyboard focus; элементы не должны перекрываться.

## 5. Главный экран — зоны A–H

### A — TOP BAR / PRODUCT HEADER
Единая верхняя панель: PPGU mark, имя продукта, предыдущий/следующий preset, текущее имя/категория, Favorite/Save (только после реального user-state implementation), поиск/статистика, SKIN, Import Guitar, Auto Discover, Curator, Settings, About, оконные действия. Не реализованные действия показывать как disabled с понятным tooltip или не показывать до реализации; не рисовать активные заглушки.

### B — PRESET BROWSER
Общий browser surface: вкладки BROWSER / FAVORITES / CATEGORIES, поиск, фильтры, список/карточки, artwork, имя, категория, описание и реальные counts. Папки/банки отображать как дерево с раскрытием, ясными иконками, текущим путём и состояниями пусто/загрузка/ошибка/готово. Не показывать придуманные counts, artwork, лицензии или READY badges. Поиск/фильтр не должен менять звучащий preset до явного выбора.

### C — PLAYING CORE
Центральная рабочая область: MELODY / PATTERN / PUNK GROUP как отдельные режимы согласно реальной реализации; hero area; текущий preset/instrument; Electric/Acoustic/Hybrid selector только при наличии соответствующего bank; девять основных rotary controls с неизменными назначениями:
1. GAIN → `ampGain`
2. TREBLE → `eqHigh`
3. MID → `eqMid`
4. BASS → `eqLow`
5. VELOCITY → `velocity`
6. DYNAMICS → `dynamics`
7. SWING → `swing`
8. HUMANIZE → `humanize`
9. ACCENT → `accent`
Отсутствующий PUNK GROUP не имитировать визуально как работающий режим.

### D — FX CHAIN
Панель цепи с реальными состояниями bypass/on для семи текущих slots: Compressor, EQ, Distortion, Chorus, Delay, Reverb, Cabinet IR. Каждая карточка/педаль: название, power state, краткий readout и вход в FX Edit. Flanger и другие дополнительные эффекты не показывать как готовые активные слоты до проверки DSP routing и соответствующего binding.

### E — AUTOCHORDS
Отдельная гармоническая панель: Key, Scale/Quality, Chord Mode, Pattern, Speed, Current Chord, progression pads, fretboard, Preview/Apply/Back/Options — только в объёме подтверждённой функции. Пока MIDI generation не интегрирован, на панели обязателен честный статус SETUP ONLY / MIDI GENERATION NOT CONNECTED; кнопки генерации disabled.

### F — FX EDIT
Единое окно редактора эффектов: список реально подключённых DSP modules, power/bypass, preset (если существует), параметры с units/ranges, значения, meters/graphs только от реального DSP telemetry. Визуально единый rack: header, effect selector, parameter grid, response/meter zone, status. Текущая панель содержит группы Compressor, EQ, Distortion, Chorus, Flanger, Delay, Reverb, Cabinet IR; это не подтверждает целевой release scope 10–15 готовых DSP processors.

### G — AMP & TONE
Аппаратный amp/tone rack: модель усилителя, Gain/Bass/Mid/Treble, output/level, cabinet/IR controls — только если соответствующие параметры реально подключены к DSP. Визуальные ручки не могут существовать без APVTS/action binding.

### H — PERFORMANCE / KEYBOARD
Нижняя зона: реальная MIDI keyboard activity, note/chord/key readouts, pitch/mod wheels, velocity/sustain/transport/tempo/sync/volume/status. Клавиши подсвечиваются только при реальных MIDI note-on/held state и корректно гаснут на note-off. CPU/RAM meters — только измеренные значения; иначе скрыть их или честно показать NOT AVAILABLE, не подставлять demo numbers.

## 6. Визуальные стандарты компонентов
- **Rotary:** металлический face, outer scale, active arc, pointer, центральный cap, label, value/unit; hover/focus/drag/disabled. Значение доступно для чтения и host automation.
- **Toggle / power:** отчётливые ON/OFF, focus, disabled; активность соответствует реальному APVTS state.
- **Push button:** primary, secondary, destructive, utility styles; нажатие/hover/focus/disabled; короткий verb label.
- **ComboBox / selector:** одинаковая высота, рамка, arrow, selected/hover/focus/disabled.
- **Text/search:** видимый placeholder, clear/search affordance только при наличии action, keyboard focus.
- **Tabs:** mutually exclusive state, active underline/filled state, focus ring; режим меняется только через реальный callback/APVTS.
- **LCD/readout:** моноширинные цифры, фиксированные units, clipping-safe, real-time data source only.
- **LED/meter:** OFF/ON/peak/clip состояния от реального источника; fake pulsing запрещён.
- **Folders/files:** folder/file/selected/expanded/loading/invalid/missing/locked states; drag-drop target с явной зоной и текстом, если drag-drop реализован.
- **Cards/tiles:** preview, name, family/category, current marker, selection; artwork lazy-loaded и безопасно кэшируется.
- **Keyboard:** реальные pressed/hover/focus, octave labels, clear note range; pitch/mod wheel controls должны быть functional, не декоративными.
- **Dialogs/menus/tooltips:** единая типографика, отступы, фокус, клавиатурная навигация и ясные confirm/cancel/error states.

## 7. Все окна и поверхности — одна тема
1. Main PluginEditor — главный NEON STAGE shell.
2. Preset Browser / Library — browser surface и дерево папок.
3. Skin Gallery — preview cards, search, family filters, active/default badges; будущий transaction flow Preview → Apply / Back rollback, Favorites и keyboard navigation.
4. Import Guitar — drag/drop zone, Browse, SFZ/WAV map summary, sample preview, license/provenance and checksum status, Load/Cancel, progress/error/result.
5. FX Edit — общий FX rack design.
6. AutoChords — harmony setup; generation controls disabled до интеграции.
7. Settings — workspace scale/gap, restore NEON STAGE, Gallery; будущие разделы Engine/Audio/MIDI/Library добавлять только с реальными настройками.
8. Performance — read-only telemetry; без hard-coded values.
9. Auto Discover — будущая discovery surface; не отображать как рабочую, пока pipeline не интегрирован.
10. Curator — будущая GREEN/YELLOW/RED review surface; не отображать как рабочую до production implementation.
11. About — продукт, версия, build, licenses/notices; показывать только фактические сведения.
12. Workspace/overlays — общий title bar, close, focus/z-order, resize/maximize/restore, saved bounds, DPI/work-area safety.

Каждое окно наследует текущую NEON STAGE palette и общие component styles. Смена skin не меняет функциональность, не сбрасывает параметры, не закрывает несвязанные окна и не меняет project/preset state.

## 8. Статус реальной реализации (не путать с дизайном)

| Поверхность | Фактический статус по source audit | Правило оформления |
|---|---|---|
| Main shell / theme palette | SOURCE PRESENT / PARTIAL | Оформлять существующие компоненты; полный approved-layout parity ещё не подтверждён |
| Preset selector/category/search | SOURCE + BINDING | Полноценный browser/card/artwork flow частичный |
| MELODY/PATTERN | SOURCE + BINDING | PUNK GROUP не активировать до реализации |
| 9 rotary / 7 FX toggles | SOURCE + APVTS ATTACHMENTS | Сохранить ID и порядок |
| MIDI keyboard 36–84 | SOURCE + BINDING | Подсветка только от реального MIDI state |
| FX Edit | SOURCE / PARTIAL | Параметры привязаны; DSP completeness требует отдельного аудита |
| Import Guitar | SOURCE + ASYNC SFZ PATH | Не подменять loader; отображать реальные результаты |
| Skin Gallery | SOURCE / PARTIAL | Cards/search/family/active; транзакционный Preview/Apply/Back и Favorites ещё не завершены |
| Settings | SOURCE / UI-ONLY | Не заявлять полный engine/audio settings |
| AutoChords | SETUP ONLY / DISABLED BY DESIGN for generation | Не рисовать активную генерацию MIDI |
| Auto Discover / Curator / About | NOT IMPLEMENTED | Не показывать активные фальшивые кнопки |
| Pitch/Mod wheels, full telemetry | MISSING / PARTIAL | Не рисовать функциональные контролы без источника/связи |
| OverlayManager lifecycle | PARTIAL / NOT ACCEPTED | Требуются focus/z-order/lifetime/resize tests |

## 9. Обязательный component ledger
Для каждого отображаемого элемента поддерживать запись: `control_id` → окно/панель → JUCE component → callback/action или APVTS parameter → диапазон/единицы → persistence → automation/MIDI → keyboard/focus → состояния → тест → статус.

Правила:
- Каждый активный control имеет реальную функцию и доказуемую связь.
- Неактивная/отложенная функция обозначается как disabled/deferred с причиной.
- Не удалять важную функцию из интерфейса молча; сначала проверить другие окна и записать решение.
- Skin grid coordinates — design metadata, пока layout engine не потребляет их в runtime.
- Любое изменение только визуальное; аудио/MIDI/APVTS invariance обязательна.

## 10. Приёмка NEON STAGE
1. Проверить точную связь утверждённых mockups 01–07 с зонами A–H; оригиналы не менять.
2. Component-by-component matrix: required / source exists / binding / persistence / automation / tested / missing.
3. Визуальная проверка на 1402×1122, затем resize, DPI, compact и overflow.
4. Проверить каждое окно, все кнопки, rotary, selectors, tabs, folders, lists, tooltips, menus, dialogs, disabled/error/loading/ready states.
5. Проверить реальную клавиатурную реакцию, focus, keyboard navigation и accessibility.
6. Audio invariance: до/после смены темы сравнить выходной буфер на одинаковом deterministic input; нулевые функциональные изменения.
7. Официальные Skin Studio G1–G4 и full registry validators на exact SHA.
8. Windows x64 Release build, полный CTest, pluginval strictness 5 — каждый на exact SHA.
9. FL Studio 20 manual host acceptance и CPU/RAM/audio profiling фиксируются отдельно.
10. Staging ZIP не является release package.

**Definition of Done:** единая NEON STAGE оболочка визуально согласована во всех окнах и отображает весь обязательный интерфейс без потери функций; отсутствующие backend features не маскируются; все активные controls имеют реальные bindings; validators/build/tests/host gates пройдены и задокументированы.