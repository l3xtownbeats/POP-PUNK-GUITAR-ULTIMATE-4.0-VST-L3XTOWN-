# PPGU SIGNAL FORGE v10.1 — инженерная редакция и уточнения

**Дата:** 2026-11-12  
**Ветка:** `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`  
**Базовый SHA:** `7a4abc486038ad820fba6af04d5fdbd5c2238797`  
**Статус:** NORMATIVE CLARIFICATION / DOCUMENTATION ONLY  
**Приоритет:** этот документ уточняет измерения, границы и неоднозначности v10; не отменяет пользовательские требования v10. При конфликте с функциональным требованием v10 сохраняется его смысл, но критерий приёмки трактуется по измеримому контракту ниже.

## 1. Главные исправления v10

### 1.1 Разделить гарантии аудиопотока и стоимость интерфейса
Фраза «скин не расходует CPU/RAM» физически слишком абсолютна: любая отрисовка и декодированный bitmap имеют стоимость. Обязательная гарантия формулируется так:
- **Audio hard guarantee:** skin code не вызывается из `processBlock`, не меняет DSP/MIDI/sampler/APVTS и не создаёт аллокаций в audio callback.
- **UI budget:** стоимость skin измеряется отдельно от аудиотракта; она ограничена бюджетами paint-time, memory cap и частоты repaint.
- **CPU delta 0%** трактуется как «нет статистически значимого изменения audio-thread CPU». Для UI-потока допустима измеряемая стоимость, но регрессия относительно code-drawn baseline не должна превышать 5% p95 paint-time и не должна вызывать пропуски кадров.
- **RAM zero-residue** означает отсутствие удерживаемых skin-owned объектов/декодированных ресурсов после unload и возврата к baseline после стабилизационного окна GC/allocator/OS. Не сравнивать мгновенный Working Set Windows как доказательство утечки или её отсутствия.

### 1.2 Зафиксировать методику измерений
Все метрики снимаются на одном устройстве/сборке/хосте, с указанием CPU, RAM, ОС, sample rate, buffer size, voices, DPI, состояния кэша и числа повторов.
- Latency и paint-time: минимум 100 повторов; публиковать median, p95 и max.
- Hot-swap: от подтверждённого выбора до завершённого repaint всех видимых окон; цель 150 ms p95 на warmed assets. Первый импорт/декод не включать в hot-swap: он выполняется заранее в фоне.
- Key reactivity: MIDI event timestamp → первый видимый кадр подсвеченной клавиши; цель ≤16 ms p95 при активном окне. Отдельно тестировать виртуальную клавиатуру и MIDI от хоста.
- CPU: фиксированные 60 s steady-state для 4 и 64 voices, одинаковый проект и buffer; сравнивать audio-thread и UI-thread отдельно.
- RAM: baseline после 60 s idle; после 10 смен ждать 30 s стабилизации; фиксировать private bytes и skin-owned live bytes. Утечка — монотонный рост skin-owned live bytes или превышение заданного cap, а не разовый шум Working Set.
- Audio identity: offline render одинакового MIDI/параметров до и после skin switch; сравнение PCM checksum при детерминированном рендере. Если DSP недетерминирован, сравнивать null-test/RMS/peak с заранее заданным допуском.

### 1.3 Исправить чрезмерно жёсткие latency-обещания
- «Первый звук ≤50 ms» применим только к already-loaded preset и resident preview page при прогретом хосте; cold-start и cold-disk измеряются отдельно и документируются.
- «Пресет ≤300 ms» — p95 для переключения между уже индексированными пресетами; загрузка отсутствующих/новых библиотечных страниц учитывается отдельно.
- «Мгновенно» заменяется на конкретный p95 budget; UI не должен блокироваться, а аудиопоток — ждать файловую систему или декодер.
- Ни один deadline не достигается ценой dropouts, блокировки message thread или аллокаций в audio callback.

### 1.4 Разрешить противоречие unlimited vs quotas
«Неограниченная библиотека» означает отсутствие compile-time лимита на число пользовательских скинов, пресетов и библиотек. Это не означает неограниченное потребление RAM/диска:
- RAM/cache/decoded assets ограничены настраиваемыми квотами;
- большие банки индексируются и стримятся страницами;
- лимиты относятся к одновременно resident данным, а не к общему числу ресурсов;
- manifest count в ZIP — фактическое число поставляемых скинов, не жёсткая константа 50;
- начальная поставка обязана включать пять утверждённых скинов; дополнительные — опциональные и обнаруживаются автоматически.

### 1.5 Уточнить ONE-FOLDER HOME для Windows VST3
Единый корень `PPGU/` — portable content root, а не требование хранить сам VST3 bundle только внутри нестандартного каталога. Установщик может разместить VST3 в стандартном VST3 location Windows, а контент — в `PPGU/`; portable mode допускает VST3 и контент рядом. Порядок поиска:
1. явно заданный пользовательский content root;
2. portable `PPGU/` рядом с плагином;
3. пользовательская папка данных PPGU.
Путь сохраняется как относительный/переносимый там, где это возможно. Запрещены traversal (`..`), абсолютные пути из недоверенного manifest, symlink escape и ZIP Slip. При недоступности root — понятный статус и safe fallback, не падение плагина.

### 1.6 Уточнить Open Source Compliance
«Любая совместимая лицензия» не означает автоматическое разрешение на закрытую коммерческую поставку. До встраивания фиксируются SPDX ID, версия, URL исходника, commit/tag, copyright notices, условия модификации, статическое/динамическое связывание и обязательные attribution/NOTICE. MIT/BSD/ISC/zlib/Apache-2.0 обычно подходят при выполнении условий; Apache-2.0 требует сохранения NOTICE/атрибуции и проверки патентных условий. LGPL требует отдельного юридико-технического анализа способа линковки и возможности замены библиотеки; GPL/AGPL не встраивать в закрытый бинарник без отдельного утверждённого лицензионного решения. Лицензия неясна — компонент не включать.

## 2. Архитектурные оптимизации

1. **Single source of truth:** один schema registry для controls, APVTS mapping, skin tokens, panel inventory и validator. Генерировать из него checklist/manifest assertions; не поддерживать параллельные вручную расходящиеся списки.
2. **Skin package:** manifest + assets + metadata + checksums + license/attribution. Валидировать schema/version/size/path/dimensions/decompression limits до декода.
3. **Immutable snapshot:** подготовить и декодировать новый skin вне message/audio thread; публиковать immutable shared snapshot на message thread; старый snapshot освобождать после завершения paint readers. Не использовать атомарный raw pointer с возможным use-after-free.
4. **Asset cache:** bounded LRU с лимитом decoded bytes; ключ — canonical asset ID + content hash; shared assets между скинами не дублировать; unload удаляет только неиспользуемые ресурсы.
5. **No filesystem in audio:** весь I/O, hashing, parsing, decode, scanning и logging выполняются worker thread. Audio thread получает только заранее подготовленные данные через существующие realtime-safe механизмы.
6. **Failure isolation:** повреждённый skin не блокирует загрузку проекта; rollback к последнему валидному skin; пользовательский skin открывается в sandbox-подобном ограниченном asset loader, без исполняемых скриптов.
7. **Versioned contracts:** manifest schema, control schema, sound registry и package manifest имеют собственные версии и migration tests. Не менять ID/семантику APVTS ради визуального соответствия.
8. **Build speed:** ccache/PCH включать только после измерения чистой и инкрементальной сборки; не ухудшать воспроизводимость. UI-only tests могут быть affected-only, но release gate всегда запускает полный набор на одном SHA.
9. **Parallelism truth:** фронты могут выполняться параллельно только при независимых файлах/контрактах. Если фактически доступен один исполнитель, задачи выполняются последовательно; нельзя заявлять работу нескольких агентов или параллельные merge без реального подтверждения.
10. **Package integrity:** один package manifest связывает SHA исходников, VST3 binary, SoundBank, presets, skins и licenses. ZIP создаётся из staging root и проверяется распаковкой в чистую папку с запуском portable smoke test.

## 3. Уточнённая модель статусов

Использовать только:
- `DESIGNED` — описано, реализации нет.
- `SOURCE PRESENT` — код существует, выполнение не подтверждено.
- `TESTED` — соответствующие тесты прошли на указанном SHA.
- `MEASURED` — приведены методика, окружение, median/p95/max и raw result.
- `HOST VERIFIED` — проверено вручную в FL Studio 20 на указанном SHA и конфигурации.
- `RELEASED` — пакет собран, checksum зафиксирован, clean-folder smoke test пройден.
- `PARTIAL` / `BLOCKED` / `FAIL` — с конкретной причиной.

Статусы не подразумевают друг друга автоматически. Например, `TESTED` не означает `HOST VERIFIED`, а зелёный CI не означает прохождение performance gates.

## 4. Порядок гейтов без дублирования

- **Tier 0 — Safety/Integrity:** path validation, manifest validation, license ledger, package checksum.
- **Tier 1 — Build:** configure, compile, link, VST3 bundle validation.
- **Tier 2 — Correctness:** unit/contract/CTest, lifecycle, fallback, control parity.
- **Tier 3 — Realtime:** allocation audit, callback timing, audio identity, dropout tests.
- **Tier 4 — UI/UX:** key reactivity, DPI, accessibility/contrast, screenshot parity, panel/window flows.
- **Tier 5 — Performance:** CPU/RAM/paint/hot-swap/streaming budgets.
- **Tier 6 — Host/Package:** pluginval, FL Studio 20 manual test, clean-root relocation, final ZIP audit.

Merge блокируется только гейтом, относящимся к изменённой области, плюс обязательными Tier 0–2; release блокируется всеми применимыми Tier 0–6. Для release все результаты относятся к одному SHA и одному package manifest.

## 5. Исправления исходного v10, которые теперь считаются обязательными

- «CPU delta = ровно 0%» заменить на «audio-thread CPU не имеет статистически значимой дельты; UI cost измеряется отдельно и укладывается в бюджет».
- «RAM после смены строго равна исходной» заменить на «skin-owned live allocations возвращаются к baseline после стабилизационного окна; private bytes не демонстрируют монотонного роста и остаются в cap».
- «50 манифестов» считать примером/минимумом только если это отдельно утверждённый объём поставки; для unlimited skins пакет проверяет все найденные манифесты, а не ровно 50.
- «300 пресетов / ≥100 WAV» — release content targets, не основание генерировать фиктивные записи или включать неразрешённые ассеты.
- «полностью параллельно» — организационная цель, а не заявление о фактически запущенных агентах.
- «≤15 минут на коммит» — SLO CI, а не критерий корректности; превышение фиксируется как CI performance issue, но не оправдывает пропуск тестов.
- «без ограничений на инструменты» всегда подчиняется A12: совместимость лицензии, безопасность supply chain, воспроизводимость и attribution обязательны.

## 6. Что не меняется

Сохраняются все функциональные требования v10: пять стартовых skins, unlimited user skins, зоны A–H, REV.17 control parity, одна функциональная архитектура, сохранение выбора, key reactivity, один переносимый content root, интеграция SoundBank/presets, все запреты L1–L22 и цикл A6. Это уточнение делает требования проверяемыми и устраняет физически/технически неоднозначные критерии, не снижая целевой уровень продукта.

**Статус документа:** DESIGN CLARIFICATION. Это изменение спецификации; оно не является доказательством реализации, сборки, измерений или host verification.
