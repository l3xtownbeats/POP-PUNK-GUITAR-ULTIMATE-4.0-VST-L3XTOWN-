# NEON STAGE — единая система оформления интерфейса

**Продукт:** POP PUNK GUITAR ULTIMATE 4.0  
**Тема / skin / GUI / оболочка:** NEON STAGE  
**Default technical ID:** neon-stage-default-skin-1  
**Роль:** единая визуальная система главного окна и всех вспомогательных окон.

## 1. Принцип единого интерфейса

NEON STAGE — единая оболочка всего плагина: главный экран, preset browser, все controls, FX EDIT, AutoChords, Import Guitar, Settings, Skin Gallery и будущие окна. Общими являются палитра, типографика, поверхности, рамки, интерактивные состояния, отступы, focus, hover, disabled, active и selected states.

Никакое отдельное окно не должно выглядеть как чужое приложение. Все окна используют один ThemeManager и передают активную палитру в уже открытые и вновь открываемые theme-aware панели.

## 2. Визуальный язык

- Основа: глубокий midnight / carbon фон; панели на один-два уровня светлее.
- Главный акцент: electric cyan; мягкий cyan glow используется дозированно.
- Вторичные stage accents: blue / violet / magenta допустимы в artwork или дополнительных темах; функциональные состояния используют semantic tokens.
- Контролы: металлические/анодированные поверхности, чёткая граница, тонкий световой контур; без чрезмерного bloom, fake-3D и текстур поверх подписей.
- Типографика: Segoe UI для интерфейса, Consolas для цифровых readouts/LCD. Текст и значения остаются векторными.

## 3. Канонические 21 semantic tokens

| Token | Назначение |
|---|---|
| window_bg | фон всего редактора/workspace |
| panel / panel_elevated | базовая и поднятая поверхности окон/секций |
| control_bg / control_face | фон и лицо интерактивных контролов |
| control_border | рамки, разделители, края окон |
| text_primary / text_secondary / text_disabled | иерархия текста и недоступные состояния |
| accent / accent_glow | основной акцент, active/focus highlight |
| danger / warning / success | семантические состояния |
| lcd_bg / lcd_text | status/LCD readouts |
| led_on / led_off | зарезервированные реальные LED состояния; не рисовать активность без источника |
| shadow / highlight / lowlight | глубина и материал поверхностей |

Текущие значения токенов берутся только из SKIN/Skin_052/theme.json; цвета не дублируются в компонентах как независимая палитра.

## 4. Main screen — зоны и действующие элементы

1. Header: product mark, subtitle, IMPORT GUITAR, AUTOCHORDS, SETTINGS, SKIN.
2. Preset Library: factory preset selector, category selector (ALL / MELODY / PATTERN), text search, реальный status/selection feedback.
3. Instrument / Performance: MELODY и PATTERN tabs, mode selector, девять APVTS rotary: GAIN, TREBLE, MID, BASS, VELOCITY, DYNAMICS, SWING, HUMANIZE, ACCENT.
4. AMP & TONE / FX CHAIN: семь настоящих APVTS bypass toggles в неизменном порядке: COMPRESSOR, EQ, DISTORTION, CHORUS, DELAY, REVERB, CABINET IR. FX EDIT открывается отдельным рабочим окном.
5. Playback / feedback: HOST / HOST START / INTERNAL sync selector, live harmony/performance display и status/LCD feedback.
6. MIDI input: on-screen keyboard MIDI 36–84 с цветами NEON STAGE для белых/чёрных клавиш, separators, hover и key-down.

## 5. Состояния компонентов

- Normal: panel/control surface, readable label, semantic border.
- Hover: умеренное повышение яркости/контраста, без сильного свечения.
- Pressed / selected / toggled: cyan active surface, сохраняется читаемый foreground и видимый active marker.
- Keyboard focus: усиленный accent-glow outline; фокус не обозначается только цветом текста.
- Disabled: приглушённая поверхность/граница и text_disabled; не маскировать отсутствующую функцию под активную.
- Status: LCD background + LCD text; показывать только реальные статусы processor/library.

## 6. Вспомогательные окна и галерея

- FX EDIT: та же панельная поверхность, заголовок, selector, bypass/power, реальные parameter controls, график и status. Только существующие APVTS parameters.
- AUTOCHORDS: общая оболочка; generation остаётся честно отключённой, пока MIDI/transport engine не интегрирован.
- IMPORT GUITAR: themed SFZ import, loading state и реальный результат/ошибка.
- SETTINGS: реальные UI-only workspace scale/gap, restore NEON STAGE DEFAULT, open Skin Gallery.
- SKIN GALLERY: cards, preview, search, family filter, active marker. Preview/Apply/Back transaction, favorites и полноценная keyboard navigation пока не считаются реализованными.
- Workspace windows: единые titlebars, close, maximize/restore, resize, border, background, focus; bounds persistence и tiling не меняют processor/audio state.

## 7. Запрет фиктивных controls

LED activity, Auto Discover, Curator, About, Favorites, production skin rescan и дополнительные параметры, для которых нет реального binding/action, не должны отображаться как работающие кнопки. Допускается только честный disabled/deferred status после отдельного UI/UX решения. Категории и поиск библиотеки — это существующие browser controls, а не выдуманная файловая система.

## 8. Геометрия и безопасность

control_schema.json и theme.json задают ID и design grid. Пока runtime layout engine не использует controls.*.grid, фактические JUCE bounds остаются источником реальной геометрии. Не менять размеры/hit targets вслепую и не объявлять pixel parity до проверки семи исходных макетов при 100/125/150% DPI.

Любое оформление является visual-only: запрещено менять APVTS IDs/attachments, параметрические значения, DSP, sampler, MIDI, audio callback или поведение пресетов. Новые функции реализуются отдельными этапами с bindings, тестами и записью в генеральном журнале.

## 9. Приёмка

- Все реальные элементы главного окна и всех открытых окон используют активные NEON STAGE tokens.
- Normal/hover/pressed/selected/focus/disabled состояния различимы.
- Текст и числовые readouts остаются векторными и читаемыми.
- Никаких неработающих декоративных кнопок и выдуманных meter values.
- Build + CTest на точном SHA; затем визуальная проверка при 100/125/150% DPI и FL Studio 20 host acceptance.
- Отдельно: pixel-to-reference acceptance по всем семи утверждённым изображениям.