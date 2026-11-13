# PPGU — skin manifests, palette runtime и Skin Studio

## Фактическое состояние

В JUCE editor реализовано переключение визуальных палитр через кнопку **SKIN** и Ctrl+K. Выбор сохраняется в пользовательских настройках. Runtime skin root — папка `Skins/` рядом с VST3 bundle; в исходном дереве разработчика используется `SKIN/`.

`ThemeManager` поддерживает два versioned manifest contract:
- `Skin_01`…`Skin_51`: legacy nested-token schema `theme.schema.json`.
- `Skin_052` и последующие Skin Studio themes: плоская 21-token palette + control map по `control_schema.json`.

Для новых Skin Studio тем в редакторе применяются палитровые цвета. Координаты `controls.*.grid` пока остаются design metadata: они **не управляют реальным расположением JUCE controls**. Отдельная Theme Gallery, живой layout reflow, независимые popup windows и panel manager не должны считаться реализованными до отдельного кода и host acceptance.

Текущий fallback/default в коде — **Blackout Voltage (skin-51)**. Skin_052 Carbon Relay добавлен в реестр как selectable theme; это не изменение официального default. Любая смена темы должна оставаться visual-only: не менять APVTS, MIDI, presets, sampler или DSP.

## Форматы и каталоги

- `theme_manifest.json` — canonical registry, источник количества, ID, индексов и относительных путей.
- `theme.schema.json` — legacy manifest contract для Skin_01…Skin_51; не использовать его как schema для Skin Studio.
- `control_schema.json` — REV14 Skin Studio schema, 21 palette tokens, required control IDs и layout grid.
- `theme_pack_manifest.json` — состав следующего неслитого Skin Studio pack.
- `Skin_NNN/theme.json` — новая тема; PNG preview/hero и SVG texture/bezel/crest находятся рядом в соответствующих подкаталогах.
- `backups/` — сохранённые версии registry/schema перед merge; резервные копии не являются runtime themes.

## Skin Studio pipeline

Из корня репозитория:

```bash
python tools/generate_skin_batch.py --dry-run
python tools/generate_skin_batch.py
python tools/validate_skin_pack.py --root SKIN --manifest SKIN/theme_pack_manifest.json
python tools/merge_skin_pack.py --dry-run
python tools/merge_skin_pack.py
python tests/ValidateSkinManifests.py
```

Генератор по умолчанию готовит ровно 50 тем: шесть визуальных семейств по восемь вариантов и две специальные темы. Он читает следующий индекс из canonical registry, не перезаписывает существующие папки, создаёт PNG/SVG assets и запускает G1–G4 validator. Merge сначала валидирует pack, проверяет непрерывность/уникальность registry, создаёт backups и только затем атомарно обновляет реестр. Он сохраняет legacy и REV14 schemas раздельно.

Не запускайте merge, если pack validator не завершился PASS. Не удаляйте папки для обхода registry parity и не меняйте счётчик вручную.

## Release package

Workflow собирает sibling runtime folder `Skins/` рядом с VST3 bundle и включает canonical registry, legacy schema и REV14 control schema. Количество тем берётся из registry, а не из фиксированного числа. Полный ZIP публикуется только после Windows x64 Release, полного CTest, pluginval strictness 5 и успешной упаковки. Это всё ещё не заменяет ручную проверку загрузки/переключения в FL Studio 20.

## Artwork и host evidence

PNG/SVG assets — ресурсы skin pack, но сами по себе не доказывают внешний вид или корректность собранного плагина. Единственное доказательство реального UI — свежий capture собранного VST3 в host с указанием commit SHA и CI run. Не маркировать дизайн-макет как screenshot плагина.

## Связанные документы

- `../README.md` — продукт.
- `SKIN_ENGINE_GAP_AUDIT_RU.md` — оставшиеся runtime gaps.
- `CONTROL_INVENTORY_MATRIX_RU.md` — фактические controls.
- `../docs/MASTER_PRODUCT_SPEC_4_0_RU.md` — продуктовая спецификация.

> **ОБЯЗАТЕЛЬНОЕ ПРАВИЛО РОЛЕЙ ТЕМ (13.11.2026):** SIGNAL FORGE / NEON — единственная основная тема продукта, DEFAULT/startup/fallback. **BLACKOUT VOLTAGE (`skin-51`) — только дополнительная тема**: не DEFAULT, не startup и не fallback. Сохранить её ID, имя, папку и ресурсы без переименования/перенумерации. Это правило действует для всех разделов проекта, включая реестр, ThemeManager, галерею, тесты, документацию и сборку. Текущий `skin-51` default/fallback в исходниках/манифесте — зафиксированное несоответствие, исправлять после завершения аудита Фазы 0; до этого код и манифесты не менять.
