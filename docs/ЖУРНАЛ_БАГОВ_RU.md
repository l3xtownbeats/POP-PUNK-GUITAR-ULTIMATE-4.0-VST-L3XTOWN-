# Журнал багов — POP PUNK GUITAR ULTIMATE / NEON STAGE

Статусы меняются только при наличии воспроизводимого тестового доказательства. Записи не означают релизную приёмку.

## UI-SKIN-001 — Apply доступен до явного Preview

- **Severity:** Low (ошибка транзакционного UX; не повреждает аудиосостояние).
- **Обнаружено:** 2026-11-15, ветка `ui/neon-stage-playing-core`.
- **Root cause:** `PPGUSkinGalleryPanel` инициализировал `previewedIndex` текущим активным индексом. Из-за этого исходная тема ошибочно считалась прошедшей Preview в текущей сессии; кнопка Apply оставалась доступной сразу после открытия галереи.
- **Ожидаемое поведение:** Apply недоступна при открытии и после одного лишь выбора карточки. Она включается только после успешного Preview текущего выбора. Ошибка Preview не должна открывать путь к сохранению.
- **Исправление:** `previewedIndex = 0` при старте сессии и явный `applyButton.setEnabled(false)`. Уточнён комментарий об active registry: архив legacy skins не является runtime registry.
- **Regression coverage:** исходная точка подтверждена анализом обработчиков; автоматическая UI-level regression проверка ещё не добавлена. Точная проверка нового HEAD ожидает CI.
- **Commits:** `e28aa685bf99f605e71a7cc4c1bc2d8caa44514c`, `0e472a056377925a24a2f8f868fedd8ee0d77530`, `869625491b182a8ae84706072e5f7087599f49a2`.
- **CI:** run `43718887827` — `completed / success` для SHA `cd4935576c93f6974c6a5af4f319413b3ae03024`; он покрывает исправление галереи, но не последующие коммиты About panel. Текущий HEAD требует отдельной exact-HEAD проверки.
- **Status:** FIX COMMITTED / CI PENDING / UI HOST ACCEPTANCE OPEN.
## UI-SKIN-002 — Устаревшая подпись восстановления темы

- **Severity:** Low (несоответствие утверждённой идентичности в UI).
- **Root cause:** кнопка Settings называла действие `RESTORE NEON STAGE DEFAULT`, хотя утверждённая SKIN-1 называется `NEON FORGE (DEFAULT)`; NEON STAGE — название общей GUI design system.
- **Fix:** подпись заменена на `RESTORE NEON FORGE DEFAULT`.
- **Scope:** только видимый текст; control/APVTS IDs, callback, выбор темы и persistence не изменялись.
- **Commit:** `c58252182aa11144b6b14951fc6f97e512202ab8`.
- **Validation:** run `43718887827` — `completed / success` для SHA `cd4935576c93f6974c6a5af4f319413b3ae03024`; исправление подписи включено. Этот run не покрывает более поздний About panel. Текущий HEAD требует отдельной exact-HEAD проверки.
- **Status:** FIX COMMITTED / CI PENDING / VISUAL HOST ACCEPTANCE OPEN.


## UI-ABOUT-001 — About panel отсутствовала в production UI

- **Severity:** Low (отсутствовала информационная панель, не затрагивала audio path).
- **Fix:** добавлен `PPGUAboutPanel` (workspace panel ID `2103`), подключён действующий ABOUT entry point. Панель показывает только подтверждённые название/тип продукта, студию и краткое описание; активная тема применяется к типографике и цветам. Не добавлены вымышленные build/version claims или фиктивные действия.
- **Scope:** `Source/PluginEditor.h`, `Source/PluginEditor.cpp`; APVTS, DSP, MIDI, processor state и аудиодвижок не менялись.
- **Commits:** `8adb6cab870fad2e777510a640d503f21a7fc9b4`, `806b6b39ca9644ff50fb5e1b07db91e77c4d660b`, `1a1910aad51538ef556f7b09902f52bdea3b069b`.
- **Validation:** source implementation committed; exact-HEAD Windows build, CTest, pluginval, panel resize/theme runtime checks and FL Studio 20 acceptance pending.
- **Status:** SOURCE IMPLEMENTED / VALIDATION PENDING / HOST ACCEPTANCE OPEN.
