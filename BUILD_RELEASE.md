# BUILD & RELEASE — журнал багов, фиксов и сборок

**Назначение:** единый технический журнал ошибок, исправлений, правок, CI, тестов, сборок и релизных пакетов.  
**Формат записи:** дата · commit SHA · bug/change ID · причина · исправление · проверка · статус.

## Статусы
- **OPEN** — проблема подтверждена, исправления нет.
- **FIXED IN SOURCE** — изменение внесено, но проверка ещё не прошла.
- **PASS / VERIFIED** — указанная проверка успешно выполнена на конкретном commit.
- **BLOCKED** — дальнейшая работа зависит от внешнего условия.
- **NOT REPRODUCED** — проблема не воспроизведена; не считать исправленной.

## Открытые технические риски

| ID | Проблема / риск | Статус | Следующее действие |
|---|---|---|---|
| BR-001 | Безопасное освобождение старых SamplePool snapshots при активных audio readers | OPEN | Ввести и протестировать quiescence/epoch или эквивалентную схему |
| BR-002 | Лимиты до 128 snapshots и 8 streaming roots на экземпляр; рост памяти/потоков | OPEN | Профилировать и разработать безопасную reclamation/resource policy |
| BR-003 | Streaming worker не восстанавливается автоматически после исключения | OPEN | Тест worker failure и определить recovery/recreate policy |
| BR-004 | Cache starvation / queue overflow при высокой полифонии и loop prefetch | OPEN | Stress tests с 64 voices, ограниченным cache и быстрыми note on/off |
| BR-005 | Cancellation/destruction races во время SFZ load и streaming service lifecycle | OPEN | Проверить отмену на всех этапах, уничтожение processor и отсутствие UAF/hang |
| BR-006 | Pattern Scheduler ещё не прошёл полную runtime-приёмку с реальным SamplerEngine и DAW | OPEN | Интеграционные тесты host start/stop/seek/restart и FL Studio 20 acceptance |
| BR-007 | SoundBank/DiscoveryEngine end-to-end и SOUND HUNTER DEEP SEARCH не реализованы | PLANNED | Сначала проверить S6.5 contract, затем policy/provenance tests |

## Последние подтверждённые изменения в исходниках

### 2026-11-11 — SamplePool / SamplerEngine lifecycle audit
- Commit: `74c7df1b0ba7b6307d759400592adec61f788e7f`.
- Результат: проведён аудит lifecycle; зафиксированы snapshot retention, пределы 128 snapshots / 8 roots, 64 voice slots, non-blocking cache miss и unrecoverable worker exception.
- Статус: AUDIT RECORDED. Production code не менялся; сборка и тесты этим аудитом не запускались.

## Архитектурные и продуктовые изменения

### 2026-11-11 — Общий Audio Engine для Guitar и Bass
- Commit документа: `855f21c00f967483903cd57a3896a3ba72cd260d`.
- Решение: общий низкоуровневый Engine, отдельные Guitar Edition и Bass Edition, раздельные product profiles / SoundBanks / presets / articulations.
- Статус: DESIGNED. Отдельный production Engine target и Bass VST ещё не реализованы.

### 2026-11-11 — REV 10.1 SOUND HUNTER
- Commit спецификации: `cdcb02dea03d2098bb1c515089191be3f330ee0f`.
- Commit записи в журнал: `69161d90a78653e7304f809a8dc0c22afdc4f113`.
- Зафиксированы asset-level GREEN/YELLOW/RED policy, provenance, SHA-256, guitar-only gate, Deep Search, style profiles, curator review и честные preset counts.
- Статус: DESIGNED. Worker, classifier, curator UI, автоматический поиск, новые sounds и H9 tests не подтверждены как реализованные.

## История CI / релизов

Старые успешные CI runs и артефакты относятся только к тем commit SHA, которые указаны в исходных GitHub Actions runs. Они не доказывают успешную сборку текущего HEAD. Не считать старый артефакт последним релизом без повторной проверки.

При добавлении новой записи CI обязательно указывать:
1. GitHub Actions run ID и URL;
2. точный commit SHA;
3. результат каждого job;
4. CTest summary;
5. pluginval version/strictness и результат, если запускался;
6. artifact ID, размер и SHA-256;
7. known limitations и срок действия artifact;
8. отдельно — подтверждён ли запуск в FL Studio 20.

## Шаблон новой записи

```text
### YYYY-MM-DD — краткое название
- Commit:
- Bug/Change ID:
- Причина:
- Изменение:
- Проверка: NOT RUN / FAIL / PASS (команды и результат)
- CI run:
- Artifact:
- Статус:
- Осталось:
```

## Правило приёмки

Не закрывать баг только потому, что код изменён. Для статуса PASS указывать конкретный тест/CI run на commit, содержащий исправление. Сборка, CTest, pluginval и ручная проверка в DAW — разные уровни подтверждения.
