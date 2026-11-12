# PPGU — GUI / SKIN VISUAL PARITY MASTER
Дата: 12.11.2026
Ветка: `POP-PUNK-GUITAR-ULTIMATE-VST-REV-17`
Назначение: единый обязательный перечень ВСЕХ видимых элементов оболочки, которые должны получать тему, состояния, масштабирование и проверку визуальной целостности.

> Это инвентарный контракт по исходникам, а не заявление о завершённой реализации. Наличие параметра в APVTS само по себе не означает наличие GUI. Не показывать несуществующие функции как рабочие.

## 1. Главная оболочка — подтверждённые элементы

| Зона | Элемент | Назначение / обязательное визуальное покрытие |
|---|---|---|
| Header | titleLabel | Название продукта; основной текстовый токен |
| Header | subtitleLabel | L3XTOWN AUDIO LAB; вторичный текст |
| Header | statusLabel | Статус библиотеки/пресета/операции; success/warning/error/neutral |
| Header | skinButton | Открытие выбора темы; hover/pressed/focus/disabled |
| Header | loadButton (IMPORT GUITAR) | Открытие импорта SFZ; loading/success/error |
| Header | autoChordsButton | Открытие AutoChords; все состояния кнопки |
| Preset browser | presetBox | Выбор заводского пресета; selected/hover/focus |
| Preset browser | categoryBox | ALL / MELODY / PATTERN; выбранная категория |
| Preset browser | presetSearch | Ввод поиска; placeholder/focus/selection/clear |
| Mode | melodyTab | Выбор MELODY; active/inactive/hover |
| Mode | patternTab | Выбор PATTERN; active/inactive/hover |
| Mode | modeBox | Режим воспроизведения, attachment к playMode |
| AMP & TONE | knobs[0..3] | GAIN, TREBLE, MID, BASS; шкала, pointer, track, value, label |
| PERFORMANCE | knobs[4..8] | VELOCITY, DYNAMICS, SWING, HUMANIZE, ACCENT |
| FX CHAIN | fxButtons[0..6] | COMPRESSOR, EQ, DISTORTION, CHORUS, DELAY, REVERB, CABINET IR; bypass ON/OFF |
| PLAYBACK | startModeBox | HOST / HOST START / INTERNAL; syncMode |
| PLAYBACK | patternSlotLabel + patternSlotBox | Выбор pattern slot |
| PLAYBACK | patternLengthLabel + patternLengthBox | Длина паттерна |
| PLAYBACK | syncModeLabel | Подпись режима синхронизации |
| PLAYBACK | bpmLabel + bpmSlider | Темп и его readout |
| PLAYBACK | startOnFirstChordButton | Автостарт при первом аккорде |
| Keyboard | MidiKeyboardComponent | Клавиши, белые/чёрные, pressed/hover, range/scroll и видимые границы |

## 2. Дополнительные / дочерние окна

| Окно | Подтверждённые элементы | Требования к оформлению |
|---|---|---|
| Import Guitar | heading, description, status, LOAD SFZ | Единая типографика, status semantic colours, disabled/loading/error |
| FX Edit | title, effectSelector, enable/bypass, динамический набор параметров | Общий стиль rotary/slider/toggle/selector; все 8 эффектов |
| AutoChords / workspace | Компоненты, перечисленные в реализации AutoChords и PPGUMultiWindowWorkspace | Не терять заголовок, close-кнопку, resize bounds, фокус, z-order |
| Skin selector (текущий popup) | Пункт выбора темы | Следующий этап — полноценная галерея с preview, поиском, фильтрами и избранным; до реализации не выдавать popup за галерею |

### FX Edit — полный список групп и параметров, найденный в исходнике
- COMPRESSOR: compressorThreshold, compressorRatio, compressorAttack, compressorRelease, compressorAmount.
- EQ: eqLow, eqMid, eqHigh, eqLowFreq, eqMidFreq, eqHighFreq, eqMidQ.
- DISTORTION: distortionDrive, distortionTone, distortionMix.
- CHORUS: chorusRate, chorusDepth, chorusMix.
- FLANGER: flangerRate, flangerDepth, flangerFeedback, flangerMix.
- DELAY: delayTime, delayFeedback, delayMix.
- REVERB: reverbRoomSize, reverbDamping, reverbMix.
- CABINET IR: cabinetMix, cabinetLowCut, cabinetHighCut.

## 3. Обязательные визуальные состояния каждого компонента
1. Default / idle.
2. Hover (где поддерживается указатель).
3. Pressed / active.
4. Keyboard focus — видимый focus ring.
5. Disabled / unavailable — контрастный, но явно неактивный вид.
6. Selected / current value.
7. Loading / busy — только для реальной асинхронной операции.
8. Success / warning / error — только для реального результата.
9. Value editing / text selection для полей ввода.
10. MIDI key down / key up для клавиатуры.
11. FX enabled / bypassed для каждого FX toggle.
12. Window active / inactive, resize grip, close affordance для отдельных окон.

## 4. Единая токенизация — ни одного случайного hardcoded цвета
Все поверхности, текст, линии, обводки, подсветки, состояния, тени, индикаторы и LCD/readouts должны ссылаться на 21 token:
`window_bg`, `panel`, `panel_elevated`, `control_bg`, `control_face`, `control_border`, `text_primary`, `text_secondary`, `text_disabled`, `accent`, `accent_glow`, `danger`, `warning`, `success`, `lcd_bg`, `lcd_text`, `led_on`, `led_off`, `shadow`, `highlight`, `lowlight`.

Правило: компонент не должен содержать собственный HEX для постоянного визуального цвета. Допустимы вычисляемые alpha/gradient/geometry на основе токенов. Палитра применяется централизованно через ThemeManager и LookAndFeel. Нельзя менять bounds/hit targets/назначение/attachments ради скина.

## 5. Что нельзя фальсифицировать
- LED_indicator, Auto_Discover, Curator, Settings и About в текущей инвентарной матрице отмечены как NOT IMPLEMENTED. Пока не созданы реальные компоненты и обработчики — показывать как отсутствующие/недоступные, не рисовать фиктивную активность.
- LCD_display пока является statusLabel, а не самостоятельным LCD-widget.
- PUNK GROUP не реализован в mode selector.
- APVTS-параметры master, voices, palmMuteThreshold, tuning, pan, output, articulation, pickStyle, vibrato, slideTime, legato, hammerPull, pitchBendTarget, doubleTrackOn и прочие не добавлять в визуальную оболочку без реального компонента, корректного attachment и UX-назначения.
- Скин не изменяет аудио, MIDI, пресеты, APVTS, sampler, паттерны, DSP, порядок обработки или значения параметров.

## 6. Темы и Skin Gallery — требования к каждой теме
- Стабильный ID; имя; family; версия схемы; описание.
- Preview с реальным интерфейсом, не абстрактный постер; hero/texture только как вспомогательный арт.
- Все 21 палитровых токена; проверка контрастности G1.
- Полный control map по canonical IDs; проверка G2 parity.
- Проверка PNG/SVG размеров, форматов и загрузки G3.
- Совместимость ID/индекса/каталога G4; динамическое обнаружение без фиксированного лимита.
- Favorites, family filters, поиск, клавиатурная навигация, DEFAULT, понятные error/unavailable states.
- Safe path containment: все ресурсы остаются внутри папки текущей темы; отклонять traversal/symlink escape.
- Несовместимая/повреждённая тема не должна ломать оболочку: вернуть DEFAULT и показать причину.
- Переключение — визуальное, плавное 150 ms; не трогать audio thread/APVTS/processor state.
- ECO: отключить/снизить необязательные анимации и дорогие эффекты.

## 7. Общий стандарт качества оболочки
- Премиальная аппаратная эстетика: чёткая иерархия, аккуратные панели, точная сетка, умеренные акценты, без дешёвого bevel/glow.
- Подписи и значения остаются текстом JUCE, не запекаются в bitmap.
- Все rotary, sliders, buttons, tabs, combo boxes, text editors, toggles, labels, keyboard, panels и window chrome оформляются единообразно.
- Контраст проверять для normal/disabled/selected/hover/focus; не полагаться только на цвет для различения состояний.
- DPI 100/125/150%, минимальный размер, clipping, длинные названия пресетов, длинные статусы, масштаб окна и многооконный режим.
- Если открыто несколько окон — они уменьшаются/раскладываются так, чтобы оставались видны и доступны элементы каждого окна; отдельное закрытие крестиком и независимое перемещение/масштабирование.
- Сохранить выбранную тему и пользовательские UI preferences; не сохранять временное состояние загрузки как постоянное.

## 8. План закрытия — строго по циклу
1. Проверка: снять фактическую карту всех компонентов и прямых цветов из PluginEditor, FX panels, AutoChords и workspace.
2. Анализ: сопоставить каждый компонент с токеном, состоянием, ID, attachment и реальным действием.
3. Исправление: централизовать LookAndFeel/ThemeManager, добавить недостающие визуальные состояния и убрать hardcoded colours.
4. Повторный анализ: поиск оставшихся HEX, parity-компонентов, missing attachments, contrast и bounds.
5. Результат: compile + unit/CTest + validator G1–G4 + UI screenshot review + DPI + FL Studio 20 host test. Не объявлять PASS без логов.

## 9. Текущий честный статус
- Главная оболочка: часть реальных компонентов подтверждена исходниками.
- FX Edit: 8 групп параметров перечислены в исходнике.
- ThemeManager: 21-token palette model присутствует в header; legacy parsing и runtime coverage требуют полной проверки.
- Полное тематизирование всех элементов: НЕ ПОДТВЕРЖДЕНО.
- Полноценная Skin Gallery / dynamic discovery / 150 ms fade / ECO / G1–G7: НЕ ЗАКРЫТО.
- Compile, CTest, pluginval strictness 5, DPI и FL Studio 20 host acceptance: требуется отдельный запуск и отчёт.

Следующий кодовый этап: провести точечную миграцию всех draw/setColour/look-and-feel участков PluginEditor и дочерних панелей на 21 токен, затем добавить runtime parity test, после чего реализовать динамический Theme Gallery без фиксированного лимита 51.
