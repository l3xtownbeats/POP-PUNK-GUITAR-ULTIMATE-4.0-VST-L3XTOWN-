# POP PUNK GUITAR ULTIMATE — GUI / UX blueprint
**Target:** JUCE 7 · VST3 · Windows 10 x64 · FL Studio 20  
**Design direction:** premium contemporary music-software graphics with a confident pop-punk identity. Combine the energy, attitude and color language associated with Blink-182 and Sum 41 with the finish of a high-end 2026 instrument: deep graphite/black materials, precision-machined metal details, rich layered surfaces, controlled neon lighting, cinematic contrast, premium typography, crisp vector iconography and original photorealistic guitar/studio artwork. Pop-punk references should appear as tasteful design cues (checkerboard micro-details, gig-poster composition, sticker-like labels), never as a cheap distressed collage or a retro imitation. Do not copy band logos, album covers, photographs, trademarks or protected artwork.  
**Status:** implementation specification and theme-token foundation; this document does not claim that every described panel is already implemented.

## 0. Reference-product principles

Use the usability and visual hierarchy of **reFX Nexus / FLEX** and the depth, premium finish and immersive instrument presentation of **Spectrasonics Omnisphere** as product-quality references—not as templates to copy. The plugin must retain its own pop-punk guitar identity.

- **Nexus / FLEX principle:** fast access to sounds, clear preset browsing, immediate feedback, compact controls, low-friction workflow and no unnecessary navigation.
- **Omnisphere principle:** premium visual depth, strong hero artwork, coherent material design, carefully layered panels and a sense of a complete flagship instrument.
- **PPGU principle:** every visual layer must support the guitar workflow. The hero/background art is atmospheric and secondary; controls, preset names, articulation state, pattern information and performance feedback always remain readable and visually dominant.
- Keep the main performance controls in a stable, predictable grid. Decorative artwork must never sit behind dense labels or rotary value readouts. Use opaque or sufficiently solid control cards over detailed artwork.
- Provide a restrained **Performance / Focus view** where the hero artwork recedes and the active sound controls become more prominent. The normal view balances brand identity and function.
- Preset selection should be one or two actions at most; Melody and Pattern remain immediately accessible. Do not hide essential parameters inside decorative tabs or deep menus.
- Use consistent spacing, alignment, contrast and component sizing. The UI should feel designed as one instrument, not a collage of unrelated panels.
- Do not copy Nexus, FLEX or Omnisphere layouts, assets, logos, icons or proprietary graphics. Borrow only broad interaction and presentation principles.

## 1. Visual language

The default skin is **Neon Riot**: near-black graphite canvas, layered charcoal/black panels, subtle brushed-metal and anodized-aluminium cues, electric magenta primary accent, cyan secondary accent and carefully rationed acid-yellow highlights. Use premium gradients, precise 1 px borders, soft ambient shadows, controlled edge lighting, subtle material texture and original high-resolution guitar/studio artwork. The result should feel like a flagship virtual instrument from a leading audio company: modern, expensive, tactile and performance-focused. Keep pop-punk character in the attitude, contrast, checkerboard micro-details and energetic typography—not in visual clutter. Keep controls highly legible; decoration stays behind controls and never competes with parameter labels. Use consistent 8 px spacing, 10–14 px card radii, clear section headings, compact labels, readable numeric values and large hit targets. Use a contemporary condensed display face for headings, clean UI typography for controls, restrained highlight effects and carefully art-directed negative space. Avoid excessive glow, tiny text and distressed textures that make the interface look dated.

The top-level layout is responsive and resizable. At normal desktop size target 1280×860; minimum usable size 1040×760. Below the wide breakpoint, panels reflow into two columns and the right-side browser becomes a collapsible drawer. No control may be clipped at the minimum size. Tooltips explain every non-obvious control. Parameter values remain visible while dragging and are editable by double-click where appropriate.

## 2. Main window hierarchy

### A. Header / global strip
- Product mark and version/build status.
- Unified preset browser entry point: search field, category filter, favourites, previous/next, preset name and dirty-state indicator.
- Preset categories: **All**, **Melody**, **Pattern**, **Artist / Style DNA**, **Acoustic**, **Clean**, **Crunch**, **Drive**, **Muted**. Artist/style reference presets remain in the same unified browser and are not hidden by the active performance mode.
- **PREVIEW** audition control; **LOAD SFZ** library import; sample/library status and missing-library warning.
- One **THEMES** button opens the theme gallery with 20 thumbnail cards, live preview, Apply and Restore Default. Theme choice persists. Changing a skin must not reset the preset, APVTS values, voices, transport or playback.

### B. Performance mode deck
Prominent mode switch: **MELODY / PATTERN**. Keep both modes one click away and retain the selected preset when switching.

**MELODY panel**
- Articulation selector: Open, Palm Mute, Flageolet / Harmonics, Tremolo, Clean, Crunch, Distortion, Acoustic, Slide, Legato.
- Pick / Finger selection.
- Mono / Poly mode, legato, hammer-on / pull-off, slide modifier, slide time/sensitivity, vibrato and mod-wheel vibrato.
- Melody pattern slot and ending controls (short / medium / long, palm hit, string slap).
- On-screen MIDI keyboard with active-note highlighting and octave labels.
- Velocity response, dynamics and palm-mute threshold. Velocity mapping must preserve musical dynamics while allowing a configurable low-velocity palm-mute articulation; show the threshold clearly.

**PATTERN panel**
- Eight selectable pattern slots with name, length, groove/style and active indicator.
- Search/filter pattern library; preview before loading; drag MIDI pattern to host where supported.
- Pattern length (1/2/4 bars), host sync / host-start / internal sync, quantize, swing base, BPM fallback.
- Swing, Humanize, Accent, Start on First Chord, Preserve Human Timing.
- AutoChords controls: root, scale, slash-bass interpretation and single MIDI note as chord root.
- Chord/held-note display, detected chord label, generated voicing/fingering preview, transport state and pattern position.
- Stop/reset/panic controls; all state changes must be safe at transport boundaries.

### C. Sound design row
Three coordinated cards, with the selected card expandable into a detailed editor:

**AMP & TONE**
- Amp model / character selector, Gain, Tone, Bass, Mid, Treble, Presence, Output.
- Guitar tuning, pan, pick/finger, acoustic/electric mode.
- Voices / polyphony (1–64), master output, velocity response and dynamics.
- Double Track On/Off and Mix must be explicit and visible.

**FX CHAIN**
- Seven slots: Compressor, EQ, Distortion, Chorus, Delay, Reverb, Cabinet IR.
- Each slot has bypass, active state, reorder handle and a detail view. Drag reorder changes the displayed processing order without changing stable parameter IDs.
- Compressor: threshold, ratio, attack, release, amount.
- EQ: low/mid/high gain, low/mid/high frequency, mid Q.
- Distortion: drive, tone, mix.
- Chorus: rate, depth, mix.
- Flanger: on/off, rate, depth, feedback, mix (available as a dedicated effect module alongside the seven-slot chain).
- Delay: time, feedback, mix.
- Reverb: room size, damping, mix.
- Cabinet IR: load/select IR, low cut, high cut, mix, bypass.
- Provide factory chain quick-loads: Tight Pop Punk, Aggressive Skate Punk, Wide Modern Pop Punk. These are starting points, not locked processing modes.

**PLAYBACK / PERFORMANCE**
- Swing, Humanize, Accent, Start Mode, Sync Mode, BPM fallback, quantize.
- MIDI input/activity indicator, panic, active voices meter, CPU/RAM readouts (only when measured; never display fabricated values).
- MIDI learn / automation affordances for exposed parameters.

### D. Bottom status rail
- Library name and indexed sample/region count; loading progress and actionable error text.
- Active mode, preset name, detected chord / pattern slot, MIDI activity.
- Voice count / polyphony cap, output meter, CPU and memory monitor when profiling data is available.
- Help / manual and panic all-notes-off.

## 3. Secondary windows / drawers

All secondary windows are modeless, resizable and visually themed. They must not block audio processing.

1. **Preset Browser:** unified Melody + Pattern + artist/style reference search, tags, favourites, recent presets, preview, load, import/export. Filtering must never silently load another preset.
2. **Sample Browser:** folder/library tree, SFZ/WAV metadata, key/velocity coverage map, articulation labels, missing-file diagnostics, audition/preview, load and rescan. Disk scans and decoding happen off the audio thread.
3. **Pattern Editor:** timeline/grid, bars/beats, note/chord events, strum direction, velocity/accent, timing offsets, humanize, loop/length and MIDI export/drag.
4. **FX Detail:** focused editor for selected effect, parameter values, bypass and chain order.
5. **Theme Gallery:** 20 distinct complete skins from `Themes/theme_manifest.json`, thumbnail preview, current/selected state, Apply, Cancel and Default.
6. **Settings / MIDI:** audio-independent UI preferences, MIDI input/channel display, MIDI learn, keyboard range, tooltip toggle, UI scale and diagnostics.
7. **About / Help:** version, build ID, manual, credits and license notices.

## 4. Layout map

At 1280×860:
- Header: 64 px.
- Performance mode deck: 190–220 px.
- Sound design row: 300–330 px; AMP & TONE 30%, FX CHAIN 43%, PLAYBACK 27%.
- Bottom keyboard / performance strip: 76–92 px.
- Bottom status rail: 28 px.
- 12–16 px gutters; cards align to a shared grid.

At narrower widths:
- Header controls wrap into two rows.
- Sound cards become a 2-column grid; Playback spans full width below.
- Keyboard stays full width at the bottom.
- Preset browser and detailed editors open as drawers/dialogs rather than squeezing main controls.
- At every size, scrolling is contained within the relevant card; the main window itself should not hide critical controls.

## 5. Theme system contract

Theme manifest is data-only and contains color tokens. Runtime UI components consume tokens through a single ThemeManager / LookAndFeel layer. A theme changes the complete shell: canvas, card surfaces, borders, typography colors, rotary controls, sliders, buttons, tabs, combo boxes, keyboard, meters, icons, thumbnails and window chrome. Themes must not alter parameter ranges, parameter IDs, layout semantics, preset content or audio routing.

The initial 20 theme identities are:
1. Neon Riot (default pop-punk poster skin)
2. Dark Metal
3. Modern Studio
4. Punk Rock
5. Electric Guitar
6. Dark Carbon
7. Neon Cyber
8. Vintage Amplifier
9. Minimal Black
10. Glass UI
11. Sunset Drive
12. Arctic Blue
13. Toxic Green
14. Purple Voltage
15. Blood Red
16. Ocean Stage
17. Golden Era
18. Mono Chrome
19. Festival Lights
20. Studio Olive

Theme loading must validate tokens and fall back to Neon Riot on malformed or missing data. Cache decoded visual assets outside the audio callback. Theme switching runs on the message thread, is atomic from the user's perspective, and must never stop notes, clear effects, reload samples or modify APVTS state. Persist the theme identifier in UI/plugin state with a safe default.

## 6. Engineering rules

- Every visible control that represents a DSP/performance parameter must attach to the existing APVTS parameter ID; do not create duplicate controls with disconnected local state.
- Stable parameter IDs are compatibility contracts. Do not rename or repurpose them to make the UI easier.
- The current parameter inventory includes master, voices, velocity, dynamics, palmMuteThreshold, tuning, pan, output, playMode, articulation, pickStyle, vibrato, slideTime, legato, hammerPull, scaleRoot, scaleMode, monoMode, fretNoiseOnChordChange, slideModifier, slideVelocitySensitivity, modWheelVibrato, melodyPatternSlot, endingLength, palmHitOnEnding, stringSlapOnEnding, patternSlot, patternLength, syncMode, swingBase, globalPpqOffset, chordSlashBass, singleMidiNoteAsChordRoot, preserveHumanTiming, DNA controls, seven fxSlot selectors, compressor/EQ/distortion/chorus/flanger/delay/reverb/cabinet controls, doubleTrackOn/Mix, BPM, swing, humanize, accent, startOnFirstChord and patternQuantize.
- A control may be shown as disabled only when its feature is genuinely unavailable in the current build; label it honestly and expose a tooltip/status reason. Do not fake functional behavior.
- No disk access, allocation, mutex waits, sample loading, preset parsing or theme file decoding in processBlock.
- UI timers poll lightweight atomics/diagnostics at a modest rate; never poll large sample data.
- Accessibility: keyboard focus, meaningful labels, contrast, tooltip text, predictable tab order and scalable UI.
- Performance: reuse components, cache theme assets, avoid per-frame image decoding and unnecessary repaint loops.

## 7. Acceptance checklist

- [ ] All agreed functional blocks are present and reachable.
- [ ] Unified preset browser searches both Melody and Pattern without hiding artist/style presets.
- [ ] Preset preview is separate from loading; filtering does not change the current sound.
- [ ] All controls map to valid APVTS IDs or are explicitly identified as non-parameter UI actions.
- [ ] Theme gallery displays 20 distinct previews and each skin changes the complete interface.
- [ ] Theme persists across host save/reopen and has a fallback if a theme asset is missing.
- [ ] Switching themes preserves preset, parameters, active voices and playback.
- [ ] Main window remains usable at minimum and maximum sizes; no clipping/overlap.
- [ ] Secondary windows are modeless and do not block the audio thread.
- [ ] Windows Release VST3 builds; CTest and pluginval pass.
- [ ] Manual FL Studio 20 checks cover resize, state recall, MIDI, automation, keyboard, dialogs and theme changes during playback.

## 8. Premium contemporary pop-punk art direction / legal boundaries

The design must look current and expensive, not like a deliberately low-resolution retro flyer. Use premium 3D/photorealistic original electric-guitar hero artwork, refined studio/rack materials, precision control detailing, modern typography, cinematic lighting and restrained pop-punk accents. References to Blink-182 and Sum 41 should communicate attitude, melodic energy and early-2000s skate-punk heritage through broad visual language only: selective checkerboard details, energetic diagonals, high-contrast accent colors and gig-poster-inspired hierarchy. Keep these cues subtle and integrated into a polished contemporary instrument UI. Use original vector shapes, original textures and commissioned/original artwork only. Do not reproduce band logos, album art, signature illustrations, photographs, exact merchandise graphics or imply endorsement. Preset names may describe broad musical traits (e.g. melodic skate-punk, tight palm-muted chorus, octave lead) without claiming official affiliation.

**Asset quality gate:** hero artwork and theme artwork must be high-resolution, correctly cropped at multiple aspect ratios, color-managed, compressed for fast loading, and accompanied by a lightweight vector/fallback state. No stretched low-resolution images, random stock photos, fake 3D bevels, noisy glow or mismatched assets. Load/decode artwork off the audio thread and cache it. Theme switching must not reload samples or interrupt playback.

## 9. Current repository baseline / honesty note

The current `Source/PluginEditor.cpp` is an early dark/magenta editor with a preset combo/search, SFZ load, Melody/Pattern selector, nine rotary controls, seven FX bypass buttons and a keyboard. It does **not** yet implement the complete layout, all APVTS controls, pattern editor, sample browser, preview workflow or runtime theme gallery described above. The manifest in `Themes/theme_manifest.json` establishes the 20 palette tokens; it is not itself a finished runtime theme manager. Do not mark the GUI milestone accepted until the implementation and checks above pass.

## 10. Главный UX-принцип: интуитивность прежде визуальных эффектов

PPGU должен быть понятен музыканту без чтения руководства. Визуальная простота и скорость взаимодействия Nexus/FLEX — главный ориентир; гитарная специализация и выразительная подача Electric Sunburst Deluxe — второй ориентир. Ни один декоративный элемент не имеет приоритета над ясностью управления.

### Правила интерфейса
- **Один главный маршрут:** выбрать пресет → выбрать Melody или Pattern → играть. Базовый сценарий доступен с первого экрана без открытия дополнительных окон.
- **Ясные названия:** использовать понятные подписи (например, «Звук», «Стиль», «Артикуляция», «Эффекты», «Голоса»). Не полагаться только на загадочные пиктограммы или аббревиатуры.
- **Группировка по задаче:** все параметры исполнения находятся рядом с выбранным режимом; AMP & TONE управляет тембром; FX — обработкой; глобальные настройки — отдельно.
- **Постоянная навигация:** Preset Browser, Melody/Pattern, Preview, Skin и основные статусы находятся в предсказуемых местах во всех темах.
- **Не перегружать главный экран:** расширенные параметры открываются в понятных дополнительных панелях/окнах; базовые регуляторы остаются на виду.
- **Без скрытых состояний:** выбранный режим, активная артикуляция, включённые эффекты, синхронизация и текущий пресет всегда визуально обозначены.
- **Безопасное прослушивание:** Preview прослушивает звук, не заменяя текущий пресет; Load применяет пресет. Эти действия должны быть визуально и функционально различимы.
- **Подсказки по делу:** у каждого нестандартного регулятора есть короткий tooltip с объяснением эффекта и, где полезно, диапазона. Название и текущее значение доступны без наведения.
- **Предсказуемые контролы:** одинаковые типы переключателей, списков, ручек и bypass ведут себя одинаково во всём плагине. Цвет не является единственным признаком состояния.
- **Обратная связь:** действия подтверждаются ненавязчиво; ошибки загрузки показывают причину и следующий шаг, а не технический код без объяснения.
- **Быстрый возврат:** предусмотрены Reset для отдельных секций, Undo там, где операция разрушительная, и понятный Panic/Stop для остановки звучания.
- **Единая логика тем:** смена SKIN меняет оформление, но не расположение, названия, поведение и значения контролов.

### Проверки понятности перед релизом
- [ ] Новый пользователь может найти и загрузить пресет без подсказки и не более чем за 2 очевидных действия.
- [ ] Можно понять, какой режим активен, что сейчас звучит и какие эффекты включены, одним взглядом.
- [ ] Preview и Load невозможно перепутать по подписи или поведению.
- [ ] Основной сценарий «выбрать звук и сыграть ноту» не требует открытия вторичных окон.
- [ ] Ни одна тема не ухудшает читаемость, контраст, навигацию или доступность контролов.
- [ ] Все пиктограммы имеют подписи или tooltip; все важные состояния различимы не только цветом.
- [ ] Studio и Focus сохраняют одинаковую логику и порядок основных действий.
