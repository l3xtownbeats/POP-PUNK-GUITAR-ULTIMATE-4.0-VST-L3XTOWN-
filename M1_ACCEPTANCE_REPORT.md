# M1 — Каркас: отчёт по v33 FINAL

Статус: **Windows x64 Release VST3 собран и опубликован; пользователь подтвердил загрузку и работу плагина в FL Studio 20; остальные host-проверки M1 выполняются по отдельности.**

## Что реализовано

1. Структура проекта и репозитория.
2. CMake 3.22+, C++20, JUCE 7.0.12 через FetchContent.
3. VST3-only target, Windows x64 / MSVC v143.
4. Post-build копирование VST3 bundle через штатный механизм JUCE `COPY_PLUGIN_AFTER_BUILD` + `VST3_COPY_DIR`.
5. `PluginProcessor` с временным MIDI sine pass-tone.
6. APVTS layout со стабильными ID для глобальных, Melody, Pattern, Style DNA и семи FX-слотов/параметров.
7. Сериализация и восстановление APVTS state.
8. Скелет MIDI panic: All Notes Off / All Sound Off запускает 30 ms fade.
9. Отдельный Windows GitHub Actions workflow для сборки, проверки bundle и публикации артефакта.
10. Статический аудит APVTS ID: проверяет дубликаты, ключевые контракты и генерацию 7 FX-слотов.
11. README с командами сборки и ручным host-checklist.

## Исправление найденной ошибки CI

- **Симптом:** MSVC остановился на `C1083: Cannot open include file: 'JuceHeader.h'`.
- **Корень:** проект включал compatibility header, но CMake не вызывал `juce_generate_juce_header`.
- **Фикс:** добавлен вызов `juce_generate_juce_header(PopPunkGuitarUltimate)` после `target_link_libraries`, чтобы генератор видел подключённые JUCE-модули.
- **Повторная проверка:** Windows workflow `42699834793` на commit `6dcf93dbe7e23b380fa2abb5a2d3c477a30aeeaf` завершился `success`. Полный лог подтверждает компиляцию MSVC, создание VST3 bundle и публикацию артефакта `PPGU-v33-M1-Windows-x64-VST3` (artifact ID `13709809766`, SHA-256 `7a4293bf2c2a11d4564321d2f190f78a4d4182c597558be5e605107c7a285008`).

## Проверки и доказательства

- [x] CMake configure на Windows runner — успешно.
- [x] Корень ошибки `JuceHeader.h` подтверждён реальным MSVC логом.
- [x] Исправление добавлено и отправлено отдельным Git-коммитом.
- [x] Статический аудит: 84 статически объявленных уникальных APVTS ID, обязательные M1 ID присутствуют, генерация 7 FX slots обнаружена.
- [x] Чистая Windows x64 Release сборка после исправления (GitHub Actions / MSVC v143).
- [x] VST3 bundle создан; бинарный модуль найден в `Contents/x86_64-win/POP PUNK GUITAR ULTIMATE.vst3` (3,769,344 байта).
- [x] Артефакт VST3 опубликован в Actions (1,679,316 байт; хранится до 2026-12-09).
- [x] Загрузка и работа плагина в FL Studio 20 — подтверждено пользователем.
- [x] Базовый MIDI pass-tone: пользователь подтвердил звук при нажатии и прекращение звука при отпускании одной ноты. Перекрывающиеся ноты и длительный MIDI stress-test отдельно не подтверждены.
- [ ] Проверена автоматизация параметров в FL Studio 20.
- [ ] Проверено сохранение/восстановление проекта и APVTS.
- [ ] Проверен 30 ms MIDI panic fade в хосте.

## Критерий приёмки M1

**ЧАСТИЧНО ВЫПОЛНЕН.** Компиляция, bundle, публикация и факт загрузки/работы в FL Studio 20 подтверждены. Полная приёмка M1 ожидает отдельной проверки MIDI-поведения, автоматизации, state и panic fade. Отсутствие доступа к локальному FL Studio нельзя выдавать за успешный тест.

## Границы M1

Sine pass-tone — только проверка MIDI → audio пути. Это ещё не гитарный сэмплер и не финальный звук. Сэмплер, SamplePool, RR, VoiceManager, PatternScheduler, DSP-эффекты и GUI не реализуются в M1. Переход к M2 запрещён до выполнения критериев M1.
