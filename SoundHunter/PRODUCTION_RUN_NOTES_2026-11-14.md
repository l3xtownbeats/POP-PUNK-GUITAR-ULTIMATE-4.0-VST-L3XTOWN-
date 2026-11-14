# Sound Hunter production run — 2026-11-14

User-approved execution priority: acquire additional real-recorded guitar sample libraries, verify source licences, preserve mappings and provenance, then connect valid samples to MELODY/PATTERN presets and validate the resulting audio.

The existing `.github/workflows/sound-hunter-real-guitar.yml` workflow is intentionally additive. It must run the importer regression tests, acquire only catalogued REAL_RECORDED sources, verify repository/release licences and SHA-256 manifests, enforce the aggregate 40,000,000,000-byte managed Samples ceiling, preserve every existing asset, and upload the imported result as a workflow artifact. No existing Samples file may be overwritten, deleted, moved, renamed or deduplicated by deletion.

The VST3 build workflow separately downloads the approved Meta Open Sound, FreePats and Karoryfer CC0 source sets into its build workspace. The resulting build artifact is not evidence of runtime integration by itself.

After acquisition, inspect the artifact and registry, audit source mappings and per-file provenance, then create actual preset-to-sample mappings. Only presets with functioning mappings and successful listening/audio validation may be marked READY. A catalog record or successful CI validation alone is not proof of playable guitar sound.

Visual invariant: NEON STAGE DEFAULT retains its existing approved overall appearance and visual concept; UX refinements must stay within that visual identity.
