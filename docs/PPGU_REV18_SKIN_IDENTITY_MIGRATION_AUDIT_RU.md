# PPGU REV 18 — аудит и безопасная миграция идентичности NEON STAGE

Дата: 2026-11-15  
Ветка: `work/neon-stage-reference-preview-fix`  
Статус: **CORRECTED MIGRATION CONTRACT / RUNTIME PATCH STILL REQUIRED**

## 1. Обязательное решение пользователя

Единственная каноническая тема по умолчанию:

- Пользовательское имя: **NEON STAGE DEFAULT SKIN-1**
- Каноническая последовательность: **SKIN-001**
- Каноническая папка: **Skin_001**
- Канонический стабильный ID: **skin-001**
- Позиция в активной галерее: **1**

Все последующие новые темы получают последовательные идентификаторы SKIN-002 / Skin_002 / skin-002, SKIN-003 / Skin_003 / skin-003 и далее. Имена следующих тем должны быть уникальными.

Это решение заменяет прежние утверждения этого документа о `skin-053 / Skin_053`, а также более ранние решения, считавшие `Skin_052` каноном. Ни один из этих старых ID/path не должен оставаться default или fallback после завершения миграции.

## 2. Фактическое состояние HEAD до миграции

Проверенный исходный HEAD: `2970a3486d5717670209b13e4f6a94f41b0a88a9`.

В текущем дереве обнаружено:

| Компонент | Фактическое состояние |
|---|---|
| `SKIN/theme_manifest.json` | 102 записи; индексы 1…102; default `neon-stage-default-skin-1`; запись NEON STAGE указывает на `Skin_052/theme.json` с index 52. |
| `SKIN/theme_pack_manifest.json` | Skin Studio pack охватывает 52…102; Skin_052 указан как NEON STAGE, Skin_053 — как архивная запись. |
| `SKIN/Skin_052/theme.json` | ID `neon-stage-default-skin-1`, index 52. |
| `SKIN/Skin_053/theme.json` | ID `skin-053`, index 53; прежняя маркировка LEGACY ARCHIVE. |
| `SKIN/Skin_01/theme.json` | Уже существует legacy-папка с другим содержимым и ID `skin-01`; она НЕ является требуемой папкой `Skin_001`. |
| `Source/PluginEditor.cpp` | Runtime registry требует индексы 1…total без пропусков; особый default ID жёстко привязан к index 52; saved IDs 052/053 переписываются в прежний default. |
| `Source/UI/ThemeManager.cpp` | Skin Studio validator допускает canonical ID только как `neon-stage-default-skin-1` + index 52; numeric Skin Studio IDs ограничены индексами >=52. |
| `tests/ValidateSkinManifests.py` | Ожидает прежний default ID и специальную обработку Skin Studio для индексов 52/53. |
| `tools/validate_skin_pack.py` | Валидирует старую pack/registry модель; генератор и merger также требуют отдельной проверки перед сменой модели. |

**Вывод:** одной замены `defaultSkin`, ID или папки недостаточно. Она сломает текущую валидацию, выбор по индексу, галерею, загрузку сохранённой темы и/или Skin Studio pack workflow.

## 3. Безопасная модель миграции

Не переиспользовать историческое поле `index` для двух разных смыслов. В итоговом контракте должны быть разделены:

- `index` / `legacyIndex`: исходный исторический номер записи, если он нужен для совместимости и архивного аудита;
- `activeOrdinal`: позиция темы в пользовательской последовательности SKIN-001, SKIN-002…;
- `active`: включена ли тема в активную галерею;
- `id` и `path`: стабильная идентичность и реальное расположение файлов.

Предпочтительная целевая запись NEON STAGE: `id=skin-001`, `activeOrdinal=1`, `path=Skin_001/theme.json`, `active=true`, `name=NEON STAGE DEFAULT SKIN-1`.

Исторические данные не удалять и не перезаписывать:

1. Сохранить `Skin_01`…`Skin_051` байт-в-байт как legacy records; не путать `Skin_01` с новой папкой `Skin_001`.
2. Сохранить `Skin_052` и `Skin_053` со всеми исходными ID, index, JSON и artwork. После проверки зависимостей пометить их архивными и исключить из active gallery.
3. Сохранить `Skin_054`…`Skin_102` и все ресурсы; их будущие activeOrdinal назначать только после проверки полноты и готовности каждой темы. Не перенумеровывать их автоматически на основании только старого физического index.
4. Создать отдельную папку `Skin_001` с manifest и ресурсами, используя утверждённый NEON STAGE дизайн как источник. Не переименовывать и не удалять `Skin_01`, `Skin_052` или `Skin_053`.
5. Старые сохранённые значения `skinId` мигрировать по явной таблице совместимости. Не переписывать все сохранённые темы в NEON STAGE: пользовательские выборы других тем должны сохраняться. Если ID неоднозначен или отсутствует, использовать новый canonical default безопасно.

## 4. Обязательные consumers и проверки

До изменения runtime registry проверить и обновить согласованно:

- `Source/PluginEditor.cpp/.h`: registry parsing, count, manifest lookup, active ordering, gallery filtering, default/fallback и persisted selection;
- `Source/UI/ThemeManager.cpp/.h`: допустимая пара ID/manifest metadata и reset-to-default palette;
- `SKIN/theme_manifest.json`, `SKIN/theme_pack_manifest.json`, `SKIN/control_schema.json`, JSON schema и install/package registry;
- `tests/ValidateSkinManifests.py`, `tools/validate_skin_pack.py`, генераторы, merger, pack validators и соответствующие tests;
- все references к `defaultSkin`, `neon-stage-default-skin-1`, `skin-052`, `skin-053`, `Skin_052`, `Skin_053`, `index`, gallery sort и saved `skinId`.

Требуемые regression cases:

1. Новый install без сохранённого выбора загружает `skin-001` из `Skin_001/theme.json`.
2. Сохранённый `skin-001` остаётся выбранным после перезапуска.
3. Legacy saved ID `neon-stage-default-skin-1` разрешается в новую canonical тему согласно migration table.
4. Legacy saved IDs прочих тем не теряют свой выбор.
5. `Skin_01`, `Skin_052`, `Skin_053` остаются физически целыми и не конфликтуют с `Skin_001`.
6. Active gallery показывает только активные темы в порядке activeOrdinal; архивы не отображаются как новые SKIN themes.
7. Неизвестный ID, отсутствующий manifest или повреждённый asset безопасно откатывается к Skin_001 без потери сохранённых данных.
8. Pack generation/merge не переиспользует исторические индексы и не создаёт дубликаты ID/path/activeOrdinal.
9. Control parity проверяется против утверждённого inventory; обнаруженные ранее 27 required IDs против 28 theme keys (включая `sync_selector`) нельзя устранять механическим удалением поля без проверки consumers.

## 5. Границы и release gates

- Только визуальная тема/registry/runtime theme selection. Не менять DSP, audio callback, APVTS IDs/semantics, sampler, MIDI, presets, transport, articulations или automation.
- Не объявлять визуальные mockups или artwork скриншотами работающего VST3.
- Не объявлять G1–G4 PASS без результатов валидаторов на том же SHA.
- После миграционного patch требуются unit/manifest tests, G1–G4, CTest, Windows x64 Release, VST3 packaging и pluginval strictness 5. Отдельно необходима ручная проверка в FL Studio 20.
- PR #33 остаётся Draft и не сливается до прохождения проверок и визуальной приёмки.

## 6. Текущий статус этой записи

Этот документ исправляет ошибочную цель миграции и фиксирует фактические конфликты. **Он сам по себе не выполняет runtime migration.** До внесения и тестирования согласованных изменений в код, registry, validators, generator и packaging канонический Skin_001 считается целевым контрактом, но не подтверждённым runtime default.

Исходные SHA и данные аудита относятся к проверенному HEAD `2970a3486d5717670209b13e4f6a94f41b0a88a9`; после новых коммитов требуется повторная проверка дерева и всех выводов.
