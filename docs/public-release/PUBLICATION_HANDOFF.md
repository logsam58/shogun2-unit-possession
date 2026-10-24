> **Historical record:** This page describes the v0.31 installer release handoff.
> The current release is v0.32 / Test166 Workshop. Use the
> [developer guide](../README.md), [release identity](../current-release.json),
> and [release procedure](RELEASING.md) for current work. Commands, pending
> tasks and approvals below apply only to the recorded session.

# v0.31 release handoff

The owner requested the quick-start schematic and instructions in the camera chooser, followed by a v0.31 commit and release. main remains the only branch and repository visibility stays private. The existing v0.30 release, earlier tags and downloads remain unchanged. A concurrent owner edit to the slowdown notes was preserved.

Current downloads are Shogun2-Unit-Possession-v0.31-Setup.exe, Shogun2-Unit-Possession-v0.31-Source.zip and SHA256SUMS.txt. The standard Inno setup contains code only and prepares game resources from each player's installation. Retired embedded-resource installers must not be re-uploaded.

Setup supplies a one-time Steam launch-option line. It launches the chooser before the game process, displays the control schematic and guide, and passes the selected default through the existing process-local preference. Closing cancels launch. The loader no longer opens a dialog inside the game. Without the helper option, the game starts in first person with no popup. Remove the line before uninstalling.

Fresh guide/prelaunch/loader/installer checks and exact retained gameplay evidence are distinct in [v0.31 validation](V0_31_VALIDATION.json). Standard fixtures cover exact v0.30 rollback and actual Inno upgrade with the old app directory and snapshots preserved. The production wrapper has not been executed on the owner's game. A real swap requires fresh CLOSED and rollback; live testing requires a fresh cue and exact loaded/native-hook/actual recording verification. Recording baseline changes are not part of this release task.

Charging/climbing, crashes, Ultra slowdown and POV/isolated targeting limitations remain open. Naval possession does not work; multiplayer is only under consideration. No legal clearance is inferred from local resource preparation. Owner installation and Steam settings remain unchanged by release preparation.
