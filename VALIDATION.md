# Validation

**v0.32 uses the accepted Test166 Workshop build without rebuilding its native
payload.** The owner reported that it worked in game. Steam's successful update
of item 3825625050 and the exact downloaded pack/preview were independently
verified. The release's [validation record](docs/public-release/V0_32_VALIDATION.json)
separates packaging checks from earlier build and gameplay evidence.

The retained Test166 component suite passed 98 adapter, 50 QTE and 8 HUD
executables plus resource format/renderer checks. Workshop fixtures passed
actual float-Lua/loadlib and all 37 stock callbacks, nine process startups
without cache growth, shared resources and verified copy fallback, foreign-file
preservation, staging recovery, failed-extraction cleanup, 100 normal receipt
lifetimes and five owned forced exits. [Exact results](docs/public-release/TEST166_VALIDATION.json)
and the [storage design](docs/public-release/WORKSHOP166.md) describe the boundaries.

Test165 separately passed a fresh subscribed runtime gate: direct Steam content
path, exact loaded components, status channels and 17 core hook destinations,
with no old DInput mod loader or local publisher pack. Its owner playtest was
accepted. That gate does not substitute for a Test166 gate.

Test166's owner gameplay report is accepted, but its formal loaded-hash/hook
gate was not completed. The playtest timing relative to local publisher-copy
cleanup was unspecified. Do not claim independent Subscribe-only Test166
runtime activation, explicit full-battle completion, exhaustive feature
coverage, second-machine validation or long-term stability from that report.
There is no need to repeat play merely to acknowledge the owner's acceptance.

The v0.32 release rechecks source correspondence, archive contents/checksums,
Python tests and generated metadata. It does not launch a game, attach a
debugger or enable recording. Future live investigation requires fresh
readiness and exact loaded identity/hook checks. Recording remains optional
and is not shipped or started by the Workshop mod.

Older Test159–163 installer/chooser receipts remain historical. Unavailable
optional archives are skipped, never counted as passing checks. Synthetic
fixtures do not establish driver/world/castle behavior, race freedom, absence
of leaks, power-loss recovery or physical display latency. Crashes, crowd
slowdown, intermittent charging/climbing and first-person clipping remain open.
