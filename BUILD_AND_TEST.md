# Build and test

The public-release branch builds **Test159**, a v0.28 candidate that prepares artwork, fonts and audio on the player's PC during setup. The accepted personal build and installation are separate. No game launch or real installation is part of a build.

## Build the local-resource player installer

Builders need Windows 10/11, Python 3.12, MSVC x86 Build Tools and Windows SDK, and Inno Setup 7.1.0. Players need none of those tools. Windows N needs the Media Feature Pack. Use the original supported Windows Steam game with `data.pack` and the supported English `local_en.pack`. The engine SHA-256 is:

    022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5

From PowerShell in this source folder, replace these example paths:

    $game = 'D:\Steam Library\steamapps\common\Total War SHOGUN 2'
    $work = 'D:\Shogun Mod Build'
    $vc = 'C:\BuildTools\VC\Auxiliary\Build\vcvars32.bat'
    $iscc = 'C:\Tools\Inno Setup 7\ISCC.exe'
    py -3.12 tools/build.py all --local-assets --tests --candidate 159 --game $game --output-dir "$work\native" --vcvars $vc
    py -3.12 tools/installer/build_local_setup159.py --build "$work\native" --vcvars $vc --iscc $iscc --output "$work\setup"

Every output folder must be new and outside the game. The build snapshots source and records hashes/logs. Local bundles in the build's `bin`, `tests` and `local-assets` folders are private test inputs; **do not distribute those folders**. The setup builder audits PE resources and packages only the loader/marker, native extraction tool, integration helper and notices. The result is `output/Shogun2-First-Person-v0.28-LocalAssets-Setup.exe`.

Run `py -3.12 -m unittest discover -s tests -v` and the generated role/table checks as well. Native checks include the 96 adapter, 50 QTE, 8 HUD, 2 loader and 2 current installer executables, plus native resource format and both production renderer checks. Existing installer child-kill tests cover 15 boundaries, not physical power loss. Optional historical archives remain separate; absence is reported as skipped.

## Test the standard setup safely

Build a second wrapper with `--fixture`. It uses a different HKCU uninstall identity and requires an isolated profile override. Then run:

    py -3.12 tools/installer/verify_local_setup159.py --build "$work\setup-fixture" --engine "$game\Empire.Retail.dll" --game $game --assets "$work\native\local-assets" --output "$work\setup-tests"

This hard-links legitimate pack/engine inputs for read-only fixture use, never executes the game, and exercises six-target backup, repair, rollback, unknown-edit preservation and real Inno install/uninstall. Production setup is not executed by these tests. Real installation and gameplay need separate authorization and a fresh loaded-build/capture check.

## Metadata and older build profiles

The native extractor uses checked-in entry hashes and atlas coordinates, with no asset bytes. To regenerate these coordinates, first use the independent [resource preparation pipeline](docs/public-release/RESOURCES.md), then run `tools/installer/generate_asset_recipe159.py --prepared-source <prepared tree>`. Review metadata changes and compare pixels before accepting them.

The older `--resources` embedded profile, `build_setup.py --private-preview` and custom setup/CLI remain historical developer tools. They are not the local-resource player distribution. Their resource-containing binaries must stay private. See [standard setup](docs/public-release/STANDARD_SETUP.md) and [release procedure](docs/public-release/RELEASING.md).
