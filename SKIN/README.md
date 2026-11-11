# PPGU SKIN packs

The repository contains authored theme manifests and a visual-token contract. These files are not, by themselves, finished graphical skins or proof of runtime theme switching.

## Runtime installation rule

**The installed plugin must use a sibling folder named Skins next to the installed .vst3 bundle.** The primary runtime source is not %APPDATA%, Documents, or a separate system theme directory.

Expected installed layout:

```text
VST3 install directory/
├── POP PUNK GUITAR ULTIMATE.vst3/
└── Skins/
    ├── default/
    │   ├── theme.json
    │   ├── assets/
    │   ├── svg/
    │   └── textures/
    ├── skin-02/
    └── ...
```

The source folder SKIN/ is a development/catalogue folder. Packaging and installation must transform/copy its accepted theme packs into the runtime sibling folder Skins/. The plugin must resolve the installed VST3 bundle path, not use the DAW current working directory. Do not silently substitute AppData as the main theme location.

## Catalogue and current gaps

- The registry and validator define 50 canonical slots (skin-01 through skin-50).
- The current source tree also contains Skin_51 / skin-51, which is outside the schema/registry range and is not an accepted catalogue entry.
- Theme manifests exist, but the specified preview.png, hero.webp and texture.png files are not present in the theme folders. Control artwork, sprite sheets, fonts and SVG layers are also not complete.
- Runtime switching, SKIN button/browser, Ctrl+K, preview, hot-swap, persistence, directory scanning, full token application, LRU texture loading and ECO behavior are not implemented/accepted in the current main editor.
- The current ThemeManager is a partial manifest/palette validator and is not connected to PluginEditor.

See [SKIN Engine gap audit](SKIN_ENGINE_GAP_AUDIT_RU.md) for the source-by-source findings, test gaps and implementation priorities.

## Runtime contract

- The SKIN selector is the only user-facing theme entry point.
- Theme changes are presentation-only: no APVTS parameter, audio engine, preset, MELODY/PATTERN or layout changes.
- Preserve hit areas, control IDs, attachment bindings, focus/hover/pressed/disabled states and scaling.
- Missing/invalid theme data must fall back to the built-in default without interrupting audio; log the issue and mark the incompatible theme in the browser.
- Do not mark a theme accepted until Windows x64 build, automated tests and FL Studio 20 host checks pass.

## Status

**SPECIFIED / PARTIALLY SCAFFOLDED / NOT RUNTIME-INTEGRATED.** Manifests are authored; artwork assets and runtime integration are pending.
