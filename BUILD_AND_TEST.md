# Build and test

v0.32 uses the accepted **Test166 Workshop runtime**. Directory suffixes such as
45, 99, 159 and 165 are historical implementation names, not product versions.
A build never installs, launches the game, edits Steam settings or publishes.
Use the [developer guide](docs/README.md) for the reading path and the
[tool guide](tools/README.md) to distinguish current commands from operator and
historical helpers.

## Build the Workshop pack

Developers need Windows 10/11, Python 3.12, MSVC x86 Build Tools with the Windows
SDK, and an x86 Lua 5.1 interpreter/compiler configured with float numbers.
The Lua chunk header must be `1b4c75615100010404040400`; the usual double-number
compiler is incompatible. Players need none of these development tools.

Use an owned original Windows Steam installation, including data.pack and the
English local_en.pack. The supported Empire.Retail.dll SHA-256 is:

    022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5

In PowerShell, replace the example paths and use new output directories:

    $game = 'D:\Steam Library\steamapps\common\Total War SHOGUN 2'
    $work = 'D:\Shogun Mod Build'
    $vc = 'C:\BuildTools\VC\Auxiliary\Build\vcvars32.bat'
    $lua = 'C:\Tools\Lua51Float\lua51.exe'
    $luac = 'C:\Tools\Lua51Float\luac51.exe'
    py -3.12 tools/build.py workshop --tests --candidate 166 --game $game --output-dir "$work\native" --vcvars $vc
    py -3.12 tools/workshop/build_workshop165.py --candidate 166 --game $game --components "$work\native\bin" --output "$work\workshop" --vcvars $vc --lua $lua --luac $luac

The distributable folder is `workshop/package`. Its filename retains
`shogun2_unit_possession_workshop165.pack` to update the existing Workshop item.
Keep the rest of the build private: fixtures contain recovered game scripts
and locally generated resource bundles. Never upload complete build folders.
The Workshop target excludes the legacy loader, camera chooser and installer.

The native bootstrap recovers the original UI script and prepares required
artwork/audio locally. It does not modify Steam settings. See the [bootstrap
design](docs/public-release/WORKSHOP165.md) and [storage changes](docs/public-release/WORKSHOP166.md).

## Checks

    py -3.12 -m unittest discover -s tests -v
    py -3.12 tools/generate_roles.py --check
    py -3.12 tools/generate_tables.py --check
    py -3.12 tools/generate_native_inventory.py --check

These Python and metadata checks need no installed game or native compiler.
The inventory is a source index; after relevant native source changes, regenerate
it with `py -3.12 tools/generate_native_inventory.py` and review its diff. Do not
regenerate factual tables merely to make a regression pass.

The accepted component build passed 98 adapter, 50 QTE and 8 HUD executables,
plus local-resource format/renderer checks. Separate pack fixtures cover actual
float-Lua/loadlib startup and all 37 stock callbacks, native payload closure,
resource sharing and copy fallback, conservative staging recovery, foreign-file
preservation, repeated process startup without growth and receipt deletion on
normal/forced exit. Only owned synthetic processes are terminated by fixtures.

These results are retained Test166 evidence, not a fresh gameplay rebuild for
the v0.32 label. See [validation](VALIDATION.md). Python checks and source/pack
integrity checks are rerun for this release. Native changes require appropriate
new native checks and separate live acceptance; fixtures cannot establish all
unit/map combinations, stability or real display/input latency.

## Resources and source export

The extractor stores entry hashes and atlas coordinates, not game asset bytes.
It reads each player's owned game and prepares the HUD/QTE resources there.
The independent [resource pipeline](docs/public-release/RESOURCES.md) can be used
for comparisons and metadata regeneration. Do not distribute generated .s2fp
bundles, fonts, art, audio or recovered stock scripts.

    py -3.12 tools/package_source.py --output-dir "$work\source-export"

The source kit contains the Workshop builder and native sources, a file/hash
manifest, and no compiled payloads or game resources. Optional diagnostic
source is included for developers; those tools are not in the player pack and
are not started by building or playing.

## Older setup builds

Legacy Inno setup and DInput-loader sources remain for historical compatibility
and transaction tests. They are not the v0.32 player route. Inno Setup is not a
Workshop build dependency. Do not install the old loader alongside Workshop.
Older versioned validation documents describe those earlier builds only.
