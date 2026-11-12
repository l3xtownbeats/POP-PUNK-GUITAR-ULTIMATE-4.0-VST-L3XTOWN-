# PPGU REV 17 — Theme visual identity and control parity contract

Status: USER-APPROVED PRODUCT DECISION / IMPLEMENTATION CONTRACT
Branch: POP-PUNK-GUITAR-ULTIMATE-VST-REV-17
Date: 2026-11-12

## 1. Decision

A theme is a complete visual identity, not a colour preset. Switching themes must change the visual character of the shell through coordinated materials, panel treatment, control construction, borders, lighting, typography and presentation layout. Every theme must retain mandatory control parity and functional meaning.

The approved 1402x1122 reference remains the main-shell composition source of truth. Theme-specific layouts may rearrange the presentation of existing controls only within the declared layout contract; they may not remove, hide without an accessible route, rename, repurpose or falsify controls.

## 2. Visual identity dimensions

Every theme manifest must describe or resolve the following visual dimensions:

- **Material language:** e.g. brushed aluminium, powder-coated metal, carbon composite, painted stage metal, wood/tape/vintage surfaces. Material treatment is implemented through reusable procedural drawing and/or licensed theme assets.
- **Panel construction:** base surface, inset/elevated panels, seams, bevels, corner radius, edge treatment, depth and shadow.
- **Control construction:** rotary face, cap, indicator/pointer, ring, tick marks, toggle/button body, pressed/hover/disabled states and focus treatment.
- **Frame and trim:** outer chassis, inner bezel, separators, borders, screws/fasteners where appropriate, highlight and lowlight.
- **Lighting:** accent glow, LED/LCD treatment and restrained state feedback. No fake activity or fabricated meter readings.
- **Typography and hierarchy:** title, section label, control label, value display and status text.
- **Presentation layout:** approved layout family and responsive placement rules for the same canonical controls.
- **Artwork:** optional theme hero/texture/crest assets, loaded only from a validated theme root.

A palette token alone must never be treated as proof that these dimensions have been implemented.

## 3. Mandatory control parity

The canonical controls remain present and semantically identical in every theme:

- preset_browser, mode_selector;
- knob01..knob09 in the canonical order: GAIN, TREBLE, MID, BASS, VELOCITY, DYNAMICS, SWING, HUMANIZE, ACCENT;
- fx_switch01..fx_switch07 in the canonical order: COMPRESSOR, EQ, DISTORTION, CHORUS, DELAY, REVERB, CABINET IR;
- midi_keyboard, LCD_display, LED_indicator, SKIN_button, Import_Guitar, Auto_Discover, Curator, Settings, About, sync_selector.

The mapping contract does not manufacture functionality. Controls whose runtime feature is not implemented must remain visibly unavailable/disabled with a truthful explanation. No decorative mock control may be presented as active.

A layout may reposition controls, but it must preserve:
1. every canonical ID;
2. each control's existing action and APVTS attachment;
3. keyboard accessibility and readable labels;
4. minimum hit-target and accessibility dimensions;
5. access to controls in compact/overflow layouts.

Theme code must not modify APVTS IDs/values, presets, MIDI, DSP, sampler, scheduler, processor state or audio-thread behavior.

## 4. Layout families

The following layout families are presentation templates, not separate product modes:

- STUDIO_CONSOLE — broad studio-console arrangement, main reference composition.
- WORKSTATION_TOPBAR — compact horizontal topbar with grouped work areas.
- RACK_SIDE — vertical rack-inspired organization with side navigation.
- COMPACT_MINI — condensed responsive layout; all controls remain reachable by scroll, paging or explicit panel navigation.

A family can alter grouping, order and visual placement only. It cannot change control meaning or parameter mapping. Layout changes require deterministic bounds tests at supported window sizes and DPI scales.

## 5. Rendering architecture

Introduce a typed theme style model alongside the existing 21 colour tokens. At minimum it must represent:

- material family and surface finish;
- panel shape/depth/edge profile;
- rotary style and indicator style;
- button/toggle style;
- frame/bezel treatment;
- lighting/glow policy;
- typography scale/weight;
- layout family and spacing density.

The editor LookAndFeel must render from this model and tokens. Remove hardcoded visual colours and one-off decorative styles from the editor incrementally. Reusable drawing primitives must be shared so that theme variants remain consistent and testable.

Legacy manifests remain loadable through deterministic compatibility defaults. Missing optional style fields must not break old themes. Invalid manifests/assets must fall back safely to DEFAULT and report the reason.

## 6. Required validation

- Manifest/schema: valid style identifiers, supported layout family, safe relative asset paths and schema compatibility.
- Control parity: exact canonical ID set; runtime audit against actual components and attachments.
- Visual identity: each style dimension resolves to an implemented renderer; no palette-only false pass.
- Layout: all controls reachable, no overlap/clipping, readable at supported sizes and DPI.
- Theme isolation: no APVTS/preset/MIDI/audio state changes during theme switch.
- Hot swap: UI-only transition; no file I/O, allocations, locks or waits in audio callback.
- Regression: legacy themes continue to load; malformed themes restore DEFAULT.

## 7. Implementation sequence

1. Extend ThemeManager with typed style descriptors and backward-compatible defaults.
2. Route rotary, buttons, toggles, combo boxes, panels, frame, keyboard and labels through themed LookAndFeel/drawing primitives.
3. Introduce layout-family resolver while keeping functional component IDs and attachments fixed.
4. Add runtime parity tests and layout geometry tests.
5. Migrate theme manifests and previews to describe real style differences.
6. Run CTest, Windows x64 Release build, pluginval strictness 5 and separate FL Studio 20 visual/functional acceptance.

## 8. Acceptance rule

A theme is not COMPLETE merely because its JSON loads or its colours change. It is COMPLETE only when its material/panel/control/frame identity is visibly distinct, its declared layout is applied, every mandatory control retains parity, and automated plus host-side checks pass.
