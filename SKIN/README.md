# PPGU — SKIN / NEON STAGE theme system

## Fixed product boundary

The product has **exactly 16 approved factory themes**. No historical/legacy theme pack is part of the product, runtime gallery, installer, or future generation pipeline.

- **NEON STAGE** is the shared GUI design system and visual language for the plugin.
- **SKIN-1 — NEON FORGE (DEFAULT)** is the existing canonical factory theme, stable ID `skin-053`, folder `Skin_053/`. It is the default at startup.
- **SKIN-2…SKIN-16** are the other fifteen approved factory themes, with immutable names/order/stable IDs defined in `approved_theme_sequence.json`.

The distinction above is intentional: NEON STAGE names the overall interface system; NEON FORGE is the first individual theme.

## Approved factory sequence

| Logical ID | Factory theme | Stable ID / folder | Current status |
|---|---|---|---|
| SKIN-1 | NEON FORGE (DEFAULT) | `skin-053` / `Skin_053` | Existing canonical default |
| SKIN-2 | OBSIDIAN GOLD | `skin-103` / `Skin_103` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-3 | CARBON FORGE | `skin-104` / `Skin_104` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-4 | MIDNIGHT CRIMSON | `skin-105` / `Skin_105` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-5 | ARCTIC PLATINUM | `skin-106` / `Skin_106` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-6 | VIOLET NOIR | `skin-107` / `Skin_107` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-7 | TITANIUM STORM | `skin-108` / `Skin_108` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-8 | EMERALD CIRCUIT | `skin-109` / `Skin_109` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-9 | SOLAR FLARE | `skin-110` / `Skin_110` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-10 | DEEP OCEAN | `skin-111` / `Skin_111` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-11 | ROSE CHROME | `skin-112` / `Skin_112` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-12 | DESERT AMBER | `skin-113` / `Skin_113` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-13 | GLACIER TEAL | `skin-114` / `Skin_114` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-14 | PHANTOM GRAPHITE | `skin-115` / `Skin_115` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-15 | ULTRAVIOLET DRIVE | `skin-116` / `Skin_116` | Approved specification/SVG; finish artwork and acceptance |
| SKIN-16 | VOLCANIC STEEL | `skin-117` / `Skin_117` | Approved specification/SVG; finish artwork and acceptance |

## What is implemented versus pending

- The active runtime registry intentionally exposes only SKIN-1 until the other fifteen themes pass their artwork, asset, runtime and host acceptance gates.
- SKIN-2…SKIN-16 already have approved theme specifications and SVG foundations. This does **not** mean all fifteen are complete gallery-ready themes.
- Remaining work per theme: finished preview/hero artwork, asset integrity and G1–G4 checks, registry/gallery promotion, runtime theme switching, exact-SHA Windows x64 build, CTest, pluginval Strictness 5 and FL Studio 20 acceptance.
- The active registry `theme_manifest.json` is the sole source of themes currently selectable in the plugin. The approved sequence file is the source of the fixed sixteen-theme product order.

## Historical theme cleanup

The old 101 non-product theme records and their archived asset folders have been removed from the current branch. The former 102-entry registry snapshots and obsolete pack snapshots have also been removed from the current working tree. They are not shipped, scanned, shown, loaded, or counted as factory themes.

This is a source-tree cleanup, not a rewrite of Git history: prior commits retain normal repository history for audit/recovery. No historical theme is to be restored or regenerated into the product without a new explicit user decision.

## Safety and compatibility rules

- Do not create theme 17 or generate arbitrary 50-theme batches.
- Do not change the approved sixteen names, order, stable IDs, or folder paths.
- Do not promote SKIN-2…SKIN-16 into the runtime registry before all acceptance gates pass.
- Theme changes are visual-only. Preserve APVTS IDs/bindings, the 27-control parity contract, seven approved FX bypass IDs, processor state, DSP, MIDI and audio callback behavior.
- `theme.schema.json` remains a schema/compatibility contract; it is not a collection of legacy themes.
- `control_schema.json` defines the shared 21-token palette/control contract.
- `approved_theme_sequence.json` is the immutable factory sequence.
- `theme_pack_manifest.json` is retired and intentionally empty; batch generation and merge scripts are disabled.

## Validation

Run:

```bash
python tests/ValidateSkinManifests.py
python tools/audit_skin_rev18_assets.py --root SKIN
python tests/test_skin_asset_audit.py
```

A passing source validator does not replace Windows x64 VST3 build, CTest, pluginval, visual comparison or FL Studio 20 host acceptance.
