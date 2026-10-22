# v0.30 release handoff

The owner authorized renaming the mod and repository to Shogun 2 Unit Possession, adding clear support warnings and the startup view chooser, and freshly confirmed CLOSED for the build. main remains the only branch; repository visibility stays private. Historical v0.29 and earlier releases/tags are preserved.

Current downloads are Shogun2-Unit-Possession-v0.30-Setup.exe, Shogun2-Unit-Possession-v0.30-Source.zip and SHA256SUMS.txt. Resources remain prepared from each owned game. Retired resource-containing installers must not be re-uploaded; see the [deletion record](LEGACY_DOWNLOAD_CLEANUP.json).

Test160 reads the loader's process-local camera choice before adapter initialization and applies it to each new possession lease. P can still toggle within the lease. The chooser runs on the autoload worker, outside DllMain/native callbacks; no persistent settings or protocol layouts change. The setup keeps its existing AppId and ownership identity so updates/uninstall retain old records. See [v0.30 validation](V0_30_VALIDATION.json).

Naval possession does not work and is planned for later. Multiplayer is not implemented; Versus and campaign drop-in are under consideration. Visual/gameplay bugs and occasional crashes are expected. Charging/climbing, crowd slowdown, clipping and isolated acquisition remain open.

The new build is offline-tested only, not installed or live-accepted on the owner's game. Test159 baseline acceptance and its missed formal preplay gate remain historical facts. A future real swap/test needs fresh cues and loaded/native-hook/actual capture checks. No game/recorder/settings/CPU changes are part of this release work.
