# M1–M3 — аудит перед допуском к M4

Дата аудита: 2026-11-10. Ветка: `feat/m3-transport-scheduler-v2`.

Этот документ фиксирует проверенные по исходникам блокирующие дефекты. Наличие исходника или теста не считается подтверждением прохождения теста/сборки/проверки в DAW.

## M1 — VST3 / жизненный цикл / состояние

### Подтверждено по исходникам
- JUCE VST3 target x64 и параметры APVTS объявлены в CMake/PluginProcessor.
- Есть state serialize/restore через ValueTree XML.
- Есть async SFZ load через ThreadPool, а destructor ожидает завершения jobs.

### Не подтверждено
- Реальная установка и сканирование в FL Studio 20 x64 на Windows 10 1809.
- Сохранение/повторное открытие проекта в FL Studio 20, автоматизация каждого параметра, смена аудиоустройства, длительный playback.
- Проверка на целевой машине 2-core / 2 GB RAM / NVIDIA 900M 1 GB.
- Cold start, preset switch latency, CPU/RAM и отсутствие dropouts.

### Риски/дефекты для закрытия
- В state сохраняются APVTS параметры, но выбранная SFZ-библиотека/логический library ID не сохраняется и не восстанавливается.
- Async loader API не отменяет устаревшую задачу при последовательной смене банков; завершившаяся более поздно старая загрузка может опубликовать не тот банк.
- Callback completion вызывает MessageManager::callAsync; необходимо проверять закрытие editor/processor и lifetime получателя callback.

## M2 — SamplePool / sampler runtime

### Подтверждённые ограничения
- В SamplePool.cpp есть жёсткий лимит 120 секунд на один WAV.
- Декодирование всей записи в RAM; поддерживается только WAV.
- Жёсткий лимит 1 GiB decoded storage на экземпляр.
- Жёсткий лимит 128 snapshots на экземпляр; после него требуется перезапуск.
- Каналы источника сводятся максимум к двум.
- RegionView и позиции воспроизведения используют int frame indices.
- Все sample assets и snapshots удерживаются до уничтожения SamplePool для безопасности сырых указателей голосов; нет reclamation старых snapshots.
- Нет streaming-подключения к голосам: WavPageReader/PageCacheSlots пока изолированы от SamplerEngine.
- Нет полного набора тестов на 24/32-bit PCM, float WAV, mono/stereo, большие файлы, большое число reload, RR, velocity crossfade, click-free stealing и стресс 100+ нот.

### Архитектурное решение
- Не удалять лимиты простым удалением констант: при полном декодировании это позволит неконтролируемо исчерпать 2 GB RAM.
- Убрать произвольные лимиты из логической модели библиотеки, но перенести ограничение ресурсов в потоковый runtime: metadata-only bank index, page-based reads, фиксированный RAM cache, pinned pages и управляемая очередь запросов.
- Для пользовательского импорта принимать исходные форматы через отдельный import/convert pipeline; нормализовать в внутренний формат (PCM WAV/float cache + SFZ/manifest), не пытаться читать все возможные форматы непосредственно в аудиопотоке.
- Не использовать слово «без ограничений» как обещание бесконечной памяти: ограничение должно быть физическим (диск/RAM/cache budget), настраиваемым и безопасным.

## M3 — transport / Pattern scheduler

### Подтверждено
- PatternScheduler имеет fixed-size event array, host/internal clock, PPQ/sample conversion, quantize и pending NoteOffs.
- Unit test покрывает базовые host/internal сценарии и длинный gate.

### Блокирующий дефект
- В PluginProcessor::processBlock() вызывается beginBlock(), но не вызывается PatternScheduler::schedule().
- Ни один ScheduledEvent не отправляется в SamplerEngine; параметр playMode не меняет обработку MIDI на Pattern.
- Pattern mode не является рабочим исполнением и не должен считаться завершённым.
- Host transport stop вызывает sampler.allNotesOff(), что снимает и обычные Melody notes; поведение нужно разделить по источнику нот.
- Нет доказательства обработки seek/loop jumps, tempo changes within playback, host invalidation и совпадающих NoteOff/NoteOn при реальном DAW playback.

## Порядок закрытия
1. M2 streaming voice data path: page requests → worker decode → pinned page cache → voice render; no allocation/I/O/locks on audio thread.
2. SamplePool refactor to metadata-backed sample assets and versioned immutable map; remove 120 s / 1 GiB / 128-snapshot artificial caps only after streaming and safe lifetime management are active.
3. Async bank load generation token/cancellation and stable library identity in DAW state (M1).
4. M3 scheduler dispatch and Pattern/Melody source separation; do not invent musical patterns where no performance data exists.
5. Regression tests + Windows x64 Release build + CTest.
6. Host validation in FL Studio 20 x64 on Windows 10 1809 and low-spec target machine. This requires actual machine/DAW run; CI cannot substitute for it.

## Release gate
M1–M3 считаются закрытыми только после успешной Windows сборки и CTest, подтверждённого VST3 bundle artifact, прохождения тестов аудиожизненного цикла и реальной проверки FL Studio 20. После этого M4+ можно считать допущенными к полноценной интеграции, не раньше.
