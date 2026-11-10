# PPGU 4.0 — Theme Packs

This directory is the **built-in theme pack source**. Each theme is an independent JSON token pack; visual assets are stored inside that theme's `assets/` subdirectory.

## Runtime behavior
- The editor exposes one **THEME** button. Clicking it opens a compact theme picker; choosing a pack applies it immediately without restarting the plugin.
- The selected theme ID is persisted in plugin state. If a selected pack is missing or invalid, fall back to `black-pink`.
- Theme changes affect presentation only. They must never change audio, MIDI, preset, sampler, or transport state.
- Theme loading and image decoding must happen off the audio thread. Never access files, allocate memory, or lock from the audio callback.
- Built-in packs ship with the plugin. Optional user packs are discovered in:
  `%APPDATA%/L3XTOWN/POP PUNK GUITAR/Themes/`
- User packs may override a built-in pack only when the user explicitly installs them; do not silently overwrite factory themes.

## Pack format
Each folder contains:
- `theme.json` — stable ID, display name, version, color tokens and asset references.
- `assets/` — optional high-resolution PNG/WebP/SVG artwork, backgrounds and control textures.

Use vector SVG for scalable logos/icons and raster artwork at 2x/3x target size (sRGB, lossless PNG or high-quality WebP). Avoid baking labels or controls into background images: text and controls must remain crisp, accessible and responsive. Provide @2x/@3x assets where raster is required.

## Included starter packs
- `black-pink` — Black Pink
- `cyber-cyan` — Cyber Cyan
- `amber-rock` — Amber Rock
- `blood-red` — Blood Red
- `purple-neon` — Purple Neon
- `ice-silver` — Ice Silver
- `toxic-green` — Toxic Green
- `sunset` — Sunset
- `classic-studio` — Classic Studio
- `pop-punk-riot` — Pop Punk Riot

These JSON packs establish the theme system and palette tokens. They are not a claim that final high-resolution artwork has already been authored or that runtime integration is complete.
