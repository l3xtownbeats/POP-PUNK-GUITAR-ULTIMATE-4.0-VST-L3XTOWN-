# PPGU REV.17 — Full-tree reconciliation preflight (2026-11-14)

## Scope and evidence

Base branch: POP-PUNK-GUITAR-ULTIMATE-VST-REV.17  
Base / parent HEAD: f135b0f5c9609286086772450f69d287d8ff71c5  
Working branch: work/rev17-full-reconciliation-2026-11-14

The recursive Git tree contains **769 tracked entries**. The inventory includes source, tests, build/workflow files, docs, 102 theme directories, and binary assets. This is not a claim that every binary image, ZIP archive, or WAV payload was decoded or auditioned. Targeted text review covered the processor/editor, sampler/performance code, CMake/workflow, tests, theme/gallery, sound licensing/policy, task registries, README and journals.

## Current implementation map

| Area | Live REV.17 evidence | Honest status |
|---|---|---|
| Windows x64 VST3 | CMake JUCE 7.0.12 / C++20; CI run 43463167899 on SHA 8c75820… | Prior exact-SHA build/test/pluginval/package PASS |
| FL Studio 20 | User confirmed plugin launches and works | User-reported host evidence; exact installed binary SHA not matched |
| Sampler | Source/Sampler production modules and async SFZ loading | Source present; full format compatibility not claimed |
| PATTERN | Host/internal PPQ scheduler, chord transitions, 8 rhythm masks | Runtime path present; section-first UX absent |
| Presets | 300 definitions/catalog | Not proof of 300 fully playable presets |
| Skin Gallery | Image cards, search, family filter, active marker | Partial; click commits immediately |
| Skin Engine | 21 tokens / 28 control IDs in contract; 102 themes | Reconcile runtime schema and current tree before further merge |
| Overlay / secure import | No live production OverlayManager/SecureSkinImport files | Blocked / absent |
| Sound Hunter | Validator and tests; empty registry | Policy tooling present; discovery/curation/approved assets absent |

## Highest-value safe integration order

### P1 — PATTERN Section → Rhythm/Pattern
Add a distinct musical section model (Intro, Verse, Chorus, Bridge, Outro) and a separate rhythm-pattern catalog/selection stage. Persist section-to-pattern associations and preserve old sessions using the legacy global patternSlot as migration fallback. Preview must not silently commit. Keep scheduler sample accuracy, host/internal clock, chord handling, and audio-thread boundedness unchanged. Add tests for association, persistence/migration, MIDI, and transport.

### P1 — Skin Engine security foundation
Do not transplant the uploaded ZIP tree wholesale. Its OverlayManager close behavior has a failing assertion, and its SVG policy has confirmed mixed-case event-handler and protocol-relative href bypasses. Reimplement/adapt against current REV.17 APIs, test security edge cases, then integrate workspace overlays and ZIP quarantine/import.

### P2 — Gallery transaction UX
Add testable draft selection state first, then enlarged Preview, Apply, Back/cancel and Favorites. Preserve the existing theme identity and ensure theme operations never touch APVTS/DSP/MIDI.

### P2 — Sound Hunter
Keep asset-level legal review, provenance, SHA-256, quality screening and guitar-only classification mandatory. Registry is currently empty; no new sample should enter the distributable bank until exact per-asset rights and technical evidence are present. DeepSearch/Curator is not production-integrated.

## Conflicts requiring explicit correction

1. PPGU_TASK_REGISTRY.md says SH-01 sound-registry CTest assertions are unfixed; later Info.txt and CODE records document test corrections and CTest success on SHA 8c75820…. Preserve history and add a dated correction rather than silently editing the old record.
2. SKIN/SKIN_ENGINE_GAP_AUDIT_RU.md describes older REV.14 fixed 1..51 behavior; live REV.17 has a 102-theme registry and a more developed gallery. Treat the old audit as historical, not current runtime truth.
3. Earlier README/INFO entries say FL Studio 20 host validation pending on prior SHAs. The user's 2026-11-14 confirmation establishes successful use, but not the exact binary SHA installed. Keep host evidence distinct from CI evidence.
4. Windows Release/CTest/pluginval strictness 5/package PASS belongs to SHA 8c75820cab1b909ff43c55736f25150338e384d0 / run 43463167899. It does not automatically certify later HEAD f135b0f5c9609286086772450f69d287d8ff71c5.

## Non-negotiable invariants

- SIGNAL FORGE / NEON visual identity remains unchanged.
- No silent APVTS ID/semantic changes.
- No unbounded work, file I/O, locks, allocations, waits, networking or decoding in audio callback.
- Preserve sampler snapshot lifetime, cache/voice limits, MIDI scheduler and host timing.
- No unsupported sample licensing claims; no ZIP-wide rollback.
- Update INFO and CODE in the same transaction as every accepted change; record exact SHA and exact checks.

## This checkpoint

This branch records reconciliation only. No production source, CMake target, binary asset or runtime behavior was modified. No build, CTest, pluginval, package or host test was run on this checkpoint. Next source change should be a separately scoped PATTERN section model with regression coverage, after the complete APVTS/state/preset migration map is finalized.
