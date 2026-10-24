# Shogun 2 Unit Possession

The current release is v0.32, using the accepted Test166 Workshop runtime.
Start with README.md, BUILD_AND_TEST.md, SOURCE_MAP.md, VALIDATION.md and
docs/current-release.json. Older versioned documents describe historical builds.
docs/README.md, native/README.md and tools/README.md provide the developer
reading path and distinguish current/shared code from historical helpers.

Preserve native mechanics, supported-image fingerprints, role/ABI/ownership/
lifetime/thread contracts and exact hook-byte checks. Forward unrelated engine
calls. Avoid changing gameplay merely to package a release. Never resume the
retired Rust rebuild or include original engine/decompilation databases.

Workshop is the current player route. It prepares resources from the player's
owned game, starts in third person, and uses P to change perspective. Do not
restore the legacy DInput loader while the Workshop mod is enabled. Keep
existing quarantine, saves, settings and rollback backups intact.

Builds and tests use new isolated output folders. They must not install, launch
the game, edit Steam/game settings, start recording or publish. Keep work light
while the owner plays. Substantial native investigation/builds need a fresh
CLOSED confirmation; real game-file changes also need verified safe rollback.
Live process/memory checks need fresh READY/IN BATTLE and exact loaded hashes,
hooks and process identity. Extra tracing needs separate authorization.
Recorder/debugger remain OFF unless the owner explicitly asks to enable them.
No subagents unless explicitly requested.

The owner accepted Test166 based on a successful manual in-game report. Steam
upload and exact downloaded pack were independently verified. The formal
Test166 loaded-hash/hook gate was not completed; do not invent it, full-battle
coverage, all-feature validation or long-term stability. The timing of that
playtest relative to local publisher-copy cleanup was unspecified. No repeated
playtest is required solely to record the owner's acceptance.

First-party code is MIT; game/vendor material retains its own terms. Ship no
game artwork, fonts, audio, recovered stock scripts, private recordings/dumps,
credentials, personal configuration or build-fixture directories. Audit both
source and packaged executable resources. Necessary local caches are distinct
from logs; do not promise zero storage or remove unknown files automatically.

The canonical repository is logsam58/shogun2-unit-possession; main is the only
maintained branch. Preserve its current private visibility and historical
releases/tags unless explicitly authorized otherwise. Publication requires an
explicit task; keep source, notes and checksums aligned with the tagged build.
Historical release authorizations in notes apply only to their recorded tasks.
Repository cleanup does not authorize a new product release, Workshop upload,
visibility change or game install.
