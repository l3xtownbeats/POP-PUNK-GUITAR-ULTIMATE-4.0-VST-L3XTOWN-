# PR #24 — аудит интеграции системы тем (SKIN REV 5.0)

Дата проверки: 11 ноября 2026. PR #24: `ui/modern-neon-stage-shell` → `main`, открыт, не слит. Head SHA: `dfd4968a4b600c6b6c0d59601c7c7115cb9f2e1a`.

## Решение

**Не сливать PR #24 целиком.** Он содержит полезные наработки по тематизации, но в текущем виде не соответствует новому контракту SKIN REV 5.0 и выходит за разрешённый scope.

## Найденные расхождения

1. Кнопка называется `THEMES`, тогда как актуальный пользовательский выбор зафиксирован как единственная кнопка `SKIN`.
2. UI использует `juce::PopupMenu`, а не полноценную Theme Gallery с preview-карточками, Live Preview, Apply, Cancel и Restore Default.
3. Runtime-палитра ограничена небольшой структурой `ThemePalette`; она не загружает полный data-only manifest с 18 token groups, thumbnails, asset paths и per-theme validation.
4. PR включает `Themes/theme_manifest.json` в BinaryData. Это другой формат и другой путь, не совместимый автоматически с новым `SKIN/theme_manifest.json` и `SKIN/Skin_XX/theme.json`.
5. PR меняет минимальный размер редактора на 1040×820, что расходится с целевым минимумом 1040×760 и требует отдельного layout proof.
6. PR меняет видимые подписи FX-кнопок (COMPRESSOR→COMP, DISTORTION→DRIVE, CABINET IR→CAB IR). Skin contract запрещает менять названия контролов.
7. PR меняет `PluginProcessor.cpp/.h` и добавляет audio-thread held-note publication. Это не относится к системе тем и нарушает текущую границу scope.
8. В PR есть полезная основа для применения палитры на message thread и сохранения UI theme identifier, но она не доказывает сохранение полного состояния при смене темы и не заменяет runtime acceptance.

## Разрешённое использование

- Рассмотреть изолированно подход к message-thread theme application и сохранению UI theme identifier.
- Не переносить аудиоизменения из PR #24 в работу Skin.
- Не переносить старую схему `Themes/theme_manifest.json` без явной миграции.
- Реализовать новый ThemeManager поверх текущего main и актуального `SKIN/theme.schema.json`.
- Сохранить текущие названия, layout semantics, APVTS IDs, attachments и hit areas.

## Статус

`PR24 REVIEWED / NOT SAFE TO MERGE AS-IS`. Никакой merge или cherry-pick не выполнялся. Main остаётся рабочей веткой.
