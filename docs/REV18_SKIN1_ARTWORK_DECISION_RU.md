# NEON STAGE — artwork package REV18

Статус: **[DECISION] — оригинальные artwork-ресурсы SKIN-1**.
Область: hero, preview, фон, carbon texture, baked FX overlays и texture atlas.
Это отдельный визуальный пакет; он не меняет APVTS, JUCE layout ownership,
MIDI, preset state, DSP, sampler или audio path.

## Причина и принцип

В исходном `Skin_053` были только низкоразмерные hero/preview, без master-файлов,
фона, текстур, FX layers и atlas. Старые `assets/hero.png` и `assets/preview.png`
сохранены без изменений и продолжают обслуживать legacy loader. Новый комплект
создаётся отдельно в REV18 master/runtime каталогах. Ресурсы рисуются заново на
целевых разрешениях; runtime-файлы не получаются растяжением старых картинок.

## Состав и размеры

| Ресурс | Master | Runtime |
|---|---:|---:|
| Hero | 1600×900 | 800×450 |
| Gallery preview | 720×480 | 360×240 |
| Main background | 2560×1440 | 2560×1440 |
| Carbon weave texture | 1024×1024 | 1024×1024 |
| Stage glow overlay | 1600×900 | 800×450 |
| Edge highlight overlay | 1600×900 | 800×450 |
| Icon atlas | 768×512 | 768×512 |

Texture atlas содержит шесть TOP BAR icon frames: `skins`, `import_guitar`,
`auto_discover`, `curator`, `settings`, `about`. Координаты и размеры кадров
зафиксированы в `atlas/atlas.json`.

## Генерация и воспроизводимость

`tools/generate_rev18_skin_artwork.py --root SKIN` создаёт master/runtime PNG,
atlas и `assets/artwork_manifest.json`. Режим `--check` повторно вычисляет
каждый ресурс и сравнивает байты, включая manifest. Все файлы имеют
SHA-256, CRC32, byte count и `[DECISION]` provenance в artwork manifest.

## Ограничения и статус

Это первая оригинальная итерация artwork, а не заявление о полном коммерческом
финальном дизайне. Перед release нужны визуальная приёмка в реальном JUCE UI,
проверка сочетания с зонами A–H и проверка качества на 100%/HiDPI. Control/state sprite package (270 master/runtime
файлов) и Inter fallback с полным OFL 1.1 notice добавлены отдельными ресурсными
этапами. Их подключение к runtime UI и загрузка fallback-шрифта пока не включены.

Baked overlays являются ресурсами оформления. Их наличие не включает эффекты
автоматически и не добавляет новые FX controls или DSP-пути.
