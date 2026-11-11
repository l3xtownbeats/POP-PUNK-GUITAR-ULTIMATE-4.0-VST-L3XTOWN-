# REV10 unified modules — integration review

## Decision

The supplied PPGU MODULES UNIFIED header is accepted as a functional specification and API sketch, but MUST NOT be copied into the production target unchanged. It contains placeholders and concurrency/lifetime defects that conflict with the REV10 real-time and managed-resource contracts.

## Repository compatibility facts (checked on branch rev9-sb1-bank-scanner)

- The project currently pins JUCE 7.0.12 in CMake, not JUCE 8. The supplied header says JUCE 8. Do not silently change the dependency without a dedicated compatibility build and regression run.
- Existing sampler namespace is `ppgu::sampler`; the proposed header declares `ppgu`.
- Existing production cache API is `SampleStreamingService::tryAcquire(PageKey, ReadLease&)` / `release(ReadLease&)`; there is no `SamplePageCache::tryGet(bankKey)` matching the sketch.
- Existing SamplePool exposes RegionView and StreamingVoiceReader; new layer rendering must use a stable page-lease abstraction rather than an invented cache API.
- Existing SamplePool currently supports streamed WAV-backed SFZ regions; it is not yet a general .pgp bank runtime.

## Blocking issues in the supplied sketch

### LayerEngine
1. `renderVoice()` does not actually read or mix samples. It increments a counter and leaves output untouched.
2. It does not clear/initialize the destination buffer, so a caller cannot rely on deterministic output.
3. It mutates `LayerStack::lastCorrelation` and reads mutable stack data directly; this is not an atomic immutable snapshot as claimed.
4. `maxActiveLayersForBudget()` uses correlation as a proxy for CPU load. Correlation is not a CPU measurement and the role-priority policy is not implemented.
5. The cache lookup result is discarded; no sample position, interpolation, layer envelope, delay, filters, transient handling, or sum-before-PickupBody path exists.
6. It does not prove a single voice accounting model or bounded page lease lifetime.

### ImportManager
1. No worker thread is created or started; `running_` defaults false and `processQueue()` is never invoked.
2. `cancel()` only cancels queued/screening jobs, but the worker state/progress synchronization contract is absent.
3. `lastSuggestion()` returns shared mutable state without synchronization.
4. `NotifyFn` is declared but unused; no safe message-thread notification path exists.
5. The conversion, screening, .pgp writer, atomic index publication and failed-file isolation are comments only.
6. Queue is an unbounded vector with front erasure; no queue capacity, shutdown/join or backpressure contract is present.

### DiscoveryEngine
1. JUCE Timer callbacks execute on the message thread; calling `pollSources()` there would perform network/filesystem work on the UI thread, not a dedicated worker.
2. `addManual()` uses `const_cast` and mutates caller-owned candidate state.
3. `candidates()`, `sources_` and `filters_` are exposed/read without synchronization while timer/worker mutation is implied.
4. `processCandidate()`, `pollSources()` and `loadSources()` are placeholders, not a working discovery engine.
5. No bounded download queue, HTTP timeout/cancellation, checksum validation, retry scheduler, source license revalidation or shared conversion dispatch exists.
6. The timer lifecycle and import pointer ownership are not safe for arbitrary shutdown order.

## Required integration sequence

1. Keep the stable sampler playback path unchanged.
2. Implement SB1 BankScanner on a control worker and validate its manifest against the existing metadata scanner.
3. Implement a common .pgp conversion service with one bounded job queue and cancellation token; ImportManager and DiscoveryEngine must submit to it rather than duplicate conversion.
4. Implement immutable LayerStack snapshots and a page-reader adapter over the existing SampleStreamingService leases. Only then implement actual preallocated audio rendering.
5. Add dedicated tests for each module before wiring it into the processor/editor.
6. Upgrade JUCE 7.0.12 to JUCE 8 only in a separate branch/change with a clean Windows Release + CTest + pluginval comparison.

## Status

This review does not claim that LayerEngine, ImportManager or DiscoveryEngine are implemented. It records integration blockers and the safe order of work. The supplied header is a design baseline, not production-ready code.
