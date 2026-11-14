# PPGU REV.18 — соглашение об asset-path и аудит SKIN

Статус: **[DECISION] — техническая конвенция хранения ассетов**.  
Область: только визуальные ресурсы. Не меняет APVTS, runtime-layout, MIDI,
DSP, audio path, preset state или поведение процессора.

## Зачем это добавлено

Исторический пакет SKIN Studio REV14 использует компактную структуру
`assets/hero.png`, `assets/preview.png` и `svg/*.svg`. Этого недостаточно
для REV.18: спецификация требует отдельные master-исходники, runtime-экспорты,
состояния контролов, фон, baked FX layers, иконки и atlas. Старые файлы
не удаляются и не перезаписываются этим соглашением.

## Каноническая структура для новых REV.18 ассетов

Для каждой темы `SKIN/<SkinFolder>/`:

```text
assets/
  master/
    controls/<control-id>_<state>.png
    hero/hero.png
    preview/preview.png
    textures/<name>.png
    background/main.png
    fx_layers/<layer>.png
    icons/<topbar-name>.svg
  controls/<control-id>_<state>.png
  hero/hero.png
  preview/preview.png
  textures/<name>.png
  background/main.png
  fx_layers/<layer>.png
  icons/<topbar-name>.png
fonts/
  <licensed-font-files>
  LICENSE.txt
atlas/
  atlas.png
  atlas.json
theme.json
```

Состояния контролов: `default`, `hover`, `pressed`, `disabled`,
`dragged`. Имена control-ID берутся только из
`SKIN/control_schema.json`; новые APVTS ID не создаются.

Top-bar icon basenames: `skins`, `import_guitar`, `auto_discover`,
`curator`, `settings`, `about`. Нереализованные точки входа могут
иметь визуально disabled-состояние, но не могут показывать фиктивную работу.

## Минимальные размеры

| Класс | Master | Runtime |
|---|---:|---:|
| Контролы, каждый кадр | 512×512 | 512×512 |
| Hero | 1600×900 | 800×450 |
| Preview | 720×480 | 360×240 |
| Texture | 1024×1024 | 1024×1024 |
| Main background | 2560×1440 | 2560×1440 |

Текстуры должны быть бесшовными. Все PNG проверяются по CRC каждого chunk
и декодируемой длине pixel payload. SVG проходят XML-разбор и не должны
содержать script, event handler, external href/src, DOCTYPE или ENTITY.

## Read-only аудит

Новый инструмент:

```powershell
python tools/audit_skin_rev18_assets.py --root SKIN --skin-id skin-053
```

Сохранить машинно-читаемый отчёт:

```powershell
python tools/audit_skin_rev18_assets.py --root SKIN --skin-id skin-053 --report build/skin-053-rev18-audit.json
```

Код возврата: `0` — PASS, `1` — найдены несоответствия, `2` — невозможно
прочитать контракт/реестр/тему. Инструмент ничего не генерирует и не изменяет.

Проверка контраста строго требует не менее 7:1 для `text_primary` к
`window_bg`. Secondary, disabled, LCD и accent измеряются и выводятся
отдельно; порог 7:1 к ним автоматически не применяется, поскольку REV.18
задаёт для них измерение/семантику, но не универсальный порог 7:1.

## Разделение валидаторов

- `tools/validate_skin_pack.py`: прежняя проверка G1–G4 и registry/pack.
- `tools/audit_skin_rev18_assets.py`: полнота master/runtime asset package.
- Оба результата обязательны перед заявлением о готовности темы.
- Отсутствующий ресурс считается FAIL; скрипт не создаёт placeholder и
  не выдаёт существующий runtime-файл за master.

## Текущая граница

Это соглашение и аудитор — первый инженерный шаг. Оно не означает, что
SKIN-1 уже содержит полный REV.18 комплект, и не активирует новые темы.
Создание/экспорт графики выполняется отдельным следующим этапом после
проверки результатов аудита и сохранения исходных NEON STAGE ассетов.
