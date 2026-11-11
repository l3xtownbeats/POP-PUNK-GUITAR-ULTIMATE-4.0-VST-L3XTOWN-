# PRESET BROWSER 5.0 — единая система звука и стиля

Статус: архитектурное решение для реализации. Этот документ не утверждает, что весь новый браузер и все связанные sample/performance layers уже подключены к DSP.

## 1. Главный принцип: три независимые оси

1. **PLAY MODE** — как MIDI управляет инструментом: MELODY или PATTERN.
2. **SOUND** — чем звучит гитара: инструмент/семпловый банк, усилитель, кабинет, EQ и FX.
3. **STYLE** — как исполняется нота или аккорд: lead, solo, open chords, palm mute, chug, strumming, tremolo и т. п.

Не смешивать SOUND и STYLE в одну длинную категорию. Один и тот же стиль должен работать с разными тембрами, а один тембр — с разными стилями.

## 2. MELODY — одиночные ноты и ручное построение аккордов

В MELODY одна MIDI-клавиша запускает одну хроматическую ноту (без автоматического ритмического рисунка). Пользователь выбирает SOUND, а затем при желании накладывает STYLE.

### SOUND-категории MELODY

- **ACOUSTIC** — steel-string bright, warm acoustic, nylon, acoustic muted.
- **CLEAN ELECTRIC** — glassy clean, neck clean, bright clean, chorus clean, tremolo clean.
- **EDGE / CRUNCH** — edge-of-breakup, classic crunch, pop-punk crunch, tight crunch.
- **HIGH GAIN / DISTORTION** — modern pop-punk rhythm, saturated lead, aggressive skate-punk.
- **AMBIENT** — wide chorus, shimmer-like ambience (только если соответствующие FX реально доступны).
- **SPECIAL TONES** — flageolet/harmonic, octave-like tone, filtered/lo-fi; отдельная артикуляция или обработка, а не обещание несуществующего семпла.

### STYLE-категории MELODY

- **LEAD** — протяжные ноты, контролируемый sustain, умеренный room/reverb; не менять автоматически ноту на аккорд.
- **SOLO** — lead с более выраженной атакой/динамикой и опциональными vibrato/legato/slide, только если они поддержаны движком/семплами.
- **PALM MUTE** — короткая приглушённая нота/chug; выбирать muted articulation, если она есть в банке. Если нет, не имитировать её простым снижением громкости и показывать отсутствие нужной артикуляции.
- **OPEN / RINGING** — открытая нота с естественным хвостом.
- **TREMOLO PICKING** — повторная быстрая атака; это performance behaviour, не название чистого тембра.
- **FLAGEOLET / HARMONIC** — натуральный/искусственный флажолет, только при наличии корректно записанного слоя/маппинга.
- **OCTAVE / HOOK** — музыкальная роль; гармонизация включается только если отдельная функция явно выбрана, иначе одна клавиша = одна нота.
- **FINGER / PICKED** — техника атаки/источник артикуляции.

Пример: SOUND = Clean Acoustic + STYLE = Lead даёт чистую протяжную акустическую ноту. SOUND = Crunch Electric + STYLE = Lead даёт протяжную электрогитарную ноту. SOUND = Clean Electric + STYLE = Palm Mute запрашивает приглушённую артикуляцию, если она записана; при отсутствии muted-зоны браузер не должен молча подменять её другим звуком.

## 3. PATTERN — аккорды и готовые гитарные исполнения

В PATTERN factory preset — это **базовый музыкальный performance preset**: гармоническое поведение, ритмический рисунок, артикуляционная логика, динамика и его собственный базовый тон/FX. Клавиши меняют root/chord, а рисунок сохраняется.

### STYLE-категории PATTERN

- **LEAD PATTERN** — ритмически оформленные лидирующие/одноголосные фразы или верхние ноты; не путать с простым MELODY Lead.
- **SOLO / MELODIC** — мелодические проходы и переходы.
- **PALM-MUTED RHYTHM** — короткие приглушённые восьмые/шестнадцатые, tight chugs, verse drive.
- **OPEN CHORDS** — широкие звенящие аккорды, chorus sustain.
- **POWER CHORDS** — плотные двух-/трёхнотные power-chord удары и ритмические рисунки.
- **DOWNSTROKE** — ровные жёсткие удары вниз.
- **ALTERNATE PICKING** — чередование вниз/вверх.
- **POP-PUNK STRUM** — характерный аккордовый бой.
- **ACOUSTIC STRUM** — акустический бой/арпеджио, если performance library содержит нужные записи.
- **ARPEGGIO** — разложенные аккорды.
- **OCTAVE / HOOK** — октавные хуки/мелодические ритм-фразы.
- **TREMOLO / FAST DRIVE** — быстрые повторные атаки.
- **STOP-START / STACCATO** — короткие удары, паузы и синкопы.
- **AMBIENT / WIDE** — протяжённые аккорды и разреженная игра.

### Поведение выбранного пресета

- Если STYLE override не выбран (**AUTO / PRESET DEFAULT**), звучит авторский стиль самого пресета.
- Если выбран override, он заменяет только совместимую performance/articulation часть. Базовый SOUND, артистический tonal target и FX пресета остаются.
- Если override несовместим с библиотекой, он не должен молча включаться: показать понятное сообщение и оставить AUTO.
- Style override может содержать артикуляцию, рисунок, strum direction, note length, chug density, accents, swing/humanize и sample requirement. FX не должны незаметно переопределяться стилем.

Пример: PATTERN preset **Blink-182 — Lead** без override играет своим штатным лидирующим рисунком и тоном. Выбор **Palm-Muted Rhythm** переключает артикуляцию/исполнение на короткое приглушённое исполнение, но сохраняет базовый Blink-182 tonal preset и его FX. При отсутствии подходящих palm-muted performance samples браузер сообщает об этом, а не подменяет звук синтезированной заглушкой.

## 4. Общий каталог артистических направлений

Все пользовательские референсы сохраняются как tonal/performance направления, а не как официальные пресеты или копии записей:

- Blink-182 — яркий плотный современный pop-punk, tight verse / wide chorus.
- Sum 41 — агрессивный skate-punk, плотный palm mute, резкая атака.
- Simple Plan — полированный мелодичный pop-punk, чистая читаемая верхняя середина.
- New Found Glory — быстрый melodic punk, чёткие downstrokes и tight muted verses.
- Good Charlotte — упругий mid-forward ритм, компактные хуки и октавы.
- Mest — мелодичный pop-punk lead и энергичный rhythm.
- Angels & Airwaves — пространственные слои, длинный sustain и modulation.
- The All-American Rejects — radio-ready pop-rock crunch.
- MGK — современный насыщенный яркий pop-punk.
- Avril Lavigne — яркая мелодика, acoustic-to-crunch диапазон.
- Дополнительные существующие направления каталога (The Offspring, Green Day, Bowling for Soup, Sugarcult, All Time Low и другие) остаются в каталоге и получают те же общие SOUND/STYLE фильтры.

## 5. Единый браузер: рекомендуемый интерфейс

В верхней части один Preset Browser с поиском и быстрыми фильтрами:

- **MODE:** MELODY / PATTERN (синхронизирован с главным переключателем Play Mode).
- **SOUND:** All, Acoustic, Clean, Crunch, Distortion, Ambient, Special.
- **STYLE:** Auto / Preset Default, Lead, Solo, Palm Mute, Open Chords, Power Chords, Downstroke, Alternate, Strum, Arpeggio, Octave Hook, Tremolo, Staccato, Ambient.
- **SOURCE:** Factory / User Banks / All (будет подключён к Bank Sounds).
- **SEARCH:** ищет имя, artist reference, sound, style, sample bank и теги.

Результат — карточка/строка пресета: название, MODE, SOUND, STYLE default, sample bank readiness, FX summary. Выбор строки загружает базовый пресет; изменение фильтра никогда не должно случайно загружать другой звук.

## 6. Архитектура данных

У каждого пресета должны быть независимые поля:
- id, name, mode, tags, referenceArtist (необязательный reference);
- soundProfileId, sampleBankId, requiredArticulations;
- defaultStyleId, supportedStyleOverrides[];
- parameters / FX snapshot;
- patternClipId или performanceId для PATTERN;
- availability/status и provenance/license metadata.

У каждого STYLE profile:
- id, name, compatibleModes[];
- articulationId, performancePatternId (опционально);
- strum/picking direction, gate/note length, accents, swing, chug density;
- requiredArticulations, compatibleSoundProfiles;
- fallbackPolicy = reject-and-explain (никогда не подменять незаметно).

Ссылки на sampleBank — стабильные IDs из Bank Sounds, а не жёсткие пути к файлам. Factory preset не должен ссылаться на пользовательский абсолютный путь.

## 7. Правила приоритета и совместимости

1. Play Mode определяет движок (MELODY/PATTERN).
2. Preset загружает свой SOUND, FX и default STYLE.
3. Явный STYLE override заменяет только стиль/исполнение, если совместим.
4. Пользовательские ручные FX edits после выбора пресета остаются ручными; смена STYLE не сбрасывает FX.
5. Новый SOUND preset может менять sample source и tonal FX, но не должен менять Play Mode без явного выбора пользователя.
6. Если нужные samples/articulations отсутствуют, сообщить об этом и сохранить предыдущий рабочий звук.
7. Любая загрузка выполняется вне audio thread; в processBlock — только заранее опубликованные immutable state/clip references.

## 8. Важное разделение терминов

- **Clean / Crunch / Distortion / Acoustic** — SOUND.
- **Lead / Solo / Palm Mute / Open Chords / Downstroke / Strum / Arpeggio** — STYLE или performance role.
- **Open / Muted / Flageolet / Tremolo** могут обозначать артикуляцию. Они относятся к STYLE/performance layer, хотя UI может показывать их как быстрый подфильтр.
- **MELODY / PATTERN** — только Play Mode, не категории тембра и не стили.

## 9. Этапы внедрения

1. Нормализовать существующий factory catalog и мигрировать старые soundCharacter/groove/articulation поля без потери 300 записей.
2. Реализовать единый браузер с отдельными SOUND и STYLE фильтрами и корректным сохранением выбранного пресета.
3. Подключить реальные APVTS/DSP mappings для всех sound controls и style overrides; проверить каждый parameter ID.
4. Связать soundProfileId/sampleBankId с Bank Sounds и проверкой доступности лицензированных семплов.
5. Связать PATTERN style с настоящими MIDI/performance clips и sample articulations.
6. Проверить комбинации, отсутствие семплов, переключение режимов, восстановление состояния, host automation и отсутствие работы с файлами в audio callback.

**Критерий готовности:** каждый пункт в браузере имеет реальное слышимое действие, корректную совместимость и понятный статус. Никакие пустые категории или декоративные параметры не выдаются за работающие функции.


## 8. Папки тембров и музыкальных стилистик по референсу Electric Sunburst Deluxe

В браузере создаются две независимые ветви, чтобы не смешивать тембр гитары и способ исполнения.

### MELODY → Tone Presets

Папки в таком порядке:
1. **Clean Bright** — яркий чистый тон.
2. **Clean Warm** — тёплый чистый тон.
3. **Crunch** — лёгкий/средний перегруз.
4. **Lead Crunch** — более плотный mid-forward лид-тон.
5. **Heavy** — плотный high-gain.
6. **Fuzz / Distortion** — фузз и дисторшн.
7. **Lo-Fi / Vintage** — винтажная фильтрация/сатурация.

Это оригинальные звуковые профили нашего проекта по описанию характеров звучания, а не официальные пресеты Native Instruments. Каждый профиль должен хранить собственные ссылки на sample bank, FX snapshot и требования к артикуляциям. Папка не означает, что все необходимые сэмплы/эффекты уже реализованы.

### PATTERN / SONG → Performance Styles

Отдельная ветвь для музыкальных исполнений: **Pop / Pop-Rock**, **Pop-Punk / Punk**, **Electronic**, **Riffs**, **Arpeggios**, **Strumming**. Здесь хранятся ритмические рисунки/исполнительские клипы и их chord/strum maps, а не звуковые тембры. Конкретный паттерн показывается как доступный для воспроизведения только после наличия и проверки performance data и sample coverage.

### Правила браузера

- **NOT SOUND** остаётся первым пунктом каждой вкладки и вложенной папки.
- MELODY tone folders фильтруют тембры для хроматической игры; PATTERN / SONG folders фильтруют музыкальные рисунки.
- Звуковые категории не должны автоматически переключать MELODY/PATTERN.
- Не импортировать и не копировать закрытые Kontakt/NKS-пресеты, паттерны MIDI, сэмплы, IR или графику NI. Создавать собственные профили с проверенными лицензиями.
- Текущая запись в taxonomy — структура каталога и метаданные; интеграция папок в GUI, сопоставление с реальными банками и воспроизведение остаются отдельными acceptance gates.
