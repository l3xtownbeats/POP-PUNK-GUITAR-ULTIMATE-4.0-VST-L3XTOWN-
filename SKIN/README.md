# PPGU — skin manifests, palette runtime и Skin Studio

## REV.18 — canonical identity correction (2026-11-15)

- Binding identity: **SKIN-1 = NEON STAGE = skin-053 / Skin_053**. Stable ID and numeric index 53 are preserved; no IDs are renumbered or deleted.
- `theme_manifest.json` now selects `skin-053` as default; the registry entry is named `NEON STAGE`.
- `Skin_053` has the 21-token graphite / cyan / electric-blue / violet / magenta palette and new vector bezel, crest and texture assets. Default display mode is MINIMALISM; premium effects are opt-in.
- The previous 102-entry registry is preserved at `archive/theme_manifest.legacy-102.json`; legacy source folders and assets are retained but excluded from the active factory gallery.
- This branch is an artwork/identity correction, not a host-verified release. JUCE build, runtime ThemeManager load, G1–G4, PNG CRC, pluginval and FL Studio 20 capture remain mandatory.

## Historical implementation notes

Earlier REV.17 notes below are historical and do not override the REV.18 identity contract.
- `ThemeManager` поддерживает legacy nested-token schema для Skin_01…51 и Skin Studio 21-token palette/control-map schema для Skin_052+.
- Кнопка **SKIN** и Ctrl+K открывают встроенную в Multi-Window Workspace **Premium Skin Gallery**: карточки с preview, именем, семейством/layout, активной темой, поиском и фильтром семейств. Нажатие на карточку применяет тему и сохраняет stable ID.
- Preview PNG декодируются лениво по мере отображения карточек. Отсутствующая preview-картинка даёт fallback-card, а не блокирует выбор палитры.
- Historical registry state contained 102 entries. The active factory registry has now been migrated to the exact approved 16 themes with a separate logical `ordinal` and immutable stable `id`/`index`. The previous 102-entry registry and 50-theme pack manifest are preserved under `SKIN/archive/` and are not runtime gallery entries.

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
- `archive/theme_pack_manifest.legacy-50.json` — archived historical 50-theme pack; not an active factory manifest.
- `Skin_NNN/theme.json` — новая тема; PNG preview/hero и SVG texture/bezel/crest находятся рядом в соответствующих подкаталогах.
- `archive/` — no-loss backups of the former 102-entry registry and historical 50-theme pack; archived records are not runtime themes.

## Approved 16-skin artwork pipeline

The old 50-theme batch generator/merge pipeline is historical and must not be used to add factory themes. The approved factory sequence is fixed at 16.

From the repository root:

```bash
python -m pip install Pillow==12.3.0
python tools/render_approved_skin_artwork.py --root SKIN
python tools/audit_skin_rev18_assets.py --root SKIN --strict-assets
python tests/ValidateSkinManifests.py
```

The renderer reads the approved sequence and each theme's own palette/material identity. It creates only missing 360x240 preview and 800x450 hero PNGs, preserves existing assets by default, and does not alter manifests or functional controls. The strict audit checks the complete 16-theme sequence, SVG safety, PNG dimensions/CRC, palette, control-map parity and active registry mapping.

The previous 102 registry entries and 50-theme pack are archived, not deleted. Their source folders remain untouched and are excluded from the active gallery. Do not run the legacy 50-theme generator or merge scripts against the approved 16-theme registry.

## Release package

Windows workflow собирает sibling runtime folder `VST3/Skins/` рядом с VST3 bundle через `tools/package_active_skins.py`. Скрипт копирует только active `theme_manifest.json` и 16 folders, перечисленных в нём, включая их theme.json + preview/hero/texture/bezel/crest assets; control schema, sequence metadata, архивные/legacy folders и 102 старые записи в runtime package не попадают. Полный ZIP публикуется только после Windows x64 Release, полного CTest, pluginval strictness 5 и успешной упаковки. Это всё ещё не заменяет ручную проверку загрузки/переключения в FL Studio 20.

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
| 02 | OBSIDIAN GOLD | `skin-103` / `Skin_103` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 03 | CARBON FORGE | `skin-104` / `Skin_104` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 04 | MIDNIGHT CRIMSON | `skin-105` / `Skin_105` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 05 | ARCTIC PLATINUM | `skin-106` / `Skin_106` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 06 | VIOLET NOIR | `skin-107` / `Skin_107` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 07 | TITANIUM STORM | `skin-108` / `Skin_108` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 08 | EMERALD CIRCUIT | `skin-109` / `Skin_109` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 09 | SOLAR FLARE | `skin-110` / `Skin_110` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 10 | DEEP OCEAN | `skin-111` / `Skin_111` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 11 | ROSE CHROME | `skin-112` / `Skin_112` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 12 | DESERT AMBER | `skin-113` / `Skin_113` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 13 | GLACIER TEAL | `skin-114` / `Skin_114` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 14 | PHANTOM GRAPHITE | `skin-115` / `Skin_115` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 15 | ULTRAVIOLET DRIVE | `skin-116` / `Skin_116` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |
| 16 | VOLCANIC STEEL | `skin-117` / `Skin_117` | Theme spec + 3 SVG foundations + preview/hero PNG; strict asset audit PASS |

## Per-skin visual contract

- Each theme has its own material identity, palette, texture, bezel, crest and individual art-direction description. Do not reduce the sequence to one template with 15 accent-colour changes.
- Preserve the canonical NEON STAGE component language and SIGNAL FORGE CONTROL DECK functional structure. The graphic skin must not change control positions, parameter identity or behaviour.
- Preserve the exact 21 palette tokens, existing control-map contract, `reduceInEco=true` and `hotSwapFadeMs=150`.
- No APVTS, DSP, sampler, MIDI, preset, audio-callback, parameter-ID or functional-layout changes are part of a skin-only task.
- Artwork assets are not screenshots of the running VST3. Runtime acceptance requires a fresh host capture with the exact commit SHA and CI run.

## Current completion gates

1. **Theme specifications:** all 15 additional theme manifests exist; exact approved names, 21 palette tokens, control-map entries and individual art-direction descriptions have been checked.
2. **Vector foundations:** all 45 SVG texture/bezel/crest files for SKIN 02–16 are present. SVG safety and final visual review remain part of acceptance.
3. **Raster artwork:** all 30 required `assets/preview.png` (360×240) and `assets/hero.png` (800×450) files for SKIN 02–16 have been generated and committed. The strict asset audit reports 0 errors and 0 warnings; PNG dimensions and CRC are verified.
4. **Registry migration:** complete on the working branch. The active registry now contains exactly 16 factory themes; the former 102-entry registry and 50-theme pack are preserved in `SKIN/archive/`. Legacy source folders were not deleted.
5. **Runtime mapping:** implemented on the working branch. The registry carries explicit logical ordinals 01–16 and immutable stable IDs `skin-053` and `skin-103…skin-117`; the editor resolves stable IDs to ordinals instead of parsing the ID suffix as a gallery index.
6. **Acceptance:** the strict 16-theme asset audit and registry/control-map contract test pass. SVG texture/bezel/crest decoding and drawing have been wired into ThemeManager, the main console, gallery and auxiliary panels on this branch. Still required: Windows x64 compilation of that code, real VST3 visual inspection, package-content verification, full CTest, pluginval strictness 5, 100/125/150% DPI checks, then manual FL Studio 20 preview/apply/cancel/persistence and audio-state invariance checks.

The 15 themes now have complete generated artwork asset packs and a passing strict asset audit. Their SVG layer loading/rendering has been wired into the UI source on the working branch, but that C++ change is not yet build-verified or host-captured. They are **not** declared host-verified or release-approved until the build, package, performance and host gates pass. Existing legacy assets remain preserved in place and their former registry/pack manifests are archived.
