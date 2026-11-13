# SIGNAL FORGE / NEON — АУДИТ ФАЗЫ 0 (сводный, сверен с журналом INFO)

**Ветка:** `work/signal-forge-panel-shell`
**Родительский SHA ветки:** `a669c9b067aa0ab36d0bc090f10821af66671d29`
**Последний подтверждённый CI-SUCCESS:** run #1162 / ID `43226106697` на SHA `d98794f92771df23567c8b02d6b679f863e11c88` (Windows x64 Release configure/build, полный CTest, VST3 locate, pluginval strictness 5, COMPLETE package — всё PASS).
**Более поздние коммиты:** CI на точном SHA не подтверждён.
**Дата сводки:** 13 ноября 2026.
**Источник фактов:** журнал `Info.txt` (единственный журнал проекта) + предыдущие находки аудита.

---

## ГЛАВНОЕ ПРАВИЛО

1. Ничего не переписываем до завершения аудита фазы 0.
2. Статусы: Р — реализовано, Ч — частично, О — отсутствует, И — требует исправления.
3. Стандарт SIGNAL FORGE / NEON неприкосновенен. Никаких самовольных упрощений и возврата к старому дизайну.
4. PLANNED/DESIGNED не называть IMPLEMENTED; исходники не называть BUILT без сборки; тесты не называть TESTED без результата; DAW не называть HOST-VERIFIED без проверки в хосте.
5. PASS не переносится с одного SHA на другой: каждый новый коммит требует своего CI-прогона.

---

## ОБЯЗАТЕЛЬНОЕ ПРАВИЛО РОЛЕЙ ТЕМ (подтверждено пользователем, 2026-11-13)

- **SIGNAL FORGE / NEON — единственная основная тема продукта.** Главный визуальный стандарт, целевая тема DEFAULT, startup и fallback.
- **BLACKOUT VOLTAGE (`skin-51`) — только дополнительная тема.** Не DEFAULT, не startup, не fallback. В галерее остаётся самостоятельной дополнительной темой.
- Правило действует во всех разделах: код, ThemeManager, manifest, реестр, меню/галерея, документация, тесты, сборка, release notes.
- `skin-51`, имя Blackout Voltage, папка `Skin_51` и все ресурсы сохраняются как есть: не удалять, не переименовывать, не перенумеровывать.
- Исправление конфликтующего default/fallback **отложено до окончания аудита фазы 0 и проверки готовых NEON runtime-ресурсов. До окончания аудита код и манифесты не менять.**

---

## СВЕРКА: ПРЕЖНЯЯ ТАБЛИЦА АУДИТА ↔ ЖУРНАЛ

| № | Направление | Статус (аудит) | Что подтверждает журнал | Поправка к статусу |
|---|---|---|---|---|
| 01 | Project Structure | Р / И | Структура модульная; найден дубликат ресурса (Skin_052 вне legacy registry — уже устранено merge'ем, реестр = 52 темы); выявлены divergent-ветки (`feature/auxiliary-panel-layout`, `work/integrate-uploaded-theme-manager`, `ui/modern-neon-stage-shell`, `feat/theme-system-20-full-skins`, `feature/m4-autochords-foundation`, `feature/pattern-melody-fx-spec`) — только изолированный перенос файлов | Р / И: добавлен риск ветвления |
| 02 | GUI Architecture | Ч / И | Панели SIGNAL FORGE staged (EditorOverlayPanel, FXEditPanel, AutoChordsPanel, ImportGuitarPanel, SettingsPanel, PerformanceDisplay), переведены на 21-token palette; оконный менеджер: до 6 окон на страницу, PAGE n/N, expanded-dock layout, восстановление состояния панелей; FX strip расширен до 8 (Flanger); исправлен fxAttachments 7→8; AutoChords neon mockup reconstruction committed | Ч / И: подтверждено, прогресс значительный |
| 03 | Theme System | И | **Подтверждено журналом.** Ранний аудит (SKIN_ENGINE_GAP_AUDIT_RU) зафиксировал hardcoded цвета в PluginEditor; затем на ветке `work/signal-forge-theme-tokens` введён canonical 21-token API `token(name)` (window_bg … lowlight), legacy skins 1–51 маппятся в tokens, Skin Studio themes загружаются полностью; ThemeManager test проверяет все 21 токен Skin_052 | Ч / И: token API реализован на ветке, CI pending; правило ролей тем (см. выше) зафиксировано, исправление DEFAULT/fallback отложено |
| 04 | Auxiliary Windows | Р / Ч | **Подтверждено журналом.** Дизайн-базилика: docs/AUXILIARY_PANEL_MANAGER_DESIGN_RU.md; geometry module AuxiliaryPanelLayout + тесты (0/1/2/3/4/6 панелей, неперекрытие, compact, overflow paging, DPI, детерминизм); hardening: CHECK-механизм вместо assert (NDEBUG в Release), защита от переполнений; expanded panel layout contract; независимое закрытие, square maximize/restore, сохранение состояния | Р / Ч: geometry READY-level, window shell/manager реализован на ветке, CI на свежем SHA pending |
| 05 | Persistence | Ч / И | Журнал добавляет объём: сохраняются выбранная тема (user settings), состояние открытых панелей и их bounds (восстановление при следующем открытии), expand/restore синхронизируется между панелями; тест на сохранение состояния панелей запланирован | Ч / И: охват Persistence шире, чем предполагали |
| 06 | Parameters | Ч / И | APVTS: 9 rotary с точными ID/диапазонами (ampGain 0..10 def 5; eqHigh/Mid/Low −12..12 def 0; velocity 0..2 def 1; dynamics 0..1 def 0.5; swing 0..1 def 0; humanize 0..1 def 0; accent 0..1 def 0.75); 7 FX bool → 8 после Flanger (`flangerOn`, новый ID не добавлялся); все 8 переключателей имеют собственные ButtonAttachment | Ч / И: «7 FX» устарело — стало 8 |
| 07 | Audio Engine | — | Sampler/StreamPool: 64 voice slots, 128 snapshots, 8 streaming roots, атомарная публикация snapshots; открытые риски (use-after-free, worker recovery, stress 64 voices) зафиксированы в INFO; Pattern Scheduler есть, runtime-приёмка не подтверждена | Ждёт аудита; у журнала есть детальный чек-лист |
| 08 | Resource Management | — | WAV metadata index / page streaming / fixed page cache реализованы; SoundHunter REV 17: policy validator + 16 unit cases, registry пуст; full pipeline PENDING; следующий шаг — проверка 8-root cap и stop/drain contract | Ждёт аудита |
| 09 | Tests | — | История: run #827 (12/12 + pluginval), #860 (11/12 — Skin_052 вне реестра, уже исправлено merge'ем), #1162 (полный PASS) | Последний известный PASS — #1162; каждый новый SHA требует нового прогона |
| 10 | Build System | — | JUCE 7.0.12 / CMake 3.22 pin против требования REV 13 §13.2 (JUCE 8 / CMake ≥3.24) — известное расхождение, обновление отдельным контролируемым этапом; BinaryData gate (check_binary_data_links.py) перед configure; CMake include(CTest) порядок исправлен (a686f627) | Ждёт аудита |

---

## КЛЮЧЕВЫЕ УТОЧНЕНИЯ К ПРЕЖНИМ ВЫВОДАМ АУДИТА

### 1. Две системы тем — вывод скорректирован
Ранее мы фиксировали риск «LookAndFeel против ThemeManager — два источника истины». Журнал уточняет реальную картину:
- Ранний gap-аудит: PluginEditor вообще не использовал ThemeManager, цвета были hardcoded.
- Затем последовательно: palette switching foundation (SKIN-кнопка, popup, Ctrl+K, сохранение skin ID, sibling `Skins/` resolver) → Skin Studio 21-token loader → canonical `token(name)` API на ветке `work/signal-forge-theme-tokens` → перевод текущих элементов редактора на token API, удаление hardcoded pink/pale/muted цветов.
- Вывод: направление верное, infrastructure уже есть на ветке; осталось подтвердить CI и проверить полноту перевода всех контролов (GUI-005).

### 2. Neon-макеты — это целевой дизайн, а не текущий рантайм
Скриншоты SIGNAL FORGE / NEON (AutoChords, FX EDIT, Skins Gallery, Import Guitar и т. д.) — эскизы целевого интерфейса. Журнал подтверждает: AutoChords neon mockup visual reconstruction committed на ветке; AutoChordsPanel — **UI only, все контролы и Preview отключены до реального backend** (progression/MIDI). Вкладка PUNK GROUP отсутствует в текущем `playMode` (только MELODY/PATTERN); Auto Discover, Curator, Settings, About — без полной runtime-функции. LCD_display пока отображается через status label.

### 3. AutoChords: root cause зафиксирован, «исправлено» не объявляется
Аудит на SHA `d98794f92771df23567c8b02d6b679f863e11c88` (CI run #1162 SUCCESS) подтвердил: AutoChords в MELODY функционально не реализован. Требуется отдельная реализация backend (AutoChordEngine → progression/MIDI) и профильные integration tests. Успешная общая сборка не считается исправлением.

### 4. Persistence — реальный охват шире
Журнал подтверждает сохранение/восстановление: выбранной темы (skin ID в user settings), состояния открытых панелей, их bounds, expand/restore-состояния. Плюс базовая сериализация состояния процессора в XML (`getStateInformation`/`setStateInformation`, `parameters.copyState/replaceState`, `sfzFilePath` с асинхронной перезагрузкой). Чек-лист проверок сохраняется: атомарность `replaceState`, `MessageManagerLock` у `copyState`, гонка с незавершённой загрузкой SFZ, поведение `sfzFilePath` при отсутствии файла, smoke-тест двух хостов.

### 5. FX: 7 → 8, идентичность модулей
- FX strip расширен до 8 переключателей: Compressor, EQ, Distortion, Flanger, Chorus, Delay, Reverb, Cabinet IR. Flanger использует существующий `flangerOn`; новые APVTS ID не добавлялись.
- Исправлена несовместимость `fxButtons` (8) / `fxAttachments` (7) — теперь 8/8.
- Утверждённое правило визуальной идентичности FX: Distortion — зелёный, Compressor — янтарный, EQ — cyan, Flanger — electric blue, Chorus — violet, Delay — orange, Reverb — teal, Cabinet — steel/metal. Не одинаковые универсальные панели с заменой названия; при этом все модули в единой премиальной системе и подчиняются ThemeManager.
- Каталог FX принят: Overdrive, Distortion, Fuzz, Delay, Reverb, Chorus, Flanger, Phaser, Tremolo, Wah-Wah, Compressor, EQ, Pitch Shifter/Octaver, Envelope Filter/Auto-Wah (14 модулей = каталог типов, не одновременные 14 insert-слота; существующие 7→8 slot IDs не менять без MAJOR-решения).

### 6. Хост-приёмка FL Studio 20 (user-reported smoke tests)
Подтверждены два факта: (1) VST3 открывается в FL Studio 20; (2) MIDI-клавиатура воспроизводит звук гитары. Это не полный QA: note-off/choke, полифония, preset recall, automation/state restore, длительная стабильность, CPU/RAM — остаются в acceptance checklist.

### 7. R28 boundary — граница аудита
Аудит не меняет DSP/MIDI/APVTS/preset/Sampler lifecycle. UI-изменения живут в своей области. Перед любой правкой сверять активную ветку, HEAD, реальные APVTS IDs, DSP signal path, FX rack mapping, ThemeManager tokens, skin schema и validators.

---

## СЛОВАРЬ NEON (визуальная спецификация v1 — для сверки GUI-001/GUI-005)

Семь конструктивных зон: хедер (лого + навигация пресетов + поиск + 6 быстрых кнопок) / левая боковая зона / центральная рабочая зона (баннер, MELODY/PATTERN/PUNK GROUP) / правая рейка (LED, AMP & TONE) / оверлей-панели (единая анатомия: иконка + заголовок слева, X справа) / нижняя зона (PITCH/MOD + MIDI-клавиатура) / статус-бар.

Словарь контролов (~15 типов): неон-кнопки (заливка+глоу / контур / призрачная), пилюли-тумблеры, металлические ручки с синим дуговым кольцом, неон-слайдеры, сегментные переключатели, дропдауны, списки строк с эмблемой и звездой, табы двух видов, поисковые поля, графики на сетке, вертикальные GR/LED-метры, дисплей аккорда с грифом, мини-осциллограмма, плитки галереи тем, Reset to Default.

Палитра (оценка по эскизу): подложка почти чёрная с синевой; панели — тёмный полупрозрачный navy; акцент — электрик-синий/циан с глоу; вторичные — маджента/фиолетовый в артах; золото — только брендинг и звёзды; зелёный/жёлтый — статусы. Точные HEX берутся из темы, не из скриншота.

Соответствие token API (21 токен): window_bg, panel, panel_elevated, control_bg, control_face, control_border, text_primary, text_secondary, text_disabled, accent, accent_glow, danger, warning, success, lcd_bg, lcd_text, led_on, led_off, shadow, highlight, lowlight — это мост между словарём макета и кодом.

**Несоответствие, которое надо отследить:** черновые HEX-оценки макета NEON не должны «победить» 21-token модель. Проверка: каждый контрол читает цвет только через `token()`, никаких локальных hex.

---

## РЕЕСТР РИСКОВ GUI (обновлён)

| ID | Риск | Приоритет | Статус по журналу |
|---|---|---|---|
| GUI-001 | Соответствие GUI стандарту SIGNAL FORGE / NEON | Критический | Открыт; макеты = цель, текущий рантайм — палитровая переработка; сверка словаря ↔ token API |
| GUI-002 | Незавершённые функциональные разделы | Высокий | Подтверждён журналом: AutoChords UI-only, PUNK GROUP отсутствует, Auto Discover/Curator/Settings/About не реализованы |
| GUI-003 | Жизненный цикл доп. окон | Высокий | Частично закрыт: SafePointer/lifetime/teardown зафиксированы в дизайн-базилике; реальная parity-проверка всех панелей не завершена |
| GUI-004 | Сохранение выбранной темы | Высокий | Частично закрыт: skin ID сохраняется в user settings; конфликт DEFAULT/fallback (Blackout Voltage) зафиксирован правилом ролей тем, исправление отложено до конца аудита |
| GUI-005 | Единообразие элементов управления | Высокий | Открыт; сверка «каждый контрол → token()» на ветке theme-tokens, CI pending |
| GUI-006 | Соответствие размеров окон утверждённой компоновке | Высокий | Частично закрыт: AuxiliaryPanelLayout + expanded layout contract + compact/overflow paging реализованы; полная проверка в рантайме pending |

**Новые риски из журнала:**
- RISK-B1: расхождение pin'ов JUCE 7.0.12 / CMake 3.22 против REV 13 §13.2 (JUCE 8 / CMake ≥3.24) — обновление отдельным этапом с полной регрессией.
- RISK-B2: CTest history показывает чувствительность к registry consistency (Skin_052 вне реестра ломал упаковку) — любое добавление темы требует синхронного обновления registry/schema/validator.
- RISK-B3: тесты на assert в Release (NDEBUG) — исправлено на CHECK-механизм; при добавлении новых тестов использовать CHECK, не assert.

---

## НЕПОДТВЕРЖДЁННОЕ (без отметки «ГОТОВО»)

- Theme Gallery: preview/hero decoding, card layout, search/family filters/favorites, dynamic rescan, hot-swap fade 150 ms — НЕ реализовано полностью.
- Runtime skin switching визуально работает (palette switching), но полная skin-to-component/control parity не завершена.
- Windows Release / CTest / pluginval / packaging на свежих SHA ветки — pending.
- FL Studio 20 полный host acceptance — только smoke-факты.
- CPU/RAM profiling и playable count 300 пресетов — не подтверждены.
- AutoChords backend, FX-модули (14 каталог), Cabinet IR workflow, Looper/Tuner — REQUIREMENTS ACCEPTED, реализация по этапам.

---

## ПОРЯДОК ДАЛЬНЕЙ РАБОТЫ

1. Завершить инвентаризацию фазы 0 по ветке (файлы 02-направления: PluginEditor.h/.cpp, ThemeManager.h/.cpp, AuxiliaryPanelWindowManager.h/.cpp, AuxiliaryPanelLayout.h/.cpp, семь доп. разделов).
2. Сверка словаря NEON ↔ token() API по каждому контролу (GUI-005), включая 8 FX-модулей с индивидуальной идентичностью.
3. По результатам — согласовать исправление default/fallback (правило ролей тем) и только после этого менять код/манифесты.
4. Затем: CI на точном SHA, полный release gate, FL Studio acceptance.
5. После каждого этапа — запись в Info.txt с датой, SHA, статусом и фактическим результатом.

**Материалы, которые нужны дальше:** исходники из списка п. 1; свежий лог GitHub Actions для HEAD ветки; текущий `theme_manifest.json`.

---

*Сводный документ. Всё внесено из фактически полученных данных (аудит + журнал INFO). Предположения помечены явно (например, HEX-оценки палитры). PASS не переносится между SHA.*
