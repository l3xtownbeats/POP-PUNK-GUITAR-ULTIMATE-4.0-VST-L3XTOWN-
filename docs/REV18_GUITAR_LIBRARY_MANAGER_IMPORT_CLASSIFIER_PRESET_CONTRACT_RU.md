# REV.18 — GUITAR LIBRARY MANAGER, IMPORT CLASSIFIER & PRESET-ASSET CONTRACT
**Document ID:** PPGU-REV18-GLM-001  
**Date:** 2026-11-17  
**Status:** USER-APPROVED REQUIREMENTS / DESIGN BASELINE — implementation not yet claimed  
**Approved working line:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV.18`  
**Implementation work branch:** `work/rev18-guitar-library-manager`  
**Product:** POP PUNK GUITAR ULTIMATE, Windows 10 x64, VST3, JUCE/C++20, FL Studio 20.

## 0. Binding approval and purpose
The user explicitly approved continuing on REV.18 and confirmed this extension. This document records the approved direction for a unified guitar-library intake/classification workflow. The feature is controlled from the plugin's Import Guitar workflow, but expensive file analysis must run on bounded background workers and never in the audio callback.

The goal is not merely to sort filenames. The system must preserve useful material, identify its musical/instrument role as accurately as evidence allows, map compatible assets into the existing SoundBank/S6 and sampler architecture, and make those assets straightforward to select from factory/user preset definitions.

**No second parallel import/search pipeline is allowed.** Reconcile with and extend the existing Import Guitar / SoundBank S6 / S6.5 DiscoveryEngine / asset registry architecture after source audit. Preserve current SamplePool, SFZ, streaming, preset and realtime contracts.

## 1. User workflow and folder contract
Plugin entry point: **IMPORT GUITAR**. The managed intake folder is named exactly:
`IMPORT SAMPLES GUITAR`

Workflow:
1. User places complete source libraries (folders, archives, WAV/SFZ and supported formats) into the intake folder or selects them through Browse/drag-drop.
2. Plugin shows source inventory, detected library boundaries, estimated work, disk space and license/provenance status.
3. User starts SCAN / ANALYZE / SORT from the plugin. UI remains responsive; progress, pause, cancel and resume/checkpoint are available.
4. The manager analyses files and their relationships as a library, not as unrelated isolated filenames.
5. Eligible assets are staged, validated, indexed and transactionally published into the existing approved BASIC LIBRARY/SoundBank layout.
6. After copy/hash/index/reference verification succeeds, successfully committed source items are removed from the *working intake copy* only. Original source libraries/archives must remain preserved in a separate immutable source/backup location; never destroy the only copy.
7. Unsupported, ambiguous, unclassified, incomplete, duplicate-conflict, or otherwise useful-but-not-yet-approved material remains available in the intake with a reason/status. Technically corrupt items are isolated in QUARANTINE, never silently deleted.
8. A report records every action and reason.

The intake folder is not a permanent second SoundBank. On a successful transaction it should contain only unresolved/review-needed items and quarantined items, plus its control/report files. Do not empty it by force and do not classify unknown as defective.

### Required UI actions/states
- Browse / drag-drop target
- Scan Library
- Analyze
- Sort / Commit
- Preview / audition (must not mutate active preset or playback state)
- Pause, Cancel, Resume
- View Report / Open unresolved items
- Retry failed item / Recheck
- Explicit confirmation before destructive cleanup of working copies
- Status counts: discovered, supported, classified, ready, committed, unresolved, quarantine, duplicate, license-blocked, failed

## 2. Classification model — broad, extensible, non-destructive
Each asset may have multiple orthogonal tags. Do not force a single exclusive label where a sound can have multiple roles. Maintain raw source metadata and confidence/evidence for inferred values.

### 2.1 Instrument and tone family
- Electric guitar: clean, edge-of-breakup, crunch, overdrive, distortion, high-gain, lead, rhythm, muted/chug, ambient/swell, DI, amp/mic/cab/IR only where source proves it.
- Acoustic guitar: steel-string, nylon, classical, acoustic-electric, fingerstyle, strummed, picked, percussive body, muted.
- Guitar role: rhythm, lead, melody, octave line, double-stop, chordal, texture/ambient, transition/fill.
- Bass, drums, vocals and other instruments may be detected as non-guitar content for reporting/quarantine routing, but must not be silently inserted into the Guitar SoundBank. Bass assets require the separately approved product/edition boundary.
- Unknown instrument => GUITAR_REVIEW / UNCATEGORIZED; never discard solely due to unknown classification.

### 2.2 Musical note and pitch metadata
Support all 12 pitch classes and enharmonic spellings (C/C#, Db, D, D#/Eb, E, F, F#/Gb, G, G#/Ab, A, A#/Bb, B), octave conventions with explicit MIDI note number as canonical value, note ranges, root note, tuning, transposition, pitch bend and cents offset when detectable.
Record:
- root MIDI note and pitch class;
- playable key range / note range;
- tuning/reference pitch if known;
- pitch confidence and analysis method;
- detected melody notes/contour for loops and phrases where reliable.
Never infer missing notes as absent content. Unknown/ambiguous key is a valid UNKNOWN value, not a rejection.

### 2.3 Key, scale and tonal center
All major and minor keys; relative major/minor; modal/borrowed material and chromatic content. Scale labels may include major/ionian, natural minor/aeolian, harmonic minor, melodic minor, dorian, phrygian, lydian, mixolydian, locrian, pentatonic major/minor, blues, chromatic and unknown/custom. Store estimated tonic, mode, confidence and method. Do not reject atonal, modal, chromatic or ambiguous material.

### 2.4 Chord recognition and chord catalog
Recognize and represent, when acoustically supported:
- triads: major, minor, diminished, augmented, sus2, sus4, power/fifth, no-third;
- sevenths: maj7, min7, dominant 7, min7b5/half-diminished, diminished 7, min-major7, 7sus4, 7b5, 7#5;
- extended/added: add2/add9, add4, 6, min6, 6/9, 9, maj9, min9, 11, 13 and altered dominant variants;
- suspended, quartal, pedal/bass-note and slash chords;
- inversions, voicings, drop voicings, open/closed positions, octave doubles, omitted/doubled tones;
- guitar-specific voicings, movable shapes, open chords, barre chords, partial chords and power-chord shapes.
Support all roots in all 12 pitch classes and enharmonic names. Store detected pitch classes/notes as evidence alongside symbolic chord label. A symbolic label is not asserted if confidence is low; keep the pitch set and mark chord UNKNOWN/REVIEW. A missing chord in a source collection is a coverage gap, not a defective asset.

### 2.5 Articulations and performance techniques
Tag only when source metadata, SFZ opcodes, naming evidence or reliable audio analysis supports it:
- palm mute / muted / chug / staccato / dead note / mute scrape;
- downstroke, upstroke, alternate picking, economy picking, tremolo picking;
- legato, hammer-on, pull-off, slide up/down, bend, release, vibrato;
- natural/artificial/pinch harmonics, tapping, trill, rake, pick scrape;
- open strings, let ring, choke, stop, fret noise, string noise;
- fingerstyle, pick, hybrid picking, strum, finger-pluck;
- acoustic strum/fingerpicked, brush/percussive hits;
- round-robin, keyswitch, sustain/release, velocity transition, alternate take.
Do not confuse tremolo picking with a tremolo effect. Do not fabricate palm-mute or other articulations when the sample does not contain them. Low velocity may select a real muted layer only where the mapping/source supports that behavior.

### 2.6 Sound object / asset role
Classify independently as:
- single note / multisample zone / velocity layer / round-robin take;
- single chord / chord set / chord progression;
- one-shot / hit / transient / accent / FX;
- guitar loop / rhythm loop / strum loop / riff loop / lead loop;
- sample melody / melody phrase / hook / octave melody / lead phrase;
- sample pattern / performance pattern / MIDI pattern / audio loop pattern;
- arpeggio / picking pattern / strumming pattern;
- fill / ending / transition / intro / verse / pre-chorus / chorus / bridge / outro / stop-time;
- ambience / sustain / swell / drone / texture;
- impulse response/cabinet capture only when supported by actual format/content.
Do not treat a WAV loop as a MIDI pattern. Preserve the source representation and declare playback requirements separately.

### 2.7 Loop Guitar and Sample Melody Patterns (explicit scope)
Dedicated semantic families:
- LOOP GUITAR: electric/acoustic/nylon/steel; clean/crunch/high-gain; rhythm/lead; palm-muted/open; chordal/single-note; straight/swing/shuffle where supported; tempo, meter, bars, start/end, loop points, pickup/anacrusis, tail and sync quality.
- SAMPLE MELODY: single-note phrases, lead hooks, octave lines, harmonized leads, motifs, fills, transitions, endings; key/scale/root/range/contour and tempo metadata where reliable.
- SAMPLE MELODY PATTERNS: repeating note/phrase patterns, arpeggios, picking sequences, rhythmic motifs, MIDI patterns, audio phrase loops; distinguish audio from MIDI and distinguish phrase library from the plugin's generated PATTERN scheduler.
- SAMPLE RHYTHM PATTERNS: strums, down/upstroke sequences, eighth-note patterns, gallops, half-time/double-time, ska/offbeat, stop-start, chugs and palm-muted verse/open chorus structures where actual recordings or MIDI support them.
- CHORD LOOPS / CHORD PROGRESSIONS: progression sequence, per-chord duration, key/tonality estimate, voicing and tempo/meter where supported.
These families must be selectable as preset ingredients only when the engine has a compatible playback path. Unsupported playback representation remains indexed and reviewable, not silently converted or advertised as playable.

### 2.8 Genre and style profiles
Support multi-label style metadata and a confidence score; style is a descriptive/curation hint, not proof of origin:
- Pop Punk / Skate Punk / Punk Rock / melodic punk;
- Alternative Rock / Emo / Post-Hardcore-adjacent melodic guitar;
- Modern Pop Rock / radio rock / power pop;
- Indie/College Rock / Garage Rock;
- Acoustic Rock / acoustic pop punk;
- melodic lead/ambient guitar textures;
- generic rock, metal/high-gain and other useful guitar-adjacent material as separate labels, not forced into pop-punk.
Existing internal profiles PP-ROOT, PP-NEO2000, PP-MODERN, PP-EMO remain authoritative where already defined. Do not create artist/track imitation claims or imply endorsement. Preserve UNCATEGORIZED for material without sufficient evidence.

### 2.9 Technical metadata
Container/codec, sample rate, bit depth, channels, duration, loudness/peak/RMS, clipping, DC offset, noise estimate/SNR where measurable, silence/trailing silence, transient/attack, loop continuity/crossfade, tempo/BPM confidence, meter, beat grid, bar length, sample root/key, stereo correlation/phase, file integrity and SHA-256. Measurements must include method/version and must not be presented as musical/legal truth.

## 3. SFZ and multisample mapping
- Parse SFZ opcodes and hierarchy with include/path resolution, groups, regions, keys/key ranges, pitch_keycenter, lokey/hikey, lovel/hivel, xfin/xout/crossfade, sw_last/sw_down/sw_up, sw_default, off_by, sequence/round-robin, trigger, ampeg release, loop_mode and relevant supported opcodes.
- Preserve original SFZ and source-relative paths; resolve WAV references without flattening or breaking include structure.
- Validate missing sample references, duplicate region collisions, unreachable zones, note/velocity gaps, overlaps, switch collisions and unsupported opcodes.
- Build a coverage matrix per instrument/articulation: MIDI note × velocity × keyswitch × round-robin.
- Record partial coverage as PARTIAL, not as file corruption. Never invent missing note/velocity layers.
- Use the existing SFZ runtime's supported opcode list as the actual compatibility authority; unsupported opcodes are reported and preserved, not silently discarded.

## 4. Classification confidence and anti-false-rejection policy
Every inferred field carries:
- value / UNKNOWN / MULTI;
- confidence 0..1;
- evidence source (SFZ, filename, folder, embedded metadata, audio analysis, curator);
- classifier/analyzer version;
- review state.

Thresholds are per field and calibrated with test fixtures. Do not use one global confidence threshold for all categories.

Statuses:
- READY: technical validation passed, guitar content confirmed, rights gate cleared, and compatible playback mapping exists.
- PARTIAL: useful content but incomplete notes/velocity/articulation/metadata coverage.
- REVIEW: uncertain instrument, note, chord, style, BPM or articulation.
- UNSUPPORTED_PLAYBACK: valid content but current engine cannot play this representation.
- LICENSE_BLOCKED: rights/provenance missing or incompatible.
- DUPLICATE: identical content hash; retain provenance and reference canonical asset.
- QUARANTINE: demonstrably corrupt, unsafe or unreadable file; reason and hash retained.
- REJECTED: only explicit curator decision with reason, never automatic based only on unknown key/chord/style.
- COPYRIGHT_SUSPECT: hold for human review; no SoundBank commit.

**Critical rule:** no musical incompleteness or unknown label is sufficient to call an asset defective. Unknown, uncommon, atonal, chromatic, partial, rare and out-of-profile guitar material is retained in REVIEW/UNCATEGORIZED unless a separate objective gate fails.

## 5. Rights, provenance and quality gates
Use the existing asset-level GREEN/YELLOW/RED policy:
- GREEN auto-add only with confirmed CC0/PD or other specifically approved license evidence permitting commercial use, modification and redistribution inside the plugin, plus provenance and SHA-256.
- YELLOW requires manual import/review and cannot auto-add.
- RED is excluded from distributable SoundBank and cannot be overridden by ordinary curator action.
- Missing/unclear rights => LICENSE_BLOCKED, not QUARANTINE and not deletion.
- COPYRIGHT SUSPECT blocks commit until human review.
- Technical screening does not establish musical quality or legal clearance. GOLD/playable factory use requires human audition and curator approval under existing policy.

## 6. Transactional sort / move contract
- Analyze in place or from a safe working copy; do not alter source archives.
- Stage candidate outputs in a transaction-specific staging directory.
- Validate file integrity, SHA-256, SFZ references, index schema and destination paths before publication.
- Commit atomically or roll back completely. Never partially publish a broken SFZ/WAV group.
- After successful publication, verify destination hash and index references; only then remove the corresponding files from the *working intake copy*.
- Preserve immutable original libraries outside the intake. No destructive action on the only copy.
- Duplicate content is not recopied; provenance from every source remains recorded.
- Unknown/review/unsupported/license-blocked items remain in intake with machine-readable reason.
- Corrupt files are moved to QUARANTINE only after recording source path, hash if readable, error and recovery information.
- A failure/cancel must leave recoverable state and permit resume without double-copying.

## 7. Preset-ready asset index contract
Extend the existing versioned SoundBank/S6 index; do not create a second incompatible database. Each asset record should expose, where applicable:
- stable asset_id, content SHA-256, source library/source path and provenance;
- license state/evidence reference and quality/curator state;
- instrument/tone family, style profiles, role, asset type;
- root note, MIDI range, tuning, key/scale, chord symbol + detected pitch set;
- articulation, velocity range/layer, keyswitch, round-robin group;
- BPM, meter, bars, loop points, sync/stretch capability;
- channel/sample format/length and technical analysis;
- playback representation and compatible engine route;
- classification confidence/evidence and unresolved fields.
Preserve backwards compatibility through schema versioning and explicit migrations. Presets reference stable asset IDs, not fragile absolute paths. Preset validation must detect missing IDs, unavailable playback route, license-blocked assets and incomplete dependencies.

### Preset ingredient model
A preset may compose supported layers such as:
- primary instrument/multisample;
- rhythm L/R or double-track layers;
- chord/strum layer;
- lead/melody phrase layer;
- loop/pattern layer (audio or MIDI, distinct type);
- articulation/velocity/round-robin mappings;
- optional ambience/texture.
Only supported and rights-cleared ingredients can be committed to factory presets. The system must not automatically stack every tagged asset; preset recipe selection remains intentional and audibly validated.

## 8. Runtime and resource constraints
- No disk I/O, file parsing, decoding, hashing, network requests, blocking locks, unbounded loops or heap allocation in audio callback.
- All scan, classification, waveform analysis, hashing, conversion and indexing run in bounded worker/service context.
- Audio thread receives immutable prepared mappings/snapshots through the existing safe publication mechanism.
- Keep worker concurrency bounded, cancellation/lifetime safe, disk quota aware and memory bounded.
- UI shows progress without blocking. Preview uses isolated audition path and must not alter current preset/project state.
- No new sampler engine or duplicate soundbank pipeline. Reuse existing SamplePool/SFZ/streaming and S6.5 path.

## 9. Required tests and acceptance gates
1. All 12 pitch classes and enharmonic names, MIDI octave edge cases.
2. Major/minor/modal/chromatic/unknown key cases; confidence and unknown preservation.
3. Chord families, inversions, slash chords, power chords, partial voicings and ambiguous polyphonic mixes.
4. Articulation labels from SFZ vs filename vs audio evidence; no false palm-mute inference.
5. LOOP GUITAR, SAMPLE MELODY, SAMPLE MELODY PATTERNS, audio loops vs MIDI patterns, tempo/meter/loop points.
6. Acoustic/steel/nylon/electric and non-guitar negative fixtures.
7. SFZ includes, zones, velocity, switches, RR, unsupported opcodes, missing WAV and partial coverage.
8. License GREEN/YELLOW/RED, unknown license, copyright suspect, provenance/hash mismatch.
9. Duplicate same-content/different-name and same-name/different-content.
10. Transaction rollback, interrupted copy, disk full, cancel/resume, source preservation and no intake cleanup before verified commit.
11. Index schema migration/backward compatibility and preset references.
12. Preview isolation, UI responsiveness, bounded workers, audio callback realtime safety.
13. Exact-SHA Windows x64 Release, all CTest, pluginval strictness 5, package validation; FL Studio 20 host acceptance separately.

No task may be called IMPLEMENTED/TESTED until source code exists and relevant checks actually pass on the stated exact SHA. Human listening and rights approval remain separate acceptance gates.

## 10. Implementation sequence
A. Preflight/audit existing REV.18 Import Guitar, S6/S6.5, SoundBank index, SFZ parser/runtime, SamplePool, preset schema, tests, CMake and CI.  
B. Reconcile this contract with current schemas and existing implementation; identify extension points and avoid duplicate pipelines.  
C. Fix existing sound asset registry CTest regression before building on that gate.  
D. Implement versioned classifier metadata and fixture-driven tests; begin with deterministic metadata/SFZ parsing, then optional audio feature analysis.  
E. Implement transactional intake staging/commit and reports.  
F. Connect the existing plugin Import Guitar UI to bounded background service and progress/cancel/resume.  
G. Extend index and preset dependency resolution; ensure asset IDs are easily selectable in preset authoring/curation.  
H. Add LOOP GUITAR / SAMPLE MELODY / SAMPLE MELODY PATTERNS / rhythm/chord progression families with playback capability flags.  
I. Run exact-SHA CI, pluginval and host acceptance.  
J. Only after approved legal assets and playable mappings are ready, use them for the already-required factory preset program. Existing requirement to finish BASIC LIBRARY and 500 core factory presets before plugin sound-quality/final-build testing remains in force.

## 11. Non-goals and invariants
- Do not manually pre-sort every library into a hand-built folder taxonomy before import.
- Do not add a new parallel Discovery/Import/Sampler pipeline.
- Do not modify existing APVTS IDs/semantics, MIDI timing, DSP routing, SamplePool lifetime or preset state format without explicit migration and approval.
- Do not treat style guesses as facts or use artist/track names as sound-source provenance.
- Do not auto-reject unknown/partial musical content.
- Do not delete original libraries or the only source copy.
- Do not claim the plugin already implements this workflow: this document is the approved design baseline, not proof of implementation.

**Initial status:** APPROVED / DESIGNED. Source audit and implementation are next; no production code, tests, build or host verification are claimed by this document.
