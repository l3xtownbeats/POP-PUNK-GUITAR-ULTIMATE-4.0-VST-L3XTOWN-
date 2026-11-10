# Отчёт цикла — расширение Sound Bank — 11.11.2026

## Цель
Расширить bundled Sounds Bank качественными гитарными библиотеками, не ограничиваясь прежними Meta Steel/Nylon и FreePats Clean/Distortion 1/2.

## Добавлено в production workflow
- Karoryfer Emilyguitar — 323 WAV, CC0-1.0.
- Karoryfer Shinyguitar — 846 WAV, CC0-1.0.
- Karoryfer Black And Green Guitars — 1,776 WAV, CC0-1.0.
- FreePats FSBS Direct DI — 122 WAV, CC0-1.0.
- FreePats FSBS Jazz — 135 FLAC, CC0-1.0.
- Прежние Meta Open Sound Steel/Nylon и FreePats Clean/Distortion 1/2 сохранены.

Количество файлов выше — подсчёт Git tree upstream, а не подтверждённое содержимое уже скачанного release. Workflow проверяет минимальные количества, наличие SFZ и LICENSE, удаляет Git metadata, создаёт SHA-256 для WAV + FLAC. Cache key обновлён, чтобы принудительно получить расширенный набор.

## Технические ограничения, выявленные при проверке
- SFZ parser PPGU не поддерживает полностью расширенные include, default_path variables, random-region semantics и часть управляющих/артикуляционных opcodes в отдельных upstream картах.
- SamplePool streaming playback пока WAV-focused. FLAC банк Jazz должен пройти пользовательский import/conversion path до использования в WAV-ориентированном runtime.
- Importer создаёт упрощённые root-note карты по именам файлов; автоматически не сохраняет все исходные velocity layers, round robins, string-position logic и articulations.
- Поэтому добавленные исходные банки можно распространять по CC0, но нельзя выдавать за полностью готовые и протестированные factory presets до следующей итерации SFZ parser и per-bank acceptance.

## Проверки
- Проверены upstream GitHub metadata: SPDX license CC0-1.0 для всех пяти добавленных репозиториев.
- Подсчитаны WAV/FLAC/SFZ и проверено наличие LICENSE в upstream trees.
- Изменён GitHub Actions workflow для скачивания, минимальных порогов, проверки license/SFZ, SHA-256 и упаковки.
- Запущенный CI run был отменён следующими последовательными commit push (concurrency cancel-in-progress); это не результат теста и не PASS.
- Новый финальный CI run требуется завершить без последующих push, затем проверить configure/build, CTest, pluginval и release ZIP contents.
- Звуковое прослушивание/слепое сравнение и ручной тест FL Studio 20 в этом цикле не выполнялись.

## Результат
Source set и упаковочный workflow расширены. Готовый VST3 installer не объявляется до успешного CI build и проверки опубликованного artifact.
