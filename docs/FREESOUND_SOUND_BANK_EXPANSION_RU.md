# Freesound sound-bank expansion workflow

The PPGU sound bank already has its CC0 baseline libraries (Meta Open Sound Steel/Nylon and FreePats FSBS Clean/Distortion variants). Freesound is an additional discovery source, not a replacement for those multisample banks.

## What is eligible
- The importer accepts only the exact Freesound API license label **Creative Commons 0**.
- Attribution, NonCommercial, ShareAlike, custom or missing licenses are not auto-imported into the distributable factory bank.
- A CC0 result is still a candidate: listen to it, inspect the recording, confirm it is an isolated playable source, verify tuning/noise/clipping, record provenance and checksum, then map it in SFZ. A one-shot or riff loop must not be presented as a chromatic multisample.
- Do not ingest copyrighted song excerpts, artist/song stems, commercial sample-pack content, or recordings with unclear provenance.

## API setup
1. Sign in to Freesound and request an APIv2 credential at https://freesound.org/apiv2/apply.
2. Store the API key in the environment variable `FREESOUND_API_KEY`; never commit it or paste it into source code.
3. Search and create a metadata-only candidate manifest:
   ```powershell
   $env:FREESOUND_API_KEY = "YOUR_API_KEY"
   python tools/freesound_ingest.py --output Samples/Freesound-CC0/manifest.json
   ```
4. Original-quality downloads require OAuth2. Register the application and complete the Freesound OAuth2 authorization-code + PKCE flow. Set the short-lived bearer token in `FREESOUND_ACCESS_TOKEN`, then run:
   ```powershell
   python tools/freesound_ingest.py --download --output Samples/Freesound-CC0/manifest.json
   ```
5. The Freedsound API documents original download as OAuth2-only; preview URLs are separate compressed previews and are not used as factory sample masters.

## Runtime / release gate
- Candidate manifest is not a playable bank. Every accepted recording must pass listening, file-integrity, format, sample-rate, tuning, note-root, articulation and license review.
- Build SFZ maps only for assets that form a usable instrument mapping. Keep isolated one-shots and loops in a separate FX/phrase bank.
- Retain source URL, sound ID, creator, exact license label, retrieval date, original file hash and any required attribution alongside every asset.
- Do not add any Freesound candidate to a factory preset until it passes the full library audit and runtime note-on/audio-output tests.
