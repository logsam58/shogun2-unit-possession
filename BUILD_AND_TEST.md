## v0.25 / accepted Test140

Promote exact Test140 gameplay with charging marked buggy. Release metadata uses revision140; retain adapter80/QTE47/HUD6 and run fresh loader2, complete installer upgrades/rollback and Python7. See docs/releases/v0.25-technical.md.

## Test140 charge contact candidate

Use explicit `--candidate 140`: adapter80, QTE47, loader2, the complete installer suite including exact Test139 plus prior upgrades/seven interruptions/repeat backups, and Python7. Retain exact HUD6. Install with the game closed and preserve exact Test139 rollback alongside accepted Test138. Fresh loaded hashes and recording must be checked after READY before formal play.

## Test139 charging candidate

Use explicit `--candidate 139`. Run adapter80, loader2, the complete installer suite including exact installed Test138 and all prior predecessors/seven interruptions/repeat backups, and Python7. Retain exact accepted HUD6/QTE46. Close the game for replacement, preserve exact Test138 rollback and require a fresh loaded-build/capture cue afterward. See docs/development/Test139.md for original-code/provider boundaries. v0.24/Test138 remains the accepted release baseline until gameplay feedback.

## v0.24 release baseline

The release retains exact accepted Test138 gameplay. `--release` now labels revision138 as v0.24 with no development suffix. Run loader2, the complete installer suite (exact Test138 plus all earlier predecessors, seven interruptions/repeat backups), and Python7. Retain adapter79/HUD6/QTE46 rather than rebuilding gameplay. Pin all tracked native/tool inputs and verify embedded payloads/PE/marker before packaging. See docs/releases/v0.24-technical.md and its receipt. Candidate sections below are historical build records.

## Test138 / v0.24 locomotion rollback

Use explicit --candidate 138: adapter79, exact retained HUD6/QTE46, loader2, full installer including exact Test137 and all prior upgrades/seven interruptions/repeat backups, Python7. Verify VERSION/PE/marker0.24 together. Preserve exact137 rollback and existing armed recorder; require closed-game replacement and fresh loaded/capture cue afterward.

## Test137 personal melee presentation

Use explicit --candidate 137: adapter79, retained exact HUD6/QTE46, loader2 and complete installer including exact Test136 upgrades/all7 interruptions/repeat backup, plus Python7. New fixtures execute original state/pose/turn and missing-squad read blocks with documented provider limits. Keep the existing recorder armed; install only under the closed-game guard.

## Test136 Rapid Shift ceiling

Use explicit --candidate 136: adapter77, loader2, full installer including exact Test135 upgrades/all7 interruptions/repeat-backup retention, and Python7. Retain exact HUD6/QTE46. verify_rapid136 exercises the production scoped cap with synthetic native peers; verify_rapid132 retains original effect/hash/stat coverage. Keep automatic recorder armed; installation requires the closed-game guard.

## Test135 Rapid gait correction

Use explicit --candidate 135: adapter76, loader2 and the complete installer suite including exact Test134 through seven interruptions/repeated backup retention. Retain exact Test134 HUD6 and QTE46 evidence/binaries. Expanded verify_rapid132 executes original18c120 and18b310; standalone fatigue provider covers selected native gait. Ordinary recorder remains armed; game/session must be closed for installation.

# Build and test

## Test134 native mode toggle

Use explicit `--candidate 134`: adapter76, HUD6, loader2 and complete installer suite, including exact installed Test133 with seven interruptions/repeat backup. Run Python7 and retain exact QTE. Keep the existing recorder armed with no game/session during replacement. See docs/development/Test134.md.

## Test133 correction

Use explicit `--candidate 133` for adapter76, HUD6, loader2 and complete installer regression, including exact Test132,131,130 and published v0.23 predecessors with all seven interruptions and repeat backups. Retain exact QTE. Keep the existing watcher armed, with no active game/session during replacement; do not stop ordinary recording. See docs/development/Test133.md.

## Test132 revision

Use explicit `--candidate 132`. Run adapter76, loader2 and the complete installer suite including the exact installed Test131 predecessor, all seven interruptions and repeat-install backup retention. Retain exact HUD/QTE. See docs/development/Test132.md; diagnostics stay stopped through deployment and are then re-armed as requested.


## Test131 development checkpoint

Use explicit `--candidate 131` for adapter, loader and installer builds. Retain the exact accepted Test130 HUD/QTE; they are not rebuilt for this checkpoint. New adapter fixtures compare the production speed reader with original native18b310, exercise bounded mouse/body turning, filter overlapping native Q hits, and serialize/acknowledge the native whole-unit melee-mode order.

The complete installer suite additionally needs the published v0.23 loader/marker in ignored `native/work/autoload-v023-baseline/`, extracted as data from verified v0.23 setup resources201/202. Exact Test130 and prior fixtures remain required. Both predecessors must complete all seven interrupted rollback stages and repeat-install retention before installation. Keep diagnostics off. See [Test131](docs/development/Test131.md) for this checkpoint and the remaining charging/ability/bow work.

## Requirements

- Windows, Python 3.10+ and Visual Studio 2022 / Build Tools with MSVC x86 C++ tools and the Windows SDK.
- The supported retail SHOGUN 2 engine for exact-native fixtures. Empire.Retail.dll SHA256: `022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5`.
- For styled QTE/HUD: 24 separately prepared UI inputs matching tools/local-assets.json. They are not bundled with source.
- For the full installer upgrade/rollback suite: separately retained exact predecessor loader/marker/runtime fixture files.

A game SDK is not required. The adapter can compile without the UI closure. A fresh clone cannot build the complete styled runtime until those local resources are supplied; the repository does not currently include a standalone extraction pipeline. See [local dependencies](docs/LOCAL_DEPENDENCIES.md).

The build wrapper discovers a registered Visual Studio installation using vswhere. Alternatively set SHOGUN2_VCVARS to vcvars32.bat or configure it below. Native fixtures currently contain the standard Steam engine path `C:\Program Files (x86)\Steam\steamapps\common\Total War SHOGUN 2\Empire.Retail.dll`; using another location requires adjusting those fixture paths. Setting a game path in local configuration does not rewrite C++ fixture literals.

## Configure inputs

Run from the repository root:

```powershell
py tools/configure.py --vcvars 'C:\path\to\VC\Auxiliary\Build\vcvars32.bat'
py tools/configure.py --asset-source 'C:\path\to\prepared-resource-tree'
py tools/configure.py --legacy-fixtures 'C:\path\to\historical-fixture-tree'
```

The UI and fixture source roots must match the relative input structure checked by configure.py. UI hashes are published in tools/local-assets.json. Historical fixture files are used only by installer regression. Settings go into ignored tools/local.json; inputs are copied into ignored locations. Configuration does not install or launch the game.

## Build commands

```powershell
py tools/build.py adapter --tests
py tools/build.py qte --tests
py tools/build.py hud --tests
py tools/build.py loader --tests
py tools/build.py installer --tests
# Or build all components in dependency order:
py tools/build.py all --tests
py -m unittest discover -s tests -v
```

Adapter, QTE and HUD must exist in artifacts/bin before building the loader. Build the loader before the installer. Omitting `--tests` compiles the selected component without its offline fixture suite. `all` builds each component in order. The x86 release build uses MSVC C++17, optimization and the static C runtime.

Outputs and compiler logs go to artifacts/bin, artifacts/obj, artifacts/tests and artifacts/logs. artifacts/build-results.json records checks actually executed. Read [VALIDATION.md](VALIDATION.md) for suite coverage and retained release evidence. The setup wizard and maintenance CLI are compiled but never installed by build.py. Installer fixtures operate in temporary workspace directories and an isolated HKCU test key.

## Installation and live checks

Do not run setup as an offline fixture. For a gameplay change, preserve an exact rollback, close SHOGUN 2, install the candidate and start a fresh normal Steam process. Verify loaded paths/hashes, engine compatibility and capture before asking for formal play. A newly compiled file can have different hashes even with unchanged source; backing-file equality and runtime-patch state are different facts.

Existing automatic recording must continue between sessions and start on launch without waiting for pause, READY, battle or mod availability. Recorder installation/configuration is separate; see tools/diagnostics/README.md. The build tools neither launch the game nor attach a recorder.

## Release process

VERSION defines the release. Update the numeric and string Windows version fields in `native/work/installer-siege99/setup.rc` alongside it; package verification rejects mismatches. Prepare complete player release notes and a validation receipt under docs/releases/. Build/test gameplay components as needed, then build/test the release loader and installer. Keep component hashes and whether checks were freshly run or retained explicit.

Review/stage source and run `py tools/audit_repository.py`, then commit it. `py tools/package_release.py` packages the clean pinned VERSION source/build, verifies versions, embedded resources and receipts, and emits installer/ZIP/manifest/checksums under ignored artifacts/releases/v<VERSION>. Its target check uses the maintenance CLI's read-only `--check-only` operation.

When publication is requested, create an annotated tag matching VERSION on the reviewed commit and push commit/tag. When publication is explicitly requested, `py tools/github_release.py --publish` verifies authentication/repository/tag, uploads a draft, downloads and hashes each asset, and publishes the regular release. The GitHub publication tool uses VERSION and remains scoped to this repository. It verifies the tagged source, exact asset list and checksums, then downloads every uploaded asset before publishing. Later releases also require a source archive whose Git blobs match the tagged commit; already published releases are never overwritten. Local packaging does not publish. Credentials stay in memory. Repository visibility and installed game files are not changed. Published versions are not overwritten.


## Unpublished development candidate

Build adapter/HUD and their fixtures normally. To produce a distinct development loader, run `py tools/build.py loader --candidate 104 --tests`, then `py tools/build.py installer --candidate 104 --tests`. The marker identifies Test104 and generates a different cache identity. VERSION defines its product version independently from the Test number; building does not publish, install or replace v0.15 assets. Installer regression additionally requires exact published-v0.15 loader/marker fixtures in ignored `native/work/autoload-v015-baseline/`, extracted as data from verified release setup resources 201 and 202. Never execute the old installer to obtain those fixtures.

Test102 installer regression also needs the exact installed-Test101 loader/marker in ignored `native/work/autoload-test101-baseline/`. Preserve those before rebuilding reused loader output paths. The suite covers its upgrade, all seven interruption stages and repeat-install rollback retention.

Test103 additionally requires exact installed-Test102 loader/marker fixtures in ignored `native/work/autoload-test102-baseline/`. `py tools/package_release.py --development-package` keeps a numbered local candidate under its own version/Test directory, preserving earlier local version packages. It does not publish.

Test104 additionally requires exact installed-Test103 loader/marker fixtures in ignored `native/work/autoload-test103-baseline/`. The final installer suite exercises that exact upgrade, seven interrupted rollback stages and repeat-install retention.

## Historical v0.17 release metadata

For the historical Test105 audit, build with `--candidate 105`. After pinning its full source and validation receipt, package with `py tools/package_release.py --development-package --candidate 105`. This reads `docs/development/Test105-validation.json` and writes its own `artifacts/releases/v0.17-Test105/` folder; it does not rewrite the published v0.17 validation receipt or downloads.

The historical v0.17 promotion retained the exact accepted Test104 adapter, HUD and QTE. Run `py tools/build.py loader --release --tests`, then `py tools/build.py installer --release --tests`. At that commit, the release flag removed the development label while retaining implementation revision104; it is mutually exclusive with `--candidate`. Neither command installs, launches or publishes.

Installer regression additionally needs the exact accepted Test104 loader/marker in ignored `native/work/autoload-test104-baseline/`. Those fixtures exercise the actual upgrade, all seven interrupted rollbacks and repeat-install backup retention. After reviewing/committing source, run `py tools/package_release.py` to produce `artifacts/releases/v0.17/`. For publication, add the exact committed source ZIP and `SOURCE_SHA256SUMS.txt` to the pinned upload manifest; the publisher verifies every archive member against HEAD before uploading. The version/marker/PE and embedded payload hashes are checked; current target validation is read-only.


## Test106 presentation development

Build with `--candidate 106` and use `docs/development/Test106-validation.json` plus `py tools/package_release.py --development-package --candidate 106`. Adapter tests now link Windows SDK d3d11/dxgi/d3d9/d3dcompiler libraries and include private actual-device compositor tests. ID3D11DeviceContext1 support is checked at runtime; unsupported initialization leaves the existing Windows fallback available. These tests neither launch the game nor change desktop display mode. Exact Test105 loader/marker fixtures in ignored `native/work/autoload-test105-baseline/` are required by the final installer predecessor/rollback tests. Preserve the installed eight-file rollback and recorder baseline independently. Published v0.17 packages/receipt are not replaced.


## Test107 QTE timing development

Retain the exact verified Test106 adapter/HUD binaries and source inputs. Build/test QTE, then loader and installer with `--candidate 107`; run the complete changed QTE suite. The final installer additionally requires exact Test106 predecessor loader/marker fixtures in ignored `native/work/autoload-test106-baseline/`. Pin a development receipt that separates fresh QTE/loader/installer checks from retained Test106 adapter/HUD evidence. Package with `py tools/package_release.py --development-package --candidate 107`; published v0.17 assets/receipt are not replaced. The running game remains untouched until closed-game installation.

## v0.18 release metadata

Retain the exact accepted Test107 adapter, HUD and QTE, and their gameplay source inputs. Run `py tools/build.py loader --release --tests`, then `py tools/build.py installer --release --tests`. The release flag removes the development label and retains implementation revision107. At the v0.18 commit the default development candidate was107; use an explicit later `--candidate` for new work. These commands never install, launch or publish.

The complete installer suite additionally requires exact Test107 loader/marker fixtures in ignored `native/work/autoload-test107-baseline/` and the published v0.17 loader/marker in `native/work/autoload-v017-baseline/`. Extract old setup resources201/202 as data only; do not execute an old installer. The suite verifies both exact upgrades, seven interrupted stages each and repeat-install retention. Keep the historical fixture closure as well. A failed fixture remains recorded; diagnose and rerun the full final payload before pinning a passing receipt.

After review/audit and a clean commit, `py tools/package_release.py` produces `artifacts/releases/v0.18/`. Add a source ZIP whose every member matches the committed Git blob, its `SOURCE_SHA256SUMS.txt`, and both records to the six-file upload manifest. Explicitly requested publication uses the annotated v0.18 tag on that reviewed commit and downloads/hashes every uploaded asset before making it public to repository readers. Existing release assets and repository visibility remain unchanged.


## Test109 HUD and friendly-fire development

Run `tools/prepare_audio109.py --game <local game> --ffmpeg <ffmpeg.exe>` to prepare the separately verified private voice input. Build/test adapter and HUD using explicit `--candidate 109`; retain the exact Test108 QTE and source inputs. Then build/test loader and installer with candidate109. The current default is109. Test109 adds a passive native impact observer and out-of-process Windows audio playback, with native-ABI and notification/PCM fixtures. Use its own development receipt/package; published v0.18 remains immutable.

## v0.19 release metadata

Retain exact accepted Test109 adapter/HUD/QTE binaries and all their native output source inputs. Build/test only loader and installer with `--release`; it removes the development label while retaining implementation revision109. The default development candidate is109. Prepare exact installed-Test109 loader/marker fixtures in `native/work/autoload-test109-baseline/` and published-v0.18 fixtures in `native/work/autoload-v018-baseline/`, extracting setup resources201/202 as data only. Retain historical fixtures. The complete final installer suite covers both new exact upgrades, seven interruption stages each and repeat-install rollback retention.

Pin docs/releases/v0.19-validation.json with fresh and retained evidence, audit/stage/commit source, then run `py tools/package_release.py`. Add a source ZIP matching every committed Git blob and its checksum. Local packaging does not publish/install/launch. Published v0.18 and earlier assets stay exact; the installed Test109 and recorder baseline109 remain unchanged until an explicitly authorized replacement.

## v0.20 release metadata

The default development revision and `--release` implementation revision are115. Retain exact Test115 adapter/HUD/QTE binaries and all native output source inputs; build/test only the release loader and installer. In addition to historical inputs, prepare exact Test115 loader/marker fixtures in `native/work/autoload-test115-baseline/` and local v0.19 fixtures in `native/work/autoload-v019-baseline/`. Extract old setup resources201/202 as data only. The full installer suite checks both upgrades, seven interrupted stages each and repeat-install retention.

Pin `docs/releases/v0.20-validation.json`, audit staged source, commit, and package with `py tools/package_release.py`. Add an exact committed source ZIP/checksum to the six-file upload manifest. Explicit publication uses an annotated v0.20 tag, a normal fast-forward push, and `py tools/github_release.py --publish`, which downloads/hashes all uploaded assets before publishing. Existing v0.18 and earlier published downloads and the local v0.19 archive remain unchanged. Packaging/publication does not install, launch, replace the recorder or prove an intermittent slowdown/crash fix.

## v0.21 release

Default implementation revision is122. Retain the exact accepted Test122 adapter/HUD/QTE binaries, then build loader and installer using `--release --tests`. The complete native source matches the tested Test122 closure. Test116 clock and Test117 render-copy experiments are excluded. Prepare exact Test122 and published-v0.20 loader/marker data in `native/work/autoload-test122-baseline/` and `native/work/autoload-v020-baseline/`; never execute an older installer to extract them. Upgrade fixtures cover seven interruption stages and repeat-install backup retention. Follow the release process above with v0.21 notes/receipt and source archive. Packaging does not install or launch the game.

## v0.22 release

Default implementation revision is124. Retain exact accepted Test124 adapter/HUD/QTE binaries and their native source; build/test only the release loader and installer with `--release --tests`. Prepare exact Test124 and published-v0.21 loader/marker fixtures in `native/work/autoload-test124-baseline/` and `native/work/autoload-v021-baseline/`. Extract predecessor installer resources201/202 as data only. Full installer regression covers both upgrades, all seven interruptions and repeated-install backup retention, alongside historical fixtures. Pin fresh release results separately from retained Test123 adapter62/Test124 QTE46/Test122 HUD5 evidence. Follow the release process with v0.22 notes/receipt and an exact committed source ZIP. Publication does not install or change game preferences, affinity or recording.

## v0.23 release

Default revision is130. Retain exact accepted Test130 adapter/HUD/QTE and native output source; build/test only loader and installer with `--release --tests`. Exact Test130 and published-v0.22 data fixtures are in ignored `native/work/autoload-test130-baseline/` and `native/work/autoload-v022-baseline/`. Full upgrade tests cover seven interruptions and repeat-install retention. Pin all native/tool inputs, separating retained adapter71/QTE46/HUD5 evidence from fresh packaging. Packaging and committing do not replace Test130, start diagnostics, launch the game or publish a GitHub release.
