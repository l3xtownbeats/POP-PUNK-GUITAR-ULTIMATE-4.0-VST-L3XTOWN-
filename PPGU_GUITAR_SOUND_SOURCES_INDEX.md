# PPGU — Guitar sound download source index (NATURAL SAMPLES ONLY)

Policy: NATURAL_SAMPLES_ONLY (dated correction 2026-11-13). No synthesized guitar audio is shipped.
Factory bank ships 0 samples; the bank is populated exclusively through **Import Guitar**
(single files or whole libraries; WAV / FLAC / OGG / AIFF / MP3), which converts to WAV 44.1/48 kHz,
computes SHA-256 provenance and distributes results into MELODY and PATTERN/PUNK GROUP mappings.

## Verified sources with permissive licenses

| Source | Content | License | Access |
|---|---|---|---|
| FreePats (FSBS Clean / Distorted Electric Guitar) | Natural electric guitar multisamples, SFZ | CC0 1.0 | freepats.opensrc.se |
| Karoryfer free guitar libraries | Recorded guitar sample libraries | CC0 | karoryfer.com |
| Freesound (e.g. Ax_Grinder "Electric Guitar Power Chords", Jackson Warrior + POD XT) | Real recorded guitar | per-file CC (verify each) | freesound.org |
| Signature Sounds | Recorded WAV one-shots | CC0 | github.com |
| Looperman | User-uploaded guitar loops | per-file (verify each) | looperman.com |

## Import gate (REV 17.0)
- SNR >= 40 dB, no clipping, DC offset <= 0.0001, integrity PASS.
- Guitar-only gate: guitarConfirmed == true required.
- SHA-256 recorded; license evidence stored per asset.
- GREEN: AUTO ADD. YELLOW: manual review only. RED: never overridable. GOLD: human listening required.