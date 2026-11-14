# PPGU — РЕЕСТР УТВЕРЖДЁННЫХ ПАРАМЕТРОВ (v2, 2026-11-14)

Назначение: единая точка сверки утверждённых числовых значений и канонических
констант. Этот документ НЕ вводит новых утверждений — он консолидирует
уже зафиксированные в INFO/CODE. При конфликте приоритет у INFO.

## 1. Theme / Skin Studio

| Параметр | Утверждённое значение | Источник |
|---|---|---|
| Канонические токены палитры | 21 (window_bg ... lowlight) | CODE 2026-11-12 ThemeManager |
| Контрольный набор манифеста | 27 required IDs + sync_selector = 28 | CODE validateSkinStudioManifest |
| Реестр тем | 102 записи | INFO skin pack merge |
| Default skin | skin-052 SIGNAL FORGE | INFO/CODE |
| Legacy schema | Skin_001..051, version 21, двузначные ID | SKIN structure canon |
| Studio schema | Skin_052+, ID из 3+ цифр | SKIN structure canon |
| Package gate: skins | >= 52 директорий | build.yml |
| Package gate: presets | ровно 500 (250 MELODY + 250 PATTERN) | build.yml |

## 2. Workspace / панели

| Параметр | Значение | Источник |
|---|---|---|
| workspacePanelScale | 0.50–1.50 | INFO settings panel |
| workspacePanelGap | 0–32 px | INFO settings panel |
| Персистентные ключи | workspacePanelScale, workspacePanelGap | INFO |
| OverlayManager stack capacity | 8 | Master Spec O1/O2 |
| FX EDIT pages | 8 | INFO visual contract |
| FX EDIT bypass switches | 7 (Flanger без 8-го) | INFO visual contract |
| Settings APVTS controls | 6 | INFO visual contract |

## 3. Sampler / Engine (подтверждено аудитом)

| Параметр | Значение |
|---|---|
| Snapshots на экземпляр | до 128 |
| Streaming library roots | до 8 |
| Voice slots | 64 (preallocated) |
| Cache miss | не блокирует audio thread |
| Stream stop | worker дренирует очередь |

## 4. Sound Hunter policy

| Параметр | Значение |
|---|---|
| SNR (GREEN) | >= 40 dB |
| Clipping | отсутствует |
| DC offset | <= 0.0001 |
| GREEN AUTO ADD лицензии | только CC0/PD с provenance + SHA-256 |
| YELLOW | manual import only |
| RED | не переопределяется |
| GOLD | tech check + human listening |
| Цели MELODY/PATTERN/PUNK | 30 / 50 / 40 (не достигнуты) |

## 5. Ограничения безопасности (неизменяемые инварианты)

- Skin Engine не меняет DSP, audio callback, sampler, MIDI, APVTS
  IDs/semantics, presets, SoundBank, поведение существующих контролов.
- Сетевые операции и анализ файлов — вне audio callback.
- Импорт .ppguskin: только allowlist (theme_manifest.json, PNG, SVG),
  quarantine + SHA-256; лимиты 64 MiB / 8 MiB; SVG без script/ENTITY/
  foreignObject/внешних href.

## 6. Правило изменения реестра

Любое изменение утверждённого значения требует: (1) отдельной записи в INFO
с обоснованием, (2) записи в CODE при затрагивании кода/схем/валидатора,
(3) обновления этого реестра. Нумерация версий реестра — только вверх.


## 7. Factory Preset Bank (v2, 2026-11-14)

| Параметр | Утверждённое значение |
|---|---|
| Заводской каталог | 500 авторских preset definitions |
| MELODY | 250 |
| PATTERN | 250 |
| Профили исполнителей | 21 |
| Семейства жанров/стилей | 16 |
| Melody archetypes | 20 |
| Pattern archetypes | 20 |
| Pattern grid | 16 шагов на такт 4/4; compact tuple-v1 |
| Reference policy | только широкие стилевые ориентиры; без копирования записей/риффов |

Статус: каталог определений создан и структурно проверен; это не означает,
что все 500 уже загружены в runtime или прошли прослушивание. Каждый preset
остаётся AUTHORED / RUNTIME VALIDATION PENDING до реализации загрузчика,
привязки sample zones/articulations, pattern-event mapping и проверки звука.
