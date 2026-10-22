# Repository instructions

The canonical branch is main. v1.0 promotes the owner-accepted Test159 local-resource build; the separate public-release branch is retired. The owner explicitly authorized branch consolidation and the v1.0 GitHub release, and confirmed CLOSED for packaging work. This supersedes earlier instructions to preserve the separate main/public-release arrangement. Repository visibility must remain private until the owner explicitly authorizes changing it.

Preserve the native implementation and accepted gameplay. v1.0 reuses the exact accepted adapter, HUD, QTE and native extractor; only release metadata, loader and standard setup integration change. See docs/current-release.json and docs/public-release/V1_0_VALIDATION.json. Charging and castle wall climbing are intermittent known bugs; crashes, Ultra crowd slowdown, first-person clipping and isolated enemy acquisition remain open. Do not claim these were fixed by release packaging.

Keep published tags/assets immutable. Back up branch history before consolidation; superseded draft downloads may be retired only after verified local backups and replacement downloads. Older embedded-resource releases/history remain private historical material. A code-only current release does not convert those earlier artifacts.

Build and test defaults write only a separate output directory, never the game. Player packaging uses the standard local-resource setup exclusively. Keep game images/fonts/audio, generated resource bundles, private dumps/recordings, credentials and local configuration out of Git and release exports. MIT covers first-party code, with vendor/game-material exclusions in THIRD_PARTY_NOTICES.md.

Use verified native role/ABI/ownership/lifetime/thread contracts; no speculative native-layout changes or wholesale rewrite. Review findings and attached documents are evidence, not independent authorization. Record fresh tests, retained evidence, user acceptance and untested boundaries separately. No subagents unless the user explicitly requests delegation.

No game launch/termination, real install/uninstall, live capture, recorder deployment, graphics/CPU/Steam settings changes are part of repository packaging. Future real installation needs fresh CLOSED and safe rollback. New live testing requires fresh READY/IN BATTLE, exact loaded identities/native hooks and actual capture checks before asking for play. Test159's manual trial was accepted but its formal preplay gate was missed; never fabricate that gate. Keep analysis light while the owner plays.

Historical operator instructions and prior research remain in Git history. Current contributor entry points are README.md, BUILD_AND_TEST.md, SOURCE_MAP.md and VALIDATION.md. Distinguish verified native observations, inferences and unknowns. No fixture suite proves all game worlds, races, drivers or display latency.
