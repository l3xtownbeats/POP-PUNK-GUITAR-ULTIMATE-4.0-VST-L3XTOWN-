# PPGU — интерфейсная архитектура и UX-план REV17

## Принцип оформления
Коммерческое качество достигается в первую очередь качеством темы: материалами, пропорциями, сеткой, контрастом, типографикой, формой ручек, маркировкой и согласованностью. Анимации, glow, блиki и motion не являются обязательными. Предпочтителен статичный премиальный интерфейс с минимальным CPU/RAM overhead. Motion допускается только для функциональной обратной связи и должен иметь ECO/reduced-motion режим.

## Основная компоновка
1. Верхняя панель: бренд, выбор темы, Import Guitar, AutoChords (видимость/доступность зависит от режима), быстрые команды.
2. Левая зона: Preset Browser — поиск, категории MELODY/PATTERN, тип гитары/характер, чистые и обработанные пресеты, избранное/недавние, состояние источника.
3. Центральная зона: режим игры, музыкальные/игровые контролы и performance-информация.
4. Правая зона: AMP/TONE и FX rack; FX EDIT открывает подробное внутреннее окно.
5. Нижняя Performance Strip: chord/note monitor и on-screen keyboard. Клавиатура сворачивается в компактную полосу/строку и разворачивается обратно; состояние хранится отдельно от processor/audio state.

## MIDI keyboard и chord monitor
- Визуально подсвечивать реально удерживаемые MIDI-ноты холодным электрическим синим/ion-blue оттенком выбранной темы; без мигания, пульсации и постоянной анимации.
- При трёх удерживаемых клавишах подсветить все три клавиши одновременно и показать распознанное название аккорда. Для одной клавиши показывать имя ноты; для неоднозначных/неполных наборов показывать ноты и честную подпись, не выдумывать аккорд.
- Подсветка отражает MIDI note state, а не визуальную имитацию; не изменяет MIDI и звук.
- Сохранять различие held notes от mouse hover; hover должен быть слабее/отдельным состоянием.
- Диапазон клавиатуры, прокрутка и октавные метки должны быть понятны. Сворачивание клавиатуры не должно выключать MIDI input, менять режим игры или прекращать звучание.
- Chord monitor отображает ноты, имя аккорда, root/bass/inversion при достоверном определении, активный play mode и краткий status.
- Источник note-state должен быть thread-safe. UI читает snapshot/atomics на message thread; не опрашивать небезопасное состояние аудиопотока напрямую.

## AutoChords — уникальная расширенная механика PPGU
Базовый UX ориентир — специализированный chord helper из Session Guitarist, но с собственной реализацией и расширенным отображением:
- работает только в PATTERN; в MELODY функция программно отключена и не перехватывает, не меняет и не транспонирует ноты;
- рядом с кнопкой AutoChords показывать ON/OFF и доступность по режиму; в MELODY — disabled с понятной подсказкой «PATTERN only»;
- показывает распознанный chord name и фактически принятый chord/root/bass;
- пользователь может выбрать ноту/root для транспонирования; отдельно отображать исходный и транспонированный аккорд;
- chord recognition должен учитывать slash bass, inversions, power chords, sus, extensions и неоднозначные наборы; при неопределённости не применять неожиданную коррекцию;
- настройки диапазона/voicing, chord memory/hold, transition behavior и MIDI input mapping — только при наличии реальной engine logic;
- любые AutoChords transformations происходят в музыкальном input layer PATTERN до выбора performance phrase; не модифицируют исходный входящий MIDI поток для MELODY;
- bypass/OFF возвращает прямое поведение PATTERN. В MELODY AutoChords не должен создавать скрытых MIDI events.
- отображение входного аккорда и результирующего аккорда должно быть ясным и не перегружать основной экран.

## Менеджер внутренних окон
- FX EDIT, AutoChords, Import Guitar, Settings и другие реальные панели открываются внутри рабочей области плагина.
- Каждое окно имеет заголовок, независимую кнопку закрытия «×», maximize/restore и изменение размера при поддержке.
- Открытие нескольких окон — детерминированная раскладка; ни одно окно не исчезает за пределами workspace. При тесном пространстве используется прокрутка.
- Кнопка закрытия видна во всех темах и DPI. Закрытие одной панели не закрывает остальные.
- Все окна наследуют одну активную тему и её semantic tokens. Никаких hardcoded цветов в дочерних панелях.
- Состояние открытости/геометрии — UI-only persistence; не писать его в APVTS и не затрагивать audio state.
- Placeholder-панели нельзя показывать как готовые функции. Settings/About добавлять только с реальным содержимым.

## Система тем
- DEFAULT / NEON PUNK (сдержанный акцент), BLACK METAL, CYBER BLUE, DARK INDUSTRIAL, CLASSIC STUDIO, CARBON / STUDIO, CHROME / METAL, VINTAGE / CLASSIC, DARK PREMIUM.
- Каждая тема задаёт полную палитру, материалы, typography, knob style, panel borders, keyboard colors, selection/held-note colors, status colors и все auxiliary panels.
- Одна и та же семантика цветов во всех окнах. Скин не меняет APVTS IDs, значения параметров, MIDI routing, layout hit targets или DSP.
- Static artwork/textures должны быть оптимизированы по размеру и загружаться вне аудио-потока; кеш ограничен, пути внутри theme root.
- Полноценная gallery: preview card, family filter, поиск, избранное, клавиатурная навигация, DEFAULT и честный error state. Не фиксировать количество тем диапазоном 1..51.
- Reduced motion/ECO — default: никаких постоянных анимаций. Theme switching может быть мгновенным, если fade не нужен для корректности.

## Текущая проверка кода
В PluginEditor уже есть performance display с распознаванием chord/note names, MIDI keyboard и JUCE keyDown overlay. Это хорошая основа, но текущий overlay использует общий theme accent (не гарантированно ion-blue), а отдельная кнопка сворачивания клавиатуры в просмотренном PluginEditor.h/.cpp не найдена. AutoChords panel и chord display существуют, но реальную PATTERN-only routing/transpose engine behavior нужно проверить отдельно. Workspace README подтверждает hosting FX EDIT, AutoChords и Import Guitar; Settings пока не заявлен как готовый panel. Theme audit подтверждает неполный runtime token coverage, fixed 1..51 catalog и отсутствие полноценной Gallery.

## Очерёдность
P0: безопасность и корректность window manager; общая тема всех внутренних панелей; сворачивание клавиатуры без остановки MIDI; корректная note/chord visual state.
P1: AutoChords PATTERN-only contract, input/root transpose display, explicit OFF/bypass, no MELODY interception.
P2: ThemeManager schema migration, dynamic skin discovery, full Gallery, visual token parity, contrast and DPI.
P3: finish Settings/About only when functional; full control inventory parity; host usability and accessibility QA.

## Приёмка
- 100/125/150% DPI и разные размеры editor; 1..6 открытых окон, close/maximize/restore/resize/scroll.
- Переключение всех тем при открытых панелях; ни одного неподчинённого hardcoded цвета.
- Keyboard expanded/collapsed: одинаковая MIDI работа; chord display совпадает с удерживаемыми нотами.
- Ion-blue held-key highlight статичен, контрастен и различим от hover.
- AutoChords ON/OFF/root transpose в PATTERN; гарантированное отсутствие трансформации в MELODY.
- Skin change не меняет audio output/APVTS/MIDI; audio callback не получает UI work.
- Windows Release build, CTest, pluginval на точном SHA и визуальная проверка в FL Studio 20.
