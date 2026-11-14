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


## ПОДТВЕРЖДЕНИЕ ИСТОРИЧЕСКОЙ ФИНАЛЬНОЙ СПЕЦИФИКАЦИИ ОТ 13 НОЯБРЯ — APPROVED WITH ADDITIONS

Статус: APPROVED BY USER; DOCUMENTED. Пользователь подтвердил эту запись как восстановление и закрепление договорённостей, обсуждавшихся и согласованных 13 ноября. Это не новая разработка структуры и не разрешение заменять согласованные решения поздними реконструкциями. Запись дополняет предыдущие журнальные разделы; история сохраняется append-only.

### 1. MELODY — три области, согласованные 13 ноября
- **PRESET** — выбор гитарного пресета, звуковой/гитарной идентичности, жанра, группы, композиции и соответствующей секции.
- **STYLE** — артикуляция и способ исполнения мелодии (например, OPEN, MUTED, PALM MUTE, TREMOLO, Open Chord и другие только при наличии реальной поддержки источниками/mapping).
- **PATTERN** — ритмическая вариация внутри MELODY.

PRESET, STYLE и PATTERN остаются тремя разными функциональными областями. Не переименовывать их смысл, не сливать и не записывать как впервые появившиеся новые функции.

### 2. MELODY — обычное MIDI и дополнительный PATTERN
- Без дополнительной команды обычная MIDI-нота исполняется как нота; несколько одновременно заданных основных MIDI-нот задают соответствующий аккорд/набор нот.
- PATTERN не должен самопроизвольно превращать обычное нажатие MIDI-клавиши в strumming или ритмическую последовательность.
- Дополнительная MIDI-прорисовка/команда в Piano Roll запускает выбранный PATTERN.
- При активном PATTERN удерживаемая нота исполняется согласно выбранному ритмическому рисунку; отпускание корректно завершает исполнение.
- Предусмотрена отдельная кнопка PATTERN в интерфейсе как способ активации/управления, а также переключение ритмических вариаций.
- Переключение во время воспроизведения должно быть музыкально синхронным и не создавать stuck notes, дублирующиеся триггеры, скачки тайминга или некорректные обрывы.
- MIDI-зона C0–C1 для PATTERN triggers и C1 и выше для обычных нот MELODY зафиксирована как утверждённое пользователем требование в последующем явном подтверждении полного описания. Граничная нота C1 должна иметь однозначную трактовку в MIDI map с учётом host octave convention; не допускать двойного назначения.

### 3. MELODY и PATTERN — два самостоятельных режима
- Это два самостоятельных режима/инструмента.
- Настройки, состояния и пресеты режимов хранятся раздельно.
- Библиотеки исполнения и структуры режимов не объединяются.
- Допускается общая **PPGU SAMPLE LIBRARY** как общий пул аудиоисточников; при этом логика исполнения и параметры режимов остаются разными.
- MELODY: PRESET / STYLE / PATTERN.
- PATTERN: PRESET / PLAY STYLE / ARTICULATION / RHYTHMIC PATTERN / AUTO CHORDS.

### 4. PATTERN mode — два самостоятельных окна
- **Окно 1 — PRESET / GUITAR IDENTITY:** жанр, группа, композиция, секция и соответствующие подпункты.
- **Окно 2 — PLAYING STYLE / PERFORMANCE:** стиль исполнения, артикуляция, ритмическая категория, PATTERN и вариации.
- **AUTO CHORDS** сохраняется отдельной функцией режима PATTERN.
- Не объединять выбор источника звучания, артикуляцию и ритмический рисунок в одну категорию.

### 5. Главный интерфейс и браузер
- Главные области продукта: **MELODY**, **PATTERN**, **FX EDIT** — самостоятельны и не подменяют друг друга.
- Браузер размещается слева; раскрытие/сворачивание браузера не должно сдвигать основные области интерфейса.
- Браузеры и overlays не должны перекрывать MIDI-клавиатуру или блокировать её hit-testing.
- Общие средства поиска/фильтрации/избранного/предпрослушивания могут использоваться совместно только там, где это не объединяет раздельные структуры, состояния и правила режимов.

### 6. Функциональный ориентир и технические ограничения
- **Session Guitarist — Electric Sunburst Deluxe** является функциональным ориентиром для поведения гитарного исполнения; защищённые код, сэмплы, пресеты, контент и UI не копировать.
- Все комбинации должны опираться на реальные source recordings и mapping. Не показывать фиктивные playable варианты и не подменять отсутствующие источники случайным звуком.
- Это пользовательская спецификация. Она не разрешает молча менять APVTS IDs/semantics, MIDI contracts, DSP contracts, sampler lifecycle, SoundBank, engine behavior, preset schema или другие строгие контракты. При конфликте реализации требуется аудит и явное согласование, а не догадка.
- APPROVED / DOCUMENTED / VERIFIED относятся к требованиям и проверке наличия текста в журналах. Они не означают IMPLEMENTED или TESTED в исходном коде.


## СТРОГАЯ ФИКСАЦИЯ ПОЛЬЗОВАТЕЛЬСКИХ УТВЕРЖДЕНИЙ — PATTERN / AUTO CHORDS / ГЛАВНЫЙ UI И ОКНА
Дата подтверждения: 14 ноября 2026.
Статус: **APPROVED BY USER — DOCUMENTATION UPDATE**.
Это append-only дополнение к ранее утверждённым требованиям. Оно не заменяет и не переписывает историю. Любое изменение этих пунктов допускается только после личного явного согласования пользователя. Никакие дополнительные предложения разработчика не считаются утверждёнными сами по себе.

### 1. PATTERN — строго два самостоятельных окна

**Окно 1 — PRESET / GUITAR IDENTITY**
- Выбор музыкальной и гитарной идентичности: GENRE, ARTIST / BAND, PUNK GROUP, SONG, SECTION и соответствующие подпункты.
- Секции включают INTRO, VERSE, PRE-CHORUS, CHORUS, BRIDGE, OUTRO там, где они реально представлены в библиотеке.

**Окно 2 — PLAYING STYLE**
- PLAYING STYLE — одно цельное окно/среда исполнения. Его нельзя дробить на дополнительные самостоятельные окна или превращать внутренние категории в новые верхнеуровневые окна.
- Внутри PLAYING STYLE размещаются взаимосвязанные средства выбора и настройки: playing techniques, articulation, articulation variations, rhythmic categories, rhythmic patterns, pattern variations и другие доступные performance options.
- Техники, артикуляции, ритмические рисунки и их вариации являются внутренним содержимым единого PLAYING STYLE.
- Функциональный ориентир по принципу организации и исполнения — Native Instruments Session Guitarist — Electric Sunburst Deluxe. Это ориентир поведения, а не разрешение копировать защищённые UI, код, контент, пресеты или аудиоматериалы.
- Показывать и давать выбирать только те техники, артикуляции, рисунки и сочетания, которые действительно поддерживаются движком, исходными записями и mappings. Недоступное обозначать честно; не подменять случайным или несовместимым звуком.

### 2. AUTO CHORDS — полноценная функция PATTERN

- AUTO CHORDS остаётся отдельной функцией режима PATTERN и не смешивается с категориями PLAYING STYLE, артикуляциями или ритмическими рисунками.
- Функция должна работать как часть настоящего гитарного PATTERN-инструмента: принимать MIDI-аккордовое управление, распознавать поступающие ноты/аккорд в рамках поддерживаемого MIDI-контракта и адаптировать исполнение выбранного PATTERN к заданному аккорду.
- При смене аккорда исполнение должно корректно переходить к новому аккорду с музыкальной синхронизацией; выбранный ритмический рисунок и характер PLAYING STYLE сохраняются, если совместимы.
- Гитарное озвучивание должно учитывать доступные корректные аппликатуры/voicings, струны, регистры и реальные source recordings/mappings. Не имитировать отсутствующие аппликатуры или артикуляции и не подставлять случайные ноты.
- Предусмотреть распознавание распространённых типов аккордов (включая мажорные, минорные, септаккорды и обращения) только в пределах реально реализуемой и проверенной поддержки движка и библиотеки. Расширение списка типов не даёт права объявлять неподдерживаемые варианты рабочими.
- Смена/отпускание MIDI-аккордов, остановка транспорта и смена рисунка должны корректно завершать/переключать голоса без stuck notes, дублирующихся триггеров, тайминговых скачков и некорректных обрывов.
- AUTO CHORDS взаимодействует с выбранными PRESET и PLAYING STYLE, но остаётся отдельной функциональной системой PATTERN.
- Любые изменения существующего MIDI contract, APVTS IDs/semantics или engine behavior запрещены без предварительного аудита и личного явного согласования пользователя.

### 3. Главный интерфейс — визуальный эталон

- Семь ранее предоставленных пользователем макетов являются обязательным визуальным стандартом PPGU. Все окна и дополнительные интерфейсные поверхности должны соответствовать этому стандарту; нельзя самовольно упрощать дизайн, заменять его другим стилем или возвращаться к старому интерфейсу.
- Общий стиль обозначен как **SIGNAL FORGE / NEON**: единая палитра, неоновые акценты, материалы/текстуры, кнопки, переключатели, регуляторы, индикаторы, панели, типографика, рамки, подсветка, визуальная иерархия, состояния и анимации. Конкретные графические решения сверяются с семью пользовательскими макетами и журналом, а не придумываются вместо них.
- Верхний ряд главного окна: **MELODY**, **PATTERN** и крупная, хорошо заметная **FX EDIT**. FX EDIT занимает место прежнего PANGROUP в этом ряду; его нельзя прятать, уменьшать до второстепенной кнопки или визуально обесценивать.
- **SKIN**, **IMPORT GUITAR**, **AUTO DISCOVER**, **CURATOR**, **SETTINGS**, **ABOUT** сохраняют ранее утверждённые расположение, назначение и поведение. Не переименовывать, не удалять и не перестраивать эти элементы без отдельного согласования.
- Главная компоновка и геометрия элементов управления неизменны между 16 скинами. Скин меняет только графическое оформление в рамках утверждённой темы; он не меняет расположение, размеры/координаты, функции, bindings, значения или звуковое поведение контролов.
- Все дополнительные окна и панели обязаны продолжать тот же визуальный язык: палитру, неоновые акценты, материалы, кнопки, переключатели, регуляторы, индикаторы, панели, типографику, рамки, подсветку, иерархию, состояния и анимации. Не допускается отдельный несогласованный стиль для FX EDIT, браузеров, настроек или других окон.

### 4. Открытие, переключение и закрытие окон

- Дополнительные окна открываются поверх главного интерфейса как отдельные рабочие поверхности/overlays согласно утверждённым макетам.
- Открытие дополнительного окна не должно перестраивать, сдвигать, сжимать или менять геометрию главного окна и его основных областей.
- Закрытие окна или возврат из него возвращает пользователя к главному интерфейсу в прежней компоновке; уже применённые настройки сохраняются. Отмена предварительного выбора выполняет согласованный rollback.
- Переключение между окнами/рабочими поверхностями должно быть предсказуемым и единообразным; не должно создавать дублирующиеся окна, терять состояние или самопроизвольно применять предварительный выбор.
- FX EDIT открывается отдельным окном поверх главного интерфейса. Главная FX-панель остаётся на главном экране; соответствующие параметры/регуляторы синхронизированы в обе стороны. Закрытие FX EDIT возвращает в главное окно без потери применённых настроек.
- Браузер располагается слева; его раскрытие/сворачивание не сдвигает главные области. Ни браузер, ни overlay, ни дополнительное окно не должны перекрывать MIDI-клавиатуру или блокировать её hit-testing.
- При любом обнаруженном расхождении между макетами, журналом и фактическим интерфейсом нельзя исправлять его молча: сначала показать пользователю соответствующую запись журнала и фактическое состояние, объяснить последствия, предложить варианты исправления и дождаться личного решения.

### 5. Приоритет, неизменность и честность статусов

- Данный раздел является строгим пользовательским требованием. Изменять, сокращать, переосмысливать или заменять его можно только после личного явного согласования пользователя.
- Он действует вместе с более ранними строгими правилами PPGU и не отменяет их: audio callback safety, стабильность MIDI/APVTS/DSP/preset contracts, sampler lifecycle, SoundBank, лицензирование, отсутствие фиктивных контролов и источников.
- При противоречии с более ранней записью применяется только явно утверждённое более позднее изменение и только в пределах его точного предмета; остальные положения сохраняются.
- Статус этой записи: APPROVED и DOCUMENTED после внесения в журналы. Это не означает, что описанный UI, AUTO CHORDS или поведение окон уже реализованы в исходном коде либо прошли тестирование. IMPLEMENTED и TESTED присваиваются только после реальных изменений кода и проверок с привязкой к конкретному SHA.


## ДОПОЛНИТЕЛЬНОЕ ПОДТВЕРЖДЕНИЕ — ВСЕ ОБСУЖДЁННЫЕ ДОПОЛНЕНИЯ УТВЕРЖДЕНЫ ПОЛЬЗОВАТЕЛЕМ
Дата: 14 ноября 2026.
Статус: **APPROVED BY USER — ОБЯЗАТЕЛЬНО К РЕАЛИЗАЦИИ**.
Пользователь явно подтвердил все дополнения и предложения, перечисленные в непосредственном предыдущем сводном описании. Поэтому элементы, ранее помеченные как предложения без подтверждения, с этого момента имеют статус APPROVED. Этот раздел дополняет журналы append-only и является обязательным требованием разработки. Изменение любого пункта возможно только после личного явного согласования пользователя.

### 1. AUTO CHORDS — дополнительные музыкальные средства управления
- Предусмотреть **ROOT NOTE** — выбор основной ноты из всех 12 хроматических нот.
- Предусмотреть **MODE** — выбор поддерживаемого музыкального лада/режима в рамках возможностей движка.
- Предусмотреть наглядную экранную клавиатуру, отображающую ноты выбранного аккорда и/или выбранного музыкального режима.
- Ноты, входящие в текущий аккорд/режим, должны визуально подсвечиваться; подсветка должна обновляться при изменении ROOT NOTE, MODE, входящего MIDI-аккорда и других применённых музыкальных параметров.
- Подсветка, клавиатура и все соответствующие элементы должны соответствовать утверждённой визуальной системе SIGNAL FORGE / NEON и активному скину.
- AUTO CHORDS остаётся отдельной функцией PATTERN, взаимодействует с PRESET и единым PLAYING STYLE и не превращается в отдельное верхнеуровневое окно.
- Эти средства управления не отменяют требований к реальному распознаванию аккордов, корректным voicings/fingerings, источникам и mappings. Нельзя показывать неподдерживаемые типы аккордов/ладов как работающие.

### 2. PRESET BROWSER — обязательная иерархия и навигация
Утверждённая последовательность навигации:
**GENRE → ARTIST / BAND → SONG / SECTION → STYLE → PATTERN**.
- Внутри SONG / SECTION поддерживаются музыкальные секции INTRO, VERSE, PRE-CHORUS, CHORUS, BRIDGE, OUTRO при наличии соответствующего контента в библиотеке.
- Последовательность служит навигационной иерархией и не смешивает функциональные значения PRESET/source identity, STYLE/performance и PATTERN/rhythm.
- Выборы и доступные подпункты контекстно согласуются с реальным контентом и mappings; отсутствие контента не маскируется фиктивными пунктами.

### 3. Боковой браузер — обязательное поведение
- PRESET / STYLE browser располагается слева.
- Предусмотреть удобную ширину для чтения иерархии библиотеки и возможность пользовательского изменения ширины.
- Предусмотреть сворачивание и повторное раскрытие браузера.
- Раскрытие, сворачивание и изменение ширины браузера не должны хаотично сдвигать или перестраивать основные области интерфейса.
- Браузер сохраняет контекст навигации при обычных действиях пользователя, если он явно не выбрал другой контекст.
- Его структура и функциональность остаются одинаковыми во всех скинах; меняется только графическое оформление согласно правилам скинов.

### 4. Browser tools — все перечисленные средства обязательны
В соответствующих браузерах предусмотреть:
- **SEARCH** — поиск;
- **FILTERS** — фильтрация;
- **FAVORITES** — избранное;
- **RECENT** — недавно использованные элементы;
- **SORT** — сортировка;
- **PREVIEW** — предварительное прослушивание;
- **PREVIOUS / NEXT** — переход между элементами;
- **CARDS** — карточное представление;
- **CONTEXT** — контекстная фильтрация совместимых элементов;
- **SONG STRUCTURE** — навигация по структуре композиции;
- **SMART FILTERS** — интеллектуальная фильтрация по реальной совместимости.
Все средства должны быть функциональными, а не декоративными. Они не должны объединять раздельные состояния MELODY и PATTERN или смешивать PRESET, STYLE, артикуляции и ритмические рисунки.
- PREVIEW остаётся non-destructive; SELECT/APPLY подтверждает выбор; CANCEL выполняет rollback предварительного выбора.
- PREV/NEXT, SEARCH, FILTER, SORT и FAVORITES должны сохранять текущий контекст браузера там, где это не противоречит явному новому выбору пользователя.
- Недоступные сочетания обозначаются честно; не выполнять случайную замену sample или pattern.

### 5. Открытие, размещение и переключение дополнительных окон — уточнённое обязательное правило
- Дополнительные окна/рабочие поверхности открываются поверх главного интерфейса и используют единый утверждённый визуальный язык.
- Открытие дополнительного окна не должно приводить к потере текущего рабочего контекста, применённых настроек или состояния других окон.
- **FX EDIT не обязан закрываться при открытии другого дополнительного окна.** В предусмотренных макетами и сценариями случаях FX EDIT может уменьшаться и/или перемещаться, чтобы освободить рабочее пространство для дополнительного окна.
- Пользователь должен иметь возможность вручную изменять положение и размер соответствующих окон там, где это предусмотрено утверждённым дизайном.
- Переключение между окнами должно сохранять их состояние и применённые параметры; не должно самопроизвольно подтверждать preview, создавать дублирующиеся окна или терять настройки.
- Закрытие дополнительного окна возвращает пользователя к предыдущему рабочему контексту. Применённые изменения сохраняются; неподтверждённый preview отменяется по правилам PREVIEW / SELECT-APPLY / CANCEL.
- FX EDIT остаётся отдельной рабочей поверхностью, а главная FX-панель остаётся доступной на главном экране; соответствующие параметры синхронизируются в обе стороны.
- Браузер остаётся слева и не сдвигает главные области. Ни браузеры, ни overlays, ни дополнительные окна не должны блокировать MIDI-клавиатуру или её hit-testing.
- **Уточнение приоритета:** этот пункт конкретно уточняет ранее внесённую формулировку о неизменной геометрии главного окна. Главная компоновка не должна произвольно перестраиваться, однако FX EDIT может уменьшаться/перемещаться при открытии дополнительных окон в соответствии с описанным поведением и утверждёнными макетами. Это узкое уточнение не отменяет остальные правила геометрии и скинов.

### 6. Разрешение документальных расхождений
- Все browser tools, CARDS, RECENT, SMART FILTERS, SONG STRUCTURE, ROOT NOTE, MODE, экранная клавиатура с подсветкой и описанное поведение окон, которые в предыдущем сводном сообщении были названы предложениями или не имели отдельной отметки подтверждения, теперь явно утверждены пользователем и обязательны к реализации.
- Если более ранняя запись называла эти функции только предложениями, данный раздел является более поздним явным подтверждением и меняет статус именно перечисленных функций на APPROVED.
- Если ранее записанная формулировка поведения FX EDIT противоречит этому разделу, применяется настоящее более позднее узкое уточнение; остальные требования сохраняются.
- Никакие дополнительные предположения, не перечисленные здесь и не подтверждённые пользователем, не получают статус APPROVED автоматически.

### 7. Обязательность исполнения и критерии готовности
- **APPROVED означает MUST IMPLEMENT**: каждое утверждённое требование обязательно к реализации в продукте.
- **DOCUMENTED** означает наличие требования в журнале и не заменяет реализацию.
- **IMPLEMENTED** присваивается только после фактической реализации в исходном коде.
- **TESTED** присваивается только после реального выполнения соответствующих тестов с зафиксированным результатом.
- **VERIFIED** указывается с точным объектом проверки и SHA/доказательствами.
- **COMPLETE** допускается только после реализации и успешной проверки всех обязательных требований соответствующего объёма.
- Нельзя считать продукт или функцию готовыми, если она лишь описана, показана графически, частично работает или не прошла соответствующие тесты.
- Если требование конфликтует с неизменяемыми техническими контрактами, лицензиями или безопасностью audio callback, остановиться, провести аудит и запросить личное решение пользователя. Не упрощать и не менять требование молча.


## 2026-11-14 — USER APPROVAL: CANONICAL PUNK GROUP PRESET CATALOG / SOUND DNA
Пользователь явно утвердил обязательное внесение в factory preset system всех ранее обсуждавшихся и отражённых в development journals групп, их жанров, звуковых описаний, ритмики, стилей исполнения и музыкальных ориентиров. Каноническая подробная карта: `docs/PPGU_CANONICAL_PUNK_GROUP_PRESET_SOUND_MAP_RU.md`.

### Обязательный охват — 21 PUNK GROUP / ARTIST PROFILE
Blink-182; Sum 41; Simple Plan; New Found Glory; Good Charlotte; Mest; Angels & Airwaves; The All-American Rejects; MGK; Avril Lavigne; The Offspring; Green Day; Bowling for Soup; Sugarcult; All Time Low; Fall Out Boy; Yellowcard; My Chemical Romance; Less Than Jake; Fenix TX; Midtown.

### Обязательные sound DNA / стилистические профили
- Blink-182: bright modern pop-punk crunch, palm-muted verse → wide open chorus, driving eighths, power-chord accents, octave hooks.
- Sum 41: aggressive skate-punk attack, dense rhythm drive, articulated leads, fast downstrokes, palm-muted gallop, alternate picking, stop-start power-chord hits.
- Simple Plan: polished melodic pop-punk, bright attack, controlled low end, clean arpeggios, open chords and melodic hooks.
- New Found Glory: fast melodic punk, sharp downstrokes, tight palm-muted verses and open power-chord chorus.
- Good Charlotte: mid-forward punchy pop-punk, compact rhythm, octave-lead hooks and controlled open-chord accents.
- Mest: melodic pop-punk lead and energetic rhythm; preserve `Photographs` (2005), 0:13–0:37 as a sound-character reference only.
- Angels & Airwaves: spacious layered guitars, modulation, ambient sustain, arpeggios and wide atmospheric textures.
- The All-American Rejects: radio-ready pop-rock/pop-punk, defined crunch, tight eighths, clean arpeggio → crunch chorus; `Dirty Little Secret` and `Move Along` are style references.
- MGK: modern saturated pop-punk, aggressive bright rhythm, muted verse pulse → explosive open chorus; `Bloody Valentine`, `Papercuts`, `Tickets to My Downfall` are style references; preserve Bloody Valentine Inspired Lead/Chorus sound targets.
- Avril Lavigne: bright melodic pop-punk, crunchy rhythm, fast picking and open-chord strums; `Bite Me` is the late-era style reference.
- The Offspring: fast aggressive punk, tight distorted rhythm, continuous downstrokes and stop-start accents.
- Green Day: classic punk midrange crunch, focused power chords, straight eighth-note downstrokes.
- Bowling for Soup: bright upbeat pop-punk, lighter crunchy rhythm, upbeat down/up eighths and open strums.
- Sugarcult: glossy alternative pop-punk, wide rhythm layers, octave pulse, arpeggios and dynamic builds.
- All Time Low: modern melodic pop-punk, polished clean-to-crunch, down/up eighths and melodic octave accents.
- Fall Out Boy: syncopated emo-pop-punk, octave hooks, stop-start accents and polished high-mid crunch.
- Yellowcard: melodic pop-punk with acoustic/electric layering, bright open chords, picked arpeggios and ringing upper strings.
- My Chemical Romance: dramatic emo-punk, forceful downstrokes, octave lines and quiet-to-explosive dynamics.
- Less Than Jake: upbeat ska-punk, crisp offbeat upstrokes, short chord chops and bright clean-to-crunch.
- Fenix TX: fast melodic skate-punk, tight eighth-note downstrokes, punchy palm-muted verses and open choruses.
- Midtown: urgent melodic emo-pop-punk, ringing octave leads, tight rhythm attacks and energetic dynamic changes.

### Жанровые и ритмические оси
Сохранить жанровые фильтры Classic Punk / Punk Rock, British Punk, Pop Punk, Hardcore / Skate Punk, Ska Punk, Post-Punk; продуктовые категории Pop Punk Essentials, Modern Pop Punk, Retro Punk, Emo Vibes, Energetic Rock, Clean & Bright, Crunchy Power, Dreamy Atmosphere.
Ритмический словарь включает straight/continuous downstrokes, down/up eighths, alternate picking, palm-muted rhythm/gallop, muted verse → open chorus, open chord/power-chord strums, ska offbeats, syncopated stops, double hits, ghost notes, staccato, accents/rests, sixteenth-note wall, octave hooks, arpeggios, tremolo picking, d-beat, half-time, acoustic strums/fingerpicking. Последние техники показываются как playable только при наличии совместимых реальных source/performance mappings.

### Правила распределения по пресетам
- Сохранить 300 как минимум: 150 MELODY + 150 PATTERN; каталог открыт для append-only расширения.
- Все 21 профиля должны иметь согласованные связи с реальными MELODY и PATTERN preset definitions, а не только текстовые browser tags.
- В каждом preset record согласовать stable ID, mode, group/profile ID, genre tags, soundCharacter, sampleProfile, tone/FX, артикуляции, rhythm/groove, section/variation, source/performance maps, provenance/license и runtime status.
- MELODY остаётся one-key/one-pitched-note; PATTERN остаётся chord/voicing + rhythmic performance. PRESET identity, tone, articulation и rhythm не смешивать.
- Электрические и акустические исполнения требуют соответствующих самостоятельных source/performance mappings; нельзя выдавать EQ-перекраску за другой тип исполнения.
- В браузере можно показывать каталоговую цель, но playable статус разрешён только при наличии проверенных assets, mappings и playback path. Placeholders и дубликаты не засчитываются.

### Данные, лицензии и аудит
- Имена групп/композиций и sound DNA — только оригинальные стилистические ориентиры; не копировать master recordings, stems, riffs/transcriptions, коммерческие пресеты, samples или artwork без явных прав.
- Существующий `presets/factory_bank_300.json` уже содержит 21 group profile и 150+150 definitions, но требует отдельного data-integrity audit: в частности, проверить соответствие referenceArtist/referenceTracks, styleProfileId, sampleProfile (electric/acoustic), soundCharacter, playingStyle/groove, section/variation и mappings. Не считать случайные cross-artist song references или несогласованные acoustic/electric labels утверждёнными характеристиками.
- Отсутствие подтверждённого названия песни означает пустое поле, а не выдуманный или чужой пример.
- До аудита не объявлять 300 записей playable, implemented, tested или полностью согласованными.
- Жанровые семейства British Punk и Post-Punk не связывать с конкретными группами догадкой; точные group mappings добавляются только по подтверждённым записям.

### Приоритет и защита от конфликтов
Это дополнение расширяет каталог и описывает sound DNA; оно не отменяет и не ослабляет ранее утверждённые строгие правила, UI/окна, семантику MELODY/PATTERN, MIDI/APVTS/DSP/audio-thread contracts, SoundBank lifecycle и license policy. При любом конфликте остановить затронутую часть и запросить личное решение пользователя. Более поздняя запись меняет старое требование только в явно обозначенной части.
**Статус записи:** APPROVED by user; DOCUMENTED after commit; VERIFIED только после повторного fetch. IMPLEMENTED / TESTED не присваиваются документированием.


## УТОЧНЕНИЕ ПОЛЬЗОВАТЕЛЯ — КОЛИЧЕСТВО FACTORY PRESETS НЕ ИМЕЕТ ВЕРХНЕГО ПРЕДЕЛА
Дата: 14 ноября 2026.
Статус: **APPROVED BY USER — MUST IMPLEMENT**.

Пользователь уточнил и утвердил: количество пресетов PPGU может быть больше любого ранее указанного числа; **верхнего ограничения на общее количество заводских пресетов нет**.

- Число 300 (не менее 150 MELODY и 150 PATTERN) — это только ранее установленный минимальный контрольный ориентир/минимальная планка, а не фиксированный размер банка и не потолок.
- Factory preset catalog является открытым, расширяемым, append-only каталогом: допускается любое дальнейшее количество MELODY и PATTERN presets, а также дополнительных согласованных категорий, без искусственного ограничения по общему числу.
- Новые пресеты добавляются по мере появления проверенных, уникальных и качественных sound profiles, source assets, performance mappings и музыкально обоснованных вариаций. Не создавать пустые placeholders, дубликаты или фиктивные пресеты ради увеличения счётчика.
- Расширение каталога не должно ухудшать запуск, производительность, память, поиск, навигацию или стабильность. Использовать подходящие индексы/ленивую загрузку/кэширование; не загружать весь контент и samples в audio callback.
- Любые количественные счётчики в UI и документации должны показывать фактическое количество записей и не представлять 300 как максимум.
- Это уточнение не отменяет требования к стабильным ID, backward compatibility, лицензиям, реальным mappings, разделению MELODY/PATTERN, качеству и тестированию.
- Если старый текст допускает неоднозначное прочтение числа 300 как лимита, применяется настоящее уточнение: **никакого верхнего лимита нет**. Остальные требования остаются в силе.

**Статусы:** APPROVED / DOCUMENTED после внесения; VERIFIED только после повторного fetch. IMPLEMENTED / TESTED этим документированием не подтверждаются.


## РАСШИРЕННОЕ УТОЧНЕНИЕ — НЕОГРАНИЧЕННЫЙ РОСТ ВСЕХ КАТЕГОРИЙ PRESET BROWSER В MELODY И PATTERN
Дата: 14 ноября 2026.
Статус: **APPROVED BY USER — MUST IMPLEMENT**.

Пользователь уточнил, что отсутствие верхнего ограничения относится не только к общему числу factory presets, но и ко всем соответствующим каталогам и категориям браузера — как для одиночной игры MELODY, так и для режима PATTERN.

### 1. Неограниченное количество элементов
Без фиксированного верхнего предела могут расширяться:
- MELODY / single-play presets;
- PATTERN presets;
- PRESET / GUITAR IDENTITY профили, GENRE, ARTIST / BAND / PUNK GROUP, SONG, SECTION;
- STYLE presets и playing-style профили;
- articulation presets, артикуляции, техники исполнения и их варианты;
- rhythmic categories, PATTERNs, groove profiles, pattern variations;
- tone / sound-character / guitar-source профили и поддерживаемые акустические/электрические варианты;
- performance options, chord/voicing options и другие предусмотренные спецификацией пункты браузера;
- SEARCH/FILTER/FAVORITES/RECENT/SORT/CARDS/CONTEXT/SONG STRUCTURE/SMART FILTERS должны находить и обслуживать весь фактический расширяемый каталог, а не ограниченный фиксированный список.

### 2. Независимость MELODY и PATTERN
- Каталоги MELODY и PATTERN могут расти независимо и не имеют искусственного лимита количества записей.
- Расширение одного режима не должно ограничивать или изменять содержимое другого.
- Сохраняется функциональное разделение: MELODY — обычная одиночная игра нотами; PATTERN — управляемое ритмическое исполнение по утверждённому MIDI/chord contract.
- STYLE, articulation и rhythm остаются отдельными смысловыми уровнями; не смешивать их в один список и не подменять друг другом.

### 3. Без искусственных квот на группы и категории
- Не устанавливать лимиты количества пресетов на одну группу, жанр, композицию, секцию, стиль, артикуляцию, технику или ритмическую категорию.
- Допускается любое количество обоснованных пресетов и вариаций в пределах каждой категории.
- Ранее установленный минимум 300 (150 MELODY + 150 PATTERN) сохраняется только как нижний контрольный ориентир, но не ограничивает ни общий каталог, ни отдельную категорию.
- Добавление новых групп/категорий не должно требовать удаления, замены или переименования уже утверждённых элементов.

### 4. Требования к масштабируемости
- Browser и preset data model должны поддерживать большие каталоги без жёстко зашитых размеров массивов, UI-списков, лимитов на категории или фиксированных верхних счётчиков.
- Применять стабильные IDs, индексацию, lazy loading, pagination/virtualized lists, кэширование и асинхронную подготовку данных там, где это соответствует архитектуре.
- Никакой disk/network I/O, parsing, blocking или неограниченных allocations в audio callback.
- Масштабирование не должно ухудшать время запуска, отзывчивость UI, поиск, переключение пресетов, CPU/RAM или стабильность аудиодвижка.
- Все browser tools должны работать на полном фактическом каталоге; UI показывает реальные количества и не скрывает элементы из-за произвольного внутреннего лимита.
- Производительность и память должны быть измерены на репрезентативно больших каталогах; нельзя считать масштабируемость подтверждённой только по отсутствию UI-потолка.

### 5. Контроль качества и честный статус
- Неограниченный каталог не означает автоматическое добавление пустых, повторяющихся, невалидных или неподдерживаемых записей.
- Каждый элемент должен иметь стабильную идентичность, корректные связи с профилями, источниками и mappings, лицензионный статус и проверяемую совместимость.
- Неподдерживаемые комбинации показываются как недоступные с понятной причиной; нельзя имитировать playable status.
- Нельзя считать новый preset/style/articulation/pattern реализованным до реализации его реального поведения и прохождения соответствующих тестов.

### 6. Приоритет
Это уточнение расширяет предыдущее правило «без верхнего лимита» на все перечисленные сущности и категории обоих режимов. Оно не отменяет и не ослабляет ранее утверждённые правила структуры браузеров, MELODY/PATTERN, MIDI/APVTS/DSP/audio-thread contracts, лицензий, визуальной системы и качества. При обнаружении конфликта остановить затронутую часть и запросить личное решение пользователя.

**Статусы:** APPROVED / DOCUMENTED после внесения; VERIFIED только после повторного fetch. IMPLEMENTED / TESTED не присваиваются этим документированием.
