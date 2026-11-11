# PPGU — журнал разработки
Формат: хронологический журнал проверок, решений, изменений, сборок и тестов.
Правило ведения: каждая завершённая проверка или изменение фиксируется здесь с датой, веткой, commit SHA, статусом и фактическим результатом. Запланированное не отмечается как реализованное; сборка/тест не отмечаются успешными без подтверждённого run и логов.

---

## 2026-11-11 — Продолжение архитектурного аудита общего Audio Engine

**Ветка:** `main`  
**HEAD на момент аудита:** `855f21c00f967483903cd57a3896a3ba72cd260d`  
**Статус:** CODE AUDIT / DESIGNED. Изменений production-кода в этой записи нет.

### Проверено
- Последние коммиты main; последний — `855f21c`, архитектурное решение о семействе Guitar/Bass.
- `CMakeLists.txt`: один production JUCE target `PopPunkGuitarUltimate`; отдельного production target общего PPGU Audio Engine или Bass Edition пока нет.
- `Source/PluginProcessor.h/.cpp`: JUCE host adapter, APVTS, MIDI routing, sampler/pattern orchestration, FX buffers и state handling сосредоточены в Guitar processor.
- `Source/Sampler/BoundedSpscQueue.h`: фиксированная SPSC очередь; строго один producer и один consumer.
- `Source/Sampler/PageCacheSlots.h/.cpp`: фиксированный page cache с атомарным pin/release и worker-side eviction.
- `Source/Sampler/SampleMetadataIndex.{h,cpp}`, `WavPageReader.{h,cpp}`: WAV metadata inventory и page decoding.
- `Source/Sampler/SampleStreamingService.{h,cpp}`: background worker, bounded request queue, decoding и статистика.
- `Source/Sampler/StreamingVoiceReader.{h,cpp}`: per-voice leases, prefetch и Hermite interpolation.
- `Source/Sampler/SfzParser.{h,cpp}`: SFZ parser на стандартной библиотеке C++.
- `Source/Sampler/SamplePool.{h,cpp}`: SFZ regions, key/velocity/channel mapping, RR, streaming service ownership и immutable snapshots.
- `Source/Performance/PatternScheduler.h`: block clock/PPQ и bounded event scheduling.
- `Source/Performance/ChordTransition*`: гитарная логика переходов аккордов; не является общей басовой механикой.
- `Source/UI/ThemeManager.{h,cpp}`: визуальные palette manifests и fallback; отдельный UI utility, не аудиодвижок.
- Существующий документ семейства продуктов: `docs/PPGU_AUDIO_ENGINE_PRODUCT_FAMILY_RU.md`.

### Выводы аудита
1. Общие низкоуровневые компоненты уже существуют, но общего production Engine target пока нет.
2. SPSC очередь и streaming service нельзя безопасно разделять между несколькими audio producers без отдельного доказательства/переработки топологии.
3. SamplePool нельзя переносить целиком как нейтральный Engine: он смешивает streaming ownership с SFZ/SoundBank mapping и round-robin.
4. Guitar chord transition logic, Guitar APVTS IDs, Guitar UI и Guitar SoundBank policy остаются в Guitar Product Profile.
5. Bass Edition должен получить отдельные Product Profile, APVTS/state schema, артикуляции, SoundBank и контент; общий код — только механизмы.
6. Старые snapshots/services SamplePool сейчас удерживаются до уничтожения пула ради безопасности raw pointers. Reclamation нельзя менять без reader-quiescence/epoch или эквивалентной проверенной схемы.
7. Не следует делать массовый перенос файлов/targets до добавления boundary tests и подтверждения Windows Release build.

### Следующие инженерные шаги
- [ ] Проверить полный lifecycle SamplerEngine/voices и точные места смешения с SamplePool.
- [ ] Составить ownership/threading map: audio thread, loader worker, streaming workers, message thread; определить producer/consumer для каждой очереди.
- [ ] Выделить независимые C++ примитивы в отдельный внутренний target только после тестов, сохранив прежние публичные API Guitar.
- [ ] Разделить SFZ/SoundBank mapping от общих streaming/page-cache механизмов.
- [ ] Добавить архитектурные boundary tests, запрещающие зависимости общего Engine от Guitar/Bass Product Profile.
- [ ] Только затем менять CMake targets и проверять Windows x64 Release, CTest и pluginval.
- [ ] После подтверждённого CI run записать SHA, run ID, все jobs, CTest/pluginval и ссылки на артефакты.

### Что НЕ выполнено этой записью
- Общий PPGU Audio Engine не выделен в отдельную библиотеку/target.
- Bass VST не создан.
- BankScanner/SB1 не реализован.
- Новая сборка не запускалась и не подтверждалась.
- Host verification в FL Studio 20 не выполнялась.
- Производительность CPU/RAM не измерялась.

---

## Правило следующих записей
Каждый следующий этап добавлять новой датированной секцией, не переписывая историю. При изменении исходников указывать файлы и commit SHA; при сборке — workflow/run ID, commit SHA, итог каждого job и артефакты; при ошибке — точный failing step и краткую причину.