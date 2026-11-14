# PPGU UI THEME SPEC — REV 18 (v2.0 — FINAL ASSEMBLED EDITION)

Статус: BINDING USER SPECIFICATION. Область действия: исключительно визуальный слой — темы, оболочка, окна, контролы, палитра, ассеты, resize и рендеринг. Не разрешает изменения Audio Engine, DSP, сэмплов, Sound Hunter, MIDI/APVTS или CI-контрактов.

## 1. Каноническая тема и линейка

- Главная тема: **NEON STAGE DEFAULT SKIN-1**, канонический ресурсный ID `skin-053` / `Skin_053`.
- Первый запуск, reset и безопасный fallback всегда выбирают NEON STAGE; она первая в галерее и не редизайнится в рамках миграции.
- SOUND FORGE и SIGNAL FORGE не являются именами отдельных тем. SIGNAL FORGE может оставаться инженерным названием стандарта.
- Целевая релизная линейка — ровно 16 активных тем: SKIN-1 NEON STAGE DEFAULT и SKIN-2…SKIN-16. Новые темы — вариации общей дизайн-системы NEON STAGE, только палитра и artwork различаются.
- Нумерация активных тем последовательная; существующие исторические ID нельзя удалять, переименовывать или перенумеровывать. Legacy-записи (включая Neon Riot, Dark Metal, Modern Studio, Carbon Relay, Blackout Voltage и прочие) проходят no-loss migration audit и не превращаются автоматически в активные темы.
- Отсутствующие элементы создаются автоматически на основе Info.txt и утверждённых макетов; если их нет — с нуля с меткой [DECISION]. Placeholder всегда маркируется. Лицензионная чистота, G1–G4, PNG CRC и неизменность аудио/APVTS обязательны.

## 2. Runtime-контракт и файлы

- Runtime-папка строго `Skins/` рядом с установленным .vst3; AppData не является основным источником.
- `Skins/theme_manifest.json` — registry тем (ID, имя, версия, порядок, active, asset paths).
- `Skins/control_schema.json` — 27 control-ID и ровно 21 palette token для Skin Studio 52+. Legacy `theme.schema.json` Skin_01..51 не взаимозаменяема с control_schema.
- `Skins/<skin-id>/theme.json` — палитра, STUDIO_CONSOLE layout, control mapping, asset references; APVTS/layout/audio flags = false.
- Полный набор темы: theme.json; controls sprites; hero; gallery preview; seamless textures; background; baked FX layers; SVG + PNG icons; license-clean fonts; texture atlas + frame metadata.
- Мастера и runtime exports хранятся отдельно. Runtime generation не перезаписывает masters.

## 3. Качество и регистрация ассетов

Мастер-размеры: каждый control/state sprite не менее 512×512; hero не менее 1600×900 (runtime 800×450); preview не менее 720×480 (runtime 360×240); seamless texture не менее 1024×1024; main background не менее 2560×1440. Иконки TOP BAR имеют SVG source и PNG exports требуемых плотностей.

Качество: commercial-grade, чистые края, резкость при 100%, без compression artifacts, blur, upscaling, watermark и чужих логотипов; палитра только из canonical tokens. Baked alpha layers предпочтительнее runtime shaders. Использовать texture atlases вместо повторяющихся копий. Каждый asset и effect layer получает registration record и CRC.

Проверки: `tools/validate_skin_pack.py` (G1–G4, включая PNG CRC/G3); `tools/merge_skin_pack.py` (backup + atomic write). Для каждого ассета фиксируются имя, master/runtime размеры, sprite states, layers, ресурсная стоимость и режим отображения.

## 4. Палитра и layout

- Ровно 21 canonical HEX token; hardcoded HEX запрещён в `PluginEditor.cpp` и `Source/UI/*`.
- Primary contrast к window background не менее 7:1; отдельно проверяются secondary, disabled, LCD и accent.
- Layout: STUDIO_CONSOLE, сетка 12×16, zero-based coordinates.
- Все окна/панели наследуют одну тему через `ThemeManager::applyPalette`; один источник света; состояния hover/active/disabled/success/warning/error используют только tokens.
- Тема не меняет APVTS, layout ownership, audio path или MIDI.

## 5. Main window — зоны A–H

A TOP BAR: SKINS, Import Guitar, Auto Discover, Curator, Settings, About. Все entry points предусмотрены; нереализованные — disabled и честно объяснены.
B BROWSER: preset selector, category filter, search, status.
C PLAYING CORE: MELODY/PATTERN, hero artwork, 9 rotary.
D FX CHAIN: ровно 7 toggles; переключение должно отражать реальный DSP routing.
E AUTOCHORDS: setup-only до backend; гармонические/ритмические selection независимы; 16 чипов: OPEN CHORDS, PALM MUTE, POWER CHORDS, DOWNSTROKE, ALTERNATE, CHUGS, SKANK, ARPEGGIO, STRAIGHT 8THS, FAST SKATE-PUNK, GALLOP, HALF-TIME, STOP-START, VERSE→CHORUS, OFFBEAT, OCTAVE HOOK. Display не перекрывает MIDI keyboard.
F FX EDIT: одно окно, 8 страниц, 7 bypass slots; 10–15 реальных DSP effects — целевой gap, не основание имитировать эффекты.
G AMP & TONE: amp model, Gain/Bass/Treble — только реальный signal path.
H PERFORMANCE: read-only MIDI monitor; BPM/bars/CPU/RAM/wheels отсутствуют.

## 6. Контролы — неизменяемый inventory

9 rotary/APVTS IDs:
- ampGain 0..10 default 5 (C)
- eqHigh, eqMid, eqLow −12..12 default 0 (G)
- velocity 0..2 default 1 (C)
- dynamics 0..1 default 0.5 (C)
- swing 0..1 default 0 (C)
- humanize 0..1 default 0 (C)
- accent 0..1 default 0.75 (C)

Ровно 7 bool bypass IDs: compressorOn, eqOn, distortionOn, chorusOn, delayOn, reverbOn, cabinetOn. Cabinet IR disabled без реальных IR assets и convolution path. flangerOn не добавляется в inventory и отдельный переключатель FLANGER запрещён.

Служебные элементы: preset selector, category filter, preset search, единственный status/LCD_display, MELODY/PATTERN (без PUNK GROUP), sync selector, LOAD SFZ = Import_Guitar, SKIN button (Ctrl+K), patternSlot 0..7, MIDI keyboard 36–84. Подсветка клавиатуры отражает только реальное MIDI state. Никакой фальшивой активности.

27 control-ID parity сохраняется между schema и кодом. Legacy/reserved IDs, если они есть в contract, не дают права рисовать несуществующие функции. Не показывать PREV/NEXT/FAVORITE/SAVE, PUNK GROUP, LEDs, pitch/mod wheels или CPU/RAM meters.

## 7. Окна и галерея

Пять auxiliary DocumentWindow (native title bar off, JUCE bar с X и maximize/restore): FX EDIT; AUTOCHORDS (disabled до backend); IMPORT GUITAR (async SFZ: LOADING→SUCCESS/FAILURE, без блокировки audio thread); SETTINGS (только 6 существующих APVTS controls, плюс display mode); PERFORMANCE (read-only MIDI monitor). ABOUT показывает только реальные product/build metadata и RU/EN manuals.

Tiled geometry: 1=single, 2=2×1, 3–4=2×2, 5–6=2×3; при нехватке места — focus paging, никогда overlap. Persistence через PropertiesFile с clamp к work area. X не уничтожает состояние. Visual hot-swap 150 ms; reduceInEco=true.

Галерея: cards/search/family/count/active marker. Порядок внедрения: selection state → Preview/Apply/Back → enlarged preview/favorites/keyboard navigation/persistence tests. Текущая реализация немедленного применения при клике не должна выдаваться за полный workflow.

## 8. Resize и масштабирование

Правый край меняет ширину; нижний — высоту; нижний правый угол — обе оси. Live resize без лагов. Содержимое плавно масштабируется вместе с окном, без clipping, overlap, MIDI occlusion и мыльного runtime upscaling. Использовать masters/appropriate density assets. Layout 12×16 + tiled/focus paging. Минимальный размер сохраняет читаемость; геометрия сохраняется и clamp-ится к рабочей области. Drag-resize перерисовывает только dirty regions.

HiDPI 100–400%, 4K/8K и multi-monitor поддерживаются по реальной необходимости с минимальными затратами.

## 9. Display modes и resource budget

**МИНИМАЛИЗМ — default** при первом запуске/reset/fallback: чистый стандартный вид без дополнительных эффектов/анимаций, минимальное CPU/RAM/GPU/video-core use. **ЭФФЕКТЫ** — полный premium rendering, только по явному выбору пользователя в SETTINGS или SKIN menu; выбор сохраняется и самопроизвольно не меняется. Это два режима одной темы, не отдельные темы; palette/layout/control inventory идентичны.

Render only on changes; cache static layers; lazy load assets; bounded LRU/atlas; no busy-wait, redundant timers or leaks; pause repaint while minimized; audio thread always has priority. Animations 30–60 Hz only when needed; ECO simplifies effects. Target stable 60 FPS, profile before claiming.

## 10. Запрет имитации и safety anchors

Не имитировать PUNK GROUP, Auto Discover/SOUND HUNTER (0 approved assets), Curator, About metadata, LED, wheels, CPU/RAM meters, AutoChords MIDI generation/playback, OverlayManager, SecureSkinImport, transport/BPM snapshot, eighth FX switch или performance data без realtime-safe snapshot. Не заполнять незавершённые панели фиктивной активностью.

НИКОГДА: менять APVTS IDs/диапазоны/дефолты; имитировать отсутствующие функции; завышать статусы; удалять/перенумеровывать skin IDs; обходить G1–G4; hardcode HEX в UI; выдавать placeholder за готовый asset; менять audio path на UI этапе; создавать отдельные SOUND FORGE/SIGNAL FORGE темы; превращать legacy names в новые active themes; нарушать последовательность SKIN numbering; редизайнить NEON STAGE DEFAULT; включать ЭФФЕКТЫ по умолчанию; нарушать resize или растягивать/обрезать контент; использовать ресурсы без необходимости.

## 11. Definition of Done / выдача

1. Полные theme.json / compatible manifest files, без ellipses.
2. Таблица всех 27 control-ID: element → APVTS ID → coordinates/token.
3. Asset inventory: master/runtime sizes, sprite states, effect layers, resource cost, display mode.
4. Missing assets created, либо честно отмечены placeholder с regeneration plan.
5. Contrast report: primary/secondary/disabled/LCD/accent; primary ≥7:1; G1–G4 и PNG CRC.
6. Exact-SHA CTest / pluginval strictness 5 / FL Studio 20 acceptance gates; явно разделять CI и пользовательские host checks.
7. Итог с честными статусами и точным следующим шагом.

## 12. Порядок реализации

CHECK → ANALYZE → IMPLEMENT/FIX → RETEST → REPORT. Перед source changes сверить Info.txt, branch HEAD, current mockups, schema, manifest, theme files and current UI code. Сначала no-loss registry/asset migration plan; затем один изолированный visual-only change. Никаких APVTS/audio/DSP/MIDI modifications в theme phase.

[DECISION] Любые созданные с нуля элементы должны быть явно отмечены в журнале. Этот документ фиксирует пользовательскую спецификацию v2.0 и сам по себе не утверждает, что перечисленные требования уже реализованы.
