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
