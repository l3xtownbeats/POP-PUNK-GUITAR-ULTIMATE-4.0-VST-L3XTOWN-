# DOWNLOAD & INSTALL — REV.17

## Полный пакет
`POP_PUNK_GUITAR_ULTIMATE_Windows_x64_VST3_COMPLETE.zip` — создаётся GitHub Actions workflow (см. .github/workflows/build.yml) и публикуется как artifact run'а. Внутри: VST3 bundle, SKINS (52), PRESETS (300), SOUNDBANK (policy + источники), Resources (документация, лицензии, checksums).

## Текущий статус
- BUILD/CTest/pluginval: НЕ подтверждены на этом SHA — запусти workflow.
- FL Studio 20 host acceptance: ручная проверка после установки.

## Установка
1. Распакуй ZIP.
2. `VST3/*.vst3` → `C:\Program Files\Common Files\VST3\`.
3. `SKINS/` → рядом с bundle или в портативный content root `PPGU/SKINS/`.
4. Открой плагин в FL Studio 20 → IMPORT GUITAR → импортируй натуральные сэмплы (формат выбирается до импорта).
