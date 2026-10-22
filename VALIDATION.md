# Validation

[Build commands](BUILD_AND_TEST.md) and [source-kit engineering summary](docs/public-release/VALIDATION_RESULTS.json) and [local-resource installer validation](docs/public-release/LOCAL_ASSETS159_VALIDATION.json) are current. Raw logs, toy dumps and game resources stay out of the source kit.

HUD tests run production measure() for ten final-human roles and negative identity/liveness/ownership cases. Diagnostic tests run production snapshot logic with synthetic reads and prohibit process access in help smoke tests. Fresh staging includes accuracy_read90.py and actor_roles.py; existing installed recording was not invalidated by the historical deployment omission.

Current installer tests cover install, repeated repair, encoding, collisions, exception rollback and 15 actual child kills followed by recovery. Unknown target/registry edits, legacy stages and journal extras remain intact. A filename alone does not authorize deletion. Optional historical compatibility is explicitly skipped when unavailable, never counted as pass.

Fixtures have finite native/synthetic provider boundaries. Passing them does not prove all driver/world/castle behavior, race freedom, absence of leaks, or physical display latency.

No real game was launched, installed, attached or profiled for Test159. Installed Test158 and its limited reported acceptance stay separate. A new candidate needs separate install authorization and a fresh READY/IN BATTLE, exact loaded hashes/native hooks and actual recording check before formal play.

Crashes, crowd slowdown and first-person clipping remain open. No review finding established their cause; a native-module fault address alone does not determine causation.
