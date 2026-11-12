# PPGU Multi-Window Workspace

JUCE child-component workspace for independently openable and closable panels inside the plugin editor.

## Current scope
- Stable panel IDs; duplicate or invalid registrations are rejected.
- Each panel has its own title bar, close (×), maximize/restore and resize grip.
- One open panel expands to the available workspace; multiple panels are tiled using the tested, JUCE-free `AuxiliaryPanelLayout` geometry module.
- Layout is deterministic: two columns, then up to three rows (2×3 for five or six panels), subject to each panel's minimum size and DPI scale.
- When the available area cannot fit all panel minimums, every panel is tiled at minimum size on a one- or two-column scrollable surface; no panel is silently dropped.
- Per-panel bounds persistence, palette-driven window chrome and live palette propagation into theme-aware panel content; newly opened panels inherit the current palette.
- Workspace owns UI components only; it has no dependency on processor, APVTS, MIDI, sampler, DSP or audio callback.

## Integration
The workspace is registered in `PluginEditor` and currently hosts the real FX EDIT, AutoChords and Import Guitar panels. Other product panels are added only when their real implementation is available; placeholders are not presented as completed features.

## Verification boundary
The pure geometry contract is covered by `AuxiliaryPanelLayoutTests`, including scroll overflow and no-overlap cases. Gallery and JUCE workspace integration require Windows x64 build/CTest on the exact SHA and visual acceptance at 100/125/150% DPI plus FL Studio 20 host review.
