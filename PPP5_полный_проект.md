# Полный проект: POP PUNK GUITAR ULTIMATE 5.0
## Анализ движков, энергозатраты, ревью ThemeManager.cpp

*Составлено: Алиса AI, ноябрь 2026. Содержание: анализ Session Guitarist Electric Sunburst Deluxe, анализ Nexus 5, сравнение по энергозатратам, ревью и исправленный код ThemeManager.cpp.*

---

# Часть 1. Движок Session Guitarist Electric Sunburst Deluxe

## 1.1 Состав движка

**Сэмпловая база (~13,6 ГБ несжатых сэмплов).**
Одна гитара (Strat-типа), записанная с нескольких позиций: бриджевый звукосниматель, нековый, микрофон над струнами. Каждая нота на каждом ладу каждой струны — с несколькими velocity-слоями и циклами round-robin. Отдельные слои артикуляций: palm mute, хаммеры, слайды, скрипы, шумы рук.

**Движок строя и связок (strum engine).**
Ключевая фишка библиотеки: струны звучат каждая со своим ладом, задержки между щипками при брунькании рандомизируются, при смене аккорда работают легато, слайды и приглушения. В движке хранится «физика» инструмента: распределение нот по струнам, позиции на грифе, поведение открытых струн.

**Движок паттернов.**
237 паттернов — MIDI-данные с разметкой артикуляций и динамики. Движок мапит входящие аккорды с MIDI-клавиатуры на ритмический «скелет» паттерна. Риффы адаптируются хроматически: движок подменяет ступени лада под взятый аккорд, переборы распределяют ноты по струнам. Сюда же — синхронизация с хостом (tempo sync), вариации с циклами. Всего: 237 паттернов, 51 сонг, 27 новых струмов и 28 арпеджио в Deluxe-версии.

**Два режима.**
Performance — аккорды + паттерны. Melody — сольная игра с легато, бендами, вибрато (слайды запускаются при velocity > 110).

**Обработка звука.**
Микс-консоль: баланс датчиков, примесь микрофона, цепочка амп-симуляций, кабинетов и педалей (Choral, Flair, Phasis, Replika Delay, алгоритмический ревербератор).

**Пресеты.**
Sound Presets = настройки Guitar Settings + Amps & FX. Загрузка сонга при включённом Link to Song подтягивает его звуковой пресет. Точные названия фабричных пресетов в открытом доступе не публикуются — список виден в выпадающем меню Sound Preset в плагине или по файлам .nka в папке библиотеки.

## 1.2 Принцип работы по шагам

1. Игрок нажимает аккорд на MIDI-клавиатуре.
2. Движок распознаёт аккорд (внутренняя таблица интервалов).
3. Выбранный паттерн (MIDI-скелет) перестраивается: ноты скелета проецируются на реальный строй гитары под взятый аккорд — хроматическая адаптация ступеней.
4. Strum-движок рассчитывает задержки между струнами, легато, глушения при смене аккорда.
5. Семплы проигрываются с правильными velocity, round-robin и кроссфейдами.
6. Звук проходит микс-консоль (датчики + микрофон) и цепочку эффектов.

Итого: паттерн = ритмический скелет, движок = «клей», пересобирающий записанные фразы под живую гармонию игрока.

## 1.3 Сравнение с аналогами

| Библиотека | Философия струм-движка |
|---|---|
| Session Guitarist (NI) | Паттерны + распознавание аккордов, «пресет = готовая партия» |
| Wavesfactory Strum Guitar | Прямое воспроизведение брунькания с клавиатуры |
| Ilya Efimov LP Strum | Детальная физика струн, ручное управление |
| Ample Sound | Гибрид: паттерны + глубокая ручная игра |
| Vir2 Electri6ity | Максимальная детализация артикуляций, сложнее в освоении |

## 1.4 Шпаргалка приёмов из мануала

- Переходы между аккордами: слайды и легато запускаются автоматически при velocity > 110.
- Двойной экземпляр плагина: один для риффов (Performance), второй для соло (Melody) — быстрый способ разделить партии.
- Accent: выразительность управляется отдельной клавишей-акцентом, а не только velocity.

## 1.5 Итоговая оценка архитектуры

Сильные стороны: реалистичная физика струн, хроматическая адаптация паттернов, готовые сонги, оптимизированный движок (работает на средних ноутбуках).
Слабые: закрытость формата, привязка к Kontakt, невозможность разбора флагманских сэмплов до первоосновы.

---

# Часть 2. Движок Nexus 5 (reFX)

## 2.1 Эволюция из ромплера

Nexus 2–4 были закрытыми плеерами пресетов. В ноябре 2024 пятая версия вскрыла движок: пользователи получили тот же доступ к параметрам, что и штатные саунд-дизайнеры reFX.

## 2.2 Архитектура

- 8 генераторов: VA (виртуальный аналоговый), сэмплер с тайм-стретчем, wavetable, ретро-сэмплер 80-х, Grain, Cloud, 4-операторный FM.
- До 4 слоёв обработки, до 64 осцилляторов в патче.
- Слой = генератор + фильтр + собственная секвенция; всё связывается Mod Matrix.
- Контент: 5352 пресета, 6560 мультисэмплов (~62,3 ГБ), 215 импульсных откликов, 119 арпеджио, 52 транcгейта.

## 2.3 Принцип работы (сигнальный путь)

Слои → фильтры → Mod Matrix → пер-слойные эффекты → мастер-эффекты → арпеджиатор/trancegate → выход.

- Арпеджиатор: режимы Up/Down/Order/Seq до 256 шагов, функция Strum.
- Trancegate: гейт по секвенции, 52 пресета.

## 2.4 Что портировать в свой движок

1. Слой = генератор + фильтр + своя секвенция — модульная схема вместо жёсткой.
2. Пресет «запечён» под микс: эффекты внутри пресета — пользователь получает готовый звук сразу.
3. Простые в реализации фишки: ретро-биткрашер, trancegate, BBD-дилей.

## 2.5 Риски для пользователей из РФ

В 2022 году reFX заблокировала учётные записи клиентов из России и Беларуси и отозвала лицензии; сайт компании с тех пор недоступен на территории этих стран. Это важно учитывать при выборе референса и покупке.

## 2.6 Аналоги

Serum 2/Vital, Avenger, Omnisphere 3, Falcon 3, HALion 7 — с общим трейд-оффом: глубина против скорости.

## 2.7 Итоговая оценка

Плюсы: скорость работы, низкий CPU, открытость v5.
Минусы: workflow остаётся пресетным, флагманские сэмплы не разбираются до первоосновы.

---

# Часть 3. Сравнение движков и выбор для своего плагина

## 3.1 Энергозатраты

По «энергоэкономичности» выигрывает принцип Nexus 5 (ромплер: один голос = сэмпл + фильтр + пара эффектов — сотни голосов без напряжения). Но для гитарного плагина правильнее механика Sunburst — просто её нужно сделать лёгкой: движок там решает задачу (голоса на 6 струн, расчёт задержек струн, распознавание аккордов, активный стриминг), а не проигрывает файл.

## 3.2 Практические решения для экономии CPU

| Решение | Экономия | Комментарий |
|---|---|---|
| Сэмплы вместо синтеза звука струн | Высокая | Синтез гитары — dead end |
| Гибрид «Nexus-эффекты + Sunburst-логика» | Оптимально | Струм-движок свой, обработка — лёгкие DSP или хост |
| Запечённые в сэмплы шумы, слайды, release | Высокая | Меньше скриптовых огибающих в рантайме |
| Паттерны как MIDI-скелеты, а не сэмплы | Высокая | Копия подхода Sunburst, почти бесплатная по CPU |
| Отказ от встроенных амп-моделей на старте | Высокая | DI-сэмплы, обработку — хосту |

## 3.3 Рекомендованный стек

- **HISE** — ядро: семплер + скриптинг + UI + экспорт VST/AU/AAX. Лицензия GPL v3 или коммерческая для закрытого кода; сэмплы — отдельно (CC0 идеален). Обходной путь без компиляции — открытый плеер Rhapsody от Libre Wave.
- **JUCE (C++)** — фреймворк под HISE; самостоятельная разработка на нём — дорога в полтора года вместо месяцев.
- **SFZ + sforzando** — быстрый прототип маппинга; струм-механику собрать частично (delay, sw_last), но без живой логики аккордов.
- **Decent Sampler** — канал дистрибуции ранних версий; при сложных паках упираешься в потолок движка.
- Сэмплы: Freesound.org, CC0-каталоги (например, халлоубоди Karoryfer, 500 МБ, CC0) или своя запись. MIDI-паттерны: royalty-free каталоги с CC0/CC-BY.
- Ампы на старте не писать: чистые DI-сэмплы + внешнее амп-моделирование.

Целевая нагрузка: 5–10% CPU на проект.

---

# Часть 4. Ревью ThemeManager.cpp

## 4.1 Критичное

**1. drawWhiteKey / drawBlackKey не соберутся с override.**
В JUCE у LookAndFeel_V4 эти методы имеют другую сигнатуру — drawWhiteKey (int midiNoteNumber, Component& keyboard, Graphics&, const Rectangle<float>& area, bool isDown, bool isOver, ...). Вариант (Graphics&, int x, int y, int w, int h, bool down) ничего не переопределяет, и override даст ошибку компиляции. Если клавиатура — собственный компонент, убрать override и переименовать под свой интерфейс; если juce::KeyboardComponentBase — переписать под его сигнатуру.

**2. Заливка слайдера перевёрнута.**
`auto fill = track.withBottom (sliderPos);` — заполняется область ВЫШЕ бегунка: на нуле слайдер закрашен полностью, на максимуме пуст. Для вертикального слайдера нужно:

```cpp
auto fill = track.withTop (sliderPos);   // заливка от бегунка вниз
```
Заодно зажать sliderPos в диапазоне трека через jlimit.

**3. Слайдеры других стилей исчезнут.**
Перегружен drawLinearSlider, рисуется только LinearVertical, базовый метод не вызывается — горизонтальный слайдер отрисуется пустым. Добавить в конец:

```cpp
LookAndFeel_V4::drawLinearSlider (g, x, y, w, h, sliderPos, min, max, style, slider);
```

## 4.2 Архитектурные ловушки

**4. ThemeSettings — глобальный статик на все инстансы плагина.**
`inline static juce::String themeId` — общий на весь процесс: два инстанса в DAW будут перетирать тему друг друга. Тема должна жить в инстансе — полем Processor/Editor или в APVTS-состоянии каждого инстанса.

**5. Тема не восстановится из сохранённого проекта.**
В attach() тема читается один раз; листенера нет, а комментарий его обещает. При setStateInformation тема не применится. Лечение — перекладывать тему в APVTS-параметр (AudioParameterChoice со списком тем из scanThemes): сохранение/восстановление DAW решает сама.

**6. Поиск цвета по строке на каждый кадр.**
col() дёргает поиск по строке в каждом paint-вызове (десятки раз в секунду на каждый элемент). Разово при applyTheme() резолвить все ключи в готовые juce::Colour и держать кэш в Theme.

## 4.3 Мелочи

- В col() добавить jassertfalse при отсутствии ключа в палитре (в отладочной сборке) — сейчас опечатка молча подменяется розовым фолбэком.
- Не покрыты: drawToggleButton, ComboBox (список пресетов), Label, ScrollBar, TableHeaderComponent — они отрисуются дефолтным V4-стилем и будут выглядеть инородно.
- FontOptions (height, styleFlags) — API свежих JUCE 7.x; на старых версиях не соберётся.

## 4.4 Что хорошо

Разделение «LookAndFeel только читает палитру, менеджер владеет темой» — правильное; фолбэки на месте; скругления и толщины вынесены в код; интеграция по пяти шагам в шапке файла — по-взрослому.

---

# Часть 5. Исправленный ThemeManager.cpp

```cpp
/*
    ThemeManager.cpp  (исправленная версия)
    POP PUNK GUITAR ULTIMATE 5.0
    LookAndFeel, рисующий все элементы по палитре темы.

    Изменения против исходной версии:
      1. Убран override с клавиатурных методов (сигнатуры не совпадают с JUCE).
      2. Заливка вертикального слайдера исправлена (withTop вместо withBottom),
         sliderPos зажат в диапазоне трека.
      3. Неизвестные стили слайдера делегируются базовому LookAndFeel_V4.
      4. ThemeSettings больше не static: тема живёт в инстансе.
      5. Тема хранится как AudioParameterChoice в APVTS — восстанавливается DAW.
      6. Цвета резолвятся в кэш один раз при applyTheme(), а не на каждый кадр.
*/

#include "ThemeManager.h"

namespace ppgu
{

//==============================================================================
class PpguLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void setThemeManager (ThemeManager* tm) { manager = tm; }

    Theme* theme() const
    {
        return manager ? manager->current() : nullptr;
    }

    juce::Colour col (const juce::String& key, juce::Colour fallback) const
    {
        if (auto* t = theme())
        {
            if (auto c = t->cached (key))
                return *c;

            jassertfalse; // ключ не найден в палитре — исправь имя ключа
        }
        return fallback;
    }

    // ---- Ручки (круглые) -------------------------------------------------
    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override
    {
        auto box = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (2.0f);
        auto radius = juce::jmin (box.getWidth(), box.getHeight()) / 2.0f;
        auto centre = box.getCentre();
        auto angle  = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        g.setColour (col ("knob.face", juce::Colours::darkgrey));
        g.fillEllipse (juce::Rectangle<float> (radius * 2.0f, radius * 2.0f).withCentre (centre));

        juce::Path arc;
        arc.addCentredArc (centre.x, centre.y, radius, radius, 0.0f,
                           rotaryStartAngle, angle, true);
        g.setColour (col ("knob.ring", juce::Colours::pink));
        g.strokePath (arc, juce::PathStrokeType (3.0f));

        juce::Line<float> pointer (centre.x, centre.y,
                                   centre.x + std::cos (angle - juce::MathConstants<float>::halfPi) * radius * 0.8f,
                                   centre.y + std::sin (angle - juce::MathConstants<float>::halfPi) * radius * 0.8f);
        g.setColour (col ("knob.pointer", juce::Colours::white));
        g.drawLine (pointer, 2.0f);
    }

    // ---- Кнопки ----------------------------------------------------------
    void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                               const juce::Colour&, bool highlighted, bool down) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
        bool active = button.getToggleState();

        auto bg = active ? col ("button.active", juce::Colours::pink)
                         : (down || highlighted ? col ("button.hover", juce::Colours::darkgrey)
                                                : col ("button.normal", juce::Colours::grey));
        g.setColour (bg);
        g.fillRoundedRectangle (bounds, 6.0f);
        g.setColour (col ("panel.border", juce::Colours::black));
        g.drawRoundedRectangle (bounds, 6.0f, 1.0f);
    }

    juce::Font getTextButtonFont (juce::TextButton&, int height) override
    {
        return juce::Font (juce::FontOptions (juce::jmin (height * 0.55f, 20.0f), juce::Font::bold));
    }

    // ---- Слайдеры --------------------------------------------------------
    void drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h,
                           float sliderPos, float minPos, float maxPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (style == juce::Slider::LinearVertical)
        {
            auto track = juce::Rectangle<float> ((float) x + w / 2.0f - 3.0f, (float) y, 6.0f, (float) h);
            g.setColour (col ("slider.track", juce::Colours::darkgrey));
            g.fillRoundedRectangle (track, 3.0f);

            // ИСПРАВЛЕНО: заливка от бегунка вниз, позиция зажата в треке
            auto clamped = juce::jlimit (track.getY(), track.getBottom(), sliderPos);
            auto fill = track.withTop (clamped);
            g.setColour (col ("slider.fill", juce::Colours::pink));
            g.fillRoundedRectangle (fill, 3.0f);
            return;
        }

        // ИСПРАВЛЕНО: остальные стили рисует базовый LookAndFeel
        LookAndFeel_V4::drawLinearSlider (g, x, y, w, h, sliderPos,
                                          minPos, maxPos, style, slider);
    }

    // ---- Клавиатура (собственный компонент: override убран) --------------
    void drawWhiteKey (juce::Graphics& g, int x, int y, int w, int h, bool down)
    {
        auto key = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h);
        g.setColour (down ? col ("keyboard.active", juce::Colours::pink)
                          : col ("keyboard.white", juce::Colours::white));
        g.fillRect (key);
        g.setColour (col ("panel.border", juce::Colours::black));
        g.drawRect (key, 1.0f);
    }

    void drawBlackKey (juce::Graphics& g, int x, int y, int w, int h, bool down)
    {
        auto key = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h);
        g.setColour (down ? col ("keyboard.active", juce::Colours::pink)
                          : col ("keyboard.black", juce::Colours::black));
        g.fillRect (key);
    }

private:
    ThemeManager* manager = nullptr;
};

//==============================================================================
// ИСПРАВЛЕНО: тема принадлежит инстансу плагина, а не процессу.
// Хранится как AudioParameterChoice в APVTS — DAW сохраняет и восстанавливает её сама.
class ThemeSettings
{
public:
    static void attach (juce::AudioProcessorValueTreeState& apvts,
                        const juce::StringArray& availableThemeIds)
    {
        if (apvts.getParameter ("themeId") == nullptr)
            apvts.createAndAddParameter (std::make_unique<juce::AudioParameterChoice> (
                "themeId", "Theme",
                availableThemeIds, getDefaultIndex (availableThemeIds)));
    }

    static juce::String current (juce::AudioProcessorValueTreeState& apvts)
    {
        if (auto* p = apvts.getRawParameterValue ("themeId"))
        {
            if (auto* choice = dynamic_cast<juce::AudioParameterChoice*> (
                    apvts.getParameter ("themeId")))
                return choice->getAllValueStrings()[(int) p->load()];
        }
        return "neon_riot";
    }

private:
    static int getDefaultIndex (const juce::StringArray& ids)
    {
        return juce::jmax (0, ids.indexOf ("neon_riot"));
    }
};

} // namespace ppgu
```

Кэш цветов (пункт 6 ревью) реализуется в ThemeManager.h: при applyTheme() Theme проходит по всем ключам палитры и заполняет `std::map<juce::String, juce::Colour> cache;` — метод `cached(key)` читает из него, а не ищет по строке.

---

# Приложение. Структура сэмплинг-сессии (минимум для запуска движка)

- Одна гитара, 2 позиции датчиков + микрофон над струнами.
- Ноты по полутонам на рабочем диапазоне каждой струны (или каждые 2–3 полутона с пичшифтом).
- 3–4 velocity-слоя, 3 round-robin-цикла на ноту.
- Отдельные артикуляции: palm mute (по тем же нотам), слайды вверх/вниз, шумы рук, release.
- Хранение: HLAC (HISE) или FLAC; DI-сэмплы без обработки.
