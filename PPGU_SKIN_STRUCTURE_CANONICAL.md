# PPGU SKIN STRUCTURE — CANONICAL ALIGNMENT (REV.17)

Исправление: структура `themes/signal-forge-neon/` из раннего драфта не соответствует
канону PPGU. Канонические источники: SKIN/Skin_XXX/theme_manifest.json,
SKIN/theme_manifest.json (реестр, 102 темы, default = skin-052 SIGNAL FORGE),
Source/UI/ThemeManager.{h,cpp}, tests/ValidateSkinManifests.py.

Этот документ НЕ создаёт конкурирующую спецификацию — он маппит идеи
(states/animations/usage) на каноническую схему Skin Studio.

## 1. Каноническая структура (без изменений)

SKIN/
├── theme_manifest.json          # реестр: 102 темы, defaultSkin = skin-052
├── theme.schema.json            # legacy-схема для Skin_01..51 (двузначные ID)
├── control_schema.json          # схема Skin Studio 52+ (ID из 3+ цифр)
├── Skin_001/ … Skin_051/        # legacy: theme_manifest.json (version 21)
└── Skin_052/                    # SIGNAL FORGE, канонический дефолт
    ├── theme_manifest.json
    ├── theme.json               # canonical palette для fallback (см. CODE journal)
    ├── preview.png
    ├── hero.png
    ├── bezel.svg
    ├── crest.svg
    └── texture.svg

## 2. Канонические 21 токенов (не менять имена!)

window_bg, panel, panel_elevated, control_bg, control_face, control_border,
text_primary, text_secondary, text_disabled, accent, accent_glow,
danger, warning, success, lcd_bg, lcd_text, led_on, led_off,
shadow, highlight, lowlight

## 3. Маппинг драфт-полей на канонические токены

| Драфт (theme.json)       | Канонический токен            |
|--------------------------|-------------------------------|
| window.baseColor         | window_bg                     |
| background.gradientTop   | panel                         |
| background.gradientBottom| panel_elevated                |
| panel.borderColor        | control_border                |
| panel.glowColor          | accent_glow                   |
| controls.knob.arcColor   | accent                        |
| controls.slider.fillColor| accent                        |
| button.textColor         | text_primary                  |
| button.textColorDisabled | text_disabled                 |
| —                        | text_secondary (метки)         |
| —                        | lcd_bg / lcd_text (экраны)     |
| —                        | led_on / led_off (индикаторы)  |
| —                        | shadow / highlight / lowlight  |
| status.ok                | success                        |
| status.warn              | warning                        |
| status.error             | danger                         |

Правило: цвет ищется ТОЛЬКО по токену через ThemeManager; hex-значения живут
в манифестах скинов, а не в коде редактора.

## 4. Дополнительный слой (usage / states) — как дополнение, не замена

usage.json (рядом с манифестом, схема — в control_schema.json):
{
  "usage": {
    "knobArc": "accent",
    "clipLED": "danger",
    "panelLabels": "text_secondary",
    "valueReadouts": "lcd_text"
  },
  "states": {
    "bypassed": { "overlayToken": "shadow", "alpha": 0.45 },
    "active":   { "overlayToken": "shadow", "alpha": 0.0 }
  }
}

- значения — только ссылки на канонические токены (никаких hex внутри usage);
- валидатор обязан проверять: каждый ключ usage/resolved существует в наборе 21;
- ThemeManager резолвит токен -> цвет в рантайме, legacy-скины получают
  детерминированные fallback.

## 5. Правила ассетов (адаптировано из theme-spec драфта)

- PNG: только preview.png и hero.png, размеры фиксируются манифестом;
  validateAssetSpec() проверяет размеры как int64 (без narrowing, см. REV.17 E1).
- SVG: bezel / crest / texture — только безопасные (без скриптов и внешних
  ссылок), проверка на этапе G3 и при .ppguskin-импорте (quarantine + SHA-256).
- Именование: snake_case; никакие цветные градиенты не запекаются в PNG там,
  где цвет должен браться из токенов.
- Retina/DPI: масштабы берутся из манифеста, не из имени файла (никаких
  thumb@2x.png вне схемы).

## 6. Ограничения (non-goals, по CODE journal)

- Никаких изменений DSP, processBlock, audio callback, MIDI, APVTS IDs/semantics,
  presets, SoundBank, поведения существующих контролов.
- Control parity: Skin Studio 52+ обязан объявлять ровно 27 canonical control ID
  плюс sync_selector (итого 28 properties); missing/unknown отклоняются.
- Legacy-схему 1..51 не переписывать под схему 52+ — расхождение интенционально.

## 7. Чек-пойнты перед следующим шагом (preflight)

1. Подтвердить актуальный branch/HEAD (журнал требует preflight перед любым
   code change).
2. Сверить control_face в SKIN/Skin_052/theme.json: в манифесте-снимке видно
   #364050, а canonical fallback в ThemeManager.h — #26343A; расхождение нужно
   расследовать до использования fallback в новых UI-компонентах.
3. Проверить статус точного SHA: build/CTest/pluginval/FL Studio acceptance
   в журнале помечены PENDING — не считать их пройденными.
4. Любое изменение usage/states оформить записью в INFO.txt и CODE.txt по
   шаблону (append-only), с точными SHA и путями.
