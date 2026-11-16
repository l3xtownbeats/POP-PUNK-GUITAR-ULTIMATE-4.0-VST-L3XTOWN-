# PPGU — skin manifests, palette runtime и Skin Studio

## REV.18 — canonical identity correction (2026-11-15)

- Binding identity: **SKIN-1 = NEON STAGE = skin-053 / Skin_053**. Stable ID and numeric index 53 are preserved; no IDs are renumbered or deleted.
- `theme_manifest.json` now selects `skin-053` as default; the registry entry is named `NEON STAGE`.
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


## Approved factory sequence — 16 GRAPHIC SKINS (2026-11-16)

The following 16 names and their order are canonical. The stable IDs remain immutable and are deliberately distinct from the logical factory ordinals.

| Factory ordinal | Approved name | Stable ID / source folder | Current verified source state |
|---:|---|---|---|
| 01 | NEON STAGE | `skin-053` / `Skin_053` | Canonical visual baseline; preview/hero PNG present |
| 02 | OBSIDIAN GOLD | `skin-103` / `Skin_103` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 03 | CARBON FORGE | `skin-104` / `Skin_104` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 04 | MIDNIGHT CRIMSON | `skin-105` / `Skin_105` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 05 | ARCTIC PLATINUM | `skin-106` / `Skin_106` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 06 | VIOLET NOIR | `skin-107` / `Skin_107` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 07 | TITANIUM STORM | `skin-108` / `Skin_108` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 08 | EMERALD CIRCUIT | `skin-109` / `Skin_109` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 09 | SOLAR FLARE | `skin-110` / `Skin_110` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 10 | DEEP OCEAN | `skin-111` / `Skin_111` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 11 | ROSE CHROME | `skin-112` / `Skin_112` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 12 | DESERT AMBER | `skin-113` / `Skin_113` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 13 | GLACIER TEAL | `skin-114` / `Skin_114` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 14 | PHANTOM GRAPHITE | `skin-115` / `Skin_115` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 15 | ULTRAVIOLET DRIVE | `skin-116` / `Skin_116` | Theme spec + 3 SVG foundations; preview/hero PNG missing |
| 16 | VOLCANIC STEEL | `skin-117` / `Skin_117` | Theme spec + 3 SVG foundations; preview/hero PNG missing |

## Per-skin visual contract

- Each theme has its own material identity, palette, texture, bezel, crest and individual art-direction description. Do not reduce the sequence to one template with 15 accent-colour changes.
- Preserve the canonical NEON STAGE component language and SIGNAL FORGE CONTROL DECK functional structure. The graphic skin must not change control positions, parameter identity or behaviour.
- Preserve the exact 21 palette tokens, existing control-map contract, `reduceInEco=true` and `hotSwapFadeMs=150`.
- No APVTS, DSP, sampler, MIDI, preset, audio-callback, parameter-ID or functional-layout changes are part of a skin-only task.
- Artwork assets are not screenshots of the running VST3. Runtime acceptance requires a fresh host capture with the exact commit SHA and CI run.

## Current completion gates

1. **Theme specifications:** all 15 additional theme manifests exist; exact approved names, 21 palette tokens, control-map entries and individual art-direction descriptions have been checked.
2. **Vector foundations:** all 45 SVG texture/bezel/crest files for SKIN 02–16 are present. SVG safety and final visual review remain part of acceptance.
3. **Raster artwork:** all 30 required `assets/preview.png` (360×240) and `assets/hero.png` (800×450) files for SKIN 02–16 are currently missing. This is the immediate artwork blocker.
4. **Registry migration:** the active registry still contains the old 102 entries. Back up the old registry and safely migrate the active gallery to exactly these 16 factory skins; do not delete legacy source folders or overwrite stable IDs.
5. **Runtime mapping:** update registry/loader handling so logical ordinals 01–16 resolve to immutable stable IDs `skin-053` and `skin-103…skin-117`. Do not assume an ID suffix is the gallery ordinal.
6. **Acceptance:** run the read-only 16-theme asset audit, registry and control-map tests, Windows x64 VST3 build, full CTest, pluginval strictness 5, then manual FL Studio 20 preview/apply/cancel/persistence and audio-state invariance checks.

The 15 themes are **not** declared GRAPHICS COMPLETE, INTEGRATED, VERIFIED or APPROVED until their individual gates pass. Existing legacy assets are preserved until the no-loss registry migration is validated.
