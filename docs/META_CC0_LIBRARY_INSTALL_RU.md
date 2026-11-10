# Установка акустических библиотек Meta CC0

## Что устанавливается

Скрипт `tools/Download_Meta_CC0_Guitar_Libraries.ps1` загружает официальные WAV-релизы Meta Open Sound:

- **Meta Steel Guitar** — 375 WAV-сэмплов, E2–C6, 3 velocity layers.
- **Meta Nylon Guitar** — 348 WAV-сэмплов, E2–B5, 3 динамических слоя.

Обе инструментальные коллекции опубликованы под CC0 1.0 Universal. Скрипт сохраняет оригинальные SFZ-карты, LICENSE, manifest и SHA256SUMS, проверяет количество WAV, контрольные суммы и разрешимость OGG-ссылок в same-name WAV файлы.

Официальные источники:
- https://github.com/facebookincubator/Meta-open-sound
- https://github.com/facebookincubator/Meta-open-sound/releases

## Установка в Windows

1. Открой PowerShell в папке проекта.
2. Выполни:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\tools\Download_Meta_CC0_Guitar_Libraries.ps1
```

3. Дождись проверки SHA256 и сообщения об успешной установке.

По умолчанию библиотеки устанавливаются в:

`%APPDATA%\PopPunkGuitarUltimate\Samples\SourceLibraries`

## Загрузка в плагин

В текущем M2 Editor нажми **LOAD SFZ** и выбери карту:

- Steel: `Meta-Steel-Guitar\meta-steel-guitar.sfz`
- Nylon: `Meta-Nylon-Guitar\meta-nylon-guitar.sfz`

Точные имена и расположение файлов могут включать дополнительную вложенную папку из upstream ZIP; ориентируйся на фактический путь установленного `.sfz`.

## Текущие ограничения M2

- Загрузчик читает WAV; ссылки на `.ogg` в оригинальных Meta SFZ разрешаются поиском WAV с тем же именем.
- Для Meta Steel поддерживается основная channel-1 first-position карта, velocity zones и базовая логика group/off_by.
- Meta Nylon использует keyswitch и velocity crossfade opcodes. M2 пока не реализует полную обработку keyswitch/crossfade; default Auto mapping может играться упрощённо, но переключение Finger/Pick не обещается.
- Эти библиотеки — акустические steel/nylon гитары, а не настоящие электрогитарные записи для поп-панк ритмов. Distortion может обработать их, но не превращает их в электрогитарный семплсет.
- Установка библиотек не означает, что весь финальный электрогитарный банк, factory preset browser или FL Studio 20 ручной тест уже готовы.

Не удаляй LICENSE и SHA256SUMS из установленных папок.
