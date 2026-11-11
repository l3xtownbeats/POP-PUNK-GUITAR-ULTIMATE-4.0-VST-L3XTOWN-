# PPGU 5.0 — implementation log (2026-11-11)

## Change delivered in this revision

The unified preset browser search now indexes the descriptive metadata already present in the 300-entry factory catalog, not only the visible preset name and a subset of fields.

The editor's preset model retains:
- reference artist;
- reference track titles;
- tonal target / sound description;
- sample profile;
- catalog status;
- optional tags.

The search query now checks these fields in addition to preset name, category, sound character, style profile, groove and articulation. This makes artist- and track-inspired presets discoverable by the reference material and sound description already authored in the catalog.

Files changed:
- `Source/PluginEditor.h`
- `Source/PluginEditor.cpp`

## Validation status

- Source change committed to branch `5.0`.
- Windows x64 workflow must compile and test this exact revision before the change is considered build-verified.
- The previous successful run (before this change) is not evidence for this revision.
- No claim is made that the 300 catalog entries are all fully playable: the catalog itself labels its sample/performance integration as pending.
- No claim is made that Pattern mode, MIDI Piano Roll, or multicore processing is complete. Existing source audit records those as blockers / disabled controls.
- FL Studio 20 manual validation remains a separate release gate.

## Next engineering gates

1. Make the browser's SOUND and STYLE selectors orthogonal and non-destructive, while keeping one preset identity across MELODY and PATTERN.
2. Connect preset selection to real sample-set resolution and FX snapshots; do not present catalog metadata as loaded audio.
3. Complete the PatternScheduler-to-sampler event path and verify host transport behavior.
4. Integrate page-cache streaming into actual voice rendering before relaxing decoded-memory caps.
5. Replace palette-only skin switching with complete, resource-backed skin packs while preserving the approved dark/charcoal and magenta visual direction.
6. Re-enable MIDI and MULTICORE controls only after their respective functions are implemented and tested.

## Follow-up browser revision

The unified browser's STYLE filter now presents playing techniques instead of a long list of individual groove labels:
- Lead Guitar
- Solo Guitar
- Palm Mute
- Open Chords
- Rhythm Guitar
- Arpeggio
- Ska Upstrokes

The filter evaluates the preset's name, groove, articulation, tonal target and style profile. MELODY remains the separate sound-character selector, and PUNK GROUP remains independent of the selected performance mode.

The sample-profile search field also falls back to the catalog's actual `sampleRequirement` property when `sampleProfile` is absent.

Latest source commits:
- `e02298c` — sample requirement metadata fallback.
- `56a7eb6` — playing-style taxonomy filters.

The earlier CI run for `e02298c` was cancelled before compilation. The current branch revision requires a fresh Windows x64 build and tests; no build pass is claimed yet.


## 2026-11-11 — уточнение mode/browser behavior и FX EDIT navigation

Документационная синхронизация с актуальными решениями пользователя:

- При выборе режима исполнения MELODY AUTOCHORDS автоматически bypass-ится; функция не строит аккорды и не изменяет входящие ноты в MELODY. Ручное OFF отключает автодостраивание в PATTERN.
- В режиме исполнения MELODY доступны пресеты из всех разделов PUNK GROUPS, MELODY, STYLE и USER. Загрузка пресета не должна сама включать PATTERN.
- FX EDIT закрывается красной кнопкой X в правом верхнем углу; небольшая вспомогательная надпись BACK допустима внизу сбоку и, если кликабельна, возвращает на главный экран. Надпись MAIN не нужна.
- Полная спецификация находится в `docs/PRESETS_BROWSER_AND_MODE_UI_5_0_RU.md`.
- Документационные commits: `e78cd252330abd32206315ffb1cf5dab6ef0d003`, `b21205a93149205c2fa85e8f1cf9ae3a641201d8`.
- Эти коммиты изменяют Markdown-документы; исходный код плагина ими не менялся. Build/CTest/pluginval/FL Studio 20 validation для новых требований не подтверждены.


## 2026-11-11 — PATTERN runtime independent from AutoChords

- Clarified that PATTERN is an independent performance engine and must remain active with AUTOCHORDS OFF.
- AUTOCHORDS OFF disables only automatic chord construction; the Pattern engine continues to rhythmically play the incoming MIDI note or user-played chord without adding notes.
- AUTOCHORDS ON may harmonize a single incoming key using the selected diatonic key and MAJOR/MINOR map before sending the chord through the same Pattern engine.
- Specification commit: `94cf692c5879d8555e89e467f54ac72d669c2e8f`.
- Status: SPECIFIED / IMPLEMENTATION AND TESTS PENDING. Do not mark runtime behavior complete before Processor tests, Windows CI and FL Studio 20 host validation.


## 2026-11-11 — Strict Pattern stop at Piano Roll MIDI note end

- Added a strict note-duration requirement: the last MIDI note-off must stop further Pattern events; queued future hits are cancelled and active voices are released/killed according to the strict release policy.
- Legato continuation is permitted only while another controlling MIDI note remains held/overlaps. Transport stop, seek, cycle/loop jumps and mode changes must flush stale scheduled events.
- Applies equally to PATTERN with AUTOCHORDS ON and OFF. Hidden latch-tail is not allowed by default.
- Specification commit: `360b852079ac0d84ba23a804c0ce176910d13000`.
- Status: SPECIFICATION ONLY. Scheduler/DSP implementation, regression tests, Windows build and FL Studio 20 validation remain pending.


## 2026-11-11 — Live note and chord information display

- Specified a persistent main-screen LIVE NOTE / CHORD display for both MELODY and PATTERN.
- It shows the active single note (including chromatic pitch classes) or a confidently recognized chord symbol; ambiguous note sets remain displayed as note names rather than guessed chord labels.
- PATTERN reflects the effective harmony after AUTOCHORDS processing: generated chord when ON, actual incoming notes/chord when OFF. MELODY reflects actual held notes without auto-harmonization.
- Audio-to-UI transfer must use a preallocated/lock-free snapshot; chord naming and string formatting stay off the audio thread.
- Specification commit: `664b3f6b4131e70ddacf618e3d7041816913ec6d`.
- Status: SPECIFICATION ONLY; source implementation, tests, CI and FL Studio 20 validation pending.
