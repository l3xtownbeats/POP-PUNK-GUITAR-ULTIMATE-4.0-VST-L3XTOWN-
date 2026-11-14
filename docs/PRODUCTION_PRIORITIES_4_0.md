# POP PUNK GUITAR ULTIMATE 4.0 — production priorities

This document records the non-negotiable product priorities for the 4.0 release.

## Product goal

A Windows x64 VST3 instrument for FL Studio 20+ that opens quickly and feels immediate: selecting a factory preset or sound should not cause long waits, UI freezes, audio-thread disk access, or large avoidable memory spikes. The user should be able to play a useful guitar sound immediately after loading the plugin.

The musical experience is inspired by the workflow and playability of Session Guitarist – Electric Sunburst Deluxe. This is a functional reference only: do not copy proprietary code, samples, UI assets, or commercial content.

## Required release scope

1. **At least 300 unique, fully implemented and audio-validated factory presets across MELODY + PATTERN (hard release minimum); 500–600 is a stretch goal**, organized by style, artist-inspired category, gain/amp character, and use case. Include the agreed pop-punk reference palette: Blink-182, Sum 41, Simple Plan, New Found Glory, Good Charlotte, Mest, Angels & Airwaves, All-American Rejects, MGK, and later Avril Lavigne-era pop-punk. Artist names describe tonal inspiration, not endorsements or copied recordings.
2. **Melody mode**: each MIDI key plays the corresponding sampled guitar note; correct pitch mapping, velocity layers, round-robin where available, palm mute/open articulations, release behavior, bounded polyphony, voice stealing, and acoustic/electric instrument profiles.
3. **Pattern mode**: playable guitar performances following the player's chord changes, with Open Strum, Tight Palm Mute, Dead/Mute Percussion, Power Riff, Arpeggio/Picking, and Alternate Down-Up families. Use real performance-event/sample metadata. Do not fake this with a generic chord pad or blindly transposed MIDI phrase.
4. **Fast preset and sound loading**: scan/index libraries off the audio thread; lazy-load/prefetch sample data; cache recently used material; show loading/coverage status; keep audio callback allocation-free and free of file I/O/locks; release unused bank memory safely. Preset browsing must remain responsive while audio is playing.
5. **Library support**: SFZ/WAV loading, explicit sample-root/velocity/articulation metadata, missing-file handling, independent Steel Guitar and Nylon Guitar banks where licensed. Bundle only redistributable samples and include attribution/license manifests and SHA-256 hashes. Never include commercial Session Guitarist or commercial riff-pack samples.
6. **FX and tone**: distortion/amp tone, chorus, flanger, delay, reverb, and acoustic mode. Effects must be bounded and efficient, with sensible defaults and no unnecessary high-CPU processing.
7. **Preset system and browser**: MELODY/PATTERN tabs, search, categories/folders, favorites, previous/next, save user preset, import/export banks. File types: .ppgm, .ppgp, .ppgbank, optional .ppgstate. These formats must be connected to the actual runtime loader/saver before being advertised as usable.
8. **Interface**: retain the previously approved visual direction and skin source; add selectable skins without changing functionality. Keep controls clear and immediate.
9. **Performance controls**: bounded voices/polyphony and a practical quality setting. Do not add a fake “multicore” control unless processing is genuinely parallelized and measured.

## Acceptance gates

- Clean Windows x64 VST3 build and successful Pluginval.
- Automated tests for sample mapping, velocity layers, articulation selection, preset serialization, chord recognition, legal string/fret voicings, pattern scheduling, tempo sync, voice limits, and missing/corrupt samples.
- Manual FL Studio 20+ checks: load/unload, MIDI, automation, preset switching during playback, Melody+Pattern together, tempo/transport, save/reopen, audio-device change, and extended playback.
- Measure cold-start time, preset-switch latency, sample-bank memory, and CPU on a named test machine.
- Audit all bundled samples and publish license/attribution/hash manifest.
- Ship VST3, required sample assets, licenses, version information, and installation instructions.

## Status discipline

A catalog, mockup, design document, static audit, or successful compile is not proof of a working runtime feature. Track each feature as implemented, partial, deferred, or unverified. Do not label 4.0 production-ready or publish it as final until every release gate is evidenced.

## Host compatibility and sampler workflow (confirmed)

- **Operating system floor:** Windows 10 x64, version 1809 (build 17763) or newer. Do not silently raise the minimum OS version. The release build and dependency choices must be checked against this baseline.
- **DAW host:** FL Studio 20 is the required compatibility baseline. Validate VST3 discovery, loading/unloading, MIDI input, automation, preset/state save and restore, audio-device changes, and sustained playback in FL Studio 20 on Windows 10 1809 or newer. Newer FL Studio versions may be tested as additional coverage, not as a substitute.
- **Kontakt 6-like operating model:** treat the plugin as a sample-based instrument/sampler with a browsable sound library, loadable instrument/preset content, mapped multisamples and velocity/articulation zones, audition/preview, and playable content immediately after selection. This describes workflow and sampler architecture only; do not copy Kontakt's proprietary code, UI assets, or content.
- **Low system footprint is a release requirement:** prioritize fast cold start, fast preset switching, low idle and playback CPU, bounded RAM, and no audio glitches while browsing/loading. Perform library discovery/indexing and sample preparation outside the audio callback; use lazy loading, prefetch and bounded caching; swap prepared immutable sample/bank state safely. The audio callback must not do disk I/O, allocate memory, or wait on blocking locks.
- **Performance evidence:** record cold-start time, preset-switch latency, peak/steady-state RAM, idle/playback CPU, and glitch-free playback on the minimum supported OS/host configuration. Do not claim “lightweight” based only on source inspection or a successful compile.

## Core playing concept — preset-first, keyboard-playable instrument

The central user workflow is: choose a musical preset (for example, **Lead**) in the desired mode, then write/play music directly from the MIDI keyboard or FL Studio Piano Roll.

- **Melody / Lead preset:** every pressed MIDI key plays its own corresponding single guitar note. The keyboard behaves chromatically: each key is a distinct note, with the selected preset supplying its guitar sound, articulation and response.
- **Pattern / Lead preset:** every pressed MIDI key triggers a guitar harmonic/rhythmic performance associated with that key's note. The key determines the musical note/harmonic position for the performance; the preset supplies the pop-punk rhythm, guitar performance, voicing and articulation. This must be a playable, note-addressable pattern instrument, not a one-shot drum-pad bank or one fixed loop that ignores which key was pressed.
- **AutoChords in Pattern:** when AutoChords Key/Scale is enabled, the selected tonic and scale guide the harmonic/chord content associated with the pressed key so the performance stays in the chosen harmony. When disabled, the Pattern preset's direct key-to-harmony mapping is used without scale remapping.
- The selected preset may be named **Lead** in either mode, but its behavior is determined by the active mode: single-note melodic playing in Melody, harmonized rhythmic guitar performance in Pattern.
- Pattern performance must respond musically to successive keys and chord/note changes, maintain timing and tempo sync, and stop/release correctly on note-off/transport stop. Acoustic and electric presets use their own appropriate pop-punk performance material.

## Clarified keyboard behavior: Melody vs Pattern

The two modes share a direct one-key-at-a-time MIDI keyboard workflow, but the musical result is different:

- **Melody mode:** every MIDI key triggers its own correctly mapped chromatic guitar note from the selected preset. Example: C4 triggers C4, D4 triggers D4. The preset determines instrument/tone, articulation, velocity response, and performance character.
- **Pattern mode:** every MIDI key is a chord trigger/root-position mapping entry, not a single-note trigger. Each key selects its assigned chord and launches a complete, genre-appropriate guitar rhythm performance for that chord. Adjacent keys select different chords. The selected preset determines the chord map/voicing behavior, acoustic or electric guitar character, and pop-punk rhythmic performance.
- Pattern performances must use guitar-specific strumming/picking/performance events (including down/up strokes, palm-muted strokes, dead/muted hits, power-chord rhythms, and acoustic strums as appropriate), with correct timing, chord transitions, tempo synchronization, and note-off/stop behavior. Do not implement Pattern as a generic MIDI pad, a single sustained chord, or an unrelated loop that ignores the played key.
- Pattern presets must identify their instrument profile (acoustic or electric), tuning/voicing assumptions, chord-key map, rhythmic pattern/event data, tempo behavior, and articulation/performance metadata. Melody presets must identify their note/sample mapping and articulation metadata.
- **Kontakt 6 / Session Guitarist-style workflow reference:** the user chooses an instrument/preset and plays it from the MIDI keyboard; the two modes above define the actual note-trigger behavior. This is a workflow and musical interaction reference only, not a request to copy proprietary code, samples, UI assets, or commercial content.

## Pattern mode — AutoChords harmony controls

The scale/tonic selection behavior belongs specifically to **AutoChords mode** within Pattern mode. Do not present it as a global Pattern behavior or conflate it with the ordinary direct chord map.

When Pattern's **AutoChords** mode is active, provide two harmony workflows:

1. **Key/Scale enabled:** the user selects a tonic/root note and a musical key/scale. Incoming MIDI keys are interpreted through a deterministic, documented scale-aware chord map so the triggered guitar chord/performance stays harmonically compatible with the selected key. Changing tonic or scale updates the mapping without stuck notes or timing glitches.
2. **Key/Scale disabled:** AutoChords does not apply scale-based remapping; MIDI keys use the chord assignments defined by the selected AutoChords preset/map.

The Pattern UI must identify this control as part of AutoChords and expose the Key/Scale switch plus tonic and scale selectors when enabled. Both workflows must work with acoustic and electric pop-punk Pattern presets and preserve settings in preset/plugin state. Each chord trigger launches the preset's guitar-specific rhythmic performance, not merely a sustained chord. Test both workflows across key changes, chord transitions, tempo sync, note-off/stop, and DAW state save/restore.

Pattern mode must provide two user-selectable harmony workflows:

1. **Key/Scale mode ON:** the user selects a tonic/root note and a musical key/scale. Incoming MIDI keys are interpreted through a documented scale-aware chord map so the triggered chord/performance is harmonically compatible with the selected key. The mapping must be deterministic and visible/documented; it must not silently produce unrelated chords. Changing tonic or scale updates the mapping without stuck notes or timing glitches.
2. **Key/Scale mode OFF (Direct Chord Map):** incoming MIDI keys trigger the chords assigned directly to those keys by the selected Pattern preset, without scale-based remapping. The preset defines the key-to-chord assignments and voicings.

The Pattern UI must expose the mode switch and, when Key/Scale mode is enabled, tonic and scale selectors. Both workflows must work with acoustic and electric pop-punk Pattern presets and preserve the selected settings in preset/plugin state. Pattern chord triggers must launch the preset's guitar-specific rhythmic performance, not merely a sustained chord. Test both modes across key changes, chord transitions, tempo sync, note-off/stop, and DAW state save/restore.

## Consolidated specification

The complete Russian-language product requirements, including all previously agreed features and the clarified Melody/Pattern keyboard behavior, are maintained in [MASTER_PRODUCT_SPEC_4_0_RU.md](./MASTER_PRODUCT_SPEC_4_0_RU.md). Treat that document as the comprehensive user-facing scope; this file remains the concise production-priority and acceptance-gate summary.


## Canonical execution roadmap

The consolidated approved execution order, all user-approved additions, cross-module dependencies, current verified CI status and release gates are maintained in [`PPGU_MASTER_DEVELOPMENT_PLAN_RU.md`](./PPGU_MASTER_DEVELOPMENT_PLAN_RU.md). Use it together with the detailed product specification; it does not convert documented scope into implemented status.


## Latest approved preset scope expansion

The hard release minimum is at least 300 unique, fully implemented and audio-validated MELODY + PATTERN factory presets total. The 500–600 range remains a stretch goal. The 82 Travis Barker-produced-album guitar presets are distributed inside the corresponding artist/band categories and counted once in the overall catalogue; they are not a separate category. The legacy 300-entry catalogue remains to be audited; catalog records are not proof of playable, validated presets.

Add Angels & Airwaves album-era families for *We Don't Need to Whisper* (2006), *I-Empire* (2007), and *Lifeforms* (2021), plus Tom DeLonge technique presets in both MELODY and PATTERN. Research notes, confirmed facts vs interpretations, tone concepts, legal limits and tests are in [PPGU_ANGELS_AIRWAVES_TOM_DELONGE_PRESET_RESEARCH_RU.md](./PPGU_ANGELS_AIRWAVES_TOM_DELONGE_PRESET_RESEARCH_RU.md).

Research public interviews, gear/tone breakdowns and free preset examples. Include third-party preset files/IRs/samples only when their actual license permits commercial redistribution inside the plugin. Otherwise build an independent tone recipe. Never copy protected riffs, stems, proprietary presets or samples.



## Блокирующий quality gate: запрет пластикового / искусственного гитарного звука

**APPROVED / MANDATORY / RELEASE-BLOCKING.** Не допускать в PPGU 4.0 пластмассовые, искусственные, синтетически-негитарные, дешёвые или нереалистичные тембры. Звуковой уровень продукта должен соответствовать дорогому профессиональному виртуальному инструменту. Это требование относится ко всем гитарам, акустическим и электрическим, всем фабричным пресетам, предпрослушиванию и демонстрационным материалам.

Проверять на слух весь диапазон, velocity, атаку, сустейн и release, артикуляции, ноты/аккорды, переходы и выразительность. Источники могут быть DRY, NATIVE TONE MATCH (нативный сэмпл уже совпадает с целью), PROCESSED SOURCE MATCH (качественная обработанная запись уже совпадает) или WET (обработка в плагине). Не добавлять эффекты ради количества; избегать двойной обработки готовых источников и использовать нулевую/минимальную DSP-цепь, если она звучит убедительно и естественно. Но экономия CPU никогда не оправдывает ухудшение звука.

Любой пластиковый или нереалистичный результат = REJECTED / NOT READY и блокирует включение пресета в релиз до исправления и повторной слуховой проверки. Автотесты, графики и pluginval не подтверждают музыкальную реалистичность сами по себе.


## Жёсткий релизный минимум каталога: 300 READY-пресетов

Пользователь подтвердил: в фабричном релизе должно быть **не менее 300 уникальных полноценных MELODY + PATTERN пресетов суммарно**. Это обязательный нижний предел. Учитываются только работающие, легально обеспеченные реальными источниками, музыкально различимые и прошедшие функциональную и слуховую проверку пресеты; пустые записи, placeholders и простые переименования не засчитываются.

Распределять каталог по всем утверждённым группам/artist-inspired профилям, жанрам и поджанрам, акустическим и электрическим гитарам, а также музыкальным ролям и секциям песни: INTRO, VERSE, PRE-CHORUS, CHORUS, POST-CHORUS, BRIDGE, BREAKDOWN, RIFF, LEAD/HOOK, ARPEGGIO/PICKING, FILLS/TRANSITIONS, OUTRO — с учётом уместности для конкретного стиля. Разные секционные варианты считаются отдельными только при реальном музыкальном различии и самостоятельной проверке, а не за счёт переименования.

Цель расширения 500–600 сохраняется как stretch goal после обеспечения 300 качественных READY-пресетов. 82 пресета по альбомам с подтверждённым участием Travis Barker распределяются по соответствующим группам/артистам и входят в общий счёт один раз. Приоритет: качество и музыкальная полезность; никакое число не оправдывает пластиковый звук.


## Политика FX: обязательны, когда их требует целевой звук

Не считать минимальную цепь обработки универсальным правилом. Если для достоверного характера пресета нужны конкретные эффекты, они должны быть включены и настроены. FX также разрешены для дополнительного приближения уже качественной гитары к целевому звучанию. Полностью необработанный SOURCE MATCH допустим только тогда, когда исходник сам по себе уже соответствует пресету.

Сценарии: (1) исходник уже совпадает с целью — дополнительная обработка не нужна; (2) FX являются необходимой частью целевого тона — использовать их; (3) исходник уже хорош, но FX улучшают соответствие — использовать обоснованную обработку. Для обработанных сэмплов не дублировать эффекты вслепую; оценивать звук и добавлять FX только по результату. Не жертвовать требуемым тоном ради экономии CPU: оптимизировать реализацию после достижения качества, без слышимого ухудшения.


## Travis Barker 82 — распределённые альбомные guitar-пресеты, не отдельная категория артиста

Уточнение пользователя заменяет прежнее понимание «Travis Barker 82» как отдельной категории. 82 — это уникальные guitar presets, основанные на исследовании гитарных партий/звучания альбомов других групп и исполнителей, где Travis Barker имеет подтверждённый producer/co-producer credit. Они располагаются внутри соответствующих artist/band/style категорий Pop Punk и смежных жанров, рядом с другими пресетами этих исполнителей. Это не отдельный раздел пресетов личного гитарного звучания Travis Barker.

Покрывать роли гитар в песне: chorus lead/hooks, rhythm guitars, palm-muted verses, open chorus chords, pre-chorus builds, bridge/breakdown, arpeggios/picking и fills/transitions, где это соответствует конкретному альбому. Проверять producer credits и отдельно исследовать гитарную аранжировку; не приписывать Barker авторство каждого гитарного решения без доказательств. Внутренний тег для происхождения сохраняется. Все 82 входят в общий счёт каталога и каждый считается только один раз; они могут засчитываться в обязательный минимум 300, а не добавляются отдельной надбавкой. Не копировать защищённые риффы/записи/stems/presets.


## Global product-concept and UX optimization mandate — USER APPROVED

The user has delegated and approved independent optimization of the entire PPGU concept for end-user clarity, efficiency, speed, and logical flow. This covers visual design, navigation, terminology, screen structure, workflows, control behavior, preset/library organization, internal architecture, loading/performance, settings, diagnostics, documentation, and onboarding.

Design for immediate comprehension and a fast path to a playable guitar sound. Keep core musician tasks prominent; place specialist controls in clearly named Advanced/Details areas rather than removing them. Use consistent terms and control behavior, clear units/tooltips, visible Loading/Ready/Missing/Error states, actionable errors, readable contrast and hit targets, responsive layouts, and asynchronous non-blocking operations. Protect samples and user work with safe destructive-action handling. Simplify presentation without reducing sound quality, MIDI behavior, articulations, FX capability, or professional control.

Safe and reversible UX/code/documentation improvements may be implemented autonomously. Before changing audio/MIDI contracts, project-state compatibility, content formats, licensing, user data, or approved visual invariants, audit dependencies and provide compatibility/migration tests; never change these silently. Every improvement needs a clear user benefit, regression coverage, performance/audio verification where relevant, and a journal entry. NEON STAGE DEFAULT and the approved skin geometry/function invariants remain authoritative.
