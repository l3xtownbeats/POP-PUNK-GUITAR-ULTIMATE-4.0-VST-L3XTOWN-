# PPGU SKIN packs

50 skin slots are reserved. Skin_01 is Neon Riot. This first authored batch contains Skin_02–Skin_11.

Each pack is independent and contains a complete `theme.json` with explicit token values. Preview and hero image paths are specifications; image files are not included until separately generated and validated.

## Runtime contract
- The SKIN selector is the only user-facing theme entry point.
- Theme changes are presentation-only: no APVTS parameter, audio engine, preset, MELODY/PATTERN or layout changes.
- Preserve hit areas, control IDs, attachment bindings, focus/hover/pressed/disabled states and scaling.
- Missing/invalid theme data must fall back to the built-in default without interrupting audio.
- Do not mark a theme accepted until Windows x64 build, automated tests and FL Studio 20 host checks pass.

## Status
Manifests authored; artwork assets and runtime integration are pending.
