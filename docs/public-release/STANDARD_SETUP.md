# Standard Windows setup

The current build is **v0.29**, using Inno Setup 7.1.0 and the accepted Test159 gameplay. Download `Shogun2-First-Person-v0.29-Setup.exe`. It is unsigned. The owner accepted the preceding local-resource installer; the v0.29 wrapper has fresh offline checks and has not replaced the owner’s installation. The repository remains private until its owner opens access.

## Installation

Close Shogun 2, select the supported original Steam folder, and let setup prepare resources from that copy. It first validates the game/profile, then runs the native extractor into fresh temporary staging. Failure here leaves game files unchanged. No game artwork, fonts or voice are shipped inside the setup or runtime code.

Inno owns the wizard, file deployment and Windows uninstall entry. It installs the loader, marker pack and two locally generated data files, with normal program records/notices in its own folder. A narrow integration helper manages only six targets: `DINPUT8.dll`, the marker pack, two activation files and the HUD/QTE local resource files. It uses the retained fingerprint, encoding, JSON and safe-file primitives. The old custom wizard/CLI is not invoked.

All before/after snapshots are identified and hash-checked before game-file deployment. Interrupted prefixes can restore the exact baseline when targets still match known before/after identities. Unknown changes, snapshots and unidentified directories are preserved and stop recovery. This is not a claim of power-loss testing. Repeat install repairs the same identified installation; earlier snapshots remain after removal and reinstall. v0.29 uses version-specific rollback records, preserving preview records on upgrade. Exact known Test158/Test159 and former v1.0 predecessors are recognized; unknown injectors are still refused.

Use Windows Settings > Apps > Shogun 2 First-Person Mod > Uninstall with the game closed. Only unchanged current mod/local-resource files are removed; unrelated mods and later activation entries survive. Unknown replacement files stop uninstall. Rollback snapshots and old runtime caches are retained. Setup does not touch saves, graphics/CPU/Steam settings, recorder or debugger, and never launches the game.

Run setup from the Windows account used to play. It requests administrator access for protected installation folders. The fixture wrapper has a distinct HKCU identity and mandatory synthetic profile, and must not be confused with production setup.

## Build and verify

See [BUILD_AND_TEST.md](../../BUILD_AND_TEST.md). `build_local_setup159.py` accepts a successful isolated local-assets Test159 build, audits PE resource closure and packages code/notices only. It deliberately excludes the privately generated `local-assets`, `bin/*.s2fp` and `tests/*.s2fp` inputs. The fixture verification runs the actual Inno engine against isolated read-only game-input views, including preparation on the test computer.

Earlier embedded-resource setups and ZIPs containing them have been deleted from GitHub after private backup/hash verification. The player build now has one standard local-resource route.
