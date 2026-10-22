# Build and test

main builds **v0.29** using the accepted Test159 gameplay schema and the local-resource player installer. A build never launches or installs into the game. Historical directory suffixes and the number 159 describe implementation formats, not a separate product branch.

## Build the player installer

Builders need Windows 10/11, Python 3.12, MSVC x86 Build Tools with Windows SDK, and Inno Setup 7.1.0. Players need none of those tools. Use the original supported Windows Steam game, data.pack and English local_en.pack. Windows N needs the Media Feature Pack. The supported Empire.Retail.dll SHA-256 is:

    022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5

From PowerShell in this source folder, replace these example paths:

    $game = 'D:\Steam Library\steamapps\common\Total War SHOGUN 2'
    $work = 'D:\Shogun Mod Build'
    $vc = 'C:\BuildTools\VC\Auxiliary\Build\vcvars32.bat'
    $iscc = 'C:\Tools\Inno Setup 7\ISCC.exe'
    py -3.12 tools/build.py all --release --tests --candidate 159 --game $game --output-dir "$work\native" --vcvars $vc
    py -3.12 tools/installer/build_local_setup159.py --build "$work\native" --vcvars $vc --iscc $iscc --fixture --output "$work\setup-fixture"
    py -3.12 tools/installer/verify_local_setup159.py --build "$work\setup-fixture" --engine "$game\Empire.Retail.dll" --game $game --assets "$work\native\local-assets" --output "$work\setup-tests"
    py -3.12 tools/installer/build_local_setup159.py --build "$work\native" --vcvars $vc --iscc $iscc --verified-fixture "$work\setup-tests" --output "$work\setup"

Every output folder must be new and outside the game. The final output is **output/Shogun2-First-Person-v0.29-Setup.exe**. Local resources are always enabled; --local-assets remains a compatibility flag. The --release flag selects product metadata, without installing or publishing anything.

The build snapshots source and records hashes/logs. Generated .s2fp bundles in bin, tests and local-assets are private test inputs; **do not distribute those folders**. The standard setup audits PE resources and packages code, extraction metadata and notices only. Game images, fonts and audio are prepared on each player's computer.

## Checks

    py -3.12 -m unittest discover -s tests -v
    py -3.12 tools/generate_roles.py --check
    py -3.12 tools/generate_tables.py --check

Native --tests checks include 96 adapter, 50 QTE, 8 HUD, 2 loader and 2 current installer executables, plus local-resource format and production renderer checks. The retained installer transaction suite covers 15 actual child kills, not physical power loss. Optional historical archives are separate and absence is reported as skipped.

The standard fixture has a separate HKCU uninstall identity and mandatory synthetic profile. It uses legitimate pack/engine files read-only, never executes the game, and verifies backup, repair, rollback, unknown-file preservation and real Inno install/uninstall. Optional --previous-bin, --previous159-bin, --previous-release-bin and --previous-setup accept retained known preview inputs for upgrade checks. These archives are not prerequisites for a new contributor's mandatory current tests. Production setup is not executed by the suite.

## Resources and source export

The native extractor uses checked-in entry hashes and atlas coordinates, with no asset bytes. The independent [resource pipeline](docs/public-release/RESOURCES.md) is available for reference comparisons and metadata regeneration with tools/installer/generate_asset_recipe159.py. Review metadata changes and compare pixels before accepting them.

    py -3.12 tools/package_source.py --output-dir "$work\source-export"

The export contains Shogun2-First-Person-v0.29-Source.zip, a file/hash manifest and no game resources or compiled payloads. Player builds use the standard local-resource setup exclusively. Legacy native installer primitives remain for fingerprints and transaction tests; the old embedded-resource setup scripts are retired. See [setup](docs/public-release/STANDARD_SETUP.md) and [releasing](docs/public-release/RELEASING.md).
