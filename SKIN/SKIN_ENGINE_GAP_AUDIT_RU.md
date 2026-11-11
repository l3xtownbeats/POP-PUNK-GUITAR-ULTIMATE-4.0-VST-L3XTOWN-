# PPGU Skin Engine — аудит разрывов реализации

Дата аудита: 11 ноября 2026  
Проверенная ветка: main  
Проверенный HEAD: 892cba3ea523af204d9c7e9c9ab0cd30809e08f1  
Область: текущий исходный код, CMake, skin manifests/schema, тесты, packaging workflow, README и предыдущая проверка PR #24.

## Итог

**Статус Skin Engine: SPECIFIED / PARTIALLY SCAFFOLDED / NOT RUNTIME-INTEGRATED.**

В репозитории есть описания, JSON-манифесты и отдельный JUCE-класс для проверки части палитры. Но текущий редактор плагина не подключает ThemeManager и не предоставляет работающую смену тем. Наличие 50/51 JSON-файлов не означает наличие 50/51 готовых графических скинов.

Пользовательское обязательное правило для следующей реализации: **папка Skins должна находиться рядом с установленным .vst3 bundle в каталоге установки плагина.** Не использовать %APPDATA%\PPGU\themes, Documents или отдельную системную папку как основной источник тем. Существующая структура SKIN/ в исходниках — каталог разработки/манифестов, а не конечный runtime-путь.

## 1. Критические функциональные пробелы

| ID | Недостаток | Что подтверждено в main | Что требуется |
|---|---|---|---|
| SK-01 | Нет кнопки SKIN ▾ | В PluginEditor.h/.cpp нет skin button, browser или shortcut handler | Постоянная кнопка в правом верхнем углу каждого GUI; Ctrl+K |
| SK-02 | Нет браузера тем | Нет Theme Gallery, карточек, preview, поиска, избранного, группировки, scroll/keyboard navigation | Browser с DEFAULT первой строкой, preview, search/favourites/families, Esc/повторный выбор для закрытия |
| SK-03 | ThemeManager не подключён к редактору | ThemeManager.cpp компилируется, но PluginEditor.cpp не включает/не создаёт PPGUThemeManager и продолжает использовать жёстко заданные цвета | Подключить manager к message-thread UI и обновлять все компоненты без изменения параметров/звука |
| SK-04 | Меняется только маленькая палитра в отдельном классе | Runtime Palette содержит ограниченный набор цветов; полные 18 token groups не применяются к контролам | Полный data-only theme model и отображение всех разрешённых токенов |
| SK-05 | Нет hot-swap | Нет смены темы в открытом окне, анимации 150 ms или доказательства отсутствия аудио-щелчка | Безопасная перерисовка на UI thread, без остановки/изменения audio thread |
| SK-06 | Нет сохранения выбранной темы | Нет user-level theme preference и восстановления темы при повторном открытии | Сохранять theme ID в пользовательской конфигурации, отдельно от preset/APVTS sound state |
| SK-07 | Нет runtime scan папки тем | loadFromFile() принимает конкретный файл; сканирования каталога и discovery нет | Сканировать соседнюю с VST3 папку Skins/, перечитывать её в фоне, не блокируя аудио |
| SK-08 | Нет требуемого fallback UX/logging | Ошибка сбрасывает объект Palette в default, но нет гарантированного лога, статуса в UI или пользовательского сообщения | Безопасный DEFAULT, предупреждение в журнале/статусе и понятная пометка несовместимой темы |
| SK-09 | Нет проверки версии плагина | Манифест содержит minimumPluginVersion, но код не сравнивает её с версией продукта | Зафиксировать одно имя поля (plugin_version_min либо единый camelCase контракт) и реализовать сравнение версий |
| SK-10 | Нет полного визуального паритета | Нет механизма, который проверяет наличие всех контролов, состояний и параметрических связей во всех темах | Единый layout/control registry; темы не могут менять bounds, APVTS IDs, attachments, hit areas или имена контролов |

## 2. Критические пробелы в данных и графических ресурсах

1. В дереве SKIN/ обнаружено **51 папка Skin_XX с theme.json**, при этом schema, registry и Python validator рассчитаны ровно на **50** тем (Skin_01…Skin_50). Skin_51/theme.json объявляет skin-51 / index 51 и выходит за допустимый диапазон схемы. Он не включён в registry. Нужно принять решение: удалить/архивировать лишний слот либо согласованно расширить schema, registry, C++ и тесты. Не считать его доступной темой до решения.
2. В папках Skin_01…Skin_51 отсутствуют заявленные графические файлы assets/preview.png, assets/hero.webp, assets/texture.png. Спецификация прямо говорит, что это задания, а не готовые ресурсы. Следовательно, превью/hero/texture сейчас не могут быть показаны/загружены.
3. Манифесты описывают token palette, но это не полноценные skins со спрайтами/векторными слоями: нет подтверждённого комплекта SVG/PNG для rotary frames, toggles, buttons, LEDs, meters, display states, fonts и icons.
4. Документация и runtime path расходятся: в репозитории каталог называется SKIN/, в package workflow создаётся SKINS/ внутри ZIP, пользовательское требование — установленная папка Skins/ рядом с VST3. Нужна единая политика: source data → packaged Skins/ → installed sibling Skins/.
5. Пользовательский текст упоминает 61 тему (6 семейств × 10 + default), тогда как repository registry объявляет 50; фактически в source tree найдено 51 манифест. Кластеры A/B/C в текущем registry не равны шести семействам. Количество и группировку нельзя рекламировать как готовые до нормализации каталога.

## 3. Недостатки валидации и безопасности

- Python validator проверяет 50 JSON-манифестов, цветовые форматы и только три выбранные пары контраста. Он не является полным JSON Schema 2020-12 validator.
- C++ ThemeManager проверяет часть структуры и цветов, но не проверяет фактическое существование/чтение preview, hero, texture и иных файлов, их формат/размер, SVG resolution, шрифты или sprite frame counts.
- Ни Python, ни C++ проверка сейчас не подтверждает полный control inventory parity для каждой темы.
- Не подтверждены защита от path traversal/выхода из корня Skins/, лимиты размера/размерностей изображения, безопасный decode и обработка повреждённых растровых файлов.
- Не реализованы lazy texture loading, LRU eviction, RAM budget и ECO animation policy reduceInEco.
- Нет проверки контраста всех реальных text/control combinations, focus/hover/pressed/disabled states и DPI 100–200%.
- В контракте используются разные формулировки fallback: «молча DEFAULT», «показать несовместимость», «честное сообщение». Требуется согласовать поведение: аудио и UI остаются работоспособными, DEFAULT применяется безопасно, предупреждение записывается, browser показывает статус.

## 4. UI/layout и функциональные границы

- Текущий редактор — фиксированная тёмно-розовая отрисовка с жёсткими цветами в namespace и отдельных setColour() вызовах; темы пока не управляют фоном, hero, панелями, rotary, FX-кнопками, keyboard, labels и status.
- Матрица CONTROL_INVENTORY_MATRIX_RU.md уже отмечает много APVTS-параметров без соответствующих GUI-контролов. Skin не может «добавить» эти функции графикой. Сначала нужен принятый базовый inventory фактически существующего UI, затем общий parity contract.
- В текущем редакторе размеры и layout фиксированы кодом; setResizeLimits(800, 790, ...) ограничивает минимальную высоту. Нет доказательства DPI scaling 100–200%, сохранения одинаковых bounds или тестов на clipping после смены темы.
- Нет тестов на то, что смена темы не меняет APVTS values, preset selection, MIDI state, processor state, sound output, keyboard focus и control attachments.

## 5. Packaging и установка

Текущий packaging workflow копирует исходный каталог SKIN/* в SKINS/ внутри ZIP и явно записывает в INSTALL_AND_CONTENTS.txt, что runtime skin switching не заявлен как host-verified. Он не создаёт Skins/ рядом с VST3 bundle в установленном каталоге, не формирует per-skin runtime assets и не проверяет соседнее расположение.

Требуемая целевая структура поставки/установки:

```text
VST3 install directory/
├── POP PUNK GUITAR ULTIMATE.vst3/
└── Skins/
    ├── default/
    │   ├── theme.json
    │   ├── assets/
    │   ├── svg/
    │   └── textures/
    ├── skin-02/
    └── ...
```

Windows VST3 bundle сам является каталогом. Runtime discovery должен надёжно получить путь установленного bundle и найти sibling Skins/; нельзя полагаться на текущую рабочую директорию DAW. Если каталог недоступен для записи (например, Program Files), плагин не должен пытаться создавать/перезаписывать его без прав: установка создаёт каталог заранее, а пользовательские темы добавляются с подходящими правами. Это не основание переносить основной путь в AppData.

## 6. Тесты и CI

- ThemeManagerTests.cpp тестирует default fallback, загрузку 50 manifest files и несколько некорректных JSON/compatibility случаев. Это не UI/runtime test и не проверка реальных skin assets.
- ValidateSkinManifests.py ожидает 50 тем, тогда как tree содержит 51. Необходим тест, который ловит orphan/extra folders и registry mismatch.
- Нет тестов scan/discovery, default ordering, invalid theme isolation, missing asset fallback, version incompatibility, path traversal, hot-swap, persistence, theme reload, keyboard navigation, DPI, contrast всех состояний, memory cap/LRU и ECO mode.
- Последний Windows packaging run #826 / 43017263448 завершился failure при компиляции ppgu_processor_pattern_tests: generated JuceHeader.h включает BinaryData.h, которого нет в include path этого test target. CTest, pluginval и packaging были пропущены. Это блокирует подтверждение нового ZIP, хотя ошибка относится к общей сборке, а не к самому skin runtime.
- Старый artifact 13766348419 — прежняя успешная сборка с прежней структурой; он не доказывает работу Skin Engine.

## 7. Внешняя проверка PR #24

SKIN/PR24_INTEGRATION_REVIEW_RU.md фиксирует, что PR #24 нельзя сливать целиком: кнопка названа THEMES вместо SKIN, popup не является Theme Gallery, старая manifest schema несовместима, изменены подписи FX и есть аудиоизменения вне skin scope. Эти ограничения остаются актуальными; PR не считается интегрированным.

## 8. План закрытия пробелов (приоритет)

1. **P0 — согласовать контракт и каталог:** 50 или 51/61; единые ID/family names; одно имя version field; окончательная структура Skins/ рядом с VST3; полный список реально существующих UI controls.
2. **P0 — базовый runtime:** безопасный path resolver sibling Skins/, scan manifests, DEFAULT, version compatibility, path containment, robust fallback + log.
3. **P0 — UI entry:** кнопка SKIN, browser, Ctrl+K, DEFAULT, preview/search/favourites/family navigation, keyboard/Esc behavior, persistent user preference.
4. **P1 — полноценная тематизация:** применить все token groups к всем существующим компонентам через общий LookAndFeel/theme model; layout/APVTS/audio invariant tests.
5. **P1 — графика:** создать и проверить реальные preview/hero/texture и control assets; не маркировать манифест как готовый skin до наличия обязательных файлов.
6. **P1 — производительность/доступность:** lazy loading, LRU/RAM budget, ECO policy, contrast, DPI 100–200%, decode/path security.
7. **P0 — сборка и acceptance:** исправить CMake generated-header dependency для pattern test target, затем Windows Release, CTest, pluginval strictness 5 и ручная проверка в FL Studio 20. Отдельно проверить, что смена темы не меняет звук и не создаёт clicks.

## 9. Критерий «Skin Engine готов»

Функцию можно считать готовой только после одновременного подтверждения:
- реальная sibling папка Skins/ рядом с установленным VST3 и runtime scan;
- все объявленные темы имеют валидные манифесты и реальные assets;
- кнопка SKIN/browser/preview/hot-swap/Ctrl+K/default/persistence работают;
- все существующие UI controls тематизируются без изменения layout/APVTS/audio;
- повреждённая/несовместимая тема не роняет плагин и не прерывает звук;
- тесты покрывают scan, fallback, assets, version, parity, persistence, hot-swap, DPI, accessibility и memory limits;
- Windows x64 build + CTest + pluginval проходят, FL Studio 20 host check выполнен.

**Текущий честный вывод:** в main имеется тематическая спецификация, 51 JSON-манифест (из них registry/validator допускают 50) и частичный ThemeManager; готового пользовательского Skin Engine и нового проверенного ZIP пока нет.
