# Contributor instructions

This is a native C++ first-person possession mod for Total War: SHOGUN 2. Read README.md, BUILD_AND_TEST.md, SOURCE_MAP.md and VALIDATION.md before changing it. Use the preserved native sources and existing indexed decoding.

## Current baseline

v0.25 retains the exact Test140 gameplay, accepted by the user with charging explicitly marked buggy. This is user-reported acceptance; no formal Test140 loaded-build gate was run. Exact accepted Test138/v0.24 remains recoverable. Preserve Test136 Rapid Advance, the native1 melee toggle, Q targeting, reversible squad detachment, independent formation firing, camera, HUD and QTE.

Ordinary native locomotion states0/4 pass unchanged. The accepted melee readiness handling and scoped advance-task detached-actor compatibility remain. This acceptance does not certify all animation assets, measured FPS or universal crash safety. First-person weapon/arm quality is deferred. Ultra crowd slowdowns and other crashes remain open.

Checkpoint2 is native infantry/cavalry charging. Test140 is an unpublished charge-contact correction after Test139 locomotion worked but contact became a QTE; its exact check/install status is in the local artifacts/Test140 receipts. The user accepted it with a charging caveat. Read docs/development/Checkpoint2.md and docs/development/Test140.md before extending it. T/G abilities and variable bow draw follow later. Use native ability definitions, restrictions and original game icons/button artwork; no placeholders.

## Changes and validation

Preserve native unit AI, formations, ranged fire, melee outcomes, movement and siege behavior. Ownership filters must never grant authority without current full actor/unit/lease proof. Keep individual-read fallbacks. Do not reintroduce discarded Test116/117 experiments; their source remains in Git history.

Run the relevant offline fixtures. Keep work light while the user plays. Close the game before replacing native DLLs and preserve an exact rollback. Live tests require a fresh user READY/IN BATTLE cue, loaded-file hashes and actual authorized capture verification before formal play. Do not launch the game, start extra profiling or change graphics/CPU/affinity settings without authorization.

Ordinary recording starts independently of READY; preserve the existing watcher and its configuration. Honor explicit recorder-off instructions. Recording health and loaded identity do not establish gameplay acceptance.

## Releases

Retain accepted gameplay binaries during promotion; rebuild only release loader/installer metadata. Pin the complete native/tool source closure, verify embedded resources and run full installer upgrade/rollback checks, including the exact immediate predecessor, all interruption stages and repeat backups. Verify VERSION, marker and PE versions together.

Publication requires an explicit user request. Keep raw dumps/input logs, game assets, absolute local paths and credentials out of Git. Local requirements are in docs/LOCAL_DEPENDENCIES.md. Historical candidate notes describe their own scope; use the latest accepted baseline above.
