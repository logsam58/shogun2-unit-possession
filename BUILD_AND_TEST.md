# Build and test

This public-release branch builds unpublished Test158. Accepted v0.28/Test157 and the installed mod stay unchanged. See [dependencies](docs/LOCAL_DEPENDENCIES.md), [resources](docs/public-release/RESOURCES.md) and [current status](docs/current-release.json).

Use Windows x64, Python 3.12.10, MSVC x86/Windows SDK, ImageMagick 7.1.2-24 Q16-HDRI, Node 24.16.0, locked npm dependencies and FFmpeg 9.0.2. The legitimate game's Empire.Retail.dll must have SHA-256:

    022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5

Other images/platforms/remasters are unsupported. Never bypass fingerprint failures.

## Clone or extract, then build

Run in PowerShell at the source root. Replace these example paths. Output directories must be new; spaces are supported. Keep paths within normal Windows file API limits. Setup refuses UNC/reparse installation paths.

    $game = 'D:\Steam Library\steamapps\common\Total War SHOGUN 2'
    $work = 'D:\Shogun Mod Build'
    $vc = 'C:\BuildTools\VC\Auxiliary\Build\vcvars32.bat'
    $node = 'C:\Tools\node\node.exe'
    $magick = 'C:\Program Files\ImageMagick-7.1.2-Q16-HDRI\magick.exe'
    $ffmpeg = 'C:\Tools\ffmpeg\bin\ffmpeg.exe'
    & $node 'C:\Tools\node\node_modules\npm\bin\npm-cli.js' ci --prefix tools/assets --ignore-scripts --no-audit --no-fund
    py -3.12 tools/prepare_resources.py --game $game --output-dir "$work\resources" --magick $magick --node $node --packer tools/assets/node_modules/free-tex-packer-cli/index.js --ffmpeg $ffmpeg
    py -3.12 tools/generate_roles.py --check
    py -3.12 tools/generate_tables.py --check
    py -3.12 -m unittest discover -s tests -v
    py -3.12 tools/build.py all --tests --candidate 158 --game $game --resources "$work\resources" --output-dir "$work\native" --vcvars $vc

Optional tools/configure.py accepts --game, --resources and --vcvars, writing ignored tools/local.json. Explicit arguments override it. Automatic MSVC discovery uses Visual Studio Installer's vswhere.exe.

The builder snapshots source into the chosen output, overlays locally prepared resources and builds adapter → QTE → HUD → loader → installer. Inputs, logs and artifact hashes are recorded. It never runs the player installer or game. Adapter-only builds can omit UI; complete integration uses all.

Current checks: 96 adapter, 50 QTE, 8 HUD, 2 loader fixtures and 2 installer executables. The transaction executable kills only its own children at 15 boundaries. This tests process termination, not physical power loss. Exact-native instructions use the fingerprinted engine; synthetic world/render/pathfinding providers remain finite boundaries.

Historical installer compatibility is optional and explicitly skipped without its verified archive. Missing current sources/resources/tests is fatal. See [historical fixtures](docs/public-release/HISTORICAL_TESTS.md). The optional recorder has its own [isolated checks](tools/diagnostics/README.md).

## Install, upgrade, repair and rollback

These are separate operator actions requiring approval. Close the game before DLL replacement, then launch a fresh process through Steam. Run the locally built bin/Shogun2-Possession-Setup.exe and choose the supported game. It refuses unrelated same-name files.

Repair uses the same installer. Recovery restores an uncommitted transaction's five managed targets and two fixed Steam prerequisite values before retrying; a committed transaction completes backup retention. Unknown changes stop recovery and remain intact. Preserve unfamiliar journals for inspection rather than deleting them.

Changed existing targets retain .possession-previous snapshots; unfamiliar older backups get preserved under identified suffixes. Prefer a reviewed earlier installer for deliberate rollback. Keep journals/backups until acceptance. This branch has no automatic uninstall/rollback CLI. Do not overwrite another mod's DINPUT8.dll or restore an entire profile.

Locally built binaries embed game resources and must not be publicly uploaded while permission is unresolved. The approved public format is a source/local-resource kit. See [release procedure](docs/public-release/RELEASING.md).
