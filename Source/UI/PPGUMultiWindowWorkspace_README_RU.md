# PPGU Multi-Window Workspace

JUCE child-component workspace for independently openable and closable panels inside the plugin editor.

## Current scope
- Stable panel IDs; duplicate or invalid registrations are rejected.
- Each panel has its own title bar, close (×), maximize/restore and resize grip.
- One open panel expands to the available workspace; multiple panels are tiled using the tested, JUCE-free `AuxiliaryPanelLayout` geometry module.
- Layout is deterministic: two columns, then up to three rows (2×3 for five or six panels), subject to each panel's minimum size and DPI scale.
- When the available area cannot fit all panel minimums, every panel remains available on a scrollable surface; no panel is silently dropped.
- Per-panel bounds persistence and palette-driven surface colours.
- Workspace owns UI components only; it has no dependency on processor, APVTS, MIDI, sampler, DSP or audio callback.

## Integration
The workspace is registered in `PluginEditor` and currently hosts the real FX EDIT, AutoChords and Import Guitar panels. Other product panels are added only when their real implementation is available; placeholders are not presented as completed features.

## Verification boundary
The pure geometry contract is covered by `AuxiliaryPanelLayoutTests`. The JUCE workspace integration requires the Windows x64 CI build/CTest on the exact commit SHA and visual acceptance at 100/125/150% DPI plus FL Studio 20 host review.
