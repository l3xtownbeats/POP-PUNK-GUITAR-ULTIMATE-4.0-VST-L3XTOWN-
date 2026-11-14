# Angels & Airwaves / Tom DeLonge — исследование и проектирование пресетов

**Дата фиксации:** 14.11.2026  
**Статус:** APPROVED / DOCUMENTED / RESEARCH BASELINE. Не означает, что пресеты уже собраны, интегрированы или прошли аудиопроверку.

## 1. Утверждённый охват

Добавить три самостоятельные альбомные эпохи Angels & Airwaves:
- *We Don't Need to Whisper* (2006), дебютный альбом;
- *I-Empire* (2007), второй альбом;
- *Lifeforms* (2021), последний выпущенный студийный альбом на дату фиксации.

Для каждой эпохи создать отдельные MELODY и PATTERN семейства с несколькими действительно различающимися ролями: clean/bright, ambient/echo, rhythmic drive, palm-muted, open power chords, melodic lead, octave/upper-register hook, arpeggiated/chiming и более тяжёлые варианты там, где они соответствуют материалу. Не переименовывать один и тот же звук тремя названиями.

Добавить самостоятельную категорию Tom DeLonge Technique в обоих режимах. Она должна описывать музыкальные приёмы и способы построения партий, а не копировать конкретные композиции.

## 2. Источники исследования и отделение фактов от интерпретации

### 2.1 Tom DeLonge: резонирующие ноты, мелодия и echo
В интервью Ernie Ball String Theory DeLonge рассказывает, что его ранний подход формировался в том числе потому, что он был единственным гитаристом Blink-182: он использовал резонирующие ноты и дополнительное движение за основным риффом, чтобы партия сама создавала ритмическую полноту. Он также описывает себя как songwriter/melody guy. В разговоре об Angels & Airwaves он объясняет, что базовый подход к игре не был полностью заменён: изменилось ощущение риффов благодаря echo/delay и более пространственному звучанию.

Источник: [Ernie Ball String Theory — Tom DeLonge](https://www.ernieball.com/stringtheory/tom-delonge).

Практическая интерпретация для PPGU:
- резонирующие открытые ноты и дополнительные ноты между/за основными акцентами;
- самостоятельные мелодические hooks, а не только блоковые power chords;
- controlled sustain и tempo-aware echo;
- партии, где ритм и мелодия сосуществуют в одной гитарной фактуре.

### 2.2 We Don't Need to Whisper (2006)
Вторичные разборы альбома и публичные описания характеризуют его как широкий cinematic/arena sound: просторные гитары, длинные хвосты эффектов, delay/chorus shimmer, контраст между разреженными куплетами и большими припевами. Это общая тональная интерпретация, а не подтверждение одной универсальной цепи для всех треков.

Исследовательские ориентиры:
- [FTR — liner notes / album analysis](https://www.ftrecord.com/liner-notes/angels-airwaves-we-dont-need-to-whisper-digital-version-2005)
- [ToneMirror — album gear and track breakdown](https://www.tonemirror.so/albums/angels-and-airwaves-we-don-t-need-to-whisper)
- [MusicRadar — Tom DeLonge interview context](https://www.musicradar.com/news/tom-delonge-interview-blink-182)

Тональные семейства PPGU: bright clean/edge-of-breakup; wide but controlled delay; chorus/modulation; spacious reverb; separate driven rhythm/lead; sparse-to-wide arrangement presets. Не превращать все звуки в чрезмерно мокрый pad.

### 2.3 I-Empire (2007)
В интервью Guitar World обсуждается заполнение верхнего частотного пространства chiming guitar или bell-like synth-элементами после rhythm guitars, большие мелодичные припевы, а также сравнительно ограниченный набор обработок, включая compression, delay и flanger, и широкое стерео-эхо.

Источник: [Guitar World — Tom DeLonge, “Empire State of Mind”](https://www.guitarworld.com/artists/tom-delonge-interview-empire-state-mind).

Тональная интерпретация PPGU:
- читаемая и собранная ритм-гитара;
- отдельные chiming/арпеджированные верхние линии;
- stereo/ping-pong delay и контролируемая modulation;
- широкие припевные слои без избыточного накопления низких/средних частот;
- clean и driven варианты с разной динамикой.

Не добавлять синтезаторный слой внутрь гитарного пресета автоматически: если нужен отдельный synth-like элемент, он должен быть отдельной опцией и не маскировать гитарный источник.

### 2.4 Lifeforms (2021)
Интервью периода *Lifeforms* описывают более широкий диапазон, местами более сырой punk edge, заметные гитарные партии и сочетание тяжёлых элементов с атмосферными/электронными текстурами. В интервью упоминаются Gibson ES-333 с bridge humbucker как часто используемый инструмент и Vox/Hiwatt/Orange как направления для чистого, крупного и ясного британского тона. Это полезные starting points, но не доказательство, что одна модель/цепь применялась на каждой записи.

Источники:
- [Guitar.com — Tom DeLonge / Angels & Airwaves / Lifeforms](https://guitar.com/features/interviews/tom-delonge-angels-airwaves-lifeforms-blink-182-ufos/)
- [GRAMMY.com — Angels & Airwaves: Lifeforms interview](https://www.grammy.com/news/angels-and-airwaves-new-album-lifeforms-interview-tom-delonge-ilan-rubin)
- [NME — Tom DeLonge interview](https://www.nme.com/features/music-features/in-conversation-tom-delonge-interview-angels-airwaves-lifeforms-box-car-racer-2991669)
- [Guitar World — Angels & Airwaves / Lifeforms](https://www.guitarworld.com/features/angels-and-airwaves-lifeforms)

Тональная интерпретация PPGU:
- более raw/punchy punk rhythm и чёткие power chords;
- чистые, ясные amp profiles с Vox/Hiwatt/Orange-like направлением как гипотезы для A/B;
- отдельные более тяжёлые риффовые/лидовые звуки;
- дозированные delay/modulation/ambient layers в зависимости от роли.

## 3. Техники Tom DeLonge: реализация в MELODY и PATTERN

### MELODY
- Каждая MIDI-клавиша обязана играть свою точную хроматическую высоту; эта новая категория не отменяет и не ослабляет основное требование pitch-correct instrument.
- Роли: resonant-note phrases, мелодические hooks, octave lines, верхние chiming ноты, arpeggiated figures, open/ringing chords там, где карта инструмента это поддерживает, palm-muted и open articulation, sustained notes с управляемым release.
- Echo/delay не должен удерживать MIDI voice, нарушать note-off или вызывать stuck notes.
- Создавать оригинальные демонстрационные фразы; не переносить в пресеты узнаваемые защищённые song riffs.

### PATTERN
- Роли: resonant-note riff с supporting notes; tight downstroke/palm-muted verse; open power-chord chorus; octave-hook; upper-register chiming/arpeggiated pattern; spacious anthem; sparse-to-full build; accents, rests/stops и pickup notes.
- Каждый Pattern должен адаптироваться к смене входного аккорда и формировать корректный voicing; нельзя проигрывать один неизменный loop независимо от chord input.
- Groove variations: tight downstroke, controlled alternate down/up, straight eighth-note pulse, half-time spacious drive, syncopated accents, rests/stops.
- Tempo-aware delay/reverb должны сохранять читаемость атак и корректно обрабатывать смену аккорда, transport stop/seek и note-off.
- Для каждого Pattern сохранять chord map, voicing, strum direction, timing, velocity/accent, articulation, source IDs и FX recipe.

## 4. Исследование gear recipes и открытых preset packs

Пользователь разрешил широкий исследовательский поиск:
1. Изучать публичные интервью, gear breakdowns, amp/cab/mic chains, настройки эффектов, tone recipes, видео-демонстрации и открытые preset examples.
2. Сопоставлять несколько источников; помечать подтверждённые сведения отдельно от слуховых/тональных интерпретаций.
3. Использовать публично описанные настройки как ориентир для самостоятельного создания PPGU DSP-рецептов.
4. Рассматривать сторонние preset packs/IR только после проверки конкретной лицензии, версии и права коммерческого распространения внутри VST3.
5. Если лицензия не разрешает redistribution или неясна, не включать сам файл в factory bank; допускается независимое воспроизведение общего tonal concept без копирования защищённых файлов, кода, proprietary preset data или записей.
6. Не копировать коммерческие isolated stems, master recordings, song riffs, proprietary samples или брендированные материалы.
7. Сравнивать варианты A/B по отрендеренному аудио, а не только по списку gear.

Бесплатное скачивание само по себе не означает разрешения на коммерческое распространение.

## 5. Preset count — последняя утверждённая цель

Последнее решение пользователя от 14.11.2026 заменяет прежний минимум 300:
- **MELODY + PATTERN: не менее 500 уникальных factory presets суммарно; целевой общий объём — около 600.**
- **Travis Barker 82** — отдельная самостоятельная категория из 82 уникальных полноценных пресетов; вести отдельно от основных band/style categories, не дублировать один preset ID между режимами и не считать одну запись дважды.
- Отчёт должен отдельно показывать MELODY, PATTERN, Travis Barker 82 и число уникальных preset IDs.
- Существующий каталог на 300 записей — legacy baseline для аудита, а не текущий минимум.
- Количество — цель каталога, не основание для placeholder presets. Ни один пресет не считается готовым без source map, работающего поведения, прав на включение ресурсов и audio validation.

## 6. Обязательные acceptance tests
- Полный хроматический pitch sweep в MELODY, velocity sweep, overlap/release, open/palm mute, resonant-note and octave line tests.
- PATTERN: разные аккорды дают разные корректные voicings; проверить смену аккордов, strum directions, accents, rests, BPM sync, stop/seek, no stuck notes.
- Сравнить отрендеренные звуковые варианты трёх A&A альбомных эпох и доказать слышимые отличия.
- Отдельно сравнить dry source и processed artist-inspired version.
- На каждый preset: stable ID, mode, source IDs, mappings, articulation, gear/tone research notes, FX parameters, license/provenance, SHA-256 и audio-test evidence.
- Не ставить READY без проверяемого source map, законного происхождения, правильного MIDI поведения и аудиопроверки.

## 7. Факты и гипотезы
Подтверждённые интервью-ориентиры: DeLonge говорит о резонирующих нотах/дополнительном движении за риффом и об echo как важной части изменения ощущения A&A; интервью по *I-Empire* описывает chiming upper-frequency elements, delay/flanger/compression и stereo echo; материалы *Lifeforms* дают контекст более сырого/широкого гитарного диапазона и отдельных упоминаний инструментов/усилителей.
Точные настройки каждого трека, конкретная цепь для каждой дорожки и окончательные значения DSP параметров требуют дальнейшего track-by-track исследования. Gear aggregators и tone sites — вторичные отправные точки, не окончательное доказательство.
