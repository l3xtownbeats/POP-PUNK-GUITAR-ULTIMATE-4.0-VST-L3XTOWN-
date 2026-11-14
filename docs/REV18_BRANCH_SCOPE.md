# REV.18 — Working Branch Scope and Exclusions
**Branch:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV.18`  
**Initialized from:** REV.17 `6b7f847976464404d636b94d963540303a8ad4d3`  
**Purpose:** clean, isolated, active development line. This is not a release branch.

## Retained
- JUCE/CMake project, production C++ sources and headers.
- Unit/integration/security tests and test assets.
- Active GitHub Actions workflows and build configuration.
- Complete current SKIN packs/manifests/assets and UI/theme specifications.
- Current master product specification, SIGNAL FORGE engineering standard, approved parameters, audio/sampler/preset/import/licensing documentation.
- Info.txt, CODE.txt, Development Control Center, Master Implementation Register and supporting active subsystem docs.
- Factory preset definitions/examples and sound-asset registry/policy files (their presence does not certify legal redistribution or playable validation).
- Existing in-repository visual assets and the 38 separately preserved user design references indexed in `PPGU_REFERENCE_DESIGN_ASSET_REGISTER.md`.

## Excluded from the active working tree in this initialization
The following archival/packaging duplicates were removed from the REV.18 current tree; their content remains in prior Git history and REV.17 is unchanged:
- `PPGU_CC0_PACK (2).zip` — bundled archive; not a cleared distributable library.
- `PPGU_FULL_BUNDLE (1).zip`, `PPGU_VST3_Pack (2).zip`, `PPGU_TREE_2026-11-13.zip`, `build.zip` — duplicate snapshots/build/package artifacts, not active source inputs.
- `CODE_APPEND_2026-11-13.txt`, `INFO_APPEND_2026-11-13*.txt` — append fragments already consolidated into the canonical journals.

## Deliberately not removed
- Source files, tests, build workflows, runtime SKIN resources, current product specs and subsystem docs.
- Legacy/historical documents with potential decision/evidence value, even if not runtime inputs.
- Root image references already present in Git. Their approval status remains subject to the design asset register and decision ledger.

## Safety
No sample archive was treated as cleared for redistribution. No code was rewritten, renamed or merged as part of this cleanup. The REV.17 branch remains unchanged. Any further cleanup requires a path-level dependency check and journal entry.
