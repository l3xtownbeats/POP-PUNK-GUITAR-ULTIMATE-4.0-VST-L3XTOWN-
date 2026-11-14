# NEON STAGE — регистрация TOP BAR icons (REV18)

Статус: **[DECISION] — оригинальные нейтральные line-icons для SKIN-1**.
Область: только artwork. Не включает и не активирует программные функции.

## Почему созданы

В исходном пакете `Skin_053` не было набора TOP BAR SVG/PNG. Утверждённый исходный комплект иконок не найден, поэтому создан самостоятельный минималистичный набор в визуальном языке NEON STAGE: светлый контур, кислотно-зелёный акцент, прозрачный фон. Чужие логотипы и сторонние изображения не использовались.

## Состав

| ID | SVG master | PNG runtime | Размер runtime | Назначение |
|---|---|---|---:|---|
| skins | `assets/master/icons/skins.svg` | `assets/icons/skins.png` | 256×256 RGBA | Галерея тем |
| import_guitar | `assets/master/icons/import_guitar.svg` | `assets/icons/import_guitar.png` | 256×256 RGBA | Импорт SFZ/гитары |
| auto_discover | `assets/master/icons/auto_discover.svg` | `assets/icons/auto_discover.png` | 256×256 RGBA | Визуальный знак зарезервированного Auto Discover |
| curator | `assets/master/icons/curator.svg` | `assets/icons/curator.png` | 256×256 RGBA | Визуальный знак зарезервированного Curator |
| settings | `assets/master/icons/settings.svg` | `assets/icons/settings.png` | 256×256 RGBA | Настройки |
| about | `assets/master/icons/about.svg` | `assets/icons/about.png` | 256×256 RGBA | Сведения о продукте |

Полные SHA-256, CRC32, размер и происхождение каждого файла записываются в
`assets/asset_manifest.json`. PNG runtime exports имеют 256×256 RGBA.
Их геометрия согласована с SVG masters; детерминированный экспорт выполняется
`tools/generate_rev18_skin_icons.py` через Pillow и проверяется режимом
`--check`. SVG masters остаются исходниками.

## Поведение и ограничения

Наличие иконки не означает доступность функции. Auto Discover, Curator,
Settings и About остаются disabled/unavailable, пока их реальные backend/
панели не интегрированы. Иконки не отображают фиктивную активность и не
меняют APVTS, параметры, layout ownership, MIDI или audio path.

## Проверки

- PNG: размер, CRC каждого chunk и декодируемый pixel payload.
- SVG: XML parse; запрет DOCTYPE/ENTITY, script, foreignObject, iframe,
  object/embed, event-handler attributes, внешних href/src и внешних CSS url().
- Manifest: exact paths, byte count, SHA-256, CRC32 и [DECISION] provenance.
- Набор должен пройти оба валидатора: legacy G1–G4 и REV18 asset audit.
