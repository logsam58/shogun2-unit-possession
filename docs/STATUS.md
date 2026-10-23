# Status

Unpublished Test163/v0.32 repairs automatic startup setup, including account files without MostRecent and normal Steam close/reopen during installation. Players install and select their game folder; Steam Play then shows the compiled how-to/perspective chooser before starting the game. Game-process friendly-fire audio is retained. Thirty standard installer groups and the separate native checks pass; no real installation or live acceptance is claimed. See [candidate validation](public-release/TEST163_VALIDATION.json). The latest published release remains v0.31.

v0.31 / Test161 adds a quick-start controls guide and moves the camera chooser before the original game starts. Setup provides a one-time Steam Launch Options line. Steam Play then opens the guide; either camera button starts the game, while closing cancels. The loader no longer opens a popup over the fullscreen game.

The repository remains private and main is the only branch. The owner requested this release. This update has offline validation only; it has not been installed or live-tested on the owner's game. File readback confirms the installed v0.30/Test160 loader and marker; the owner reported a positive manual trial, but a complete loaded-hook/capture gate was not performed.

Support remains single-player land battles and sieges. Naval possession does not work and is planned for later. Multiplayer is not implemented; Versus and campaign drop-in are under consideration. Visual/gameplay bugs and crashes are expected. Attack charging remains partly broken and unreliable; climbing is intermittent. Ultra slowdown, clipping and isolated acquisition remain open.

See [current identity](current-release.json), [known issues](KNOWN_ISSUES.md), [startup design](public-release/STARTUP161.md) and [v0.31 validation](public-release/V0_31_VALIDATION.json). Earlier downloads remain unchanged, and retired embedded-resource installers have not been re-uploaded.
