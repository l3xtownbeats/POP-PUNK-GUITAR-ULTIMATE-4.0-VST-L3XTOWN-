# PPGU SIGNAL FORGE v7 MASTER — ACCEPTED IMPLEMENTATION STANDARD

**Дата принятия:** 2026-11-12  
**Продукт:** POP PUNK GUITAR ULTIMATE / REV.17  
**Целевая платформа:** Windows 10 x64 · VST3 · JUCE 7.0.12 · C++20 · FL Studio 20  
**Статус:** ACCEPTED REQUIREMENT / IMPLEMENTATION NOT IMPLIED

Этот документ фиксирует обязательный стандарт разработки Skin System. Принятие спецификации не является доказательством реализации, сборки, тестирования или host verification.

## 1. Конституция

- Skin / theme / оболочка — один визуальный артефакт; канонический термин в коде и тестах: **skin**.
- Один VST3 и одна функциональная архитектура; несколько сменных skins. Ни один skin не имеет отдельного DSP, MIDI, sampler, APVTS или preset поведения.
- Skin — только визуальный слой. Он не обращается к processBlock, DSP, sampler, MIDI routing или APVTS и не создаёт аллокаций в audio callback.
- Переключение выполняется без перезапуска плагина; выбранный skin сохраняется и восстанавливается при следующем запуске.
- Измеряемые цели: hot-swap около 150 ms; CPU delta 0%; отсутствие остаточного роста RAM после выгрузки старых assets; paint-time не хуже code-drawn controls. Цели не считать достигнутыми без замеров.
- Все элементы REV.17 присутствуют во всех skins. Отсутствующий в макете обязательный элемент достраивается в согласованной зоне; пропуск — FAIL.
- MIDI key reactivity: реальные note-on подсвечивают клавиши ≤16 ms; fade выполняется только UI timer; невидимое окно приостанавливает анимацию; shortcuts подсвечивают соответствующие UI элементы.
- Заимствуются архитектурные принципы, но не чужой код; лицензии и источники фиксируются в INFO.

## 2. Визуальная карта и контрольная полнота

Зоны A–H обязательны: TOP BAR; PRESET BROWSER; MELODY/PATTERN/PUNK GROUP и девять rotary controls; семь FX CHAIN modules; AutoChords; FX EDIT; AMP & TONE; MIDI keyboard/PITCH/MOD/Performance/status strip. Дополнительные панели: Import Guitar, Auto Discover, Curator, Settings, About, SKINS Gallery.

Каждый элемент должен иметь запись в control checklist: элемент ↔ зона ↔ APVTS ID ↔ статус. APVTS IDs и semantics неизменны. Визуальная подпись макета ↔ APVTS mapping фиксируется явно в `SKIN/control_schema.json`; неизвестное или неоднозначное соответствие нельзя молча угадывать.

## 3. Skin System modules

Обязательный состав: Main Skin, Skin Engine, Skin Gallery, Control Library, Window Manager, Asset Manager, Theme Persistence, Skin Validator. Каждый модуль проходит цикл A6 и имеет статус в INFO/CODE.

Theme Engine: REV14 manifest parser; canonical 21 palette tokens плюс `key_on` и `key_glow`; единый ThemeableLookAndFeel; безопасный fallback; фоновые asset decode; атомарная публикация визуального snapshot; repaint всех компонентов; выгрузка старых ресурсов; ECO mode. Skin не меняет геометрию, функциональную карту или состояние контролов.

## 4. Начальная коллекция

Пять обязательных оформлений:
1. NEON PUNK — основной, по умолчанию.
2. BLACK METAL.
3. CYBER BLUE.
4. DARK INDUSTRIAL.
5. CLASSIC STUDIO.

Кнопка SKIN/SKINS открывает SKINS GALLERY. Галерея содержит previews, search, preview/apply, save selection, restore default и fallback. Смена оформления обновляет все панели и окна, не меняя звук, MIDI, параметры, пресеты и состояние открытых панелей. MIDI key glow применяется в главном окне.

**Preflight note:** на базовом SHA `881732c44214ea9160150fc69cd77ad7d85d5bc0` registry содержит 52 записи и объявляет `skin-052` / SIGNAL FORGE default; ThemeManager fallback по-прежнему использует `skin-51`. Это несовпадение с v7 initial collection / NEON PUNK default — открытый migration item. Не переименовывать и не переназначать skins до согласованной registry/control-schema миграции и regression tests.

## 5. Рабочий цикл A6

Каждая задача выполняется строго:
1. ПРОВЕРКА — branch, HEAD, source, tests, journals, дубли и зависимости.
2. АНАЛИЗ — причина, инварианты, запретные области, минимальный объём.
3. ИСПРАВЛЕНИЕ — одна причина на commit, только REV.17, compile-check если доступен.
4. ПОВТОРНЫЙ АНАЛИЗ — тесты/измерения/visual parity на точном SHA; проверка соседних инвариантов.
5. РЕЗУЛЬТАТ — измеренный PASS либо честный PARTIAL/FAIL; синхронная запись INFO/CODE.

DESIGNED ≠ IMPLEMENTED. Build/CTest/pluginval/host/release PASS допускается только по доказательствам на конкретном SHA.

## 6. Параллельные фронты и гейты

F0 pattern research; F1 build-fix/incremental build; F2 realtime/performance; F3 Skin Engine/Asset Manager/Validator; F4 shell A–H/Control Library; F5 Window Manager; F6 panels; F7 parameter/UI inventory; F8 lifecycle, fallback, hot-swap, zero-footprint, key-reactivity tests; F9 package; F10 INFO/CODE and persistence.

Acceptance gates G-A…G-N: bit-identical audio and CPU delta; zero audio-thread allocations; RAM/paint-time parity; control parity across five skins; corrupt-manifest fallback; DPI 100/125/150%; ECO measurement; ten rapid swaps without leaks; screenshot fidelity; complete REV.17 inventory; package/runtime switching; key-reactivity ≤16 ms; A6 journal compliance; persisted skin selection and identical functional architecture.

Merge only after green CI on the exact current SHA. Release candidate additionally requires Windows x64 Release, full CTest, pluginval strictness 5, measured performance, FL Studio 20 host verification, package and reconciled journals on one SHA.

## 7. Non-negotiable boundaries

Do not modify DSP, processBlock, sampler, MIDI routing, APVTS IDs/semantics, presets or SoundBank during skin-only work. Do not claim host verification without FL Studio 20. Do not claim CPU/RAM/latency targets without measurements. Keep INFO/CODE append-only and synchronized. Work only in `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`; never alter `main`.
