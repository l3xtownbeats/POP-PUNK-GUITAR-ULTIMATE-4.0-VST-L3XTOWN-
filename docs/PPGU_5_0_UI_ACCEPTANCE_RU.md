# POP PUNK GUITAR ULTIMATE 5.0 — UI / UX ACCEPTANCE SPEC

Дата фиксации: 11 ноября 2026
Статус: согласованные требования пользователя; реализация в рабочей ветке ещё не подтверждена полностью.
Рабочая ветка: migration/full-copy-m3-transport-scheduler-v2

## 1. Визуальная основа
- Использовать только два утверждённых пользователем изображения как исходную визуальную основу оболочки.
- Сохранять узнаваемый стиль, композицию и характер обоих изображений. Разрешена адаптация/перерисовка для корректного размещения согласованных элементов управления.
- Не использовать дешёвые фоновые темы, случайные градиенты и типовые стандартные JUCE-контролы.
- Общая система: тёмный графит, магентовый акцент #ff2d78, премиальная музыкальная аппаратная эстетика, читаемая типографика, контраст, ритмичная сетка и аккуратные отступы.
- Все контролы должны иметь состояния normal/hover/pressed/disabled/focus.
- Масштаб интерфейса: 75%, 100%, 150%; текст и контролы не должны обрезаться.
- MELODY и PATTERN имеют отдельные artwork-ресурсы, не смешанные с системными skin-паками.

## 2. Темы
- 10 отдельных тем: SKIN/Skin_01 … SKIN/Skin_10.
- Каждая тема содержит необходимые изображения/ресурсы и manifest с идентификатором, названием, версиями ресурсов и параметрами раскладки.
- На главном экране одна кнопка SKINS; выбор темы сохраняется между запусками.
- Смена темы должна охватывать главное окно и отдельные окна, включая FX Studio, Browser и MIDI Piano Roll, где применимо.
- Тема не меняет аудиопараметры, preset identity или содержимое MIDI.

## 3. Главное окно
### Верхняя зона
- Единый PRESET BROWSER с вкладками MELODY, PATTERN, FAVORITES, USER.
- Поиск по имени, artist reference, sound character, playing style, tags и source.
- Фильтры Mode, Sound Character, Playing Style, Artist, Source Format, Favorites, Factory/User.
- В строке пресета: имя, режим, sound character, playing style, доступность sample source и краткая сводка FX.
- Для MELODY независимые фильтры SOUND CHARACTER и PLAYING STYLE.
- Для PATTERN — base preset и необязательный совместимый Style Overlay.

### Основные действия
IMPORT SOUNDS, BANK SOUNDS, MELODY, PATTERN, FX STUDIO, SKINS, MIDI, DOUBLE, PROCESSOR, MULTICORE.

### AMP & TONE
GAIN, TREBLE, MID, BASS, PRESENCE, DEPTH, MASTER, CABINET, OUTPUT. Каждый control должен быть связан с реальным DSP/APVTS параметром или отображаться как недоступный до реализации.

### PLAYBACK
VELOCITY, DYNAMICS, SWING, HUMANIZE, ACCENT, START MODE, VOICES/POLYPHONY. Дополнительно AUTOCHORDS ON/OFF, KEY, SCALE там, где режим PATTERN это использует.

### Центральная performance-зона
- Текущий режим и имя активного пресета.
- Текущий аккорд/корень, активные ноты и MIDI input indication.
- Визуализация MIDI keyboard и активности нот.
- Текущая артикуляция/библиотека/источник звука.
- В PATTERN — название ритма, Style Overlay и состояние 8 быстрых pattern slots.

### Нижняя статусная зона
Master Volume, MIDI Input/Activity, CPU, RAM, Voices, активная библиотека и Help.

## 4. FX Studio
Отдельное изменяемое окно. Модули: Compressor, EQ, Distortion, Chorus, Delay, Reverb, Cabinet IR. Для каждого — ON/OFF и доступные параметры; reorder drag-and-drop, если поддерживается DSP graph. Состояние FX сохраняется в preset/state. Наличие UI-кнопки не считается доказательством готового эффекта.

## 5. PATTERN Studio
Категории: Lead Pattern, Solo/Melodic, Palm-Muted Rhythm, Open Chords, Power Chords, Downstroke, Alternate Picking, Pop-Punk Strum, Acoustic Strum, Arpeggio, Octave/Hook, Tremolo/Fast Drive, Stop-Start, Ambient/Wide.
- 8 быстрых слотов, поиск ритмов, AUTO/PRESET DEFAULT.
- Reset Overlay и Reset Preset — разные действия.
- Base preset владеет sample source, тоном, FX и музыкальной идентичностью.
- Style Overlay меняет только явно объявленные совместимые поля исполнения/артикуляции.
- Если требуемая артикуляция/стиль не поддерживается, показывать понятное предупреждение и сохранять AUTO; не имитировать результат молча.

## 6. BANK SOUNDS / IMPORT SOUNDS
- Импорт файлов, папок, ZIP и SFZ.
- Форматные разделы/фильтры WAV, AIFF, FLAC, OGG, SFZ; MP3 только при реально включённом декодере.
- Оригиналы хранятся неизменными в Original; оптимизированные данные playback cache хранятся отдельно.
- Сохранять SFZ mapping и относительные пути. Не flatten SFZ в единый WAV.
- Preview, метаданные, прогресс, отмена, duplicate detection, import report, rescan, missing-file repair.
- User Bank отдельно от read-only Factory Bank.
- Сохранять лицензии, атрибуцию и SHA-256; запрещено включать в дистрибутив сэмплы без явных прав на распространение.

## 7. MIDI Piano Roll
Отдельное плавающее/изменяемое окно: Import/Export MIDI, Save/Load Clip, Play/Stop, Grid 1/4…1/32 и triplets, Snap, Zoom, Scroll, piano keyboard, редактирование нот, Velocity Lane, copy/paste/duplicate/delete.
В PATTERN MIDI-клип задаёт onset/gate/velocity/articulation и транспонируется входящим корнем/аккордом. MELODY не перехватывается pattern-логикой.

## 8. Музыкальная семантика
- MELODY: одна MIDI-клавиша = одна хроматическая нота, без автоматического ритма и аккордов. SOUND и STYLE — независимые оси.
- PATTERN: входящая клавиша/аккорд выбирает или транспонирует preset-defined rhythm/performance.
- Palm mute должен выбирать реальную muted articulation/sample layer, если она доступна. Простое уменьшение громкости не является palm mute.
- Artist references — стилистические ориентиры, не копирование записей или защищённых семплов: Blink-182, Sum 41, Simple Plan, New Found Glory, Good Charlotte, Mest, Angels & Airwaves, All-American Rejects, MGK, Avril Lavigne.

## 9. Реализация против требований
Последний проверенный заголовок Source/PluginEditor.h в этой ветке подтверждает только частичную основу: LOAD SFZ, MELODY/PATTERN tabs, preset/category/search controls, 9 knobs, 7 FX toggles и keyboard component. Полный список выше — целевое состояние и критерии приёмки, а не заявление о реализованной функциональности.

## 10. UI acceptance checklist
- [ ] Оба утверждённых изображения находятся в рабочей ветке и отображаются как основа оболочки.
- [ ] Десять Skin packs валидируются manifest checker; смена темы переживает перезапуск.
- [ ] Все окна используют согласованную тему и масштабируются 75/100/150%.
- [ ] Все главные кнопки выполняют заявленные действия; неактивные функции явно обозначены.
- [ ] Browser filters работают независимо и не мутируют активное исполнение при preview.
- [ ] Base preset/Style Overlay/reset/save precedence подтверждены тестами.
- [ ] FX controls связаны с реальным DSP и сохраняют состояние.
- [ ] Bank import/preview/metadata/missing file/duplicate/cancel/reopen сценарии проверены.
- [ ] MIDI Piano Roll import/export/edit/save/load/Pattern routing проверены.
- [ ] Keyboard, chord, active notes, MIDI activity, CPU/RAM/voices показывают реальные данные.
- [ ] Проверены UI responsiveness, no audio-thread file I/O, scaling, focus, keyboard navigation и host resize.
- [ ] Проведён визуальный smoke test в FL Studio 20 на Windows 10 x64.
