# PPGU — журнал изменений

Журнал фиксирует значимые изменения требований, архитектуры, исходников, тестов и сборок. Текущий статус каждого компонента дополнительно отражается в `REV10_ENGINEERING_STATUS_RU.md`.

## 2026-11-11

### Документация и процесс разработки
- Добавлена постоянная политика сохранения документации и наработок: `DOCUMENTATION_AND_WORKLOG_POLICY_RU.md`.
- Установлено правило: существенные решения, исходники, тесты, результаты CI, ограничения и следующие шаги фиксируются в GitHub, а не остаются только в переписке.
- Зафиксированы статусы достоверности: PLANNED, DESIGNED, IMPLEMENTED, BUILT, TESTED, HOST-VERIFIED, BLOCKED.
- Создан этот журнал для дальнейшей хронологии.

### Ранее выполненные документированные этапы
- Добавлен проектный дизайн page cache/streaming: `SAMPLE_STREAMING_PAGE_CACHE_DESIGN_RU.md`.
- Добавлен план SB1 Bank Scanner: `SB1_BANK_SCANNER_IMPLEMENTATION_PLAN_RU.md`.
- Добавлен инженерный статус REV 10: `REV10_ENGINEERING_STATUS_RU.md`.
- Добавлен разбор интеграции модулей REV 10: `REV10_UNIFIED_MODULES_INTEGRATION_REVIEW_RU.md`.
- Добавлен контракт BankManifest и тесты JSON/валидации; эта часть требует подтверждения CI и не означает, что весь BankScanner уже реализован.
- Исправлена конфигурация интеграционного тестового target для линковки `PPGUPresetData`; итог CI должен фиксироваться отдельно после завершения запуска.

### Открытые задачи на момент записи
- Повторно проверить результат GitHub Actions run `43011601549`.
- Завершить SB1 BankScanner: canonical root containment с symlink-защитой, SHA-256, incremental scan, cancellation, atomic manifest replace и тесты.
- Не считать BankManifest полноценным сканером до выполнения этих задач и зелёного CI.
- После SB1 — общий PGP conversion service, затем безопасная интеграция LayerEngine, ImportManager и DiscoveryEngine.
- JUCE 8 migration выполнять отдельно; текущая база проекта — JUCE 7.0.12.

---
Новые записи добавлять сверху, сохраняя историю и ссылки на commit SHA/CI.
