# PPGU Multi-Window Workspace

JUCE child-component workspace for independently openable/closable auxiliary panels.

## Current scope
- Independent panel instances keyed by stable positive IDs.
- Per-panel close button (×).
- Automatic tiled layout using the existing pure geometry contract.
- Adaptive compaction as additional panels open.
- Scrollable fallback when the editor cannot fit all minimum panel sizes; no panel is silently discarded.
- Theme surface colour injection.
- No dependency on processor, APVTS, MIDI, sampler, DSP or audio callback.

## Integration
Add `PPGUMultiWindowWorkspace.cpp/.h` to the plugin target (already listed in CMake after this module lands), add the workspace as a child of the editor, and register real panel components with `openPanel`. Do not register placeholder controls as completed product features.

This module is a reusable UI container; it is not yet wired to FX EDIT, AutoChords, Import Guitar, Settings or About. Host-side visual and accessibility acceptance remains required.
