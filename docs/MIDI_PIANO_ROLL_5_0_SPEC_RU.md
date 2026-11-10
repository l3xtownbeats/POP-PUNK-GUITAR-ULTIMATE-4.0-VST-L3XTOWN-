# MIDI Piano Roll — PPGU 5.0

## Цель
Кнопка MIDI в основном интерфейсе открывает отдельное плавающее окно Piano Roll внутри плагина (JUCE DocumentWindow). Окно не открывает FL Studio Piano Roll и не заменяет DAW: это встроенный редактор ритмических партий и MIDI-клипов самого инструмента.

## Пользовательский сценарий
1. Нажать MIDI — открыть/показать редактор; повторное нажатие активирует уже открытое окно, а не создаёт дубликат.
2. Импортировать .mid/.midi — прочитать выбранную MIDI-дорожку или объединить дорожки по выбору; сохранить tempo/time-signature и ноты.
3. Рисовать ноты мышью, перемещать, менять длину перетаскиванием края, удалять, дублировать, менять velocity.
4. Выбрать сетку 1/4, 1/8, 1/16, 1/32 и triplets; Snap on/off; zoom; горизонтальная прокрутка; piano keyboard слева.
5. Прослушать клип через встроенный Play/Stop и услышать его через текущий guitar preset.
6. Передать клип в PATTERN: клавиши MIDI в DAW выбирают транспозицию/root/chord, а внутренний клип задаёт ритмические события и длительности. MELODY остаётся обычным режимом одной MIDI-ноты на одну гитарную ноту.
7. Export MIDI — сохранить отредактированный рисунок как стандартный .mid, пригодный для FL Studio и других DAW.
8. Save/Load PPGU Clip — сохранять собственный клип вместе с метаданными сетки, длины, темпа и названием.

## Piano Roll UI
- Отдельное окно, тёмная тема PPGU, масштабируемое.
- Слева вертикальная клавиатура; по горизонтали такты/доли; строки — MIDI-ноты.
- Контрастные линии тактов и долей, подсветка выбранной сетки.
- Note rectangles с drag/move/resize, selection marquee, multi-select, copy/paste, duplicate, delete.
- Velocity lane под сеткой с редактируемыми столбцами.
- Toolbar: Import MIDI, Export MIDI, Save Clip, Load Clip, Play, Stop, Grid, Snap, Zoom, Clear.
- Отображать выбранный MIDI-канал и длину клипа.

## Поведение PATTERN
- Каждая нота входящего MIDI-контроллера/клавиатуры в PATTERN задаёт root/chord как сейчас.
- Встроенный клип задаёт onset, gate/duration, velocity и артикуляцию каждого события.
- Транспозиция клипа относительно root должна быть отдельной настройкой; режимы Root-follow и Absolute.
- Chord notes are mapped through guitar voicing engine; avoid blindly triggering every note as an unrelated monophonic sample.
- Если клип пуст, использовать текущий factory rhythm/pattern generator — backward compatibility.
- MELODY playback must not be intercepted by the clip scheduler.
- MIDI import must never silently change the active preset or play mode.

## Технические ограничения
- Audio thread: никаких файловых операций, MIDI parsing, UI calls, heap allocation, blocking mutexes или directory scans.
- Import/export и clip serialization идут на message/background thread.
- Clip playback uses fixed-capacity immutable snapshots / lock-free publication to audio thread; overflow must fail safely and never leave stuck notes.
- MIDI export uses JUCE MidiFile/ MidiMessageSequence; preserve note lengths, velocity, tempo and time signature where representable.
- Handle overlapping same-pitch notes, NoteOff ordering, sustain pedal, tempo map and malformed MIDI safely.
- Window lifetime must be owned safely by editor; close/reopen cannot leave dangling processor/editor callbacks.
- Tests: import/export round-trip, note timing/duration/velocity, quantize and triplets, empty clip fallback, multi-track selection, tempo/time signature, overlapping notes, event-capacity overflow, no audio-thread allocation/locks.

## Phased implementation
A. Clip model + MIDI import/export + round-trip tests.
B. Piano Roll window and editing tools.
C. Lock-free clip publication and PATTERN scheduler integration.
D. FL Studio 20 validation: MIDI import/export, tempo sync, playback, window reopen, project save/restore.
E. Packaging and user guide.

## Acceptance criteria
- MIDI button opens one separate embedded plugin window.
- User can import, draw/edit, audition, save and export a rhythm.
- Exported MIDI opens correctly in FL Studio 20.
- Edited clip plays through the guitar engine in PATTERN, with each incoming root/chord key changing harmony while preserving rhythm.
- Existing factory patterns and MELODY behavior remain intact.
- No UI or file I/O work runs on the audio thread.
