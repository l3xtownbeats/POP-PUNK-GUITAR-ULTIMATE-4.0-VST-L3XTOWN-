# PPGU SKIN packs

The full 50-slot manifest catalog is authored: Skin_01 is Neon Riot (default), Skin_02–Skin_50 are 49 new identities.

Each pack is independent and contains a complete `theme.json` with explicit token values. Preview, hero and optional texture paths are specifications; image files are not included until separately generated and validated. See ASSET_SPECIFICATIONS_RU.md for exact per-theme briefs and CONTROL_INVENTORY_MATRIX_RU.md for the shared functional contract. PR24_INTEGRATION_REVIEW_RU.md records why PR #24 is not safe to merge as-is.

## Runtime contract
- The SKIN selector is the only user-facing theme entry point.
- Theme changes are presentation-only: no APVTS parameter, audio engine, preset, MELODY/PATTERN or layout changes.
- Preserve hit areas, control IDs, attachment bindings, focus/hover/pressed/disabled states and scaling.
- Missing/invalid theme data must fall back to the built-in default without interrupting audio.
- Do not mark a theme accepted until Windows x64 build, automated tests and FL Studio 20 host checks pass.

## Status
Manifests authored; artwork assets and runtime integration are pending.
