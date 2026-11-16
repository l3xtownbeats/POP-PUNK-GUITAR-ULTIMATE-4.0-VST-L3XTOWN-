# PREMIUM SKIN PERFORMANCE & SAFE-SWITCHING ADDENDUM
## Approved addition to the locked PREMIUM COMMERCIAL SKIN UPGRADE baseline

**Project:** POP PUNK GUITAR ULTIMATE  
**Scope:** All 16 GRAPHIC SKINS, including NEON STAGE  
**Status:** APPROVED ADDITIVE REQUIREMENT — mandatory alongside the master baseline  
**Recorded:** 2026-11-16

This addendum supplements the previously approved master directive and baseline approval. It does not replace, weaken or supersede any existing requirement. All sixteen skins must achieve premium visual quality while remaining exceptionally lightweight and performance-efficient.

## 1. Performance objective
Every skin must be optimized to minimize additional:
- RAM usage and retained asset memory;
- CPU load during normal display, interaction and skin changes;
- GPU/rendering workload where applicable;
- disk and resource-loading overhead;
- startup and skin-switch latency.

The visual upgrade must not turn the instrument into a resource-heavy interface. Premium appearance must be achieved through efficient vector artwork, carefully optimized raster assets, restrained effects and shared rendering infrastructure—not expensive real-time effects or oversized duplicated resources.

## 2. One shared rendering foundation
- All skins use the same SIGNAL FORGE CONTROL DECK components, layout, state model and rendering implementation.
- Skins supply visual tokens and assets only; do not duplicate widgets, control logic, parameter state or rendering engines per skin.
- Reuse common geometry, component code, fonts, icons and neutral assets wherever possible.
- Store genuinely skin-specific assets separately, but avoid duplicating common resources into every skin package.
- Preserve the exact approved 27-control parity and seven FX Bypass IDs. This optimization must not change any functional contract.

## 3. Asset and memory optimization
- Optimize every SVG: remove editor metadata, redundant nodes, unnecessary filters, oversized embedded data and excessive path complexity while preserving visual fidelity.
- Optimize PNG/raster assets for their actual display dimensions; avoid oversized images and unnecessary alpha channels.
- Prefer scalable vectors for simple interface geometry and icons; use raster artwork only where it materially improves visual quality.
- Avoid large uncompressed textures, full-window background bitmaps and duplicate copies of identical assets.
- Load only the active skin's required assets plus a deliberately bounded cache. Do not keep all sixteen complete skin asset sets resident in RAM by default.
- Release obsolete skin resources safely after a successful switch. Cache eviction must never invalidate resources currently used by the renderer.
- Keep memory usage predictable and document measured per-skin and shared-resource footprints.

## 4. CPU and rendering optimization
- No continuous animation, animated texture, blur, bloom, particle effect or expensive shader/filter should run merely because a skin is selected.
- Default appearance must be static and low-cost. Optional glow and decorative effects must be restrained and disabled or reduced in Eco/performance mode.
- Avoid repeated SVG parsing, image decoding, font loading, path construction or expensive layout calculations during ordinary repaint.
- Reuse cached, immutable render resources where safe; invalidate only what actually changes.
- Avoid unnecessary repainting and per-frame allocations. Do not perform blocking file I/O, asset decoding, memory allocation or expensive skin construction on the audio thread.
- Keep audio processing independent from skin loading, rendering and switching.

## 5. Safe, non-disruptive skin switching
Changing a skin must be a visual-only operation and must not interrupt or alter instrument operation.

The switch process must:
1. Prepare and validate the requested skin resources away from the audio thread.
2. Keep the currently active skin visible and valid while preparation is in progress.
3. Commit the new visual resource set atomically or through an equivalent safe UI-thread handoff.
4. Preserve all parameter values, automation, MIDI state/flow, preset state, keyboard interaction, voice playback, FX states and audio continuity.
5. Avoid clicks, audio dropouts, stuck notes, lost automation, temporary blank controls, crashes, deadlocks and invalid resource references.
6. Release old resources only after no renderer can still reference them.
7. If loading or validation fails, retain or restore the previous working skin and report the error without affecting audio or controls.

Skin switching must not reset the instrument, reload presets, reconstruct the audio engine or change any parameter/APVTS state.

## 6. Required performance and regression validation
Test all sixteen skins, not just NEON STAGE. Record:
- baseline and per-skin RAM footprint, including shared and incremental asset memory;
- CPU usage at idle, during interaction, during repaint and during repeated skin changes;
- initial skin load and subsequent switch latency;
- resource-cache growth and eviction behavior over repeated switching;
- behavior under memory pressure and failed/corrupt asset loading;
- long-session stability and repeated switch stress;
- audio continuity, MIDI, note release, automation, preset and FX-state preservation during switching;
- high-DPI/scaling and Eco-mode behavior;
- regression results on the supported Windows x64 VST3 target and FL Studio 20 host.

Use repeatable test conditions and record hardware, OS, host, build configuration, sample rate, buffer size, test duration and measured results. Set explicit numeric performance budgets from a measured canonical baseline before claiming optimization. Do not invent performance figures or mark unrun tests as passed.

## 7. Acceptance criteria
A skin is not accepted solely because it looks premium. It must also:
- meet the visual G1-G4 gates in the master baseline;
- pass asset integrity and bounded-memory checks;
- show no material or unexplained CPU/RAM regression against the agreed baseline;
- switch repeatedly without audio, MIDI, automation, control or stability regressions;
- preserve all approved architectural and parameter contracts.

**Binding rule:** Maximum visual quality and maximum practical efficiency are simultaneous requirements. No skin may be accepted by sacrificing responsiveness, stability, audio continuity or reasonable system-resource use.

**Relationship to prior approval:** This is an additive, mandatory part of the previously approved PREMIUM COMMERCIAL SKIN UPGRADE baseline for all 16 skins. All earlier requirements remain in force.
