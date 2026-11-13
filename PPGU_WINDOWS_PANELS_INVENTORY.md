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
| 8 | AutoChords | селекторы/превью | DISABLED BY DESIGN | backend/MIDI routing |
| 9 | Auto Discover | discovery UI | NOT IMPLEMENTED | pipeline S6.5 |
| 10 | Curator | GREEN/YELLOW/RED разбор | NOT IMPLEMENTED | policy in production |
| 11 | About | информация | NOT IMPLEMENTED | — |
| 12 | Overlay windows (popup, z-order, focus, persistence, tiling) | — | NOT IMPLEMENTED | порядок: OverlayManager -> workspace -> окна |

Правила:
- Ни одна панель не объявляется «визуально завершённой» на основании одной
  только спецификации.
- DISABLED BY DESIGN элементы не рисуются активными, пока их runtime-функции
  не реализованы и не проверены.
- Каждый переход статуса — через отдельную запись INFO (+CODE при изменении
  исходников) с exact-SHA доказательством.
