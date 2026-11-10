# Журнал багов — POP-PUNK-GUITAR-ULTIMATE

Дата обновления: 10 ноября 2026
Ветка: `migration/full-copy-m3-transport-scheduler-v2`
HEAD до записи: `56c507d844861b71e23c2c5c2b6b84287dde237a`

Статусы: **FIXED — AWAITING CI** означает, что правка внесена, но успешный актуальный build/test не подтверждён; **OPEN** — исправление или приёмка не завершены.

| ID | Приоритет | Область | Дефект / риск | Исправление или действие | Статус |
|---|---|---|---|---|---|
| M3-001 | High | MIDI chord runtime | Held chord обновлялся после планирования, новый аккорд запаздывал на один audio block. | Held MIDI state применяется до chord snapshot. | FIXED — AWAITING CI |
| M3-002 | High | MIDI timing | Не учитывался MIDI samplePosition при смене аккорда. | Старый сегмент рендерится до sample offset; смена применяется на точной позиции события. | FIXED — AWAITING CI |
| M3-003 | High | Pattern quantize | UI/APVTS предлагал 1/16 и 1/32, runtime использовал фиксированную 1/16. | Runtime читает patternQuantize, использует 0.25/0.125 PPQ; добавлены тесты. | FIXED — AWAITING CI |
| M3-004 | High | Pattern configuration | Смена pattern slot/length не перезапускала фразу на квантованной границе. | Отслеживаются предыдущие slot/length; конфигурационные переходы квантуются. | FIXED — AWAITING CI |
| M3-005 | High | Transport | Host seek мог оставить pending NoteOff и зависшие ноты. | PPQ discontinuity очищает pending gates; Processor отпускает sampler voices. | FIXED — AWAITING CI |
| M3-006 | High | Transport source | Переход Host→Internal и Internal→Host мог продолжить старую временную шкалу. | Оба направления handoff считаются discontinuity; добавлен regression test. | FIXED — AWAITING CI |
| M3-007 | Medium | Event timing | Округление sample offset могло поставить событие раньше фактического времени у границы блока. | Используется первый sample не раньше события; добавлен near-tail test. | FIXED — AWAITING CI |
| M3-008 | High | Pending gate capacity | Переполнение очереди NoteOff могло потерять событие и вызвать hanging note. | Увеличена bounded queue; overflow вызывает all-notes-off fail-safe; тест добавлен. | FIXED — AWAITING CI |
| M3-009 | Medium | APVTS validation | Regex валидатора неверно экранированы и давали ложные ошибки. | Экранирование исправлено; статическая проверка ID ранее прошла. | FIXED — AWAITING CI |
| M3-010 | High | Build verification | Промежуточные Windows CI запуски завершились failure; успешная сборка не подтверждена. | Исправления внесены; run 42769587683 ожидает завершения на момент записи. | OPEN |
| M3-011 | High | DAW integration | Не подтверждены playback/stop/seek/pattern transitions в FL Studio 20. | Ручной acceptance test на Windows 10 / FL Studio 20. | OPEN |
| M3-012 | Medium | Block-size stress | Полный runtime стресс при 64–1024 samples и BPM automation не подтверждён. | Проверить CTest/DAW при разных block sizes и изменениях темпа. | OPEN |

## Правила закрытия багов
1. Не закрывать баг только на основании изменения исходника.
2. Для runtime-багов нужны regression tests; для build-багов — успешный Windows CI на актуальном commit.
3. Для DAW integration нужны ручные результаты FL Studio 20 с версией хоста, sample rate, buffer size и сценарием.
4. Если CI обнаруживает регрессию, добавить новый ID либо обновить существующий статус и указать commit/run.