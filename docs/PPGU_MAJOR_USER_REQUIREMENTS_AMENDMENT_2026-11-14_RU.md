# MAJOR USER-APPROVED PRODUCT REQUIREMENTS AMENDMENT — 2026-11-14
## PPGU — mandatory requirements; append-only; documentation baseline

**Authority:** explicit user approval in the conversation dated 2026-11-14. This amendment supersedes earlier conflicting product decisions only in the domains listed below. All unrelated master invariants, safety rules, compatibility rules, realtime rules, stable identifiers, and release gates remain in force.

**Status vocabulary:** APPROVED / REQUIRED / DESIGNED / SOURCE PRESENT / INTEGRATED / TESTED / HOST VERIFIED / RELEASED. This document records approved requirements; it does not claim that the corresponding production implementation exists.

## 1. FX EDIT and DSP — expanded mandatory scope

- FX EDIT is not limited to the seven effects shown in the earlier compact inventory.
- The complete FX EDIT must support **up to 15 fully implemented effect/processors**, plus a separate cabinet section (cabinet models and/or cabinet/IR handling as specified by the approved FX design).
- The DSP product requirement is **at least 10 processors/effects, with a target scope of 15**. The 15 FX EDIT slots are the upper design scope; cabinets are additional and are not silently counted as a substitute for the minimum DSP processors.
- Each module must have real DSP/backend wiring, parameter mapping, truthful enable/bypass state, preset/state persistence, automation where supported, safe parameter updates, and tests. No decorative or fake controls.
- Preserve the approved FX EDIT mockup: do not redesign its composition or module placement as part of backend implementation.
- Existing APVTS IDs and semantics remain immutable. New parameters/IDs or routing changes require scoped compatibility review and migration/regression tests; do not rename or repurpose existing IDs.
- The exact 15-module inventory must be reconciled against the user's earlier FX specification and repository design records before implementation. Do not invent module names to fill slots.

## 2. AUTO DISCOVER — full implementation required

- AUTO DISCOVER is a mandatory complete production feature, not a placeholder.
- Preserve the approved UI contract: explicit ON/OFF control inside the window; OFF means no background or persistent discovery; ON permits discovery but does not start an implicit scan; a separate SCAN/SEARCH action starts a bounded, cancellable task; the user can stop it.
- No scheduler, automatic repeat, or silent background scanning without separate explicit approval.
- Run discovery only outside the audio callback and through the existing approved discovery/import pipeline (no second competing pipeline).
- Provide real progress, results, errors, cancellation, deduplication, provenance/checksum and clear actions. Never fabricate activity or results.
- Discovery must not prevent the user from manually importing or using a library.

## 3. SKIN SYSTEM — NEON STAGE DEFAULT is the primary default

- The primary/default theme display name is **NEON STAGE DEFAULT**. This is the user's explicit MAJOR change to the former I7 default-theme naming rule.
- The first user-supplied NEON STAGE mockup is the authoritative reference for exact layout, proportions, component placement and visual appearance. The second supplied mockup is the authoritative colour-palette reference.
- Do not substitute a fresh design, simplify the approved layout, move controls, change functions, change bindings, or alter sound.
- Additional themes are derived from the NEON STAGE DEFAULT master layout. Skins may change graphic presentation, textures, colours, decorative assets and approved visual states only. They must not change control inventory, coordinates/layout geometry, functions, APVTS bindings, MIDI behaviour, DSP, sampler behaviour or sound.
- Convert the approved mockups into real, correctly sliced/exported PNG asset sets (and other explicitly approved asset formats where required). Each asset must map to a real component/state/scale use; include manifests, dimensions, checksums, density/scale variants as needed, and validation. Do not treat one flattened mockup image as a functioning interface.
- Default theme must load automatically on first launch and when no valid saved theme exists. A user's valid saved theme may persist according to the existing state/persistence contract.
- Retain stable IDs and migration safety. The canonical default display name changes to NEON STAGE DEFAULT; retain the existing canonical default ID `default-ppgu` unless a separately reviewed migration proves a safe additive ID transition. Do not silently break stored projects or skin references.
- Update the previous I7/R18 default-name contract through this explicit MAJOR amendment; all unrelated I1–I8/R01–R34 invariants remain unchanged.
- Full runtime integration, gallery application, persistence, visual parity, zero-residue and all-panel tests remain required; design assets alone are not implementation.

## 4. Factory preset catalog — open-ended, extension-only

- The factory catalog is **300 or more**; 300 is a minimum baseline, not a maximum or fixed target.
- Add as many factory presets as can be supported by verified source material, valid performance maps and quality control. There is no arbitrary 300-preset ceiling.
- Every factory preset must correspond to its intended PUNK GROUP / sound-group definition and actually produce the intended tone and playing behaviour. The existing user-supplied PUNK GROUP list is the canonical source for group/style mapping; do not replace it with an invented list.
- Each preset must identify its actual guitar source, tone/DSP configuration, articulation/performance map, playable range/velocity coverage and required assets. Missing source mappings must be marked unavailable/design-only, not called playable.
- Existing public internet research and ready-made development work may be reused only where the exact rights permit the intended use. Do not copy proprietary samples, closed commercial presets, protected riffs/transcriptions, or other restricted assets. Keep source/provenance, license, checksum and attribution records.
- Factory catalog remains append-only: stable IDs, factory read-only, no silent overwrite/deletion/substitution. User presets/banks remain separate and user data survives updates.

## 5. Preset Browser — first category and navigation

- The first top-level category is **Pop Punk Essentials**.
- Preserve the approved contextual sequence: **GENRE → ARTIST/BAND → SONG/SECTION → STYLE → PATTERN**, with section choices INTRO, VERSE, PRE-CHORUS, CHORUS, BRIDGE and OUTRO.
- STYLE and PATTERN remain separate browser entities/windows as already approved; the hierarchy must not flatten their different meanings. STYLE selects how the guitar is performed; PATTERN selects the rhythmic figure.
- Browser controls and behaviour remain mandatory: BROWSER, FAVORITES, CATEGORIES, SEARCH, FILTER, SORT, PREV/NEXT, FAVORITE, PRESET NAME, CATEGORY, PREVIEW and SELECT/APPLY/CANCEL as applicable.
- PREVIEW must not modify the active preset, project, persistent DAW state or saved Pattern Slot. PREV/NEXT must remain inside the current search/filter/sort result context.
- The ARTIST/BAND visible-name restriction in the pre-existing master rules is not silently waived by this amendment. Preserve the hierarchy slot, but do not display restricted artist/band/product names until that separate restriction is explicitly amended. PUNK GROUP mapping may still be used internally for sound matching and QA.

## 6. SOUND HUNTER / commercial libraries — user-use non-blocking rule

- SOUND HUNTER must **not prohibit or restrict the user's ability to download, import, load or locally use commercial sample libraries** that the user has lawfully obtained or is otherwise entitled to use.
- Licensing labels/checks are for provenance, factory-content clearance and redistribution/package decisions; they are not a local-use lockout.
- GREEN/YELLOW/RED/GOLD may describe whether PPGU may redistribute/include a library in its own installer. They must not silently disable, delete, quarantine, hide or block the user's locally selected library solely because it is commercial or not redistributable by PPGU.
- The plugin must distinguish: (a) user-local use, (b) inclusion in a PPGU factory bank, and (c) redistribution in a release package. Only (b)/(c) require rights covering that distribution.
- Preserve safe path validation, checksum/provenance metadata, corruption handling, explicit warnings and user control. Technical capacity limits are engineering constraints, never disguised licensing restrictions.

## 7. Implementation order and acceptance

1. Reconcile this amendment with the exact FX/DSP list, PUNK GROUP source list, approved mockup files and current source/control schemas.
2. Update canonical requirements and journals without rewriting history.
3. Implement in scoped changes: NEON STAGE asset slicing/theme integration; AUTO DISCOVER; FX/DSP modules; preset catalog and first browser category; commercial-library non-blocking behaviour.
4. For each change, record exact files/symbols, before/after, unchanged invariants, tests and exact commit SHA.
5. Require Windows x64 Release, full CTest, pluginval strictness 5, package audit and separate FL Studio 20 host acceptance on the same release-candidate SHA.
6. No implementation, test, CI, host-verification or release status may be inferred from this requirements amendment.

## 8. Unchanged strict rules

Realtime callback remains free of file/network I/O, blocking locks/waits, GUI calls, decoding and unbounded allocation. Existing APVTS IDs/semantics, MIDI contracts, sampler safety, stable preset IDs, factory read-only/append-only behaviour, explicit import conflict handling, secure path/asset validation, exact-SHA release evidence and honest implementation statuses remain mandatory.


## 9. MAJOR UI clarification — MELODY and PATTERN browser behaviour (2026-11-14)

This section records the user's explicit clarification of the previously agreed browser design. It supplements section 5 and supersedes only conflicting browser-display restrictions to the extent stated below.

### 9.1 MELODY browser: exactly three coordinated areas
The MELODY-mode browser is split into three distinct areas, in this order:

1. **PRESET / GUITAR IDENTITY** — pop-punk guitar/genre, PUNK GROUP or band-style identity, and the individual song part/section. This selects **which guitar/source and which section character** is used.
2. **STYLE / PLAYING ARTICULATION** — how the guitar performs the notes. Examples: **Open Chord, Palm Mute, Muted**, plus other styles/articulations only where supported by actual mapped recordings. STYLE does not select the rhythmic sequence.
3. **PATTERN / RHYTHM VARIATION** — the rhythmic figure/variation associated with the selected guitar/section/style. It is an optional layer available from within MELODY mode and does not turn ordinary melody notes into chord triggers.

Keep the three areas visually and functionally separate. Preserve independent selection state and compatibility filtering; do not silently conflate guitar identity, articulation and rhythm.

### 9.2 MELODY note semantics and optional pattern activation
- Ordinary MELODY keys play their corresponding pitched guitar notes. A single ordinary melody note is played once and decays; it does not automatically launch a strumming loop.
- The optional PATTERN layer is activated by additional pattern-trigger MIDI notes/events drawn or played in the Piano Roll, or by an explicit dedicated PATTERN on-screen button/control.
- The approved Piano Roll key-zone convention reserves **C0–C1** for pattern-trigger keys; **C1 and above** remain the ordinary melody-note range. Pattern-trigger keys must not also play as ordinary chromatic guitar notes. The exact boundary and mapping must be covered by tests and exposed clearly to users.
- Holding a valid pattern-trigger note sustains/runs the selected rhythmic pattern according to its gate/host-transport rules; releasing it ends the pattern cleanly.
- The PATTERN area/button lets the user switch among different rhythm variations. While playing, changes must transition safely at a musical boundary, avoiding stuck notes, duplicate triggers, timing jumps and abrupt invalid cuts.
- The on-screen activation route must not alter the ordinary melody-note MIDI semantics.

### 9.3 PATTERN mode remains separate
- MELODY and PATTERN are separate performance modes with distinct MIDI semantics and preset/performance state, while sharing the approved sample library/cache and compatible source assets.
- In PATTERN mode, mapped keys select chord/voicing and trigger the corresponding recorded or validly assembled rhythmic guitar performance.
- Guitar/source identity, PLAYING STYLE / ARTICULATION, and RHYTHMIC PATTERN / variation remain separate selection dimensions. Preserve the already approved PATTERN categories and eight Pattern Slots per section.
- Native Instruments Session Guitarist — Electric Sunburst Deluxe is a behavioural/workflow reference only; no protected code, samples, presets or UI assets may be copied.

### 9.4 Browser invariants and visible PUNK GROUP selection
- PREVIEW remains non-destructive; APPLY/SELECT commits; CANCEL rolls back. PREV/NEXT, SEARCH, FILTER, SORT and FAVORITES preserve the current result context.
- Unsupported articulations or patterns must be visibly unavailable/disabled with a truthful reason; no decorative controls or silent substitution.
- The PRESET/GUITAR IDENTITY area explicitly includes the user's PUNK GROUP/band-style identity and song-section selection. This direct clarification supersedes the earlier visible-name restriction only for this approved selection dimension. Use the canonical user-provided PUNK GROUP list once reconciled; do not invent or substitute group names.
- All realtime, APVTS, sampler, MIDI, stable-ID, factory/user separation and exact-SHA validation rules remain unchanged.

**Status:** APPROVED / DOCUMENTATION ONLY. No production code or runtime behaviour is claimed implemented or tested by this amendment.


## 10. PATTERN mode — two-window preset and performance browser (2026-11-14)

This records the user's explicit clarification of the previously agreed PATTERN-mode browser layout. PATTERN mode has **two separate windows**:

### 10.1 Window 1 — PRESET / GUITAR IDENTITY
A hierarchical browser for:
- pop-punk genre / guitar type;
- PUNK GROUP / band identity;
- song-based selection;
- individual song section and compatible nested sub-items.

The group/band and song/section levels are required dimensions and must not be omitted or collapsed into generic categories. Use the canonical user-provided group list and real source mappings; do not fabricate names or claim unsupported recordings exist.

### 10.2 Window 2 — PLAYING STYLE / RHYTHMIC PERFORMANCE
A separate browser/window for the performance choices associated with the selected guitar identity:
- **PLAYING STYLE / ARTICULATION** — how the guitar is played;
- **RHYTHMIC CATEGORY / PATTERN / VARIATION** — which rhythmic figure is performed and which variation is selected.

STYLE and PATTERN remain distinct dimensions inside this second window. The approved PATTERN categories remain DOWNSTROKES, DOWN/UP, PALM MUTE, POWER CHORDS, ALTERNATE PICKING and CUSTOM PATTERNS. Preserve P1–P10 only where these IDs exist in the canonical pattern catalog, and preserve eight Pattern Slots per section. Do not manufacture missing IDs or present unsupported mappings as playable.

### 10.3 Mode semantics and state
- PATTERN is a separate performance mode from MELODY. PATTERN plays rhythmic guitar performances and retains the approved AUTO CHORDS ON/OFF behaviour; MELODY remains one ordinary key = one pitched note.
- Window 1 selects guitar/source identity and song context; Window 2 selects playing manner and rhythmic figure. They must not be merged into a single flattened list.
- Guitar/source identity, chord/voicing, tone preset/FX, playing style/articulation and rhythm pattern remain separate semantic axes. Context filtering may narrow available choices without merging their state.
- Keep PREVIEW non-destructive; SELECT/APPLY commits; CANCEL restores prior state. PREV/NEXT, SEARCH, FILTER, SORT and FAVORITES preserve the active browser scope.
- Unsupported combinations must be unavailable with a truthful reason. No fake controls, random/incompatible sample substitution or silent fallback.
- Keep the MIDI keyboard visible and interactive; preserve all realtime, APVTS, sampler, MIDI, stable-ID, factory/user and exact-SHA rules.

**Status:** APPROVED REQUIREMENT / DOCUMENTATION ONLY. No production UI, browser backend, pattern engine, MIDI implementation or validation is implied.


## PATTERN — ЗАФИКСИРОВАННАЯ СТРУКТУРА ДВУХ ОКОН

**СТАТУС: УТВЕРЖДЕНО ПОЛЬЗОВАТЕЛЕМ. ТЕКСТ И СТРУКТУРА ЗАФИКСИРОВАНЫ БЕЗ ИЗМЕНЕНИЙ.**

Два самостоятельных окна.
Первое — выбор PRESET и музыкальной идентичности: жанр, группа, композиция, секция и соответствующие подпункты.
Второе — стиль исполнения, артикуляция, ритмическая категория, PATTERN и вариации.
AUTO CHORDS сохраняется как отдельная функция режима.
Не объединять выбор источника звучания, артикуляцию и ритмический рисунок в одну категорию.

**Правило приоритета:** эта формулировка является прямой утверждённой фиксацией пользователя. При реализации и дальнейшей редакции журналов сохранять её смысл и состав без переименования, упрощения, объединения окон или подмены назначений. Более поздние явно утверждённые пользователем изменения могут дополнять её, но не считаются разрешением на молчаливое изменение.

**Граница статуса:** зафиксировано и утверждено как требование. Само по себе это не подтверждает реализацию интерфейса, браузеров, AUTO CHORDS или backend и не является результатом сборки/тестирования.


## MELODY — подтверждение финальной структуры и поведения (пользовательское утверждение)

**СТАТУС: УТВЕРЖДЕНО ПОЛЬЗОВАТЕЛЕМ.** Эта запись фиксирует подтверждение пользователем ранее согласованной финальной версии; не является новым проектным предложением.

### MELODY — три отдельные области
- **PRESET** — выбор гитарного пресета.
- **STYLE** — артикуляция и способ исполнения мелодии.
- **PATTERN** — ритмическая вариация внутри MELODY.
- Одна обычная MIDI-клавиша соответствует одной мелодической ноте. Обычное нажатие не должно самопроизвольно запускать strumming loop или PATTERN.

### Включение PATTERN внутри MELODY
- PATTERN внутри MELODY является дополнительной функцией.
- Для запуска используется дополнительная MIDI-прорисовка/триггер в Piano Roll.
- Зона C0–C1 выделяется для PATTERN-триггеров; выше располагается обычный мелодический диапазон. Триггерная зона не должна одновременно звучать как обычные хроматические ноты.
- Предусмотрено переключение вариаций PATTERN.
- Обычная MELODY и основной режим PATTERN остаются разными режимами.
- Без дополнительного запуска PATTERN MIDI исполняется обычно: одна нота звучит как нота, несколько одновременно заданных нот образуют соответствующий аккорд. Дополнительная MIDI-команда запускает выбранный PATTERN; основные MIDI-ноты задают исполняемую ноту или аккорд.
- PATTERN не должен самопроизвольно изменять обычное мелодическое исполнение.

### Главная структура интерфейса и расположение браузера
- Главные области интерфейса: **MELODY**, **PATTERN**, **FX EDIT**.
- Браузер располагается слева; его раскрытие и сворачивание не должно смещать основные области интерфейса.

**Правило приоритета и совместимости:** это пользовательское утверждение финальной версии. Сохранять все перечисленные пункты вместе с ранее утверждёнными подробными спецификациями MELODY и PATTERN. При реализации трактовать одновременные обычные MIDI-ноты как заданную пользователем ноту/аккорд, не смешивая их с дополнительными PATTERN-триггерами. Если обнаружится противоречие с более поздним явно утверждённым требованием, остановить изменение и запросить разрешение, а не исправлять молча.

**Граница статуса:** APPROVED REQUIREMENT / DOCUMENTATION ONLY. Эта запись не подтверждает реализацию UI, MIDI routing, pattern engine, browser, сборку или тестирование.


## ИСТОРИЧЕСКОЕ УТОЧНЕНИЕ И ПОДТВЕРЖДЕНИЕ РАНЕЕ СОГЛАСОВАННОЙ СТРУКТУРЫ MELODY И PATTERN — APPROVED

Статус: APPROVED BY USER; DOCUMENTED BY APPEND-ONLY JOURNAL UPDATE. Это уточнение восстанавливает ранее согласованные решения и не создаёт новую структуру вместо них. Оно не отменяет более поздние утверждённые изменения, кроме явного исправления ошибочной трактовки истории.

### 1. MELODY — ранее согласованные три графы
В обсуждении 13 ноября были согласованы три самостоятельные графы:
- PRESETS — звуки, гитары, артисты/группы и выбор соответствующего звучания;
- MELODY / ARTICULATION — способы исполнения, включая OPEN, MUTED, PALM MUTE, TREMOLO и другие артикуляции, только если они реально поддержаны источниками и mapping;
- PATTERN — ритмическая последовательность/вариация.

Это ранее согласованная структура, а не новая реконструкция. Термины и назначения граф сохраняются раздельными; не смешивать выбор источника/звуковой идентичности, артикуляцию и ритмический рисунок.

### 2. MELODY — ранее согласованное MIDI-поведение PATTERN
- Дополнительная MIDI-прорисовка/команда в Piano Roll активирует ритмическое исполнение выбранного PATTERN.
- Одиночная обычная MIDI-нота без дополнительной команды PATTERN звучит один раз как нота.
- При активном PATTERN удерживаемая нота исполняется согласно выбранному ритмическому рисунку.
- Обычные MIDI-ноты сохраняют свою мелодическую семантику; PATTERN trigger не должен случайно дублироваться как обычная нота.
- Session Guitarist — Electric Sunburst Deluxe используется только как функциональный ориентир поведения, без копирования защищённых материалов, кода, сэмплов, пресетов или UI.

### 3. MELODY и PATTERN — самостоятельные режимы
Ранее согласовано, что MELODY и PATTERN являются двумя самостоятельными режимами/инструментами:
- настройки, состояния и пресеты режимов хранятся раздельно;
- библиотеки исполнения и структуры режимов не объединяются;
- общая PPGU SAMPLE LIBRARY допустима как общий пул доступных аудиоисточников, но режимы сохраняют раздельную логику исполнения и собственные параметры.

Согласованное распределение измерений:
- MELODY: PRESET / STYLE / PATTERN;
- PATTERN: PRESET / PLAY STYLE / ARTICULATION / RHYTHMIC PATTERN / AUTO CHORDS.

Общая библиотека сэмплов не означает объединение режимов или их preset/state schemas.

### 4. Границы утверждения по MIDI trigger range
Диапазон C0–C1 не подтверждён приведённой исторической записью как ранее окончательно утверждённое пользователем решение. Поэтому данный диапазон НЕ получает статус APPROVED на основании этого подтверждения. Существующие записи о нём сохраняются как техническое предложение/требование, нуждающееся в отдельном подтверждении, и не должны выдаваться за исторически утверждённый факт. Любое применение диапазона в реализации требует проверки актуального канонического MIDI-контракта и отдельного согласования, если оно меняет утверждённую семантику.

### 5. Приоритет и неизменность
Это подтверждение фиксирует ранее согласованные структуры и разделение режимов. Оно не разрешает менять APVTS IDs/semantics, MIDI contracts, DSP contracts, sampler lifecycle, SoundBank, engine behavior, preset schema или другие строгие контракты без предусмотренного аудита и согласования. Документирование не означает IMPLEMENTED, TESTED или VERIFIED в исходном коде.


## MELODY — КАНОНИЧЕСКОЕ ОПИСАНИЕ ТРЁХ ОТДЕЛЬНЫХ ОБЛАСТЕЙ (РАНЕЕ УТВЕРЖДЕНО)

Статус: APPROVED BY USER; DOCUMENTED. Это точное краткое описание уже утверждённой структуры MELODY, а не новое предложение.

MELODY содержит три отдельные функциональные области:

1. **PRESET** — выбор гитарного пресета.
2. **STYLE** — артикуляция и способ исполнения мелодии.
3. **PATTERN** — ритмическая вариация внутри MELODY.

Области самостоятельны и не должны сливаться или подменять друг друга:
- PRESET определяет выбранный гитарный пресет/звуковую идентичность.
- STYLE определяет способ исполнения и артикуляцию.
- PATTERN определяет ритмическую вариацию внутри режима MELODY.

Это описание фиксирует ранее утверждённое назначение трёх областей. Оно не меняет MIDI-контракты, назначение триггеров, диапазоны клавиш, APVTS IDs, DSP, движок, SoundBank или схемы пресетов. Подробные совместимости и доступность конкретных вариантов определяются только реальными mapping и поддерживаемыми источниками. Документирование не означает IMPLEMENTED или TESTED.


## MELODY — УТВЕРЖДЁННОЕ MIDI-ПОВЕДЕНИЕ PATTERN В PIANO ROLL

Статус: APPROVED BY USER; DOCUMENTED. Пользователь отдельно подтвердил следующие правила поведения:

1. Дополнительная MIDI-прорисовка/команда в Piano Roll включает ритмическое исполнение выбранного PATTERN.
2. Одиночная обычная MIDI-нота без такого PATTERN звучит один раз как нота.
3. При использовании PATTERN удерживаемая нота исполняется согласно выбранному ритмическому рисунку.
4. В качестве полного функционального ориентира выбран **Session Guitarist — Electric Sunburst Deluxe**. Он служит только ориентиром поведения и функциональности; запрещено копировать его защищённые код, сэмплы, пресеты, контент или UI.

Эти правила относятся к поведению MELODY и не объединяют режимы MELODY и PATTERN. Они не задают конкретный MIDI note range, trigger-note mapping или изменение существующих MIDI/APVTS контрактов. Такие технические детали должны соответствовать каноническим контрактам проекта и не могут выводиться из этого утверждения автоматически.

Документирование не означает IMPLEMENTED, TESTED или VERIFIED в исходном коде.


## MELODY — УТВЕРЖДЁННАЯ ТРЁХЧАСТНАЯ СТРУКТУРА БРАУЗЕРА И ПОЛНОЕ ПОВЕДЕНИЕ PATTERN

Статус: APPROVED BY USER (latest explicit approval); DOCUMENTED. Настоящая запись фиксирует утверждённое пользователем полное описание браузера и поведения PATTERN внутри режима MELODY. Она уточняет и дополняет ранее внесённые краткие описания, не заменяя их.

### 1. Браузер MELODY — одно окно, три самостоятельные части

| Часть | Назначение | Содержание |
|---|---|---|
| PRESET / GUITAR IDENTITY | Выбор гитарного звучания и музыкального контекста | Pop-punk guitar/genre, группы (PUNK GROUP), отдельные композиции и соответствующие части/секции |
| STYLE / PLAYING ARTICULATION | Выбор способа исполнения | Open Chord, Palm Mute, Muted и другие артикуляции/способы исполнения, только если реально поддержаны source recordings и mapping |
| PATTERN / RHYTHM VARIATION | Выбор ритмического рисунка | Ритмические PATTERN и их вариации; разные ритмики можно переключать через эту область |

Это три визуально и функционально самостоятельные части одного браузерного окна MELODY. PRESET выбирает гитару/жанровую идентичность/группу/секцию; STYLE определяет, КАК исполняется гитара; PATTERN определяет, КАКОЙ ритмический рисунок исполняется. Не объединять эти оси и не подменять одну другой.

### 2. Активация PATTERN в MELODY

PATTERN может быть активирован двумя предусмотренными способами:
- дополнительными MIDI-нотами/командой, прописанными в Piano Roll;
- отдельной кнопкой PATTERN в интерфейсе.

Обычная MIDI-клавиша MELODY остаётся источником отдельной мелодической ноты: без активации PATTERN нота звучит один раз и естественно затухает. При активном PATTERN удерживаемая нота поддерживает исполнение выбранного ритмического рисунка. PATTERN area/button позволяет выбирать и переключать разные ритмические вариации.

Удержание корректного MIDI-триггера поддерживает выбранное ритмическое исполнение в соответствии с host transport/gate semantics; отпускание корректно завершает исполнение. Переключение вариации во время playback должно происходить музыкально синхронно, без stuck notes, duplicate triggers, timing jumps или некорректного обрыва.

### 3. Утверждённая MIDI-зона триггеров

В рамках этого последнего явного утверждения пользователя фиксируется:
- C0–C1 — нижний регистр, предназначенный для PATTERN triggers;
- C1 и выше — обычный диапазон нот MELODY.

Реализация MIDI map обязана однозначно определить границу, чтобы одна и та же MIDI-нота (в особенности C1) не трактовалась одновременно как PATTERN trigger и обычная MELODY note. До изменения или фиксации фактического MIDI contract необходимо проверить текущую нумерацию октав/host convention и существующие MIDI/APVTS contracts; документ не разрешает нарушать строгие контракты. Если обнаружится несовместимость, остановить реализацию и вынести точное техническое разрешение на согласование, не менять контракт молча.

### 4. Доступность и безопасность
- Не показывать playable варианты без реальных source recordings и mappings.
- Не подменять отсутствующий/несовместимый источник случайным звуком.
- Сохранять разделение PRESET, STYLE и PATTERN.
- UI браузера не должен перекрывать MIDI keyboard, блокировать hit-testing или менять назначение других главных областей интерфейса.
- Эта запись описывает требования к поведению; сама по себе не доказывает реализацию или прохождение тестов.

### 5. Разграничение режимов
Эта трёхчастная структура относится к браузеру MELODY. Она не объединяет MELODY с самостоятельным режимом PATTERN и не отменяет утверждённую отдельную структуру/окна режима PATTERN.


## ОБЩАЯ ПОСЛЕДНЯЯ УТВЕРЖДЁННАЯ ОСНОВА ОТОБРАЖЕНИЯ И РАБОТЫ MELODY И PATTERN

Статус: APPROVED BY USER; DOCUMENTED. Эта запись закрепляет последнюю согласованную пользователем основу для обоих режимов. Она имеет приоритет над более ранними вариантами структуры/отображения там, где они расходятся, но не отменяет строгие технические контракты и не переписывает историю предыдущих решений.

### A. MELODY — один режим, три самостоятельные графы
1. **PRESET** — выбор гитарного пресета и музыкальной идентичности: жанр, группа, композиция, секция и соответствующие подпункты.
2. **STYLE** — способ исполнения и артикуляция.
3. **PATTERN** — ритмический рисунок и его вариации внутри MELODY.

Без дополнительного запуска PATTERN MIDI работает обычно: одна MIDI-нота исполняет одну ноту; несколько одновременно заданных MIDI-нот формируют соответствующий аккорд/набор нот. Основные MIDI-ноты задают исполняемую ноту или аккорд. Дополнительная MIDI-команда запускает выбранный PATTERN. PATTERN не должен самопроизвольно изменять обычное мелодическое исполнение.

### B. PATTERN — самостоятельный режим с двумя окнами
**Окно 1 — PRESET / GUITAR IDENTITY:** выбор жанра, группы, композиции, секции и соответствующих подпунктов; определяет источник/гитарную идентичность и музыкальный контекст.

**Окно 2 — PLAYING STYLE / PERFORMANCE:** выбор способа исполнения, артикуляции, ритмической категории, PATTERN и его вариаций.

**AUTO CHORDS** сохраняется как отдельная функция режима PATTERN. Не объединять выбор источника звучания, артикуляцию и ритмический рисунок в одну категорию.

### C. Общие правила отображения и поведения
- MELODY и PATTERN остаются двумя самостоятельными режимами с раздельными настройками, состояниями и пресетами.
- Их общая PPGU SAMPLE LIBRARY не означает объединения логики исполнения или структур режимов.
- PRESET/source identity, STYLE/articulation, rhythmic PATTERN и AUTO CHORDS — разные функциональные измерения.
- В MELODY область PATTERN относится к ритмическим вариациям внутри MELODY; в режиме PATTERN отдельное второе окно содержит стиль/артикуляцию и ритмическую структуру.
- Совместимость может фильтровать недоступные сочетания, но не смешивать значения и не подменять источник случайным звуком.
- Предпрослушивание не должно незаметно применять выбор; подтверждение выбора фиксирует его, отмена возвращает предыдущее состояние.
- Отображение/браузеры не должны перекрывать MIDI-клавиатуру или нарушать её hit-testing.
- Последняя согласованная структура/отображение является базовой; любые дальнейшие изменения требуют отдельного согласования пользователя.

### D. Приоритет и технические границы
Эта запись фиксирует пользовательские требования к структуре и поведению. Она не разрешает молча менять APVTS IDs/semantics, MIDI contracts, DSP contracts, sampler lifecycle, SoundBank, engine behavior, preset schema или иные строгие контракты. Неоднозначности реализации должны быть вынесены на аудит, а не разрешены догадками. Документирование не означает IMPLEMENTED, TESTED или VERIFIED в исходном коде.
