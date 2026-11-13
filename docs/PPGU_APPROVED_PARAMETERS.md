# PPGU — РЕЕСТР УТВЕРЖДЁННЫХ ПАРАМЕТРОВ (v2, 2026-11-13)

Назначение: единая точка сверки утверждённых числовых значений и канонических
констант. При конфликте приоритет у актуального Info.txt. Этот документ
консолидирует подтверждённые решения и отдельно помечает то, что требует
проверки в исходниках.

## 1. Theme / Skin Studio

| Параметр | Утверждённое значение | Статус / источник |
|---|---|---|
| Канонический набор цветовых токенов | 21 | утверждённый контракт; фактическая token parity ещё проверяется |
| Реестр тем | 52 темы | исправление прежнего ошибочного значения 102 |
| Default skin | Skin_052 / SIGNAL FORGE / NEON | утверждённый визуальный стандарт |
| Legacy schema | Skin_001..Skin_051 | сохранять обратную совместимость |
| Studio schema | Skin_052+ | не перезаписывать Skin_052 при добавлении новых тем |
| Skin usage | usage.json, только semantic token references | hex/rgb literals запрещены в usage |
| Runtime skin root | Skins/ рядом с VST3 bundle | не AppData |
| Skin package | allowlist + quarantine + SHA-256 | проверка вне audio callback |

## 2. Workspace / панели

| Параметр | Значение | Статус / источник |
|---|---|---|
| OverlayManager stack capacity | 8 | утверждённый O1/O2 контракт; source/CI в интегрированной ветке не подтверждены |
| workspacePanelScale | 0.50–1.50 | существующая настройка |
| workspacePanelGap | 0–32 px | существующая настройка |
| Persist keys | workspacePanelScale, workspacePanelGap | существующий контракт |
| FX EDIT pages | 8 | визуальный контракт |
| FX EDIT bypass switches | 7 | Flanger не получает дополнительный bypass без решения |
| Settings APVTS controls | 6 | существующий UI-контракт |

## 3. Sampler / Engine

| Параметр | Значение | Статус |
|---|---|---|
| Streaming library roots | максимум 8 | ограничение sampler |
| Voice slots | до 64 | preallocated sampler voices; не означает parallel DSP |
| Cache miss | не блокирует audio thread | realtime invariant |
| Snapshot reclamation | только после доказательства quiescence | realtime invariant |
| Audio callback | без locks, allocations, file I/O и logging | обязательный инвариант |

## 4. Sound Hunter / SoundBank policy

| Параметр | Значение | Статус |
|---|---|---|
| SNR GREEN | >= 40 dB | порог качества |
| Clipping | отсутствует | GREEN gate |
| DC offset | <= 0.0001 | GREEN gate |
| GREEN AUTO ADD | CC0/PD + provenance + SHA-256 | ограниченная политика |
| CC-BY | допустим при asset-level attribution | подтверждённая политика S1 |
| EXPLICIT_PERMISSION + legalReview=PENDING | блокируется | подтверждённая политика S1 |
| YELLOW | manual import only | политика |
| RED | запрещено; не переопределяется | политика |
| GOLD | tech check + human listening | политика |
| Цели MELODY/PATTERN/PUNK | 30 / 50 / 40 | цели, не достигнутые объёмы |

## 5. Secure skin import — ограничения

- Пакет .ppguskin: только allowlist manifest/theme metadata, PNG и SVG.
- Запрет path traversal, абсолютных путей, drive letters, скрытых/пустых сегментов.
- Лимиты, заявленные для O1/O2: архив 64 MiB, запись 8 MiB, 512 записей,
  путь 512 символов; подтвердить production implementation и CI.
- SVG: блокировать script, ENTITY, foreignObject, javascript:, event
  handlers и внешние href.
- SHA-256 provenance обязателен.
- Импорт/сеть/анализ файлов не выполняются в audio callback.

## 6. Правило изменения реестра

Изменение утверждённого значения требует отдельной записи в Info.txt
с обоснованием и exact SHA; при изменении кода/схем/валидатора обновляются
соответствующие исходники и тесты. Статус IMPLEMENTED/TESTED/BUILD VERIFIED
не выставляется без доказательства на точном commit SHA.
