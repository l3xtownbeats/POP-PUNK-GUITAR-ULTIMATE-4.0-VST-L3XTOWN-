# SKIN-08 — EMERALD CIRCUIT
## Individual Premium Commercial Upgrade Brief

**Stable identity:** `skin-109`  
**Canonical folder:** `Skin_109`  
**Baseline material identity:** black ceramic and emerald circuitry  
**Status:** APPROVED INDIVIDUAL EXECUTION BRIEF — visual implementation and acceptance still pending unless evidenced elsewhere.

### Individual creative direction
Create a distinctive, luxury-grade commercial VST interface around this skin's own identity. deep black ceramic, fine emerald circuit traces used only as ornamental surface detailing, precision green-lit indicators and restrained translucent technical overlays. Circuit artwork must be decorative and must not imply extra controls or signal-routing functions.

Do not copy another skin's visual treatment. This skin must remain immediately distinguishable in the gallery by its material language, lighting behavior, surface detailing and composition—not merely by changing the accent color. Preserve the approved name, stable ID, index, folder, base layout and existing palette identity. Refine the palette only where needed for professional contrast and accessibility; document any token change.

### Required craft
- Build a coherent layered console: base chassis, inset panel planes, control wells, machined edges, separators and restrained depth.
- Give each existing knob a convincing material face, calibrated indicator, readable scale and consistent state feedback. Do not add labels or parameter identities not present in the canonical inventory.
- Give existing FX switches clear and consistent off/on/hover/focus/disabled states; only the seven approved FX bypass controls may be represented as bypass controls.
- Make LCD and LED treatments legible at normal plugin size and at supported scaling factors.
- Apply typography, spacing, icon weight, corner radii and edge treatment consistently. Maintain clear hierarchy between navigation, primary performance surface and secondary controls.
- Use high-quality SVG/vector construction for scalable interface parts; create the required preview and hero raster assets from the approved final artwork.
- Lighting and glow are opt-in accents, not permanent haze. Preserve the default minimal mode and Eco-mode reduction.
- Decorative marks must never resemble additional buttons, meters, routing nodes, switches or functional readouts.

### SIGNAL FORGE integration
This skin is a GRAPHIC SKIN only. SIGNAL FORGE remains the one CONTROL DECK and functional source of truth. Skin every applicable canonical component, including PLAYING CORE, FX CHAIN, FX EDIT, AUTOCHORDS, Import Guitar, Settings, About and Performance when those views exist in the working REV.19 implementation. If a view is not yet implemented, report the coverage gap; do not fabricate it. Reuse real component rendering/state bindings. Do not fork behavior per skin.

### Immutable technical constraints
- Exact 27-control parity.
- Only the seven approved FX bypass IDs.
- No inferred/new APVTS IDs; no ID changes.
- No invented controls, functions, windows or runtime states.
- No separate SOUND FORGE or SIGNAL FORGE theme.
- No audio, DSP, MIDI, sampler, preset, parameter, layout or functional-architecture changes.
- Follow the canonical REV.19 inventory, Info.txt, theme schema and NEON STAGE reference.
- Preserve stable ID/index/folder identity and approved control geometry.

### Required deliverables
1. Finished individual theme artwork and editable vector source.
2. `assets/preview.png` at 360×240 and `assets/hero.png` at 800×450, generated from the same accepted visual design.
3. Skin-specific SVG `texture.svg`, `bezel.svg` and `crest.svg` with unique treatment consistent with this brief.
4. Updated theme tokens only where justified; valid schema and existing runtime bindings.
5. Asset manifest/checksums and a visual comparison against canonical NEON STAGE layout.
6. Per-skin QA record for G1 identity, G2 assets, G3 parity/contracts and G4 runtime behavior.

### Acceptance rule
Do not mark this skin complete because its theme.json or SVG concept exists. Completion requires finished assets, verified package paths/integrity, exact control/state parity, runtime display/switching/scale checks and recorded evidence. Windows x64 VST3 build, CTest, pluginval Strictness 5 and FL Studio 20 host acceptance must be recorded separately and only when executed.

**Creative identity to protect:** EMERALD CIRCUIT — black ceramic and emerald circuitry.