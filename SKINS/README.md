# PPGU 5.0 — Premium SKINS library

Ten selectable studio colour themes for the POP PUNK GUITAR ULTIMATE shell.

1. NEON PINK — approved signature dark graphite / hot magenta.
2. MIDNIGHT BLUE — deep navy studio.
3. BLOOD RED — stage red.
4. VIOLET — afterhours purple.
5. CYBER GREEN — neon green.
6. AMBER — vintage amplifier amber.
7. ARCTIC — cool cyan.
8. GRAPHITE — monochrome black label.
9. SUNSET — warm tour edition.
10. GOLD EDITION — restrained premium gold.

## Loading
Press **SKINS** in the plugin → **LOAD SKIN JSON...** → choose a JSON file from this folder. The selected theme path is stored with the plugin instance. Keep this folder in place after loading. The two approved hero artworks remain embedded and independent of the colour themes.

## Format
JSON schema: `ppgu.skin.v1`. Required ARGB fields: `background`, `panel`, `panel2`, `accent`, `text`, `muted`. Each colour is eight hex digits in JUCE ARGB order, e.g. `ff111116`. Themes can optionally include `melodyArtwork` and `patternArtwork` paths relative to the JSON file. The two approved styles are assigned to the first two themes and are included under `Resources/Artwork/`. The files theme the existing interface components; they do not alter sound or DSP.

The repository currently contains ten JSON theme definitions and two approved artwork assets. The earlier collection of approximately 60 full visual skin designs was not present in the available repository/library snapshot, so it is not represented as 60 recovered designs.
