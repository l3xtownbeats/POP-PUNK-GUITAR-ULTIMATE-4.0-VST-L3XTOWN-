# PPGU — skin manifests, palette runtime и Skin Studio

## REV.18 — canonical identity correction (2026-11-15)

- Binding identity: **SKIN-1 = NEON STAGE DEFAULT = skin-053 / Skin_053**. Stable ID and numeric index 53 are preserved; no IDs are renumbered or deleted.
- `theme_manifest.json` now selects `skin-053` as default; the registry entry is named `NEON STAGE DEFAULT`.
- `Skin_053` has the 21-token graphite / cyan / electric-blue / violet / magenta palette and new vector bezel, crest and texture assets. Default display mode is MINIMALISM; premium effects are opt-in.
- `skin-052` and all legacy records remain untouched as historical data.
- This branch is an artwork/identity correction, not a host-verified release. JUCE build, runtime ThemeManager load, G1–G4, PNG CRC, pluginval and FL Studio 20 capture remain mandatory.

## Historical implementation notes

Earlier REV.17 notes below are historical and do not override the REV.18 identity contract.
- `ThemeManager` поддерживает legacy nested-token schema для Skin_01…51 и Skin Studio 21-token palette/control-map schema для Skin_052+.
- Кнопка **SKIN** и Ctrl+K открывают встроенную в Multi-Window Workspace **Premium Skin Gallery**: карточки с preview, именем, семейством/layout, активной темой, поиском и фильтром семейств. Нажатие на карточку применяет тему и сохраняет stable ID.
- Preview PNG декодируются лениво по мере отображения карточек. Отсутствующая preview-картинка даёт fallback-card, а не блокирует выбор палитры.
- Skin_053–102 с PNG preview/hero и SVG bezel/crest/texture опубликованы в commit `3a2ceb52062be112429f96d411ae223d87df17a6`; workflow `43195205929` завершился SUCCESS. Registry: 102 entries, pack state MERGED, default skin-052.

Оставшиеся ограничения, которые нельзя выдавать за готовые:
- Favorites, полноценная клавиатурная навигация внутри gallery и фоновой filesystem watcher/rescan не реализованы.
- `controls.*.grid` остаётся design metadata и не перестраивает реальные JUCE controls.
- 150 ms animated hot-swap, ECO animation policy и asset memory LRU/budget не подтверждены.
- Не все декоративные материалы Skin Studio (texture/bezel/crest) загружаются как отдельные runtime layers; gallery использует preview artwork, а editor и все открытые auxiliary panels получают semantic palette через theme-aware panel contract.
- Runtime control parity по всем обязательным IDs остаётся частичной: отсутствующие LED/Auto Discover/Curator/About не становятся активными только из-за skin manifest. Settings panel имеет реальные UI-only настройки workspace.
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


## REV.18 — approved sequential theme concepts (2026-11-15)

The approved logical theme sequence is recorded in `approved_theme_sequence.json`:

| Logical theme | Name | Immutable stable ID / registry index | Current asset state |
|---|---|---|---|
| SKIN-1 | NEON STAGE DEFAULT | `skin-053` / 53 | Canonical default |
| SKIN-2 | OBSIDIAN GOLD | `skin-103` / 103 | Manifest + 3 SVG foundations; PNG pending |
| SKIN-3 | CARBON FORGE | `skin-104` / 104 | Manifest + 3 SVG foundations; PNG pending |
| SKIN-4 | MIDNIGHT CRIMSON | `skin-105` / 105 | Manifest + 3 SVG foundations; PNG pending |
| SKIN-5 | ARCTIC PLATINUM | `skin-106` / 106 | Manifest + 3 SVG foundations; PNG pending |
| SKIN-6 | VIOLET NOIR | `skin-107` / 107 | Manifest + 3 SVG foundations; PNG pending |

The legacy registry already contains indices 1–102. Logical SKIN numbering is therefore mapped to new stable IDs 103–107; no old directory, ID, registry entry or asset is renamed or overwritten.

All five new theme manifests preserve the 21-token palette contract and the same 28-entry control map. They vary only palette/material tokens and retain the shared NEON STAGE component language. No APVTS, DSP, MIDI, audio, parameter/control ID or live layout changes are authorized.

**Important:** these are approved design specifications with vector texture/bezel/crest foundations. They are not yet complete gallery/runtime themes. PNG preview/hero assets, G1–G4 and CRC validation, registry/gallery registration, package/runtime loading and host acceptance remain pending. Do not represent them as available in the plugin until those gates pass.


### REV.18 — SKIN-7…SKIN-16 distinct vector asset correction

Each of Skin_108…Skin_117 now contains three separate palette-derived SVG files: `svg/texture.svg`, `svg/bezel.svg`, and `svg/crest.svg`. The `effects.*` and `assets.*` references in each theme manifest point to the matching resource. These are vector foundations, not finished gallery cards or screenshots.

Still pending for SKIN-7…SKIN-16: exact-size `assets/preview.png` and `assets/hero.png`, G1–G4/CRC validation, canonical registry registration, runtime gallery and asset-load checks, exact-SHA Windows x64 VST3 build, CTest, pluginval strictness 5 and FL Studio 20 host acceptance. The themes are not runtime/release ready.
