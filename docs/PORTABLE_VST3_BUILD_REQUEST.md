# Portable Windows x64 VST3 build request

Date: 2026-11-15

This tracked build request intentionally triggers the Windows VST3 CI workflow for the current reviewed branch.

## Required validation

- Configure and build the VST3 target with Visual Studio 2022 x64 Release.
- Run the CTest regression suite.
- Run pluginval at strictness level 5.
- Package the real `.vst3` bundle with its required portable resources, presets, approved registered skins, sample library, licenses, and checksums.
- Verify the canonical default skin is `skin-053` / `NEON STAGE DEFAULT`.
- Upload `POP_PUNK_GUITAR_ULTIMATE_PORTABLE_VST3_Windows_x64.zip`.

## Release honesty

The current workflow labels its output an interim QA build. A successful CI artifact is not equivalent to final product release approval or FL Studio 20 manual host acceptance. Do not claim either until separately verified.

The workflow must preserve the approved REV.18 contracts and only clean temporary release staging; it must not delete or prune source repository files.
