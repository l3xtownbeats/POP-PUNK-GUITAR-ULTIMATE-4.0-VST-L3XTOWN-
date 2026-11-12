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
