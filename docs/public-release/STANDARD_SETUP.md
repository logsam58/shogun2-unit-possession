# Standard Windows setup

This checkout prepares the unpublished v0.32 / Test163 installer repair with Inno Setup 7.1.0. The published private release is still v0.31. Setup is unsigned. See [startup design](STARTUP163.md) for the current development flow and [v0.31 validation](V0_31_VALIDATION.json) for the immutable published build.

## Installation

Close Shogun 2, run setup normally from the Windows account used to play, and select the original Steam game folder. Players do not run a script, paste a command, select an account or edit Steam properties. Setup connects the compiled how-to and camera chooser automatically. If Steam is running, setup requests its normal shutdown and reopens it afterward as the original unelevated user. It never force-terminates Steam. A client that does not finish normal shutdown causes setup to stop before configuration or file deployment. Steam initially closed stays closed.

Steam Play then opens the guide; choosing First-Person or Third-Person starts the original Creative Assembly launcher/game. Closing the guide cancels. No popup runs inside the game, and game executables are not replaced. Setup edits only Shogun 2's LaunchOptions for the resolved account, preserving ordinary game arguments and unrelated configuration. It retains a narrow ownership/rollback record. Unknown wrappers or edits are preserved and stop setup.

The native account resolver handles a sole account even without MostRecent, a unique explicit recent account, a unique newest timestamp, and an unambiguous automatic-login fallback. Ambiguous or corrupt records are preserved. Setup reads artwork, unit cards, ability icons, fonts and voice from the owned game into fresh temporary staging; those resources are not in the download. Windows N needs the Media Feature Pack.

Inno owns the wizard, file deployment and Windows uninstall entry. The integration helper manages six game/profile targets: DINPUT8.dll, marker pack, two activation files and two local HUD/QTE data files. All before/after snapshots are hash-checked before deployment. Unknown files or snapshots stop recovery. Earlier rollback snapshots survive upgrade; this candidate uses rollback-v0.32-test163. Exact known Test158/Test159, former v1.0 and v0.29–v0.32 predecessors are recognized. This does not claim physical power-loss testing.

Exit Shogun 2 and Steam before using Windows Settings > Apps > Shogun 2 Unit Possession > Uninstall. The owned launch option is restored automatically. Only unchanged mod/local-resource files are removed; later unrelated mods/config entries survive. Unknown replacements stop removal. Old runtime caches and rollback snapshots remain. Saves, graphics, CPU, recorder and debugger settings are not changed. Setup does not launch the game.

## Build and verify

See [BUILD_AND_TEST.md](../../BUILD_AND_TEST.md). The build snapshots source and records native/retained checks separately. The standard packager audits PE resource closure and ships code/notices only. Its synthetic wrapper has a distinct HKCU uninstall identity and marked fixture Steam directory. The actual Inno engine tests installation, repair, uninstall and recovery against isolated game-input views without executing the game. A separately compiled toy Steam client verifies normal shutdown/reopen and timeout behavior without touching real Steam. The production administrator wrapper is not executed by the fixture suite.

Retired embedded-resource downloads remain deleted. The local-resource standard setup is the sole player build route.
