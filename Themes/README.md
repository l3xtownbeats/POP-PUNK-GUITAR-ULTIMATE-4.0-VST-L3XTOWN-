# PPGU Theme Library

This directory is the runtime theme library for POP PUNK GUITAR ULTIMATE.

- Install the folders from the companion `PPGU_Themes_1.0.zip` archive here.
- Each theme is a separate folder containing `theme.json`.
- Optional PNG/font assets are resolved relative to that folder; missing assets must use procedural JUCE drawing and the default font.
- Themes are presentation-only. They must not modify APVTS audio parameters, preset data, MIDI behavior, or automation.

The complete 30-theme folder pack and its schema documentation are supplied in the downloadable archive. The current repository ThemeManager source is a baseline and still requires the folder-scanning/editor integration before runtime theme switching can be called complete.