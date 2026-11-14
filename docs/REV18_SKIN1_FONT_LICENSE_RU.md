# NEON STAGE — лицензированный font fallback

Для REV18 SKIN-1 добавлен Inter Variable из официального репозитория
`google/fonts`:

- файл: `SKIN/Skin_053/fonts/Inter[opsz,wght].ttf`;
- лицензия: SIL Open Font License 1.1;
- полный текст лицензии: `SKIN/Skin_053/fonts/OFL.txt`;
- источник: `google/fonts/ofl/inter`;
- контроль целостности: `font_manifest.json` — Git blob SHA-1, размер и CRC32.

Файл включён как optional cross-platform fallback. Текущие метаданные NEON STAGE
остаются Segoe UI / Consolas; загрузка Inter не включается автоматически.
До интеграции и тестирования загрузчика шрифтов ThemeManager активная типографика
не меняется. Шрифт распространяется вместе с полным OFL 1.1 notice.
