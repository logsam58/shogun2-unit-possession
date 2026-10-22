# Validation

[v1.0 release checks](docs/public-release/V1_0_VALIDATION.json) distinguish fresh wrapper tests from retained exact accepted gameplay evidence. The v1.0 setup was not installed or live-tested on the owner’s game.

[Build commands](BUILD_AND_TEST.md) and [source-kit engineering summary](docs/public-release/VALIDATION_RESULTS.json) and [local-resource installer validation](docs/public-release/LOCAL_ASSETS159_VALIDATION.json) are current. Raw logs, toy dumps and game resources stay out of the source kit.

HUD tests run production measure() for ten final-human roles and negative identity/liveness/ownership cases. Diagnostic tests run production snapshot logic with synthetic reads and prohibit process access in help smoke tests. Fresh staging includes accuracy_read90.py and actor_roles.py; existing installed recording was not invalidated by the historical deployment omission.

Current installer tests cover install, repeated repair, encoding, collisions, exception rollback and 15 actual child kills followed by recovery. Unknown target/registry edits, legacy stages and journal extras remain intact. A filename alone does not authorize deletion. Optional historical compatibility is explicitly skipped when unavailable, never counted as pass.

Fixtures have finite native/synthetic provider boundaries. Passing them does not prove all driver/world/castle behavior, race freedom, absence of leaks, or physical display latency.

The owner subsequently installed Test159 through the standard setup and accepted the manual trial. Readback matches the installed payload/resources and own activation; existing recorder metadata matches all four runtime backing files. The formal preplay native-hook/capture gate was not completed and is not retrospectively claimed. This acceptance does not establish universal unit/map coverage. No new launch, live read, trace or recorder change was performed while recording that feedback. Future testing still needs a fresh cue and exact loaded/capture verification.

Crashes, crowd slowdown and first-person clipping remain open. No review finding established their cause; a native-module fault address alone does not determine causation.
