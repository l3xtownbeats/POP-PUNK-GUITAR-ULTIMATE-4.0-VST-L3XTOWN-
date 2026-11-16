# POP PUNK GUITAR ULTIMATE — PREMIUM COMMERCIAL SKIN UPGRADE
## MASTER BASELINE — ALL 16 GRAPHIC SKINS × SIGNAL FORGE CONTROL DECK

**Status: APPROVED / CONFIRMED / BASELINE / REQUIRED**

### Objective
Upgrade all sixteen approved factory GRAPHIC SKINS to a premium, luxury, professional, modern commercial VST-instrument visual standard. Improve composition, material definition, textures, micro-detail, lighting, shadows, panels, control faces, knobs, switches, displays, indicators, typography, iconography, separators, borders and visual hierarchy. Do not treat a palette swap or concept mockup as completion.

### Architecture (immutable)
- **SIGNAL FORGE = CONTROL DECK / FUNCTIONAL FOUNDATION.**
- **NEON STAGE and the other fifteen themes = GRAPHIC SKINS.**
- Every skin is a visual presentation of the same functional foundation. Skins may not create their own functional architecture, parameter model, state logic or behavior.
- Preserve the canonical NEON STAGE component language and the approved REV.19 layout/control map. Each skin retains a distinct material, composition and visual identity.

### Approved sequence and immutable identities
1. SKIN-1 NEON STAGE — `skin-053` / `Skin_053` (canonical visual reference).
2. SKIN-2 OBSIDIAN GOLD — `skin-103` / `Skin_103`.
3. SKIN-3 CARBON FORGE — `skin-104` / `Skin_104`.
4. SKIN-4 MIDNIGHT CRIMSON — `skin-105` / `Skin_105`.
5. SKIN-5 ARCTIC PLATINUM — `skin-106` / `Skin_106`.
6. SKIN-6 VIOLET NOIR — `skin-107` / `Skin_107`.
7. SKIN-7 TITANIUM STORM — `skin-108` / `Skin_108`.
8. SKIN-8 EMERALD CIRCUIT — `skin-109` / `Skin_109`.
9. SKIN-9 SOLAR FLARE — `skin-110` / `Skin_110`.
10. SKIN-10 DEEP OCEAN — `skin-111` / `Skin_111`.
11. SKIN-11 ROSE CHROME — `skin-112` / `Skin_112`.
12. SKIN-12 DESERT AMBER — `skin-113` / `Skin_113`.
13. SKIN-13 GLACIER TEAL — `skin-114` / `Skin_114`.
14. SKIN-14 PHANTOM GRAPHITE — `skin-115` / `Skin_115`.
15. SKIN-15 ULTRAVIOLET DRIVE — `skin-116` / `Skin_116`.
16. SKIN-16 VOLCANIC STEEL — `skin-117` / `Skin_117`.

Stable IDs, indices and folder names are immutable. The individual brief for each skin is mandatory; do not replace them with one universal visual prompt.

### Required visual coverage
The presentation must correctly skin every applicable existing surface and view: PLAYING CORE, FX CHAIN, FX EDIT, AUTOCHORDS, Import Guitar, Settings, About, Performance, preset/mode areas, all existing knobs, switches, LCD/display surfaces, LEDs, keyboard and navigation controls. Do not add a visual element unless it maps to an approved existing component. If a named view is absent from the current canonical implementation, record it as a dependency/coverage gap; do not invent a working feature.

### Hard technical contracts
- Preserve the **27-control parity contract** exactly.
- Use only the **seven approved FX bypass IDs**.
- Never derive or invent APVTS IDs from labels, mockups, artwork or screenshots.
- Do not add controls or unsupported components absent from the canonical REV.19 inventory and approved Info.txt.
- Do not create separate SOUND FORGE or SIGNAL FORGE themes.
- Do not portray unimplemented functionality as operational.
- No changes to audio engine, DSP, MIDI, sampler, presets, parameter behavior, APVTS IDs, control IDs or approved functional architecture as part of a visual-only skin upgrade.
- Theme changes affect visual tokens, materials and artwork only. Respect schema constraints, runtime binding, layout geometry, 150 ms hot-swap behavior and Eco-mode reduction unless canonical source explicitly says otherwise.

### Individual execution method
For each skin, separately:
1. Read its own theme specification, artwork/assets, current runtime state and canonical control map.
2. Record its original identity, palette, material concept, strengths, deficiencies and asset gaps.
3. Follow its dedicated individual brief in this directory. Do not homogenize the skins.
4. Produce final vector artwork and required raster deliverables at the specified sizes; retain editable source assets.
5. Validate asset paths, dimensions, transparency, SVG safety, PNG decoding/CRC and resource packaging.
6. Verify every applicable CONTROL DECK component uses the same real control/state binding as the canonical implementation.
7. Compare the rendered interface against NEON STAGE for layout/function parity while preserving this skin's distinct design identity.
8. Run visual QA for contrast, legibility, scaling, clipping, focus/hover/pressed/disabled/on/off states, and reduced-effects/Eco mode.
9. Integrate only after the skin passes the checks. A theme specification or artwork file alone is not an active runtime skin.

### Required acceptance gates (per skin)
- G1 — identity and individual concept fidelity.
- G2 — finished artwork/assets, correct paths, dimensions and integrity.
- G3 — exact CONTROL DECK parity and technical-contract compliance.
- G4 — runtime rendering, state coverage, switching, scaling and regression acceptance.
- Release gate — exact-SHA Windows x64 VST3 build, CTest, pluginval Strictness 5, and separate FL Studio 20 host acceptance. Do not claim host acceptance unless actually run.

### Status vocabulary
Track separately: CONCEPT / ARTWORK IN PROGRESS / ARTWORK COMPLETE / INTEGRATION IN PROGRESS / INTEGRATED / G1-G4 PASS / RELEASE VALIDATED. Never collapse these into a generic “done”.

### Reporting
For each skin record: commit SHA, changed files, asset inventory, dimensions/checksums, control-parity result, G1-G4 evidence, runtime/build/test results, known gaps and exact next action. An unrun test is NOT a pass.

**Final directive:** PREMIUM COMMERCIAL SKIN UPGRADE — ALL 16 SKINS; FULL SIGNAL FORGE CONTROL DECK INTEGRATION; INDIVIDUAL SKIN MODERNIZATION; MAXIMUM VISUAL QUALITY; PRESERVE APPROVED ARCHITECTURE; NO FUNCTIONAL DEVIATIONS.