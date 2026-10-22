# Releasing

The canonical repository is logsam58/shogun2-unit-possession and main is the only maintained branch. The owner authorized this rename and the v0.31 release inside the private repository. Public visibility remains a separate owner decision. Keep old published versions immutable; do not re-upload the retired game-resource-containing downloads.

Build the standard local-resource setup with [BUILD_AND_TEST.md](../../BUILD_AND_TEST.md). Run relevant full native/Python/resource and isolated Inno checks. v0.31 changes the prelaunch chooser, guide, loader startup and setup. Its launch/loader/installer checks are fresh; the exact passing Test161 gameplay binaries and source are retained, with their earlier full suites recorded separately. Record live acceptance separately. Never distribute generated resource bundles or complete build directories.

Commit reviewed source, export the clean source kit, verify CRC/hash/file closure and links, then tag the same commit. Upload versioned Setup.exe, Source.zip and SHA256SUMS.txt and independently download/hash-check all three. Preserve prior tags/releases and repository privacy. Repository rename must preserve its numeric identity, history and source; update remotes and canonical links afterward.

The startup chooser has no game settings side effects. Preserve stable installer AppId, internal ownership/activation identifiers and cache conventions during branding changes. A real swap requires fresh CLOSED and rollback; new live testing requires fresh READY/IN BATTLE plus exact loaded/native-hook/actual recording checks.

Naval possession is unavailable, multiplayer is under consideration, and visual/gameplay bugs/crashes are expected. Do not remove these notices based on offline tests. Asset exclusion does not establish legal clearance for native hooks; see [provenance](PROVENANCE.md).
