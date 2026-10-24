# Release engineering references

For **v0.32 / Test166**, use [release identity](../current-release.json), the
[build guide](../../BUILD_AND_TEST.md), and [validation summary](../../VALIDATION.md).
This directory also retains earlier engineering notes and receipts at their
original paths so that recorded evidence remains traceable.

## Maintained references

| Reference | Purpose |
| --- | --- |
| [Releasing](RELEASING.md) | Current Workshop/source archive procedure |
| [Native contracts](NATIVE_CONTRACTS.md) | ABI, ownership, forwarding, threading and shutdown boundaries |
| [Derived data](DERIVED_DATA.md) | Canonical inputs and generated outputs |
| [Resources](RESOURCES.md) | Independent local resource-reference pipeline |
| [Provenance](PROVENANCE.md) | First-party, vendor and game-material scope |
| [Historical tests](HISTORICAL_TESTS.md) | Optional archive-dependent compatibility suite |
| [GitHub presentation](GITHUB_PRESENTATION.md) | Repository identity and current navigation |

## Current implementation and evidence

- [Workshop165](WORKSHOP165.md) explains the bridge introduced before Test166.
  Its Test165 hashes, counts and live gate apply to that earlier build only.
- [Workshop166](WORKSHOP166.md) describes the current storage changes.
- [Test166 receipt](TEST166_VALIDATION.json) records component/pack fixtures.
- [v0.32 receipt](V0_32_VALIDATION.json) records release packaging and separates
  retained native evidence from the owner's gameplay acceptance.

## Historical material

`PLAN.md`, `PUBLICATION_HANDOFF.md`, `STANDARD_SETUP.md`, and `STARTUP161`–`163`
describe the earlier source-kit and installer/chooser work. `WORKSHOP164.md`
records the inert entry investigation; it is not a current live-test checklist.
Their original bodies are retained below explicit historical notices.

`TEST162`–`TEST165`, `V0_29`–`V0_31`, `VALIDATION_RESULTS.json`,
`STANDARD_SETUP_VALIDATION.json`, `LOCAL_ASSETS159_VALIDATION.json`, and
`LEGACY_DOWNLOAD_CLEANUP.json` retain evidence for their named builds or tasks.
Do not combine their results into a claim that Test166 passed those exact
checks. `native-inventory.json` is a generated source index, not an execution
receipt or proof of native semantics.

Historical plans, owner approvals and "current" statements apply to their
recorded session. Current work needs its own scope and validation; no old note
authorizes installation, recording, publication or a visibility change.
