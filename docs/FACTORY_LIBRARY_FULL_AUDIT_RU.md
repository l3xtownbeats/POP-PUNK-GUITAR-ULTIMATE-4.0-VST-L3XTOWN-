# Полный аудит заводской библиотеки пресетов PPGU 5.0

Дата: 11 ноября 2026  
Проверенная ветка: `work/integrate-uploaded-theme-manager`

## Что проверено непосредственно в исходниках

- `presets/factory_bank_300.json`: 165 MELODY + 165 PATTERN = 330 записей.
- 330 уникальных ID; 22 исполнителя; все карточки имеют категорию браузера.
- У каждой карточки есть числовые drive/tone/reverb и заданный звуковой характер.
- MELODY содержит `sampleProfile` и `noteBehavior`.
- PATTERN содержит groove и performance metadata, но ранее не имел `sampleProfile`.
- В исходном редакторе каталог читается из встроенного JSON и отображается в браузере.
- `applyFactoryPreset()` применяет только ограниченную часть параметров: distortion drive, tone, reverb mix, output, условный pattern slot, articulation и on/off отдельных эффектов.
- Пресет не инициирует автоматическую загрузку соответствующего SFZ/банка. Реальный SFZ сейчас выбирается через IMPORT SOUNDS.
- В репозитории нет Samples/ с исходными WAV: папка создаётся CI при скачивании банков. Поэтому наличие sample-set в JSON не доказывает, что библиотека уже доступна пользователю в установленном плагине.

## Изменения в этом проходе

1. Для всех 330 карточек добавлены `sampleSetId`, `articulationProfile` и явный `runtimeBindingStatus`.
2. Всем 165 PATTERN карточкам назначен `sampleProfile` (electric/acoustic) и требование к соответствующему performance/voicing asset.
3. Добавлен реестр целевых банков:
   - `meta-nylon-guitar` — акустический nylon;
   - `freepats-electric-clean` — чистая электрогитара;
   - `freepats-electric-dist1` — средний drive;
   - `freepats-electric-dist2` — высокий drive;
   - `meta-steel-guitar` — разрешённый акустический набор для будущего выбора.
4. Добавлен `tools/audit_factory_library.py`: строгая проверка 330 карточек, режимов, ID, browser metadata, sample targets, articulation и performance metadata; с `--samples-root` проверяет физические папки, SFZ, WAV и license-файлы.
5. Windows VST3 workflow теперь запускает эту проверку после скачивания и проверки sample banks, до сборки/упаковки.

## Блокеры до статуса 100% рабочей библиотеки

### A. Runtime sample binding — критический
Каталог знает целевой sampleSetId, но UI/runtime не разрешает этот ID в SFZ-путь и не загружает его автоматически при выборе пресета. Поэтому карточка может менять часть тембровых параметров, но не гарантирует правильный источник звука.

### B. Sample format / SFZ semantics — критический
Встроенный потоковый движок читает WAV. SFZ parser поддерживает только ограниченный набор opcode. Meta Nylon использует keyswitch/crossfade semantics, которые в clearance record прямо отмечены как неподдерживаемые. Нельзя считать Nylon полностью проверенным по факту наличия CC0 WAV.

### C. PATTERN performance — критический
Текущий scheduler умеет планировать MIDI-события, но наличие groove/palmMuteMap/accentMap в JSON не доказывает, что каждый рисунок исполняется как записанная гитарная партия. Для Session-Guitarist-подобной реалистичности нужны реальные chord/strum performance assets с темпом, направлением удара, voicing, артикуляцией и переходами. Одни одиночные ноты не заменяют такие записи.

### D. Артикуляции и velocity
В JSON артикуляции описаны, но это не означает, что каждый SFZ содержит реальные отдельные velocity layers, round robins, release samples, palm mute, harmonic и tremolo. Для каждого банка нужна проверенная карта зон и тесты note-on/note-off.

### E. Проверка в DAW
Автоматическая сборка, CTest и pluginval не подтверждают, что все 330 пресетов звучат корректно в FL Studio 20. Нужна ручная проверка установленного VST3, Piano Roll, переключения режимов, chord changes, транспорта, сохранения состояния и всех групп пресетов.

## Текущий честный статус

- Каталог и структура связей: проверяемы автоматически.
- Sample target metadata: добавлена для всех 330.
- Физические банки: workflow загружает CC0 банки и проверяет папки/лицензии; результат конкретного запуска CI ещё требуется.
- Автозагрузка sampleSetId по выбору пресета: НЕ завершена.
- Полная реализация 330 уникальных звуков: НЕ подтверждена.
- Полная реализация 165 pattern performance: НЕ подтверждена.
- FL Studio 20 acceptance: НЕ выполнена в рамках этого аудита.

**Запрет на ложный PASS:** успешный каталог-аудит не переименовывается в «330 готовых звуков». Статус 100% разрешён только после runtime sample binding, покрытия SFZ, реального playback и host acceptance.

## Следующие технические этапы

1. Реализовать SampleSetRegistry в runtime: ID → установленная папка → SFZ, проверка лицензии/manifest, асинхронная загрузка, fallback и отчёт об отсутствующих файлах.
2. Переделать FactoryPreset runtime model так, чтобы все аудио-параметры, sampleSetId и articulation применялись целиком, а не только четыре тональных параметра.
3. Реализовать и проверить отдельную модель исполнения PATTERN; не выдавать эвристический слот scheduler за 165 уникальных performance.
4. Добавить automated tests на каждый preset ID: resolve → load → note-on/off → non-silent output → release; для Pattern: chord change, tempo sync, stop/seek, note-off.
5. Собрать Windows x64 VST3, прогнать CTest и pluginval 5, затем выполнить FL Studio 20 acceptance.

