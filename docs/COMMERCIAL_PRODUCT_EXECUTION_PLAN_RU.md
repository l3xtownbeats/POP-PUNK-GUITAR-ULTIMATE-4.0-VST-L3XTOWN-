# PPGU — коммерческий план доведения продукта до уровня профессионального инструмента

Дата: 12 ноября 2026  
Ветка: `work/ppgu-toolchain-qa-rev17-1`  
Статус: PRODUCT EXECUTION BASELINE — требования и аудит; не заявление о готовности.

## 1. Цель продукта

POP PUNK GUITAR ULTIMATE должен ощущаться как законченный профессиональный sample-based guitar instrument: музыкально убедительный звук сразу после выбора пресета, быстрый браузер библиотеки, предсказуемое переключение инструментов, выразительная игра с MIDI-клавиатуры и готовые гитарные исполнения в PATTERN.

Основные функциональные ориентиры:
- **Session Guitarist / Electric Sunburst Deluxe** — специализированная гитарная игровая механика, реалистичные артикуляции, исполненные ритмы/бои, адаптация к аккордам, музыкальные переходы и понятное разделение MELODY/PATTERN.
- **NEXUS 5** — цельный preset-first workflow: пресет сразу вызывает согласованный звук/набор ресурсов, быстрый поиск, категории, preview, предсказуемое состояние и готовые production-звуки.
- **Omnisphere** — глубина библиотеки, продуманная организация источников, вариативность тембров и выразительные комбинации звуков/слоёв там, где это оправдано.

Это ориентиры пользовательского опыта и уровня качества, не разрешение копировать исходный код, интерфейсные assets, proprietary content, sample recordings, performances или factory presets этих продуктов. PPGU должен иметь собственную идентичность, собственные легально распространяемые записи и собственный sound design.

## 2. Единая модель: Preset + SoundBank + Performance

Пресет — не просто набор FX-параметров. Это единый загружаемый инструментальный state, который связывает:
- sound source(s) из Factory/User SoundBank;
- реальные key/root-note, velocity, articulation и round-robin mappings;
- профиль инструмента и режим игры;
- для PATTERN — performance phrase/loop/strum assets и их chord/tempo metadata;
- FX chain и mix settings, включая полностью сухой SOURCE/DRY путь;
- динамику, release/tail, voice/polyphony и playback options.

После выбора пресета runtime должен подготовить ресурсы вне audio callback, показать статус, атомарно опубликовать полностью готовое состояние и не заменять предыдущий рабочий звук до готовности нового. Preview и реальная игра используют один и тот же разрешённый preset state и playback route.

Новые WAV и Imported Guitar — равноправные локальные sound sources после валидации и корректного mapping. Происхождение файла не является основанием для запрета использования в пресетах. Право локального использования отделено от права включать файл в factory installer/distribution.

## 3. MELODY — живой полифонический гитарный инструмент

- Одна клавиша = одна mapped гитарная нота; одновременные клавиши = независимые ноты/аккорд. Полифония является штатным поведением, если пользователь явно не выбрал отдельный mono/legato mode.
- Никаких автоматических PATTERN loops или chord-pad замен в MELODY.
- Реальные multisamples по регистрам, velocity layers, alternate takes/round robin и articulations — только там, где исходная библиотека действительно содержит эти данные.
- MIDI Note On запускает записанную атаку; удержание сохраняет естественное sustain/loop; Note Off корректно запускает release/tail. Не бесконечный звук и не резкий обрыв.
- Velocity должна выбирать записанную динамику/артикуляцию при наличии слоёв; иначе плавно управлять громкостью без ложных обещаний новых recorded layers.
- Поддерживать управление voice count, корректный voice stealing, sustain pedal/CC, pitch bend и all-notes-off. Поведение документировать и тестировать.
- Чистые bank presets и SOURCE/DRY presets должны играть через MELODY без обязательных FX.
- Не растягивать один сэмпл по всему диапазону; транспозиция допускается только в музыкально и технически валидных пределах mapping.

## 4. PATTERN — настоящие гитарные исполнения, а не ритм-генератор из нот

Цель — механика Session Guitarist: пользователь удерживает/меняет MIDI-аккорд, а инструмент исполняет выбранный гитарный стиль реальными записями.

PATTERN должен использовать специально записанные или лицензированные performance assets:
- down/up strums, palm-muted eighths/sixteenths, open power chords, dead/mute strokes, ska offbeats, arpeggios/picking, octave hooks, accents, stops, fills, endings;
- разные динамики, позиции/voicings, варианты исполнения и alternate takes;
- musical metadata: chord type/voicing, root/bass, key/range, tempo range, meter, groove, phrase length, start/loop/end markers, articulation, velocity/dynamics, transition compatibility;
- chord adaptation должна выбирать валидные записанные voicings/performances, а не бездумно транспонировать один MIDI-паттерн или один сэмпл;
- host BPM/PPQ/transport sync, quantization, swing/human timing, clean chord transitions, stop/hold/release и предсказуемое поведение при смене аккорда;
- Preview должен проигрывать фактический performance asset.

Scheduler отвечает за выбор, синхронизацию, очередность и переходы. Sampler/streaming engine воспроизводит реальные записанные аудиоматериалы. Генерируемые тестовые MIDI-события полезны для unit/integration QA, но не считаются готовым коммерческим PATTERN-контентом.

## 5. SoundBank и расширяемость

Pipeline: discover → validate → provenance/license + SHA-256 → technical inspect → map/index/cache → prepare → publish.

- WAV — основной поддерживаемый формат; фактические ограничения по sample rate, bit depth, channels и duration должны быть явными.
- SFZ поддерживается в пределах реализованного и протестированного parser subset; не заявлять полную совместимость без тестового покрытия.
- Любые другие форматы добавлять только через проверенный decoder/import abstraction.
- Разделять file format и instrument/performance category: electric/acoustic/nylon guitar, articulations, loops/performance phrases, future compatible categories.
- Mapping editor/auto-mapping должны показывать пробелы, перекрытия, неоднозначности и фактическое покрытие; не придумывать отсутствующие зоны.
- Ошибки missing/corrupt/unsupported изолировать; не терять предыдущую активную библиотеку/пресет.
- Preview и live playback используют одинаковые source/mapping/layer/FX resolution.
- Файловый I/O, decode, scan, hashing и mapping preparation — только вне audio callback. Никаких блокировок, ожиданий или динамических аллокаций в callback.

## 6. Preset Browser и пользовательский опыт

- Режимы MELODY и PATTERN фильтруют совместимые пресеты, но используют общую модель SoundBank.
- Search, категории, instrument type, style, articulation, clean/processed, favorites, recent, tags и понятный missing-content status.
- Preview, audition stop, previous/next, Save, Save As, Rename, Delete, Import/Export user presets/banks.
- Пресет показывает режим, тип гитары, исходный sound source/performance, сухой или обработанный маршрут и доступность ресурсов.
- При смене пресета: loading state, отмена устаревшего запроса, отсутствие щелчков/зависаний; предыдущий звук сохраняется до атомарной готовности нового.
- Preset switching не должен самовольно менять play mode, MIDI routing или несвязанные пользовательские настройки.
- Factory и User assets разделены; user assets не копируются в factory content без согласия/лицензии.

## 7. Звук и коммерческая планка

Звук строится от качественной записи, не от количества FX:
1. выбрать подходящий исходный записанный инструмент/исполнение;
2. использовать корректные mappings, динамику и артикуляции;
3. оставить SOURCE/DRY, если исходник уже звучит правильно;
4. добавлять FX, amp/cab/IR или layering только по музыкальной необходимости.

Не принимать: пластмассовый синтезаторный оттенок, повторяющийся machine-gun playback, плохие loop seams, грубую транспозицию, неестественные аккордовые переходы, чрезмерную обработку, фазовые проблемы, клиппинг, streaming underruns.

Для коммерческого factory content обязательны provenance, лицензия на распространение, SHA-256, technical screening и human listening QA. Автоматические тесты не заменяют прослушивание в реальном VST3/DAW.

## 8. Производительность и надёжность

- Быстрый cold start; preset preview не должен блокировать UI.
- Lazy loading, prefetch атак, bounded page cache, измеренный RAM budget, eviction без invalidating active voices.
- Предварительно выделенные voice/event buffers; безопасная передача immutable prepared state.
- Проверить voice lifetime/page leases при preset switch, bank reload, voice stealing, panic, worker exception и plugin shutdown.
- Измерить idle/playback CPU, peak/steady RAM, cold-start time, preset-switch latency, underruns и callback worst-case на целевом Windows x64/FL Studio 20 setup.
- Не добавлять «multicore» toggle, если реальная параллельная DSP-работа не реализована и не измерена.

## 9. Реальный аудит текущего кода на ветке REV17

Проверено в `Source/PluginProcessor.cpp`, `Source/PluginProcessor.h`, `Source/Performance/PatternScheduler.h`, `docs/MELODY_SESSION_GUITARIST_PLAYABILITY.md`, `presets/factory_bank_300.json`:

1. **MELODY:** processBlock напрямую передаёт MIDI Note On/Off в SamplerEngine; в коде есть ограничение числа голосов. Это правильная базовая архитектура для полифонической игры. Однако APVTS параметр `monoMode` объявлен со значением по умолчанию `true`, а в просмотренном PluginProcessor.cpp найдено только его объявление, не runtime применение. Это несогласованность контракта: необходимо решить и реализовать реальный mono/legato option либо убрать/переопределить его; штатный MELODY по умолчанию должен быть полифоническим.
2. **PATTERN — критический продуктовый gap:** текущий processBlock формирует `rhythmMasks` и процедурно создаёт MIDI Note On/Off события из удерживаемых MIDI-нот/аккорда. PatternScheduler синхронизирует эти события с транспортом. Это полезная scheduler/integration основа, но не является Session Guitarist-style playback записанных гитарных performance loops/phrases. Нельзя считать PATTERN коммерчески готовым, пока его runtime не переключён на реальную performance asset library.
3. **Factory presets:** `presets/factory_bank_300.json` содержит описания 300 целей каталога; сам файл прямо говорит, что определения не доказывают runtime playback. Отдельный readiness audit отмечает 150 Melody как ожидающие интеграции/mapping и 150 Pattern как ожидающие recorded performance library/engine. Количество записей не равно количеству готовых пресетов.
4. **SoundBank / preset resolution:** подтверждённый кодовый путь содержит SFZ/WAV sampler infrastructure, но полноценная единая связь всех Factory/User/Imported sources с каждым preset, одинаковый Preview/live resolver и полный end-to-end reload workflow требуют отдельного кода и QA.
5. **monoMode и Pattern gaps** — не косметика UI, а блокеры основной игровой механики.

Этот аудит ограничен указанными файлами и не заменяет полный code audit всех классов SamplerEngine, UI, bank ingestion и state persistence.

## 10. Очерёдность исполнения — не распыляться

### P0 — основа игрового инструмента
1. Зафиксировать единый runtime contract Preset → SoundBank source/mapping → Engine → Preview/Live.
2. Сделать MELODY штатно полифоническим; исправить/удалить неработающий monoMode contract; проверить attack/sustain/release и многоголосие.
3. Довести bank source resolution для factory/user/imported assets и dry playback.
4. Проверить Preview/live parity и preset save/reload.
5. Исправить критические RT/lifetime/resource issues, найденные тестами.

### P1 — настоящий PATTERN
1. Специфицировать performance asset format и chord/tempo/phrase metadata.
2. Подготовить легально распространяемую записанную performance library.
3. Интегрировать asset-driven performance player с текущим PatternScheduler и SamplerEngine.
4. Реализовать chord adaptation, voicing selection, transitions, fills/endings и transport sync.
5. Пройти музыкальную QA на реальных MIDI аккордах и в FL Studio 20.

### P2 — контент и браузер
1. Подключить 300 definitions к runtime schema и проверить каждый пресет по фактическому source coverage.
2. Создать новые сухие пресеты из подходящих исходников без искусственных FX и без дублирования.
3. Довести Import Guitar/Bank ingestion и пользовательские пресеты.
4. Реализовать browser search/filter/favorites/preview/save/import/export.

### P3 — коммерческая стабилизация
1. UI/skin parity, accessibility, keyboard navigation, tooltips, loading/error states.
2. Измерения производительности и stress tests.
3. Windows 10 x64 + FL Studio 20 validation, pluginval, CI, installer, license/attribution/hash package.
4. Release candidate, regression suite и документированный acceptance report.

## 11. Release gates

Продукт не объявляется коммерчески готовым до выполнения всех пунктов:
- все опубликованные factory presets реально загружаются и звучат;
- MELODY polyphony, note mapping, velocity, articulations, attack/sustain/release подтверждены;
- PATTERN воспроизводит реальные записанные performance assets и адаптируется к аккордам/темпу;
- dry presets обходят FX и сохраняют характер исходной записи;
- Factory/User/Imported assets корректно загружаются, сохраняются и восстанавливаются;
- Preview и live MIDI имеют одинаковый source/mapping/routing;
- нет audio-thread I/O/locks/allocations и подтверждена стабильность resource lifetime;
- human listening QA пройдена на целевой DAW;
- CI/build/pluginval/FL Studio 20 и лицензии/атрибуция подтверждены на точном release commit.

**Правило:** каждый цикл — ПРОВЕРКА → АНАЛИЗ → ИСПРАВЛЕНИЕ → ПОВТОРНЫЙ АНАЛИЗ → РЕЗУЛЬТАТ. В отчёте отделять implemented / tested / listened / unverified. Не выдавать документацию или каталог за готовый функционал.
