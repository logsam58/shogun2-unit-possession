# Build and offline checks

## Requirements

- Windows with Visual Studio 2022 / Build Tools C++ x86 tools and a Windows SDK.
- Python 3.10 or later. No Rust, game SDK or whole-engine rebuild is used.
- The retail SHOGUN 2 installation for exact native fixtures. Expected Empire.Retail.dll SHA256: `022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5`.
- The local UI resource closure for styled QTE/HUD. Game artwork/fonts are omitted from Git; the 24 required files and hashes are recorded in tools/local-assets.json.
- Historical loader/marker files only when running the full installer upgrade/rollback fixture. These remain ignored local test inputs.

The production source and original fixture algorithms are unchanged. Exact native fixture C++ files currently use the standard Steam path `C:\Program Files (x86)\Steam\steamapps\common\Total War SHOGUN 2\Empire.Retail.dll`; another installation location requires updating those fixture paths. Build scripts discover a registered Visual Studio installation or accept SHOGUN2_VCVARS / ignored tools/local.json. This does not make the shared decoding library portable or bundle game resources.

## Configure local inputs

From the repository root, point the helper at an existing local preserved native-mod source tree containing the matching resource closure, such as the original FP99 candidate. The asset manifest verifies hashes before copying. It does not extract from arbitrary game packs or download assets.

```powershell
py tools/configure.py --vcvars 'C:\path\to\VC\Auxiliary\Build\vcvars32.bat' --asset-source 'C:\path\to\preserved-native-mod'
py tools/configure.py --legacy-fixtures 'C:\path\to\preserved-native-mod'
```

Settings are saved to ignored tools/local.json. Local source snapshots and installed recorder remain separate from Git. Configure only copies UI or historical test dependencies into ignored locations; it does not install or launch the game.

## Commands

```powershell
py tools/build.py adapter --tests
py tools/build.py qte --tests
py tools/build.py hud --tests
py tools/build.py all --tests
py -m unittest discover -s tests -v
py tools/audit_repository.py
```

The all target builds adapter, styled QTE, HUD, integrated DInput loader and setup wizard. With --tests it also runs their original offline fixtures. Installer fixtures operate in temporary workspace game/user directories and an isolated HKCU fixture key. Historical loader inputs are required for upgrade and seven-stage rollback cases. The built setup wizard and install_cli are compiled but never invoked by the build tool. No normal game process is attached or started.

Build outputs, compiler logs and fresh receipts are in artifacts/. The adapter suite includes50 executables. Styled QTE fixtures include the actual hidden-window renderer and reusable-surface checks. See VALIDATION.md for the import's actual results rather than assuming all checks ran.

Do not run a freshly built installer as an offline test. Freshly rebuilt hashes/cache IDs differ from the installed candidate, even with unchanged source. Installation needs a separate closed-game step, an exact rollback, then a fresh user Steam launch/READY and loaded-hash/capture checks. The persistent crash watcher continues independently.
