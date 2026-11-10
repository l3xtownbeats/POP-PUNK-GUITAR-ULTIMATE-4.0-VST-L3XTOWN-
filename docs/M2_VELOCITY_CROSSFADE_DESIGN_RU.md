# M2 — SFZ velocity crossfade playback

## Goal
Play overlapping SFZ velocity layers simultaneously with per-region fade-in/fade-out gain. The implementation is isolated in branch `work/m2-velocity-crossfade` / draft PR #5 until its Windows CI passes.

## SFZ semantics
- `xfin_lovel` / `xfin_hivel`: region fade-in; gain is zero at/below the lower bound and full at/above the upper bound.
- `xfout_lovel` / `xfout_hivel`: region fade-out; gain is full at/below the lower bound and zero at/above the upper bound.
- `xf_velcurve=gain`: linear gain interpolation.
- `xf_velcurve=power`: equal-power interpolation; default. Fade-in uses sin(t·π/2), fade-out uses cos(t·π/2).
- A region must still pass its key, velocity (`lovel/hivel`) and MIDI-channel conditions before it is eligible.

SFZ references: https://sfzformat.com/opcodes/xfin_lovel/ and https://sfzformat.com/opcodes/xfout_lovel/.

## Runtime design
- `SamplePool::findRegions` selects up to eight matching regions into a fixed `std::array`; no vector or heap allocation is performed on the audio callback.
- Fade gains are computed once at NoteOn, not per sample.
- Round-robin sets are resolved in one pass over the immutable snapshot with a fixed-size choice array; counters are preallocated in the published snapshot.
- `SamplerEngine::noteOn` starts one voice per selected non-zero layer. NoteOff releases all voices matching the MIDI note/channel.
- If the active polyphony budget cannot accommodate every layer, strongest layers are retained rather than letting later layers steal voices started for the same note.
- Existing streaming page readers and bounded cache continue to supply sample data; crossfade adds voices, not decoded sample buffers.

## Automated regression
- Parser tests cover `xf_velcurve=power`, `gain`, and invalid values.
- SamplePool tests check two overlapping regions at the equal-power midpoint, one active layer at each fade boundary, expected per-layer gains, and two active SamplerEngine voices at the midpoint.
- Latest branch commit: `ef1ee5b04c3dc6653f749d7602b92a683253d059`.
- Windows run `42843931173` is in progress at this documentation update. Do not accept until build, CTest, pluginval strictness 5 and packaging succeed.

## Remaining M2 acceptance
- Confirm expected summed output in a playback/render regression, not only layer selection and voice counts.
- Verify real guitar libraries use overlapping `lovel/hivel` zones and correct xfin/xfout ranges.
- Implement/validate palm-muted articulation mapping from velocity threshold and real articulation sample zones.
- Measure click/pop behavior over repeated attacks, cache pressure, concurrent voices, reload and long playback.
- Prove no audio-thread allocation, blocking lock or file I/O.
- Run FL Studio 20 on Windows 10 x64 with the actual library.

Do not mark M2 complete until runtime, library and host checks pass.
