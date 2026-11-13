# PPGU SKIN PACK — PREMIUM COLLECTION v1.1 (Skin_053 … Skin_072)

Статус: PACK SPEC GENERATED / G1-CONTRAST PRE-CHECKED (все пары >= 4.5, secondary >= 7, primary >= 10) / NOT MERGED / NOT RUNTIME-INTEGRATED
База: REV 14.0, 21-token model, canonical theme.schema.json; при merge обязателен контроль parity 27 control IDs.
Нумерация непрерывная после эталона Skin_052 (Carbon Relay). v1.1: автокоррекция контрастов textSecondary/textDisabled по валидатору.

## 0. Исправления главной темы SIGNAL FORGE / NEON (SKIN-1)

1. **DEFAULT/fallback возвращён к `skin-01` (SIGNAL FORGE / NEON).** Состояние main (`skin-51` как default/fallback) противоречит утверждённому правилу от 13.11: Blackout Voltage — только дополнительная тема. Исправление — отдельным этапом после аудита, без пересоздания компонентов.
2. **Усилен контраст вторичного текста:** `textSecondary` >= 7:1 на window, `textDisabled` >= 4.5:1 — читаемость подписей на малых размерах и при DPI-масштабировании.
3. **`accentGlow` — отдельный токен** для outer-glow, чтобы не пересвечивать тонкие элементы через accentPrimary.
4. **`focusRing` — отдельный токен** для клавиатурной навигации (Ctrl+K) вместо наследования activeState.
5. **Правило арта:** сплэт-арт и hero — только баннер/превью; контролы всегда чистые.

## 1. Правила пакета

- Каждая тема: 21 palette token, 27 обязательных control IDs, layout-variant, `reduceInEco=true`, `hotSwapFadeMs=150`, compatibility flags APVTS/layout/audio = false.
- Палитровые варианты с единым layout-инвентарём; hero/texture — SVG, preview — PNG 360×240, hero — PNG 800×450 (по эталону Skin_052).
- Порядок: pack manifest 53–72 → `tools/merge_skin_pack.py` (с backup) → синхронное обновление registry/schema/validator (RISK-B2) → G1–G4 в CI → только потом статусы VALIDATION PASS / MERGED.

## 2. Каталог тем

### Skin_053 — FLEX PULSE

- ID: `skin-053` · family: `light_desk` · layout variant: `LIGHT_DESK`
- Референс-стилистика: Nexus 3 / Flex-стиль: светлая рабочая станция.
- Контрасты (G1): text/window **14.09:1**, secondary/window **7.23:1**, disabled/window **4.76:1**, LCD text/LCD **10.07:1**, accent/panel **4.27:1**.

| Токен | Значение |
|---|---|
| `window` | `#eef1f5` |
| `panel` | `#ffffff` |
| `panelEdge` | `#c9d2dd` |
| `textPrimary` | `#1a2230` |
| `textSecondary` | `#445060` |
| `textDisabled` | `#646b73` |
| `accentPrimary` | `#1f7ae0` |
| `accentSecondary` | `#00a6c8` |
| `accentGlow` | `#66b8ff` |
| `controlSurface` | `#f6f8fb` |
| `controlMark` | `#8fa4ba` |
| `activeState` | `#1f7ae0` |
| `inactiveState` | `#c9d2dd` |
| `lcdBackground` | `#12203a` |
| `lcdText` | `#8fd4ff` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#e0b400` |
| `meterHigh` | `#e04141` |
| `overlayScrim` | `#10182855` |
| `textureTint` | `#dce6f2` |
| `focusRing` | `#1f7ae0` |

### Skin_054 — SESSION WOOD

- ID: `skin-054` · family: `warm_acoustic` · layout variant: `STUDIO_CONSOLE`
- Референс-стилистика: NI Session Guitarist / Kontakt-стиль: тёплое дерево и крем.
- Контрасты (G1): text/window **14.06:1**, secondary/window **8.81:1**, disabled/window **4.69:1**, LCD text/LCD **14.36:1**, accent/panel **7.08:1**.

| Токен | Значение |
|---|---|
| `window` | `#241a12` |
| `panel` | `#2f2318` |
| `panelEdge` | `#4a3826` |
| `textPrimary` | `#f2e8d8` |
| `textSecondary` | `#c9b8a0` |
| `textDisabled` | `#938471` |
| `accentPrimary` | `#e8a33d` |
| `accentSecondary` | `#c96f2e` |
| `accentGlow` | `#ffce7a` |
| `controlSurface` | `#3a2c1d` |
| `controlMark` | `#d9b26a` |
| `activeState` | `#e8a33d` |
| `inactiveState` | `#4a3826` |
| `lcdBackground` | `#1a120b` |
| `lcdText` | `#ffdf9e` |
| `meterLow` | `#5a8f4a` |
| `meterMid` | `#d9b23d` |
| `meterHigh` | `#c94f2e` |
| `overlayScrim` | `#0e0a06aa` |
| `textureTint` | `#33261a` |
| `focusRing` | `#e8a33d` |

### Skin_055 — ANALOG TAPE

- ID: `skin-055` · family: `vintage_rack` · layout variant: `VINTAGE_RACK`
- Референс-стилистика: Vintage tape machine: VU-стрелка, крем и латунь.
- Контрасты (G1): text/window **13.15:1**, secondary/window **8.46:1**, disabled/window **4.63:1**, LCD text/LCD **14.85:1**, accent/panel **5.99:1**.

| Токен | Значение |
|---|---|
| `window` | `#26221c` |
| `panel` | `#332e25` |
| `panelEdge` | `#4c4436` |
| `textPrimary` | `#f0ead8` |
| `textSecondary` | `#c6bda6` |
| `textDisabled` | `#938a78` |
| `accentPrimary` | `#d9a441` |
| `accentSecondary` | `#a67c3b` |
| `accentGlow` | `#ffd98a` |
| `controlSurface` | `#3d362b` |
| `controlMark` | `#cbb277` |
| `activeState` | `#d9a441` |
| `inactiveState` | `#4c4436` |
| `lcdBackground` | `#15120d` |
| `lcdText` | `#ffe2a8` |
| `meterLow` | `#6a9a55` |
| `meterMid` | `#cdb04a` |
| `meterHigh` | `#c46a3a` |
| `overlayScrim` | `#0d0b08aa` |
| `textureTint` | `#2e2a20` |
| `focusRing` | `#d9a441` |

### Skin_056 — PRO GRADIENT

- ID: `skin-056` · family: `minimal_console` · layout variant: `STUDIO_CONSOLE`
- Референс-стилистика: FabFilter-стиль: сдержанный серо-синий минимализм.
- Контрасты (G1): text/window **13.71:1**, secondary/window **8.23:1**, disabled/window **4.58:1**, LCD text/LCD **13.46:1**, accent/panel **5.49:1**.

| Токен | Значение |
|---|---|
| `window` | `#23272e` |
| `panel` | `#2d323a` |
| `panelEdge` | `#434a54` |
| `textPrimary` | `#f2f5f9` |
| `textSecondary` | `#b8c1cc` |
| `textDisabled` | `#878f99` |
| `accentPrimary` | `#38b6e0` |
| `accentSecondary` | `#2f80c8` |
| `accentGlow` | `#7cd8ff` |
| `controlSurface` | `#1c2026` |
| `controlMark` | `#5f8fb8` |
| `activeState` | `#38b6e0` |
| `inactiveState` | `#434a54` |
| `lcdBackground` | `#10151c` |
| `lcdText` | `#a8e6ff` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#d8b83a` |
| `meterHigh` | `#e05050` |
| `overlayScrim` | `#0a0e14aa` |
| `textureTint` | `#262c34` |
| `focusRing` | `#38b6e0` |

### Skin_057 — OMNI NEBULA

- ID: `skin-057` · family: `cosmic_stage` · layout variant: `DARK_STAGE`
- Референс-стилистика: Omnisphere-стиль: глубокий космос и фиолетовая туманность.
- Контрасты (G1): text/window **16.61:1**, secondary/window **9.33:1**, disabled/window **4.67:1**, LCD text/LCD **13.08:1**, accent/panel **4.31:1**.

| Токен | Значение |
|---|---|
| `window` | `#120b1e` |
| `panel` | `#1c1330` |
| `panelEdge` | `#332354` |
| `textPrimary` | `#f0ecff` |
| `textSecondary` | `#b9aee0` |
| `textDisabled` | `#8176aa` |
| `accentPrimary` | `#8d5cff` |
| `accentSecondary` | `#c44dff` |
| `accentGlow` | `#b98aff` |
| `controlSurface` | `#231740` |
| `controlMark` | `#8f7ad0` |
| `activeState` | `#8d5cff` |
| `inactiveState` | `#332354` |
| `lcdBackground` | `#0a0614` |
| `lcdText` | `#d8c9ff` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#d8b83a` |
| `meterHigh` | `#e05050` |
| `overlayScrim` | `#06040caa` |
| `textureTint` | `#1a1030` |
| `focusRing` | `#8d5cff` |

### Skin_058 — SERUM VOLTAGE

- ID: `skin-058` · family: `aggressive_modern` · layout variant: `DARK_STAGE`
- Референс-стилистика: Serum-стиль: угольная тьма и кислотная зелень.
- Контрасты (G1): text/window **17.43:1**, secondary/window **9.95:1**, disabled/window **4.61:1**, LCD text/LCD **17.37:1**, accent/panel **13.16:1**.

| Токен | Значение |
|---|---|
| `window` | `#101312` |
| `panel` | `#181d1a` |
| `panelEdge` | `#2a332c` |
| `textPrimary` | `#eefaf2` |
| `textSecondary` | `#a8c4b0` |
| `textDisabled` | `#6b8475` |
| `accentPrimary` | `#57ff9a` |
| `accentSecondary` | `#1fd4c8` |
| `accentGlow` | `#9dffc8` |
| `controlSurface` | `#1a221d` |
| `controlMark` | `#4fae7f` |
| `activeState` | `#57ff9a` |
| `inactiveState` | `#2a332c` |
| `lcdBackground` | `#070a08` |
| `lcdText` | `#b8ffd9` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#e0b400` |
| `meterHigh` | `#ff5050` |
| `overlayScrim` | `#040605aa` |
| `textureTint` | `#121814` |
| `focusRing` | `#57ff9a` |

### Skin_059 — VITAL FLOW

- ID: `skin-059` · family: `gradient_modern` · layout variant: `STUDIO_CONSOLE`
- Референс-стилистика: Vital-стиль: тёмный корпус и живой градиент маджента-бирюза.
- Контрасты (G1): text/window **16.52:1**, secondary/window **8.94:1**, disabled/window **4.59:1**, LCD text/LCD **12.34:1**, accent/panel **5.19:1**.

| Токен | Значение |
|---|---|
| `window` | `#131419` |
| `panel` | `#1b1d26` |
| `panelEdge` | `#2e3140` |
| `textPrimary` | `#f2f2fa` |
| `textSecondary` | `#b0b4c8` |
| `textDisabled` | `#7b7e91` |
| `accentPrimary` | `#e04df0` |
| `accentSecondary` | `#22d3ee` |
| `accentGlow` | `#f09aff` |
| `controlSurface` | `#20222e` |
| `controlMark` | `#9a6fd8` |
| `activeState` | `#e04df0` |
| `inactiveState` | `#2e3140` |
| `lcdBackground` | `#0a0b10` |
| `lcdText` | `#eebbff` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#e0b400` |
| `meterHigh` | `#ff5050` |
| `overlayScrim` | `#05060aaa` |
| `textureTint` | `#171821` |
| `focusRing` | `#e04df0` |

### Skin_060 — TRAKTOR DECK

- ID: `skin-060` · family: `dj_hardware` · layout variant: `STUDIO_CONSOLE`
- Референс-стилистика: Traktor/NI-стиль: инженерный серый и неоновый оранж.
- Контрасты (G1): text/window **15.62:1**, secondary/window **8.67:1**, disabled/window **4.79:1**, LCD text/LCD **13.51:1**, accent/panel **5.61:1**.

| Токен | Значение |
|---|---|
| `window` | `#1b1d1f` |
| `panel` | `#26292c` |
| `panelEdge` | `#3d4145` |
| `textPrimary` | `#f5f6f7` |
| `textSecondary` | `#b6babf` |
| `textDisabled` | `#84898e` |
| `accentPrimary` | `#ff7a1a` |
| `accentSecondary` | `#ff3d3d` |
| `accentGlow` | `#ffa666` |
| `controlSurface` | `#202427` |
| `controlMark` | `#c97a3d` |
| `activeState` | `#ff7a1a` |
| `inactiveState` | `#3d4145` |
| `lcdBackground` | `#0c0e10` |
| `lcdText` | `#ffcf9e` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#e0b400` |
| `meterHigh` | `#ff4040` |
| `overlayScrim` | `#08090aaa` |
| `textureTint` | `#1e2124` |
| `focusRing` | `#ff7a1a` |

### Skin_061 — MPC GOLD

- ID: `skin-061` · family: `hiphop_hardware` · layout variant: `STUDIO_CONSOLE`
- Референс-стилистика: MPC-стиль: матовый чёрный, пад-сетка и золото.
- Контрасты (G1): text/window **16.42:1**, secondary/window **9.62:1**, disabled/window **4.65:1**, LCD text/LCD **16.01:1**, accent/panel **9.28:1**.

| Токен | Значение |
|---|---|
| `window` | `#141110` |
| `panel` | `#1e1a18` |
| `panelEdge` | `#332c27` |
| `textPrimary` | `#f5efe4` |
| `textSecondary` | `#c4b8a6` |
| `textDisabled` | `#877d70` |
| `accentPrimary` | `#e6b83c` |
| `accentSecondary` | `#b3742e` |
| `accentGlow` | `#ffd97a` |
| `controlSurface` | `#241f1c` |
| `controlMark` | `#cfa852` |
| `activeState` | `#e6b83c` |
| `inactiveState` | `#332c27` |
| `lcdBackground` | `#0b0908` |
| `lcdText` | `#ffe4a8` |
| `meterLow` | `#4fae58` |
| `meterMid` | `#e0b83a` |
| `meterHigh` | `#e0563a` |
| `overlayScrim` | `#080604aa` |
| `textureTint` | `#1a1614` |
| `focusRing` | `#e6b83c` |

### Skin_062 — SYNTHWAVE SUNSET

- ID: `skin-062` · family: `retrowave` · layout variant: `DARK_STAGE`
- Референс-стилистика: Ретровейв: закатный градиент и неоновая сетка.
- Контрасты (G1): text/window **17.05:1**, secondary/window **9.45:1**, disabled/window **4.58:1**, LCD text/LCD **12.48:1**, accent/panel **5.30:1**.

| Токен | Значение |
|---|---|
| `window` | `#160c1c` |
| `panel` | `#221229` |
| `panelEdge` | `#3d1e4a` |
| `textPrimary` | `#ffeef6` |
| `textSecondary` | `#d9a8cc` |
| `textDisabled` | `#9a6f91` |
| `accentPrimary` | `#ff3d8a` |
| `accentSecondary` | `#ff8a3d` |
| `accentGlow` | `#ff77b8` |
| `controlSurface` | `#251430` |
| `controlMark` | `#d45fa8` |
| `activeState` | `#ff3d8a` |
| `inactiveState` | `#3d1e4a` |
| `lcdBackground` | `#0d060f` |
| `lcdText` | `#ffb8d8` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#ffb400` |
| `meterHigh` | `#ff4060` |
| `overlayScrim` | `#070408aa` |
| `textureTint` | `#1c0e22` |
| `focusRing` | `#ff3d8a` |

### Skin_063 — EMERALD RACK

- ID: `skin-063` · family: `rack_hardware` · layout variant: `VINTAGE_RACK`
- Референс-стилистика: Классический rack-эффект: изумруд и латунь.
- Контрасты (G1): text/window **16.65:1**, secondary/window **10.16:1**, disabled/window **4.69:1**, LCD text/LCD **16.39:1**, accent/panel **8.18:1**.

| Токен | Значение |
|---|---|
| `window` | `#0f1712` |
| `panel` | `#16221a` |
| `panelEdge` | `#28402f` |
| `textPrimary` | `#eef7ee` |
| `textSecondary` | `#aec8b2` |
| `textDisabled` | `#6f8775` |
| `accentPrimary` | `#35d07a` |
| `accentSecondary` | `#8fc89a` |
| `accentGlow` | `#7affc0` |
| `controlSurface` | `#182219` |
| `controlMark` | `#5fae7f` |
| `activeState` | `#35d07a` |
| `inactiveState` | `#28402f` |
| `lcdBackground` | `#081009` |
| `lcdText` | `#a8ffcc` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#e0c040` |
| `meterHigh` | `#e05540` |
| `overlayScrim` | `#050a06aa` |
| `textureTint` | `#101a12` |
| `focusRing` | `#35d07a` |

### Skin_064 — CRIMSON STAGE

- ID: `skin-064` · family: `aggressive_stage` · layout variant: `DARK_STAGE`
- Референс-стилистика: Метал-сцена: багровый на чёрном.
- Контрасты (G1): text/window **16.67:1**, secondary/window **9.01:1**, disabled/window **4.64:1**, LCD text/LCD **11.45:1**, accent/panel **5.27:1**.

| Токен | Значение |
|---|---|
| `window` | `#150c0c` |
| `panel` | `#211414` |
| `panelEdge` | `#3d1f1f` |
| `textPrimary` | `#f7ecec` |
| `textSecondary` | `#cfa8a8` |
| `textDisabled` | `#997373` |
| `accentPrimary` | `#ff4545` |
| `accentSecondary` | `#ff8a5f` |
| `accentGlow` | `#ff7a7a` |
| `controlSurface` | `#241717` |
| `controlMark` | `#c95f4f` |
| `activeState` | `#ff4545` |
| `inactiveState` | `#3d1f1f` |
| `lcdBackground` | `#0d0707` |
| `lcdText` | `#ffb0a8` |
| `meterLow` | `#4fae58` |
| `meterMid` | `#e0b83a` |
| `meterHigh` | `#ff4040` |
| `overlayScrim` | `#080404aa` |
| `textureTint` | `#1a0f0f` |
| `focusRing` | `#ff4545` |

### Skin_065 — FROSTBITE

- ID: `skin-065` · family: `light_desk` · layout variant: `LIGHT_DESK`
- Референс-стилистика: Ледяная светлая тема: белый металл и циан.
- Контрасты (G1): text/window **13.81:1**, secondary/window **7.17:1**, disabled/window **4.81:1**, LCD text/LCD **11.42:1**, accent/panel **2.46:1**.

| Токен | Значение |
|---|---|
| `window` | `#e8eef2` |
| `panel` | `#ffffff` |
| `panelEdge` | `#c3d2dc` |
| `textPrimary` | `#16222c` |
| `textSecondary` | `#3b5060` |
| `textDisabled` | `#5d6972` |
| `accentPrimary` | `#00b4d8` |
| `accentSecondary` | `#0090c8` |
| `accentGlow` | `#5fe0ff` |
| `controlSurface` | `#f4f8fa` |
| `controlMark` | `#7fa8c0` |
| `activeState` | `#00b4d8` |
| `inactiveState` | `#c3d2dc` |
| `lcdBackground` | `#0e2a3a` |
| `lcdText` | `#a8ecff` |
| `meterLow` | `#2f9e5a` |
| `meterMid` | `#c8a400` |
| `meterHigh` | `#d84848` |
| `overlayScrim` | `#0c141a55` |
| `textureTint` | `#dce8f0` |
| `focusRing` | `#00b4d8` |

### Skin_066 — SOLAR FORGE

- ID: `skin-066` · family: `forge_metal` · layout variant: `DARK_STAGE`
- Референс-стилистика: Кузница: расплавленное золото на графите.
- Контрасты (G1): text/window **16.30:1**, secondary/window **9.67:1**, disabled/window **4.64:1**, LCD text/LCD **15.40:1**, accent/panel **9.28:1**.

| Токен | Значение |
|---|---|
| `window` | `#171310` |
| `panel` | `#211a14` |
| `panelEdge` | `#3a2c20` |
| `textPrimary` | `#f7f0e4` |
| `textSecondary` | `#cbb9a2` |
| `textDisabled` | `#8d7d6a` |
| `accentPrimary` | `#ffae2e` |
| `accentSecondary` | `#ff6a2e` |
| `accentGlow` | `#ffd066` |
| `controlSurface` | `#241c14` |
| `controlMark` | `#c99a4f` |
| `activeState` | `#ffae2e` |
| `inactiveState` | `#3a2c20` |
| `lcdBackground` | `#0f0a06` |
| `lcdText` | `#ffe0a0` |
| `meterLow` | `#4fae58` |
| `meterMid` | `#e8b400` |
| `meterHigh` | `#ff5030` |
| `overlayScrim` | `#080503aa` |
| `textureTint` | `#1a140e` |
| `focusRing` | `#ffae2e` |

### Skin_067 — TOKYO NEON

- ID: `skin-067` · family: `cyber_city` · layout variant: `DARK_STAGE`
- Референс-стилистика: Кибер-город: бирюза и роза на влажном асфальте.
- Контрасты (G1): text/window **16.89:1**, secondary/window **10.01:1**, disabled/window **4.56:1**, LCD text/LCD **17.05:1**, accent/panel **10.05:1**.

| Токен | Значение |
|---|---|
| `window` | `#0c1418` |
| `panel` | `#122028` |
| `panelEdge` | `#1f3a46` |
| `textPrimary` | `#eaf6fa` |
| `textSecondary` | `#9ec4d4` |
| `textDisabled` | `#5d8395` |
| `accentPrimary` | `#18e0d8` |
| `accentSecondary` | `#ff5fa8` |
| `accentGlow` | `#5ffff0` |
| `controlSurface` | `#152630` |
| `controlMark` | `#3da8b8` |
| `activeState` | `#18e0d8` |
| `inactiveState` | `#1f3a46` |
| `lcdBackground` | `#060d10` |
| `lcdText` | `#a8fff5` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#e0b400` |
| `meterHigh` | `#ff4060` |
| `overlayScrim` | `#04080aaa` |
| `textureTint` | `#0e1a20` |
| `focusRing` | `#18e0d8` |

### Skin_068 — PHOSPHOR TERMINAL

- ID: `skin-068` · family: `retro_crt` · layout variant: `VINTAGE_RACK`
- Референс-стилистика: CRT-терминал: зелёный фосфор и сканлайны.
- Контрасты (G1): text/window **17.76:1**, secondary/window **10.10:1**, disabled/window **4.56:1**, LCD text/LCD **17.38:1**, accent/panel **13.35:1**.

| Токен | Значение |
|---|---|
| `window` | `#080f08` |
| `panel` | `#0f1a0f` |
| `panelEdge` | `#1a2e1a` |
| `textPrimary` | `#d8ffd8` |
| `textSecondary` | `#8fc89f` |
| `textDisabled` | `#5c8462` |
| `accentPrimary` | `#39ff6a` |
| `accentSecondary` | `#88ff44` |
| `accentGlow` | `#7affb0` |
| `controlSurface` | `#0f1a10` |
| `controlMark` | `#4fae5f` |
| `activeState` | `#39ff6a` |
| `inactiveState` | `#1a2e1a` |
| `lcdBackground` | `#040804` |
| `lcdText` | `#b8ffc0` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#d8c040` |
| `meterHigh` | `#ff5a3a` |
| `overlayScrim` | `#030603aa` |
| `textureTint` | `#0a140a` |
| `focusRing` | `#39ff6a` |

### Skin_069 — ARCHITECT MONO

- ID: `skin-069` · family: `minimal_mono` · layout variant: `LIGHT_DESK`
- Референс-стилистика: Инженерный монохром: чертёжная точность.
- Контрасты (G1): text/window **16.46:1**, secondary/window **7.92:1**, disabled/window **4.69:1**, LCD text/LCD **17.00:1**, accent/panel **13.68:1**.

| Токен | Значение |
|---|---|
| `window` | `#f2f2f2` |
| `panel` | `#fbfbfb` |
| `panelEdge` | `#d2d2d2` |
| `textPrimary` | `#141414` |
| `textSecondary` | `#4a4a4a` |
| `textDisabled` | `#6c6c6c` |
| `accentPrimary` | `#2b2b2b` |
| `accentSecondary` | `#6a6a6a` |
| `accentGlow` | `#b0b0b0` |
| `controlSurface` | `#fafafa` |
| `controlMark` | `#b8b8b8` |
| `activeState` | `#141414` |
| `inactiveState` | `#d2d2d2` |
| `lcdBackground` | `#101010` |
| `lcdText` | `#f2f2f2` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#c8a400` |
| `meterHigh` | `#d84040` |
| `overlayScrim` | `#0c0c0c55` |
| `textureTint` | `#e4e4e4` |
| `focusRing` | `#2b2b2b` |

### Skin_070 — VIOLET HAZE

- ID: `skin-070` · family: `cosmic_stage` · layout variant: `STUDIO_CONSOLE`
- Референс-стилистика: Глубокая фиалка и лаванда-свечение.
- Контрасты (G1): text/window **16.36:1**, secondary/window **9.17:1**, disabled/window **4.73:1**, LCD text/LCD **12.85:1**, accent/panel **4.88:1**.

| Токен | Значение |
|---|---|
| `window` | `#14101e` |
| `panel` | `#1e182e` |
| `panelEdge` | `#332a4c` |
| `textPrimary` | `#f2eef8` |
| `textSecondary` | `#bcb0d4` |
| `textDisabled` | `#857aa3` |
| `accentPrimary` | `#9d6bff` |
| `accentSecondary` | `#5f8aff` |
| `accentGlow` | `#c39aff` |
| `controlSurface` | `#221a36` |
| `controlMark` | `#8f7ac9` |
| `activeState` | `#9d6bff` |
| `inactiveState` | `#332a4c` |
| `lcdBackground` | `#0c0814` |
| `lcdText` | `#d8c8ff` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#e0b83a` |
| `meterHigh` | `#e05080` |
| `overlayScrim` | `#060408aa` |
| `textureTint` | `#171224` |
| `focusRing` | `#9d6bff` |

### Skin_071 — CHROME ANVIL

- ID: `skin-071` · family: `industrial_metal` · layout variant: `STUDIO_CONSOLE`
- Референс-стилистика: Шлифованная сталь и индустриальный блеск.
- Контрасты (G1): text/window **15.17:1**, secondary/window **8.92:1**, disabled/window **4.67:1**, LCD text/LCD **15.41:1**, accent/panel **5.89:1**.

| Токен | Значение |
|---|---|
| `window` | `#1c1e20` |
| `panel` | `#26292c` |
| `panelEdge` | `#3f444a` |
| `textPrimary` | `#f2f4f6` |
| `textSecondary` | `#b8bec4` |
| `textDisabled` | `#82888e` |
| `accentPrimary` | `#8fa8b8` |
| `accentSecondary` | `#c8d2d8` |
| `accentGlow` | `#cfe4ee` |
| `controlSurface` | `#222528` |
| `controlMark` | `#8fa0ac` |
| `activeState` | `#8fa8b8` |
| `inactiveState` | `#3f444a` |
| `lcdBackground` | `#0f1112` |
| `lcdText` | `#dceaf2` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#d8b83a` |
| `meterHigh` | `#e05540` |
| `overlayScrim` | `#080909aa` |
| `textureTint` | `#1e2124` |
| `focusRing` | `#8fa8b8` |

### Skin_072 — OCEAN SIGNAL

- ID: `skin-072` · family: `deep_ocean` · layout variant: `STUDIO_CONSOLE`
- Референс-стилистика: Глубина океана: тёмная бирюза и коралловый маркер.
- Контрасты (G1): text/window **16.84:1**, secondary/window **9.80:1**, disabled/window **4.69:1**, LCD text/LCD **17.02:1**, accent/panel **8.95:1**.

| Токен | Значение |
|---|---|
| `window` | `#0a1418` |
| `panel` | `#10201f` |
| `panelEdge` | `#1e3a38` |
| `textPrimary` | `#eaf6f2` |
| `textSecondary` | `#9cc4bc` |
| `textDisabled` | `#618780` |
| `accentPrimary` | `#1fd4b8` |
| `accentSecondary` | `#3da8c8` |
| `accentGlow` | `#7affdd` |
| `controlSurface` | `#132422` |
| `controlMark` | `#3da89a` |
| `activeState` | `#1fd4b8` |
| `inactiveState` | `#1e3a38` |
| `lcdBackground` | `#060d0c` |
| `lcdText` | `#a8fff0` |
| `meterLow` | `#3fae5a` |
| `meterMid` | `#e0b400` |
| `meterHigh` | `#ff6a4f` |
| `overlayScrim` | `#040807aa` |
| `textureTint` | `#0e1a18` |
| `focusRing` | `#1fd4b8` |

## 3. Соответствие референсам известных VST

| Тема | Референс | Что взято |
|---|---|---|
| FLEX PULSE | Nexus 3 / Flex | светлая панель, холодный циан, мягкие скругления |
| SESSION WOOD | NI Session Guitarist | тёплое дерево, кремовый текст, янтарный акцент |
| PRO GRADIENT | FabFilter | сдержанный минимализм, один акцент, крупные значения |
| OMNI NEBULA / VIOLET HAZE | Omnisphere | глубокий фиолетовый фон и мягкое свечение |
| SERUM VOLTAGE | Serum | угольный фон, кислотная зелень, агрессивный глоу |
| VITAL FLOW | Vital | маджента-бирюзовый градиент, живые подсветки |
| TRAKTOR DECK | Traktor | инженерный серый, неоновый оранж |
| MPC GOLD | Akai MPC | матовый чёрный, пад-сетка, золото |

Референсы — по стилистике, без копирования чужих интерфейсов и товарных знаков: ни логотипов, ни шрифтов, ни графики оригиналов.
