# PPGU — Система визуальных тем: полный справочник. Часть 2
## Системы тем других инструментов + файловая спецификация темы

**Проект:** POP PUNK GUITAR ULTIMATE (PPGU)  
**Дата:** 2026-11-13  
**Тип:** справочник и спецификация  
**Статус:** SPECIFIED_NOT_YET_VERIFIED_IN_SOURCE  
**Связанный контракт:** часть 1 — галерея, транзакции PREVIEW/APPLY/BACK и порядок тем.

> Этот документ фиксирует целевую архитектуру. Сам по себе он не доказывает, что функции уже реализованы в C++ или что ресурсы существуют в репозитории.

## 1. Референсные системы

### 1.1 Serum — растровая система
Serum использует папки скинов в пользовательской папке пресетов, растровую color map и варианты ресурсов 1x/2x. Это простой для художника подход: палитра редактируется в изображении, а не в коде. Недостатки для PPGU: геометрия скрыта в бинарнике, формат сам по себе не обеспечивает транзакции, проверку целостности и безопасный fallback.

### 1.2 Vital — токены и редактор темы
Vital представляет темы как набор именованных цветов, сгруппированных по областям интерфейса. Дизайн меняется без изменения общей компоновки. Это подтверждает выбранное PPGU правило: токены могут меняться, структура и функциональность контролов — нет. Skin Designer в рантайме не является обязательной частью MVP PPGU.

### 1.3 Surge XT — декларативная система
Surge XT использует skin bundle с skin.xml, globals, именованными цветами/изображениями, классами компонентов и адресацией контролов через UI identifiers. Поддерживаются SVG/PNG, многорезолюционные ресурсы, вертикальные спрайты и hover overlays. Это полезный архитектурный референс для переиспользуемых материалов и именованных ресурсов.

**Ограничение заимствования:** Surge может позволять скину переопределять позицию, размер или видимость контрола. PPGU этого не разрешает. В PPGU skin control map — только декларация визуальных ресурсов и проверяемых ссылок на заранее существующие control IDs. Геометрия, видимость, hit areas, parameter bindings и поведение принадлежат общей реализации и не могут изменяться темой.

### 1.4 REAPER — конфигурация и изображения
REAPER использует ReaperThemeZip, rtconfig/WALTER и ресурсы нескольких разрешений. Это демонстрирует мощный, но слишком свободный контроль раскладки. PPGU не принимает WALTER-подобное управление геометрией из темы.

### 1.5 Сводный вывод
PPGU использует гибрид:
- токены Vital/Surge для цветов, шрифтов и метрик;
- растровые материалы и color-map подход Serum;
- вертикальные sprites и named assets по образцу Surge;
- 1x/2x и список поддерживаемых zoom levels;
- code-owned shared control schema;
- обязательную офлайн-валидацию, checksums и runtime fallback;
- транзакционную галерею из части 1.

## 2. Четыре слоя оболочки

1. **STRUCTURE** — общая control map, геометрия, IDs, parameter bindings, hit areas и поведение. Одинакова для всех тем; принадлежит коду и общей схеме.
2. **TOKENS** — цвета, шрифты, радиусы, ширина рамок, glow metrics.
3. **MATERIALS** — панели, текстуры, bezel, control sprites, raster surfaces.
4. **DECOR** — glow, highlights, анимации, branding/crest.

Меняются только слои 2–4. Слой 1 не загружается из темы и не применяется при PREVIEW. Любые поля layout, bounds, position, size, visibility, parameter или behavior в старых манифестах считаются legacy metadata и неавторитетны; валидатор должен отклонять попытку их использовать как runtime override.

## 3. Файловая структура темы

Целевая структура пакета:

```text
Skin_NN/
  theme.json
  preview.png
  hero.png
  images/
    1x/
      panel.png
      header.png
      footer.png
      keyboard_bg.png
      bezel_main.png
      bezel_section.png
      knobs/
        knob_lg_strip.png
        knob_md_strip.png
        knob_sm_strip.png
      buttons/
        btn_tab.png
        btn_chip.png
        btn_icon.png
        btn_toggle.png
      sliders/
        slider_track.png
        slider_thumb.png
      leds/
        led_meter.png
        led_status.png
      lists/
        list_row.png
        list_scrollbar.png
      icons/
        icon_<name>.png
    2x/                 # та же относительная структура, 2x dimensions
  svg/                  # редактируемые исходники; не runtime по умолчанию
  fonts/
  states/               # optional
  animation/            # optional
  LICENSES/             # если есть сторонние ресурсы
  SHA256SUMS
```

### 3.1 Обязательность
- `theme.json` обязателен всегда.
- `preview.png` (360×240) и `hero.png` (800×450) обязательны для темы, помеченной как графически завершённая и доступная для публикации в галерее. Черновые темы должны явно иметь draft/unpublished статус и не маскироваться под завершённые.
- `images/1x/` содержит полный набор runtime assets, требуемый общей control schema и выбранными слоями.
- `images/2x/` зеркалит структуру 1x. Пропуск допускается только при явном per-asset fallback flag.
- `svg/` содержит редактируемые исходники, но не загружается runtime, если отдельный безопасный SVG pipeline не утверждён.
- `fonts/` опциональна, но каждый встроенный шрифт требует лицензии, разрешающей распространение.
- `states/` и `animation/` опциональны.
- `LICENSES/` обязательна при наличии third-party assets.
- `SHA256SUMS` обязателен для распространяемого пакета.

### 3.2 Безопасность путей и форматов
- Runtime images: PNG RGBA с валидной сигнатурой, корректными chunk CRC и успешным pixel decode.
- JPEG не использовать для неоновых UI материалов.
- Пути только относительные, нормализованные и остающиеся внутри корня темы. Запрещены absolute paths, traversal, symlink escape и неизвестные схемы.
- Имена файлов — lowercase English snake_case без пробелов.
- SHA256SUMS включает все распространяемые файлы, кроме самого SHA256SUMS.
- Recolorable materials предпочтительно нейтральные grayscale/alpha; цвет добавляется токенами. Полноцвет допустим для явно объявленного статичного декора.
- Для сторонних файлов нужны автор, источник, точный license text/URL, условия модификации и распространения, attribution и решение о включении. При неопределённой лицензии файл не включать в дистрибутив.

## 4. Модель theme.json

Ниже — пример целевого контракта. Он не доказывает наличие соответствующих файлов и не должен копироваться в runtime без сверки со shared control schema.

```json
{
  "schemaVersion": 1,
  "id": "signal-forge-neon",
  "name": "SIGNAL FORGE / NEON",
  "category": "signature",
  "family": "forge",
  "author": "PPGU Team",
  "version": "2.2.0",
  "minEngineVersion": "2.2.0",
  "role": {
    "primaryDefault": true,
    "startupDefault": true,
    "fallbackDefault": true,
    "immutableApprovedLayout": true
  },
  "gallery": {
    "preview": "preview.png",
    "previewSize": [360, 240],
    "hero": "hero.png",
    "heroSize": [800, 450]
  },
  "tokens": {
    "colors": {
      "bg.base": "#050B14",
      "bg.panel": "#0A1626",
      "bg.panel.elev": "#0E2038",
      "stroke.neon": "#2ED0FF",
      "stroke.neon.dim": "#1A5A7A",
      "text.primary": "#EAF6FF",
      "text.secondary": "#8FAEC7",
      "accent.primary": "#1E9BFF",
      "accent.warm": "#FFC53D",
      "led.green": "#38FF9C",
      "led.red": "#FF4D5E",
      "glow.primary": "#00C2FF"
    },
    "fonts": {
      "ui.primary": "fonts/PpguUi-Regular.ttf",
      "ui.bold": "fonts/PpguUi-Bold.ttf",
      "display": "fonts/PpguDisplay-Bold.ttf"
    },
    "metrics": {
      "cornerRadius": 8,
      "bezelWidth": 2,
      "glowRadius": 12,
      "glowIntensity": 0.55
    }
  },
  "controls": {
    "mapVersion": 1,
    "controls": []
  },
  "images": {
    "panel": "images/1x/panel.png",
    "header": "images/1x/header.png",
    "footer": "images/1x/footer.png",
    "bezelMain": "images/1x/bezel_main.png",
    "bezelSection": "images/1x/bezel_section.png",
    "hidpi": {
      "pathSuffixMap": {
        "images/1x/": "images/2x/"
      },
      "supportedZoomPercent": [100, 125, 150, 200]
    }
  },
  "assets": {
    "checksumFile": "SHA256SUMS",
    "verifyOnLoad": true
  }
}
```

Пустой массив controls — намеренная заглушка в примере. Production IDs и parameter references должны поступать из действующей общей control schema, а не из иллюстративного JSON. Темы не могут объявлять новые функциональные контролы или подменять параметрические привязки.

## 5. Спрайты и HiDPI

### 5.1 Knobs
- Вертикальный strip из квадратных кадров.
- Допустимое число кадров: ровно 64, 128 или 256, согласно manifest.
- Кадр 0 — минимум; последний — максимум.
- `frameIndex = round(value01 * (N - 1))`.
- `frameHeight = stripWidth`; общая высота изображения равна `frameHeight * N`.
- Угол вращения по умолчанию 300°, если общая control schema не задаёт иное.

### 5.2 Buttons/sliders
- Порядок состояний декларируется и валидируется.
- Типовые button states: normal, hover, pressed, selected.
- Toggle states: off, off_hover, on, on_hover.
- Slider track/thumb декларируют свои состояния/кадры.
- Hover overlays могут быть отдельными файлами, но не меняют hit area или bounds контрола.

### 5.3 HiDPI
- 1x и 2x имеют одинаковые относительные пути.
- Размеры 2x — ровно удвоенные относительно 1x, кроме явно описанных исключений.
- Целевые zoom levels: 100/125/150/200%.
- Runtime выбирает ближайший подходящий ресурс. Поиск файлов, checksum, декодирование и аллокации не выполняются в audio callback.

### 5.4 Color maps
Color-map atlas допустим для однотипных групп (VU segments, progress bars), если mapping versioned, документирован и проверяется валидатором.

## 6. Обязательные токены

Цвета: `bg.base`, `bg.panel`, `bg.panel.elev`, `stroke.neon`, `stroke.neon.dim`, `text.primary`, `text.secondary`, `accent.primary`, `accent.warm`, `led.green`, `led.red`, `glow.primary`.

Шрифты: `ui.primary`, `ui.bold` обязательны; `display` опционален.

Метрики: `cornerRadius`, `bezelWidth` обязательны; `glowRadius`, `glowIntensity` опциональны.

В draw paths запрещены hard-coded color literals. Компоненты получают визуальные цвета из активного immutable theme snapshot/token set. Исключения для системных цветов должны быть явно перечислены в общей UI спецификации.

## 7. Создание новой темы

1. Клонировать approved SIGNAL FORGE / NEON package как визуальную базу; не копировать несуществующие или неподтверждённые файлы.
2. Сохранить общую геометрию, control IDs, parameter bindings, hit areas и поведение.
3. Изменять только tokens, materials и decor.
4. Экспортировать PNG RGBA в 1x и 2x с согласованной структурой.
5. Обновить manifest и галерейные preview/hero.
6. Сгенерировать SHA256SUMS по всем распространяемым файлам, исключив сам checksum-файл.
7. Запустить валидатор схемы, путей, ассетов, декодирования, размеров, спрайтов, 1x/2x, checksum и лицензий.
8. Зарегистрировать тему в реестре без удаления, переименования или перенумерации существующих тем.
9. Пройти runtime acceptance: PREVIEW/APPLY/BACK, persistence, corrupt-resource fallback, audio invariance, HiDPI и visual regression.

## 8. Галерея и runtime integration

- ThemeManager загружает и валидирует тему вне audio callback.
- Декодирование PNG выполняется на message/UI thread или подготовительном worker с безопасной передачей готового immutable snapshot.
- PREVIEW временно заменяет только слои 2–4 и не изменяет сохранённый committed ID.
- Новый PREVIEW заменяет предыдущий временный preview.
- APPLY атомарно сохраняет committed theme ID.
- BACK/закрытие без APPLY восстанавливает entry theme.
- Ошибка загрузки/декодирования восстанавливает предыдущий preview либо approved NEON fallback согласно состоянию транзакции; плагин не падает.
- Переключение темы не меняет AudioProcessor, APVTS, MIDI routing, sampler, DSP и параметры.
- Дополнительные окна используют текущую тему, открываются поверх основного интерфейса и не вызывают reflow главного окна.

## 9. Обязательный validator / release checklist

Validator должен проверять:
1. JSON schema, schemaVersion, уникальность stable ID и отсутствие неизвестных/запрещённых полей.
2. Полный набор обязательных токенов и корректные значения цветов/метрик.
3. Все ресурсы, на которые ссылается control schema и manifest.
4. PNG signature, chunk CRC, успешное декодирование и размеры.
5. Точное число и геометрию кадров sprite strips.
6. Полное соответствие 1x/2x, размеры и разрешённые fallback exceptions.
7. SHA256SUMS, включая отсутствие неперечисленных runtime-файлов.
8. Безопасность нормализованных путей.
9. Наличие и допустимость лицензий сторонних ассетов.
10. Запрет изменения control IDs, bounds, parameter bindings, visibility и behavior темами.

Приёмка:
- PREVIEW → APPLY сохраняет выбор после перезапуска.
- PREVIEW → BACK и close без APPLY восстанавливают entry theme.
- Повторный PREVIEW корректно заменяет временный.
- Повреждение/отсутствие ресурса вызывает диагностику и fallback на SIGNAL FORGE / NEON без crash.
- Theme switch не меняет APVTS/MIDI/DSP.
- Скриншоты/visual regression подтверждают соответствие утверждённому макету.
- Несколько экземпляров плагина проверяются на корректную изоляцию состояния темы, если выбор темы является per-instance.
- Windows Release build, полный CTest, pluginval и FL Studio 20 host acceptance должны быть реально выполнены до объявления готовности.

## 10. Правило DEFAULT и сохранность существующих тем

- `signal-forge-neon` — утверждённая primary/startup/fallback тема, первая в галерее, label DEFAULT.
- Blackout Voltage (`skin-51`) — вторая, отдельная тема, не default и не fallback.
- Все остальные существующие темы сохраняют stable IDs и ресурсы.
- Порядок галереи не кодируется через numeric ID.
- Нельзя назначать canonical NEON ID существующей теме без доказательства, что её ресурсы являются согласованным NEON master.
- Если реальный NEON asset package ещё отсутствует, это отдельный блокер ресурсов, а не повод подменить его другой темой.

## 11. Глоссарий

- **Token:** именованная величина — цвет, шрифт или метрика.
- **Sprite sheet:** изображение с несколькими кадрами/состояниями.
- **Control map:** декларация визуальных ресурсов для фиксированных control IDs.
- **Bezel:** декоративная рамка/ободок панели.
- **Fallback theme:** SIGNAL FORGE / NEON.
- **Preview state:** временное состояние темы в памяти, не персистится.
- **Entry theme:** committed theme, зафиксированная при открытии галереи и восстанавливаемая при BACK.

## 12. Референсы

- Serum skin overview: https://iqsounds.com/en-au/blogs/news/customizing-serum-a-guide-to-unique-skins
- Serum preset installation: https://www.syntorial.com/tutorials/install-serum-presets/
- Vital custom themes: https://www.terrvform.com/post/how-to-make-install-custom-vital-skins-free-vital-skins
- Surge XT Skin Manual: https://surge-synthesizer.github.io/skin-manual/
- Surge skin architecture: https://deepwiki.com/surge-synthesizer/surge/3.1-skinning-system
- Surge skin library: https://surge-synthesizer.github.io/skin-library/
- REAPER theme overview: https://reaper.blog/2020/01/custom-reaper-6-theme/
- REAPER theme tweaking: https://reapertips.com/post/how-to-tweak-any-reaper-theme
- WALTER: https://www.soundonsound.com/techniques/walter-customisation-reaper

Источники выше — технические референсы; их лицензии и детали реализации не переносятся автоматически на PPGU.
