# POP PUNK GUITAR ULTIMATE 5.0 — MELODY / PATTERN и проверка пресетов

Статус: обязательная спецификация реализации и приёмки. Документ не утверждает, что перечисленные функции уже завершены.

## 1. MELODY

- MELODY и PATTERN взаимоисключающие режимы; режим можно выключить вручную. При переключении/выключении прекращаются ноты, созданные предыдущим режимом.
- В MELODY каждая входящая MIDI-клавиша запускает ровно одну соответствующую хроматическую ноту гитары. Никаких аккордов, автогармонизации или ритмических замен.
- Выбранный пресет меняет тембр и параметры исполнения: например Acoustic, Clean, Crunch, Distortion, Lead, Palm Mute. Одна клавиша остаётся одной нотой.
- Note-off, velocity, release, pitch bend, sustain и polyphony должны передаваться в sampler корректно.

## 2. PATTERN — preset-driven performance

- PATTERN запускает не одиночную ноту, а записанный/описанный ритмический рисунок выбранного пресета.
- MIDI-клавиша задаёт корень/тональность рисунка. Следующая клавиша меняет корень и транспонирует тот же выбранный рисунок без сброса темпа и без изменения его характера.
- Характер исполнения определяется пресетом, а не только общим номером Pattern Slot:
  - LEAD: протяжные, разреженные или мелодические фразы с длинными gate и lead articulation.
  - DISTORTION / CRUNCH: плотные ритмические удары, palm-muted chugs, акценты и открытие на сильных долях.
  - CLEAN: чистый бой/арпеджио; если выбран акустический профиль — акустический down/up strumming.
  - ACOUSTIC: акустический бой с реальными акустическими струнными voicing/performance samples, динамикой down/up и корректным затуханием.
- Рисунок должен быть музыкально согласованным с BPM/transport хоста, длиной паттерна, swing, humanize и сменой корня.
- Для каждого PATTERN-пресета хранятся независимые поля soundCharacter, playingStyle/groove, articulation, sampleProfile/performanceMap, rhythm steps, gate, velocity/accent и humanization. SoundCharacter нельзя использовать как единственный идентификатор рисунка.
- Смена пресета в PATTERN должна немедленно переключать и тембр, и исполнительскую модель/рисунок; это не просто смена Drive/Tone/Reverb.
- Поведение должно быть по схеме preset-driven performance, ориентированной на workflow NI Session Guitarist — Electric Sunburst Deluxe, но с собственным кодом и только лицензированными ассетами.

## 3. AutoChords

- AutoChords работает только при PATTERN + AutoChords ON.
- Выбранная клавиша задаёт root C–B; режим лада/тональности задаётся отдельным параметром. Для каждой root/scale комбинации генератор строит корректный гитарный voicing/chord shape, а рисунок исполняет этот voicing.
- AutoChords OFF: не переписывать входящие MIDI-ноты; использовать обычные MIDI/Piano Roll ноты/аккорды как источник voicing.
- В MELODY AutoChords не должен менять MIDI ни при каком состоянии своего переключателя.
- Смена root-клавиши во время PATTERN должна менять voicing на ближайшей безопасной музыкальной границе/по установленному quantize, без зависших нот и обрыва аудио.

## 4. Сохранение пресетов

### Factory
- Все заводские определения должны иметь стабильный уникальный ID, mode, name, soundCharacter, playingStyle/groove, articulation, параметры и ссылки на существующий sample/performance profile.
- При загрузке должны применяться все параметры пресета, а не только Drive/Tone/Reverb/Output.
- В PATTERN factory entry без доступного performance map/sample profile должен быть помечен как unavailable/incomplete и не выдаваться за полностью играющий пресет.

### User
- Поддержать Save, Save As, Rename, Delete, Import и Export для .ppgm (MELODY) и .ppgp (PATTERN), а также user bank.
- Проверять JSON/schema, mode-extension consistency, уникальность ID, диапазоны параметров, существование sample/performance profile и валидность pattern steps.
- User preset должен сохранять и восстанавливать параметры, root/scale, AutoChords, pattern/groove, articulations и sample profile. Сохранение не должно перезаписывать factory bank.
- Повреждённый/неподдерживаемый файл не должен ломать загрузку плагина; показать понятную ошибку и оставить текущий пресет неизменным.

## 5. Проверки перед объявлением готовности 5.0

1. Factory catalog: ровно 150 MELODY + 150 PATTERN; уникальные IDs; обязательные поля и корректные диапазоны.
2. Runtime browser: перечисляет все доступные factory и user presets, фильтрует по режиму/категории и корректно загружает каждый доступный пресет.
3. MELODY MIDI matrix: ноты по всей клавиатуре, velocity layers, note-off/release, pitch bend, sustain, polyphony, отсутствие pattern interception.
4. PATTERN matrix: Lead / Distortion / Clean / Acoustic; root C–B; major/minor/chromatic; смена клавиш; MIDI chords; note-off; tempo/transport start-stop-seek; swing/humanize; no stuck notes.
5. AutoChords ON/OFF: только в PATTERN; проверить C, D, F#, Bb roots и Major/Minor; OFF сохраняет исходные MIDI notes; MELODY неизменен.
6. User presets: Save/Save As, перезапуск DAW/плагина, recall, rename, delete, import/export, malformed JSON, duplicate ID, missing assets.
7. Host acceptance: Windows x64 VST3, FL Studio 20, MIDI keyboard и Piano Roll; CTest + pluginval level 5. CI не заменяет реальный host test.

## 6. Текущий аудит репозитория (2026-11-11)

- `presets/factory_bank_300.json`: обнаружено 150 MELODY + 150 PATTERN, всего 300; обязательные базовые поля присутствуют.
- Все 150 MELODY помечены `catalog-definition-awaiting-plugin-integration-and-sample-validation`.
- Все 150 PATTERN помечены `catalog-definition-awaiting-pattern-engine-and-recorded-performance-library`.
- Pattern soundCharacter распределён по 8 группам (Clean, Crunch, Drive, Palm Mute, Open Chords, Acoustic, Lead Drive, Muted Chugs), но эти записи пока не доказывают готовность реальных performance maps.
- В UI текущий загрузчик каталога извлекает только name/category/mode/soundCharacter/groove/articulation и применяет Drive/Tone/Reverb/Output. В статусе PATTERN явно указано `PERFORMANCE ENGINE REQUIRED`.
- Текущий audio Pattern path строит ритм из восьми фиксированных rhythm masks и использует входящие MIDI ноты как аккордовые ноты. В этом пути пока не найдено применение preset-specific groove/performance map или AutoChords root/scale.
- `presets/examples` содержит один .ppgm и один .ppgp. Оба файла прямо помечены как format example not yet loaded by plugin. Текущий validator проверяет эти два примера, но это не проверка сохранения/загрузки пользовательских пресетов в самом VST3.
- Следовательно, 300 factory entries — это каталог определений, а не 300 полностью загружаемых и звучащих пресетов. User preset runtime workflow также ещё не принят.

## 7. Правило честного статуса

Не отмечать PATTERN, AutoChords, factory bank или user preset workflow как завершённые только потому, что JSON проходит проверку, scheduler unit tests проходят или VST3 компилируется. Завершение подтверждается runtime MIDI tests, проверкой ассетов, host acceptance и проверкой сохранения/восстановления.
