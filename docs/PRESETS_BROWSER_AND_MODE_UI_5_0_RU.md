# PRESETS BROWSER + INSTRUMENT MODE — PPGU 5.0 UI CONTRACT

Этот документ фиксирует обязательную структуру браузера пресетов и переключение режимов инструмента. Это спецификация реализации и критерий приёмки; описанные функции нельзя считать готовыми только на основании этого документа.

## 1. PRESETS BROWSER

В верхней части главного интерфейса находится единый блок **PRESETS BROWSER** с четырьмя самостоятельными разделами/вкладками в следующем порядке:

1. **PUNK GROUPS**
2. **MELODY**
3. **STYLE**
4. **USER**

Каждая вкладка показывает собственный список пресетов и имеет независимое состояние выбора/фильтрации. Переключение вкладки не должно самопроизвольно загружать другой пресет или менять режим MELODY/PATTERN.

### NOT SOUND — первый пункт каждой вкладки

Самой первой строкой списка в КАЖДОЙ из четырёх вкладок всегда находится специальный пункт **NOT SOUND**.

Поведение NOT SOUND:
- Это штатный пустой/беззвучный выбор, а не обычный гитарный пресет.
- При выборе останавливается предпрослушивание и звучание предыдущего выбранного пресета.
- Не загружаются сэмплы и не запускаются ноты/паттерны.
- Состояние эффектов и другие настройки не должны неожиданно сбрасываться, если пользователь отдельно не выбрал команду сброса.
- Пункт всегда остаётся первым, даже после поиска, фильтрации, импорта новых звуков или добавления USER-пресетов.
- При открытии новой вкладки её начальное значение — NOT SOUND, если пользователь ещё не выбирал пресет в этой вкладке. Можно сохранять последнее выбранное значение каждой вкладки в состоянии экземпляра плагина.

### Содержание вкладок

**PUNK GROUPS**
- Пресеты сгруппированы по артистам/панк-группам из утверждённого каталога.
- Выбор артиста показывает его пресеты.
- Не смешивать названия артистов с категориями MELODY или STYLE.
- NOT SOUND всегда первый элемент общего списка/пункта выбора.

**MELODY**
- Пресеты тембра и характера звучания: Clean, Bright Clean, Chorus Clean, Tremolo Clean, Acoustic, Nylon, Muted, Palm Mute, Crunch, Drive, Distortion, Lead и другие утверждённые варианты.
- Это библиотека звуков/тембров, а не переключатель режима исполнения.
- NOT SOUND всегда первый элемент.

**STYLE**
- Пресеты манеры исполнения и ритмической техники: Lead Guitar, Solo, Octave Hooks, Alternate Picking, Tight Downstrokes, Strumming, Palm-Muted Eighths, Gallop, Arpeggio, Ska Upstrokes и т. п.
- Не путать стиль исполнения с тембровой категорией MELODY.
- NOT SOUND всегда первый элемент.

**USER**
- Пользовательские пресеты, созданные через Save As или сохранённые из текущих настроек.
- Поддержать загрузку, сохранение, переименование и удаление USER-пресетов; удаление требует подтверждения.
- USER-пресет хранит полный набор параметров и ссылки на используемые локальные банки/инструменты, не копируя большие аудиофайлы в каждый пресет.
- NOT SOUND всегда первый элемент; он не является пользовательским файлом.

## 2. IMPORT GUITAR / SOUNDS BANK

**IMPORT GUITAR** — единственное пользовательское название функции импорта и управления гитарной библиотекой.

- Надписи **IMPORT SOUNDS** и отдельная кнопка/раздел **Sounds Bank** как дублирующие названия не использовать.
- В главном интерфейсе одна кнопка **IMPORT GUITAR**.
- Через неё пользователь открывает единый экран/панель, где можно импортировать файлы, папки, ZIP/SFZ, просматривать и управлять библиотекой Sounds Bank.
- **Sounds Bank** остаётся внутренним названием самой библиотеки/хранилища, а не второй кнопкой или отдельной конкурирующей функцией.
- Импортированные звуки могут использоваться при создании и обновлении пресетов после проверки карты нот, velocity и поддержки формата.
- Не обещать playback формата, пока декодер и sampler-путь для него не реализованы и не протестированы.

## 3. MELODY / PATTERN — переключатель рабочего режима инструмента

**MELODY** и **PATTERN** — две основные кнопки-переключателя режима инструмента на главном экране. Это не фильтры браузера и не декоративные вкладки.

- Выбрана **MELODY**: инструмент работает как обычный хроматический инструмент для непосредственного исполнения отдельных нот, мелодий, соло и вручную сыгранных аккордов. Одна нажатая клавиша = одна нота/один звук. Несколько одновременно нажатых клавиш = соответствующие одновременно звучащие ноты; например, три клавиши дают трезвучие/аккорд. Аккорд в этом режиме — именно комбинация нот, которую музыкант нажал сам, а не автоматически сформированный ритмический аккомпанемент. Каждая нота имеет самостоятельную высоту, velocity и длительность; ноты звучат, пока удерживается соответствующая клавиша (с учётом огибающей и release). Никакой автоматической смены боя, перебора, ритмического рисунка или аккомпанемента в режиме MELODY нет.
- Выбран **PATTERN**: отдельный режим исполнения гитарных ритмических паттернов/аккомпанемента. Клавиша запускает выбранный стиль/рисунок, а высота клавиши определяет соответствующую ноту или аккорд согласно карте паттерна. Ритмическая последовательность исполняется движком; это не простое одновременное удержание отдельных нот, как в MELODY.
- Пример различия: в MELODY нажать C, E и G одновременно — прозвучат три ноты трезвучия. В PATTERN нажать одну клавишу — инструмент исполнит заданный ритмический рисунок/бой для соответствующей гармонической позиции.
- В каждый момент активен ровно один режим.
- Активная кнопка визуально выделена; при переключении обновляется фактический параметр движка/APVTS, а не только внешний вид.
- Переключение режима не должно автоматически менять вкладку браузера или выбранный пресет.
- Состояние режима сохраняется в состоянии плагина/проекте DAW и восстанавливается при повторном открытии.
- MIDI-вход, MIDI-клавиатура плагина и MIDI Editor должны учитывать текущий режим.
- Если выбран NOT SOUND, переключение режима не должно создавать звук.

### Доступность пресетов в режиме MELODY

- При активном режиме исполнения **MELODY** пользователь может выбирать и использовать пресеты из всего браузера: **PUNK GROUPS, MELODY, STYLE и USER**. Ни одна вкладка или группа пресетов не должна скрываться или блокироваться только из-за того, что выбран режим MELODY.
- В частности, пресет артиста/группы из PUNK GROUPS, пресет тембра из MELODY, стиль из STYLE и пользовательский пресет из USER должны быть доступны для выбора в режиме MELODY.
- Выбор пресета не должен самопроизвольно переключать режим исполнения на PATTERN. В режиме MELODY любой выбранный совместимый пресет используется для непосредственной игры нотами; автоматический ритмический рисунок не запускается.
- Совместимость конкретного пресета с режимом должна обрабатываться предсказуемо: использовать его тембр и применимые параметры, но не включать PATTERN-движок без явного переключения пользователем.

## 4. Разделение понятий — обязательное

- **PUNK GROUPS / MELODY / STYLE / USER** — вкладки/разделы браузера пресетов.
- **MELODY / PATTERN** — режим работы звукового движка.
- **IMPORT GUITAR** — единая кнопка импорта гитарных звуков и доступа к Sounds Bank.
- **NOT SOUND** — первый беззвучный пункт каждой вкладки браузера.

Одинаковое слово MELODY в браузере и в переключателе режима обозначает разные уровни интерфейса. Их состояния независимы: вкладка MELODY в браузере не обязана включать режим MELODY, а режим PATTERN не должен скрывать вкладку MELODY.

## 5. Acceptance checklist

- [ ] В браузере ровно четыре верхнеуровневые вкладки: PUNK GROUPS, MELODY, STYLE, USER.
- [ ] NOT SOUND — первый пункт в каждой вкладке и остаётся первым при поиске/фильтрации.
- [ ] NOT SOUND останавливает предыдущий звук/preview и не запускает новые ноты.
- [ ] USER-пресеты изолированы от Factory и имеют операции Save As / Rename / Delete.
- [ ] В интерфейсе одна кнопка IMPORT GUITAR; нет дублирующей кнопки IMPORT SOUNDS.
- [ ] Sounds Bank открывается/управляется из IMPORT GUITAR.
- [ ] MELODY/PATTERN — две взаимоисключающие кнопки, переключающие фактический режим движка.
- [ ] В MELODY одна клавиша запускает одну ноту; одновременное нажатие нескольких клавиш звучит как самостоятельно сыгранный музыкантом аккорд (без автоматического ритма/аккомпанемента).
- [ ] В MELODY ноты сохраняют самостоятельные высоту, velocity и длительность; note-off/release корректно обрабатываются.
- [ ] В PATTERN одна клавиша запускает заданный ритмический рисунок/аккомпанемент для соответствующей гармонической позиции.
- [ ] Браузерные вкладки и рабочий режим движка независимы.
- [ ] В режиме исполнения MELODY доступны для выбора пресеты из всех вкладок браузера: PUNK GROUPS, MELODY, STYLE и USER.
- [ ] Все состояния корректно сохраняются и восстанавливаются из проекта DAW.
- [ ] Проверено в VST3 и вручную в FL Studio 20; CI-прохождение само по себе не заменяет ручную проверку.

## 5. FX EDIT — единый центр редактирования усилителя и эффектов

Отдельные главные кнопки, секции или страницы **AMP** и **TONE** не нужны: их функции дублируют параметры обработки, которые должны находиться в **FX EDIT**. Убрать самостоятельные AMP/TONE элементы управления с главного экрана, не удаляя сами звуковые возможности и параметры из движка.

- На главном экране остаётся одна кнопка **FX EDIT**.
- Нажатие **FX EDIT** открывает редактор как внутреннюю страницу/вкладку внутри окна самого плагина. Не открывать отдельное системное окно и не выводить пользователя из интерфейса инструмента.
- Внутри FX EDIT собрать все настройки усилителя, тембра и эффектов: Amp Gain / Amp Tone, Drive / Distortion, EQ (Low / Mid / High и частоты/Q), Compressor, Chorus, Flanger, Delay, Reverb, Cabinet / IR, Double Track и Output.
- AMP/DISTORTION может оставаться названием модуля внутри FX EDIT; это не отдельная кнопка главного интерфейса.
- Параметры AMP и TONE не должны существовать в двух местах интерфейса. Каждый параметр редактируется через свой модуль FX EDIT и управляет тем же самым параметром DSP/APVTS.
- Сохранение пресета должно сохранять настройки всех модулей FX EDIT. Загрузка пресета должна восстанавливать их без расхождения между отображаемыми значениями и звуком.
- В правом верхнем углу страницы FX EDIT должна быть кнопка закрытия: небольшой **красный квадрат с чёрным крестиком (X) внутри**, визуально как кнопка закрытия окна в браузере. Нажатие закрывает страницу FX EDIT и возвращает на главный экран плагина.
- Дополнительно разрешена небольшая надпись **BACK** в нижней части страницы, сбоку (не в центре и не вместо кнопки X). Это вспомогательная подпись/навигационная надпись; если она оформлена как кнопка, её действие — то же закрытие FX EDIT и возврат на главный экран. Надпись **MAIN** не добавлять.

### Acceptance checklist — FX EDIT
- [ ] На главном экране нет самостоятельных AMP и TONE кнопок/секций/дублирующих ручек.
- [ ] Все настройки AMP/TONE доступны в FX EDIT.
- [ ] FX EDIT открывается внутри окна плагина, а не в отдельном OS DocumentWindow.
- [ ] В правом верхнем углу FX EDIT есть красная квадратная кнопка с чёрным X; нажатие закрывает редактор и возвращает на главный экран.
- [ ] Внизу сбоку допускается небольшая надпись BACK как вспомогательная навигация; если она интерактивная, выполняет то же закрытие FX EDIT. Надписи MAIN нет.
- [ ] Каждая ручка FX EDIT подключена к реальному параметру DSP/APVTS.
- [ ] Нет дублирующих регуляторов одного параметра на главном экране и в FX EDIT.
- [ ] Состояния FX EDIT сохраняются и восстанавливаются вместе с пресетом и проектом DAW.


## 6. AUTOCHORDS — гармоническая тональность только для PATTERN

**AUTOCHORDS** — отдельная функция гармонической интерпретации MIDI-входа для режима **PATTERN**. По рабочей логике это ориентир на chord/key workflow из Session Guitarist – Electric Sunburst Deluxe Edition; реализовывать собственный код и интерфейс проекта, не копируя закрытый код, графику или аудиоматериалы Native Instruments.

### Элементы управления
- Кнопка **AUTOCHORDS ON/OFF** включает и выключает функцию.
- Переключатель **MAJOR / MINOR** задаёт мажорный или минорный лад.
- Селектор **KEY** содержит ровно семь вариантов: **C, D, E, F, G, A, B**. Не показывать в этом селекторе C#, D#, F#, G# или A#.
- Кнопка и оба селектора относятся к одной функции AUTOCHORDS и расположены рядом, чтобы пользователь видел выбранную тональность.

### Поведение
- AUTOCHORDS действует только при активном режиме **PATTERN**. Как только пользователь выбирает режим **MELODY**, AUTOCHORDS автоматически перестаёт участвовать в обработке MIDI: не преобразует ноты и не строит аккорды. В MELODY функция считается выключенной/обойдённой, а сам режим остаётся полностью хроматическим. Элементы AUTOCHORDS в MELODY должны быть визуально неактивны (или явно показывать, что функция недоступна в этом режиме), чтобы исключить случайное ожидание автодостраивания аккордов.
- Пользователь также может вручную нажать **OFF** у AUTOCHORDS. В PATTERN при OFF автоматическое построение аккордов не выполняется. Ручное OFF должно сохраняться и уважаться при дальнейшей работе; переключение в MELODY в любом случае гарантирует bypass AUTOCHORDS. Не переключать режим исполнения автоматически.
- В PATTERN при AUTOCHORDS = OFF входящие MIDI-аккорды/ноты обрабатываются обычным движком PATTERN без автоматического достраивания аккорда.
- В PATTERN при AUTOCHORDS = ON одиночная входящая клавиша интерпретируется как гармоническая позиция в выбранной тональности и ладу. Движок формирует соответствующий диатонический аккорд (как минимум трезвучие: основная ступень, терция и квинта) и исполняет его выбранным PATTERN.
- Выбор KEY и MAJOR/MINOR меняет гармоническую карту: одна и та же входящая клавиша должна давать соответствующую ступень и качество аккорда в выбранной тональности. Транспонирование должно быть музыкально корректным, с сохранением интервалов и правильной мажорной/минорной структуры; нельзя просто сдвигать все ноты на одинаковый интервал и игнорировать ступень лада.
- Если пользователь уже передаёт несколько нот как готовый MIDI-аккорд, не достраивать поверх него ещё одно автоматическое трезвучие; сохранить исходный аккорд согласно выбранной политике chord input.
- При смене KEY или MAJOR/MINOR во время удержания аккорда движок должен безопасно обновить гармонию на музыкально корректной границе/перезапуске паттерна, не оставляя зависших нот.
- Значения AUTOCHORDS, KEY и MAJOR/MINOR сохраняются в состоянии плагина/проекте DAW и восстанавливаются при открытии.

### Acceptance checklist — AUTOCHORDS
- [ ] AUTOCHORDS имеет явные состояния ON и OFF.
- [ ] Функция реально применяется только в PATTERN; при выборе MELODY она автоматически bypass/неактивна и не изменяет входящие ноты.
- [ ] Пользователь может вручную установить AUTOCHORDS = OFF; в PATTERN при OFF аккорды автоматически не достраиваются.
- [ ] PATTERN продолжает исполнять выбранный ритмический рисунок при AUTOCHORDS OFF; выключение AUTOCHORDS не отключает Pattern engine.
- [ ] Выбор лада содержит только MAJOR и MINOR.
- [ ] Выбор тональности содержит ровно C, D, E, F, G, A, B.
- [ ] При ON одиночная MIDI-клавиша задаёт гармоническую ступень и запускает соответствующий аккордный PATTERN.
- [ ] KEY и лад действительно меняют высоты и интервальную структуру аккорда, а не только надписи UI.
- [ ] При OFF входящие MIDI-ноты/аккорды не подвергаются автоматическому построению трезвучия.
- [ ] Смена настроек не оставляет зависших нот; значения сохраняются и восстанавливаются.
- [ ] Проверены все 7 тональностей в MAJOR и MINOR на нескольких входящих клавишах в VST3 и вручную в FL Studio 20.



## 7. PATTERN — точное соблюдение длительности MIDI-ноты из Piano Roll

**Обязательное требование пользователя:** ритмический рисунок/аккомпанемент не должен продолжать исполняться после окончания длины MIDI-ноты, нарисованной в Piano Roll DAW. В POP PUNK GUITAR ULTIMATE не переносить поведение с отложенным остановом (Latch tail), если оно приводит к звучанию паттерна за пределами MIDI note-off.

### Правила остановки
- Начало исполнения PATTERN определяется MIDI note-on и правилами синхронизации с host transport.
- Конец нарисованной MIDI-ноты в Piano Roll создаёт соответствующий MIDI note-off. Этот note-off является жёсткой границей длительности команды на исполнение.
- Когда заканчивается последняя удерживаемая управляющая нота/аккорд, Pattern engine прекращает планирование новых ритмических событий и отменяет все ещё не воспроизведённые события данного запуска.
- Все активные голоса, принадлежащие завершившемуся запуску паттерна, получают корректное завершение/остановку; нельзя допускать новых ударов, щипков, страмов или арпеджио после границы note-off.
- Не добавлять скрытый latch, автоматическое продление MIDI-ноты или фиксированный хвост аккомпанемента после note-off. Если позднее будет добавлена опция Latch, по умолчанию она должна быть OFF, явно управляться пользователем и не нарушать режим точного окончания при OFF.
- При legato-переходе между соседними MIDI-нотами/аккордами Pattern engine может непрерывно перейти к следующей гармонии только если следующая нота уже удерживается/перекрывает предыдущую. Если после note-off не осталось удерживаемых управляющих нот, паттерн должен остановиться.
- При остановке host transport, перемотке, loop/cycle jump, удалении/отмене события и смене режима движок обязан сбросить устаревшие запланированные события и не воспроизводить их позже.
- Естественный короткий release уже начавшегося сэмпла допустим только в пределах настроенного release, но он не должен запускать последующие ноты паттерна или создавать слышимый продолжающийся аккомпанемент после конца MIDI-ноты. Для строгого режима допускается короткое затухание/voice kill, если иначе сэмпл явно выходит за границу.

### Acceptance tests — MIDI note duration gate
- [ ] Короткая нота в Piano Roll запускает PATTERN только в пределах её длительности; после её note-off новые ритмические атаки отсутствуют.
- [ ] Длинная нота удерживает PATTERN до своего note-off; длительность не ограничивается произвольным коротким таймером.
- [ ] Несколько перекрывающихся нот: переход между гармониями не обрывает паттерн, пока остаётся удерживаемая управляющая нота.
- [ ] После note-off последней удерживаемой ноты отменяются все будущие события scheduler и завершаются активные голоса согласно строгому release policy.
- [ ] PATTERN/OFF и PATTERN/ON одинаково соблюдают границу MIDI note-off.
- [ ] Проверены короткие/длинные ноты, перекрытия, повторные ноты, sustain/CC64 policy, stop/seek/loop в тестах scheduler и вручную в FL Studio 20.
- [ ] Нет зависших нот и нет ритмических атак после конца нарисованной MIDI-ноты.

**Отличие от референса:** у Session Guitarist есть настраиваемый Latch Mode, который может задерживать остановку после отпускания клавиш. В этом проекте приоритет — точное соблюдение MIDI note length из Piano Roll; отложенный latch-tail не должен быть поведением по умолчанию и не должен нарушать указанное требование. См. описание Latch Mode в официальном руководстве: https://docs.native-instruments.com/ni-tech-manuals/electric-sunburst-manual/en/the-playback-page.

## 8. LIVE NOTE / CHORD DISPLAY — информационное табло исполнения

В главном интерфейсе предусмотреть постоянно видимое информационное табло **LIVE NOTE / CHORD**, показывающее музыкальное имя реально удерживаемых MIDI-нот и распознанного аккорда. Табло работает одинаково в режимах MELODY и PATTERN и не зависит от AUTOCHORDS ON/OFF.

### Отображение
- При одной удерживаемой ноте показывать её музыкальное имя: **C, D, E, F, G, A, B**, а для хроматических нот также корректные обозначения с диезами/бемолями согласно выбранной политике нотной записи (например C#, D#, F#, G#, A# или Db/Eb/Gb/Ab/Bb). Нельзя принудительно отображать только семь белых нот, если пользователь сыграл чёрную клавишу.
- По возможности показывать октаву рядом с нотой (например C4), но основное крупное обозначение ноты должно оставаться читаемым.
- При одновременном нажатии нескольких нот распознавать аккорд и показывать общепринятое обозначение: например **C**, **Cm**, **C5**, **Cmaj7**, **Am**, **G/B**, **Dsus4** — только если такое качество/обращение действительно следует из сыгранных MIDI-нот.
- Если аккорд нельзя уверенно определить, показывать список нот (например C + E + F#) или UNKNOWN CHORD с перечнем нот; не угадывать название.
- Для нескольких независимых/неаккордовых нот в MELODY показывать все активные ноты, а распознанное аккордовое имя — только когда набор нот соответствует распознаваемой структуре.
- В PATTERN показывать ноту/аккорд, который фактически принят Pattern engine после MIDI input processing. При AUTOCHORDS ON табло отражает сформированный гармонический аккорд; при OFF — реально сыгранные пользователем входящие ноты/аккорд без автоматического достраивания.
- При смене управляющей ноты/аккорда табло обновляется без заметной задержки. После note-off последней удерживаемой ноты очищается либо показывает нейтральное состояние (—), не оставляя устаревший аккорд.
- При перекрывающихся MIDI-нот табло показывает актуальный набор удерживаемых нот; отпускание одной ноты удаляет только её, пока остальные остаются активными.
- MIDI channel handling должен быть последовательным: не смешивать независимые каналы без явной политики. Приём MIDI note-on velocity 0 как note-off обязателен.
- Табло только отображает состояние и не меняет MIDI, гармонию, режим, preset или PatternScheduler.

### Технические требования
- Состояние для UI публикуется из audio thread через lock-free/preallocated snapshot (например атомарная маска активных нот и версия snapshot); UI читает его по Timer с ограниченной частотой. Не использовать UI/audio thread locks, выделение памяти или форматирование строк внутри processBlock.
- Распознавание имён нот и аккордов, локализация/нотная запись и форматирование текста выполняются на UI/message thread по полученному snapshot.
- Смена режима MELODY/PATTERN и AUTOCHORDS ON/OFF не должна вызывать зависшие ноты или рассинхронизацию табло с реальным MIDI-состоянием.

### Acceptance checklist — LIVE NOTE / CHORD
- [ ] Табло присутствует на главном экране и читаемо при разных skins и размерах окна.
- [ ] Одна MIDI-клавиша показывает правильную ноту и, если включено, октаву.
- [ ] Хроматические ноты отображаются корректно, не только C/D/E/F/G/A/B.
- [ ] C+E+G отображается как C; A+C+E как Am; C+G как C5; обращения распознаются только при достоверном соответствии нот.
- [ ] Неопределённые/неполные сочетания показывают ноты, а не выдуманное имя аккорда.
- [ ] MELODY показывает реально удерживаемые одиночные/одновременные ноты и аккорды.
- [ ] PATTERN + AUTOCHORDS OFF показывает входящую ноту/реально сыгранный аккорд.
- [ ] PATTERN + AUTOCHORDS ON показывает фактически сформированный диатонический аккорд.
- [ ] Note-off, velocity-zero note-on, перекрытия, смена канала, stop/seek и panic корректно очищают/обновляют табло.
- [ ] Snapshot не создаёт блокировок и аллокаций в audio thread; проверена синхронизация с реальными событиями sampler/pattern engine.

### Уточнение: точное распознавание ноты, аккорда и его мажорного/минорного характера

Главная задача LIVE NOTE / CHORD — не просто показать список нажатых клавиш, а правильно определить музыкальное содержание сыгранного набора и показать его понятным обозначением. Табло обязано одинаково работать независимо от выбранного пресета, режима MELODY/PATTERN и AUTOCHORDS ON/OFF.

- Одна активная MIDI-нота всегда отображается как точное название ноты (pitch class) и, желательно, октава. Например C4, F#3, Bb4. Нельзя ошибочно называть одиночную ноту аккордом.
- Для нескольких одновременно активных нот распознаватель анализирует интервалы относительно возможных корней, а не только первую/самую низкую ноту. Он должен определять root, качество аккорда и обращение, когда это однозначно следует из набора высот.
- Минимум обязательно различать major и minor: C / Cm, D / Dm, E / Em, F / Fm, G / Gm, A / Am, B / Bm. Также распознавать power chords (C5), diminished, augmented, sus2/sus4, maj7, m7, dominant 7 и другие качества, если набор нот соответствует им.
- Отображать качество/настроение явно и коротко: например `C — MAJOR`, `Am — MINOR`, `Bdim — DIMINISHED`, `Csus4 — SUSPENDED`. Для обычных аккордов главным крупным текстом остаётся символ аккорда, а тип/характер может отображаться меньшей подписью.
- Распознавать обращения и slash chords, если басовая нота отличается от корня и определение подтверждается набором нот: например G/B. Если обращение определить нельзя надёжно, не придумывать его.
- Учитывать дубли нот в разных октавах: они не должны мешать распознаванию качества аккорда. Октавы и фактические ноты можно дополнительно показать вторичной строкой.
- Для неполных voicings и гитарных аппликатур допускается распознавание по информативным интервалам (например root + minor/major third, root + fifth), но только при достаточной уверенности. Если несколько названий одинаково подходят, показывать ноты и пометку `AMBIGUOUS`, а не выдавать случайное название.
- Не путать enharmonic spelling: применять согласованную политику отображения диезов/бемолей и сохранять читаемое корректное написание в зависимости от тонального контекста, если он доступен.
- В PATTERN + AUTOCHORDS ON табло анализирует именно итоговые ноты сформированного аккорда; в PATTERN + OFF и MELODY — фактически удерживаемые пользователем ноты. Выбранный preset не должен влиять на музыкальное распознавание.
- При отпускании клавиш распознавание пересчитывается по оставшимся удерживаемым нотам. При отсутствии активных нот табло показывает нейтральное состояние.

### Дополнительные обязательные тесты распознавания
- [ ] Проверить все 12 корней для major и minor triads (24 базовых случая).
- [ ] Проверить все 12 корней для power chords и основные sus/dim/aug/seventh качества.
- [ ] Проверить обращения, басовую ноту, дубли в октавах и неполные гитарные voicings.
- [ ] Проверить одинаковые результаты при любом factory/user preset и во всех режимах, с оговорённым различием источника нот для PATTERN + AUTOCHORDS ON.
- [ ] Для каждого теста сверять chord symbol и MAJOR/MINOR/quality label с ожидаемым результатом; неоднозначные наборы должны явно оставаться неоднозначными, без ложной уверенности.

# SOUND QUALITY TARGET — REALISTIC ELECTRIC & ACOUSTIC GUITAR

## Product ambition and honest acceptance criteria

POP PUNK GUITAR ULTIMATE must aim for exceptionally convincing, mix-ready electric and acoustic guitar performance, with the ambition to compete with and, in defined use cases, surpass the perceived realism and usability of commercial instruments such as Session Guitarist – Electric Sunburst Deluxe and Ample Sound Guitar products. This is a development target, not a claim that the current build already exceeds those products. No superiority claim is allowed until controlled listening tests and independent host validation support it.

A convincing result must come from the complete instrument: source recordings, multisampling, articulation selection, performance engine, transitions, amp/cabinet processing, dynamics, stereo behavior and mix-ready presets. Reverb, distortion or EQ alone cannot make a static/limited sample set sound like a live guitar.

## 1. Source recordings and sample-library integrity

- Prefer high-quality, cleanly recorded DI electric guitar and dedicated acoustic guitar recordings, with documented instrument, pickup/microphone, string, tuning, playing technique, recording chain, sample rate/bit depth and license provenance.
- Record or acquire separate, genuinely different performances for articulations and dynamics; do not create fake velocity layers by merely changing gain or pitch.
- Build note-by-note multisampling across the useful fretboard range, with multiple velocity layers and round-robin variations where source material permits.
- Electric and acoustic instruments must have separate source sets and articulation maps. Acoustic must not be an electric DI passed through an EQ preset.
- Never redistribute commercial reference-library samples or extracted content. Every bundled sample must have explicit redistribution rights, attribution/notice requirements recorded, and a reproducible license audit.
- Preserve original recordings and create optimized derived cache files separately. Do not destructively rewrite source assets.

## 2. Electric guitar performance realism

Target articulations, only when corresponding real source material and mappings exist:
- picked open notes and sustained notes; downstroke/upstroke where recorded;
- palm mute with multiple mute depths and velocity/dynamics response;
- dead notes / muted scratches; natural harmonics and pinch harmonics;
- slides, legato transitions, hammer-ons, pull-offs, bends and vibrato;
- fret/string position variation, alternate picking and round-robin selection;
- realistic note release, string/fret noises and transitions without excessive or repetitive noise.

The engine must avoid the obvious sampler effect: repeated identical attacks, abrupt sample cuts, unnatural sustain, identical timbre at every velocity and inconsistent transitions. Articulation switching must be deterministic and sample-accurate enough for MIDI performance.

## 3. Acoustic guitar realism

- Use dedicated steel-string and nylon-string acoustic source material, not an electric-guitar approximation.
- Support realistic picked notes, fingerstyle, strumming, muted/percussive hits and natural decay where source recordings allow.
- Preserve transient attack, string resonance, body resonance and natural note decay; do not impose an artificial electric-guitar sustain envelope.
- Provide separate acoustic presets and performance behavior; acoustic strum patterns must use recorded or convincingly modeled strokes, not simply repeat a single sample.
- Keep acoustic dynamics and stereo image natural and controllable.

## 4. Performance engine and musical behavior

- Velocity must select meaningful recorded dynamics/articulations as well as control level; low velocity may select palm-mute only when the selected articulation map explicitly supports that behavior.
- Round robin must avoid immediate repetition where alternatives exist; selection must be reproducible for offline rendering and project recall.
- Handle overlapping notes, note-off, sustain, voice stealing, all-notes-off, transport stop/seek and pattern cancellation without stuck notes or cut-off tails.
- PATTERN mode must produce convincing strumming/picking timing with adjustable humanization, stroke direction, accents, timing variation and chord transitions. Humanization must be bounded, tempo-aware and reproducible.
- MELODY mode remains true chromatic note playing; it must not inject automatic rhythm or chord tones.
- Chord transitions should use legato/voice-leading only when musically and MIDI-duration appropriate; no hidden extension beyond Piano Roll note-off.
- Provide useful playable presets, but do not mask weak source recordings with excessive FX.

## 5. Tone chain and mix readiness

- Electric path: DI/sample source → amp stage → cabinet/IR → EQ/dynamics → modulation/time FX → output, with documented routing and sensible gain staging.
- Acoustic path: dedicated acoustic source → restrained tone shaping/dynamics → optional room/reverb, avoiding an obligatory distorted amp/cab chain.
- Cabinet/IR loading, oversampling and expensive effects must be optional and quality/CPU scalable.
- Presets must have controlled peak level, no unintended clipping, consistent loudness ranges and useful dry/wet variants.
- Double-track must create a genuinely differentiated second performance/voice or a clearly documented approximation; simple duplicated identical samples with a small delay must not be marketed as real double tracking.

## 6. Quality assurance gates — no subjective claims without evidence

- [ ] Verify sample coverage, velocity layers, round-robin counts, articulation maps and license records for every shipped bank.
- [ ] Build automated tests for key/velocity mapping, articulation selection, note-off/release, round robin, sample boundaries and voice cleanup.
- [ ] Render repeatable MIDI test suites covering single notes, chords, palm mutes, sustained notes, slides/bends where implemented, acoustic picking and PATTERN strumming at multiple tempos.
- [ ] Measure peak/RMS/loudness, clipping, latency, CPU and memory at defined polyphony levels on documented Windows hardware.
- [ ] Conduct level-matched blind listening comparisons against reference instruments using the same MIDI, musical phrase, mix and loudness. Evaluate attack, articulation, transitions, dynamics, realism and mix placement separately.
- [ ] Test in FL Studio 20 on Windows 10 x64, including project save/reopen, automation, offline render and transport behavior.
- [ ] Mark each articulation and instrument family as implemented only when its source assets, mapping, DSP path and tests are present. Unsupported techniques must be clearly identified rather than simulated deceptively.

**Acceptance principle:** sound quality is demonstrated per instrument, articulation and performance scenario. A single good preset or a large sample count does not prove that the whole instrument is realistic.


# MELODY VELOCITY ARTICULATION — DRIVE / DISTORTION PALM-MUTE TO SUSTAIN

## Goal and scope

Implement a Piano Roll velocity articulation switch inspired by the documented Ample Metal Eclipse workflow: low-velocity notes trigger progressively tighter palm mutes; higher-velocity notes trigger open sustained notes suitable for lead lines. This feature is intentionally restricted to **MELODY playback + presets explicitly classified as DRIVE or DISTORTION**. It must not alter CLEAN, ACOUSTIC, NYLON, CRUNCH-unless-explicitly-tagged, PATTERN, or other preset families. The selected preset's explicit articulation profile is authoritative; the global distortion FX switch alone is not sufficient to activate the behavior.

This is an original implementation of the musical idea, not a copy of Ample's code, samples, thresholds, or proprietary assets.

## 1. MIDI velocity mapping

- Read the original MIDI note velocity before any global velocity scaling/dynamics curve. Keep two independent values: (a) audible note intensity, shaped by the existing velocity/dynamics controls; (b) articulation selector, derived from the raw MIDI velocity so the Piano Roll remains predictable.
- Default mapping for the first implementation should be configurable and based on the following five zones:
  - 1–15: FULL MUTE / very tight palm mute.
  - 16–31: DEEP MUTE / tight palm mute.
  - 32–63: HALF MUTE / shorter palm-muted note.
  - 64–126: OPEN SUSTAIN / sustained lead note.
  - 127: STRONG SUSTAIN / accent layer only if a matching real sample layer exists; otherwise use the strongest valid sustain layer. Never trigger a harmonic/pop merely because velocity is 127 unless a separate explicit articulation/preset mapping requests it.
- Expose user-adjustable zone boundaries and/or a compact VELOCITY ARTICULATION curve in the instrument UI in a later UI pass. Store values in APVTS so automation and project recall work. Initial default behavior must work without extra setup.
- A low velocity must not simply become a quieter open note. It must select the corresponding palm-muted sample/articulation when that zone exists.
- High velocity must not force palm mute. It selects the open/sustain sample and preserves the MIDI note's natural duration until Note Off/release.
- Preserve musical dynamics: velocity continues to influence amplitude/attack, but articulation selection is a separate mapping. Avoid sudden loudness jumps at zone boundaries; level-match source layers and test transitions.

## 2. Preset and mode gating

- Add an explicit preset metadata field such as `velocityArticulationProfile` with values `off` and `drivePalmMuteSustain`. The latter is valid only for DRIVE/DISTORTION electric-guitar presets.
- Factory preset metadata must classify eligible presets deliberately; do not infer eligibility from preset display-name substring alone at runtime.
- USER/imported presets default to `off` until the user explicitly selects the compatible profile and the bank's sample map proves that the required articulations exist.
- Runtime gate is strict: `playMode == MELODY && activePreset.velocityArticulationProfile == drivePalmMuteSustain && activeInstrumentFamily == electric`.
- PATTERN always keeps its own preset/pattern articulation rules and must not inherit this MELODY velocity map.
- CLEAN, BRIGHT CLEAN, CHORUS CLEAN, TREMOLO CLEAN, ACOUSTIC, NYLON and other non-eligible presets retain their existing velocity behavior.
- Explicit manual articulation selection (for example a deliberate Palm Mute articulation) has priority over automatic velocity mapping. Switching preset/mode must not leave a stale articulation override.

## 3. Sample-bank requirements and fallback

- Each enabled preset must provide correctly mapped note/range coverage for at least the open sustain layer and the available palm-mute depth layers, including velocity zones and round-robin variation where available.
- Use genuinely recorded palm-muted performances at different mute depths. Do not manufacture three acoustic/PM layers by gain changes, pitch shifts, EQ-only changes or arbitrary sample truncation.
- If a bank contains only one PM layer, map the PM zones to that real layer and mark the reduced depth resolution in the bank audit; do not claim three distinct PM articulations.
- If no compatible PM layer exists for a note/range, use the closest valid mapped articulation with a safe fallback and expose a diagnostic; never silently select an unrelated sample.
- Preserve sample provenance, redistribution licenses, checksums and mapping metadata. Commercial reference-library samples must never be extracted or bundled.

## 4. Real-time and MIDI correctness

- Implement the velocity-to-articulation decision in the audio thread with bounded, allocation-free operations. Do not allocate strings/vectors, lock, scan the preset catalog, or parse SFZ in `processBlock`.
- Resolve the active preset profile and sample-zone mapping before playback or publish a compact immutable/precomputed mapping to the audio thread.
- Apply the articulation choice per incoming note, not globally to all currently held notes. Chords in MELODY may therefore contain notes with different velocity/articulation layers.
- MIDI Note Off must release the exact voice started by that Note On. Respect note duration, overlapping same-pitch notes, sustain pedal if supported, all-notes-off, panic, transport stop and preset changes.
- Velocity-zero Note On is Note Off and must not start a palm-muted voice.
- PATTERN and AutoChords behavior are unchanged.

## 5. Required regression tests / acceptance

- [ ] In MELODY + eligible DRIVE preset: velocity 1, 15, 16, 31, 32, 63 select the intended PM zones; 64, 126, 127 select sustain/accent fallback as specified.
- [ ] Test every zone boundary at adjacent values to catch off-by-one errors.
- [ ] In MELODY + CLEAN / ACOUSTIC / NYLON / untagged presets: automatic PM switching is disabled at all velocities.
- [ ] In PATTERN + DRIVE preset: this MELODY-only mapping is disabled; pattern articulation logic remains unchanged.
- [ ] Chord test: simultaneous notes with different velocities select independent articulations.
- [ ] Confirm MIDI velocity still affects amplitude/attack and that layer loudness is level-matched.
- [ ] Confirm high-velocity sustained notes follow Piano Roll note length and Note Off; low-velocity palm-muted notes have natural recorded short decay without stuck tails.
- [ ] Confirm project save/reopen and automation restore profile and threshold settings.
- [ ] Run sample-map coverage audit and render a velocity sweep for low E/A/D strings and representative upper-register notes; inspect waveforms, loudness, transient and decay.
- [ ] Compare the same MIDI passage against reference instruments at matched loudness; do not claim equivalent or superior realism without listening evidence.

**Important current-code finding:** the existing MELODY path currently compares the shaped/scaled velocity to one `palmMuteThreshold` and collapses every below-threshold note to a single sample velocity (`palmMuteThreshold - 1`). It also applies this automatic selection to all MELODY presets, not only DRIVE/DISTORTION. This does not provide multi-depth palm mute, and it violates the requested preset-family restriction. Replace this behavior with the explicit preset-gated multi-zone mapping above; do not stack a second velocity switch on top of the old one.
