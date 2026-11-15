# M13 — clean-install and minimum-PC smoke checklist

Target baseline: Windows 10 x64 version 1809 (build 17763) or newer; FL Studio 20 VST3 host.

## Clean install
- [ ] Start from a clean Windows user profile or VM snapshot.
- [ ] Install the VST3 bundle to the standard Common Files\VST3 location.
- [ ] Install/copy only redistributable sample banks; preserve license and attribution manifests.
- [ ] Launch FL Studio 20 and rescan VST3 plugins.
- [ ] Insert the instrument; verify first-open behavior and missing-library messaging.
- [ ] Select an Electric Melody preset and verify MIDI note-on/note-off.
- [ ] Select an Acoustic preset and verify its actual mapped sample profile.
- [ ] Save project, close FL Studio, reopen project and verify plugin state and library restoration.
- [ ] Test preset switching during playback, host automation, transport start/stop/seek, and audio-device change.
- [ ] Run at least 20 minutes of playback with repeated notes and concurrent voices; record audible glitches.

## Performance evidence
Record the exact CPU model, RAM, Windows build, FL Studio version, plugin commit, and sample bank/version.
- [ ] Cold start time (plugin insert to playable state).
- [ ] Preset-switch latency (median and worst case across 20 switches).
- [ ] Peak and steady-state process RAM with no preset loaded and with a large bank.
- [ ] Idle and playback CPU at 1, 8, and 32 voices.
- [ ] Cache misses/page underruns during sustained playback.
- [ ] Confirm no UI freeze or audio glitch while scanning/browsing/loading.
- [ ] Attach FL Studio logs, test result, and a short screen/audio capture where permitted.

Do not mark M13 complete from CI alone. This checklist requires a real Windows 10 x64 / FL Studio 20 test machine.
