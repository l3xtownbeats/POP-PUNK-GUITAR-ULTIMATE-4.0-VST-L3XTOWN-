# PPGU ALL-IN-BOX — REV 18: интеграция и порядок запуска

## Состав
| Файл | Назначение | CTest target |
|---|---|---|
| Source/UI/SkinGalleryAllInOne.cpp | Галерея REV 18 v2.0: Select/Preview/Apply/Back/Close, Favorites, поиск, фильтр семейств, клавиатура | ppgu_skin_gallery_contract |
| Source/UI/OverlayManagerAllInOne.cpp | O1: AMP & TONE / FX CHAIN / PLAYBACK как независимые окна; z-order, фокус, клампинг, ECO, persistence | ppgu_overlay_contract |
| Source/Security/SecureSkinImportAllInOne.cpp | S1: whitelist путей, traversal-защита, PNG/WAV magic, SVG policy (закрыты 2 известных обхода), ZIP bounds-check, манифест 21 токена, SHA-256 KAT | ppgu_skinimport_contract |
| CMAKE_ALL_IN_BOX_SNIPPET.txt | Регистрация targets | — |
| CODE_ENTRY.txt, INFO_ENTRY.txt | Готовые append-only записи в журналы | — |

## Шаги интеграции
1. Скопировать три .cpp в канонические пути (Source/UI/, Source/Security/).
   Если Source/Security/ нет — создать; это новая canonical директория.
2. Применить сниппет из CMAKE_ALL_IN_BOX_SNIPPET.txt к основному target
   и добавить три консольных test target.
3. Подключение галереи в PluginEditor:
   - при открытии галереи собрать std::vector<ppgu::gallery::Entry> из
     SKIN/theme_manifest.json (активные записи REV18: activeOrdinal 1..N);
   - SkinGalleryPanel::Listener::galleryApplyRequested(id, index) →
     loadSkinByIndex(index, true) + persistence stable id в PropertiesFile;
   - galleryClosed() → просто закрыть окно; модель сама вернёт выбор к
     активной теме — временная палитра не остаётся;
   - favorites хранить одной отсортированной строкой в PropertiesFile
     (ppguGalleryFavourites), восстановление через toggleFavourite().
4. Подключение OverlayManager в редактор:
   - один экземпляр на editor, listener прокидывает setBounds/setVisible
     на реальные juce::DialogWindow/Component;
   - serialize() при закрытии окна → PropertiesFile "workspaceOverlays";
   - deserialize() при открытии редактора.
5. SecureSkinImport: вызывать перед записью файлов импортируемого пакета;
   путь записи — только после полного PASS валидации.

## Инварианты (обязательные)
- APVTS IDs/semantics, DSP, audio callback, sampler, MIDI, presets, SoundBank
  НЕ изменяются. Модули — визуальный слой и слой безопасности импорта.
- SKIN-1 = skin-001 = NEON STAGE DEFAULT (default/fallback/ordinal 1).
- Не заменять live-дерево ветки старыми ZIP-снапшотами (правило CODE.txt).

## Проверка
1. Windows x64 Release configure + build на exact-SHA.
2. Полный CTest (все targets, включая три новых).
3. pluginval strictness 5.
4. FL Studio 20: открытие/закрытие галереи, Preview без применения,
   Apply + перезапуск проекта (тема восстанавливается), ECO-переключение
   окон, импорт безопасного и злонамеренного пакета (ожидается отказ).
