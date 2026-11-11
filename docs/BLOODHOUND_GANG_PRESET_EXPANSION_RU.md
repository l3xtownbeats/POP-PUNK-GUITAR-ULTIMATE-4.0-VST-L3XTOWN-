# Bloodhound Gang — расширение каталога PPGU 5.0

Статус: добавлены 30 определений пресетов в factory catalog; аудиобиблиотека и отдельные performance-записи ещё не поставляются.

## Группировка в едином Preset Browser

Все варианты находятся под артистом Bloodhound Gang и разделены в фильтре групп по альбомной эпохе:

- **BLOODHOUND GANG / 2000 — HOORAY FOR BOOBIES**: The Bad Touch; The Ballad of Chasey Lain; Along Comes Mary; I Hope You Die; Mope; Three Point One Four; A Lap Dance Is So Much Better When the Stripper Is Crying; The Inevitable Return of the Great White Dope; Magna Cum Nada.
- **BLOODHOUND GANG / 2005 — HEFTY FINE**: Foxtrot Uniform Charlie Kilo; Uhn Tiss Uhn Tiss Uhn Tiss.
- **BLOODHOUND GANG / 2015 — HARD-OFF**: My Dad Says That's for Pussies.
- **BLOODHOUND GANG / LEGACY — 1996**: Fire Water Burn; Why's Everybody Always Pickin' on Me?; Kiss Me Where It Smells Funny.

Названия альбомов и годы сверены по дискографии. Hooray for Boobies вышел в 1999 году в ряде территорий и в 2000 году в США; для этого браузера используется запрошенная пользователем группа 2000. Три legacy-композиции не приписываются альбомам 2000/2005/2015: они выделены отдельно по оригинальной эпохе One Fierce Beer Coaster.

## Что добавлено

На каждую из 15 композиций созданы две независимые карточки:

- **MELODY** — одиночная нота с индивидуальной tonal-настройкой (drive, tone, reverb), без автоматического риффа.
- **PATTERN** — отдельное описание оригинального ритмического поведения, артикуляции, акцентов и chord-following для входящего MIDI.

Итого: **15 MELODY + 15 PATTERN = 30 новых определений**, общий каталог теперь содержит 165 + 165 = 330 карточек. Каждая карточка имеет уникальный ID, referenceAlbum, referenceYear, referenceTracks и browserGroup. Фильтр групп браузера теперь предпочитает browserGroup, сохраняя совместимость со старыми пресетами через fallback на referenceArtist.

## Звуки и лицензии — важное ограничение

Это оригинальные жанровые интерпретации тембра/исполнения, а не копии фонограмм. В репозиторий не добавлялись записи Bloodhound Gang, stems, риффы нота-в-ноту, чужие гитарные семплы или коммерческие аудиофайлы. Для реального звучания каждой карточке требуется совместимый лицензированный WAV/SFZ банк; до его подключения статус остаётся pending. Если подходящей артикуляции или sample bank нет, плагин не должен выдавать preset definition за готовый звук.

## Runtime readiness

Каталог компилируется через существующий JUCE BinaryData target. Текущая логика применения factory preset связывает drive/tone/reverb/output и несколько общих pattern-slot heuristics; это ещё не полноценные уникальные MIDI-performance clips для всех 15 песен. Полная слышимая реализация требует записи/подключения разрешённых sample zones, authored MIDI/performance clips и проверки каждой карточки в MELODY/PATTERN.
