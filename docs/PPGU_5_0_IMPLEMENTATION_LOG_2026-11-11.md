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
