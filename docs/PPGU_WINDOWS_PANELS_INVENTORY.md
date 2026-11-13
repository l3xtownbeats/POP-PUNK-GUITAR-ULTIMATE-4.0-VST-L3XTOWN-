# PPGU — ИНВЕНТАРЬ ОКОН И ПАНЕЛЕЙ (v2, 2026-11-13)

Инвентарь отражает доступные записи и проверенное состояние интегрированной
рабочей ветки. Описание/макет не является доказательством реализации.

| # | Окно / панель | Назначение | Статус | Блокер / следующий шаг |
|---|---|---|---|---|
| 1 | PluginEditor | главное окно инструмента | SOURCE PRESENT | 21-token parity и exact-SHA validation |
| 2 | PPGUMultiWindowWorkspace | хост auxiliary panels | SOURCE PRESENT | OverlayManager integration not verified |
| 3 | Auxiliary panels | перемещаемые/изменяемые панели | SOURCE PRESENT | geometry, lifetime, DPI CI |
| 4 | Settings panel | scale/gap/restore/gallery settings | SOURCE IMPLEMENTED | exact-SHA validation |
| 5 | Skin Gallery | каталог визуальных тем | PARTIAL | карточки, большой preview, search/filter/favorites, Preview без сохранения, Apply/Back, runtime hot-swap |
| 6 | FX EDIT | редактирование 8 групп эффектов | DESIGNED / PARTIAL SOURCE AUDIT | проверить каждую страницу и control-to-APVTS/DSP mapping |
| 7 | Performance | read-only telemetry | DESIGNED | показывать только подключённые измерения; CPU/RAM пока не измерены |
| 8 | AutoChords | аккордовая генерация | DISABLED BY DESIGN | не показывать активным до backend/MIDI routing |
| 9 | Auto Discover | поиск/обнаружение материалов | NOT IMPLEMENTED | расширение существующего S6.5 pipeline |
| 10 | Curator | GREEN/YELLOW/RED review | NOT IMPLEMENTED | production policy integration |
| 11 | About | сведения о продукте/версии/лицензиях | NOT IMPLEMENTED | scoped UI implementation |
| 12 | Overlay windows | popup, z-order, focus, modal, persistence | SOURCE NOT VERIFIED IN INTEGRATED BRANCH | пользовательский отчёт O1; OverlayManager files не обнаружены в проверенной рабочей ветке |
| 13 | Secure Skin Import | импорт .ppguskin | SOURCE NOT VERIFIED IN INTEGRATED BRANCH | пользовательский отчёт O2; SecureSkinImport files и tests должны быть внесены и проверены |
| 14 | Import Guitar / Library Workbench | импорт WAV/SFZ/папок и управление библиотекой | SPECIFICATION AVAILABLE IN OTHER BRANCHES | selective integration; явная drop-zone, Browse, preview, mapping, license/provenance |
| 15 | Pattern section chooser | выбор Verse/Chorus/Bridge/Intro/Outro | NOT IMPLEMENTED | сначала song section, затем rhythm/pattern |

## Правила

- Не объявлять панель визуально или функционально завершённой по одному макету.
- DISABLED BY DESIGN не показывать активным до реализации backend.
- Любой статус SOURCE PRESENT требует проверки фактического исходника.
- Любой переход в IMPLEMENTED+VALIDATED требует отдельного scoped commit,
  exact-SHA CI и записи в Info.txt.
- Не менять утверждённую SIGNAL FORGE / NEON визуальную основу и не упрощать
  согласованные элементы управления без отдельного согласования.
