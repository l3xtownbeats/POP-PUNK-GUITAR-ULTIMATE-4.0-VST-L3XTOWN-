# Аудит интеграции POP PUNK GUITAR ULTIMATE — 10.11.2026

## Область проверки

Проверен отдельный source-review artifact из GitHub Actions run 42704820544:
https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE/actions/runs/42704820544

Artifact: POP-PUNK-GUITAR-SOURCE-REVIEW (ID 13713421947, SHA-256 d9fff08ed05786ad43d6752a1de325763eb8c8e610589bde4440a3efb8701c68).
Внутренний архив содержит исходники JUCE-проекта версии 3.1.0. Это отдельная реализация UI и sample-bank routing, а не текущая ветка M2 SamplePool.

## Подтверждённые несоответствия

### 1. Два разных звуковых движка пока не интегрированы

- Ветка M2 содержит отдельный SFZ parser и SamplePool/SamplerEngine: WAV, MIDI note/velocity/channel zones, group/off_by metadata, offset/end, release, loop modes и voice pool.
- Source bundle 3.1.0 содержит другой JUCE Synthesiser engine: WavZoneSound/WavZoneVoice и собственный упрощённый SFZ parser внутри PluginProcessor.cpp.
- Эти реализации имеют разные API и разные ограничения. Успешная сборка M2 не подтверждает работу GUI из source bundle; pluginval для source bundle не подтверждает работу M2 SamplePool.

**План:** объединить один runtime engine и один SFZ parser; не поддерживать две независимые реализации.

### 2. SFZ-парсер source bundle не совместим со всем форматом карт Meta/FreePats

В source bundle applySfzOpcode обрабатывает только sample, lokey, hikey, key, pitch_keycenter, lovel, hivel, tune и volume. Теги <master> не применяют channel ranges. Не обрабатываются loop_mode/loop_start/loop_end, lochan/hichan, group/off_by и round-robin metadata.

M2 parser поддерживает больше нужных опкодов, но SamplePool пока принимает только WAV. Поэтому интеграция библиотек должна идти через M2 parser либо через проверенный конвертер карт, а не через старый ограниченный parser.

### 3. Sample package не совпадает с путями, которые ищет source bundle

Source bundle getSampleRoot() использует:
%APPDATA%/PopPunkGuitarUltimate/Samples

Для акустики он ищет SFZ только внутри:
%APPDATA%/PopPunkGuitarUltimate/Samples/SourceLibraries

Для электрогитары он ищет WAV по фиксированным путям:
Samples/Electric/Open/C1.wav ... C8.wav
Samples/Electric/PalmMute/C1.wav ... C8.wav

Workflow vst3-build.yml собирает папку Samples с каталогами Meta-meta-steel-guitar, Meta-meta-nylon-guitar, FreePats-Clean, FreePats-Distortion-1 и FreePats-Distortion-2. Он сохраняет карты и WAV, но не устанавливает их в ожидаемую структуру SourceLibraries/Electric/Open/Electric/PalmMute и не копирует их автоматически в AppData.

Следствие: наличие Samples в ZIP ещё не доказывает, что source bundle найдёт и загрузит эти файлы. FreePats SFZ-карты также не подключаются к электрическому загрузчику, который ищет отдельные WAV с именами MIDI-нот.

**План:** выбрать единый формат library manifest и сделать install/import workflow, который раскладывает карты и сэмплы в логические профили. Проверять реальную загрузку и note/velocity coverage, а не только наличие файлов.

### 4. Каталог 300 пресетов не равен 300 работающим factory presets

- Отдельный JSON factory catalog проверяется скриптом validate_factory_bank.py.
- Source bundle UI содержит 57 статических записей пресетов после workflow-вставки дополнительных строк.
- В просмотренном source bundle нет runtime-кода, который читает factory_bank_300.json и превращает все его 300 записей в загружаемые UI-пресеты.
- У processor getNumPrograms() возвращает 1; это также не является реализацией банка 300 пресетов.

**План:** подключить JSON catalog к реальному browser/state loader, проверить каждый preset на доступный sampleProfile и реальное воспроизведение. Только после этого считать пресет частью готового банка.

### 5. Pattern mode отсутствует в source bundle

В UI source bundle есть preset browser и Melody-style MIDI note playback. В processor каждый MIDI note запускает семплированную ноту или fallback; нет отдельного Pattern scheduler, аккордовой карты, down/up strum performance engine, host tempo/transport scheduling и записей chord/strum performances.

**План:** сначала завершить Melody multisample workflow; Pattern разрабатывать как отдельный performance engine с отдельными данными аккордов и strum events. Одиночные ноты не считать заменой pattern recordings.

### 6. Перезагрузка sample bank может давать аудиопровал

В source bundle reloadSampleBankSafely вызывает suspendProcessing(true), затем держит samplerLock во время полной загрузки WAV/SFZ. processBlock использует tryEnter(); если lock занят, он очищает output buffer и возвращает тишину. Это избегает блокировки audio callback, но не выполняет требование бесшовной смены библиотеки.

Кроме того, loadSampleBank полностью очищает текущие Synthesiser sounds/voices и заново декодирует выбранные WAV в память.

**План:** подготовка новой библиотеки на worker thread, затем атомарная публикация immutable engine snapshot; старый snapshot освобождать только после безопасного audio-thread grace period. Добавить измерения времени загрузки, RAM и тест аудио без пропусков при переключении.

### 7. Нагрузочные риски

- Source bundle eagerly декодирует найденные WAV в RAM; нет подтверждённого lazy loading/prefetch/cache budget для сотен сэмплов.
- В processBlock создаётся локальный MidiBuffer electricMidi и для каждой входящей MIDI event создаётся/добавляется event. Нужно проверить аллокации на аудиопотоке.
- Встроенная обработка source bundle включает Drive/Tone/Reverb starter DSP; это не полная amp/cab, chorus, flanger, delay и distortion chain.
- Нет опубликованных измерений cold-start, preset-switch latency, steady/peak RAM, CPU или аудиоглитчей.

**План:** профилирование и аудио-thread allocation audit до оптимизаций. Не заявлять multiprocessing без безопасной параллельной реализации и измеримого выигрыша.

## Что реально подтверждено в параллельном Windows workflow

Run 42704820544 прошёл этапы:
- factory JSON catalog validation;
- portable preset examples validation;
- x64 VST3 build;
- pluginval strictness 5;
- packaging VST3 + Samples.

На момент этого аудита upload итогового package artifact ещё выполнялся. Даже после успешной публикации этот workflow подтверждает сборку source_bundle.zip и pluginval для source bundle 3.1.0, но не интеграцию M2 SamplePool и не FL Studio 20 host test.

## Приоритеты объединения

1. Закрепить один актуальный processor/editor source и один sampler runtime.
2. Подключить M2 SamplePool к реальному MIDI processing.
3. Подключить SFZ/WAV sample browser и library profile manifests.
4. Переложить Meta Steel/Nylon в корректную структуру и проверить все карты/сэмплы.
5. Подготовить законно распространяемые электрические pop-punk записи; FreePats clean/distorted — только после проверки фактической совместимости карт и маппинга.
6. Подключить factory JSON catalog к runtime browser; проверить 300/300 загружаемых пресетов.
7. Реализовать и протестировать Pattern performance engine.
8. Убрать аудиопровалы при reload, провести concurrency, allocation, CPU/RAM и long-playback tests.
9. pluginval strictness 5 и отдельная проверка FL Studio 20 на Windows 10 x64.

## Статус

Это аудит кода и структуры пакета, не результат ручного прослушивания. Никакие отсутствующие runtime-функции не помечаются как готовые. Дальнейшая интеграция должна сопровождаться отдельными CI gates и фактической проверкой в FL Studio 20.


## Дополнение — фактическое состояние M1–M5 на 10.11.2026

- Windows x64 Release run `42841128991`: build, CTest, VST3 locate, pluginval strictness 5 и packaging/upload SUCCESS. Artifact опубликован. Отдельный MSVC M1/M2 run `42841133299`: SUCCESS.
- M1 остаётся без ручной FL Studio 20 validation.
- M2: bounded streaming/RR/cache tests подтверждены CI; velocity crossfade DSP, click/pop acceptance, palm-mute на реальном sample bank и stress validation остаются открытыми.
- M3: unit/regression scheduler CI прошёл; end-to-end scheduler-to-sampler/DAW MIDI validation остаётся открытым.
- M4 добавлен изолированно в PR #4 (AutoChords/FingeringGenerator + tests), CI run `42842050531` ожидает завершения build на момент записи.
- M5 Style Engine + tests прошёл run `42840410645`; runtime Pattern integration ещё не выполнена. PR #3 исходно направлен в main и должен быть перенесён на интеграционную ветку, а не слит напрямую.
- Полная таблица evidence, blockers и merge sequence: `docs/WORK_LOG_M1_M5_INTEGRATION_RU.md`.

## Дополнение — результаты и открытые пункты

- M3 теперь имеет автоматический module-level integration test scheduler→sampler; Windows run `42843030370` подтвердил build, full CTest, pluginval strictness 5 и packaging.
- M2 velocity crossfade DSP находится в отдельной ветке PR #5 и ждёт собственного Windows CI. В основной ветке это изменение пока не интегрировано.
- M4 algorithmic AutoChords/FingeringGenerator foundation прошёл CI; последние overflow guards ждут повторной проверки.
- M5 StyleEngine foundation прошёл CI; runtime pipeline/UI integration отсутствует.
- Ни один из этих результатов не заменяет ручную проверку FL Studio 20. Подробная схема M2 описана в `M2_VELOCITY_CROSSFADE_DESIGN_RU.md`.
