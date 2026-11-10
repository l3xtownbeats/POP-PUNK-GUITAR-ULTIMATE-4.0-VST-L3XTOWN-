# M1–M3 — актуальный release-gate аудит

Дата проверки: 2026-11-10. Ветка: `fix/m1-m2-m3-release-gates`. Основа: commit `065b8425b3e8f356f0b73a35c710e89d9c2d741c`.

Важно: старые выводы предыдущего аудита о полном отсутствии streaming и scheduler dispatch не соответствуют этой migration-ветке. Этот документ заменяет устаревшие утверждения, но сам по себе не является подтверждением прохождения тестов.

## M1 — VST3 / lifecycle / состояние

### Уже присутствует в исходниках
- APVTS состояние сериализуется в XML; путь SFZ сохраняется как `sfzFilePath` и восстанавливается через `setStateInformation()`.
- Асинхронная загрузка использует однопоточный ThreadPool и generation token: устаревшие запросы пропускаются до сканирования и не публикуют результат.
- Деструктор инвалидирует generation token и дожидается завершения worker jobs.
- File chooser и completion callback редактора используют `juce::Component::SafePointer`.

### Ещё не подтверждено
- Сохранение/повторное открытие проекта и отсутствующая библиотека в FL Studio 20 x64.
- Host automation всех APVTS параметров, смена аудиоустройства, prepare/release циклы, закрытие редактора во время загрузки.
- Параллельная последовательная смена банков и корректность результата при гонке с восстановлением состояния.
- Реальный Windows 10 1809 / низкоресурсный компьютер и длительное воспроизведение.

## M2 — SamplePool / streaming runtime

### Уже присутствует в исходниках
- Production SFZ load создаёт metadata-backed streaming regions.
- `SampleStreamingService`, `PageCacheSlots` и `StreamingVoiceReader` подключены к `SamplerEngine::noteOn()` / `render()`.
- Голос запрашивает attack/current pages, читает через pinned page leases и не выполняет файловый I/O или ожидание cache miss на audio thread.
- На cache miss sample cursor не продвигается; запрос страницы передаётся worker service.
- Текущая streaming cache ограничена фиксированным числом страниц, а не размером полного декодированного WAV.

### Оставшиеся риски/проверки
- `maximumSnapshots = 128` и `maximumStreamingLibraries = 8` — реальные ограничения, не удалять до безопасного reclamation/lifetime решения.
- Нужны stress/regression тесты: частые bank reload при активных голосах, pinned-page release, cache pressure, worker/audio concurrency, несколько корней библиотек, shutdown во время запросов.
- Проверить все поддерживаемые WAV encodings и границы страниц/loop при реальном playback.
- Доказать отсутствие allocations, locks и disk I/O в audio callback инструментально или через специализированные тесты.

## M3 — transport / Pattern scheduler

### Уже присутствует в исходниках
- `PluginProcessor::processBlock()` вызывает `beginBlock()` и `schedule()`.
- Scheduled events передаются в sampler как NoteOn/NoteOff с sample offsets.
- Есть transport stop/discontinuity обработка, очистка pending NoteOffs и отдельный учёт Pattern-owned held notes.
- Unit tests покрывают host/internal clock, PPQ/sample conversion, long NoteOff, wrap, malformed tempo/PPQ и discontinuities.

### Ещё не подтверждено
- End-to-end integration tests scheduler → sampler с точными sample offsets.
- Отсутствие влияния Pattern stop/panic на обычные Melody-held notes во всех режимах.
- FL Studio 20: play/stop, loop, seek, tempo changes, смена play mode/slot и одновременный MIDI chord input.

## Порядок release gate
1. M1/M2/M3 regression и concurrency тесты на текущем коде без повторного внедрения уже существующих функций.
2. Windows x64 Release build + полный CTest.
3. Подтверждённый VST3 bundle artifact.
4. pluginval strictness 5.
5. Ручная проверка FL Studio 20 x64 на Windows 10: проект save/reopen, automation, transport, длительный playback и смена аудиоустройства.

**M1–M3 не объявлять закрытыми, пока пункты 2–5 не подтверждены.** CI не заменяет реальный тест в FL Studio.

## CI evidence update — 10.11.2026

- Main Windows x64 Release run `42843030370` on `4d13943f15352b87f3569524e2e5b4b8f9bbafda`: SUCCESS for configure, VST3/test build, full CTest, bundle locate, pluginval strictness 5, package and upload.
- Additional MSVC v143 run `42843030228`: SUCCESS.
- M3 now has a module-level scheduler→SamplerEngine integration test in `tests/PatternSamplerIntegrationTests.cpp`; CTest confirms sample-accurate NoteOn/NoteOff dispatch and audible output.
- M1 FL Studio 20 state/automation/library/device/long-playback validation remains OPEN.
- M2 velocity crossfade playback is being validated separately in PR #5/run `42843931173`; palm-mute mapping, click/pop and concurrency stress remain OPEN.
- M3 FL Studio transport/MIDI validation remains OPEN.
- M4 initial CI passed run `42843127831`; latest integer-overflow hardening needs a fresh run. M5 foundation passed run `42840410645`; runtime integration remains OPEN.
- Do not mark M1–M5 complete or merge secondary branches until all current heads and the unified integration build pass.
