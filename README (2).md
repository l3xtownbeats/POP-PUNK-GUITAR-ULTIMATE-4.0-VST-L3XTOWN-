<div align="center">

# POP PUNK GUITAR ULTIMATE
### SIGNAL FORGE · REV.17 · WINDOWS 10 x64 · VST3 · JUCE 7.0.12 · C++20

**A focused pop-punk guitar instrument for riffs, power chords, tight rhythm parts and melodic hooks.**

**PLAY. CREATE. ROCK.**

</div>

## Установка (Windows 10 x64)
1. Скопируй `.vst3` bundle в `C:\Program Files\Common Files\VST3\`.
2. Каталог `SKINS/` (52 темы) — рядом с bundle или в портативный `PPGU/SKINS/`.
3. Звук: плагин не содержит синтезированных сэмплов. Открой **IMPORT GUITAR**, выбери формат (WAV/FLAC/OGG/AIFF/MP3) и импортируй одиночные файлы или целые библиотеки натуральных гитарных сэмплов — движок сам распределит их в MELODY и PATTERN/PUNK GROUP.
4. Проверенные источники натуральных сэмплов: `docs/PPGU_GUITAR_SOUND_SOURCES_INDEX.md`.

## Функциональность
- **MELODY / PATTERN / PUNK GROUP** — три режима игры; паттерны с swing/humanize/accent/dynamics.
- **AUTOCHORDS** — колесо квинтового круга, прогрессии, pads.
- **FX CHAIN / FX EDIT** — Distortion, Chorus, Delay, Reverb, Compressor, Limiter, Cab Sim, Flanger + EQ.
- **52 темы (skins)** — кнопка SKINS или Ctrl+K; каждая тема проверена по 21 токену и 28 контролам.
- **Отдельные окна панелей** — FX EDIT, AUTOCHORDS, IMPORT GUITAR, CURATOR, SETTINGS, PERFORMANCE, ABOUT; тайлинг, состояние выживает закрытие.
- **300 factory presets** — 8 категорий, браузер с поиском/избранным/категориями.
- **Движок**: загрузка сэмплов по одному, выгрузка банка предыдущего пресета, без накопления памяти.

## Сборка из исходников
```
git clone --recurse-submodules <repo>
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

## Статус (честно, по журналу)
Исходники: IMPLEMENTED (source present). Гейты G0–G6: PASS (валидаторы).
BUILD VERIFIED / pluginval / FL Studio 20: НЕ подтверждены — закрывается GitHub Actions workflow из пакета.

---

**L3XTOWN AUDIO LAB** — *ENGINEERED FOR THE NEXT RIFF.*
