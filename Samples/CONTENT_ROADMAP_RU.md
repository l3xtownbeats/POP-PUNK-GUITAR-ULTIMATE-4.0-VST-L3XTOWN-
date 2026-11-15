# План наполнения Samples — от pop-punk до универсальной гитары

## Основной принцип
Собираем не один узкий поп-панк банк, а общую натуральную гитарную библиотеку. Поп-панк — приоритетная витрина и первый пресетный слой, но библиотека должна покрывать чистую, акустическую, альтернативную, панк-, рок-, джазовую и другие гитарные задачи. Не выдаём кандидатов и пустые категории за уже готовые банки.

## Очерёдность
1. **Melody / Multisamples** — чистая, DI, crunch/distortion, steel/nylon/classical; ноты по клавиатуре, velocity layers, round robins и release там, где они реально присутствуют в источнике.
2. **Pattern / Performance** — реальные исполненные power-chord riffs, down/up strokes, palm-muted eighth/sixteenth patterns, open chords, arpeggios, stops, fills, transitions, dead notes, slides и bends.
3. **OneShots** — отдельные естественные атаки, одиночные аккорды, muted hits, harmonics, pick/fret/string noises.
4. **Loops** — сухие и обработанные музыкальные фразы с BPM, тональностью, размером, строем и артикуляцией.
5. **Universal guitar coverage** — clean/ambient, indie/jangle, pop-rock, punk/skate-punk, emo/alternative, classic/hard rock, blues/jazz, acoustic strumming/fingerstyle и прочие подходящие направления.

## Сбор и проверка
- Сначала проверяем первоисточник и права на конкретный архив/файл.
- Затем сохраняем LICENSE/ATTRIBUTION, исходную карту SFZ, manifest и SHA-256.
- Техническая проверка: SNR >= 40 dB, отсутствие клиппинга, DC offset <= 0.0001, целостность.
- Отдельно проверяем, что это именно запись гитары; затем слушаем вручную GOLD-кандидатов.
- Пакетируем в фабричный SoundBank только GREEN/GOLD с доказанными правами.
- CC-BY — с точной атрибуцией; CC-BY-SA — только после совместимого правового анализа; NC/ND/неясные условия — не включать.
- Looperman не использовать в factory library без явного разрешения автора на VST embedding и redistribution as a sample library.
- Коммерческие библиотеки (включая Session Guitarist/Kontakt) не извлекать и не перепаковывать без отдельного письменного разрешения.

## Архитектура и поставка
- Не ограничивать число BANKs искусственным лимитом.
- Фабричные банки, пользовательские импорты и внешние разрешённые библиотеки держать раздельно.
- Индексация/хэширование/превью/декодирование — в фоне; аудиопоток — без дисковых операций и блокировок.
- Большие банки стримить/странично читать с ограниченными voice/cache ресурсами.
- Portable ZIP: VST3 bundle + соседний SOUNDBANK + PRESETS + SKIN/Resources + notices/manifest; без установщика и обязательного AppData.

## Важная граница текущего состояния
В Build.03 перенесены source inventory, workflow ingest и проверенная графическая оболочка NEON STAGE. CI уже сообщил успех скачивания 11 источников и проверки наличия лицензий/SFZ, но полный VST3 build, CTest, pluginval и конечная упаковка должны завершиться отдельно. Performance loops и one-shots пока являются задачами наполнения, а не заявленными готовыми банками.
