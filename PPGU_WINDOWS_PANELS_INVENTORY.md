# PPGU — ИНВЕНТАРЬ ОКОН И ПАНЕЛЕЙ (v1, 2026-11-13)

Статусная модель: IMPLEMENTED+VALIDATED / SOURCE PRESENT / DESIGNED /
NOT IMPLEMENTED / DISABLED BY DESIGN. Обновлять при каждом scoped-этапе.

| # | Окно / панель | Назначение | Статус | Блокер |
|---|---|---|---|---|
| 1 | PluginEditor (главное окно) | основной UI | SOURCE PRESENT | token-parity 21 partial; exact-SHA CI |
| 2 | PPGUMultiWindowWorkspace | хост auxiliary panels | SOURCE PRESENT | OverlayManager не интегрирован |
| 3 | Auxiliary panels (mixed sizes) | параллельные панели | SOURCE PRESENT | geometry CI PENDING |
| 4 | Settings panel (ID 6) | scale/gap/restore/gallery | SOURCE IMPLEMENTED | VALIDATION PENDING |
| 5 | Skin Gallery | выбор темы | PARTIAL | runtime hot-swap, DPI/ECO gates |
| 6 | FX EDIT | 8 страниц параметров | DESIGNED | source-to-control audit |
| 7 | Performance | read-only телеметрия | DESIGNED | не рисовать неподключённые данные |
| 8 | AutoChords | Circle of Fifths и chord management / harmony setup | SOURCE PRESENT / HARMONY SETUP ONLY | MIDI generation/playback/export and progression persistence not connected |
| 9 | Auto Discover | discovery UI | NOT IMPLEMENTED | pipeline S6.5 |
| 10 | Curator | GREEN/YELLOW/RED разбор | NOT IMPLEMENTED | policy in production |
| 11 | About | фактическая информация о продукте | SOURCE IMPLEMENTED / VALIDATION PENDING | exact-HEAD CI, resize/theme propagation and FL Studio 20 acceptance |
| 12 | Overlay windows (popup, z-order, focus, persistence, tiling) | — | NOT IMPLEMENTED | порядок: OverlayManager -> workspace -> окна |

Правила:
- Ни одна панель не объявляется «визуально завершённой» на основании одной
  только спецификации.
- DISABLED BY DESIGN элементы не рисуются активными, пока их runtime-функции
  не реализованы и не проверены.
- Каждый переход статуса — через отдельную запись INFO (+CODE при изменении
  исходников) с exact-SHA доказательством.


## Scoped update — 2026-11-15 (branch `ui/neon-stage-playing-core`)

- **About:** source implementation is present as `PPGUAboutPanel` (workspace panel ID 2103), opened by the enabled ABOUT button. It shows only established product identity/description and follows the active theme palette. It does not claim an invented version/build or expose dummy actions.
- **AUTO SOUND HUNTER / CURATOR:** remain disabled; production discovery/curation backends are not established.
- **AutoChords:** Circle of Fifths and chord-name/harmony setup are present in source; MIDI generation/playback remains unavailable by design.
- **Validation:** run `43718887827` passed for SHA `cd4935576c93f6974c6a5af4f319413b3ae03024`, which predates the About-panel source commits. The current branch HEAD `1a1910aad51538ef556f7b09902f52bdea3b069b` still requires exact-HEAD CI and host acceptance.


Audit note: the latest inventory and component-status reconciliation is committed on the working branch. Do not infer visual acceptance from source presence; current About panel and the earlier gallery fixes remain subject to the exact-HEAD workflow and separate FL Studio 20 host acceptance.

## Scoped update — 2026-11-15 (auxiliary workspace typography)

- Window-header title labels now receive the active theme `uiTypeface` and `fontScale` through `Window::setThemePalette()`.
- This closes one identified shared-typography inconsistency in workspace chrome; it does not complete panel geometry, lifecycle/focus/z-order, responsive sizing or visual acceptance.
- Source commit: `3190359d295d83bb06970836c9b34c62c6ccadb4`.
- Exact-HEAD build/CTest/pluginval and runtime/FL Studio 20 validation remain pending.

## Scoped update — 2026-11-15 (AutoChords typography)

- Circle of Fifths node labels and caption now use the shared theme typography helper, inheriting active `uiTypeface` and `fontScale`.
- Existing circle geometry, fifth-order/chromatic mapping, keyboard navigation, mouse selection and APVTS callback semantics are unchanged.
- Source commit: `597ad5ffe958ad4b33d440dfeb4e81b97e001fdf`.
- Exact-HEAD build/CTest/pluginval and runtime/FL Studio 20 validation remain pending.
