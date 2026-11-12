# PPGU — Auxiliary Panel Manager / технический контракт

Дата: 12 ноября 2026  
Статус: DESIGN BASELINE / CODE IMPLEMENTATION PENDING  
Цель: независимые дополнительные окна редактора по UX-контракту пользователя.

## 1. Границы

Основной PopPunkGuitarUltimateAudioProcessorEditor остаётся главным окном и владельцем lifetime UI-сессии. Дополнительные панели — отдельные top-level JUCE windows, принадлежащие editor-side AuxiliaryPanelManager. Ни одна панель не создаёт AudioProcessor, sampler, audio thread, MIDI scheduler или собственную копию APVTS.

Первый этап — инфраструктура окон и тестируемая геометрия. Подключение реальных функций (Settings, About, Curator, Auto Discover и другие) выполняется только после подтверждения их runtime-контрактов; пустые заглушки нельзя выдавать за работающие функции.

## 2. Модель окна

Каждая панель имеет стабильный PanelId, отдельный DocumentWindow/контейнер и собственные:
- заголовок;
- кнопку квадратного действия (maximize/restore в рамках доступной рабочей области);
- кнопку X (закрыть только эту панель);
- сохранённое состояние видимости, размера и normal bounds;
- минимальный размер и DPI-aware масштабирование.

Закрытие панели скрывает/уничтожает только её window shell. Состояние проекта и APVTS не сбрасывается. Повторное открытие использует тот же PanelId и восстанавливает безопасные bounds. Закрытие главного editor уничтожает все принадлежащие ему окна; никаких окон/таймеров не оставлять после editor teardown.

## 3. Размещение и compact mode

Менеджер вычисляет геометрию на UI message thread по доступной рабочей области экрана:
1. Одна панель — обычный размер, ограниченный work area.
2. Несколько панелей — детерминированная плиточная раскладка; все открытые окна получают неперекрывающиеся bounds.
3. Если естественные размеры не помещаются, включается compact mode: панели уменьшаются до заранее заданного минимально читаемого размера; содержимое панели использует responsive layout/scroll viewport.
4. Если даже минимальные bounds не помещаются, панели остаются неперекрывающимися и доступными через последовательную вкладку/переключатель; нельзя создавать фиктивную геометрию или обрезать важные controls без индикации.
5. Пересчёт выполняется при открытии/закрытии, resize главного окна, изменении display/DPI и изменении числа панелей.

Алгоритм должен учитывать work area, taskbar/dock, display scale и доступные экраны. Не использовать размеры всего виртуального desktop как bounds одного окна.

## 4. Квадратная кнопка и X

Квадратная кнопка переключает panel window между normal и maximized-to-work-area; повторное нажатие возвращает сохранённый normal bounds. Это не сворачивание в taskbar. X закрывает только выбранную панель. Нативный close action окна должен иметь ту же семантику, что и X.

## 5. Lifetime, потокобезопасность и аудио

- Все операции с Components, DocumentWindow, bounds, focus и z-order выполняются только на JUCE Message Thread.
- Manager и окна используют SafePointer/weak lifetime guard для асинхронных callbacks.
- Никаких UI callbacks в audio callback; никакого ожидания UI из audio thread.
- APVTS attachments создаются/уничтожаются на UI thread и используют существующие стабильные parameter IDs.
- Панели не удерживают сырые указатели на временные editor children.
- При editor destruction сначала отменить async callbacks/timers, затем закрыть окна, затем освободить manager.

## 6. Персистентность

Сохранять per-panel: visible flag, normal bounds, maximized flag, selected subview (если есть). Запись выполняется с debounce на UI thread. Bounds при восстановлении валидируются против текущих экранов; если монитор исчез или DPI изменился, окно переносится в видимую work area. Закрытие окна не означает сброс его пользовательских настроек.

## 7. Реализация по этапам

A — pure geometry model: чистый класс без JUCE Component ownership: вход — work area, scale, desired/minimum panel sizes и открытые IDs; выход — tile bounds + compact/overflow flags. Unit tests на 0/1/2/3/4+ окон, portrait/small work area, DPI scale, resize и детерминизм.

B — window shell: отдельный JUCE DocumentWindow для каждой панели, собственные square/X controls, focus/z-order, SafePointer lifetime и teardown tests.

C — responsive panel content: scroll/compact layout и проверка, что все controls достижимы; не уменьшать шрифты ниже установленного accessibility floor.

D — integration: добавить только реально существующие функции; для нереализованных функций показывать disabled/not implemented state, а не фальшивую активную панель.

E — acceptance: ручная проверка Windows 10 x64, FL Studio 20, multi-monitor/DPI, открыть все панели, закрыть каждую отдельно, maximize/restore, resize host editor, close/reopen editor, verify settings persistence; затем Windows Release, полный CTest, pluginval strictness 5 и package gate.

## 8. Запреты и критерии готовности

- Не менять DSP, sampler, audio callback, MIDI scheduling, APVTS IDs или preset schema ради окон.
- Не объявлять панель IMPLEMENTED до появления рабочего window shell и тестов.
- Не объявлять compact/auto-tile PASS без геометрических unit tests и Windows UI acceptance.
- Не считать skin layout metadata готовым оконным менеджером.
- Не создавать второй основной editor и не открывать несколько копий одного PanelId.
- Все окна должны оставаться в пределах видимой рабочей области и не перекрывать друг друга в tiled/compact mode.

## 9. Непосредственный следующий coding task

Создать AuxiliaryPanelLayout как pure geometry module + unit tests. После прохождения тестов подключить его к AuxiliaryPanelManager; только затем создавать отдельные top-level windows. Это минимизирует риск UI lifetime bugs и позволяет проверить auto-arrange независимо от JUCE window system.