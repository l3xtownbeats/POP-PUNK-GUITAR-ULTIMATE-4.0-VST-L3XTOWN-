# POP PUNK GUITAR ULTIMATE — PERFORMANCE SPECIFICATION (RU)

**Target:** FL Studio 20 / Windows 10, VST3 x64, JUCE 7.0.12, C++20  
**Status:** DESIGNED / PERFORMANCE SPEC. This is a mandatory specification, not a description of completed implementation.  
**Reference baseline:** INFO dated 2026-11-12; CODE.txt governance; historical reference branch `work/signal-forge-theme-tokens` at user-supplied short SHA `57fe400`. The branch currently resolves to a different HEAD; no equivalence with 57fe400 is assumed.  
**Important:** CPU/RAM targets are not yet confirmed by measurements.

## 1. Measurable performance budgets

| Metric | Target | Verification |
|---|---:|---|
| CPU, one instance (44.1/48 kHz, 128–512 samples, 4 voices) | <= 2% on 4-core desktop CPU | FL Studio PerfMeter / profiler |
| CPU stress, 64 voices | <= 15%, no queue overflow | R4 stress test |
| Instance RAM, bank not loaded | <= 60 MB | Working set after startup |
| RAM, full SoundBank with page cache | <= 512 MB | Active memory + cache cap |
| Preset load | <= 300 ms, scaled by SFZ length | Background timing log |
| First sound after load | <= 50 ms | Latency test |
| Allocations in processBlock | 0 — HARD GATE | Audit build with malloc override / JUCE leak detector |
| Peak audio callback | <= 60% of buffer at 128 samples | Test timing |

Each budget must be represented by a CTest counter or profiling report before a status can be assigned. Until then these are targets, not achieved results.

## 2. Audio core

### 2.1 Memory discipline
- 64 voice slots are specified as preallocated. Voices must not be created/destroyed in realtime; recycle through a freelist/lock-free stack.
- Page cache has a fixed budget (initial proposal: 256 MB) and LRU eviction. Eviction is background-worker-only, never on the audio thread.
- Snapshots and streaming services use pools and generation tokens to prevent heap churn.

These are requirements to audit against the checked-out source; this document does not certify their current implementation.

### 2.2 Streaming path
- Cache misses must not block the audio thread. A voice retains its cursor until the page arrives.
- Expose queue-overflow, cache-full and miss-rate counters for diagnostics/UI.
- Preserve the existing SPSC queue and its 500k-transfer test; profile it, do not rewrite blindly.
- Preserve Hermite page interpolation. SIMD (SSE2 x64 baseline, AVX2 runtime detection) is allowed only for profiler-identified interpolation/gain-staging hot spots.

### 2.3 Audio callback prohibitions
Static checks and review must reject:
- new/delete, mutexes, allocating STL container operations, logging and system calls;
- exceptions crossing the callback boundary (worker recovery belongs to the worker);
- sleep/yield in the callback.

## 3. Windows 10 / FL Studio 20 streaming tuning
- Start experiments with 256–512 KB pages and 8–32 queued pages per root. Select final values from latency-versus-miss-rate measurements, not guesswork.
- Keep the known limit of 8 streaming roots. Enforce fixed per-root memory/thread quotas and a global aggregate cap.
- Use one shared worker pool across roots, not one OS thread per root.
- Load metadata index first; load heavy pages on demand; prioritize note-start preview pages.

## 4. UI performance
- Repaint only dirty regions. Global repaint is reserved for skin changes (150 ms fade).
- CPU/status meters update at 10–15 Hz, not 60 Hz. LED animation uses a fixed timer and suspends while the editor is hidden.
- ECO skin mode removes shadow/glow/blur effects. The stated potential 30–40% UI-load saving is a hypothesis until measured.
- Theme palette is published as an atomic snapshot so paint does not lock; apply ThemeManager hardening only after stable CI.
- Cache text/font glyph work (GlyphCache) instead of recalculating it on every paint.

## 5. Release build flags
- MSVC Release: /O2, /permissive-, /arch:SSE2 baseline, LTO (/GL /LTCG).
- /fp:fast is restricted to DSP translation units where numerically acceptable; never apply globally without review.
- Use a MinSizeRel-like approach for non-DSP code.
- Debug-only assertions/diagnostics must not burden production Release.
- Link only required JUCE modules. Candidate set: audio_basics, dsp, gui_basics, audio_processors, events, graphics, core and data_structures. Verify actual transitive dependencies before removing modules.

## 6. FL Studio 20 host constraints
- Avoid expensive logic in stateChanged/notify cycles; UI refreshes should be timer-driven, not pushed from audio.
- Declare latency honestly through setLatencySamples. Pattern Scheduler must not introduce undeclared latency.
- SFZ loading must not block the GUI thread; report progress through an asynchronous ticket.

## 7. CI acceptance gates
1. Add `ppgu_perf_bench` CTest target: processBlock benchmarks on synthetic and real SFZ fixtures, allocation counters, and preset-load timing. Run on every SHA.
2. Add a performance regression gate: >10% degradation from the last accepted benchmark fails CI and requires a reasoned review.
3. Host verification: user runs FL Studio 20 PerfMeter for startup, MELODY at 4 voices, 64-voice stress, SKIN menu open, and theme hot-swap. Store results in INFO and mark HOST VERIFIED only for the exact binary SHA.

## 8. Implementation order
- **P1:** Resolve current CI failures (including terminal results for runs #971/#972 and the currently observed #984 failure); then apply ThemeManager hardening. No perf baseline before stable CI.
- **P2:** Add performance benchmark and allocation-audit build; capture and record baseline before optimization.
- **P3:** Tune page size, shared streaming worker pool, profiler-driven SIMD, UI timers and ECO mode from data.
- **P4:** R4 stress test and FL Studio 20 host check; only then mark CPU/RAM HOST VERIFIED.
- **P5:** Expand features/panels/Skin Engine only after the performance budgets have a measured baseline.

## 9. Journal and status governance
- Never promote DESIGNED to IMPLEMENTED.
- Unmeasured metrics remain targets, not achieved.
- Every result records exact SHA and configuration: CPU, sample rate, buffer size, voice count and FL Studio version.
- Risks R1–R6 remain in the risk register; update status, do not delete them.
- One production-code reason per commit; full CI for each SHA.

## Ready-to-append INFO entry

### 2026-11-12 — Performance specification: FL Studio 20 / Windows 10 budgets

**Status:** PERFORMANCE SPEC DESIGNED; implementation PENDING (P1–P5).  
Input: minimize CPU/RAM while preserving speed and audio quality on FL Studio 20 / Windows 10. No production source changes are implied by this specification.  
Budgets: CPU <= 2% (4 voices), <= 15% (64-voice stress); RAM <= 60 MB idle / <= 512 MB full SoundBank; preset load <= 300 ms; first sound <= 50 ms; 0 allocations in processBlock (hard gate); peak callback <= 60% of buffer at 128 samples.  
Gates: ppgu_perf_bench; 10% performance regression gate; HOST VERIFIED only from FL Studio 20 measurements tied to a specific binary SHA.  
Order: P1 CI + ThemeManager hardening -> P2 baseline -> P3 data-driven optimization -> P4 stress + host check -> P5 new features.  
**Self-check:** documentation stage; CI PASS not assigned; no performance measurements exist yet. Targets are specified, not confirmed.
