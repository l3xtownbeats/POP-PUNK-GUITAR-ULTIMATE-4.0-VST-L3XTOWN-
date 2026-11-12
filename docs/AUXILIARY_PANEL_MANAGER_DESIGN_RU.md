# PPGU — Auxiliary Panel Manager / технический контракт

Дата: 12 ноября 2026  
Статус: SOURCE INTEGRATED / CI VERIFIED ON PREVIOUS SOURCE SHA / HOST UI ACCEPTANCE PENDING  
Архитектура соответствует утверждённому UX «окно внутри окна»: панели являются дочерними overlay-компонентами внутри главного editor, а не отдельными top-level DocumentWindow.

## 1. Границы и ownership

- Главный `PopPunkGuitarUltimateAudioProcessorEditor` владеет lifetime всех panel content components и `PPGUAuxiliaryPanelWindowManager`.
- Manager владеет четырьмя overlay shells: FX EDIT, AUTOCHORDS, IMPORT GUITAR, SETTINGS.
- `PPGUEditorOverlayPanel` отображает заголовок, квадратную expand/restore кнопку, X и content component.
- PERFORMANCE — постоянно встроенный read-only display в основном блоке PLAYBACK над MIDI keyboard, не отдельная overlay-панель.
- Никакая панель не создаёт AudioProcessor, sampler, audio thread, MIDI scheduler или собственную копию APVTS.

## 2. Поведение панелей

- Launcher-кнопки открывают/закрывают соответствующую панель.
- X скрывает только выбранную панель; состояние APVTS и содержимое остальных панелей не сбрасывается.
- Квадратная кнопка расширяет выбранную панель в основную область, остальные открытые панели становятся вертикальным правым dock. Повторное нажатие возвращает обычную сетку.
- Grid: 1 — одна панель; 2 — 2x1; 3–4 — 2x2; 5–6 — 2x3. При недостатке минимальной рабочей области включается paging, а не overlap/clipping.
- Для текущего editor зарегистрированы четыре overlay-панели. Page capacity остаётся ограничена шестью на будущее.
- В PropertiesFile сохраняется список открытых Panel IDs. Размеры и bounds child overlays вычисляются от текущей области editor; независимое сохранение размера каждой панели не заявляется.

## 3. Подключённые функции и ограничения

- FX EDIT: существующие APVTS attachments; ровно 7 enable switches из runtimeControlInventory: compressorOn, eqOn, distortionOn, chorusOn, delayOn, reverbOn, cabinetOn. Flanger page не добавляет отдельный enable toggle.
- AUTOCHORDS: элементы UI отображаются disabled; Preview и генерация MIDI не имитируются.
- IMPORT GUITAR: вызывает существующий asynchronous SFZ loader.
- SETTINGS: использует только существующие APVTS parameter IDs.
- PERFORMANCE: read-only MIDI note/velocity/chord monitor; не отправляет MIDI. Host BPM/position остаются недоступны без realtime-safe transport snapshot.

## 4. Потоки и lifetime

- Все Component, bounds, focus и layout операции выполняются на JUCE Message Thread.
- PerformanceDisplay получает keyboard-state notifications, использует atomic note snapshot и обновляет labels таймером только пока компонент видим.
- SFZ import продолжает использовать существующий async loader; UI callback защищён SafePointer главного editor.
- При уничтожении editor сначала сохраняется список открытых панелей, затем manager закрывает overlays и уничтожается до освобождения panel content members.
- Audio callback, DSP, sampler lifecycle, MIDI routing/emission и APVTS semantics панелями не изменяются.

## 5. Известные ограничения / acceptance

- Windows interactive UI acceptance не подтверждён: открыть все панели, X, expand/restore, resize editor, small-size paging, skin hot-swap, close/reopen editor и teardown.
- Multi-monitor/DPI поведение требует ручной проверки в Windows/FL Studio 20.
- Отдельные окна операционной системы / native title bars не являются частью текущего UX-контракта.
- Skin Gallery, dynamic discovery/rescan, resource-root resolver и полный runtime skin engine остаются отдельными этапами REV18.
- Открытые sampler lifecycle риски R32 не затрагивались.

## 6. Verification

- Source integration commit: `38b57b854dc559e5dd7c408b96c6a7821eebbd58`.
- GitHub Actions run #1168 / ID `43230019761` на exact source SHA: Windows x64 Release configure/build, CTest, VST3 locate, pluginval strictness 5 и package steps SUCCESS.
- CI PASS подтверждает compile/test/package gates, но не заменяет визуальную проверку интерфейса в DAW.
