<div align="center">

<img src="Cover%20Interface.jpg" alt="POP PUNK GUITAR ULTIMATE — утверждённый внешний вид интерфейса" width="100%">

# POP PUNK GUITAR ULTIMATE 4.0
### L3XTOWN · SIGNATURE POP-PUNK GUITAR INSTRUMENT

**RIFFS · HOOKS · POWER CHORDS · LEADS · LOUD CHORUSES**

*PLAY. CREATE. ROCK.*

</div>

---

## THE SOUND OF POP-PUNK. YOUR OWN INSTRUMENT.

**POP PUNK GUITAR ULTIMATE (PPGU)** — самостоятельный нативный гитарный VST3-инструмент для pop-punk, skate-punk, melodic punk и alternative rock. Проект создаётся как быстрый и компактный инструмент с собственной sample-engine архитектурой, библиотекой звуков, единым браузером пресетов и двумя режимами исполнения.

**Целевая среда:** Windows 10 x64 (1809+) · FL Studio 20 x64 · VST3.  
**Технологии:** JUCE 7.0.12 · C++20 · CMake · Visual Studio 2022 / MSVC v143.

> **Статус:** активная разработка. Наличие функции в техническом задании или каталоге не означает, что она уже полностью подключена к движку. Сборка CI, автоматические тесты и ручная проверка в FL Studio отмечаются отдельно.

## ДВА РЕЖИМА — ОДИН ИНСТРУМЕНТ

| MELODY | PATTERN |
|---|---|
| Прямая хроматическая игра по MIDI-нотами: риффы, лиды, соло, арпеджио и аккорды | Игра от MIDI-аккордов: ритмические рисунки, бой, риффы и арпеджио с синхронизацией по темпу хоста |
| Одна MIDI-клавиша должна запускать соответствующую гитарную ноту | Новый MIDI-аккорд должен менять гармонию/voicing, сохраняя выбранный ритмический рисунок |

Режимы используют общую идентичность пресета и общий Sounds Bank. В целевой UX-модели выбор, например, Blink-182 в едином браузере открывает связанные варианты звучания и исполнения, а не заставляет искать их в двух независимых каталогах.

## PRESET CATALOGUE

В репозитории есть два JSON-файла по 300 определений:
- `presets/factory_bank_300.json` — текущий основной каталог: **150 MELODY + 150 PATTERN**, 300 записей.
- `presets/factory_preset_bank_v1.json` — предыдущая версия/схема каталога, также 300 записей; это не ещё 300 уникальных пресетов. Суммировать их как 600 нельзя.

Основной каталог охватывает **21 style-family**: Blink-182, Sum 41, Simple Plan, New Found Glory, Good Charlotte, Mest, Angels & Airwaves, The All-American Rejects, MGK, Avril Lavigne, The Offspring, Green Day, Bowling for Soup, Sugarcult, All Time Low, Fall Out Boy, Yellowcard, My Chemical Romance, Less Than Jake, Fenix TX и Midtown.

Это оригинальные стилевые ориентиры, не официальные пресеты артистов и не копии коммерческих записей. Каталог definitions ещё требует полной связи с sample profiles, реального воспроизведения и проверки каждого пресета в хосте. См. [Preset readiness](docs/PRESET_BANK_4_0_READINESS_RU.md) и [основной JSON-каталог](presets/factory_bank_300.json).

## SOUNDS BANK — FACTORY + USER LIBRARIES

Текущий CI pipeline заявляет пять заводских исходных библиотек:
- Meta Open Sound Steel Guitar — 375 WAV, CC0 1.0.
- Meta Open Sound Nylon Guitar — 348 WAV, CC0 1.0.
- FreePats FSBS Clean #1 — CC0 1.0.
- FreePats FSBS Distorted #1 — CC0 1.0.
- FreePats FSBS Distorted #2 — CC0 1.0.

Это даёт **723 Meta WAV** плюс содержимое FreePats банков, фактическое число и контрольные суммы которого фиксируются в manifest конкретного пакета. Расширенный каталог кандидатов включает Karoryfer Emilyguitar, Shinyguitar, Black And Green Guitars, FreePats Direct DI, Clean #2/Jazz и Spanish Classical Guitar. Они не считаются включёнными в пакет, пока не пройдут скачивание, лицензионную проверку, SHA-256, SFZ mapping и playback tests.

Подробности: [Sounds Bank Catalog](docs/SOUND_BANK_CATALOG_RU.md) · [политика источников и лицензий](docs/SOUND_ASSET_SOURCES_AND_LICENSE_POLICY_RU.md) · [система BANK](docs/BANK_LIBRARY_SYSTEM_RU.md).

## IMPORT FROM SESSION GUITARIST — IMPORTANT

Пользовательские звуки из **Native Instruments Session Guitarist – Electric Sunburst Deluxe** и **Electric Vintage Deluxe** можно использовать в FL Studio как отдельный Kontakt-инструмент на отдельном канале, параллельно с PPGU. PPGU не будет извлекать или перепаковывать защищённые NI samples из установленных Kontakt-библиотек, читать закрытые форматы `.nkx/.nkc` или обходить Kontakt/Native Access. Лицензия NI разрешает использовать контент для музыкальных произведений, но запрещает превращать его в стороннюю sound library или sample-based instrument без отдельного разрешения.

Для PPGU предусмотрен легальный путь расширения:
1. Импорт собственных записей гитары и сторонних WAV/SFZ-библиотек, чья лицензия разрешает такое локальное использование.
2. Автоматическое построение/редактирование SFZ-style mapping: root key, key range, velocity layers, round robin, articulation, release/loop.
3. Создание похожего по назначению, но оригинального звучания — через разрешённые multisamples, собственные записи, velocity/articulation layers и собственную цепочку FX.
4. При желании оставить Electric Sunburst/Vintage Deluxe в отдельном Kontakt instance и смешивать его с PPGU непосредственно в микшере FL Studio.

См. [NI EULA](https://www.native-instruments.com/de/pages/end-user-license-agreement) и [правила импорта/лицензирования PPGU](docs/SOUND_ASSET_SOURCES_AND_LICENSE_POLICY_RU.md). Прямой перенос NI sample pool внутрь PPGU не заявляется.

## ENGINE & PERFORMANCE

В исходном коде реализованы SFZ/WAV-oriented sampler components, выбор зон по MIDI note/velocity/channel, ограничение голосов, loop handling, background streaming service, bounded page cache и отдельный worker. DSP-путь включает базовые distortion/drive, chorus, stereo delay и reverb. Параметры плагина хранятся через APVTS.

Главный инженерный принцип — не загружать всю библиотеку в память при выборе пресета. Метаданные индексируются отдельно, активный профиль подготавливается асинхронно, короткие участки подгружаются через bounded cache, а аудиопоток не должен выполнять файловый I/O, блокирующие операции или непредсказуемые аллокации.

Текущие оптимизации включают semaphore-based sleep для idle streaming worker вместо периодического опроса с интервалом 1 ms и упрощение вычислений компрессорной кривой в audio callback. Эти изменения проходят текущий Windows CI. Конкретные CPU/RAM/startup/preset-switch цифры не публикуются до измерений на целевой машине.

## TECHNICAL SPECIFICATIONS

| Параметр | Значение / статус |
|---|---|
| Product type | Native guitar sampler / rompler / performance instrument |
| Plugin | VST3 instrument, x64 |
| OS target | Windows 10 x64, build 1809+ |
| DAW target | FL Studio 20 x64 |
| Framework | JUCE 7.0.12 |
| Language | C++20 |
| Build system | CMake 3.22+ / Visual Studio 2022 |
| Sample maps | SFZ |
| Main audio format | WAV |
| Output | Stereo |
| MIDI | Note, velocity, channel; chord input for Pattern |
| Voice cap | Up to 64 voices |
| Factory catalogue | 300 definitions (150 Melody + 150 Pattern) |
| Style families | 21 |
| Factory sound sources | 5 source-bank families in CI pipeline |
| Package target | Windows x64 VST3 + Samples + Presets + manifests |
| UI direction | Approved black/charcoal panel with magenta accents and guitar artwork |

## UI / VISUAL IDENTITY

В качестве главного изображения проекта используется утверждённый макет **Cover Interface.jpg**: тёмная аппаратная панель, magenta controls, блоки MELODY/PATTERN, AMP & TONE, FX CHAIN, PLAYBACK и клавиатура. Это визуальный эталон, а не утверждение, что все показанные элементы уже подключены к runtime. Полноценный Theme/Skin manager с набором переключаемых оболочек остаётся отдельной задачей разработки.

## PROJECT STATUS & VALIDATION

Текущая Windows CI run: [#618 — VST3 build](https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42958088120). На момент обновления README компиляция, CTest и pluginval strictness 5 завершились успешно; workflow ещё упаковывает VST3 + samples + presets и загружает artifact. Поэтому итоговый пакет этой run пока не объявляется готовым.

Ручная приёмка в FL Studio 20 на Windows 10 Enterprise 1809, проверка каждого пресета/артикуляции, измерения cold/warm load и CPU/RAM остаются обязательными release gates.

## OFFICIAL PROJECT LINKS

- **Repository:** https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-
- **Active development branch:** https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/tree/feature/pattern-melody-fx-spec
- **Builds & CI:** https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions
- **Current Windows VST3 run:** https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/actions/runs/42958088120
- **Main preset bank:** https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/blob/feature/pattern-melody-fx-spec/presets/factory_bank_300.json
- **Sounds Bank catalogue:** https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/blob/feature/pattern-melody-fx-spec/docs/SOUND_BANK_CATALOG_RU.md
- **Technical product profile:** https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/blob/feature/pattern-melody-fx-spec/docs/PRODUCT_PROFILE_TECHNICAL_RU.md
- **Implementation audit:** https://github.com/l3xtownbeats/POP-PUNK-GUITAR-ULTIMATE-4.0-VST-L3XTOWN-/blob/feature/pattern-melody-fx-spec/docs/IMPLEMENTATION_AUDIT_2026-11-11_RU.md

---

**L3XTOWN · POP PUNK GUITAR ULTIMATE 4.0**  
*An independent guitar instrument built for the energy of pop-punk.*
