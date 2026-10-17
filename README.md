# Shogun 2 First-Person Possession

A native C++ mod that lets you possess an existing soldier in Total War: SHOGUN 2 while the battlefield, other soldiers and unit AI continue to run in the game.

**Development source: Test99, imported 17 October 2026.** The accepted release remains **v0.14 checkpoint79**. Test99's wall climbing, crest and landing received positive user feedback during extended play; that session subsequently crashed. The native effects-table failure is documented in [known issues](docs/KNOWN_ISSUES.md). This source import is not a new accepted release or a player download.

The repository contains the maintained adapter, styled QTE, HUD, integrated loader, setup wizard, regression fixtures and automatic crash-recorder source. Production C/C++/header files are imported byte for byte from the preserved candidate. The existing relative component layout is retained so native include relationships and numeric protocol versions remain intact.

## Controls

| Input | Action |
| --- | --- |
| F9 | Possess a member of the selected live unit |
| F10 / Escape | Return to the saved RTS camera |
| WASD / Shift | Move / run using native movement and fatigue |
| Mouse | Look |
| RMB then LMB | Ready/draw, then fire/release the personal weapon |
| 1 / 2 | Select melee / ranged equipment |
| Tab | Cycle available native ammunition |
| Hold Ctrl | Supported and unlocked crouched firing |
| F5 / F6 / F7 | Halt / advance / follow |
| F near a climbable wall | Enter native wall climbing |
| W / S / neither while climbing | Ascend / descend / hold; native crest animation at the top |
| F12 | Disable the mod |

HUD morale and fatigue describe the unit. Personal equipment/ammunition and QTE state describe the possessed soldier. Mounted and specialty-unit coverage has known limits; see [status](docs/STATUS.md).

## Build and development

Use Windows, Python 3.10+ and the x86 MSVC C++ toolchain with Windows SDK. Read [BUILD_AND_TEST.md](BUILD_AND_TEST.md) for local UI inputs, exact retail-engine fixture requirements and offline commands. Building does not install the mod or launch the game.

```powershell
py tools/build.py adapter --tests
py tools/build.py all --tests
```

Outputs go to the ignored `artifacts/` folder. A fresh build has its own hashes and is not the exact installed Test99 payload. Installation and live testing remain separate operations.

Original game executables/DLLs, extracted fonts/artwork, compiler/Python runtimes, shared decoding databases, recordings and crash dumps are local dependencies or evidence; they are not uploaded. Local game UI inputs are listed by hash in [tools/local-assets.json](tools/local-assets.json). MinHook's source and license are included; see [third-party notices](THIRD_PARTY_NOTICES.md).

See [SOURCE_MAP.md](SOURCE_MAP.md), [VALIDATION.md](VALIDATION.md) and the [beginner Git guide](docs/GIT_GUIDE.md). Development starts in this repository; preserved releases, older candidate roots and crash evidence remain immutable local references.
