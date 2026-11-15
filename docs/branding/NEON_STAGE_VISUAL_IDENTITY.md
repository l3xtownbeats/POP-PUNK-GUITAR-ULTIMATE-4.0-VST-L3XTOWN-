# NEON STAGE — Visual Identity & Branch Presentation Standard

## Purpose

This document defines the shared GitHub-facing visual identity for POP PUNK GUITAR ULTIMATE (PPGU). It standardizes repository presentation without changing the plugin's runtime architecture, audio behavior, approved controls, theme inventory or release claims.

## Canonical product cover

Use:

`assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg`

The current canonical artwork is a full vector product-cover composition: dark performance-stage background, NEON STAGE title treatment, branded instrument-console illustration, mode selector, performance display, control section and keyboard strip. It is not a blank placeholder.

The image is **illustrative product artwork**, not a screenshot from a running VST3 host. Never label it as a verified runtime capture or use it as evidence that every depicted control is implemented.

## README header standard

Development branches should use the canonical cover at the top of `README.md`:

```html
<div align="center">

<img src="assets/POP_PUNK_GUITAR_ULTIMATE_BANNER.svg"
     alt="POP PUNK GUITAR ULTIMATE — NEON STAGE product cover"
     width="100%">

# POP PUNK GUITAR ULTIMATE
### L3XTOWN AUDIO LAB · NEON STAGE · WINDOWS x64 · VST3

</div>
```

Branch-specific version labels may remain where they are factually correct (for example, a 5.0 development line). Do not make an older revision appear to be a newer release.

## Visual language

- **Base:** deep graphite, near-black and dark steel.
- **Primary light:** electric cyan / blue.
- **Secondary accent:** violet / magenta used sparingly.
- **Surface treatment:** layered instrument panels, subtle grid, controlled glow, clear borders and readable contrast.
- **Typography:** compact technical labels, strong product heading, consistent spacing and restrained uppercase labels.
- **Composition:** product identity first; interface artwork second; concise technical status and navigation below.

## Repository presentation rules

1. Keep the full product cover in the README; do not replace it with an emoji, generic stock image, empty rectangle or text-only placeholder.
2. Preserve approved mockups and theme-preview samples in `docs/design/reference/`.
3. Keep runtime theme resources in their existing code/manifests-referenced locations. Do not move or duplicate runtime assets merely to improve README appearance.
4. Keep branch-specific implementation notes, test evidence and status accurate.
5. Do not describe a mockup, theme preview, preset definition or checklist as a completed runtime feature.
6. Keep product identity consistent across development branches while preserving genuine version distinctions and branch-specific technical content.
7. Do not overwrite an experimental branch's deliberate alternative design during cleanup; document it as an experiment and compare it with the canonical standard first.

## Project constraints that visual work must not change

- Preserve the approved 27-control contract.
- Use only the seven approved FX bypass IDs.
- Do not invent APVTS IDs from mockup labels.
- Do not add interface elements absent from REV.18 Inventory and `Info.txt`.
- Do not create separate SOUND FORGE or SIGNAL FORGE themes.
- Keep NEON STAGE DEFAULT and approved Skin_052 / Skin_053 visual work.
- Keep GUI/theme presentation independent from the audio engine.
- Do not imply unimplemented behavior is functional.

## Branch rollout

The repository contains many independent development branches, including REV.17, REV.18, 5.0, feature, fix, audit, UI and work branches. Apply the shared cover to a branch only after checking its current README, asset path, version identity and divergence. Preserve unique source, tests, theme assets and branch-specific work; never mass-reset or mass-delete branches to achieve visual consistency.
