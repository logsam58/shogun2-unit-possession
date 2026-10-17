# Validation and evidence

## What the suites check

| Suite | Coverage |
| --- | --- |
| Adapter: 53 executables (development) | Native forwarding, strict identity/lifetime, movement, camera return, weapons/accuracy, equipment, siege path/speed/crest/landing and cleanup |
| Styled QTE: 32 executables | Combat preparation, clocks/cycles, input/deadlines, interruption feedback, registry churn, pacing and actual reusable styled surface |
| HUD: 2 executables | Consistent contextual state, visibility and presentation policy |
| Loader: 2 executables | Bootstrap, embedded payload integrity and integration |
| Installer fixture | Supported predecessor upgrades, seven interruption stages, rollback, reinstall, collisions, cache retention and isolated HKCU handling |
| Dump reader: 7 Python cases | Embedded, MemoryList and Memory64 stack layouts, exception-context preference and explicit missing-stack handling |

Some fixtures execute bounded original native functions from the supported retail engine with synthetic records and substituted external providers. They are stronger than a pure policy mock, but do not reproduce the full game's collision, AI, asset and casualty lifecycles. Exact fixture limits belong alongside the tested function. No count here proves every unit or siege map is supported.

## Recorded results

[docs/import-validation.json](docs/import-validation.json) records a complete source-checkout build: 50 adapter and 32 styled QTE executables, HUD policy, two loader fixtures, full installer regression and seven dump-reader format cases passed. It includes initial dependency/CWD failures and subsequent successful stages. [docs/source-import.json](docs/source-import.json) pins the 310 original imported source files for provenance; intentional later changes need not match that historical manifest.

[The v0.15 receipt](docs/releases/v0.15-validation.json) identifies exact retained adapter/QTE/HUD component hashes and freshly run release loader/installer checks. Release packaging verifies PE version/resources, payload hashes, marker metadata and source commit. Retaining an already verified gameplay binary is not reported as a new gameplay test run.

## Offline and live evidence

Builds/tests write ignored artifacts/ and do not install or launch the game. Installer fixtures use temporary game/user directories and an isolated registry fixture. The maintenance CLI's `--check-only` mode validates a target and embedded payload without writing the installation.

A live check requires a closed-game update, fresh launch, exact loaded backing paths/hashes and appropriate capture. Test movement, weapon shots and peer volleys, QTE timing/interruption, camera return, death, mounted behavior and siege entry/crest/landing for the affected scenario. Record map, unit, ammunition, battle speed and actual results. A stable short run does not establish long-run stability.

An extended siege session completed climbing/crest/landing but later crashed in native effects processing. The failure remains unresolved, including possible mod involvement. [Known issues](docs/KNOWN_ISSUES.md) records what the dump establishes and what it does not.

Before a source push, run `py tools/audit_repository.py`. It checks staged blob contents for excluded binaries/assets/evidence, oversized files and recognized credential patterns. Binary release assets are distributed separately with checksums; private real dumps are never part of that package.


[Test100 development validation](docs/development/Test100-validation.json) records 52 adapter executables, a final native input/modifier follow-up, both HUD and loader checks, and final-payload installer regression. Four corrected HUD images were rendered and two inspected; formation rendering remains in the game. The unchanged published QTE binary and its earlier 32-case receipt are retained. No live acceptance or crash fix is claimed.

[Test101 correction validation](docs/development/Test101-validation.json) records the final 53 adapter executables, exact native projection/camera-cache and stationary-follow/Q regressions, loader integrity and Test100 transactional upgrade/rollback. HUD/QTE sources and binaries are retained rather than claimed as newly rendered or retested. Test100 was live rejected for Q/Follow Me and rifle/arm clipping; Test101 still needs live acceptance.
