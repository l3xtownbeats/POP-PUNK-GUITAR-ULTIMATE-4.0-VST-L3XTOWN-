# PPGU — Pop-Punk Tone Research and Preset Calibration

Status: RESEARCH BASELINE (not yet implemented or audio-verified)
Updated: 2026-11-14
Purpose: build original, playable PPGU presets informed by documented rigs, settings, tuning, arrangement and production practice. Band names are style references only; do not reproduce protected master recordings, stems, riffs or proprietary presets.

## Calibration rules

- Published amp knob values are evidence-based starting points, not universal values. Preserve the original amp model and scale in source notes; translate to PPGU's amp model only after level-matched listening tests.
- Build each preset from the actual recorded sample source + pickup/voicing simulation (where appropriate) + amp/cab + effects + articulation + performance behavior. A preset is not complete because its name or JSON entry exists.
- Record confidence for every fact: CONFIRMED (primary artist/tech interview or official rig rundown), CORROBORATED (multiple credible sources), SECONDARY (specialist tone database/article), or INFERRED (engineering starting point).
- Distinguish album/era/song. One band does not have one immutable guitar tone.
- Avoid excessive preamp gain. Tight low end, controlled low-mid buildup, strong pick attack, double-tracked rhythm width and section-specific dynamics matter as much as gain.
- For rhythm presets implement separate palm-muted verse/chug, open chorus power chords, octave/lead hooks, stop/start mutes, and optional double-track spread. Do not bake stereo widening into a mono sample.
- Validate in a mix context: level-match, high-pass/low-pass appropriately, inspect transient attack, mono compatibility, noise floor, clipping, CPU and voice count.

## Evidence-backed starting points (secondary sources; verify by listening)

### Blink-182 — late-1990s / early-2000s bright, tight high-gain pop-punk
- "All the Small Things": Mesa/Boogie Dual Rectifier; reported 0–10 starting settings gain 6, bass 4, mids 4, treble 7, presence 6; standard tuning; bridge humbucker-style bright attack. Secondary source: ToneMirror song breakdown.
- "Online Songs": Mesa/Boogie Dual Rectifier; gain 6.5, bass 4.5, mids 6, treble 7.5, presence 6.5; standard tuning. Secondary source: ToneMirror song breakdown.
- "Dammit" rig evidence: DiMarzio X2N bridge pickup on the 1994 Fender Stratocaster, Marshall JCM900 + Mesa/Boogie Triple Rectifier blended; standard tuning. Secondary specialist source Rigtone, citing DeLonge interview history.
- Current touring rig is a different era: Premier Guitar Rig Rundown reports Fractal Axe-FX III, jumpered Marshall Plexi basis, Soldano X88-IR, Marshall 4x12/Celestion Vintage 30 IR for drive and Vox AC30 IR for clean. Do not apply this modern chain to every historical preset.
- Preset design: fast transient, controlled low end, bright upper mids, firm palm-mute envelope; separate clean/edge-of-breakup intro and full chorus drive.

### New Found Glory — melodic skate/pop-punk, fast open chords
- Debut-era album aggregate: Marshall JCM800 2203; Stratocaster commonly listed; Boss DS-1 appears; median gain 6.3, bass 5.3, mids 6, treble 6.8, presence 6 (0–10). Secondary source ToneMirror.
- "Sincerely Me": JCM800 2203 + DS-1; gain 6, bass 5, mids 6, treble 7, presence 6; standard tuning. Secondary source ToneMirror.
- "Catalyst" aggregate: JCM800 2203/SG reported; gain 7, bass 5.5, mids 6, treble 6.5, presence 6. Secondary source ToneMirror.
- Preset design: articulate fast downstrokes, bright chord definition, moderate-to-high gain without fuzz, tight palm-muted verse into wider open chorus.

### Good Charlotte — punchy, mid-forward Marshall/Les Paul pop-punk
- Debut album aggregate: Marshall JCM800 2203, Gibson Les Paul Studio, Boss DS-1; median gain 6.3, bass 5.5, mids 5.5, treble 6.5, presence 5.5 (0–10). Secondary source ToneMirror.
- "Say Anything": JCM800 2203 + DS-1; gain 6.5, bass 5.5, mids 6, treble 6.5, presence 6; standard tuning. Secondary source ToneMirror.
- "The Young and the Hopeless": JCM800 2203 + DS-1; gain 6.5, bass 5.5, mids 5.5, treble 6.5, presence 6; Drop D. Secondary source ToneMirror.
- Preset design: focused midrange, tight low end, controlled top end, chunky palm-muted verse, broad chorus chords; support Drop D as a distinct tuning variant.

### Avril Lavigne — pop-rock / pop-punk, clean-to-crunch section contrast
- "Under My Skin" album aggregate: Bad Cat Hot Cat 30 + Gibson Les Paul Custom; median gain 6.5, bass 5.5, mids 6, treble 6.5, presence 6 (0–10). Secondary source ToneMirror.
- "How Does It Feel": Bad Cat Hot Cat 30; gain 6, bass 5.5, mids 6, treble 6.5, presence 6; standard tuning. Secondary source ToneMirror.
- "Together": suggested Les Paul Custom / Bad Cat Hot Cat 30; reported Eb standard; treat rig as suggested rather than confirmed. Secondary source ToneMirror.
- Preset design: less saturated verse, articulate strummed/open chords, controlled crunch chorus; preserve alternate tunings as per-song variants rather than one album-wide assumption.

### The All-American Rejects — power-pop / emo-pop crossover
- Self-titled album aggregate: Marshall JCM2000 DSL100 + Les Paul; median gain 6, bass 5, mids 5.5, treble 6.5, presence 6 (0–10). Secondary source ToneMirror.
- Preset design: balanced power-chord body, polished upper mids, less scooped than the most aggressive skate-punk tones; create clean/edge verse and compressed chorus variants only after listening validation.

## Priority research queue — all 21 required style profiles

These profiles must remain present in the product catalog and receive their own source notes, presets, and validation. Do not infer undocumented gear/settings from a band's name alone.

1. Blink-182 — evidence notes above; expand by album era and song.
2. Sum 41 — research Deryck Whibley's guitar/amp/pickup/recording chains by era; distinguish early skate-punk from heavier Chuck-era tones.
3. Simple Plan — research Jeff Stinco / Sébastien Lefebvre rigs, album-era gain and clean/crunch transitions.
4. New Found Glory — evidence notes above; expand Sticks and Stones / Catalyst and song-specific articulations.
5. Good Charlotte — evidence notes above; expand The Young and the Hopeless / Chronicles of Life and Death.
6. Mest — priority: Photographs (2005); seek album-specific recording and rig evidence before setting values.
7. Angels & Airwaves — distinguish DeLonge's post-Blink spacious, delay/modulation-driven melodic parts from Blink rhythm tones; research album/era.
8. The All-American Rejects — evidence notes above; expand Move Along / When the World Comes Down.
9. MGK — distinguish pop-punk album sessions and collaborators; research actual guitar/amp/production chains rather than assuming one rig.
10. Avril Lavigne — evidence notes above; expand Let Go / Under My Skin / later album eras.
11. The Offspring — research era-specific guitar rigs and tightly gated fast rhythm behavior.
12. Green Day — research Billie Joe Armstrong guitar/amp/cab and era-specific crunch; distinguish Dookie-era from later productions.
13. Bowling for Soup — research bright pop-punk rhythm and lighter gain/clean sections.
14. Sugarcult — research Start Static / Palm Trees and Power Lines guitar production and layering.
15. All Time Low — research album-era rhythm/lead tones and modern production layers.
16. Fall Out Boy — research early pop-punk versus later layered guitar arrangements.
17. Yellowcard — research guitar parts separately from violin arrangement; melodic octave/lead support.
18. My Chemical Romance — research Three Cheers / Black Parade era guitar layering, gain and effects.
19. Less Than Jake — research ska-punk offbeat clean/chop parts alongside distorted chorus guitars.
20. Fenix TX — research album-era guitar/amp chain and fast palm-muted rhythms.
21. Midtown — research Living Well Is the Best Revenge / Forget What You Know era guitar tones.

## Sources consulted

- ToneMirror — Blink-182 "All the Small Things": https://www.tonemirror.so/tones/blink-182-all-the-small-things
- ToneMirror — Blink-182 "Online Songs": https://www.tonemirror.so/tones/blink-182-online-songs
- Rigtone — Blink-182 "Dammit" amp/gear breakdown: https://www.rigtone.com/tone/dammit-amp-settings
- Premier Guitar — Blink-182 Rig Rundown (current touring rig context): https://www.premierguitar.com/videos/rig-rundown/blink-182
- ToneMirror — New Found Glory album and "Sincerely Me" breakdowns: https://www.tonemirror.so/albums/new-found-glory-new-found-glory ; https://www.tonemirror.so/tones/new-found-glory-sincerely-me
- ToneMirror — Good Charlotte debut and "Say Anything": https://www.tonemirror.so/albums/good-charlotte-good-charlotte ; https://www.tonemirror.so/tones/good-charlotte-say-anything
- ToneMirror — Avril Lavigne Under My Skin: https://www.tonemirror.so/albums/avril-lavigne-under-my-skin
- ToneMirror — The All-American Rejects debut: https://www.tonemirror.so/albums/the-all-american-rejects-the-all-american-rejects

Research source quality caveat: ToneMirror and Rigtone are secondary databases, not artist-authored session logs. Use them as leads and calibration starting points, not definitive proof. Premier Guitar is a specialist interview/rundown source but describes the later touring rig, not every studio era.
