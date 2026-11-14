# PPGU REV 18 — аудит миграции идентичности NEON STAGE

Дата: 2026-11-15  
Режим: C3 → подготовка C2  
Ветка: `work/neon-stage-reference-preview-fix`  
Статус: **AUDIT COMPLETE / MIGRATION PATCH BLOCKED BY IDENTITY COLLISION ANALYSIS**

## 1. Приоритет контракта

[IDENTITY-RULE: REV 18 supersedes the repository's REV 14/17 theme identity declarations.]  
Обязательная целевая идентичность: **SKIN-1 = NEON STAGE = DEFAULT = skin-053 / Skin_053**. SOUND FORGE и SIGNAL FORGE не являются отдельными темами. Все остальные активные темы нумеруются последовательно SKIN-2, SKIN-3… без дыр и дублей. Исторические registry entries сохраняются без потери, но не становятся активными темами только из-за присутствия в старом registry.

## 2. Факты текущего дерева

| Источник | Найдено | Вывод |
|---|---|---|
| `SKIN/theme_manifest.json` | totalSkins=102; defaultSkin=`neon-stage-default-skin-1`; entries `skin-01…skin-102` | Старый registry считает все 102 записи активными и назначает default Skin_052. |
| `SKIN/theme_pack_manifest.json` | startIndex=52, endIndex=102, 51 entries | Pack manifest включает Skin_052 как index 52 и Skin_053 как index 53. |
| `SKIN/Skin_052/theme.json` | id=`neon-stage-default-skin-1`, index=52, name=NEON STAGE, status USER_APPROVED… | Это текущая прежняя default-запись; её нельзя молча уничтожить. |
| `SKIN/Skin_053/theme.json` | id=`skin-053`, index=53, name=NEON STAGE — LEGACY ARCHIVE, status LEGACY_ARCHIVE_NOT_ACTIVE | Именно этот стабильный ID/path REV 18 назначает каноническим SKIN-1. |
| `SKIN/control_schema.json` | 27 requiredControlIds; 21 palette tokens | Контракт schema сейчас старше REV 18 и включает legacy placeholder IDs. |
| Skin_052 и Skin_053 controls | 28 keys в каждом | Обе темы содержат дополнительный `sync_selector`, отсутствующий в 27 required IDs. Это не основание автоматически удалить его: требуется отдельная проверка inventory/consumer. |
| `Source/UI/ThemeManager.h` | default Palette id=`neon-stage-default-skin-1` | Runtime fallback также противоречит REV 18. |
| `Source/UI/ThemeManager.cpp` | JSON loader валидирует manifest и активирует palette; отсутствует registry resolver | ThemeManager сам по себе не выбирает default entry по registry; caller/runtime persistence необходимо проследить отдельно. |

## 3. Безопасная миграционная таблица

| Текущая запись | REV 18 трактовка | Действие |
|---|---|---|
| Skin_053 / `skin-053` | **SKIN-1 / NEON STAGE / DEFAULT** | Сохранить path и stable ID; установить канонический default и первую позицию галереи. |
| Skin_052 / `neon-stage-default-skin-1` | Историческая дублирующая запись NEON STAGE | Сохранить все файлы, ID и index как legacy archive; исключить из active gallery, если это подтверждается runtime registry contract. Не удалять и не переименовывать. |
| Skin_01…Skin_051 | Исторические registry entries (legacy) | Сохранить байт-в-байт; не объявлять новыми активными SKIN themes без отдельного утверждения. |
| Skin_054…Skin_102 | Существующие будущие/дополнительные записи | Не менять ID/path/index до миграции registry. После отделения legacy определить активную последовательность SKIN-2… без потери исходных индексов в архивной metadata. |

**Важно:** нельзя просто поменять `defaultSkin` на `skin-053` и `index` на 1: текущая схема требует index ≥52, pack generator/merger ожидает непрерывный диапазон 52…102, а legacy registry уже занимает номера 1…51. Такая правка создаст несогласованный registry и может сломать merge, gallery ordering или persistence.

## 4. Обязательные проверки перед миграционным patch

1. Найти все consumers `theme_manifest.json`, `theme_pack_manifest.json`, `defaultSkin`, stable theme IDs и saved user selection; проверить migration/fallback при неизвестном ID.
2. Проверить ThemeManager caller, gallery sort/filter/active selection, install-time `Skins/` packaging и preset/state serialization.
3. Сохранить snapshot исходных 102 registry records и проверить все пути/assets перед перестройкой active-vs-archive model.
4. Определить единое представление: неизменяемый historical record ID/path/index + отдельный REV 18 active ordinal. Не переиспользовать поле `index` для двух разных смыслов.
5. Установить точный набор активных тем. Skin_053 — SKIN-1; Skin_052 — archive; старые Skin_01…51 — historical archive; Skin_054…102 — последующие active themes только после проверки их готовности и порядка.
6. Обновить validator, generator, merger, runtime registry loader и tests одним совместимым контрактом; добавить migration tests для старых saved IDs.
7. Перегенерировать gallery artwork согласно A4/A7. Текущий staging mockup содержит PUNK GROUP, LED/activity, wheels и CPU/latency meters, запрещённые REV 18 как представление готовых функций.
8. Проверить control parity: schema перечисляет 27 обязательных IDs, theme maps имеют 28 ключей из-за `sync_selector`; сверить с утверждённым REV 18 inventory, не подгонять schema искусственно.
9. Выполнить G1–G4, CTest, Windows x64 Release, locate VST3, pluginval strictness 5, packaging. Отдельно требуется ручная FL Studio 20 host acceptance.

## 5. Границы текущего изменения

- Аудио, APVTS, MIDI, presets, SoundBank и DSP не менялись.
- Не переименовывались и не удалялись registry entries или asset folders.
- Не менялись runtime default/fallback, так как одного registry diff недостаточно для безопасной миграции persistence.
- Успешные G1–G4 staging runs прежнего контракта не считаются доказательством REV 18 compliance.
- Skin_052/053 runtime behavior и 27/28 parity остаются **UNVERIFIED** до выполнения перечисленных проверок.

## 6. Решение реализации

[DECISION: preserve stable historical IDs, paths, and original indices; add a separate active ordinal/visibility field rather than renumbering historical records.] Это единственная безопасная модель, которая одновременно соблюдает no-loss и позволяет SKIN-1 указывать на `skin-053/Skin_053`. Конкретное поле и формат миграции выбираются только после полного аудита всех consumers.

**Итог:** аудит завершён; destructive/partial manifest edit не выполнялся. Следующий безопасный шаг — инвентаризация всех registry consumers и migration tests, затем единый атомарный patch.


## 7. Consumer audit — registry, runtime, saved selection and validators

Проверены фактические consumers в рабочей ветке. Это статический source audit; поведение после миграции пока не запускалось.

| Consumer | Текущая зависимость | Последствие для REV 18 |
|---|---|---|
| `SKIN/theme_manifest.json` | `totalSkins=102`, `defaultSkin=neon-stage-default-skin-1`, индексы 1…102 считаются единым списком | Нельзя заменить только default ID; нужно отделить active ordinal от исторического index. |
| `SKIN/theme_pack_manifest.json` | MERGED pack 52…102; Skin_052 — прежний NEON STAGE, Skin_053 — архив | Исторический pack нельзя молча переписать: validator сверяет поля pack и registry. Нужна явная archive/provenance модель. |
| `Source/UI/ThemeManager.h/.cpp` | Fallback ID = `neon-stage-default-skin-1`; parser допускает этот ID на index 52 и numeric Skin Studio IDs на index ≥52 | Fallback identity и contract tests должны перейти на `skin-053`; legacy manifest parsing нужно сохранить. |
| `Source/PluginEditor.cpp::getSkinCount` | Возвращает total registry entries и требует индексы 1…total | После архивирования старых записей это не может быть active gallery count. |
| `Source/PluginEditor.cpp::getSkinManifest` | Ищет запись по исходному `index` | Должен искать по отдельному `activeOrdinal`, затем возвращать неизменённый stable path/index. |
| `Source/PluginEditor.cpp::loadSavedSkin` | Читает `skinId` из JUCE PropertiesFile; старые `skin-052`/`skin-053` сейчас переводит в прежний ID | Требуется миграция old default ID `neon-stage-default-skin-1` → `skin-053`; сохранённые архивные ID должны иметь явный fallback policy. |
| `Source/PluginEditor.cpp::persistSkinPreference` | Сохраняет stable palette ID в `skinId` | Продолжать хранить stable ID, не active ordinal. Не переносить skin preference в APVTS. |
| `Source/PluginEditor.cpp::openSkinGalleryPanel` | Перебирает 1…getSkinCount(), создаёт карточки и применяет выбранный числовой index | Перебор должен идти по активному порядку; archived entries не должны попадать в gallery. |
| `Source/PluginEditor.cpp::openSettingsPanel` | Restore action вызывает `loadSkinByIndex(52, true)` | После введения active ordinal restore target должен стать SKIN-1 / ordinal 1. |
| `tests/ThemeManagerTests.cpp` | Ожидает старый default ID; проверяет Skin_052 как canonical и Skin_053 как archive | Требуются новые assertions на fallback, default registry, legacy saved ID migration и сохранение Skin_052 archive. |
| `tests/ValidateSkinManifests.py` | `DEFAULT_ID` старый; проверяет все 102 индекса как одну непрерывную selector sequence | Валидировать stable registry index отдельно от active ordinal/status. |
| `tools/validate_skin_pack.py` | Для index 52 ожидает старый canonical ID; сравнивает merged pack с registry | Pack validator должен понимать immutable historical pack vs active runtime registry. |
| `tools/generate_skin_batch.py` / `tools/merge_skin_pack.py` | Следующие темы получают индекс после totalSkins; merge требует непрерывность 1…N | Генератор/merge должны назначать следующий activeOrdinal, не переиспользуя historical index. |

### 7.1 Предлагаемая модель без перенумерации

[DECISION: сохранять `index`, `id` и `path` как immutable historical identity; отдельное поле `activeOrdinal` определяет пользовательский SKIN номер.]

| Stable registry record | Active ordinal | UI label | Lifecycle |
|---|---:|---|---|
| Skin_01…Skin_051 | отсутствует | не показывать | ARCHIVED |
| Skin_052 / `neon-stage-default-skin-1` | отсутствует | не показывать | ARCHIVED |
| Skin_053 / `skin-053` | 1 | SKIN-1 / NEON STAGE | ACTIVE / DEFAULT |
| Skin_054 / `skin-054` | 2 | SKIN-2 | ACTIVE |
| Skin_055 / `skin-055` | 3 | SKIN-3 | ACTIVE |
| … | … | … | … |
| Skin_102 / `skin-102` | 50 | SKIN-50 | ACTIVE |

- `index` остаётся исходным registry index 1…102 и не используется как display number.
- `activeOrdinal` уникален и непрерывен 1…50; archived entries не имеют activeOrdinal.
- `defaultSkin=skin-053`; `activeSkinCount=50`; `totalSkins=102` остаётся количеством всех сохранённых записей.
- Stable IDs Skin_054…Skin_102 не переименовываются в skin-002…skin-050.
- Старый Skin_052 и прежний Skin_053 payload должны быть сохранены byte-for-byte в архиве до любой замены/перемещения; pack manifest/provenance должен явно описывать архивную копию.
- Галерея, Settings restore, saved preference resolver и validators должны использовать один и тот же resolver activeOrdinal ↔ stable registry record.
- Нельзя считать этот mapping реализованным, пока registry schema, pack validator, generator/merger, runtime gallery, saved-state migration и tests не обновлены согласованно.

### 7.2 Новые конкретные blockers

1. `loadSavedSkin()` не знает нового canonical ID и имеет устаревшую миграцию двух numeric IDs к старому default.
2. Settings restore жёстко привязан к index 52.
3. Gallery немедленно применяет тему и показывает все registry entries; archive visibility/Preview→Apply→Back не реализованы.
4. ThemeManager fallback ID и тестовые assertions указывают на прежний canonical ID.
5. Pack manifest и pack validator считают Skin_052…Skin_102 единым активным pack, поэтому простая перестановка registry полей нарушит G4.
6. В REV 18 inventory есть 27 required control IDs, а Skin_052/053 theme maps имеют 28 ключей с `sync_selector`; consumer/schema parity требует отдельного решения на основе binding inventory, без удаления ключа вслепую.

**Результат consumer audit:** IMPLEMENTED (статический аудит исходников). Runtime migration remains BLOCKED until the activeOrdinal/archive contract is implemented atomically across the listed consumers and tests. Audio/APVTS/MIDI/DSP remain out of scope.
