# PPGU — skin manifests, palette runtime и Skin Studio

## Фактическое состояние — рабочая ветка REV.17

Актуальная рабочая ветка: `work/ppgu-skin-expansion-53-102`.

- Runtime registry содержит **102 темы**: legacy `Skin_01`…`Skin_51` и Skin Studio `Skin_052`…`Skin_102`.
- Каноническая тема по умолчанию и fallback — **SIGNAL FORGE (`skin-052`)**.
- `ThemeManager` поддерживает legacy nested-token schema для Skin_01…51 и Skin Studio 21-token palette/control-map schema для Skin_052+.
- Кнопка **SKIN** и Ctrl+K открывают встроенную в Multi-Window Workspace **Premium Skin Gallery**: карточки с preview, именем, семейством/layout, активной темой, поиском и фильтром семейств. Нажатие на карточку применяет тему и сохраняет stable ID.
- Preview PNG декодируются лениво по мере отображения карточек. Отсутствующая preview-картинка даёт fallback-card, а не блокирует выбор палитры.
- Skin_053–102 с PNG preview/hero и SVG bezel/crest/texture опубликованы в commit `3a2ceb52062be112429f96d411ae223d87df17a6`; workflow `43195205929` завершился SUCCESS. Registry: 102 entries, pack state MERGED, default skin-052.

Оставшиеся ограничения, которые нельзя выдавать за готовые:
- Favorites, полноценная клавиатурная навигация внутри gallery и фоновой filesystem watcher/rescan не реализованы.
- `controls.*.grid` остаётся design metadata и не перестраивает реальные JUCE controls.
- 150 ms animated hot-swap, ECO animation policy и asset memory LRU/budget не подтверждены.
- Не все декоративные материалы Skin Studio (texture/bezel/crest) загружаются как отдельные runtime layers; текущая gallery использует preview artwork, а editor — semantic palette.
- Runtime control parity по всем обязательным IDs остаётся частичной: отсутствующие LED/Auto Discover/Curator/Settings/About не становятся активными только из-за skin manifest.
- Windows x64 build/CTest/pluginval и FL Studio 20 host acceptance должны подтверждаться отдельно на точном SHA.

Все изменения тем остаются visual-only: не менять APVTS, MIDI, presets, sampler, DSP или audio callback.

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
