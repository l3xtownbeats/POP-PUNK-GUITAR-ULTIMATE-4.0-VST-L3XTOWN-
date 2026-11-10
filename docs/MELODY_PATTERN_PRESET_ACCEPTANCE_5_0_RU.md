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

### 3.1. Доступность и изоляция режимов

- AutoChords существует и обрабатывает MIDI только при одновременных условиях: выбран режим PATTERN и переключатель AutoChords установлен в ON.
- В режиме MELODY AutoChords полностью исключён из MIDI/audio path: его ON/OFF, выбранная нота и лад не должны менять входящие MIDI-события, высоту, количество или длительность нот.
- Переключение MELODY/PATTERN не должно менять BPM, позицию транспорта, MIDI-клипы Piano Roll или исходную тональность проекта в DAW.
- Состояние AutoChords и выбранные root/quality сохраняются в состоянии плагина. При переходе в MELODY обработка AutoChords обходится; при возвращении в PATTERN восстанавливается выбранное пользователем состояние.

### 3.2. Панель управления AutoChords

- В интерфейсе PATTERN предусмотреть отдельную секцию AutoChords с одной явной кнопкой-переключателем ON/OFF.
- При AutoChords ON доступны два селектора:
  - ROOT / NOTE: C, C#, D, D#, E, F, F#, G, G#, A, A#, B.
  - CHORD TYPE: MAJOR и MINOR.
- Выбор root и Major/Minor формирует chord voicing для PATTERN. Изменение селекторов не меняет тональность/ноты MIDI-клипа в Piano Roll и не редактирует MIDI-данные проекта.
- Визуально и по рабочему процессу ориентироваться на удобство NI Session Guitarist — Electric Sunburst Deluxe, не копируя его код, интерфейсные ресурсы или защищённые ассеты.

### 3.3. Поведение AutoChords ON

- PATTERN получает выбранный пользователем root и chord type, строит музыкально корректный гитарный voicing и исполняет его в рамках playing style/performance map выбранного пресета.
- Root можно менять во время воспроизведения; новый voicing применяется на ближайшей безопасной границе паттерна/шага согласно quantize, без сброса BPM/transport и без зависших нот.
- Каждая клавиша, назначенная для выбора root в этом режиме, меняет корень аккорда/паттерна, а не запускает отдельную хроматическую мелодическую ноту.
- Минимальная приёмка: все 12 root-нот × Major/Minor; корректные voicings и транспонирование; note-off/panic; смена root на лету; host start/stop/seek.

### 3.4. Поведение AutoChords OFF — Piano Roll passthrough

- При AutoChords OFF PATTERN работает от входящих MIDI-нот и аккордов Piano Roll напрямую: каждая нота сохраняет исходную высоту и момент запуска.
- Не применять автоматическое определение/замену аккорда, транспонирование, переназначение root, изменение лада или тональности. Никаких скрытых chord substitutions.
- Ритмическое исполнение по-прежнему определяется выбранным PATTERN-пресетом и синхронизируется с host transport/BPM; AutoChords OFF отключает только генерацию/перезапись аккордового voicing, а не сам PATTERN-движок.
- При OFF ноты Piano Roll не должны сдвигаться по времени, менять velocity или высоту из-за AutoChords. Разрешённая ритмическая артикуляция PATTERN (strum/gate/accent) должна оставаться в рамках выбранного пресета и не изменять исходные MIDI-данные.
- В MELODY всегда действует прямое соответствие: одна входящая MIDI-клавиша = одна соответствующая нота, независимо от состояния AutoChords.

## 4. Детальная настройка пресета — отдельное FX-окно

- В основном интерфейсе должна быть кнопка **FX** для открытия окна детальной обработки текущего пресета.
- По нажатию FX открывается отдельное плавающее окно/панель поверх основного интерфейса в стиле рабочего процесса Serum / Session Guitarist: основное окно плагина остаётся доступным, а FX-окно можно перемещать и закрывать независимо.
- В FX-окне отображаются только эффекты, реально используемые текущим пресетом, в его текущем signal chain и в правильном порядке. Неиспользуемые эффекты не должны создавать ложное впечатление, что они активны; при необходимости доступен Add/Enable для поддерживаемых эффектов.
- Для каждого доступного эффекта показываются его собственные параметры и переключатель bypass/on-off. Изменение параметров применяется к текущему пресету в реальном времени без остановки MIDI/аудио и без сброса host transport.
- Поддерживаемые встроенные эффекты проекта: Compressor, EQ, Distortion, Chorus, Flanger, Delay, Reverb и Cabinet/IR (если соответствующий модуль присутствует в текущей сборке). Панель не должна показывать модуль, которого нет в активном signal chain.
- Окно закрывается отдельной кнопкой **X** в правом верхнем углу. Закрытие окна не отключает эффекты и не сбрасывает их значения — оно только скрывает окно.
- Повторное нажатие FX открывает окно снова с теми же значениями. Состояние эффектов и параметры сохраняются вместе с пресетом и состоянием плагина.
- При переключении пресета FX-окно обновляет список/параметры согласно новому пресету; нельзя переносить случайные настройки предыдущего пресета на новый, если это не предусмотрено его данными.
- Все параметры должны быть доступны через APVTS/host automation с устойчивыми ID, корректной сериализацией и безопасным обновлением из UI. UI не должен напрямую менять DSP-объекты из message thread без thread-safe механизма.
- FX-окно должно учитывать выбранную тему из 60 SKIN-паков и масштабироваться на типичных размерах окна; закрытие через X, повторное открытие, смена пресета и смена темы входят в UI regression tests.
- Приёмка: открыть FX; проверить набор модулей для нескольких пресетов Melody и Pattern; изменить параметры на воспроизведении; bypass/enable; закрыть X и открыть снова; сменить пресет; сохранить/перезагрузить проект в FL Studio 20; проверить отсутствие щелчков, зависших нот и потери автоматизации.

## 5. Сохранение пресетов

### Factory
- Все заводские определения должны иметь стабильный уникальный ID, mode, name, soundCharacter, playingStyle/groove, articulation, параметры и ссылки на существующий sample/performance profile.
- При загрузке должны применяться все параметры пресета, а не только Drive/Tone/Reverb/Output.
- В PATTERN factory entry без доступного performance map/sample profile должен быть помечен как unavailable/incomplete и не выдаваться за полностью играющий пресет.

### User
- Поддержать Save, Save As, Rename, Delete, Import и Export для .ppgm (MELODY) и .ppgp (PATTERN), а также user bank.
- Проверять JSON/schema, mode-extension consistency, уникальность ID, диапазоны параметров, существование sample/performance profile и валидность pattern steps.
- User preset должен сохранять и восстанавливать параметры, root/scale, AutoChords, pattern/groove, articulations и sample profile. Сохранение не должно перезаписывать factory bank.
- Повреждённый/неподдерживаемый файл не должен ломать загрузку плагина; показать понятную ошибку и оставить текущий пресет неизменным.

## 6. Проверки перед объявлением готовности 5.0

1. Factory catalog: ровно 150 MELODY + 150 PATTERN; уникальные IDs; обязательные поля и корректные диапазоны.
2. Runtime browser: перечисляет все доступные factory и user presets, фильтрует по режиму/категории и корректно загружает каждый доступный пресет.
3. MELODY MIDI matrix: ноты по всей клавиатуре, velocity layers, note-off/release, pitch bend, sustain, polyphony, отсутствие pattern interception.
4. PATTERN matrix: Lead / Distortion / Clean / Acoustic; root C–B; major/minor/chromatic; смена клавиш; MIDI chords; note-off; tempo/transport start-stop-seek; swing/humanize; no stuck notes.
5. AutoChords ON/OFF: только в PATTERN; проверить C, D, F#, Bb roots и Major/Minor; OFF сохраняет исходные MIDI notes; MELODY неизменен.
6. User presets: Save/Save As, перезапуск DAW/плагина, recall, rename, delete, import/export, malformed JSON, duplicate ID, missing assets.
7. Host acceptance: Windows x64 VST3, FL Studio 20, MIDI keyboard и Piano Roll; CTest + pluginval level 5. CI не заменяет реальный host test.

## 7. Текущий аудит репозитория (2026-11-11)

- `presets/factory_bank_300.json`: обнаружено 150 MELODY + 150 PATTERN, всего 300; обязательные базовые поля присутствуют.
- Все 150 MELODY помечены `catalog-definition-awaiting-plugin-integration-and-sample-validation`.
- Все 150 PATTERN помечены `catalog-definition-awaiting-pattern-engine-and-recorded-performance-library`.
- Pattern soundCharacter распределён по 8 группам (Clean, Crunch, Drive, Palm Mute, Open Chords, Acoustic, Lead Drive, Muted Chugs), но эти записи пока не доказывают готовность реальных performance maps.
- В UI текущий загрузчик каталога извлекает только name/category/mode/soundCharacter/groove/articulation и применяет Drive/Tone/Reverb/Output. В статусе PATTERN явно указано `PERFORMANCE ENGINE REQUIRED`.
- Текущий audio Pattern path строит ритм из восьми фиксированных rhythm masks и использует входящие MIDI ноты как аккордовые ноты. В этом пути пока не найдено применение preset-specific groove/performance map или AutoChords root/scale.
- `presets/examples` содержит один .ppgm и один .ppgp. Оба файла прямо помечены как format example not yet loaded by plugin. Текущий validator проверяет эти два примера, но это не проверка сохранения/загрузки пользовательских пресетов в самом VST3.
- Следовательно, 300 factory entries — это каталог определений, а не 300 полностью загружаемых и звучащих пресетов. User preset runtime workflow также ещё не принят.

## 8. Правило честного статуса

Не отмечать PATTERN, AutoChords, factory bank или user preset workflow как завершённые только потому, что JSON проходит проверку, scheduler unit tests проходят или VST3 компилируется. Завершение подтверждается runtime MIDI tests, проверкой ассетов, host acceptance и проверкой сохранения/восстановления.
