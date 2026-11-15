# NEON STAGE — control/state sprite package REV18

Статус: **[DECISION] — оригинальные sprite masters и runtime exports SKIN-1**.

## Инвентарь

Источник списка — `SKIN/control_schema.json`, поле
`x-ppgu-contract.requiredControlIds`. Генератор не добавляет и не переименовывает
APVTS IDs. Для каждого из 27 control IDs создаются пять состояний:
`default`, `hover`, `pressed`, `disabled`, `dragged`.

Для каждого состояния отдельно создаются master и runtime PNG 512×512.
Итого: 27 × 5 × 2 = **270 PNG-файлов**. Целостность, SHA-256, CRC32, размеры,
путь и `[DECISION]` provenance фиксируются в `assets/control_manifest.json`.

## Специальные ограничения

- `LED_indicator`: полностью прозрачный sprite во всех пяти состояниях.
  Светодиод не отображается и не имитирует активность.
- `Auto_Discover`, `Curator`, `Settings`, `About`: disabled-static визуал во
  всех состояниях; наличие artwork не означает готовность backend-функций.
- `mode_selector`: только MELODY/PATTERN; PUNK GROUP не добавляется.
- FX switches: ровно семь; восьмой переключатель не создаётся.
- Не добавляются PREV/NEXT/FAVORITE/SAVE, pitch/mod wheels, CPU/RAM meters.
- LCD остаётся пустой поверхностью; никаких выдуманных статусов/метрик.

## Генерация и проверка

`tools/generate_rev18_skin_controls.py --root SKIN` создаёт master/runtime
спрайты и manifest. Режим `--check` повторно строит ожидаемые PNG в памяти и
сверяет байты/manifest без изменения файлов. Генератор использует Pillow;
runtime assets не зависят от ручного масштабирования старых файлов.

Это визуальный ресурсный слой. Он не подключает новые обработчики событий,
не меняет DSP, MIDI, sampler, APVTS или состояние пресетов. Фактическое
подключение спрайтов к JUCE-компонентам и визуальная приёмка остаются отдельным
этапом до release.
