# Releasing

main is the canonical branch. v0.29 promotes the accepted local-resource gameplay and retires the separate public-release branch. The owner authorized the v0.29 release inside the private repository, with a verified full-history backup before branch consolidation. Repository visibility remains an independent owner decision.

Build the standard local-resource setup using [BUILD_AND_TEST.md](../../BUILD_AND_TEST.md). Complete native/Python/resource checks appropriate to changes and isolated Inno install/uninstall checks. Audit PE resources: no game images/fonts/audio may be shipped. Do not distribute generated .s2fp files or complete build directories. Documentation-only updates reuse the appropriate verified setup.

Review the final source, commit it, create the curated source ZIP from that clean commit, audit its hash/CRC/file closure and links, then tag the same commit. Upload the versioned Setup.exe, Source.zip and SHA256SUMS.txt; independently download and verify them. Keep published versions immutable. Retire superseded draft assets only with verified backups and explicit authorization.

v0.29 reuses exact accepted Test159 adapter/HUD/QTE/extractor binaries with retained native evidence; loader and setup tests are fresh. Never describe retained tests as freshly executed. Keep user acceptance separate from formal preplay verification. Test159's formal gate was missed. A future real swap needs fresh CLOSED and rollback; a new live process needs fresh READY/IN BATTLE and exact loaded/native-hook/actual-capture verification.

Charging and castle wall climbing are intermittent known bugs. Do not remove warnings based on offline tests or broad acceptance. Crashes/Ultra/POV/acquisition remain open.

The owner authorized deleting the 24 older installer/bundled-installer downloads before public launch. They were backed up and hash-verified; old sources, metadata, release bodies and tags are retained. Current setup excludes game images/fonts/audio. Repository visibility remains a separate owner decision.

The [download removal record](LEGACY_DOWNLOAD_CLEANUP.json) lists the exact retired assets.
