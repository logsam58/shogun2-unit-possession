# Build and test

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

VERSION defines the release. Prepare complete player release notes and a validation receipt under docs/releases/. Build/test gameplay components as needed, then build/test the release loader and installer. Keep component hashes and whether checks were freshly run or retained explicit.

Review/stage source and run `py tools/audit_repository.py`, then commit it. `py tools/package_release.py` packages the clean pinned VERSION source/build, verifies versions, embedded resources and receipts, and emits installer/ZIP/manifest/checksums under ignored artifacts/releases/v<VERSION>. Its target check uses the maintenance CLI's read-only `--check-only` operation.

When publication is requested, create an annotated tag matching VERSION on the reviewed commit and push commit/tag. When publication is explicitly requested, `py tools/github_release.py --publish` verifies authentication/repository/tag, uploads a draft, downloads and hashes each asset, and publishes the regular release. The GitHub publication tool is currently scoped to v0.15 and this repository; v0.16 publication requires a deliberate update. Local packaging does not publish. Credentials stay in memory. Repository visibility and installed game files are not changed. Published versions are not overwritten.


## Unpublished development candidate

Build adapter/HUD and their fixtures normally. To produce a distinct development loader, run `py tools/build.py loader --candidate 104 --tests`, then `py tools/build.py installer --candidate 104 --tests`. The marker identifies Test104 and generates a different cache identity. VERSION is 0.16 for this local build; it does not publish, install or replace v0.15 assets. Installer regression additionally requires exact published-v0.15 loader/marker fixtures in ignored `native/work/autoload-v015-baseline/`, extracted as data from verified release setup resources 201 and 202. Never execute the old installer to obtain those fixtures.

Test102 installer regression also needs the exact installed-Test101 loader/marker in ignored `native/work/autoload-test101-baseline/`. Preserve those before rebuilding reused loader output paths. The suite covers its upgrade, all seven interruption stages and repeat-install rollback retention.

Test103 additionally requires exact installed-Test102 loader/marker fixtures in ignored `native/work/autoload-test102-baseline/`. `py tools/package_release.py --development-package` keeps a numbered local candidate under its own version/Test directory, preserving earlier local version packages. It does not publish.

Test104 additionally requires exact installed-Test103 loader/marker fixtures in ignored `native/work/autoload-test103-baseline/`. The final installer suite exercises that exact upgrade, seven interrupted rollback stages and repeat-install retention.
