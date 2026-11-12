# POPPUNK ARCHITECT ∞ v3.0 — PPGU TONE DESIGN & RESOLUTION STANDARD

**Project:** POP PUNK GUITAR ULTIMATE / ANGELA ENGINE VER.1  
**Status:** DESIGN STANDARD ACCEPTED — implementation and listening validation pending  
**Source:** user-provided POPPUNK ARCHITECT ∞ v3.0  
**Scope:** artist/song-inspired tone design, preset authoring, fallback resolution, runtime routing and quality assurance.

## 1. Mission and truthful target

Build the closest musically useful, mix-ready approximation of a requested pop-punk guitar character using the capabilities actually available in PPGU. A specific external amplifier, plugin or IR must never be a prerequisite for attempting a tone. If a requested model is unavailable, the system must resolve an architectural analogue or construct a custom chain from available modules.

“Sound first” does **not** authorize false claims. The UI and preset metadata must distinguish:
- exact model / licensed capture actually available;
- architectural analogue;
- custom DSP approximation;
- unavailable or degraded route;
- listening-approved / mix-ready status.

Never promise literal “one-to-one” reproduction unless independently demonstrated; artist/song settings are reference starting points, not official studio knob records unless verified. Use “Artist-inspired” and “Song-tone reference”, not endorsement claims.

## 2. Mandatory fallback hierarchy

The resolver tries these levels in order and records the selected route:

1. **Direct model:** use the requested amp/cab/effect model when genuinely implemented and available.
2. **Architectural analogue:** choose the closest available circuit/voicing by gain structure, clipping behavior, EQ response, dynamics and intended role.
3. **Custom chain:** combine implemented drive/overdrive, EQ, compressor, saturation/waveshaper, filters, pitch, delay, modulation and cabinet processing to approximate the target. Do not list or instantiate a module that does not exist in runtime.
4. **Independent layers:** create separate L/R signal chains or genuinely independent performances with distinct amp/cab/EQ choices. A duplicated mono signal is not double tracking. A short-delay pseudo-double must be labelled as an effect and is not an equivalent substitute.
5. **Custom DSP development:** when no existing route is adequate, design a new bounded, realtime-safe DSP module and validate it before exposing it as available.

For every fallback store: requested target, chosen replacement, fallback level, reason, exact parameter values, available/missing dependencies, approximation caveats and QA status. No silent RAW/DI fallback.

### 2.1 Example architectural mappings (starting points, not fixed truths)

| Target | Preferred analogue direction | Custom shaping direction |
|---|---|---|
| Mesa Rectifier | two-channel high-gain architecture / 5150-6505-like tight high gain | controlled low end, firm clipping, focused upper mids |
| Marshall Plexi/JCM800 | British plexi/crunch architecture | mid-forward crunch, moderate gain, dynamic pick response |
| Vox AC30 | bright clean/top-boost architecture | chime, upper-mid presence, controlled low end |
| Fender Twin | clean studio/vintage architecture | clean headroom, broad low end, smooth top |
| Orange | dense high-gain architecture | strong ~800 Hz body, controlled top, tight lows |

Do not describe an analogue as the named original model. The mapping is an engineering approximation.

## 3. Tone-design principles

- Use the minimum gain that delivers the intended sustain and aggression; preserve power-chord separation.
- Keep the midrange meaningful; do not scoop mids by default.
- Pick attack, downstrokes, palm-mute consistency and independent performances are essential parts of the sound.
- Guitar/source/pickup characteristics affect the result; use source-aware gain, EQ and articulation behavior.
- Double tracking means two separately performed takes. If no independent performance is available, do not claim double tracking.
- Mic perspective is a virtual cabinet/mic choice only when the IR/capture exists. SM57/MD421 or ribbon names must not imply real captures without provenance.
- Gate, boost, compression, delay, modulation and reverb are role-dependent, not mandatory on every preset.
- Reverb is normally a send/parallel space treatment for tight rhythm parts; do not smear the insert signal.
- Keep output and loudness matched when comparing tones.

## 4. Reference signal chain

Typical driven rhythm starting chain:

**Source → Gate (if needed) → optional OD boost → Amp → Cabinet/IR + mic perspective → corrective EQ → light compression if needed → send effects → output trim**

Starting ranges, all subject to source, amp model and listening:
- Gate threshold: begin around -45 to -35 dB only if noise requires it; set release to avoid chopping sustains.
- OD boost: Drive 0–3/10, Level 8–10/10, Tone 5–7/10; use to tighten/drive the amp, not simply make output louder.
- Amp controls: use artist/song reference profile values as a starting point; do not transpose knob values blindly between different amp models.
- Cabinet: use a real, licensed/redistributable IR or clearly named original cabinet voicing. Keep cabinet processing distinct from insert FX bypass.
- Corrective EQ starting points: HPF 70–90 Hz; reduce 300–500 Hz by 1–3 dB only if muddy; optional +1–2 dB around 750 Hz for body; reduce 3–5 kHz by 1–2 dB only if harsh; optional gentle air shelf 8–10 kHz.
- Compression: ratio 2–4:1, attack 10–30 ms, release 100–200 ms, usually 2–4 dB gain reduction at peaks; preserve pick transient.
- Reverb: send/aux, plate-like 1–1.5 s as a starting point, low wet level; adapt to tempo/arrangement.
- Pseudo-double: 10–20 ms short delay only when necessary, explicitly labelled and checked for comb filtering/mono compatibility.

## 5. Artist and song reference profiles

These values are **reference starting points** from the supplied guide. They are not verified official studio settings. All controls are 0–10 and must be adapted to the chosen model.

### Blink-182 / Tom DeLonge
- Reference gear direction: early Strat-style source; later ES-333 / bridge humbucker; Mesa Triple Rectifier and Marshall JCM900 directions; DS-1 as an early drive reference.
- Important guide-specific idea: experiment with a highly driven clean/crunch JCM900-style channel rather than assuming the high-gain channel is always the answer.
- Dammit starting point: clean/crunch architecture, Gain 10, Bass 6, Mid 6, Treble 7, Presence 6.
- First Date: Rectifier-family analogue, Gain 6.5, Bass 5, Mid 6, Treble 6.5, Presence 6.
- Feeling This: Gain 7, Bass 5, Mid 5.5, Treble 7, Presence 6.
- Mostly tight rhythm treatment; use source-aware low-end control and clear palm mutes.

### Angels & Airwaves
- Reference gear direction: Vox AC30 + Fender Twin clean/chime; Fulltone-style boost/OD; dotted-eighth delay; spacious stereo ambience.
- The Adventure starting point: AC30/top-boost analogue, Top Boost 7, Bass 4, Treble 7, Cut ~8; delay around 420 ms, feedback ~30% as a tempo-dependent starting point.
- Use post-cab delay/modulation/ambience where appropriate; keep the core attack clear.

### Sum 41 / Deryck Whibley
- Reference gear direction: Dual Rectifier left and JCM800 right, Les Paul Custom / Duncan JB-style source.
- Fat Lip: Gain 8, Bass 6.5, Mid 5, Treble 6.5, Presence 6.
- Still Waiting: Gain 8, Bass 6, Mid 5, Treble 6, Presence 6.
- No boost is required by this supplied reference. Separate amp chains only count as a layered arrangement when driven by independent performances or deliberately distinct source layers.

### New Found Glory / Chad Gilbert
- Reference gear direction: Dual/Triple Rectifier, Les Paul / Duncan JB-style source, noise gate where needed.
- My Friends Over You: Gain 7, Bass 5, Mid 6, Treble 7, Presence 6.

### Simple Plan / Jeff Stinco
- Reference gear direction: Rectifier rhythm + Vox-style clean layers; Les Paul/PR-style source; TS/OD, delay, compression and envelope/filter colors as arrangement requires.
- Preserve separate clean and driven roles; do not place every pedal on every preset.

### Green Day / Billie Joe Armstrong
- Reference gear direction: modified Marshall Super Lead / British stack, Les Paul Junior P-90-style source, Blues Driver-style boost for some live contexts.
- Starting philosophy: broad control positions near noon with strong master/amp response; gain should feel controlled, not excessively saturated.
- P-90-like source voicing: preserve mid character and avoid brittle highs.

### The Offspring / Dexter Holland
- Reference gear direction: JCM2000 DSL + Vox clean layers; TS-style boost; Friedman BE-100 is a reference direction only if an actual model exists.
- Self Esteem: Gain 7, Bass 6, Mid 6, Treble 7, Presence 6.
- Tight rhythm, controlled low end, optional boost.

### Fall Out Boy
- Reference gear direction: Orange Dual Dark/Thunderverb family, Orange PPC-style cabinet voicing, post-processing.
- Sugar We're Goin Down: Gain 7, Bass 5, Mid 7, Treble 6, Presence 5.
- Dense but mid-forward; preserve chord separation.

## 6. General starting points

| Channel / role | Gain | Bass | Mid | Treble | Presence |
|---|---:|---:|---:|---:|---:|
| Clean | 1–2 | 5–6 | 6 | 6 | 4 |
| Crunch | 4–5 | 5 | 6–7 | 6 | 5–6 |
| Pop-punk distortion | 5.5–6 | 5 | 5.5 | 7 | 6 |
| High gain | 7–8 | 6 | 5–7 | 5–6.5 | 6 |
| Genre baseline | 5.5 | 5 | 5.5 | 7 | 6 |

The same numeric settings on different amps do not produce the same sound. Translate by ear and measured response.

## 7. ANGELA ENGINE runtime contract

The preset system must not be a collection of labels and unconnected numbers. Each tone recipe resolves to a concrete runtime graph:
- source asset/instrument and mapping;
- pickup/source profile and articulation behavior;
- amp model or explicit analogue/custom DSP route;
- cabinet/IR and mic perspective, or explicit bypass;
- FX modules and order;
- independent layer/voice routing when supported;
- output trim and gain staging;
- availability and QA state.

The resolver must fail visibly when a required asset or DSP module is absent. It may select a documented fallback only if that fallback exists and can be instantiated. Browser status vocabulary should distinguish DEFINITION, SOURCE AVAILABLE, DSP AVAILABLE, PLAYABLE, MIX-READY and LISTENING-APPROVED. A JSON profile alone is not PLAYABLE.

### 7.1 Runtime truth and persistence
- Store stable IDs for source, amp, cabinet/IR, mic perspective and FX modules.
- Save/load must round-trip the resolved route and user overrides.
- Preset selection must immediately load the selected tone route; no mismatch between displayed preset and audible state.
- Missing resources must show the exact dependency and chosen fallback (if any).
- RAW/NO FX is strictly unprocessed; never route a baked-in processed source through the RAW category.
- Factory presets remain immutable; user variations save as User Presets.

### 7.2 Realtime safety
All decode, file access, IR preparation, graph construction, allocation and asset scanning happen off the audio callback. Publish prepared immutable state safely. No blocking locks, file/network I/O, GUI calls or unplanned allocation in the callback. Audio DSP changes require separate review and regression tests.

## 8. UI and user-facing reporting

Every preset detail view should expose:
- requested tone target and musical role;
- resolved signal chain in order;
- exact available model names and source/cab provenance;
- any replacement: **requested target → actual replacement → parameter shaping**;
- approximate-vs-direct status;
- missing dependencies/degraded state;
- gain staging and output trim;
- listening QA status.

For a tone-building assistant response, use a compact format:
1. Signal chain.
2. Settings for each block.
3. **REPLACEMENT** block: unavailable target → selected substitute → shaping settings.
4. One principal risk for this tone.
If key information is missing, ask no more than three concise questions, while still providing a sensible default starting point when possible.

## 9. Quality gates

Before any tone can be marked MIX-READY / LISTENING-APPROVED:
- verify source and mapping availability;
- verify runtime graph and actual DSP route;
- verify preset save/load round-trip;
- verify no hidden RAW fallback;
- audition level-matched A/B in solo and full mix;
- test power chords, single notes, palm mutes, open chords, velocity dynamics and note transitions;
- test stereo width, phase, mono compatibility, noise, transients, sustain and clipping/headroom;
- measure CPU and stability under polyphony/streaming load;
- document the listening result and exact build/commit.

## 10. Implementation truth at adoption

This document is a design standard, not proof of implementation. The existing source audit found simplified amp gain/tanh drive and a synthetic cabinet coloration path rather than a complete set of named amp models and recorded cabinet IR captures. Therefore all new artist/song tone references remain reference-only until the runtime graph, assets, DSP, tests and listening gates above are complete.
