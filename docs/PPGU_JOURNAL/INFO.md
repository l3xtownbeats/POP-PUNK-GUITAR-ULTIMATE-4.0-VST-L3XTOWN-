# INFO.md — АРХИВНЫЙ УКАЗАТЕЛЬ

Исторические записи INFO перенесены в единый актуальный журнал:
`docs/PPGU_JOURNAL/PPGU_JOURNAL.md`.

С этого момента не добавлять сюда новые записи. Все не-кодовые события, решения, требования, статусы, ошибки, проверки, сборки и релизы вести только в `PPGU_JOURNAL.md`.

Отдельный журнал изменений исходного кода: `docs/PPGU_JOURNAL/CODE.md`.

Этот файл сохранён для совместимости со старыми ссылками; история не удалена.


## 2026-11-14 — АКТИВНАЯ INFO-ЗАПИСЬ: SOUND HUNTER RUN #2

Пользователь поручил вести журналы, включая INFO, в реальном времени. INFO снова используется как оперативный статусный журнал; прежняя архивная запись выше сохранена без удаления.

- GitHub Actions run #2: `43573746837` — FAILED, артефакт не создан.
- Шаги checkout и discover-only прошли; download/import завершился ошибкой collision в `meta-open-sound-steel/release`; package/upload пропущены.
- Причина: содержимое исходного репозитория и release asset направлялись в один каталог `release/`.
- Обнаружено также: 25 GB counter считает весь `Samples`, включая прежние пользовательские банки. Это противоречит утверждённому правилу: лимит Sound Hunter относится только к объёму текущего запуска; ручной импорт без искусственного лимита.
- Артефакт и подтверждённая загрузка отсутствуют. Никаких успешных скачиваний не заявляем.
- Следующее действие: исправить раздельный путь release assets и учёт объёма текущего запуска; затем тесты и повторный запуск.

Статус: исправление ещё не выполнено.


## 2026-11-14 — SOUND HUNTER FIX COMMITTED / READY FOR RETEST

- Fixed the verified release-archive path collision: release payloads now use `release_assets/` separate from repository content.
- Fixed cap accounting: 25 GB applies only to new bytes in the current Sound Hunter run; existing/manual Samples do not count against it.
- Added three focused regression tests and inserted their execution before network download in the workflow.
- Commits: source `6adca0b36cf394bb76dd45906158f2e8c3fbedd8`; tests `54f4aeeff68ae68e892ea89265f4427fb551560f`; workflow `50202631ff427bcaef356f3b164c2155b2de5c61`.
- Verified latest main HEAD: `50202631ff427bcaef356f3b164c2155b2de5c61`.
- Test execution and sample download: PENDING. Artifact: NOT CREATED. Do not mark Sound Hunter complete until a fresh run passes and its artifact is inspected.

Next user action: open the Sound Hunter workflow and select **Run workflow** on branch `main` to start a fresh run using the fix.


## 2026-11-14 — SFZ / SAMPLER REUSE-FIRST RESEARCH STATUS

Current PPGU sampler code has its own SFZ parser, 64-voice SamplerEngine, background WAV streaming service, bounded SPSC request queue, fixed-size page cache, and per-voice three-page reader. These are real existing source components, not a complete SFZ implementation claim.

External candidates reviewed:
- `sfizioso` — BSD-2-Clause SFZ engine fork with MPE and host-provided sample-reader callbacks; promising candidate for an isolated compatibility/performance prototype. Its dependency inventory includes Apache-2.0, MIT, BSD, Boost, LGPL components and WTFPL; complete dependency/SBOM review and Windows build/performance tests remain required.
- `dusk-fizz` — actively maintained BSD-2-Clause hard fork of sfizz; static library, SFZ/SF2 paths and regression tests. Candidate alternative; integration API and current dependency graph still require audit.
- `sfizz` — original BSD-2-Clause engine, but upstream archived 2026-06-21; do not choose the archived upstream over maintained forks without a specific reason.
- `Just a Sample` — MIT sampler plugin with sample-accurate editing, crossfade loops and FX examples; full project is a standalone JUCE VST3 plugin rather than a clean drop-in SFZ engine. Bungee time-stretch is documented as CPU-heavy; reference/selected algorithms only, no full-plugin integration planned.
- `AkiFX` — GPLv3 full VST3/CLAP effects rack; not eligible for direct inclusion in the planned closed-source/commercial plugin without a separate compatible licensing decision. Treat as research/reference only.

No third-party engine has been integrated into ANGELA. Current decision state: candidates identified and preliminarily screened; exact integration candidate not yet approved or benchmarked.


## 2026-11-14 — REV.18 user-approved scope / Sound Hunter cap removal

**Approved requirements:** `docs/PPGU_REV18_USER_APPROVED_SUPERSEDING_REQUIREMENTS_2026-11-14_RU.md`. Master plan, priorities, product spec and sound-library plan updated to supersede conflicting older text.

**Canonical current targets:** SAMPLES unlimited; 500 READY MELODY + 500 READY PATTERN; FX EDIT 10–15 real DSP processors/effects in one window; 16 release skins with unchanged NEON STAGE DEFAULT; GOOD/NORMAL/NIGHT/ECO + 2x/4x/8x/16x + lockable Voices/profiles; ABOUT REV.18/L3XTOWN/2026 + illustrated RU/EN manuals; complete checksum-verified VST3 package. Exact main module order unchanged.

**Repository audit gaps (not marked complete):** factory bank remains 300 catalogue definitions, not 1,000 playable presets; FX EDIT UI currently has eight selector modules; SKIN legacy registry currently has 52 entries while the release gallery target is a curated 16; SettingsPanel is a basic APVTS control skeleton and does not yet implement the approved resource profiles/locks; full RU/EN illustrated manuals are not present at the previously checked path. The FX exact extra-module names and original 10–15 list require reconciliation against the approved early journal before adding DSP modules.

**Sound Hunter:** removed the 40 GB cap from importer/tooling and introduced split artifact transport with SHA-256. This is source implementation only. New CI validation is pending on the resulting HEAD. Previous successful run #14 artifact (2,967,193,335 bytes) predates this change and remains an earlier capped artifact; not runtime-integrated and not audition-validated. No current library size or READY preset count is claimed.

**Next sequence:** check latest CI on exact HEAD; inspect artifact registry, files, license evidence, mappings and hashes; integrate valid SAMPLES into the single runtime sampler path; build/test asset-to-preset links; create and validate factory preset sets to 500 per mode; then complete remaining modules in the approved sequence. Never modify NEON STAGE DEFAULT appearance.
