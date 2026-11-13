# MIDI PATTERN trigger lane — C0–C1

Статус: реализация маршрутизации добавлена в рабочую ветку; приёмка ожидает Windows CI и DAW-тест.

## Назначение

Нижняя октава C0–C1 зарезервирована исключительно для команд PATTERN. Она не является диапазоном гитарных нот. Триггер запускает/удерживает выбранный рисунок, а ноты выше C1 задают гармонию.

- MIDI C0 = note 12 → Pattern 01 (slot 0)
- MIDI C#0 = note 13 → Pattern 02 (slot 1)
- MIDI D0 = note 14 → Pattern 03 (slot 2)
- MIDI D#0 = note 15 → Pattern 04 (slot 3)
- MIDI E0 = note 16 → Pattern 05 (slot 4)
- MIDI F0 = note 17 → Pattern 06 (slot 5)
- MIDI F#0 = note 18 → Pattern 07 (slot 6)
- MIDI G0 = note 19 → Pattern 08 (slot 7)
- MIDI G#0 = note 20 → Pattern 09 (slot 8)
- MIDI A0 = note 21 → Pattern 10 (slot 9)
- MIDI A#0 = note 22 → Pattern 11 (slot 10)
- MIDI B0 = note 23 → Pattern 12 (slot 11)
- MIDI C1 = note 24 → Pattern 13 (slot 12)

## Поведение

1. В MELODY без C0–C1 триггера ноты и аккорды выше C1 звучат как обычный гитарный инструмент.
2. Нота C0–C1 не запускает гитарный voice и не попадает в chord voicing.
3. Пока trigger note удерживается, ноты выше C1 формируют/меняют гармонию для выбранного рисунка.
4. При смене аккорда PatternScheduler должен сохранить ритмическую фазу и подхватить новую гармонию.
5. Note-off триггера останавливает PATTERN в гибридном MELODY режиме. Если несколько trigger keys удерживаются одновременно, действует последняя нажатая; при её отпускании выбирается последняя из оставшихся удерживаемых.
6. В отдельном PATTERN режиме существующий запуск по удерживаемому аккорду сохраняется; C0–C1 дополнительно выбирает слот.
7. Входящие all-notes-off/all-sound-off и transport stop/seek сбрасывают trigger state и pending events безопасным способом.

## Важное ограничение текущего этапа

13 MIDI-слотов и маршрутизация — это контракт управления. Наличие 13 индексов не означает, что готова полноценная музыкальная библиотека из 13 уникальных, редакторских проверенных рисунков. Сейчас в processor используются фиксированные rhythm masks; их музыкальное наполнение, секции Verse/Chorus/Bridge, предпрослушивание и полноценный Pattern Browser остаются отдельной задачей.

## Приёмка

- Unit test: границы 0–127, все 13 слотов, channel isolation, overlapping triggers, release и clear.
- Processor integration: C0 trigger не должен звучать даже если SFZ содержит регион на MIDI note 12; при удерживаемом C0 chord notes выше C1 должны запускать PATTERN в MELODY.
- Regression: обычные ноты/аккорды в MELODY без trigger работают как прежде.
- Host: FL Studio 20 Piano Roll, проверка отображения C0–C1 и соответствия octave naming в настройках MIDI.
