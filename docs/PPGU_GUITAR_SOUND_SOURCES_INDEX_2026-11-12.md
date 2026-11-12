# PPGU — Guitar Sound Sources Index
Updated: 2026-11-12

This is a discovery index for guitar-only content. A listing is not proof that every asset in a pack can be redistributed inside a commercial VST3. For each imported file preserve the exact source URL, creator, asset ID, license evidence, retrieval date, archive, and SHA-256. Only CC0/public-domain or individually cleared assets with explicit source-file redistribution permission may enter the bundled SoundBank automatically. Unclear terms remain YELLOW/manual review.

## Priority: reusable multisample instruments

1. Meta Open Sound — https://github.com/facebookincubator/Meta-open-sound
   - Steel Guitar: 375 multisamples; Nylon Guitar: 348 multisamples.
   - CC0 1.0 for the instrument collections; WAV 24-bit/48 kHz release archives, OGG previews, SFZ maps, SHA256SUMS and per-collection LICENSE.
   - Instrument collections are suitable candidates for commercial embedding and redistribution under CC0.
2. SFZ Instruments guitar catalogue — https://sfzinstruments.github.io/guitars/
   - Index lists Black And Green Guitars, Emilyguitar and Shinyguitar as CC0-1.0; Blue Jeans And Moonbeams as MIT; other entries are commercial/custom and must not be treated as cleared.
3. Karoryfer free samples — https://shop.karoryfer.com/pages/free-samples
   - Creator states free sample libraries are CC0, but old downloads may have older terms. Verify the LICENSE inside the exact archive.
   - Includes Emilyguitar and other instruments; Black And Green / Shinyguitar are listed in the SFZ catalogue.
4. FreePats Clean Electric Guitar — https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html
   - FSBS Clean #1/#2, including SFZ WAV/FLAC and SF2; page states CC0 1.0.
   - GitHub source bank: https://github.com/freepats/electric-guitar-FSBS-clean
5. FreePats Distorted Electric Guitar — https://freepats.zenvoid.org/ElectricGuitar/distorted-electric-guitar.html
   - Distorted/overdriven variants; SFZ WAV/FLAC and SF2; page states CC0 1.0.
   - FreePats repositories: https://github.com/freepats
6. FreePats Nylon Acoustic Guitar — https://freepats.zenvoid.org/Guitar/acoustic-guitar.html
   - Spanish classical nylon guitar; SFZ WAV/FLAC and SF2; page states CC0 1.0.
7. Blue Jeans And Moonbeams — https://docs.google.com/document/d/15XBK9LqTsNRG14JD23bidiAvaEaj8UXcT8O8PDTnS3o/edit
   - SFZ catalogue lists MIT; verify the canonical author release and license before use. (Catalogue entry is discovery evidence, not sufficient source-file clearance.)
8. Versilian Community Sample Library — https://versilian-studios.com/vcsl/
   - Collection claims CC0 and explicit commercial-software integration permission. Check exact guitar instrument and archive contents; not all instruments are guitars.

## Guitar loops, riffs, stems and one-shots

9. Selekt — https://selektaudio.com/
   - Search guitar-only one-shots, loops and stems. Individual assets can carry CC0 Commercial OK and a clearance certificate. Download the asset-specific certificate and verify that the downloadable file itself is covered.
   - Example CC0 guitar one-shot: https://selektaudio.com/sample/93e09da8-e68b-47bb-9977-4ce6cb22b989
   - Example cleared guitar loop: https://selektaudio.com/sample/09e3804a-e2ef-4de1-9d22-6dab859d5ed7
10. Signature Sounds — Synthetic Guitar Loops — https://signaturesounds.squarespace.com/store/p/synthetic-guitar-loops
    - Site describes 97 MB WAV pack as CC0. Obtain the pack and preserve its included license/receipt before adding it.
11. Freesound — guitar loop example (CC0) — https://freesound.org/people/deadrobotmusic/sounds/570839/
    - 156 BPM C major electric guitar loop; page identifies CC0. Login may be required to download.
12. Freesound — guitar strum/percussive loop (CC0) — https://freesound.org/people/deadrobotmusic/sounds/570837/
    - Page identifies CC0; WAV. Login may be required.
13. Freesound — acoustic guitar loop (CC0) — https://freesound.org/people/deadrobotmusic/sounds/555911/
    - E major / C# minor, 128 BPM; page identifies CC0.
14. Freesound — electric guitar / shoegaze loop (CC0) — https://freesound.org/people/deadrobotmusic/sounds/689160/
    - 96 BPM A# minor; page identifies CC0.
15. Freesound — plucky guitar loop (CC0) — https://freesound.org/people/deadrobotmusic/sounds/789017/
    - 110 BPM A# major; page identifies CC0.
16. OpenGameArt — guitar chord strum (CC0) — https://opengameart.org/content/guitar-0
    - Single guitar chord strum, OGG and WAV; page identifies CC0.
17. SoundPacks — Pasin Guitar Loops — https://soundpacks.com/free-sound-packs/pasin-guitar-loops/
    - 22 WAV files / 96.52 MB, original guitar stems, dry/wet leads and chords. Site says free for use in compositions, but does not provide sufficiently explicit raw-sample redistribution terms on the page; YELLOW/manual permission required before bundling.
18. Pulse Open Loops — https://pulse.studiosphere.space/open-loops
    - No-account previews and WAV downloads; 110 clearly licensed samples with CC0 or CC-BY labels. Search for guitar-tagged individual assets; only use CC0 for automatic intake, or handle CC-BY attribution and confirm redistribution rights per asset. Most catalogue entries are not guitar.
19. SampleStack free multisample source guide — https://samplestack.app/news/free-multisample-instrument-sources/
    - Discovery index for multisample sources; follow through to original creators and verify original licenses.
20. Libre Instrument Audio Database — https://github.com/hcsrtg/libre-instrument-audio-database
    - Community license index; useful discovery metadata only. Confirm each source at the original creator.

## Not cleared for automatic bundling

- MusicRadar SampleRadar, Splice, Loopcloud, commercial sample stores, subscription packs, promo/demo packs with vague terms, and any "royalty-free" library without explicit raw-file redistribution permission.
- "Free download", "use in your music", "commercial use" or no-password access does not alone grant the right to redistribute original WAVs inside a VST3.
- Drum-only loops, bass, vocals and complete song mixes are excluded from the Guitar SoundBank.

## Ingestion checklist

For every selected asset: exact asset-level license; source URL and creator; original archive; SHA-256; file format/sample rate/bit depth/channels; duration; tempo/key/meter for loops; guitar articulation/type; audition; duplicate/fingerprint check; and confirmation that the current sampler supports the format. Network/download/archive parsing must never run in the audio callback.

Current repository Sound Hunter registry is not proof of completed ingestion. This index records candidate sources only; do not claim files downloaded or playable until the actual archive and checksum are present in the project.


## Expanded musician/sample-provider scan — 2026-11-12

21. Ghosthack — Free Sample Packs — https://www.ghosthack.de/free_sample_packs/
    - Site advertises 10,000+ free sounds across 346 packs / 40+ GB and states handmade, AI-free and royalty-free for commercial music use. This is a discovery portal, not a guitar-only pack. Filter for guitar-only files and inspect the license for each pack.
22. Ghosthack — Guitar Sounds — https://www.ghosthack.live/guitar-sounds
    - Live-recorded and processed guitar one-shots, acoustic/electric loops, slides, ambiences, textures and song-kit stems; BPM/key labels are described. Royalty-free for music use, but Ghosthack's terms prohibit redistribution/repacking of the samples. YELLOW/RED for embedding source WAVs in PPGU unless separate written permission is obtained.
23. Ghosthack — Guitar Loops & One-Shots — https://www.ghosthack.live/guitar-loops-and-one-shots
    - Curated WAV loop/one-shot packs. Their FAQ describes royalty-free use in commercial projects, not redistribution of source samples. Do not bundle raw files based on that wording.
24. Function Loops — Free Sample Packs — https://www.functionloops.com/free-samples.html
    - Includes “Deep Twin Guitars”: site describes 650+ MB of live-performed guitar sessions, BPM/key labels, 24-bit WAV and royalty-free use in productions. Royalty-free music-use wording does not establish raw-file redistribution rights; request written permission before bundling.
25. SoundShockAudio — Free Guitar Sample Packs — https://soundshockaudio.com/guitar-sample-packs/
    - Current discovery catalogue reports 45 free guitar packs, including Vibrations, Rico Spanish Guitar, Lo-Fi Guitars and processed-guitar collections. Treat as a discovery index; verify each original creator, file and license. It also lists non-guitar packs, which must be excluded.
26. Slooply — Free Guitar Samples & Loops — https://slooply.com/collection/free-guitar-samples-loops
    - Catalogue currently lists 333 public guitar samples, including acoustic/electric riffs, chord progressions and loops, WAV with BPM/key metadata. “Free” availability is not enough to establish raw-sample redistribution; individual license evidence required.
27. Noiiz / Samplephonics — Tampered Guitars — https://www.noiiz.com/sounds/packs/208
    - 332 sounds, 861 MB ZIP; live session guitarist Carey Clayton; fingerpicking, distorted washes, processed pads, rhythmic patterns, reverse phrases and soundscapes. Paid/membership catalogue. Not cleared for redistribution; use for reference or obtain explicit license.
28. Samplephonics — Tampered Guitars Freebie — https://www.samplephonics.com/products/free/guitar/tampered-guitars-freebie
    - Free selection of processed guitar WAVs, described as royalty-free for music use; download now routed through Noiiz. Source redistribution is not established.
29. Samplephonics — Acoustic Grooves Freebie — https://www.samplephonics.com/products/free/guitar/acoustic-grooves-freebie
    - Free selection from a larger live-recorded acoustic pack; full collection describes strummed chords, solos, melodies and fingerpicking. Verify the exact freebie license and Noiiz delivery terms.
30. Abletunes — Free Acoustic Guitar Loops — https://abletunes.com/blog/free-acoustic-guitar-loops/
    - 20 free WAV loops sampled from a 255-loop live acoustic guitar pack; full pack is described as 24-bit WAV, 120–126 BPM and minimally processed. Freebie/music-use permission does not by itself grant raw WAV redistribution.
31. Abletunes — Acoustic Guitar Volume 1 — https://abletunes.com/product/sample-packs/acoustic-guitars-sample-pack-vol1
    - 255 live acoustic guitar loops, 915 MB, 24-bit WAV, key/tempo-labelled; paid pack. Royalty-free for music production, but not automatically cleared for bundling in a sampler.
32. Godlike Loops — Godlike Guitars — https://godlikeloops.com/product/godlike-guitars/
    - Free pack claims 10 acoustic/electric guitar loops, MIDI, 24-bit WAV, stems, key/tempo labels and royalty-free use in commercial compositions. Need confirm whether the current license permits redistribution of the original WAV/stems inside a commercial plugin.
33. Unison — Free Packs — https://unison.audio/free-packs
    - Includes Free Guitar Loops and Beatmaker Guitar Pack teaser. Download eligibility and music-use terms should be checked per pack; no raw-source redistribution clearance established.
34. FiftySounds — Free Loops and Samples — https://www.fiftysounds.com/royalty-free-music/free-loops-and-samples.html
    - Individual WAV guitar loops are available. However, its license expressly prohibits selling, reselling, sublicensing or distributing its music; exclude from bundled SoundBank.
35. Lots of Sounds — Guitar Sounds, Riffs and Loops — https://www.lotsofsounds.com/sounds/category/music-loops/guitar
    - Catalogue reports 892 CC0 guitar-tagged sounds, including live chord strums, electric solos and processed guitar recordings. The catalogue also contains mislabeled/non-guitar and synthetic entries; inspect and audition each file. MP3 delivery, so not suitable as multisample source without conversion and quality review; CC0 makes eligible individual assets candidates for bundling.
36. Freesound — https://freesound.org/
    - Broad source of live-recorded guitar strums, chords, riffs, slides, harmonics and room/amp recordings. Search individual guitar-only files; select CC0 assets for the easiest redistribution path. CC-BY needs attribution and legal review; NC/ND/unknown excluded.
37. Splice — Guitar samples — https://splice.com/sounds/instruments/guitar/samples
    - Large catalog with live sounds and electric guitar filters; subscription/credit downloads. Splice grants royalty-free use in music, but that is not a general right to redistribute the raw sample as a competing sample library/plugin content. Do not bundle without a separate license.
38. NDA Samples — Rock Guitar Loops — https://ndacompany.de/loops/rock-guitar
    - Real-instrument, non-AI guitar loops offered as free MP3 downloads; terms require a 50/50 split for commercial releases. Not royalty-free and not suitable for PPGU distribution without a separate agreement.
39. Musician's HQ — Free Guitar Sample Packs & Loops — https://musicianshq.com/free-guitar-sample-packs-loops/
    - Secondary discovery roundup mentioning Ghosthack, Abletunes and Echo Sound Works. Use it to discover packs, not as license evidence.
40. FiftySounds license — https://mail.fiftysounds.com/music-license.html
    - Explicitly disallows reselling, sublicensing or distributing its music; this overrides generic “free/commercial use” descriptions for our embedding decision.

### License classification from this scan

- **GREEN candidates:** individual CC0 guitar assets from Meta Open Sound, FreePats, verified CC0 Freesound entries, and verified CC0 entries at Lots of Sounds. Still preserve the exact file-level evidence and check that the content is actually a live guitar.
- **YELLOW / permission needed:** Ghosthack, Function Loops, Samplephonics/Noiiz, Abletunes, Godlike Loops, Unison, Slooply, Pasin Guitar Loops and any pack described only as royalty-free/free for music use.
- **RED / exclude from bundled source files under current terms:** FiftySounds music and NDA Samples rock guitar loops; MusicRadar SampleRadar and Splice are also not cleared for raw sample redistribution under their standard music-use licenses.
- No missing license text is treated as permission. Silence is UNKNOWN, not GREEN.


## Priority expansion: pattern-engine material (punk first)

The user's sound hierarchy for the pattern engine is now:
1. **PUNK (highest priority):** pop-punk, punk rock, emo-punk, skate punk, melodic hardcore and adjacent punk guitar performances. Seek real-player power chords, palm-muted eighths, octave riffs, open-chord choruses, ska upstrokes, leads, fills, transitions, endings, harmonics, slides and pick/noise articulations.
2. **GENERAL GUITAR LOOPS:** any style of real acoustic/electric guitar, any chord progression, tempo, key, meter, strumming/fingerpicking pattern or loop format.
3. **ROCK GUITARS:** broader rock recordings and patterns, only after the first two tiers.
4. **DRUM/PERCUSSION ONE-SHOTS:** may be used as supporting pattern-engine material when they are musically suitable (kick, snare, hats, crashes, toms, tambourine, claps). Keep them in a separate drum/percussion bank, never mislabel them as guitar samples. Do not let generic drum packs displace the punk-guitar priority.

41. Spare Bedroom Studio — Pop Punk Loop Pack — https://shop.sparebedroomstudio.com/products/pop-punk-loop-pack-guitar-loops
    - Product page states 3,600+ loops played by a real guitarist: full chords, palm-muted riffs, leads, ska rhythms, octave stabs and one-shots; every key, five tempos (145/155/170/185/200 BPM), processed and raw DI WAV; claimed size 17.53 GB.
    - Excellent stylistic match and a strong benchmark for pattern coverage. Paid pack. “Royalty-free” is for music production; no proof of permission to redistribute the raw WAV files inside PPGU. Do not bundle without written permission.
42. Looperman — live punk/emo guitar loop examples:
    - https://www.looperman.com/loops/detail/421913/shooting-star-pop-punk-x-emo-guitars-free-175bpm-punk-electric-guitar-loop (175 BPM, B, 4/4; uploader describes live guitars, pop-punk/rock/alternative/emo).
    - https://www.looperman.com/loops/detail/419325/high-emo-rock-x-pop-punk-guitars-free-135bpm-rock-electric-guitar-loop (135 BPM, A, 4/4; uploader describes live guitars).
    - https://www.looperman.com/loops/detail/394898/its-over-pop-punk-x-emo-rock-gtrs-free-160bpm-punk-electric-guitar-loop (160 BPM, D, 4/4; uploader describes live guitars).
    - https://www.looperman.com/loops/detail/314315/the-edge-mgk-x-blackbear-guitars-free-171bpm-punk-electric-guitar-loop (171 BPM, G; uploader describes live guitars).
    - Looperman grants use under its loop terms for music projects, but that is not automatic permission to republish the sample itself in a plugin. Treat as YELLOW until the current terms and uploader permission explicitly cover embedded redistribution.
43. Splice — Punk Guitar category — https://splice.com/sounds/genres/punk/guitar/packs
    - Search result reports 996 samples, 30 presets, 10 packs and filters for electric guitar, drums, bass and more. Great discovery/benchmark source for punk production; standard royalty-free music-use terms do not grant redistribution of the source samples as a competing library. Do not bundle.
44. Splice — Rock Guitar samples — https://splice.com/sounds/genres/rock/guitar/samples
    - Useful broad-rock discovery source; same raw-file redistribution restriction.
45. Soundza — Rock & Indie packs — https://soundza.com/free-sample-packs/rock-indie
    - Free 24-bit WAV packs; site license permits commercial music use but expressly prohibits reselling or redistributing unmodified sounds as a pack. Not suitable for bundling source files without a separate agreement.
46. Selekt — cleared drums and percussion — https://selektaudio.com/sound-lab/drums
    - Catalogue advertises 20,000+ drum/percussion samples, loops, one-shots and kits with per-file CC0, Public Domain or CC-BY labels and source/certificate details. Select only punk-useful acoustic hits/performances; save each certificate. CC-BY requires attribution handling.
47. Karoryfer — Big Rusty Drums — https://shop.karoryfer.com/pages/free-big-rusty-drums
    - Large acoustic drum multisample candidate listed as CC0-1.0 in the SFZ Instruments drum catalogue. Verify the exact current archive's LICENSE and mapping before importing.
48. Salamander Drumkit — https://sfzinstruments.github.io/drums/salamander/
    - Acoustic drum SFZ library listed as CC0; verify original author release, archive and license file. Useful for realistic kick/snare/toms/cymbals in pattern engine.
49. Versilian — Virtuosity Drums — https://versilian-studios.com/virtuosity-drums/
    - Jazz/acoustic drum SFZ library reported CC0; check exact archive license and whether the sound suits punk patterns before inclusion.
50. Soundza — Dust Jacket Lo-Fi Acoustic Drum Kit — https://soundza.com/packs/dust-jacket-lo-fi-acoustic-drum-kit
    - Page labels 93 WAV, 24-bit, CC0. More lo-fi than punk; optional secondary color only after audition and archive-level license verification.
51. Producer Space — https://producerspace.com/
    - Site claims CC0 for 2,322 WAV and 139 MIDI across 18 packs, but the visible inventory is mainly drums/808s/vocals/FX rather than a confirmed punk guitar source. Discovery only; do not assume guitar content.

### Pattern-engine ingestion rules

- Prefer multi-velocity, multi-articulation DI/amp multisamples and short playable one-shots for note-triggered guitar voices. Use loops/riffs/progressions as phrase-pattern assets, not as substitutes for a chromatic playable instrument.
- Tag every guitar asset with tier (PUNK / GENERAL GUITAR / ROCK), live-performance evidence, instrument type, tuning, articulation, BPM, key, meter, loop length, chord/root metadata, processed-vs-DI, dry/wet, license and SHA-256.
- For drum one-shots, maintain a separate kit taxonomy (kick/snare/hat/ride/crash/tom/percussion), velocity layers and round-robin variants where available.
- The pattern engine may reference these materials only after the file is actually downloaded, auditioned, checksum-verified, license-cleared and tested in the target playback path. No synthetic guitar generation or synthetic guitar substitutes.
- Current entries are discovery leads, not a statement that these new packs have been downloaded or imported.



63. FreePats — FSBS Jazz Electric Guitar — https://github.com/freepats/electric-guitar-FSBS-jazz
    - Real Fender electric guitar direct-sampled and processed through an amplifier/effects rack for a jazz tone; repository README identifies CC0 1.0 and contains SFZ mappings, FLAC recordings and LICENSE.txt. Suitable as GENERAL GUITAR / clean-jazz colour; archive is now added to the automated CI download manifest.
64. FreePats — Spanish Classical Guitar — https://github.com/freepats/spanish-classical-guitar
    - Real Spanish classical guitar recordings; repository README identifies CC0 1.0 and includes SFZ mappings, FLAC samples and LICENSE. Suitable for GENERAL GUITAR acoustic layers and melodic/chordal pattern material; archive is now added to the automated CI download manifest.

65. Karoryfer — Black And Green Guitars — https://github.com/sfzinstruments/karoryfer.black-and-green-guitars
    - Two real hollowbody electric guitars (Gretsch Anniversary and Hofner Club), over 1,500 samples / approximately 500 MB, including short/long notes, feedback and instrument-specific articulations. Repository contains CC0-1.0 LICENSE; added to automated CI SoundBank download.
66. Karoryfer — Emilyguitar — https://github.com/sfzinstruments/karoryfer.emilyguitar
    - Real Epiphone Emily the Strange electric guitar; 323 WAV samples, four velocity layers, three round robins, release/noise samples and SFZ mappings including power/minor/major barre-chord patterns. CC0-1.0 LICENSE; added to automated CI SoundBank download.
67. Karoryfer — Shinyguitar — https://github.com/sfzinstruments/karoryfer.shinyguitar
    - Real archtop guitar recorded from both pickup and microphone; 846 WAV samples, 24-bit/44.1 kHz, multiple articulations and control mappings. CC0-1.0 LICENSE; added to automated CI SoundBank download.


## Follow-up scan: verified high-priority candidates and pattern material

52. Freesound — Pop Punk Guitar Loop, 87 BPM, C major — https://freesound.org/people/deadrobotmusic/sounds/591622/
    - Asset page identifies it as electric/pop-punk/emo/rock guitar, WAV, 5.517 s, 44.1 kHz, 32-bit, stereo, 1.9 MB and CC0. This is a directly relevant PUNK-tier loop. Download currently requires Freesound login; no account/session was available in this workflow, so the file has NOT been downloaded.
53. FreePats — FSBS Electric Guitar Clean #1/#2 — https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html
    - Current page version 2026-08-07; explicitly direct-sampled Fender electric guitar, amplifier/effects processed clean tone, CC0 1.0. WAV/SFZ variants are listed at 456 MiB and 193 MiB; FLAC variants are 118 MiB and 63 MiB. Suitable high-quality GENERAL GUITAR multisample candidates. Source page confirms license and sample origin.
54. FreePats — FSBS Electric Guitar Direct — https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html
    - Raw DI direct electric guitar, CC0 1.0; WAV/SFZ 153 MiB and FLAC/SFZ 74 MiB. Strong foundation for the PPGU amp/FX chain; requires amplifier/cabinet processing in the plugin to sound finished.
55. FreePats — FSBS Electric Guitar Distorted #1/#2 — https://freepats.zenvoid.org/ElectricGuitar/distorted-electric-guitar.html
    - Direct-sampled electric guitar processed through amplifier/effects rack for distorted/overdriven tone; CC0 1.0. WAV/SFZ variants listed at 600 MiB and 245 MiB; FLAC/SFZ variants 300 MiB and 129 MiB. High-priority PUNK/ROCK tone candidates.
56. Meta Open Sound — Steel Guitar / Nylon Guitar release archives — https://github.com/facebookincubator/Meta-open-sound/releases
    - Release page confirms 24-bit/48 kHz WAV source and CC0 1.0 for both guitar instruments. Steel guitar: 375 samples; nylon guitar: 348 samples; each has SFZ mappings and 3 velocity layers. Strong acoustic/GENERAL GUITAR foundation.
57. Selekt — drum and percussion sample catalogue — https://selektaudio.com/sound-lab/drums
    - Page offers filters for one-shots/loops, BPM, key, CC0/CC-BY/Public Domain and categories including kick, snare, hats, cymbals, toms, tambourine and percussion. Candidate source for the separate PUNK drum bank; inspect each item's original source, certificate and exact license before import.
58. Looperman — Shooting Star, Pop Punk x Emo Guitar, 175 BPM, B major — https://www.looperman.com/loops/detail/421913/shooting-star-pop-punk-x-emo-guitars-free-175bpm-punk-electric-guitar-loop
    - Uploader describes live guitars, 4/4, pop-punk/rock/alternative/emo. Good pattern-engine candidate but current Looperman terms/uploader permission must be checked for redistribution inside a plugin; not cleared yet.
59. Looperman — 6 AM, Pop Punk x Rock Guitar, 125 BPM, C major — https://www.looperman.com/loops/detail/419324/6-am-pop-punk-x-rock-guitars-free-125bpm-punk-electric-guitar-loop
    - Uploader describes live guitars and pop-punk/rock/alternative. Same redistribution review required.
60. Looperman — MGK x Pop Punk Guitar, 170 BPM, D# — https://www.looperman.com/loops/detail/298054/last-time-mgk-x-pop-punk-guitars-by-tahto-free-170bpm-punk-electric-guitar-loop
    - Uploader describes live guitars, pop-punk/punk rock. Same redistribution review required.
61. Soundza — Rock & Indie free packs — https://soundza.com/free-sample-packs/rock-indie
    - Site explicitly permits commercial music use but prohibits reselling or redistributing unmodified sounds as a pack. Do not embed raw source samples in the distributed PPGU SoundBank without separate written permission.
62. Pulse Open Loops — CC0 complete song-starter kits — https://pulse.studiosphere.space/open-loops/kits
    - 24 kits; four-bar drums/bass/chords/percussion WAVs; CC0. However the page explicitly says these are original oscillator/noise synthesis, not stems from recorded performances. Do NOT use them as guitar recordings or as evidence of real guitar sound. They may be considered only as generic pattern-engine timing/arrangement references, not guitar assets.

### Download and import status — important

- The current GitHub connector can read/write UTF-8 repository text and inspect GitHub Actions, but it does not expose a general binary upload/download-to-repository action for arbitrary sample archives.
- Freesound's directly relevant CC0 pop-punk loop page requires login to download; no authenticated Freesound connection is available in this task.
- The Meta and FreePats pages confirm high-quality, redistributable candidate libraries and their licenses, but this pass has not placed their WAV archives into the PPGU repository or verified their local SHA-256.
- Therefore these are source/license-verified candidates, not imported audio assets. Do not count them toward the packaged SoundBank size or claim they are playable in the engine yet.


## 68. Native Instruments Session Guitarist / Komplete Start — REFERENCE ONLY, NOT A REDISTRIBUTABLE SOURCE
- Official Session Guitarist products are commercial Kontakt instruments; a free Kontakt Player is only the playback host and does not make paid Session Guitarist sample content free.
- Native Instruments' current EULA permits included samples/instruments for music and audio productions, but expressly prohibits using them to create a sound library or sample-based virtual instrument, and prohibits repackaging samples/loops as standalone sample content.
- The Native Instruments subscription terms separately prohibit using subscription content as a sound library or sample-based product. Therefore no Session Guitarist/Kontakt/Komplete Start audio is to be extracted, transformed, resampled, or bundled into PPGU unless a separate written license explicitly grants that right.
- Permitted development use: study publicly demonstrated playing techniques, articulation concepts, arrangement patterns and broad tonal characteristics; then create independent PPGU recordings from musicians / cleared CC0 recordings, with original performances and sound design. Do not recreate protected recordings or redistribute source audio.
- Official references:
  - https://www.native-instruments.com/products/kontakt-player
  - https://www.native-instruments.com/collections/free
  - https://www.native-instruments.com/pages/end-user-license-agreement
  - https://www.native-instruments.com/pages/subscription-terms-and-conditions
  - https://www.native-instruments.com/de/products/session-guitarist-strummed-acoustic
- Status: YELLOW / REFERENCE ONLY. No source audio downloaded or included in PPGU SoundBank.



## 2026-11-12 research expansion — additional electric-guitar candidates

The following sources were found in a fresh search. They are candidates, not claims of downloaded or integrated assets:

14. FreePats Electric Guitar FSBS Clean — refreshed source version 2026-08-07
   - Direct-sampled electric guitar; SFZ mapping; CC0 1.0 is stated in the repository README/LICENSE.
   - Candidate for clean DI/amp tone and later amp/cab processing. Inspect the exact archive, sample coverage, velocity/round-robin and note-off behavior before adding.
   - https://github.com/freepats/electric-guitar-FSBS-clean
15. FreePats Electric Guitar FSBS Jazz — source version 2026-08-07
   - Direct-sampled electric guitar processed for a jazz tone; SFZ; CC0 1.0 stated in repository.
   - Useful as a contrasting clean/soft electric source, not a pop-punk-specific articulation bank.
   - https://github.com/freepats/electric-guitar-FSBS-jazz
16. Nyanshell Strat — customized Stratocaster American Deluxe, SFZ/WAV virtual guitar
   - Repository describes a custom Strat pickup/strings/recording setup and SFZ player use.
   - License terms were not established from the surfaced project description; keep YELLOW/quarantine until an explicit license is located. README notes missing mf/f dynamics and common chord samples, so do not assume complete velocity or chord coverage.
   - https://github.com/nyanshell/nyanshell.strat
17. fakeguitar — electric guitar SFZ library, reported as having three articulations
   - Discovered through a third-party sample-library index, not yet verified against the original author repository or archive.
   - License and redistribution rights are unknown; keep YELLOW/quarantine. Do not package or redistribute until primary-source license evidence and exact archive are checked.
   - Discovery index: https://github.com/AprilDolly/Free-Audio-Software-List/blob/main/free_sample_libraries.md
18. Unreal Instruments Metal GTX / Standard Guitar — multi-articulation electric guitar candidates
   - SFZ Instruments catalogue lists Metal GTX (about 1.3 GB) and Standard Guitar (about 716 MB), with custom licensing rather than CC0.
   - Potentially valuable for articulation breadth, but not cleared for redistribution. Contact/inspect exact EULA for embedding raw samples in a commercial VST3 before any download intended for bundling.
   - Catalogue: https://sfzinstruments.github.io/guitars/

**Research conclusion:** FreePats Clean/Jazz are the strongest newly confirmed permissive electric-guitar leads. Nyanshell and fakeguitar are technical leads only until licensing is established. Metal GTX/Standard Guitar may have useful multi-articulation content, but custom terms block bundling until explicitly cleared. None of these findings changes the empty asset registry or proves sample ingestion.


## 2026-11-12 — new guitar-library discovery queue (priority: download + legal/technical intake)

These are newly researched leads beyond the already integrated 10 CC0 package families. A search result or product page is not proof of a downloaded archive. No candidate below is marked integrated.

19. **Guitar DI Lite — Boyss-Sound-e-Scapes (free 7th-anniversary Lite release)**
   - Official product page describes a dry electric-guitar multisample instrument; the full Guitar DI has 5 round robins and 3 velocity layers, but do not assume Lite has identical coverage. Third-party Nov 7, 2026 report describes the Lite pack as 96 WAV samples, 2 RR and 3 velocity layers; verify against the actual download.
   - Formats on the developer page: SFZ, Kontakt, Decent Sampler and unlocked WAV. Dry DI could be useful as a source for PPGU's own amp/cab processing.
   - Official source: https://www.boyss-sound-e-scapes.com/guitar-di
   - **License:** no explicit redistribution/embedding permission was established from the surfaced official page. Status **YELLOW / QUARANTINE**. Download for inspection only if the archive is accessible; do not bundle or redistribute samples until the included license or written permission explicitly covers embedding in a commercial sample-based VST.
   - Technical intake: inspect actual Lite archive, SFZ mappings, note range, velocity/RR coverage, note-off, loop/sustain behavior, file size and SHA-256.

20. **Blue Jeans And Moonbeams / Ella Gitauru — Malaclypse the Younger**
   - SFZ Instruments catalogue lists MIT; includes S-type electric and steel-string acoustic guitar. A recent technical review reports Ella Gitauru has four velocity layers and a 231 MB RAR of Ogg samples, and that its user guide includes MIT boilerplate plus an attribution condition for distributed alterations.
   - Catalogue: https://sfzinstruments.github.io/guitars/
   - **License:** promising but not yet approved for packaging. Treat as **YELLOW / QUARANTINE** until the exact archive's guide/license and rights over the sound recordings are reviewed. MIT software wording alone must not be assumed to clear third-party audio. Confirm whether redistributed sample assets and commercial plugin embedding are explicitly permitted; preserve required notices/attribution if cleared.
   - Technical intake: verify exact version, archive contents, sample format/rate, velocity coverage, key/fret mapping, articulation/key-switch data and SHA-256.

21. **FreePats FSBS Clean / Jazz (2026-08-07 versions) — update watch, not a new integrated library**
   - Official FreePats states CC0 1.0 and provides SFZ WAV/FLAC options. These versions are already represented in the previously confirmed FreePats family inventory; do not count them as new assets or duplicate the download unless archive checksums/version show a genuinely new payload.
   - Sources: https://freepats.zenvoid.org/ElectricGuitar/clean-electric-guitar.html ; https://github.com/freepats/electric-guitar-FSBS-clean ; https://github.com/freepats/electric-guitar-FSBS-jazz
   - Status: **GREEN license / version-refresh candidate**; compare manifests and hashes before replacing existing files.

22. **VCSL — Versilian Community Sample Library**
   - Repository declares CC0 and publishes an SFZ release; broad orchestral collection, not a dedicated pop-punk guitar pack. Search inventory for any guitar-family content before considering it relevant; avoid importing unrelated instruments.
   - Source: https://github.com/sgossner/VCSL
   - Status: **GREEN repository-level license lead / GUITAR CONTENT UNCONFIRMED**. Do not treat as a guitar asset until a guitar instrument is located and its per-folder provenance checked.

**Priority order for next intake:** (1) find genuinely new electric/steel-string guitar libraries; (2) obtain actual archives; (3) inspect per-asset license and redistribution rights; (4) verify SFZ/WAV maps, articulations, velocities/RR, quality and SHA-256; (5) only then ingest into the asset registry and SoundBank. Keep commercial/custom-license candidates separate. Never count a search result, catalogue entry, or metadata-only discovery as a downloaded library.
