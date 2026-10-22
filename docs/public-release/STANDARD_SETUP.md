# Standard Windows setup

This checkout builds **v0.31 / Test161**, the startup-guide correction, using Inno Setup 7.1.0. Its setup adds a small prelaunch chooser and controls guide so the original game process starts only after a camera button is chosen. The installer is unsigned and the repository remains private. Test161 is not installed or live-tested.

## Installation

Close Shogun 2, select the supported original Steam folder, and let setup prepare resources from that copy. It first validates the game/profile, then runs the native extractor into fresh temporary staging. Failure here leaves game files unchanged. No game artwork, fonts or voice are shipped inside the setup or runtime code.

After installation, paste the line shown by setup into **Steam > Shogun 2 > Properties > Launch Options**. It has the form `"<installed mod folder>\shogun2-unit-possession-launcher.exe" %command%`. Keep existing game arguments after `%command%`; another launch wrapper needs separate compatibility review. Setup also writes the exact line to `Steam-Launch-Options.txt` in its app folder. Steam Play then opens the chooser first. Either camera button starts the original game; closing the chooser cancels. No popup runs inside the game. Without the launch option, the game starts normally with the historical first-person default. Steam configuration is edited by the player, not by setup.

Inno owns the wizard, file deployment and Windows uninstall entry. It installs the loader, marker pack and two locally generated data files, with normal program records/notices in its own folder. A narrow integration helper manages only six targets: `DINPUT8.dll`, the marker pack, two activation files and the HUD/QTE local resource files. It uses the retained fingerprint, encoding, JSON and safe-file primitives. The old custom wizard/CLI is not invoked.

All before/after snapshots are identified and hash-checked before game-file deployment. Interrupted prefixes can restore the exact baseline when targets still match known before/after identities. Unknown changes, snapshots and unidentified directories are preserved and stop recovery. This is not a claim of power-loss testing. Repeat install repairs the same identified installation; earlier snapshots remain after removal and reinstall. v0.31 uses version-specific rollback records, preserving preview records on upgrade. Exact known Test158/Test159, v0.29, v0.30 and former v1.0 predecessors are recognized; unknown injectors are still refused.

Remove this mod's Steam launch-option line first, then use Windows Settings > Apps > Shogun 2 Unit Possession > Uninstall with the game closed. Removing the line restores Steam's ordinary launch command. Only unchanged current mod/local-resource files are removed; unrelated mods and later activation entries survive. Unknown replacement files stop uninstall. Rollback snapshots and old runtime caches are retained. Setup does not touch saves, graphics/CPU/Steam settings, recorder or debugger, and never launches the game.

Run setup from the Windows account used to play. It requests administrator access for protected installation folders. The fixture wrapper has a distinct HKCU identity and mandatory synthetic profile, and must not be confused with production setup.

## Build and verify

See [BUILD_AND_TEST.md](../../BUILD_AND_TEST.md). `build_local_setup159.py` accepts a successful isolated local-assets build with the required native and prelaunch checks, audits PE resource closure and packages code/notices only. It deliberately excludes the privately generated `local-assets`, `bin/*.s2fp` and `tests/*.s2fp` inputs. The fixture verification runs the actual Inno engine against isolated read-only game-input views, including preparation on the test computer.

Earlier embedded-resource setups and ZIPs containing them have been deleted from GitHub after private backup/hash verification. The player build now has one standard local-resource route.
