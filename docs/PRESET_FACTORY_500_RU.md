# PPGU — FACTORY PRESET BANK 500

Дата: 2026-11-14  
Файл каталога: `presets/factory_bank_500.json`  
Статус: **AUTHORED / STRUCTURAL VALIDATION PENDING CI / RUNTIME VALIDATION PENDING**.

## Состав
- 500 уникальных записей: 250 MELODY и 250 PATTERN.
- 21 артистический style profile, объединённый в 16 семейств жанров и направлений.
- 20 Melody archetypes: clean single notes, crunch/high-gain leads, palm-muted lead, octave hooks, tremolo, harmonics, slide/legato, hammer-on/pull-off, ambient delay, wide chorus sustain, DI attack, acoustic steel/nylon, picked hooks, arpeggios и другие.
- 20 Pattern archetypes, сгруппированных в утверждённые категории: DOWNSTROKES, DOWN/UP, PALM MUTE, POWER CHORDS, ALTERNATE PICKING, CUSTOM PATTERNS.
- Pattern presets хранят оригинальные 16-step sequences. PUNK GROUP stabs представлены как подтип PATTERN, а не отдельный host mode: активный APVTS сейчас предоставляет Melody/Rhythm.

## Компактная схема Pattern
`patternSequenceEncoding = tuple-v1`. Каждое событие — массив:
`[step, stroke, velocity, gate, articulation, accent, palmMute]`.
- step: 0..15; отсутствующие шаги — паузы.
- stroke: 0 down, 1 up.
- velocity: 0..127.
- gate: 0..1.
- articulation: 0 driven-picked, 1 open-chord-strum, 2 palm-muted, 3 lead-picked, 4 clean-picked, 5 acoustic-strum.
- accent и palmMute: 0/1.

## Правила авторства
1. Названия артистов используются как общие стилевые ориентиры, а не как официальные пресеты или утверждение об аффилиации.
2. Не копировать оригинальные записи, stems, ноты или защищённые риффы.
3. Reference tracks разрешены только если они явно указаны в artist profile самого артиста; неизвестные названия не придумывать.
4. Параметры — стартовые sound-design targets. Не считать preset рабочим только по наличию JSON.
5. До factory release preset должен быть связан с реальными sample IDs/zones, артикуляциями, цепочкой эффектов и подтверждённым engine mode.
6. Каждый preset проходит проверку: загрузка, диапазон нот, velocity, артикуляция, отсутствие тишины/клипов/щелчков, корректный паттерн, MIDI chord-follow, CPU/RAM и прослушивание.
7. Отсутствующие sample mappings не подменять случайными файлами; оставлять preset в состоянии pending.

## Ограничение статуса
Наличие 500 definitions не подтверждает, что 500 пресетов уже воспроизводятся в VST3. В текущем каталоге явно сохранён статус ожидания runtime loader, sample mapping и sound validation. Следующий этап — связать каталог с фактическими APVTS параметрами и SoundBank registry, затем провести автоматические и слуховые тесты.
