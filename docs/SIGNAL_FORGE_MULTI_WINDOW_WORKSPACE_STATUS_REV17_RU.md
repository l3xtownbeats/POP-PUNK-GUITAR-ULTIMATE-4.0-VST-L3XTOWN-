# SIGNAL FORGE — Multi-Window Workspace implementation status (REV 17)

Date: 2026-11-12
Branch: POP-PUNK-GUITAR-ULTIMATE-VST-REV-17
Status: workspace container source added; editor integration and real panel content remain in progress.

## Approved UX decisions recovered from the conversation

1. SIGNAL FORGE is the approved visual direction for the PPGU shell and its auxiliary panels.
2. The approved main-screen reference remains the source of truth; do not replace it with a generic JUCE layout.
3. Every auxiliary panel opens independently inside the plugin editor and has its own square close control with an ×.
4. Multiple open panels are automatically tiled and compacted so the user can see all panels at once whenever minimum dimensions allow.
5. Closing a panel immediately reflows the remaining panels.
6. When the available editor area is too small, panels remain available through internal scrolling; do not silently discard or hide panel instances.
7. Panels are movable by dragging their header; drag-to-panel swaps their tile positions.
8. Theme changes must restyle panel surfaces, borders, headings and controls while preserving the same control IDs, APVTS attachments and actions.
9. The workspace must not change processor, APVTS, MIDI, sampler, DSP, scheduler or audio-thread state merely because panels are opened, closed, resized or reordered.

## Source added in REV 17

- `Source/UI/PPGUMultiWindowWorkspace.h`
- `Source/UI/PPGUMultiWindowWorkspace.cpp`
- `Source/UI/PPGUMultiWindowWorkspace_README_RU.md`

The plugin target now lists the new source files in `CMakeLists.txt`.

The workspace provides:
- stable positive panel IDs and duplicate-open protection;
- independent child component ownership;
- per-panel close button;
- automatic grid layout using `AuxiliaryPanelLayout`;
- automatic compaction and reflow;
- drag-to-swap panel ordering;
- scrollable fallback when all minimum panel sizes cannot fit;
- theme colour propagation to open frames.

## Important boundary — not yet complete

The workspace is currently a reusable JUCE UI container. It has NOT yet been mounted into `PopPunkGuitarUltimateAudioProcessorEditor`, and no real FX EDIT / AutoChords / Import Guitar / Settings / About panels have been registered in it. Therefore this is not yet an end-user-visible completed feature.

The approved main editor currently contains real preset controls, SFZ import button, skin selector, MELODY/PATTERN controls, nine knob attachments, seven FX bypass attachments, sync selector and MIDI keyboard. Several requested auxiliary features remain absent or partial according to `SKIN/CONTROL_INVENTORY_MATRIX_RU.md`. Do not create active-looking dummy panels to conceal those gaps.

## Next integration sequence

1. Mount the workspace as an overlay/work area inside the main editor without disturbing the approved main-shell layout.
2. Add real panel launch affordances and panel registry entries only when the target panel has real functionality.
3. Implement FX EDIT first using existing APVTS FX detail parameters and host-automatable attachments.
4. Implement AutoChords against the real chord/scale state and MIDI/performance engine; keep all changes honest about any missing playback behavior.
5. Implement Import Guitar as a full workflow around the existing asynchronous SFZ loader.
6. Implement Settings and About from actual runtime/product data.
7. Verify keyboard focus, close behavior, drag reorder, scroll fallback, DPI scaling, theme restyling and control parity.
8. Run Windows x64 Release build, CTest and pluginval strictness 5; then separately test in FL Studio 20.

## Acceptance

Do not mark Multi-Window Workspace COMPLETE until it is integrated into the editor, contains real panel components, all required interactions work, the full test/build pipeline passes, and host-side visual/functional checks are recorded.
